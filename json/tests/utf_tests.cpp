#include "../libJSONpp/config.h"
#include "../libJSONpp/convert.h"
#include "../libJSONpp/value.h"
#include "../libJSONpp/parser_UTF8.h"

#include <iostream>


#include "tests1.h"
using namespace JSON;

#pragma region group 5

bool UTF_test::buffer1(){
	JSON::utf8buffer bfr;
	const char *teststr = "Hello World";
	if (!bfr.mapstring(teststr)) return false;

	if (bfr.len != 11)	return false;
	if (bfr.end != teststr+11) return false;
	
	if (bfr.bfr != teststr) return false;

	return true;
}

bool UTF_test::buffer2(){
	JSON::utf8buffer bfr;
	const char *teststr = "Hello World";
	if (!bfr.mapchararray(teststr,5)) return false;

	if (bfr.len != 5)	return false;
	if (bfr.end != teststr+5) return false;
	
	if (bfr.bfr != teststr) return false;

	return true;
}

bool UTF_test::buffer3(){
	JSON::utf8buffer bfr;
	if (!bfr.mapfile("test.json")) return false;
	if (bfr.len != 149) return false;
	if (strncmp(bfr.ptr,"{\n\t\"Hello\"",10)!=0) return false;
	
	return true;
}

bool UTF_test::buffer4(){
#ifdef _DEBUG
	JSON::utf8buffer *bfr = new JSON::utf8buffer();

	if (!bfr->mapfile("test.json")) return false;
	const char *localb = bfr->bfr;

	char *a = new char[10];
	SIZE_T br;

	delete bfr;

	BOOL rv = ReadProcessMemory( GetCurrentProcess(), bfr->bfr, a, 10, &br);
	
	delete [] a;
	
	if (rv != FALSE) return false;
#endif
	return true;
}

bool UTF_test::reads(){
	JSON::parser_UTF8 parser("\"Hello World\"");
	astr s;
	bool b = parser.readstring(s);
	if (!b) return false;
	if (s != A("Hello World")) return false;
	return true;
}

bool UTF_test::read_file(){
	JSON::parser_UTF8 parser;
	JSON::value value;
	parser.parsefile(value, "test.json");

	if (parser.fail()) return false;
	return true;
	astr s = value.getchild("Hello").getrawstring();
	if (s != A("world")) return false;
	return true;
}

bool UTF_test::read_file2(){
	JSON::parser_UTF8 parser;
	JSON::value value;
	parser.parsefile(value, "test_does_not_exist.json");
	if (parser.fail()) return true;
	return false;
}

bool UTF_test::reads1(){
	JSON::parser_UTF8 parser("\"This is a \\\"quote\\\".\\nThis is on a new line.\"");
	astr s;
	bool b = parser.readstring(s);
	if (!b) return false;
	if (s != A("This is a \"quote\".\nThis is on a new line.")) return false;
	return true;
}

bool UTF_test::reads2(){
	JSON::parser_UTF8 parser("\"\\x48\\x65\\x6C\\x6C\\x6F\\x00\\x57\\x6F\\x72\\x6C\\x64\"");
	astr s;
	astr s_compare;
	s_compare.assign(A("Hello\0World"),strlen("Hello World"));
	bool b = parser.readstring(s);
	if (!b) return false;

	if (s != s_compare) return false;
	return true;
}

