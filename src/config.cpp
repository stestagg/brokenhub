#include "filter.h"
#include "parser_UTF8.h"

static const char *CONFIG_PATH = "config.json";

void load_config(){
	JSON::parser_UTF8 parser;
	JSON::value root;
	parser.parse(root, CONFIG_PATH);
	if (parser.fail()){
		fprintf(stderr, "Could not read config\n");
		for (int error : parser.geterrors()){
			fprintf(stderr, " %s\n", parser.geterrorstring(error));
		}
		abort();
	}
	fprintf(stderr, "Read Config\n");
}
