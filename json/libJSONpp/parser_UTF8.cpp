#include "parser_UTF8.h"
#include "convert.h"

#include "parser_errors.h"

using namespace JSON;

#pragma region utf8buffer functions

/*!
	 * \brief
	 * unmaps and forgets about buffer
	 * 
	 * 
	 * \returns
	 * true, unless something unforseen has happened
	 * 
	 */
bool utf8buffer::unmap(){
	switch(mapping){
	case mapping_techniques::unmapped:
	case mapping_techniques::direct:	
		bfr=0;
		ptr=0;
		end=0;
		len=0;
		mapping = mapping_techniques::unmapped;
		return true;
	case mapping_techniques::mmap:
		#ifdef JSON_WINDOWS
			if (mmap_data.view){
				UnmapViewOfFile(mmap_data.view);
				mmap_data.view = 0;
			}
			if (mmap_data.map){
				CloseHandle(mmap_data.map);
				mmap_data.map = 0;
			}
			if (mmap_data.file){
				CloseHandle(mmap_data.file);
				mmap_data.file = 0;
			}
			return true; 
		#endif
		#ifdef JSON_POSIX
			munmap((void*)bfr,len);
			close(mmap_data.file);
			return true;
		#endif
	}
	return false; // if we get here, then we can assume an error
}
/*!
	 * \brief
	 * The destructor will try to unmap the buffer if it can
	 */
utf8buffer::~utf8buffer(){
	unmap();
}

#ifdef JSON_WINDOWS
	/*!
	 * \brief
	 * Uses the native Windows file mapping API to map a file into memory
	 * 
	 * \param fn
	 * The file to map
	 */
bool utf8buffer::mapfile(const char *fn){
	mmap_data.file=CreateFileA(fn, GENERIC_READ,	0,	0,	OPEN_EXISTING,	FILE_ATTRIBUTE_NORMAL,	0);
	//Failed to Open file..
	if (mmap_data.file==INVALID_HANDLE_VALUE){ return false;}
	LARGE_INTEGER ifs;
	GetFileSizeEx(mmap_data.file,&ifs);
	// File too large (< 0xffffffff) - TBH, a file this size prob won't fit into ram anyway.
	// To fix this limitation, write better code for sequential mapping.
	if (ifs.HighPart<0){ CloseHandle(mmap_data.file);  return false;  }

	mmap_data.map=CreateFileMapping( mmap_data.file, NULL,PAGE_READONLY,  0,  0,NULL);
	if (!mmap_data.map) {CloseHandle(mmap_data.file); return false;}
	mmap_data.view=(char *)MapViewOfFile(mmap_data.map,FILE_MAP_READ,0,0,0);
	if (!mmap_data.view) { CloseHandle(mmap_data.map); CloseHandle(mmap_data.file); return false;}
	
	mapping=mapping_techniques::mmap;
	ptr = bfr = mmap_data.view;
	len = ifs.LowPart;
	end = bfr + len;

	return true;
}
#endif

#ifdef JSON_POSIX
	/*!
	 * \brief
	 * Uses the mmap API to map a file into memory
	 * 
	 * \param fn
	 * The file to map
	 */
bool utf8buffer::mapfile(const char *fn){
	void *data=0;
	struct stat sbuf;

	if (stat(fn, &sbuf) == -1)          return false;

	mmap_data.file = open(fn, O_RDONLY);
	if (mmap_data.file == -1)	goto nd;
	
    data = mmap((caddr_t)0, sbuf.st_size, PROT_READ, MAP_SHARED, mmap_data.file, 0);
		if (data==0) goto nd;
	
		mapping=mapping_techniques::mmap;
		ptr = bfr = (const char*)data;
		len = sbuf.st_size;
		end = bfr + len;
		return true;
	nd:
		if (data) munmap(data,sbuf.st_size);
		if (mmap_data.file>0) close(mmap_data.file);
		return false;
}
	/*!
	 * \brief
	 * Uses the mmap API to map a file into memory, the filename is converted into UTF-8 before use.
	 * 
	 * \param fn
	 * The file to map
	 */
