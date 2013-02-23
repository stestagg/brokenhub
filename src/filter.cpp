#include <stdint.h>
#include <unistd.h>
#include "filter.h"

static unsigned long x=123456789, 
					 y=362436069, 
					 z=521288629;

unsigned long rand(void) {          //period 2^96-1
    unsigned long t;
    x ^= x << 16; x ^= x >> 5; x ^= x << 1;
    t = x; x = y; y = z;
    z = t ^ x ^ y;
    return z;
}


bool rand_test(unsigned long cutoff){
	return rand() < cutoff;
}


static config_t config;

#define MIN(x, y) (x > y) ? y : x


bool corrupt_packet(char *data, int &len){
	unsigned long n_bytes = rand() % config.corrupt_bytes;
	for (int i=0; i<n_bytes; ++i){
		size_t index = rand() % len;
		data[index] = rand() % 256;
	}	
}


bool drop_packet(char *data, int &len){
	if (rand_test(config.drop)){
		return false;
	}
}


bool filter(char *data, int &len){
	if (config.drop){
		if (!drop_packet(data, len)) return false;
	}
	if (config.corrupt_packets 
	    && config.corrupt_bytes 
		&& rand_test(config.corrupt_packets)){
		corrupt_packet(data, len);
	}
	if (config.truncate_len){
		len = MIN(len, config.truncate_len);
	}
	return true;
}

