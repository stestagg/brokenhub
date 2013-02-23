#include "types.h"


#ifdef JSON_USE_WCHAR
	bool astrCmp::operator()(const achar* s1, const achar* s2) const{
		return (s1 == s2) || (s1 && s2 && wcscmp(s1, s2) == 0);	
	}

	bool astrLT::operator()(const achar* s1, const achar* s2) const{
		return wcscmp(s1, s2) < 0;
	}

#else

	bool astrCmp::operator()(const achar* s1, const achar* s2) const{
		return (s1 == s2) || (s1 && s2 && strcmp(s1, s2) == 0);	
	}

	bool astrLT::operator()(const achar* s1, const achar* s2) const{
		return strcmp(s1, s2) < 0;
	}

#endif
