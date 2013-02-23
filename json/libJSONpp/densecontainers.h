#ifndef JSON_densemapob_h_INCLUDED
#define JSON_densemapob_h_INCLUDED

#include "maps/sparsehash/dense_hash_map"
#include <vector>
#include "maps/sfh.h"

namespace JSON{
	class value;
}

typedef google::dense_hash_map<achar *,JSON::value *,sf_hash, astrCmp> raw_object_t ;
typedef std::vector<JSON::value*> raw_array_t;

#define JSON_OBJINIT(x) (x).set_empty_key(NULL);
#define JSON_ARRAYINIT(x) 


#endif
