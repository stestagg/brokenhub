#include "../libJSONpp/types.h"
#include "../libJSONpp/convert.h"
#include "../libJSONpp/value.h"
#include "../libJSONpp/parser_UTF8.h"

#include <iostream>


#include "tests1.h"
using namespace JSON;


#pragma region group 1


bool test::fail(){
	return false;
}

bool test::types(){	
	#ifdef JSON_NUMBER_i64
		if (sizeof(aint)!=8) return false;
	#endif
	#ifdef JSON_NUMBER_INT
		if (sizeof(aint)!=sizeof(int)) return false;
	#endif
	#ifdef JSON_NUMBER_LONG
		if (sizeof(aint)!=sizeof(long)) return false;
	#endif

	#ifdef JSON_USE_WCHAR
		if (sizeof(achar)!=sizeof(wchar_t)) return false;
	#endif
	#ifdef JSON_USE_CHAR
		if (sizeof(achar)!=sizeof(char)) return false;
	#endif
	
	return true;
}

bool test::create(){
	JSON::value *newval;
	try{
		newval = new JSON::value();
	}catch(...){
		return false;
	}
	if (!newval){
		return false;
	}
	delete newval;
	return true;
}

#pragma endregion

#pragma region group 1.5

bool test::convert1(){
	std::string a = "Hi";
	std::wstring ac;
	ac = convert::towstr(ac,a);
	if (ac != L"Hi") return false;

	a = "Hi\0World";
	std::wstring b;
	b.assign(L"Hi\0World",strlen("Hi World"));
	ac = convert::towstr(ac,a);
	if (ac != b) return false;

	return true;
}


#pragma endregion

#pragma region group 2

bool test::sets1(){
	JSON::value nval;
	nval.setstring("Hello World");
	if (nval.getdatatype() != JSON::datatype::_string){
		return false;
	}
	return true;
}

bool test::sets2(){
	JSON::value nval;
	nval="Hello World";
	if (nval.getdatatype() != JSON::datatype::_string){
		return false;
	}
	return true;
}

bool test::sets3(){
	JSON::value nval;
	nval=L"Hello World";
	if (nval.getdatatype() != JSON::datatype::_string){
		return false;
	}
	return true;
}

bool test::sets4(){
	JSON::value nval;
	std::string z("Hello World");
	nval=z;
	if (nval.getdatatype() != JSON::datatype::_string){
		return false;
	}
	return true;
}

bool test::sets5(){
	JSON::value nval;
	std::wstring z(L"Hello World");
	nval.setstring(z);
	if (nval.getdatatype() != JSON::datatype::_string){
		return false;
	}
	return true;
}

bool test::setn1(){
	JSON::value nval;
	
	nval.setnumber(1);
	if (nval.getinteger(0) == 1)	return true;
	return false;
}

bool test::setn2(){
	JSON::value nval;
	
	nval.setnumber(3.14);
	if (nval.getinteger(0) == 3)	return true;
	return false;
}

bool test::setn3(){
	JSON::value nval;
	nval.setnumber(aint_max);

	if (aint_max == nval.getinteger()) return true;
	return false;
}

bool test::setn4(){
	JSON::value nval;
	nval.setnumber(3.141592);

	if ( nval.getfloat() > 3.141591 && nval.getfloat() < 3.141593 ) return true;
	return false;
}

bool test::setb1(){
	JSON::value nval;
	nval.setliteral(true);
	if ( nval.getbool() != true) return false;
	nval.setliteral(false);
	if ( nval.getbool() != false) return false;
	return true;
}

bool test::setlots1(){
	JSON::value nval;

	nval = true;
	nval = false;
	nval = "Hello World";
	nval = 3.1415926;
	nval = JSON::literals::_null;
	nval = 1024;

	if (nval.getinteger() != 1024) return false;
	return true;
}

#pragma endregion

#pragma region group 3

bool test::getstr_bool(){
	JSON::value nval;
	nval = true;
	
	std::string compval;
	nval.getstring(compval,false,false);
	if (compval == "true") return true;

	nval = false;
	nval.getstring(compval,false,false);
	if (compval == "false") return true;

	nval = JSON::literals::_null;
	nval.getstring(compval,false,false);
	if (compval == "null") return true;

	return false;
}

bool test::getstr_int(){
	JSON::value nval;
	nval = 31415;
	
	std::string compval;
	nval.getstring(compval,false,false);
	
	if (compval == "31415") return true;
	return false;
}

bool test::getstr_float(){
	JSON::value nval;
	nval = 3.14159;
	
	std::string compval;
	nval.getstring(compval,false,false);
	
	if (compval == "3.14159") return true;
	return false;
}

bool test::getstr_empty_ob(){
	JSON::value nval(JSON::datatype::_object);

	std::string compval;
	nval.getstring(compval,false,false);
	if (compval == " {  } ") return true;
	return false;
}

bool test::getstr_empty_array(){
	JSON::value nval(JSON::datatype::_array);

	std::string compval;
	nval.getstring(compval,false,false);
	if (compval == " [  ] ") return true;
	return false;
}