bool utf8buffer::mapfile(const wchar_t *fn){
	size_t ln;
	char *cfn = convert::tochar(fn, wcslen(fn), ln);
	bool rv = mapfile(cfn);
	delete [] cfn;
	return rv;
}
#endif
	/*!
	 * \brief
	 * sets up the buffer to read from the string \p buf.  The buffer reads \p buf directly, so do not prematurely delete \p buf.
	 * 
	 * \param buf
	 * The unicode string to map.  \p buf is not copied, for performance reasons.  So if the string that \p buf points to is freed, then the unibuffer will be unreadable
	 */
bool utf8buffer::mapstring(const char *buf){
	size_t len = strlen(buf);
	return mapchararray(buf,len);
}
	/*!
	 * \brief
	 * maps a binary or un-zero-terminated string.  See mapstring for more info
	 * 
	 * \param buf
	 * The unicode string to read
	 * \param len
	 * The length of the unicode string, in characters.
	 */
bool utf8buffer::mapchararray(const char *buf,int len){
	ptr = bfr = buf;
	end = bfr + len;
	this->len = len;
	mapping = mapping_techniques::direct;
	return true;
}

#pragma endregion

//which idiot decided that char would be signed???
#define US(x) ((const unsigned char *)(x))

	/*!
	 * \brief This function will increment \p test IF it is a UTF-8 whitespace char.  
	 * \param test  A pointer to a char array that holds at least one UTF-8 character to test.
	 * \param len  The length of the buffer pointed to by \p test
     * \return true if whitespace was skipped.  false otherwise
	 */
bool parser_UTF8::charskipWS(const char ** test,int len){
	
	//static const char WS1[4] = { (char)0x09, (char)0x0B, (char)0x0C, (char)0x20 };
	if (len == 0) return false;
	if (**test > 0x08 && **test < 0x0E){
		++ *test;
		return true;
	}
	if (**test == 0x20){
		++ *test;
		return true;
	}

	if ((*US(*test)) == (unsigned char)0xC2){
		if (len < 2) return false;
		if (*US(*test+1) == (unsigned char)0xA0){
			*test += 2;
			return true;
		}
	}
	if (*US(*test) == (unsigned char)0xE1){
		if (len < 3) return false;
		if (
			(((*US(*test+1)) == (unsigned char)0x9A ) && ((*US(*test+2)) == (unsigned char)0x80))
		  ||(((*US(*test+1)) == (unsigned char)0xA0 ) && ((*US(*test+2)) == (unsigned char)0x8E))
		){
			*test += 3;
			return true;
		}
 		return false;
	}
	if (*US(*test) == (unsigned char)0xE2){
		if (len < 3) return false;
		
		if (
			(
				((*(*test+1)) == (char)0x80)
				&&(
					(((*US(*test+2)) >= (unsigned char)0x80) && ((*US(*test+2)) <= (unsigned char)0x8A))
				||	((*US(*test+2)) == (unsigned char)0xA8)
				||	((*US(*test+2)) == (unsigned char)0xA9)
				||	((*US(*test+2)) == (unsigned char)0xAF)
			
				)
			) || (
				((*US(*test+1)) == (unsigned char)0x81)
			&&  ((*US(*test+2)) == (unsigned char)0x9F)
			)
		){
		*test += 3;	
		return true;
		}
		return false;
	}
	if (*US(*test) == (unsigned char)0xE3){
		if (len < 3) return false;
		unsigned short tmp =  *((unsigned short*)(*test + 1));
		if (tmp == (unsigned short)0x8080){
			*test += 3;	
			return true;
		}
		return false;
	}
	return false;
}

	/*!
	 * \brief This function will increment \p test IF it is a UTF-8 eol char.  
	 * \param test  A pointer to a char array that holds at least one UTF-8 character to test.
	 * \param len  The length of the buffer pointed to by \p test
     * \return true if end of line was skipped.  false otherwise
	 */
bool parser_UTF8::charskipEOL(const char **test,size_t len){
	char a = **test;
	char b = 0;
	char c = 0;

	if ((a==0x0A) || (a==0x0D)){ 
		*test++;
		return true;
	}	

	if (len < 3)return false;
	
	b=*(*test + 1);
	c=*(*test + 2);
		
	if (a != (char)0xE2)	return false;
	if (b != (char)0x80)	return false;
	if (c == (char)0xA8 || c == (char)0xA9){
		*test += 3;
		return true;
	}
	return false;
}

	/*!
	 * \brief This function will test if \p test COULD be part of a standard number.
	 */
