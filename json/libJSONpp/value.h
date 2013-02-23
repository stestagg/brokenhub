#ifndef JSON_value_h_INCLUDED
#define JSON_value_h_INCLUDED

#include "config.h"

#ifdef JSON_WINDOWS
#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#endif
#endif

	/*!
	 * \brief
	 * The JSON namespace should hold all public classes/functions/enums/structs of the libJSONpp library
	 */
namespace JSON{
	class value;
}

#include "types.h" // defines the achar and astr types

namespace JSON {

	/*!
	 * \brief
	 * A wrapper namespace to stop the literals enum from polluting the JSON namespace.  This also helps with IDE Auto-completion somewhat.
	 */
	namespace literals{
	/*!
	 * \brief
	 * the %JSON spec names a number of literals that should be recognised.  This enum lists them.
	 */
		enum literals{
			_true,
			_false,
			_null
		};
	}

		/*!
	 * \brief
	 * A wrapper namespace to stop the datatype enum from polluting the JSON namespace.  This also helps with IDE Auto-completion somewhat.
	 */
	namespace datatype{
			/*!
			 * \brief
			 * the different datatypes that a value can hold.  
			 *
			 * While the specification only defines one numeric type, 
			 * we try to be clever and leverage the advantages of 
			 * stronger numeric types by defining _number and _fixed_number.
			 * _fixed_number is an integer type, while _number is a double.
			 * conversion between the two is fairly free-and-easy.
			 */
		enum dt{
			_undefined,
			_string,
			/*!
			 * An unquoted-string occurs when an object member name is not quoted properly, 
			 * this is surprisingly common: e.g. { a : "b" } is strictly invalid, and should be:
			 * {'a' : "b"}
			 */
			_unquoted_string,
			_literal,

			_number,
			_fixed_number,
	
			_array,
			_object,
			/*!
			 *  Special value used for convenience
			 */
			_nullval
		};
	}

	/*!
	 * \brief
	 * Used internally to manage pointers to data.  You can safely ignore this.
	 *
	 * Its main purpose is to keep the code tidier, more readable, and more type-safe [sic.] by reducing typecasting.
	 * It doesn't actually prevent any errors from happening tho.
	 */
	union dataptr{
		void *_v;
		astr *_string;
		literals::literals _literal;
 		afloat *_number;
		aint _fixed_number;
		raw_object_t *_object;
		raw_array_t *_array;
	};

	/*!
	 * \brief
	 * This class represents any %JSON value.  A value can have any of the basic %JSON datatypes, as defined in datatype::dt.
	 * internal data management is handled automatically, along with character encoding conversion between UTF-8 and Unicode. 
	 *
	 * Because of the nature of the %JSON specification, only one class is needed to represent all datatypes.  
	 * For example, a valid %JSON file can be expressed as a single %JSON 'object'.
	 *
	 */
	class value
	{
	private:
		datatype::dt dt;
		dataptr val;

	public:
		/*!
		 * \brief
		 * Used by any function that should return a reference to a value but may not be able to, for example in search functions.
		 * It is a major hack, but to test for a valid response from these functions,
		 * compare the address of the returned reference to this variable.	
		 */

		static value nullval;

		datatype::dt getdatatype();

		bool setstring();
		bool setstring(const wchar_t *newval);
		bool setstring(const std::wstring &newval);
		bool setstring(const char *nval);
		bool setstring(const std::string &newval);

		bool setint();
		bool setfloat();
#ifndef JSON_NO_int64
		bool setnumber(int64_t newval);
		value & operator=(int64_t);
#endif
		bool setnumber(int newval);
		bool setnumber(long newval);
		bool setnumber(double newval);

		bool setliteral();
		bool setliteral(literals::literals newval);
		bool setliteral(bool newval);

		bool setarray();
		bool setarray(const raw_array_t &newval);
#ifdef JSON_UNSAFE_FUNS
		bool setarray(raw_array_t *newval);

		bool setobject(raw_object_t *newval);
#endif
		bool setobject();
		bool setobject(const raw_object_t &newval);
		
		bool setdt(datatype::dt nv);
		
		bool operator==(value &v);
		bool operator!=(value &v);

		value & operator=(int);
		value & operator=(long);
		
		value & operator=(float);
		value & operator=(double);

		value & operator=(const char *);
		value & operator=(const wchar_t*);
		value & operator=(const std::wstring&);
		value & operator=(const std::string&);

		value & operator=(bool);
		value & operator=(literals::literals);

		bool addvalue(const value &newval);
		bool addvalue(const wchar_t *newname,const value &newval);
		bool addvalue(const char *newname,const value &newval);
#ifdef JSON_UNSAFE_FUNS
		bool addvalue(value *newval);	
		bool addvalue(const char *newname,value *newval);
		bool addvalue(const wchar_t *newname,value *newval);
#endif
		


		bool deletechild(value *val);	
		bool deletechild(const wchar_t *name);
		bool deletechild(const char *name);
		
		value & getchild(const wchar_t *name);
		value & getchild(const char *name);
		
		value *childexists(value *val);	
		value *childexists(const wchar_t *name);
		value *childexists(const char *name);
		value *childexists(size_t ofs);
		
		bool getbool(bool _default=false);
		JSON::literals::literals getliteral(JSON::literals::literals _default=literals::_null);
		aint getinteger(aint _default=0);
		afloat getfloat(afloat _default=0);
		astr &getstring(astr &dest,achar *_default);
		std::wstring &getstring(std::wstring &dest,bool enquote=false,bool newline=false);
		std::string &getstring(std::string &dest,bool enquote=false,bool newline=false);

		astr &getrawstring();
		raw_array_t &getrawarray();
		raw_object_t &getrawobject();

		bool clear();

		value &operator[](aint index);
		value &operator[](const wchar_t *name);
		value &operator[](const char *name);	
		value &operator+=(const value &nval);
		

		bool deletevalue();
		
		bool clone(const value &oldval);

		value();
		value(const value &nv);

		value(int nval);
		value(float nval);
		value(double nval);
		value(long nval);
#ifndef JSON_NO_int64
		value(int64_t nval);
#endif
		
		value(const char *nval);
		value(const wchar_t *nval);

		value(literals::literals nval);	
		value(bool nval);	

		value(std::string nval);
		value(std::wstring nval);
	
		value(datatype::dt);

		~value(void);
	};
}


#endif
