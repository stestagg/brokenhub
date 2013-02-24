#include "filter.h"
#include "parser_UTF8.h"

static const char *CONFIG_PATH = "/etc/brokenhub.conf";

#define UIMAX(size) (size)(((1ull << ((sizeof(size) * 8)-1)) - 1) | ((0xffull << ((sizeof(size) * 8) - 1))))

JSON::value read_or_abort(JSON::value parent, const char* key){
	if (!parent.childexists(key)){
		fprintf(stderr, "Error: Config item '%s' missing\n", key);
		abort();
	}
	return parent.getchild(key);
}

unsigned long percent_to_long(float perc){
	return (unsigned long)((perc / 100.0) * UIMAX(unsigned long));
}

void load_config(){
	JSON::parser_UTF8 parser;
	JSON::value root;
	parser.parsefile(root, CONFIG_PATH);
	if (parser.fail()){
		fprintf(stderr, "Could not read config\n");
		for (int error : parser.geterrors()){
			fprintf(stderr, "- %s\n", parser.geterrorstring(error));
		}
		abort();
	}
	float drop_percent = read_or_abort(root, "drop_percent").getfloat();
	config.drop = percent_to_long(drop_percent);
	float corrupt_percent = read_or_abort(root, "corrupt_packet_percent").getfloat();
	config.corrupt_packets = percent_to_long(corrupt_percent);
	int corrupt_bytes = read_or_abort(root, "corrupt_packet_bytes").getinteger();
	config.corrupt_bytes = corrupt_bytes;
	config.truncate_len = read_or_abort(root, "truncate_len").getinteger();
	
	JSON::value bandwidth_value = read_or_abort(root, "bandwidth");
	if (bandwidth_value.getinteger() == 0){
		config.bandwidth = 0;
	} else {
		float bandwidth_kps = bandwidth_value.getfloat();
		unsigned long bandwidth_nspb = (1.0/(bandwidth_kps * 1024)) * 1000000000;
		config.bandwidth = bandwidth_nspb;
	}
}
