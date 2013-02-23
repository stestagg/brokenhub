#include "parser_UNI.h"
#include "convert.h"

#include "parser_errors.h"

using namespace JSON;

#pragma region unibuffer functions

/*!
	 * \brief
	 * unmaps and forgets about buffer
	 * 
	 * 
	 * \returns
	 * true, unless something unforseen has happened
	 * 
	 */
bool unibuffer::unmap(){
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
			UnmapViewOfFile(mmap_data.view);
			CloseHandle(mmap_data.map);
			CloseHandle(mmap_data.file);
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
unibuffer::~unibuffer(){
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
bool unibuffer::mapfile(const wchar_t *fn){
	mmap_data.file=CreateFileW(fn, GENERIC_READ,	0,	0,	OPEN_EXISTING,	FILE_ATTRIBUTE_NORMAL,	0);
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
	ptr = bfr = (wchar_t *)mmap_data.view;
	len = ifs.LowPart;
	end = bfr + len;
	
	if (*ptr == 0xFEFF){
		++ptr;
	}
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
bool unibuffer::mapfile(const char *fn){
	void *data=0;
	struct stat sbuf;

	if (stat(fn, &sbuf) == -1)          return false;

	mmap_data.file = open(fn, O_RDONLY);
	if (mmap_data.file == -1)	goto nd;
	
    data = mmap((caddr_t)0, sbuf.st_size, PROT_READ, MAP_SHARED, mmap_data.file, 0);
		if (data==0) goto nd;
	
		mapping=mapping_techniques::mmap;
		ptr = bfr = (const wchar_t*)data;
		len = sbuf.st_size;
		end = bfr + len;

		if (*ptr == 0xFEFF){
			++ptr;
		}

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
bool unibuffer::mapfile(const wchar_t *fn){
	size_t ln;
	char *cfn = convert::tochar(fn,wcslen(fn),ln);
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
bool unibuffer::mapstring(const wchar_t *buf){
	int len = wcslen(buf);
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
bool unibuffer::mapchararray(const wchar_t *buf,int len){
	ptr = bfr = buf;
	end = bfr + len;
	this->len = len;
	mapping = mapping_techniques::direct;
	return true;
}

#pragma endregion

	/*!
	 * \brief
	 * Is a wchar_t an ECMAScript whitespace?
	 *
	 * http://www.ecma-international.org/publications/files/ECMA-ST/Ecma-262.pdf (7.2)
	 * 
	 * \param test
	 * The character to test
	 */
bool parser_UNI::charisWS(wchar_t test){
	static const wchar_t WS[25]={0x0009,0x000A,0x000B,0x000C,0x000D,0x0020,0x00A0,0x1680,0x180E,0x2000,0x2001,0x2002,0x2003,0x2004,0x2005,0x2006,0x2007,0x2008,0x2009,0x200A,0x2028,0x2029,0x202F,0x205F,0x3000};
		for (int i=0;i<25;++i){
			if (test==WS[i]){ return true; }
		}
	return false;
}

	/*!
	 * \brief
	 * Is a wchar_t an ECMAScript end-of-line?
	 *
	 * http://www.ecma-international.org/publications/files/ECMA-ST/Ecma-262.pdf (7.3)
	 * 
	 * \param test
	 * The character to test
	 */
bool parser_UNI::charisEOL(wchar_t test){
	if (test==0x000A) return true;
	if (test==0x000D) return true;
	if (test==0x2028) return true;
	if (test==0x2029) return true;
	return false;
}

	/*!
	 * \brief
	 * Could the \p test char be part of a number?
	 *
	 * Note: This function checks to see if \p test is one of: 0123456789-+.eE
	 * 
	 * \param test
	 * The character to test
	 */
bool parser_UNI::charisNumeric(wchar_t test){
	if (test>=L'0' && test<=L'9') return true; 
	if (test==L'-' || test==L'+') return true;
	if (test==L'.') return true;
	if (test==L'e') return true;
	if (test==L'E') return true;
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
bool parser_UNI::readhexcode(const wchar_t*str,std::wstring &dst){
	char a,b;
	a=hextoint(*str);
	b=hextoint(*(str+1));
	if (a < 0 || b < 0){
		dst += L'?';
		return false;
	}
	dst += ((wchar_t)(a<<4) +b);
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
bool parser_UNI::readhexquad(const wchar_t*str,std::wstring &dst){
	char a,b,c,d;
	a=hextoint(*str);
	b=hextoint(*(str+1));
	c=hextoint(*(str+2));
	d=hextoint(*(str+3));
	if (a<0 || b<0 || c<0 || d<0){
		dst+='?';
		return false;
	}
	wchar_t tmp;
	tmp= (a<<12) + (b<<8) + (c<<4) + d;
	
	dst += tmp;
	return true;
}

	/*!
	 * \brief
	 * converts a hex character into a binary number.
	 * Takes a wchar_t in the range: 0-9, a-f, A-F and returns the numberic value that it represents.
	 * \returns -1 if the caracter is outside these ranges
	 * 
	 */
int parser_UNI::hextoint(wchar_t hexchar){
	if ((hexchar >= L'0' && hexchar <= L'9')){
		return hexchar-L'0';
	}
	if (hexchar >= L'a' && hexchar <= L'f'){
		return 10+(hexchar-L'a');
	}
	if (hexchar >= L'A' && hexchar <= L'F'){
		return 10+(hexchar-L'A');
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
void parser_UNI::parseEscapes(const wchar_t *strstart,const wchar_t *strend, astr &rv){
	std::wstring tmpstr;
	const wchar_t *escseg;
	while (escseg = (const wchar_t*)wmemchr(strstart,L'\\',strend-strstart)){
		tmpstr.append(strstart,escseg - strstart);
		strstart = escseg+1;
		switch((*strstart)){
			case L'\\':	tmpstr+=L'\\';++strstart;	break;
			case L'/':	tmpstr+=L'/';++strstart;	break;
			case L'"':	tmpstr+=L'"';++strstart;	break;
			case L'b':	tmpstr+=0x0008;++strstart;	break;
			case L't':	tmpstr+=0x0009;++strstart;	break;
			case L'n':	tmpstr+=0x000A;++strstart;	break;
			case L'v':	tmpstr+=0x000B;++strstart;	break;
			case L'f':	tmpstr+=0x000C;++strstart;	break;
			case L'r':	tmpstr+=0x000D;++strstart;	break;
			case L'x':
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
				tmpstr+=L'\\';
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
bool parser_UNI::readunquotedstring(astr &rv){
	rv=A("");

	const wchar_t *strstart = bfr.ptr;
	const wchar_t *strend = strstart;

	while (!charisWS(*bfr.ptr)){
		if (*bfr.ptr == L':') break;
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
bool parser_UNI::readstring(astr &rv){
	if (*bfr.ptr != L'"') return false;
	rv=A("");
	
	++bfr.ptr;
	const wchar_t *strstart=bfr.ptr;

	int z=0;
	const wchar_t *strend;
	while (z==0 || *(strend-1) == L'\\'){
		z=1;
		strend = (const wchar_t*)wmemchr(bfr.ptr,L'"',bfr.end - bfr.ptr);
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
bool parser_UNI::readlit(JSON::value &rv){
	const wchar_t *orig = bfr.ptr;
	bool kk = false;
	switch(*bfr.ptr){
		case L't':
			++bfr.ptr;
				if (bfr.end - bfr.ptr < 3){
					kk = false;
					break;
				}
				if (wmemcmp(bfr.ptr,L"rue",3) != 0){
					kk = false; 
					break;
				}
				bfr.ptr += 3;
				rv = JSON::literals::_true;
				kk=true;
				break;
		case L'f':
			++bfr.ptr;
				if (bfr.end - bfr.ptr < 4){
					kk = false;
					break;
				}
				if (wmemcmp(bfr.ptr,L"alse",4) != 0){
					kk = false; 
					break;
				}
				bfr.ptr += 4;
				rv = JSON::literals::_false;
				kk=true;
				break;
		case L'n':
			++bfr.ptr;
				if (bfr.end - bfr.ptr < 3){
					kk = false;
					break;
				}
				if (wmemcmp(bfr.ptr,L"ull",3) != 0){
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
bool parser_UNI::readnum(JSON::value &rv){
	bool flt=false;
	const wchar_t *strt =bfr.ptr;
	bool fail=false;
	while (charisNumeric(*(bfr.ptr))){
		if (*(bfr.ptr) == L'.' || *(bfr.ptr) == L'e' || *(bfr.ptr) == L'E'){flt=true;}
		++bfr.ptr;
		if (bfr.ptr == bfr.end) break;
	}
	int len = bfr.ptr - strt;
	wchar_t *tmpbuf = new wchar_t[len +1];
	wmemcpy(tmpbuf,strt,len);
	tmpbuf[len] = L'\0';
	if (flt){
		wchar_t *nd;
		double v = wcstod(tmpbuf,&nd);
		if (nd != tmpbuf+len) fail=true;
		rv.setnumber(v);
	}else{
		aint v;
		wchar_t *nd;
		v=wcstoaint(tmpbuf,&nd,10);
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
void parser_UNI::skipWS(){
	int cont=true;
	while (cont){
		cont=false;
		if (charisWS(*bfr.ptr)){++bfr.ptr; cont=true;}
		if (bfr.ptr >= bfr.end) return;  // >= here, just in case :)

		if (bfr.end - bfr.ptr < 2) continue;

		if (*(bfr.ptr) == '/' && *(bfr.ptr+1) == '/'){
			cont=true;
			bfr.ptr+=2;
			while(!charisEOL(*bfr.ptr)){
				++bfr.ptr;
				if (bfr.ptr >= bfr.end) return;  // >= here, just in case :)
			}
			++bfr.ptr;
			if (bfr.ptr >= bfr.end) return; 
		}
		if (*(bfr.ptr) == L'/' && *(bfr.ptr+1) == L'*'){
			cont=true;
			while (*(bfr.ptr+1) != L'/'){
				wchar_t *eptr = (wchar_t*)wmemchr(bfr.ptr+2,'*',bfr.end - bfr.ptr);
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
bool parser_UNI::readarray(JSON::value &rv){
	rv.setarray();
	if (*bfr.ptr != L'[') return false;
	++bfr.ptr;

	bool first=true;
	while (1){
		JSON::value curval;

		skipWS();
		if (bfr.ptr >= bfr.end){
			adderror(5);
			return false;
		}
		
		if (*bfr.ptr == L']'){
			++bfr.ptr;
			return true;
		}else if (*bfr.ptr != L','){
			if (first!=true){adderror(8);}
		}else{
			++bfr.ptr;
			skipWS();
			if (bfr.ptr >= bfr.end){
				adderror(5);
				return false;
			}
			if (*bfr.ptr == L']'){
				++bfr.ptr;
				return true;
			}
		}
		
		skipWS();
		if (bfr.ptr == bfr.end){
			adderror(5); return false;
		}
		
		
		getvalue(curval);
		if (curval.getdatatype() == datatype::_undefined && *(bfr.ptr)!=L','){
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
bool parser_UNI::readobject(JSON::value &rv){
	rv.setobject();
	if (*bfr.ptr != L'{') return false;
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
		
		if (*bfr.ptr == L'}'){
			++bfr.ptr;
			return true;
		}else if (*bfr.ptr != L','){
			if (first!=true){adderror(8);}
		}else{
			++bfr.ptr;
			skipWS();
			if (bfr.ptr >= bfr.end){
				adderror(5);
				return false;
			}
			if (*bfr.ptr == L'}'){
				++bfr.ptr;
				return true;
			}
		}
		if (*bfr.ptr == L':'){
			adderror(6);
			createrandomname(curname);
		}else if (*bfr.ptr == L'"'){
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
		if (*bfr.ptr != L':'){
			adderror(6); 
			if (*bfr.ptr == L'}'){  //this should allow recovery in some situations.
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
JSON::value &parser_UNI::getvalue(JSON::value &rv){
	rv.setdt(datatype::_undefined);
	skipWS();
	if (bfr.ptr >= bfr.end) return rv;
	
	if (*bfr.ptr == L'"'){
		astr tmp;
		readstring(tmp);
		rv.setstring(tmp);
		return rv;
	}
	if (*bfr.ptr == L'['){
		readarray(rv);
		return rv;
	}
	if (*bfr.ptr == L'{'){
		readobject(rv);
		return rv;
	}
	if (
		(*bfr.ptr == L't') ||
		(*bfr.ptr == L'f') ||
		(*bfr.ptr == L'n')
		){
			readlit(rv);
			return rv;
	}
	if (
		(*bfr.ptr == L'-') ||(*bfr.ptr == L'+') ||
		((*bfr.ptr >= L'0') && (*bfr.ptr <= L'9'))
		){
			readnum(rv);
			return rv;
	}
	return rv;
}

	/*!
	 * \brief Simple wrapper function for adding error information.  this may include line/col information at some date?
	 */
void parser_UNI::adderror(int num){
	errs.push_back(num);
}

	/*!
	 * \brief Property getter for the errorcount
	 */
int parser_UNI::errorcount(){
	return errs.size();
}

	/*!
	 * \brief Has error \p e been raised yet? for the list of errors, see: parser_errors.cpp
	 */
bool parser_UNI::haveerror(int e){
	std::vector<int>::iterator i;
	for (i=errs.begin();i!=errs.end();++i){
		if (e == *i) return true;
	}
	return false;
}

	/*!
	 * \brief Did parsing generate any errors?  Unfortunately, there is no way to gague the severity of the errors yet.
	 */
bool parser_UNI::fail(){
	return (errs.size()!=0);
}

	/*!
	 * \brief clear the error list.
	 */
void parser_UNI::reseterrors(){
	errs.clear();
}

const std::vector<int> parser_UNI::geterrors(){
	return errs;
}

	/*!
	 * \brief translates an error number into its description
	 */
const achar* parser_UNI::geterrorstring(int i){
	return JSON::errors::errors[i];
} 


#pragma region parse() functiosn

	/*!
	 * \brief parses a value from the input stream into \p rv.  the input buffer must already have been mapped.
	 *
	 * Remember, a valid JSON object only has one top-level value.
	 */
JSON::value &parser_UNI::parse(JSON::value &rv){
	return getvalue(rv);
}

	/*!
	 * \brief parses the JSON string \p s into the value \p rv.
	 *
	 * If the initial string mapping failed, rv is unchanged, but error 10 is added to the parser
	 */
JSON::value &parser_UNI::parse(JSON::value &rv,std::wstring s){
	if (!bfr.mapstring(s.c_str())){
		adderror(10);
		return rv;
	}
	return getvalue(rv);
}

	/*!
	 * \brief see parser_UNI::parse(JSON::value&, std::wstring)
	 */
JSON::value &parser_UNI::parse(JSON::value &rv,const wchar_t *s){
		if (!bfr.mapstring(s)){
		adderror(10);
		return rv;
	}
	return getvalue(rv);
}

	/*!
	 * \brief maps the file \p s into memory, then parses it into \p rv
	 */
JSON::value &parser_UNI::parsefile(JSON::value &rv,const wchar_t *s){
	if (!bfr.mapfile(s)){
		adderror(9);
		return rv;
	}
	return getvalue(rv);
}

	/*!
	 * \brief maps the file \p s into memory, then parses it into \p rv
	 */
JSON::value &parser_UNI::parsefile(JSON::value &rv,const char *s){
	size_t l2=0;
	wchar_t * s2 = convert::towchar_t(s,strlen(s),l2);
	bool mrv =bfr.mapfile(s2);
	delete [] s2;
	if (!mrv){
		adderror(9);
		return rv;
	}
	return getvalue(rv);
}

#pragma endregion

#pragma region Parser Constructors


/*!
	 * \brief maps the NULL TERMINATED string \p s ready for calling parse(JSON::value &)
	 */
parser_UNI::parser_UNI(const wchar_t *s){
	bfr.mapstring(s);
}

parser_UNI::parser_UNI(void)
{
}

parser_UNI::~parser_UNI(void)
{
}


#pragma endregion