bool parser_UTF8::charisNumeric(char test){
	if (test>='0' && test<='9') return true; 
	if (test=='-' || test=='+') return true;
	if (test=='.') return true;
	if (test=='e') return true;
	if (test=='E') return true;
	return false;
}
	/*!
	 * \brief
	 * Tries to decode an escaped hex code of the form \\xFF
	 *
	 * If the decoding fails, eg: \\xFG, then the character '?' is appended instead
	 * 
	 * \param str
	 * pointer to the string that contains the \\xFF.
	 *
	 *\param dst
	 * reference to a wstring to which the decoded character will be appended.
	 */
bool parser_UTF8::readhexcode(const char*str,std::string &dst){
	char a,b;
	a=hextoint(*str);
	b=hextoint(*(str+1));
	if (a < 0 || b < 0){
		dst += '?';
		return false;
	}
	dst += ((char)(a<<4) +b);
	return true;
}

/*!
	 * \brief
	 * Tries to decode an escaped hex code of the form \\uFFFF
	 *
	 * If the decoding fails, eg: \\uFGHI, then the character '?' is appended instead
	 * 
	 * \param str
	 * pointer to the string that contains the \\uFFFF.
	 *
	 *\param dst
	 * reference to a wstring to which the decoded character will be appended.
	 */
bool parser_UTF8::readhexquad(const char*str,std::string &dst){
	char a,b,c,d;
	a=hextoint(*str);
	b=hextoint(*(str+1));
	c=hextoint(*(str+2));
	d=hextoint(*(str+3));
	if (a<0 || b<0 || c<0 || d<0){
		dst+='?';
		return false;
	}
	wchar_t tmp[2];
	tmp[0]= (a<<12) + (b<<8) + (c<<4) + d;
	tmp[1]=L'\0';

	size_t olen;
	char *tmpb=convert::tochar(tmp,1,olen);
	dst.append(tmpb,olen);
	delete [] tmpb;
	return true;
}

	/*!
	 * \brief
	 * converts a hex character into a binary number.
	 * Takes a wchar_t in the range: 0-9, a-f, A-F and returns the numberic value that it represents.
	 * \returns -1 if the caracter is outside these ranges
	 * 
	 */
int parser_UTF8::hextoint(char hexchar){
	if ((hexchar >= '0' && hexchar <= '9')){
		return hexchar-'0';
	}
	if (hexchar >= 'a' && hexchar <= 'f'){
		return 10+(hexchar-'a');
	}
	if (hexchar >= 'A' && hexchar <= 'F'){
		return 10+(hexchar-'A');
	}
	return -1;
}

	/*!
	 * \brief
	 * copy a string, decoding escaped chars.
	 * The string \p *strstart ... \p *strend, will be copied into the std::[w]string \p rv.
	 * Escaped chars are decoded as specified in:
	 * http://www.ecma-international.org/publications/files/ECMA-ST/Ecma-262.pdf (7.8.4)
	 *
	 * \returns -1 if the caracter is outside these ranges
	 * 
	 */
void parser_UTF8::parseEscapes(const char *strstart,const char *strend, astr &rv){
	std::string tmpstr;
	const char *escseg;
	while (escseg = (const char*)memchr(strstart,'\\',strend-strstart)){
		tmpstr.append(strstart,escseg - strstart);
		strstart = escseg+1;
		switch((*strstart)){
			case '\\':	tmpstr+='\\';++strstart;	break;
			case '/':	tmpstr+='/';++strstart;		break;
			case '"':	tmpstr+='"';++strstart;		break;
			case 'b':	tmpstr+=0x08;++strstart;	break;
			case 't':	tmpstr+=0x09;++strstart;	break;
			case 'n':	tmpstr+=0x0A;++strstart;	break;
			case 'v':	tmpstr+=0x0B;++strstart;	break;
			case 'f':	tmpstr+=0x0C;++strstart;	break;
			case 'r':	tmpstr+=0x0D;++strstart;	break;
			case 'x':
				++strstart;
				if (strend -strstart < 2){
					adderror(2);
					break;
				}
				if (!readhexcode(strstart,tmpstr)){
					adderror(2);
				}
				strstart+=2;
				break;
			case 'u':
				++strstart;
				if (strend -strstart  < 4){
					adderror(3);
					break;
				}
				if (!readhexquad(strstart,tmpstr)){
					adderror(3);
				}
				strstart+=4;
			break;
			default:
				tmpstr+='\\';
		}
	}
	tmpstr.append(strstart,strend);
	convert::toastr(rv,tmpstr);
}

	/*!
	 * Tries to guess what the user meant, because unquoted strings are strictly invalid syntax, this is a best-guess scenario.
	 *
	 * Embedded escape chars are decoded, see parser_UNI::parseEscapes
	 * 
	 * will read all input untill a Whitespace or ':' is encoutered.
	 */
