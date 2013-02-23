#include <stdint.h>

struct config_t{
	uint32_t drop;
	uint32_t corrupt_packets;
	uint32_t corrupt_bytes;
    uint32_t truncate_len;
};

bool filter(char *data, int &len);

extern config_t config;
