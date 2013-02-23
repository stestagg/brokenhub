#pragma once

namespace JSON{
	class test{
	public:
		static bool fail();
		static bool types();
		static bool create();

		static bool convert1();

		static bool sets1();
		static bool sets2();
		static bool sets3();
		static bool sets4();
		static bool sets5();

		static bool setn1();
		static bool setn2();
		static bool setn3();
		static bool setn4();

		static bool setb1();
		static bool setlots1();
	
		static bool getstr_bool();
		static bool getstr_int();
		static bool getstr_float();

		static bool getstr_empty_ob();
		static bool getstr_empty_array();

		static bool array1();
		static bool array2();

		static bool object1();
		static bool object2();
		static bool object3();

		static bool encoding1();
		static bool example1();
	};
	class UTF_test{
	public:
		static bool buffer1();
		static bool buffer2();
		static bool buffer3();
		static bool buffer4();

		static bool reads();
		static bool read_file();
		static bool read_file2();
		static bool reads1();
		static bool reads2();
		static bool reads3();
		static bool reads4();
		static bool reads5();

		static bool reado1();

		static bool readl1();
		static bool readn1();
		static bool whitespace();
		static bool comment();
		static bool readarray();
		static bool readval();
		static bool readerrs();
	};
	class UNI_test{
	public:
		static bool buffer1();
		static bool buffer2();
		static bool buffer3();
		static bool buffer4();

		static bool reads();
		static bool reads1();
		static bool reads2();
		static bool reads3();
		static bool reads4();
		static bool reads5();

		static bool reado1();

		static bool readl1();
		static bool readn1();
		static bool whitespace();
		static bool comment();
		static bool readarray();
		static bool readval();
		static bool readerrs();
		
	};
}

using namespace JSON;

typedef bool (testfn)();


struct tst{
	const char *name;
	const char *desc;
	testfn *fn;
	bool eog;
	bool fatal;
};

#ifdef INCLUDE_TESTS_IN_THIS_MODULE
tst tests[]={
	//Group 1
	{"Always Fail","This test ALWAYS FAILS",test::fail,false,false},
	{"Types","Check that the defined types are expected sizes",test::types,false,true},
	{"Constructor","Check that the we can create a blank value object",test::create,true,true},
	//Group 1.5
	{"Encoding 1","Sample Encodings",test::encoding1,true,true},
	//Group 2
	{"Str 1","Set the value to a string, check that datatype changes correctly",test::sets1,false,true},
	{"Str 2","Set the value to a string, check that datatype changes correctly",test::sets2,false,true},
	{"Str 3","Set the value to a string, check that datatype changes correctly",test::sets3,false,true},
	{"Str 4","Set the value to a string, check that datatype changes correctly",test::sets4,false,true},
	{"Str 5","Set the value to a string, check that datatype changes correctly",test::sets5,false,true},
	{"Num 1","Set the value to a number",test::setn1,false,true},
	{"Num 2","Set the value to a number",test::setn2,false,true},
	{"Num 3","Set the value to a number",test::setn3,false,true},
	{"Num 4","Set the value to a number",test::setn4,false,true},
	{"Bool 1","Set the value to a bool",test::setb1,false,true},
	{"Set Lots","Set a value to different variables",test::setlots1,true,true},	
	//Group 3
	{"Out Bool","Testing output with bool",test::getstr_bool,false,true},
	{"Out Integer","Testing output with Integer",test::getstr_int,false,true},
	{"Out Float","Testing output with Float",test::getstr_float,false,true},
	{"Out Ar 0","Testing output with empty Array",test::getstr_empty_array,false,true},
	{"Out Ar 1","Testing output with Array of Numbers",test::array1,false,true},
	{"Out Ar 2","Testing output with Array of Strings",test::array2,false,true},
	{"Out Ob 0","Testing output with empty Object",test::getstr_empty_ob,false,true},
	{"Out Ob 1","Testing output with Object of Numbers",test::object1,false,true},
	{"Out Ob 2","Testing output with Object with Array",test::object2,false,true},
	{"Out Ob 3","Checking objects get cleaned-up",test::object3,true,true},
	//Group 4
	{"Encoding 1","Tricky Encoding, and conversion",test::encoding1,true,true},
	//Group 5
	{"UTF8 String Buffer","Creates a UFT8 buffer, and checks it is sane",UTF_test::buffer1,false,true},
	{"UTF8 char Buffer","Creates a UFT8 buffer, and checks it is sane",UTF_test::buffer2,false,true},
	{"UTF8 mmap file Buffer","Creates a mmapped UFT8 buffer.  Will fail if test.json does not exist.",UTF_test::buffer3,false,false},
	{"UTF8 mmap file Buffer","Checks that mmapped buffers get unmapped.",UTF_test::buffer4,false,true},
	{"UTF8 string reading","Reads a Simple UTF-8 quoted string.",UTF_test::reads,false,true},
	{"UTF8 file parsing","Parse test.json.",UTF_test::read_file,false,true},
	{"UTF8 parse non existant file","Parse non existant file.",UTF_test::read_file2,false,true},
	{"UTF8 string escaping1","Reads a UTF-8 quoted string with escapes.",UTF_test::reads1,false,true},
	{"UTF8 string escaping2","Reads a UTF-8 quoted string with \\x escapes.",UTF_test::reads2,false,true},
	{"UTF8 string escaping3","Reads a UTF-8 quoted string with \\u escapes.",UTF_test::reads3,false,true},
	{"UTF8 string escaping4","UTF-8 with broken \\u escapes.",	UTF_test::reads4,false,true},
	{"UTF8 string escaping5","UTF-8 reading.",					UTF_test::reads5,false,true},
	{"UTF8 literals",		 "UTF-8 parsing literals.",			UTF_test::readl1,false,true},
	{"UTF8 numbers",		 "UTF-8 parsing numbers.",			UTF_test::readn1,false,true},
	{"UTF8 whitespace",		 "UTF-8 whitespaces are read correctly",		UTF_test::whitespace,false,true},
	{"UTF8 comments",		 "UTF-8 comments are read correctly",		UTF_test::comment,false,true},
	{"UTF8 readarray",		 "UTF-8 read array",		UTF_test::readarray,false,true},
	{"UTF8 readval",		 "UTF-8 read simple value",		UTF_test::readval,false,true},
	{"UTF8 readerrs",		 "UTF-8 read invalid strings",	UTF_test::readerrs,true,true},
	{"UTF8 unquoted strings","Objects with unquoted names.",UTF_test::reado1,false,true},

	{"UNI String Buffer",	 "Do UNI string buffers work",	UNI_test::buffer1,false,true},
	{"UNI String Buffer",	 "Do UNI char buffers work",	UNI_test::buffer2,false,true},
	{"UNI File Mapping",	 "Do UNI file maps work",		UNI_test::buffer3,false,true},
	{"UNI string reading",	"Reads a Simple UNI quoted string.",UNI_test::reads,false,true},
	{"UNI string escaping1","Reads a UNI quoted string with escapes.",UNI_test::reads1,false,true},
	{"UNI string escaping2","Reads a UNI quoted string with \\x escapes.",UNI_test::reads2,false,true},
	{"UNI string escaping3","Reads a UNI quoted string with \\u escapes.",UNI_test::reads3,false,true},
	{"UNI unquoted strings","Objects with unquoted names.",UNI_test::reado1,true,true},
	{"Example1","Objects with unquoted names.",test::example1,true,true},
};
#endif
