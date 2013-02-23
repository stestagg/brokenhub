#ifndef JSON_convert_h_INCLUDED
#define JSON_convert_h_INCLUDED

#include "config.h"
#include "types.h"
#include <string>

namespace JSON{
	/*! \brief Character-set conversion functions.
	 *
	 * I'm sure some (many?) people are wondering why this isn't a template class
	 * Well, at some point it probably will be,
	 * But not yet.
	 *
	 * On windows, these functions use the builting API string conversions.
	 * On other operating systems, custom conversion routines are used, on 
	 * the basis that the conversion is not all that difficult.
	 * 
	 * I'm going to claim that the definitions are pretty self-explanatory,
	 * so it should be possible to work out what is going on pretty easily.
	 *
	 * The achar and astr types are used to allow us to change the internal 
	 * character type at compile time, again, templates would also allow this.
	 */
	namespace convert{

		astr &toastr(astr &dst,const char *src,size_t len);
		astr &toastr(astr &dst,const wchar_t *src,size_t len);
		astr &toastr(astr &dst,const std::string &src);
		astr &toastr(astr &dst,const std::wstring &src);

		achar *toachar(const char *src,size_t len,size_t &olen);
		achar *toachar(const wchar_t *src,size_t len,size_t &olen);

		char *tochar(const char *src,size_t len,size_t &olen);
		char *tochar(const wchar_t *src,size_t len,size_t &olen);

		wchar_t *towchar_t(const char *src,size_t len,size_t &olen);
		wchar_t *towchar_t(const wchar_t *src,size_t len,size_t &olen);
	
		std::wstring & towstr(std::wstring &dst, const char *src,size_t len);
		std::wstring & towstr(std::wstring &dst, const wchar_t *src,size_t len);
		std::wstring & towstr(std::wstring &dst, const std::wstring &src);
		std::wstring & towstr(std::wstring &dst, const std::string &src);

		std::string & tostr(std::string &dst, const char *src,size_t len);
		std::string & tostr(std::string &dst, const wchar_t *src,size_t len);
		std::string & tostr(std::string &dst, const std::wstring &src);
		std::string & tostr(std::string &dst, const std::string &src);
	}
}
#endif