bool parser_UTF8::readunquotedstring(astr &rv){
	rv=A("");

	const char *strstart = bfr.ptr;
	const char *strend = strstart;

	while (!charskipWS(&bfr.ptr,bfr.end - bfr.ptr)){
		if (*bfr.ptr == ':') break;
		++ bfr.ptr;
		strend= bfr.ptr;
		if (bfr.ptr == bfr.end){
			break;
		}
	}

	parseEscapes(strstart,strend,rv);
	return true;
}

	/*!
	 * \brief Reads a string from the input buffer and parses the esape chars.  rv is set to the result
	 * 
	 */
bool parser_UTF8::readstring(astr &rv){
	if (*bfr.ptr != '"') return false;
	rv=A("");
	
	++bfr.ptr;
	const char *strstart=bfr.ptr;

	int z=0;
	const char *strend;
	while (z==0 || *(strend-1) == '\\'){
		z=1;
		strend = (const char*)memchr(bfr.ptr,'"',bfr.end - bfr.ptr);
		if (strend == 0){
			strend = bfr.end;
			adderror(1);
			break;
		}
		bfr.ptr = strend+1;
		if (bfr.ptr == bfr.end) break;
	}
	
	parseEscapes(strstart,strend,rv);
	return true;
}

	/*!
	 * \brief Reads a JSON Literal from the input buffer and sets \p rv to it's value
	 * 
	 */
bool parser_UTF8::readlit(JSON::value &rv){
	const char *orig = bfr.ptr;
	bool kk = false;
	switch(*bfr.ptr){
		case 't':
			++bfr.ptr;
				if (bfr.end - bfr.ptr < 3){
					kk = false;
					break;
				}
				if (memcmp(bfr.ptr,"rue",3) != 0){
					kk = false; 
					break;
				}
				bfr.ptr += 3;
				rv = JSON::literals::_true;
				kk=true;
				break;
		case 'f':
			++bfr.ptr;
				if (bfr.end - bfr.ptr < 4){
					kk = false;
					break;
				}
				if (memcmp(bfr.ptr,"alse",4) != 0){
					kk = false; 
					break;
				}
				bfr.ptr += 4;
				rv = JSON::literals::_false;
				kk=true;
				break;
		case 'n':
			++bfr.ptr;
				if (bfr.end - bfr.ptr < 3){
					kk = false;
					break;
				}
				if (memcmp(bfr.ptr,"ull",3) != 0){
					kk = false; 
					break;
				}
				bfr.ptr += 3;
				rv = JSON::literals::_null;
				kk=true;
				break;
			
	}
	if (kk){ return true; }
	bfr.ptr = orig;
	return false;
}

	/*!
	 * \brief Reads an ECMAScript conformant number from the input buffer
	 */
bool parser_UTF8::readnum(JSON::value &rv){
	bool flt=false;
	const char *strt =bfr.ptr;
	bool fail=false;
	while (charisNumeric(*(bfr.ptr))){
		if (*(bfr.ptr) == '.' || *(bfr.ptr) == 'e' || *(bfr.ptr) == 'E'){flt=true;}
		++bfr.ptr;
		if (bfr.ptr == bfr.end) break;
	}
	int len = bfr.ptr - strt;
	char *tmpbuf = new char[len +1];
	memcpy(tmpbuf,strt,len);
	tmpbuf[len] = '\0';
	if (flt){
		char *nd;
		double v = strtod(tmpbuf,&nd);
		if (nd != tmpbuf+len) fail=true;
		rv.setnumber(v);
	}else{
		aint v;
		char *nd;
		v=strtoaint(tmpbuf,&nd,10);
		if (nd != tmpbuf+len) fail=true;
		rv.setnumber(v);
	}
	delete [] tmpbuf;
	return !fail;
}

	/*!
	 * \brief This function looks for, and skips over any whitespace or comments under the input pointer.
	 *
	 * After calling this, the input pointer will be on a non-whitespace char OR the end of stream.
	 */