bool UTF_test::reads3(){
	JSON::parser_UTF8 parser("\"\\u1FCB\\u1FD2\\u201A\\u2001\\u1FA5\\u1FAA\\u1E5F\\u1E3C\\u1E0D\"");
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

bool UTF_test::reads4(){
	JSON::parser_UTF8 parser("\"\\d\\u1F\"");
	astr s;
	bool b = parser.readstring(s);
	if (parser.errorcount() != 1) return false;
	if (s != A("\\d1F")) return false;
	return true;
}

bool UTF_test::reads5(){
	JSON::parser_UTF8 parser("\"\xE1\xBF\x8B\x65\x6C\x6C\x6F\xE2\x80\x9A\xE2\x80\x81\xE1\xBE\xA5\xE1\xBE\xAA\xE1\xB9\x9F\xE1\xB8\xBC\xE1\xB8\x8D\"");
#ifdef JSON_USE_CHAR
	char s_compare[] = {
		(char)0xE1, (char)0xBF, (char)0x8B, (char)0x65, (char)0x6C, (char)0x6C, (char)0x6F, (char)0xE2,
		(char)0x80, (char)0x9A, (char)0xE2, (char)0x80, (char)0x81, (char)0xE1, (char)0xBE, (char)0xA5, 
		(char)0xE1, (char)0xBE, (char)0xAA, (char)0xE1, (char)0xB9, (char)0x9F, (char)0xE1, (char)0xB8,
		(char)0xBC, (char)0xE1, (char)0xB8, (char)0x8D,(char)0x00};
#else
	wchar_t s_compare[] = {
		(wchar_t)0x1FCB, (wchar_t)0x0065, (wchar_t)0x006C, (wchar_t)0x006C, (wchar_t)0x006F, 
		(wchar_t)0x201A, (wchar_t)0x2001, (wchar_t)0x1FA5, (wchar_t)0x1FAA, (wchar_t)0x1E5F,
		(wchar_t)0x1E3C, (wchar_t)0x1E0D,(wchar_t)0x0000
	};
#endif
	astr s;
	bool b = parser.readstring(s);
	
	if (s != s_compare) return false;
	return true;
}

bool UTF_test::reado1(){
	JSON::parser_UTF8 parser("{a:\"ba\", b:2}");
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

bool UTF_test::readl1(){
	JSON::parser_UTF8 parser("true");
	JSON::value l;
	bool b = parser.readlit(l);
	if (l.getbool() != true) return false;	

	parser.bfr.mapstring("false");
	b = parser.readlit(l);
	if (l.getbool() != false) return false;	

	parser.bfr.mapstring("null");
	b = parser.readlit(l);
	if (l.getliteral(JSON::literals::_true) != JSON::literals::_null) return false;	

	return true;
}

bool UTF_test::readn1(){
	JSON::parser_UTF8 parser("1");
	JSON::value l;
	bool b = parser.readnum(l);
	if (l.getinteger() != 1) return false;	

	parser.bfr.mapstring("12345");
	b = parser.readnum(l);
	if (l.getinteger() != 12345) return false;	

	parser.bfr.mapstring("1.2345");
	b = parser.readnum(l);
	if (l.getfloat() != 1.2345) return false;	

	parser.bfr.mapstring("1e2");
	b = parser.readnum(l);
	if (l.getfloat() != 100) return false;

	parser.bfr.mapstring("3.1415z");
	b = parser.readnum(l);
	if (l.getfloat() != 3.1415) return false;

	return true;
}

bool UTF_test::whitespace(){
	char test[] ={  //Each whitespace char encoded in UTF-8 in the form .....B.....C.
		(char)0x09, (char)0x09, (char)0x0A, (char)0x0B, (char)0x0C, (char)0x0D, (char)0x20, 
		(char)0xC2, (char)0xA0, (char)0xE1, (char)0x9A, (char)0x80, (char)0xE1, (char)0xA0, 
		(char)0x8E, (char)0xE2, (char)0x80, (char)0x80, (char)0xE2, (char)0x80, (char)0x81, 
		(char)0xE2, (char)0x80, (char)0x82, (char)0xE2, (char)0x80, (char)0x83, (char)0xE2, 
		(char)0x80, (char)0x84, (char)0xE2, (char)0x80, (char)0x85, (char)0xE2, (char)0x80, 
		(char)0x86, (char)0x42, (char)0xE2, (char)0x80, (char)0x87, (char)0xE2, (char)0x80, 
		(char)0x88, (char)0xE2, (char)0x80, (char)0x89, (char)0xE2, (char)0x80, (char)0x8A, 
		(char)0xE2, (char)0x80, (char)0xA8, (char)0xE2, (char)0x80, (char)0xA9, (char)0xE2, 
		(char)0x80, (char)0xAF, (char)0xE2, (char)0x81, (char)0x9F, (char)0xE3, (char)0x80, 
		(char)0x80, (char)0x43, (char)0x00
	};
	JSON::parser_UTF8 parser(test);

	parser.skipWS();
	if (*(parser.bfr.ptr) != 'B') return false;
	parser.bfr.ptr ++;
	parser.skipWS();
	if (*(parser.bfr.ptr) != 'C') return false;
	return true;
}

bool UTF_test::comment(){
	JSON::parser_UTF8 parser("/*thisisatest*/ABCUS");

	parser.skipWS();
	if (*(parser.bfr.ptr) != 'A') return false;

	parser.bfr.mapstring(" //Boo!\n/*[hehehe]*/  /**/AB/**/CUS");
	parser.skipWS();
	if (*(parser.bfr.ptr) != 'A') return false;
	return true;
}

bool UTF_test::readarray(){
	JSON::parser_UTF8 parser("[ \"a\" , \"b\" ]");
	JSON::value a;
	parser.readarray(a);
	if (a.getrawarray().size() != 2){
		return false;
	}

	parser.bfr.mapstring("[ 3 1 4 1 5 9 2 6 5 3]");
	parser.getvalue(a);
	if (a.getrawarray().size() != 10){
		return false;
	}
	if (a[5].getinteger() != 9) return false;

	parser.bfr.mapstring("[ [1,1,2,3,5],[8,13,21,34]]");
	parser.getvalue(a);
	astr tmp;
	a.getstring(tmp);

	if (tmp != A(" [  [ 1 , 1 , 2 , 3 , 5 ]  ,  [ 8 , 13 , 21 , 34 ]  ] ")){
		return false;
	}

	return true;
}

bool UTF_test::readval(){
	JSON::parser_UTF8 parser("{ \"a\" : \"b\" }");
	JSON::value a;
	parser.getvalue(a);
	if (a["a"].getrawstring() != A("b")){
		return false;
	}

	parser.bfr.mapstring("{ a : \"b\", c : 4}");
	parser.getvalue(a);
	if (a["c"].getinteger() != 4){
		return false;
	}

	parser.bfr.mapstring("{ : \"b\", c : 4 d : \"h\"}");
	parser.getvalue(a);
	if (a["c"].getinteger() != 4){
		return false;
	}

	return true;
}

bool UTF_test::readerrs(){
	JSON::parser_UTF8 parser("{ \"a\" : \"b\" :\"v\" }");
	JSON::value a;
	parser.getvalue(a);
	

	parser.bfr.mapstring("{ a b c d e f }");
	parser.getvalue(a);


	parser.bfr.mapstring("{ identifier : \"valu");
	parser.getvalue(a);

	parser.bfr.mapstring("{ array : [ 1, 2, 3, 4, 5, }");
	parser.getvalue(a);

	parser.bfr.mapstring("{ \"string:\" : \"escape! \\");
	parser.getvalue(a);

	parser.bfr.mapstring("{ \"string:\" : \"escape! \\u21");
	parser.getvalue(a);

	parser.bfr.mapstring("{ \"string:\" : \"escape! \\x2");
	parser.getvalue(a);
	
	return true;
}


#pragma endregion
