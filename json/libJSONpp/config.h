
#ifndef JSON_WINDOWS
#ifndef JSON_POSIX

//--These can be defined by the buildtool--
//#define JSON_WINDOWS 1
//#define JSON_POSIX 1
//\---------------------/

#endif
#endif

//#define JSON_USE_WCHAR 1
#define JSON_USE_CHAR 1

#define JSON_NO_int64
//#define JSON_NUMBER_INT 1
#define JSON_NUMBER_LONG 1
//#define JSON_NUMBER_i64 1


//#define JSON_NUMBER_FLOAT 1
#define JSON_NUMBER_DOUBLE 1

//#define JSON_USE_STDCONTS 1
#define JSON_USE_DENSECONTS 1
//#define JSON_USE_SPARSECONTS 1


// Several functions can be used for optimisation but 
// do not play well with the automatic object deletion
// ALL children of a JSON object are always deleted when 
// a JSON object is deleted.  Undefine this to prevent 
// them from being compiled
#define JSON_UNSAFE_FUNS 1

