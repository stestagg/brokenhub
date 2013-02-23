#ifndef JSON_types_h_INCLUDED
#define JSON_types_h_INCLUDED

	#include "config.h"

#ifdef JSON_WINDOWS
#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#endif
#endif
	#include <string>
#include <climits>

//CHAR definition, WCHAR implies unicode, CHAR implies UTF8 or ANSII
#ifdef JSON_USE_WCHAR
	/*! If JSON_USE_CHAR is defined, then achar and astr use char type.
	 * If JSON_USE_WCHAR is defines, then achar and astr use wchar_t type
	 */
	typedef wchar_t achar ;
	typedef std::wstring astr;
	#define astrlen wcslen
	/*! A() is a macro similar to the windows T() macro. */
	#define A(x) L##x

#else
#ifdef JSON_USE_CHAR
	
	typedef char achar;
	typedef std::string astr;
	#define astrlen strlen
	#define A(x) x

#else
	#error Please define a valid char defintion macro in config.h, look here for options.
#endif
#endif

#include "string.h"

	/*!\brief this wrapper functor is used to provide correct ordering for std::map objects */
struct astrCmp {
    bool operator()( const achar* s1, const achar* s2 ) const;
};	

struct astrLT{  //compare strings for Dictionary Sorting
	bool operator()( const achar* s1, const achar* s2) const;	
};


//OBJECT definition
#ifdef JSON_USE_STDCONTS
	#include "stdcontainers.h"
#elif JSON_USE_DENSECONTS
	#include "densecontainers.h"
#elif JSON_USE_SPARSECONTS
	#include "sparsecontainers.h"
#else
#error Currently only JSON_USE_STDCONTS Implemented, it must be defined
#endif


//NUMBER definitions

#ifndef JSON_NO_int64
// Make sure that int64_t is defined
	#ifdef JSON_WINDOWS
		#define pfINT64 "I64i"
		#define int64_max LLONG_MAX

		typedef __int64 int64_t;

		#define strtoi64 _strtoi64
		#define wcstoi64 _wcstoi64
	#endif
	#ifdef JSON_POSIX
		#define pfINT64 "lli" 

		#define strtoi64 strtoll
		#define wcstoi64 wcstoll

		#include <stdint.h>
		#ifndef INT64_MAX
			# define INT64_MAX (9223372036854775807 ## LL)
		#endif
		#define int64_max INT64_MAX
	#endif
#endif


#ifdef JSON_NUMBER_INT
	#define pfINT "i"
	#define aint_max INT_MAX 
	#define strtoaint strtol
	#define wcstoaint wcstol
	typedef int aint ;
#else
	#ifdef JSON_NUMBER_LONG
		#define pfINT "li"
		#define aint_max LONG_MAX
		#define strtoaint strtol
		#define wcstoaint wcstol
		typedef long aint ;
	#else
		#ifdef JSON_NUMBER_i64
			#ifdef JSON_NO_int64
				#error Cannot define NUMBER to int64_t whe NO_int64 is defined.  CHECK YOUR CONFIG.H
			#endif
			#define pfINT pfINT64
			#define aint_max int64_max
			#define strtoaint strtoi64
			#define wcstoaint wcstoi64
			
			typedef int64_t aint;
		#else
			#error Please define a valid macro to specify the storage-type of integer NUMBERs
		#endif
	#endif
#endif

//NUMBER definitions
#ifdef JSON_NUMBER_FLOAT
	#define pfFLOAT "f"
	typedef float afloat;
#else
	#ifdef JSON_NUMBER_DOUBLE
		#define pfFLOAT "f"
		typedef double afloat;
	#else
		#error Please define a valid macro to specify the storage-type of NUMBERs
	#endif
#endif


#endif
