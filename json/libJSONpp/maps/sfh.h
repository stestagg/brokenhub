#ifndef JSON_sfh_h_INCLUDED
#define JSON_sfh_h_INCLUDED

unsigned long SuperFastHash (const char * data, int len) ;

#include "../types.h"

struct sf_hash{
	const unsigned long operator()(const achar *data) const{
		return SuperFastHash((char*)data,astrlen(data)*sizeof(achar));
	}

};



#endif