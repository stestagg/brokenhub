#include "../libJSONpp/config.h"
#include "../libJSONpp/convert.h"
#include "../libJSONpp/value.h"
#include "../libJSONpp/parser_UNI.h"

#include <iostream>


#include "tests1.h"
using namespace JSON;

#pragma region group 6

bool UNI_test::buffer1(){
	JSON::unibuffer bfr;
	const wchar_t *teststr = L"Hello World";
	if (!bfr.mapstring(teststr)) return false;

	if (bfr.len != 11)	return false;
	if (bfr.end != teststr+11) return false;
	
	if (bfr.bfr != teststr) return false;

	return true;
}

bool UNI_test::buffer2(){
	JSON::unibuffer bfr;
	const wchar_t *teststr = L"Hello World";
	if (!bfr.mapchararray(teststr,5)) return false;

	if (bfr.len != 5)	return false;
	if (bfr.end != teststr+5) return false;
	
	if (bfr.bfr != teststr) return false;
	return true;
}

bool UNI_test::buffer3(){
	JSON::unibuffer bfr;
#ifdef JSON_WINDOWS
	if (!bfr.mapfile(L"test.json.UCS2")) return false;
#endif
#ifdef JSON_POSIX
	if (!bfr.mapfile(L"test.json.UTF32")) return false;
#endif
	if (wcsncmp(bfr.ptr,L"{\n\t\"Hello\"",10)!=0) return false;
	
	return true;
}


bool UNI_test::reads(){
	JSON::parser_UNI parser(L"\"Hello World\"");
	astr s;
	bool b = parser.readstring(s);
	if (!b) return false;
	if (s != A("Hello World")) return false;
	return true;
}

bool UNI_test::reads1(){
	JSON::parser_UNI parser(L"\"This is a \\\"quote\\\".\\nThis is on a new line.\"");
	astr s;
	bool b = parser.readstring(s);
	if (!b) return false;
	if (s != A("This is a \"quote\".\nThis is on a new line.")) return false;
	return true;
}

bool UNI_test::reads2(){
	JSON::parser_UNI parser(L"\"\\x48\\x65\\x6C\\x6C\\x6F\\x00\\x57\\x6F\\x72\\x6C\\x64\"");
	astr s;
	astr s_compare;
	s_compare.assign(A("Hello\0World"),strlen("Hello World"));
	bool b = parser.readstring(s);
	if (!b) return false;

	if (s != s_compare) return false;
	return true;
}

bool UNI_test::reads3(){
	JSON::parser_UNI parser(L"\"\\u1FCB\\u1FD2\\u201A\\u2001\\u1FA5\\u1FAA\\u1E5F\\u1E3C\\u1E0D\"");
	astr s;
#ifdef JSON_USE_CHAR
	char s_compare[28] = {(char)0xE1,(char)0xBF,(char)0x8B,(char)0xE1,(char)0xBF,(char)0x92,(char)0xE2,(char)0x80,
						  (char)0x9A,(char)0xE2,(char)0x80,(char)0x81,(char)0xE1,(char)0xBE,(char)0xA5,(char)0xE1, 
						  (char)0xBE,(char)0xAA,(char)0xE1,(char)0xB9,(char)0x9F,(char)0xE1,(char)0xB8,(char)0xBC,
						  (char)0xE1,(char)0xB8,(char)0x8D,0x00};
#else
	wchar_t *s_compare = L"\u1FCB\u1FD2\u201A\u2001\u1FA5\u1FAA\u1E5F\u1E3C\u1E0D";
#endif
	bool b = parser.readstring(s);
	if (!b) return false;

	if (s != s_compare) return false;
	return true;
}

bool UNI_test::reado1(){
	JSON::parser_UNI parser(L"{a:\"ba\", b:2}");
	astr s;
	JSON::value v;
	parser.parse(v);
	if (parser.errorcount() > 2) return false; // If error 11 is enabled, we don't want to fail here
	astr dst;
	v.getstring(dst,true,false);
	if (
		(dst != A(" { \"a\" : \"ba\" , \"b\" : 2 } "))
	 && (dst != A(" { \"b\" : 2 , \"a\" : \"ba\" } "))
	){ return false;}
	return true;
}

#pragma endregion
