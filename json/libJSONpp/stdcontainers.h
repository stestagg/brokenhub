#ifndef JSON_stdmapob_h_INCLUDED
#define JSON_stdmapob_h_INCLUDED

#include <map>
#include <vector>

namespace JSON{
	class value;
}

typedef std::map<achar *,JSON::value *,astrLT> raw_object_t ;
typedef std::vector<JSON::value*> raw_array_t;

#define JSON_OBJINIT(x) 
#define JSON_ARRAYINIT(x) 

#endif
