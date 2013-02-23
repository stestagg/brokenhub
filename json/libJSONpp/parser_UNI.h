#ifndef JSON_Parser_UNI_h_INCLUDED
#define JSON_Parser_UNI_h_INCLUDED

#include "config.h"

#include "value.h"
#include <vector>
#include <string>

#ifdef JSON_WINDOWS
	#include <windows.h>
#endif
#ifdef JSON_POSIX
	 #include <sys/mman.h>
	 #include <sys/types.h>
     #include <sys/stat.h>
	 #include <fcntl.h>
     #include <unistd.h>
#endif

namespace JSON{

	class test;
	class UNI_test;
	
	/*! \brief A wrapper-namespace around the mapping_techniques::mapping_techniques enum. */
	namespace mapping_techniques{
		enum mapping_techniques{
			unmapped,
			direct,
			mmap
		};
	}
	
	/*! \brief Tracks access either to a unicode string, or a file mapping to a unicode file. */
	class unibuffer{
	
	public:
		const wchar_t * bfr;
		const wchar_t * ptr;
		const wchar_t * end;
		size_t len;
		mapping_techniques::mapping_techniques mapping;
#ifdef JSON_WINDOWS
		struct {
			char *view;
			HANDLE map;
			HANDLE file;	
		} mmap_data;
#endif
#ifdef JSON_POSIX
		struct {
			int file;
		} mmap_data;
#endif

		public:  
			unibuffer():bfr(0),ptr(0),end(0),len(0),mapping(mapping_techniques::unmapped){};
			~unibuffer();
			bool mapfile(const wchar_t *fn);
			bool mapfile(const char *fn);

			bool mapstring(const wchar_t*fn);
			bool mapchararray(const wchar_t *buf,int len);

			bool unmap();
	};
	/*! \brief A builder class for parsing UNICODE files. 
	*
	* The in-memory character encoding for strings is determined at compile time.  All conversions are handled automatically
	*/
	class parser_UNI
	{
		friend class JSON::test;
		friend class JSON::UNI_test;

		//static inline bool isEOL();	
		static inline bool readhexcode(const wchar_t *str,std::wstring &dst);
		static inline bool readhexquad(const wchar_t *str,std::wstring &dst);
		static inline bool charisNumeric(wchar_t test);
		bool charisWS(wchar_t test);
		bool charisEOL(wchar_t test);
		inline void parseEscapes(const wchar_t *strstart,const wchar_t *strend, astr &rv);

		static inline int hextoint(wchar_t  hexchar);

		std::vector<int> errs;
		void adderror(int num);
		
		bool readstring(astr &rv);
		bool readunquotedstring(astr &rv);
		bool readlit(JSON::value &rv);
		bool readnum(JSON::value &rv);
		bool readarray(JSON::value &rv);
		bool readobject(JSON::value &rv);
		void skipWS();
		JSON::value &getvalue(JSON::value &rv);
		
		unibuffer bfr;

	public:

		bool fail();
		int errorcount();
		const std::vector<int> geterrors();
		bool haveerror(int );
		const achar* geterrorstring(int);
		void reseterrors();

		parser_UNI();
		parser_UNI(const wchar_t *s);
		~parser_UNI(void);
		
		void unmap();	
	
		JSON::value &parse(JSON::value &rv);
		JSON::value &parse(JSON::value &rv,std::wstring s);
		JSON::value &parse(JSON::value &rv,const wchar_t *);
		JSON::value &parsefile(JSON::value &rv,const wchar_t *);
		JSON::value &parsefile(JSON::value &rv,const char *s);
	};
}

#endif

