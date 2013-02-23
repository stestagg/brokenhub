#include "convert.h"
#include <sstream>

#ifdef JSON_WINDOWS
	#define WIN32_LEAN_AND_MEAN
	#define VC_EXTRALEAN
	#include "windows.h"
#endif

#ifdef JSON_POSIX
	#include <iconv.h>
#endif

using namespace std;
namespace JSON{
	namespace convert{

#ifdef JSON_USE_WCHAR 
        astr &toastr(astr &dst,const char *src,size_t len){
			size_t olen;
			wchar_t *tmp = towchar_t(src,len,olen);
			dst.assign(tmp,olen);
			delete [] tmp;
			return dst;
         }

		astr &toastr(astr &dst,const wchar_t *src,size_t len){
			if (src==0) return dst;
			dst=src;
			return dst;
		}
		
		astr &toastr(astr &dst,const std::string &src){
			dst = towstr(dst,src);
			return dst; 
		}
		
		astr &toastr(astr &dst,const std::wstring &src){
			dst=src;
			return dst;
		}
		

		achar *toachar(const char *src,size_t len,size_t &olen){
			return towchar_t(src,len,olen);
		}
		
		achar *toachar(const wchar_t *src,size_t len,size_t &olen){
			olen=len;
			wchar_t *dst = new wchar_t[len+1];
			memcpy((void*)dst,(const void*)src,(sizeof(wchar_t)) * len);
			dst[len]=L'\0';
			return dst;
		}


#else
#ifdef JSON_USE_CHAR

astr &toastr(astr &dst,const char *src,size_t len){
	if (src==0) return dst;
	dst.assign(src,len);
	return dst;
}

astr &toastr(astr &dst,const wchar_t *src,size_t len){
	size_t olen;
	char *tmp = tochar(src,len,olen);
	dst.assign(tmp,olen);
	delete [] tmp;
	return dst;
}

astr &toastr(astr &dst,const std::string &src){
	dst=src;
	return dst;
}

astr &toastr(astr &dst,const std::wstring &src){
	return toastr(dst,src.c_str(),src.length());
}


achar *toachar(const char *src,size_t len,size_t &olen){
	olen=len;
	char *dst = new char[len+1];
	memcpy(dst,src,sizeof(char) * len);
	dst[len]='\0';
	return dst;
}


achar *toachar(const wchar_t *src,size_t len,size_t &olen){
	return tochar(src,len,olen);
}

#endif

#endif

char *tochar(const char*src,size_t len,size_t &olen){
	olen=len;
	char *dst = new char[len+1];
#ifdef JSON_WINDOWS
	memcpy_s(dst,len+1,src,sizeof(char) * len);
#else
	memcpy(dst,src,sizeof(char) * len);
#endif
	dst[len]='\0';
	return dst;
}


char *tochar(const wchar_t *src,size_t len,size_t &olen){
	#ifdef JSON_WINDOWS
		olen=WideCharToMultiByte(CP_UTF8,0,src,	len,0,0,NULL,NULL);
		if (olen == 0) return 0;
		char*outstr = new char[olen+1];
		WideCharToMultiByte(CP_UTF8,0,src,	len,outstr,olen+1,NULL,NULL);
		outstr[olen]='\0';
		return outstr;
	#endif
	#ifdef JSON_POSIX
		const wchar_t cranges[4]={0x00007F, 0x0007FF, 0x00FFFF, 0x10FFFF};
		const wchar_t masks[4]  ={0X3F,     0xFC0,    0x3F000,  0x1C0000};
		const char pfx[4]  ={0X0,     0xC0,    0xE0,  0xF0};
		const wchar_t *i=src;
		const wchar_t *ie = src+len;
		size_t bc=0;
		for (;i<ie;++i){
			if (*i < cranges[0]) bc+=1;
			else if (*i < cranges[1]) bc+=2;
			else if (*i < cranges[2]) bc+=3;
			else if (*i < cranges[3]) bc+=4;
			else return false;
		}
		olen = bc;
		i=src;
		char *outstr = new char[olen+1];

		char *op=outstr;
		for (;i<ie;++i){
			int sz=0;
			if (*i < cranges[0]) sz=1;
			else if (*i < cranges[1]) sz=2;
			else if (*i < cranges[2]) sz=3;
			else if (*i < cranges[3]) sz=4;
			
			if (sz == 1){
				*op = (unsigned char)*i;
				++op;
			}else{
				int strted=0;
				for (;sz>0;--sz){
					unsigned char cc;
					if (strted==0){
						strted=1;
						cc = pfx[sz-1];
					}else{
						cc = 0x80;
					}
					cc |= (unsigned char)((*i & masks[sz-1]) >> (6 * (sz-1)));
					*op = cc;
					++op;
				}
			}
		}

		outstr[olen]='\0';
		return outstr;
	#endif
}

/*char *tochar(const wchar_t *src){
	#ifdef JSON_WINDOWS
		size_t len;
		len=WideCharToMultiByte(CP_UTF8,0,src,	-1,0,0,NULL,NULL);
		if (len == 0) return 0;
		char*outstr = new char[len+1];
		WideCharToMultiByte(CP_UTF8,0,src,	-1,outstr,len+1,NULL,NULL);
		return outstr;
	#else
		size_t len = wcstombs(NULL,src,0);
		if (len == -1) return 0;
		char *outstr = new char[len+1];
		len = wcstombs(outstr,src,len+1);
		return outstr;
	#endif
}*/

wchar_t *towchar_t(const char *src,size_t slen,size_t &olen){
#ifdef JSON_WINDOWS
	olen=MultiByteToWideChar(CP_UTF8,0,src,slen,0,0);
	if (olen == 0) return 0;
	wchar_t *outstr = new wchar_t [olen+1];
	MultiByteToWideChar(CP_UTF8,0,src,	slen,outstr,olen+1);
	outstr[olen]=L'\0';
	return outstr;
#endif
#ifdef JSON_POSIX
const wchar_t 	cranges[4]={0x00007F, 0x0007FF, 0x00FFFF, 0x10FFFF};
		const wchar_t 	masks  [4]={    0X3F,    0xFC0,   0x3F000,0x1C0000};
		const unsigned char	pfx[4]={     0X0,     0xC0,      0xE0,    0xF0};
		
		const unsigned char *ep = (unsigned char *)src+slen;
		const unsigned char *ip = (unsigned char *)src;
		size_t cnt =0;
		while(ip < ep){
			if (*ip >= pfx[3]){
				ip+=4;
			}else if (*ip >= pfx[2]){
				ip+=3;
			}else if (*ip >= pfx[1]){
				ip+=2;
			}else{
				++ip;
			}
			++cnt;
		}
		olen = cnt;
		
		wchar_t *outstr = new wchar_t[olen+1];
		
		ep = (unsigned char *)src+slen;
		ip = (unsigned char *)src;
		wchar_t *op = outstr;
		wchar_t tmp;
		while (ip<ep){
			if (*ip >= pfx[3]){	
				if (
					(ep - ip < 4)
				|| (*(ip+1) < 0x80 || *(ip+2) < 0x80 || *(ip+3) < 0x80)
				){
					*op = L'?';
					++op;
				}else{
					tmp = (*(ip+3)) & ~(0xC0);
					tmp |= ((*(ip+2)) & ~(0xC0)) << 6;
					tmp |= ((*(ip+1)) & ~(0xC0)) << 12;
					tmp |= ((*(ip)) & ~(0xF8)) << 18;
					*op = tmp;
				}
				++op;
				ip+=4;
			}else if (*ip >= pfx[2]){
				if (
					(ep - ip < 3)
				|| (*(ip+1) < 0x80 || *(ip+2) < 0x80)
				){
					*op = L'?';
					++op;
				}else{
					tmp = ((*(ip+2)) & ~(0xC0));
					tmp |= ((*(ip+1)) & ~(0xC0)) << 6;
					tmp |= ((*(ip)) & ~(0xF0)) << 12;
					*op = tmp;
				}
				++op;
				ip+=3;
			}else if (*ip >= pfx[1]){
				if (
					(ep - ip < 2)
				|| (*(ip+1) < 0x80)
				){
					*op = L'?';
					++op;
				}else{
					tmp = ((*(ip+1)) & ~(0xC0));
					tmp |= ((*(ip)) & ~(0xE0)) << 6;
					*op = tmp;
				}
				++op;
				ip+=2;
			}else{
				tmp = *ip;
				*op = tmp;
				++op;
				++ip;
			}
		}
		outstr[olen]='\0';
		return outstr;
#endif
}

wchar_t *towchar_t(const wchar_t *src,size_t len,size_t &olen){
	olen=len;
	wchar_t *dst = new wchar_t[olen+1];
	memcpy(dst,src,(sizeof(wchar_t)) * olen);
	dst[len+1]='\0';
	return dst;
}

std::wstring & towstr(std::wstring &dst, const char *src,size_t len){
	size_t olen;
	wchar_t *tmp = towchar_t(src,len,olen);
	dst.assign(tmp,olen);
	delete [] tmp;
	return dst;
}

std::wstring & towstr(std::wstring &dst, const wchar_t *src,size_t len){
	dst.assign(src,len);
	return dst;
}

std::wstring & towstr(std::wstring &dst, const std::wstring &src){
	dst=src;
	return dst;
}

std::wstring & towstr(std::wstring &dst, const std::string &src){
	return towstr(dst,src.c_str(),src.length());
}


std::string & tostr(std::string &dst, const char *src,size_t len){
	dst.assign(src,len);
	return dst;
}

std::string & tostr(std::string &dst, const wchar_t *src,size_t len){
	size_t olen;
	char *tmp = tochar(src,len,olen);
	dst.assign(tmp,olen);
	delete [] tmp;
	return dst;
}

std::string & tostr(std::string &dst, const std::wstring &src){
	size_t olen;
	char *tmp = tochar(src.c_str(),src.length(),olen);
	dst.assign(tmp,olen);
	delete [] tmp;
	return dst;
}

std::string & tostr(std::string &dst, const std::string &src){
	dst = src;
	return dst;
}

}
}