bool test::array1(){
	JSON::value nval(JSON::datatype::_array);

	nval.addvalue(new JSON::value(5));
	nval.addvalue(new JSON::value(10));
	std::string compval;
	nval.getstring(compval,false,false);
//	std::cout << compval << std::endl;
	if (compval == " [ 5 , 10 ] ") return true;
	return false;
}

bool test::array2(){
	JSON::value nval(JSON::datatype::_array);

	nval.addvalue(new JSON::value("This"));
	nval.addvalue(new JSON::value(L"Is"));
	nval.addvalue(new JSON::value(std::string("A")));
	nval.addvalue(new JSON::value(std::wstring(L"Test")));
	std::string compval;
	nval.getstring(compval,true,false);
	if (compval == " [ \"This\" , \"Is\" , \"A\" , \"Test\" ] ") return true;
	return false;
}

bool test::object1(){
	JSON::value nval(JSON::datatype::_object);

	nval.addvalue("Val 1",5);
	nval.addvalue("Val 2",10);
	std::string compval;
	nval.getstring(compval,true,false );

	if (compval == " { \"Val 1\" : 5 , \"Val 2\" : 10 } " ||
		compval == " { \"Val 2\" : 10 , \"Val 1\" : 5 } ") return true;
	std::cout << compval;
	return false;
}

bool test::object2(){
	JSON::value nval(JSON::datatype::_object);

	nval.addvalue("Val 1",5);
	nval.addvalue("Val 2",10);
	nval.addvalue("Val 3",new JSON::value(JSON::datatype::_array));
	nval["Val 3"].addvalue(15);
	nval["Val 3"].addvalue(314);
	
	
	std::string compval;
	nval.getstring(compval,false,false);
	// Different containers have different orderings, so this may change:
	if (
		(compval == " { \"Val 1\" : 5 , \"Val 3\" :  [ 15 , 314 ]  , \"Val 2\" : 10 } ")
	||	(compval == " { \"Val 1\" : 5 , \"Val 2\" : 10 , \"Val 3\" :  [ 15 , 314 ]  } ")
	||	(compval == " { \"Val 3\" :  [ 15 , 314 ]  , \"Val 1\" : 5 , \"Val 2\" : 10 } ")
	||  (compval == " { \"Val 3\" :  [ 15 , 314 ]  , \"Val 2\" : 10 , \"Val 1\" : 5 } ")
	)return true;
	std::cout << compval;
	return false;
}

bool test::object3(){
	JSON::value nval;
	nval.setobject();

	nval.addvalue(L"Exhaustive","Tests");
	nval.addvalue("r",std::string("good"));
	nval.addvalue(std::wstring(L"enough").c_str(),4);
	nval.addvalue(L"business",L"!");
	nval[L"Elements get"]="Reordered, oops!";
	
	std::wstring compval;
	nval["r"].getstring(compval,false,false);
	
	if (compval != L"good")
		return false;
	
	nval = true;

	nval.setarray();

	nval+=4; nval += 6; nval += "boo! :)" ;
	nval.getstring(compval,true,false);
	
	if (compval == L" [ 4 , 6 , \"boo! :)\" ] ") return true;

	return false;
}


#pragma endregion

#pragma region group 4

bool test::encoding1(){
	const wchar_t *t1 = L"\u2085\u2075";
	JSON::value nval(JSON::datatype::_object);

	nval.addvalue(L"νăľ…₁",t1);
	nval.addvalue(L"ѴΑλ ⁴⁵",L"\\\"");
	
	
	std::wstring compval;
	nval.getstring(compval,true,false);

	if (
		(compval == L" { \"νăľ…₁\" : \"\u2085\u2075\" , \"ѴΑλ ⁴⁵\" : \"\\\\\\\"\" } ") 
		||	(compval == L" { \"ѴΑλ ⁴⁵\" : \"\\\\\\\"\" , \"νăľ…₁\" : \"\u2085\u2075\" } ") 
		)return true;
	return false;
}

bool test::example1(){
	JSON::value nval(JSON::datatype::_object);

	nval.addvalue(L"a",1);
	nval.addvalue("b","2");
	nval[L"c"]=L"3";
	nval["d"]=JSON::value(4);
	JSON::value &ary = nval["e"];
	ary.setarray();
	ary[1]="a";
	ary[2]="b";
	ary[3]=3.1415;
	ary[10]=true;

	std::string str;

	ary.getstring(str,true,false);

	std::string compare = " [ null , \"a\" , \"b\" , 3.1415 , null , null , null , null , null , null , true ] ";
	if (str != compare){
		return false;
	}

	std::string json = "{ a:1,\"b\":\"2\", c: \"3\", d: 4, e:[ null , \"a\" , \"b\" , 3.1415 , null , null , null , null , null , null , true ]}";

	JSON::parser_UTF8 parser;
	JSON::value aval;
	parser.parse(aval, json);

	if (parser.fail()){
		return false;
	}

	aval.getstring(str,true,true);
	//std::cout << str << std::endl;

	// deep, element-wise compare of the two 
	return nval == aval;
}
#pragma endregion