void parser_UTF8::skipWS(){
	int cont=true;
	while (cont){
		cont=false;
		if (charskipWS(&bfr.ptr,bfr.end-bfr.ptr)){cont=true;}
		if (bfr.ptr >= bfr.end) return;  // >= here, just in case :)

		if (bfr.end - bfr.ptr < 2) continue;

		if (*(bfr.ptr) == '/' && *(bfr.ptr+1) == '/'){
			cont=true;
			bfr.ptr+=2;
			while(!charskipEOL(&bfr.ptr,bfr.end-bfr.ptr)){
				++bfr.ptr;
				if (bfr.ptr >= bfr.end) return;  // >= here, just in case :)
			}
		}
		if (*(bfr.ptr) == '/' && *(bfr.ptr+1) == '*'){
			cont=true;
			while (*(bfr.ptr+1) != '/'){
				char *eptr = (char*)memchr(bfr.ptr+2,'*',bfr.end - bfr.ptr);
				if (eptr == 0  || eptr >= (bfr.end -1)){adderror(4); return;}
				bfr.ptr=eptr;
			}
			bfr.ptr += 2;
		}
	}
}

/*!
	 * \brief Creates a pseudorandom object name.  Used for parsing malformed input
	 *
	 * In the case where an object has a value with no name, this function is called to give it one.
	 */
inline astr &createrandomname(astr& str){
	str=A("!anon_");
	for (int i=0;i<10;++i){
		if ( (rand() & 0x1) == 1){
			int num = rand() % 10;
			str+='0' + (char)num;
		}else{
			str += 'a' + (char)( rand() % 24 );
		}
	}
	return str;
}

	/*!
	 * \brief Reads an array from the input stream into \p rv
	 */
bool parser_UTF8::readarray(JSON::value &rv){
	rv.setarray();
	if (*bfr.ptr != '[') return false;
	++bfr.ptr;

	bool first=true;
	while (1){
		JSON::value curval;

		skipWS();
		if (bfr.ptr >= bfr.end){
			adderror(5);
			return false;
		}
		
		if (*bfr.ptr == ']'){
			++bfr.ptr;
			return true;
		}else if (*bfr.ptr != ','){
			if (first!=true){adderror(8);}
		}else{
			++bfr.ptr;
			skipWS();
			if (bfr.ptr >= bfr.end){
				adderror(5);
				return false;
			}
			if (*bfr.ptr == ']'){
				++bfr.ptr;
				return true;
			}
		}
		
		skipWS();
		if (bfr.ptr == bfr.end){
			adderror(5); return false;
		}
		
		
		getvalue(curval);
		if (curval.getdatatype() == datatype::_undefined && *(bfr.ptr)!=','){
			adderror(8);
			return false;
		}
		rv.addvalue(curval);
		first=false;
	}
	return true;
}

/*!
	 * \brief Reads an object from the input stream into \p rv
	 */
bool parser_UTF8::readobject(JSON::value &rv){
	rv.setobject();
	if (*bfr.ptr != '{') return false;
	++bfr.ptr;

	bool first=true;
	while (1){
		astr curname;
		JSON::value curval;

		skipWS();
		if (bfr.ptr >= bfr.end){
			adderror(5);
			return false;
		}
		
		if (*bfr.ptr == '}'){
			++bfr.ptr;
			return true;
		}else if (*bfr.ptr != ','){
			if (first!=true){adderror(8);}
		}else{
			++bfr.ptr;
			skipWS();
			if (bfr.ptr >= bfr.end){
				adderror(5);
				return false;
			}
			if (*bfr.ptr == '}'){
				++bfr.ptr;
				return true;
			}
		}
		if (*bfr.ptr == ':'){
			adderror(6);
			createrandomname(curname);
		}else if (*bfr.ptr == '"'){
			readstring(curname);	
			if (bfr.ptr == bfr.end){
				adderror(5); return false;
			}
		}else{
			readunquotedstring(curname);
			//adderror(11); // While this is _technically_ an error, we don't want people to fail because of it
			if (bfr.ptr == bfr.end){
				adderror(5); return false;
			}
		}
		if (curname == A("")){
			adderror(7);
			createrandomname(curname);
		}

		skipWS();
		if (bfr.ptr == bfr.end){
			adderror(5); return false;
		}
		if (*bfr.ptr != ':'){
			adderror(6); 
			if (*bfr.ptr == '}'){  //this should allow recovery in some situations.
				++bfr.ptr;
			}
			return false;
		}
		++bfr.ptr;
		
		skipWS();
		if (bfr.ptr == bfr.end){
			adderror(5); return false;
		}

		getvalue(curval);
		rv.addvalue(curname.c_str(),curval);
		first=false;
	}
	return true;
}


