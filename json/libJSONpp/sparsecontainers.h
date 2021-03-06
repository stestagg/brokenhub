#ifndef JSON_sparsemapob_h_INCLUDED
#define JSON_sparsemapob_h_INCLUDED

#include "maps/sparse_hash_map"
#include <vector>
#include "maps/sfh.h"

namespace JSON{
	class value;
}

typedef google::sparse_hash_map<achar *,JSON::value *,sf_hash, astrCmp> raw_object_t ;
typedef std::vector<JSON::value*> raw_array_t;

#define JSON_OBJINIT(x) 
#define JSON_ARRAYINIT(x) 


#endif
