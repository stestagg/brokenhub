#include <stdint.h>

struct config_t{
	unsigned long drop;
	unsigned long corrupt_packets;
	unsigned long corrupt_bytes;
    unsigned long truncate_len;
	unsigned long bandwidth;
};

bool filter(char *data, int &len);

#ifndef CONFIG_HERE
extern config_t config;
#endif
