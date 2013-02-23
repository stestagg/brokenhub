#ifndef JSON_Parser_UTF8_h_INCLUDED
#define JSON_Parser_UTF8_h_INCLUDED

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
	class UTF_test;

	namespace mapping_techniques{
		enum mapping_techniques{
			unmapped,
			direct,
			mmap
		};
	}
	
	/*! \brief Tracks access either to a UTF-8 string, or a file mapping to a UTF-8 file. */
	class utf8buffer{
	
	public:
		const char * bfr;
		const char * ptr;
		const char * end;
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
			utf8buffer():bfr(0),ptr(0),end(0),len(0),mapping(mapping_techniques::unmapped){};
			~utf8buffer();
			bool mapfile(const wchar_t *fn);
			bool mapfile(const char *fn);

			bool mapstring(const char *fn);
			bool mapchararray(const char *buf,int len);


			bool unmap();
	};

	/*! \brief A builder class for parsing UTF-8 files. 
	*
	* The in-memory character encoding for strings is determined at compile time.  All conversions are handled automatically
	*/
	class parser_UTF8
	{
		friend class JSON::test;
		friend class JSON::UTF_test;
		static inline bool isEOL();	
		static inline bool readhexcode(const char*str,std::string &dst);
		static inline bool readhexquad(const char*str,std::string &dst);
		static inline bool charisNumeric(char test);
		static inline bool charskipWS(const char ** test,int len);
		static inline bool charskipEOL(const char **test,size_t len);
		inline void parseEscapes(const char *strstart,const char *strend, astr &rv);

		static inline int hextoint(char hexchar);

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
		
		utf8buffer bfr;

	public:

		bool fail();
		int errorcount();
		const std::vector<int> geterrors();
		const achar* geterrorstring(int);
		void reseterrors();
		bool haveerror(int );


		parser_UTF8();
		parser_UTF8(const char *s);
		~parser_UTF8(void);
	
			void unmap();	
		
		JSON::value &parse(JSON::value &rv);
		JSON::value &parse(JSON::value &rv,std::string s);
		JSON::value &parse(JSON::value &rv,const char *);
		JSON::value &parsefile(JSON::value &rv,const char *);
	};
}

#endif