/*!
	 * \brief Works out the type of, and then reads a value from the input stream into \p rv
	 */
JSON::value &parser_UTF8::getvalue(JSON::value &rv){
	rv.setdt(datatype::_undefined);
	skipWS();
	if (bfr.ptr >= bfr.end) return rv;
	
	if (*bfr.ptr == '"'){
		astr tmp;
		readstring(tmp);
		rv.setstring(tmp);
		return rv;
	}
	if (*bfr.ptr == '['){
		readarray(rv);
		return rv;
	}
	if (*bfr.ptr == '{'){
		readobject(rv);
		return rv;
	}
	if (
		(*bfr.ptr == 't') ||
		(*bfr.ptr == 'f') ||
		(*bfr.ptr == 'n')
		){
			readlit(rv);
			return rv;
	}
	if (
		(*bfr.ptr == '-') ||(*bfr.ptr == '+') ||
		((*bfr.ptr >= '0') && (*bfr.ptr <= '9'))
		){
			readnum(rv);
			return rv;
	}
	return rv;
}

	/*!
	 * \brief Simple wrapper function for adding error information.  this may include line/col information at some date?
	 */

void parser_UTF8::adderror	(int num){
	errs.push_back(num);
}


	/*!
	 * \brief Property getter for the errorcount
	 */
int parser_UTF8::errorcount(){
	return errs.size();
}

	/*!
	 * \brief Has error \p e been raised yet? for the list of errors, see: parser_errors.cpp
	 */
bool parser_UTF8::fail(){
	return (errs.size()!=0);
}


	/*!
	 * \brief clear the error list.
	 */	
void parser_UTF8::reseterrors(){
	errs.clear();
}


const std::vector<int> parser_UTF8::geterrors(){
	return errs;
}

/*!
	 * \brief Did parsing generate any errors?  Unfortunately, there is no way to gague the severity of the errors yet.
	 */
bool parser_UTF8::haveerror(int e){
	std::vector<int>::iterator i;
	for (i=errs.begin();i!=errs.end();++i){
		if (e == *i) return true;
	}
	return false;
}



	/*!
	 * \brief translates an error number into its description
	 */
const achar* parser_UTF8::geterrorstring(int i){
	return JSON::errors::errors[i];
} 


#pragma region parse() functiosn
	/*!
	 * \brief parses a value from the input stream into \p rv.  the input buffer must already have been mapped.
	 *
	 * Remember, a valid JSON object only has one top-level value.
	 */
JSON::value &parser_UTF8::parse(JSON::value &rv){
	return getvalue(rv);
}

	/*!
	 * \brief parses the JSON string \p s into the value \p rv.
	 *
	 * If the initial string mapping failed, rv is unchanged, but error 10 is added to the parser
	 */
JSON::value &parser_UTF8::parse(JSON::value &rv,std::string s){
	if (!bfr.mapstring(s.c_str())){
		adderror(10);
		return rv;
	}
	return getvalue(rv);
}

	/*!
	 * \brief see parser_UNI::parse(JSON::value&, std::wstring)
	 */
JSON::value &parser_UTF8::parse(JSON::value &rv,const char *s){
	if (!bfr.mapstring(s)){
		adderror(10);
		return rv;
	}
	return getvalue(rv);
}


	/*!
	 * \brief maps the file \p s into memory, then parses it into \p rv
	 */
JSON::value &parser_UTF8::parsefile(JSON::value &rv,const char *s){
	if (!bfr.mapfile(s)){
		adderror(9);
		return rv;
	}
	getvalue(rv);
	bfr.unmap();
	return rv;
	
}

#pragma endregion

#pragma region Parser Constructors

	/*!
	 * \brief maps the NULL TERMINATED string \p s ready for calling parse(JSON::value &)
	 */
parser_UTF8::parser_UTF8(const char *s){
	if (!bfr.mapstring(s)) adderror(10);
}

parser_UTF8::parser_UTF8(void)
{
}

parser_UTF8::~parser_UTF8(void)
{
}


#pragma endregion
