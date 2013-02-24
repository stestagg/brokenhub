#include <sys/epoll.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <linux/if_packet.h>
#include <sys/socket.h>
#include <linux/if_ether.h>

#include <deque>
#include <string>

#include "filter.h"
#include "config.h"


typedef std::deque<std::string> queue_t;
static bool reload_config = true;


void usage(){
	fprintf(stderr, "Usage: interface_a interface_b\n");
	fprintf(stderr, "This command must be run as root\n");
	_exit(1);
}


typedef int socket_t;
typedef struct mac_t { char address[6]; } mac_t;


void setup_iface(socket_t sock, int iface){
	// Bind socket to interface
	struct sockaddr_ll sll;
	sll.sll_family = AF_PACKET;
	sll.sll_ifindex = iface;
	sll.sll_protocol = htons(ETH_P_ALL);
	bind(sock, (struct sockaddr *)&sll, sizeof(sll)); 
	// Set promiscuous mode
	struct packet_mreq mr;
	memset(&mr, 0, sizeof (mr));
	mr.mr_ifindex = iface;
	mr.mr_type = PACKET_MR_PROMISC;
	setsockopt(sock, SOL_PACKET, PACKET_ADD_MEMBERSHIP, &mr, sizeof(mr));
}


int get_raw_iface(const char *iface){
	struct ifreq ifr;
	if (strlen(iface) > (IFNAMSIZ - 1)) usage();
	socket_t sock = socket(PF_PACKET, SOCK_RAW, ETH_P_ALL);
	// Look up the interface id for eth1 
	strncpy((char *) ifr.ifr_name, iface, IFNAMSIZ);
	ioctl(sock, SIOCGIFINDEX, &ifr);
	setup_iface(sock, ifr.ifr_ifindex);
	return sock;
};


void epolladd(int poll, socket_t sock, uint32_t data=0, bool add=true){
	epoll_event ev;
	ev.events = EPOLLIN | data;
	ev.data.fd = sock;
	int rv = epoll_ctl(poll, (add) ? EPOLL_CTL_ADD : EPOLL_CTL_MOD, sock, &ev);
    if (rv < 0){
		fprintf(stderr, "Epoll add failed: %s\n", strerror(errno));
        abort();
	}	
}


void add_reader(int poll, socket_t sock){
	epolladd(poll, sock);
}


void listen_write(int poll, socket_t sock, bool expect_write){
	if (expect_write) epolladd(poll, sock, EPOLLOUT, false);
	else epolladd(poll, sock, 0, false);
}


mac_t get_mac(socket_t sock, const char* iface){
	ifreq ifr;
	strncpy((char *) ifr.ifr_name, iface, IFNAMSIZ);
	
	if (ioctl(sock, SIOCGIFHWADDR, &ifr) != 0){
		printf("Error getting H/W address of iface: %s\n", strerror(errno));
		abort();
	}
	mac_t result;
	memcpy(result.address, ifr.ifr_hwaddr.sa_data, 6);
	return result;
}


void signal_reload_handler(int signum) {
	printf("Caught signal %d\n",signum);
	reload_config = true;
	return;
}


int cmp_times (const timespec &a, const timespec &b) {
	if (a.tv_sec == b.tv_sec){
		return (a.tv_nsec == b.tv_nsec) ? 0 : ((a.tv_nsec > b.tv_nsec) ? 1 : -1);
	}
	return (a.tv_sec == b.tv_sec) ? 0 : ((a.tv_sec > b.tv_sec) ? 1 : -1);
 }

static const size_t NS_PER_S = 1000000000;

int main(int argc, const char ** argv){
	if (argc != 3) usage();
	if (geteuid()) usage();

	socket_t write_sock = socket(PF_PACKET, SOCK_RAW, ETH_P_ALL);

	queue_t a_queue;
	queue_t b_queue;

	timespec a_queue_time = {0, 0};
	timespec b_queue_time = {0, 0};

	socket_t a_sock = get_raw_iface(argv[1]);
	mac_t a_mac = get_mac(a_sock, argv[1]);
	socket_t b_sock = get_raw_iface(argv[2]);
	mac_t b_mac = get_mac(b_sock, argv[2]);
	int poll = epoll_create(2);

	add_reader(poll, a_sock);
	add_reader(poll, b_sock);

	signal(SIGHUP, signal_reload_handler);

	char in_data[1600];
	epoll_event events[4];
	while (1){
		
		if (reload_config){
			load_config();	
			reload_config = false;
			a_queue_time = {0, 0};
			b_queue_time = {0, 0};
		}
		timespec this_tick;
		clock_gettime(CLOCK_MONOTONIC, &this_tick);
		bool write_to_a = (!a_queue.empty()) && (cmp_times(this_tick, a_queue_time) > 0);
		bool write_to_b = (!b_queue.empty()) && (cmp_times(this_tick, b_queue_time) > 0);
		
		listen_write(poll, a_sock, write_to_a);
		listen_write(poll, b_sock, write_to_b);

		int count = epoll_wait(poll, events, 4, -1);
		if (count == 0) fprintf(stderr, "Got no events?!\n");
		for (int i=0; i< count; ++i){
			epoll_event &event = events[i];
			socket_t sock = event.data.fd;
			if (event.events & EPOLLOUT){
				queue_t &queue = (sock == a_sock) ? a_queue : b_queue;
				std::string &data = *(queue.begin());
				int len = write(sock, data.c_str(), data.size());
				if (len != data.size()){
					fprintf(stderr, "Not all bytes written: %i,  %lu\n", 
						   len, data.size());
				}
				queue.pop_front();
				if (config.bandwidth){
					timespec &target_sleep = ((sock == a_sock) 
											  ? a_queue_time 
											  : b_queue_time);
					size_t ns_to_sleep = config.bandwidth * len;
					target_sleep = this_tick;
					target_sleep.tv_sec += ns_to_sleep / NS_PER_S;
					target_sleep.tv_nsec += ns_to_sleep % NS_PER_S;
				}
			}else{
				queue_t &queue = (sock == a_sock) ? b_queue : a_queue;
				mac_t &mac = (sock == a_sock) ? a_mac : b_mac;
				int len = read(sock, in_data, 1600);
				if (len < 0){
					printf("Read failed: %s from %lu\n", strerror(errno), sock);
					abort();
				}
				if (strncmp(in_data, mac.address, 6) 
					&& strncmp(&in_data[6], mac.address, 6)){
					if (filter(in_data, len)){
						queue.emplace_back(in_data, len);
					}
				}
			}
		} 
	}
	return 0;
}
