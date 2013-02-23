//#include "parser_errors.h"
#include "types.h"

namespace JSON{
	namespace errors{
	const achar *errors[]={
/*0*/		A(""),
/*1*/		A("End of input was reached while parsing a string."),
/*2*/		A("Invalid \\x escape sequence"),
/*3*/		A("Invalid \\u escape sequence"),
/*4*/		A("Unterminated /* comment"),
/*5*/		A("End of file found in middle of object/array"),
/*6*/		A("Value name not found in object (premature ':' found)"),
/*7*/		A("Object value with no name."),
/*8*/		A("missing , in object/array"),
/*9*/		A("Could not open file"),
/*10*/		A("Could not map string!"),
///*11*/		A("Object name is unquoted"),
		};
	}	
}

