#include "value.h"

#include <cmath>
#include <limits>
#include "convert.h"

using namespace JSON;
using namespace std;

value value::nullval(datatype::_nullval);

#pragma region utility functions
	/*!
	 * \brief
	 * Replace all occurrences of \p search with \p replace in \p srcstr
	 * 
	 * \param srcstr
	 * A reference to the std::string or std::wstring that is to be operated on.
	 *
	 * \param search
	 * The string to search for
	 *
	 * \param replace
	 * The string to replace matches with.
	 * 
	 * \returns
	 * nothing.  \p srcstr is modified directly.
	 * 
	 */
void replace_all(astr &srcstr,const achar *search,const achar *replace){
	size_t searchlen=astrlen(search);
	size_t replacelen=astrlen(replace);
	size_t fnd=0;
	size_t schstart=0;
	while((fnd=srcstr.find(search,schstart))!=std::basic_string<achar>::npos){
		srcstr.replace(fnd,searchlen,replace);
		schstart=fnd+replacelen;
	}
}
	/*!
	 * \brief
	 * Makes \p thestr safe for output as a %JSON string, by enquoting dangerous characters
	 * 
	 * \param thestr
	 * A reference to the std::string or std::wstring that is to be operated on.
	 * 
	 * \returns
	 * nothing.  \p thestr is modified directly.
	 * 
	 */
void quotestring(astr &thestr){
	replace_all(thestr,A("\\"),A("\\\\"));
	replace_all(thestr,A("\""),A("\\\""));
	replace_all(thestr,A("\n"),A("\\n"));
	replace_all(thestr,A("\r"),A("\\r"));
	astr tmpstr=thestr;
	thestr=A("\"");
	thestr+=tmpstr;
	thestr+=A("\"");
}

	/*!
	 * \brief
	 * Iterates over the members of an array, formatting them for output.
	 * 
	 * \param thestr
	 * A reference to a std::string or std::wstring that will have the output appended to it.
	 * 
	 * \param enquote
	 * If true, strings and names will be enquoted before output.  This is neccesary to output correct %JSON
	 *
	 * \param separate
	 * If true, will output each value on a separate line
	 *
	 * \param theval
	 * Reference to the raw_array_t that will be converted to string.
	 *
	 * \returns
	 * nothing.  \p thestr is modified directly.
	 * 
	 */
void arraygetstring(astr &thestr,raw_array_t &theval,bool enquote=false,bool separate=false){
	thestr+=A(" [ ");
	raw_array_t::iterator theiter;
	bool started=false;
	for (theiter=theval.begin();theiter!=theval.end();++theiter){
		if (started){ 
			thestr+=A(" , ");
			if (separate) thestr+=A("\n");
		}else started=true;
		astr stra;
		(*theiter)->getstring(stra,enquote,separate);
		thestr+=stra;
	}
	thestr+=A(" ] ");
}

/*!
	 * \brief
	 * Iterates over the members of an object, formatting them for output.
	 * 
	 * \param thestr
	 * A reference to a std::string or std::wstring that will have the output appended to it.
	 * 
	 * \param enquote
	 * If true, strings and names will be enquoted before output.  This is neccesary to output correct %JSON
	 *
	 * \param separate
	 * If true, will output each value on a separate line
	 *
	 * \param theval
	 * The raw_object_t to be converted to string.
	 *
	 * \returns
	 * nothing.  \p thestr is modified directly.
	 * 
	 */
void objectgetstring(astr &thestr,raw_object_t &theval,bool enquote,bool separate){
	thestr+=A(" { ");
	raw_object_t::iterator theiter;
	bool started=false;
	for (theiter=theval.begin();theiter!=theval.end();++theiter){
		if (started){
			thestr+=A(" , ");
			if (separate) thestr+=A("\n");
		}else started=true;
		astr namestr=(*theiter).first;
		quotestring(namestr);
		thestr+=namestr;
		thestr+=A(" : ");
		astr stra;
		(*theiter).second->getstring(stra,enquote);
		thestr+=stra;			
	}
	
	thestr+=A(" } ");
}

#pragma endregion

#pragma region (con/de)structors

/*!
	 * \brief
	 * Basic constructor.  Initialises a %JSON value to the 'undefined' datatype
	 * 
	 */
value::value () : dt(datatype::_undefined){}

	/*!
	 * \brief
	 * Copy constructor, will clone the contents of \p nv 
	 * 
	 */
value::value(const value &nv): dt(datatype::_undefined){
	clone(nv);
}

	/*!
	 * \brief
	 * Initialises this value to a %JSON Number.  Internally, uses an Integer for storage.
	 * 
	 */
value::value(int nval): dt(datatype::_undefined){
	setnumber(nval);
}

	/*!
	 * \brief
	 * Initialises this value to a %JSON Number.  Internally uses a float/double for storage
	 * 
	 */
value::value(float nval): dt(datatype::_undefined){
	setnumber(nval);
}

	/*!
	 * \brief
	 * Initialises this value to a %JSON Number.  Internally uses a float/double for storage
	 * 
	 */
value::value(double nval): dt(datatype::_undefined){
	setnumber(nval);
}

	/*!
	 * \brief
	 * Initialises this value to a %JSON Number.  Internally uses an integer for storage
	 * 
	 */
value::value(long nval): dt(datatype::_undefined){
	setnumber(nval);
}
#ifndef JSON_NO_int64
	/*!
	 * \brief
	 * Initialises this value to a %JSON Number.  Internally uses an integer for storage.
	 * This function is not available if the compile-time switch JSON_NO_int64 is defined
	 * 
	 */
value::value(int64_t nval): dt(datatype::_undefined){
	setnumber(nval);
}
#endif
	/*!
	 * \brief
	 * Initialises this value to a String containing a copy of \p nval. Depending on the compile options, this value may be converted from UTF-8 into Unicode
	 * 
	 */
value::value(const char *nval): dt(datatype::_undefined){
	setstring(nval);
}

	/*!
	 * \brief
	 * Initialises this value to a String containing a copy of \p nval. Depending on the compile options, this value may be converted from Unicode into UTF-8
	 * 
	 */
value::value(const wchar_t *nval): dt(datatype::_undefined){
	setstring(nval);
}

	/*!
	 * \brief
	 * Initialises this value to a 'literal'(literals::literals) (i.e. true, false or null) 
	 * 
	 */
value::value(literals::literals nval): dt(datatype::_undefined){
	setliteral(nval);
}
/*!
	 * \brief
	 * Initialises this value to the 'literal'(literals::literals) true, or false.
	 * 
	 */
value::value(bool nval): dt(datatype::_undefined){
	setliteral(nval);
}

/*!
	 * \brief
	 * Initialises this value to the datatype specified in \p ndt.  Default data %values will be used.
	 * 
	 */
value::value(datatype::dt ndt){
	dt = datatype::_undefined;
	setdt(ndt);
}

/*!
	 * \brief
	 * Initialises this value to a String containing a copy of \p nval. Depending on the compile options, this string may be converted from UTF-8 into Unicode
	 * 
	 */
value::value(std::string nval): dt(datatype::_undefined){
	setstring(nval.c_str());
}
 
/*!
	 * \brief
	 * Initialises this value to a String containing a copy of \p nval. Depending on the compile options, this string may be converted from Unicode into UTF-8
	 * 
	 */
value::value(std::wstring nval): dt(datatype::_undefined){
	setstring(nval.c_str());
}

/*!
	 * \brief
	 * Destructor, will handle freeing all memory associated with this value.
	 * 
	 */
value::~value(void){
	deletevalue();
}

#pragma endregion

#pragma region Miscellaneous functions
/*!
	 * \brief
	 * Sets this value to be an exact copy of the data in another value.  Recursively copying arrays and objects.
	 *
	 * \returns
	 * true if the copy succeeds, false if an unknown error occurred (i.e. data corruption)
	 * 
	 */
bool value::clone(const value &oldval){
	
	raw_array_t::iterator valaiter;
	raw_object_t::iterator valoiter;
	switch(oldval.dt){
		case datatype::_unquoted_string:
		case datatype::_string:
			setstring(*oldval.val._string);
			break;
		case datatype::_number:
			setnumber((*oldval.val._number));
			break;
		case datatype::_fixed_number:
			setnumber(oldval.val._fixed_number);
			break;
		case datatype::_literal:
			setliteral(oldval.val._literal);
			break;
		case datatype::_array:
			setarray();
			for (valaiter=oldval.val._array->begin();valaiter!=oldval.val._array->end();++valaiter){
				addvalue((*(*valaiter)));
			}
			break;
		case datatype::_object:
			setobject();
			for (valoiter=oldval.val._object->begin();valoiter!=oldval.val._object->end();++valoiter){
				addvalue((*valoiter).first,(*(*valoiter).second));
			}
			break;
		case datatype::_undefined:
		case datatype::_nullval:
		default:	
			return false;
	}
	return true;
	
}

#pragma endregion Holds: clone, 

#pragma region deletion functions
	/*!
	 * \brief
	 * Looks to see if this value [must be array or object] contains the value pointed to by \p val.  If it does, then dval is removed from the list and deleted.
	 * 
	 * \returns
	 * \b true if the child value is found and deleted
	 * \b false if the child value is not found
	 */
bool value::deletechild(value *dval){
	if (dt == datatype::_object){
		raw_object_t::iterator dstval=val._object->begin();
		raw_object_t::iterator nd=val._object->end();
		while (dstval != nd){
			if ((*dstval).second==dval){
				delete [] (*dstval).first;
				delete (*dstval).second;
				val._object->erase(dstval);
				return true;
			}
			++dstval;
		}
		return false;	
	}else if (dt == datatype::_array){
		raw_array_t::iterator dstval=val._array->begin();
		raw_array_t::iterator nd=val._array->end();
		while (dstval != nd){
			if ((*dstval)==dval){
				delete (*dstval);
				val._array->erase(dstval);
				return true;
			}
			++dstval;
		}
		return false;	
	}
	return false;
}

/*!
	 * \brief
	 * Searches this value [must be a %JSON object] for a child value with name \p name.  If found, the child is deleted and removed from the container.
	 * Automatic Utf-8 <-> Unicode conversion is done where neccessary
	 * 
	 * \returns
	 * \b true if the child value is found and deleted
	 * \b false if the child value is not found
	 */
bool value::deletechild(const wchar_t *name){
	size_t olen;
	achar *rname=convert::toachar(name,wcslen(name),olen);
	if (dt!=datatype::_object) return false;
	raw_object_t::iterator dstval=(val._object)->find((achar *const)rname);
	if (dstval==val._object->end()){
		return false;
	}else{
		delete [] (*dstval).first;
		delete (*dstval).second;
		val._object->erase(dstval);
		return true;
	}
	return false;
}

/*!
	 * \brief
	 * Searches this value [must be a %JSON object] for a child value with name \p name.  If found, the child is deleted and removed from the list.
	 * Automatic Utf-8 <-> Unicode conversion is done where neccessary
	 * 
	 * \returns
	 * \b true if the child value is found and deleted
	 * \b false if the child value is not found
	 */
bool value::deletechild(const char *name){
	size_t olen;
	achar *rname=convert::toachar(name,strlen(name),olen);
	
	if (dt!=datatype::_object) return false;
	raw_object_t::iterator dstval=(val._object)->find((achar *const)rname);
	if (dstval==val._object->end()){
		return false;
	}else{
		delete [] (*dstval).first;
		delete (*dstval).second;
		val._object->erase(dstval);
		return true;
	}
	return false;
}

/*!
	 * \brief
	 * Removes and deletes all child values of this array or object
	 * 
	 * \returns
	 * \b true if the value has children that were deleted
	 * \b false if the value is of the wrong datatype
	 */
bool value::clear(){
	if (dt == datatype::_array){
		raw_array_t::iterator oi;
		for (oi = val._array->begin(); oi != val._array->end(); ++oi){
			delete *oi;
		}
		val._array->clear();
		return true;
	}
	if (dt == datatype::_object){
		raw_object_t::iterator oi;
		for (oi = val._object->begin(); oi != val._object->end(); ++oi){
			delete [] (*oi).first;
			delete  (*oi).second;
		}
		val._object->clear();
		return true;
	}
	return false;
}

/*!
	 * \brief
	 * Deletes the data held in this value,(same as setting the datatype to undefined).
	 * 
	 * \returns
	 * \b true if the value was set to undefined.
	 */
bool value::deletevalue(){
	if (val._v==NULL){dt=datatype::_undefined;return true;}

	switch(dt){
		case datatype::_unquoted_string:
		case datatype::_string:
			delete val._string;
		break;
		case datatype::_number:
			delete val._number;
		break;
		case datatype::_fixed_number:
			val._fixed_number=0;
		break;
		case datatype::_literal:
			val._literal=literals::_null;
		break;
		case datatype::_array:
			clear();
			delete val._array;
		break;
		case datatype::_object:
			clear();
			delete val._object;
		break;
		case datatype::_undefined:
			return true;
		case datatype::_nullval:
		default:
			return false;
	}
	dt=datatype::_undefined;
	val._v=NULL;
	return true;
}


#pragma endregion

#pragma region Value setting functions
	/*!
	 * \brief
	 * Sets this value to an empty string
	 * 
	 * \returns
	 * \b true
	 * allocation error exceptions in the stdlib will be passed through.
	 */
bool value::setstring(){
	if (dt == datatype::_nullval) return false;
	deletevalue();
	dt=datatype::_string;
	val._string=new astr;
	return true;
}
	/*!
	 * \brief
	 * Sets this value to a string containing a copy of \p newval.
	 * Character set conversions between UTF-8 and Unicode will be handled automatically
	 * 
	 * \returns
	 * \b true
	 * allocation error exceptions in the stdlib will be passed through.
	 */
bool value::setstring(const wchar_t *newval){
	if (dt == datatype::_nullval) return false;
	deletevalue();
	dt=datatype::_string;
	val._string=new astr;
	convert::toastr(*val._string,newval);
	return true;
}

	/*!
	 * \brief
	 * Sets this value to a string containing a copy of \p newval.
	 * Character set conversions between UTF-8 and Unicode will be handled automatically
	 * 
	 * \returns
	 * \b true
	 * allocation error exceptions in the stdlib will be passed through.
	 */
bool value::setstring(const std::wstring &newval){
	if (dt == datatype::_nullval) return false;
	deletevalue();
	dt=datatype::_string;
	val._string=new astr;
	convert::toastr(*val._string,newval);
	return true;	
}

	/*!
	 * \brief
	 * Sets this value to a string containing a copy of \p newval.
	 * Character set conversions between UTF-8 and Unicode will be handled automatically
	 * 
	 * \returns
	 * \b true
	 * allocation error exceptions in the stdlib will be passed through.
	 */
bool value::setstring(const char *newval){
	if (dt == datatype::_nullval) return false;
	deletevalue();
	dt=datatype::_string;
	val._string=new astr;
	convert::toastr(*val._string,newval);
	return true;
}

/*!
	 * \brief
	 * Sets this value to a string containing a copy of \p newval.
	 * Character set conversions between UTF-8 and Unicode will be handled automatically
	 * 
	 * \returns
	 * \b true
	 * allocation error exceptions in the stdlib will be passed through.
	 */
bool value::setstring(const std::string &newval){
	if (dt == datatype::_nullval) return false;
	deletevalue();
	dt=datatype::_string;
	val._string=new astr;
	convert::toastr(*val._string,newval);
	return true;
}

	/*!
	 * \brief
	 * Sets this value to a floating point %JSON Number.  
	 * The float is not initialised to any specific number.
	 * 
	 * \returns
	 * \b true
	 * allocation error exceptions in the stdlib will be passed through.
	 */
bool value::setfloat(){
	if (dt == datatype::_nullval) return false;
	deletevalue();
	dt=datatype::_number;
	val._number=new afloat;
	return true;
}
/*!
	 * \brief
	 * Sets this value to an integer %JSON Number.  
	 * The float is not initialised to any specific number.
	 * 
	 * \returns
	 * \b true
	 * allocation error exceptions in the stdlib will be passed through.
	 */
bool value::setint(){
	if (dt == datatype::_nullval) return false;
	deletevalue();
	dt=datatype::_fixed_number;
	val._number=0;
	return true;
}

#ifndef JSON_NO_int64
	/*!
	 * \brief
	 * Sets this value to an integer %JSON Number.  
	 *
	 * This function is not available if the compile-time switch JSON_NO_int64 is defined
	 * 
	 * \returns
	 * \b true if the new data matches \p newval
	 * allocation error exceptions in the stdlib will be passed through.
	 */
bool value::setnumber(int64_t newval){
	if (dt == datatype::_nullval) return false;
	deletevalue();
	dt=datatype::_fixed_number;
	val._fixed_number=(aint)newval;
	return val._fixed_number==newval;
}

/*!
	 * \brief
	 * Sets this value to an integer %JSON Number.  
	 * 
	 * \returns
	 * \b true if the new data matches \p newval
	 *
	 * Previous values are deleted
	 * This function is not available if the compile-time switch JSON_NO_int64 is defined
	 */
value & value::operator=(int64_t newval){
	if (dt == datatype::_nullval) return false;
	setnumber(newval);
	return *this;
}
#endif

/*!
	 * \brief
	 * Sets this value to an integer %JSON Number.  
	 * 
	 * \returns
	 * \b true if the new data matches \p newval
	 *
	 * Previous values are deleted
	 */
bool value::setnumber(int newval){
	if (dt == datatype::_nullval) return false;
	deletevalue();
	dt=datatype::_fixed_number;
	val._fixed_number=(aint)newval;
	return val._fixed_number==newval;
}

/*!
	 * \brief
	 * Sets this value to an integer %JSON Number.  
	 * 
	 * \returns
	 * \b true if the new data matches \p newval
	 *
	 * Previous values are deleted
	 */
bool value::setnumber(long newval){
	if (dt == datatype::_nullval) return false;
	deletevalue();
	dt=datatype::_fixed_number;
	val._fixed_number=(aint)newval;
	return val._fixed_number==newval;
}

/*!
	 * \brief
	 * Sets this value to an floating point %JSON Number.  
	 * 
	 * \returns
	 * \b true if the new data matches \p newval
	 *
	 * Previous values are deleted
	 */
bool value::setnumber(double newval){
	if (dt == datatype::_nullval) return false;
	deletevalue();
	dt=datatype::_number;
	val._number=new afloat;
	(*val._number)=(afloat)newval;
	return true;
}

/*!
	 * \brief
	 * Sets this value to a %JSON Literal(literals::literals), and initialises it to \em null.  
	 * 
	 * \returns
	 * \b true
	 *
	 * Any previous values are deleted
	 */
bool value::setliteral(){
	if (dt == datatype::_nullval) return false;
	deletevalue();
	dt=datatype::_literal;
	val._literal = literals::_null;
	return true;
}
/*!
	 * \brief
	 * Sets this value to a %JSON Literal(literals::literals), and initialises it to newval [one of:  true, false, or null]
	 * 
	 * \returns
	 * \b true
	 *
	 * Any previous values are deleted
	 */
bool value::setliteral(literals::literals newval){
	if (dt == datatype::_nullval) return false;
	deletevalue();
	dt=datatype::_literal;
	val._literal = newval;
	return true;
}

/*!
	 * \brief
	 * Sets this value to a %JSON Literal(literals::literals), and initialises it to newval [one of:  true or false]
	 * 
	 * \returns
	 * \b true
	 *
	 * Any previous values are deleted
	 */
bool value::setliteral(bool newval){
	if (dt == datatype::_nullval) return false;
	deletevalue();
	dt=datatype::_literal;
	val._literal = (newval)? literals::_true : literals::_false;
	return true;
}

/*!
	 * \brief
	 * Sets this value to an empty %JSON Array
	 * 
	 * \returns
	 * \b true
	 *
	 * Any previous values are deleted
	 */
bool value::setarray(){
	if (dt == datatype::_nullval) return false;
	deletevalue();
	dt=datatype::_array;
	val._array = new raw_array_t;
	JSON_ARRAYINIT(*val._array);
	return true;
}

/*!
	 * \brief
	 * Sets this value to a %JSON Array, and clones \p newval by recursively copying it's members.
	 * 
	 * \returns
	 * \b true
	 *
	 * Any previous values are deleted
	 */
bool value::setarray(const raw_array_t &newval){
	if (dt == datatype::_nullval) return false;
	deletevalue();
	dt=datatype::_array;
	val._array = new raw_array_t(newval);
	JSON_ARRAYINIT(*val._array);
	return true;
}
#ifdef JSON_UNSAFE_FUNS
	/*!
	 * \brief
	 * THIS METHOD IS DANGEROUS
	 * 
	 * \returns
	 * \b true
	 *
	 * The value is set directly to the raw %JSON Array pointed to by \p newval.
	 * If \p newval is owned by another %JSON value, then you will get double frees
	 * AND YOUR PROGRAM WILL CRASH. This value will delete \p newval when it finishes with it.
	 * This should only be used for optimisation purposes, and very carefully.
	 *
	 * Any previous values are deleted
	 * This method is only available if JSON_UNSAFE_FUNS is defined at compile-time
	 * 
	 */
bool value::setarray(raw_array_t *newval){
	if (dt == datatype::_nullval) return false;
	deletevalue();
	dt=datatype::_array;
	val._array = newval;
	return true;
}

	/*!
	 * \brief
	 * THIS METHOD IS DANGEROUS
	 * 
	 * \returns
	 * \b true
	 *
	 * The value is set directly to the raw %JSON Object pointed to by \p newval.
	 * If \p newval is owned by another value, then you will get double frees
	 * AND YOUR PROGRAM WILL CRASH.  This value will delete \p newval when it finishes with it.
	 * This should only be used for optimisation purposes, and very carefully.
	 *
	 * Any previous values are deleted
	 * This method is only publically available if JSON_UNSAFE_FUNS is defined at compile-time
	 * 
	 */
bool value::setobject(raw_object_t *newval){
	if (dt == datatype::_nullval) return false;
	deletevalue();
	dt=datatype::_object;
	val._object = newval;
	return true;
}
#endif

	/*!
	 * \brief
	 * Sets this value to an empty %JSON Object
	 * 
	 * \returns
	 * \b true
	 *
	 * Any previous values are deleted
	 */
bool value::setobject(){
	if (dt == datatype::_nullval) return false;
	deletevalue();
	dt=datatype::_object;
	val._object= new raw_object_t();
	JSON_OBJINIT(*val._object);

	return true;
}

	/*!
	 * \brief
	 * Sets this value to a %JSON Object, and clones \p newval by recursively copying it's members.
	 * 
	 * \returns
	 * \b true
	 *
	 * Any previous values are deleted
	 */
bool value::setobject(const raw_object_t &newval){
	if (dt == datatype::_nullval) return false;
	deletevalue();
	dt=datatype::_object;
	val._object= new raw_object_t(newval);
	JSON_OBJINIT(*val._object);
	return true;
}

	/*!
	 * \brief
	 * Deletes the existing value, and sets the value to a new type, as specified by \p nv
	 * 
	 * \returns
	 * \p the result of calling:  setarray, setnumber, setliteral, setobject, or setstring depeding on the value of \p nv
	 *
	 * Any previous values are deleted
	 */
bool value::setdt(datatype::dt nv){
	if (dt == datatype::_nullval) return false;
	deletevalue();
	switch(nv){
		case datatype::_array:
			return setarray();
		case datatype::_fixed_number:
			return setnumber(0);
		case datatype::_literal:
			return setliteral(literals::_null);
		case datatype::_number:
			return setnumber(0.0);
		case datatype::_object:
			return setobject();
		case datatype::_unquoted_string:
		case datatype::_string:
			return setstring();
		case datatype::_undefined:
			dt = datatype::_undefined;
			return true;
		case datatype::_nullval:
			dt = datatype::_nullval;
			return true;
	}
	return false;
}

#pragma endregion

#pragma region overloaded = and ==  functions

bool value::operator!=(value &v){
	return !((*this) == v);
}	


/*!
	 * \brief
	 * Test for equality, recursively if neccessary
	 */
bool value::operator==(value &v){
	raw_array_t::iterator aia, aib;
	raw_object_t::iterator oi;

	if (v.dt == datatype::_undefined) v=literals::_null;
	if (dt == datatype::_undefined) *this =literals::_null;

	if (v.dt != this->dt) return false;
	switch(v.dt){
		case datatype::_array:
			if (val._array->size() != v.val._array->size()) return false;
			for (aia = val._array->begin(), aib = v.val._array->begin(); aia != val._array->end(); ++aia, ++aib){
				if (*(*aia) != *(*aib)){
					return false;
				}
			}
			return true;
		case datatype::_fixed_number:
			return this->val._fixed_number == v.val._fixed_number;
		case datatype::_literal:
			return this->val._literal == v.val._literal;
		case datatype::_number:
			return abs((afloat)(*this->val._number - *v.val._number)) <= std::numeric_limits<afloat>::epsilon() ;
		case datatype::_object:
			if (val._object->size() != v.val._object->size()) return false;
			for (oi = val._object->begin(); oi != val._object->end(); ++oi){
				if (v[oi->first] != *oi->second){
					return false;
				}
			}
			return true;
		case datatype::_unquoted_string:
		case datatype::_string:
			return  (*v.val._string) == (*this->val._string);
		case datatype::_undefined:
			return true;
		case datatype::_nullval:
			return true;
	}
}

/*!
	 * \brief
	 * Wrapper for value::setnumber(int)
	 */
value & value::operator=(int v){
	setnumber(v);
	return *this;
}
/*!
	 * \brief
	 * Wrapper for value::setnumber(long)
	 */
value & value::operator=(long v){
	setnumber(v);
	return *this;
}
/*!
	 * \brief
	 * Wrapper for value::setnumber(double)
	 */
value & value::operator=(float v){
	setnumber(v);
	return *this;
}

/*!
	 * \brief
	 * Wrapper for value::setnumber(double)
	 */
value & value::operator=(double v){
	setnumber(v);
	return *this;
}

/*!
	 * \brief
	 * Wrapper for value::setstring(const char *)
	 */
value & value::operator=(const char *v){
	setstring(v);
	return *this;
}/*!
	 * \brief
	 * Wrapper for value::setstring(const wchar_t *)
	 */

value & value::operator=(const wchar_t*v){
	setstring(v);
	return *this;
}

/*!
	 * \brief
	 * Wrapper for value::setstring(const std::wstring&)
	 */
value & value::operator=(const std::wstring& v){
	setstring(v);
	return *this;
}

/*!
	 * \brief
	 * Wrapper for value::setstring(const std::string &)
	 */
value & value::operator=(const std::string& v){
	setstring(v);
	return *this;
}

/*!
	 * \brief
	 * Wrapper for value::setliteral(bool)
	 */
value & value::operator=(bool v){
	setliteral(v);
	return *this;
}

/*!
	 * \brief
	 * Wrapper for value::setliteral(literals::literals)
	 */
value & value::operator=(literals::literals v){
	setliteral(v);
	return *this;
}

#pragma endregion 

#pragma region Adding Child Values

	/*!
	 * \brief
	 * Adds a copy of \p newval to the end of the %JSON array
	 * 
	 * \returns
	 * \b true if the value was added.
	 * \b false if this is of the wrong datatype
	 *
	 * Any previous values are deleted
	 */
bool value::addvalue(const value &newval){
	if (dt != datatype::_array) return false;
	if (newval.dt == datatype::_nullval) return false;
	val._array->push_back(new value(newval));
	return true;
}

#ifdef JSON_UNSAFE_FUNS
/*!
	 * \brief
	 * THIS METHOD IS DANGEROUS
	 * 
	 * \returns
	 * \b true if the value was added successfully.
	 * \b false if this is not a %JSON array.
	 *
	 * \p newval is appended directly onto this array.
	 * If \p newval is owned by another %JSON value, then you will get double frees
	 * AND YOUR PROGRAM WILL CRASH. This value will delete \p newval when it finishes with it.
	 * 
	 * This should only be used for optimisation purposes, and very carefully.
	 *
	 * Any previous values are deleted
	 * This method is only available if JSON_UNSAFE_FUNS is defined at compile-time
	 * 
	 */
bool value::addvalue(value *newval){
	if (dt != datatype::_array) return false;
	if (newval->dt == datatype::_nullval) return false;
	val._array->push_back(newval);
	return true;
}

	/*!
	 * \brief
	 * THIS METHOD IS DANGEROUS
	 * 
	 * \returns
	 * \b true if the value was added successfully.
	 * \b false if this is not a %JSON object.
	 *
	 * \p newval is added directly into this %JSON object, using \p newname as a key.
	 * A copy of \p newname will be converted into the correct character encoding if neccessary
	 * If \p newval is owned by another %JSON value, then you will get double frees
	 * AND YOUR PROGRAM WILL CRASH. This value will delete \p newval when it finishes with it.
	 * 
	 * This should only be used for optimisation purposes, and very carefully.
	 *
	 * Any previous values are deleted
	 * This method is only available if JSON_UNSAFE_FUNS is defined at compile-time
	 * 
	 */
bool value::addvalue(const wchar_t *newname,value *newval){
if (dt != datatype::_object) return false;
	if (newval->dt == datatype::_nullval) return false;
	(*val._object)[(achar *)newname]=newval;
	return true;
}
	/*!
	 * \brief
	 * THIS METHOD IS DANGEROUS
	 * 
	 * \returns
	 * \b true if the value was added successfully.
	 * \b false if this is not a %JSON object.
	 *
	 * \p newval is added directly into this %JSON object, using \p newname as a key.
	 * A copy of \p newname will be converted into the correct character encoding if neccessary
	 * If \p newval is owned by another %JSON value, then you will get double frees
	 * AND YOUR PROGRAM WILL CRASH. This value will delete \p newval when it finishes with it.
	 * 
	 * This should only be used for optimisation purposes, and very carefully.
	 *
	 * Any previous values are deleted
	 * This method is only available if JSON_UNSAFE_FUNS is defined at compile-time
	 * 
	 */
bool value::addvalue(const char *newname,value *newval){
	if (dt != datatype::_object) return false;
	if (newval->dt == datatype::_nullval) return false;
	size_t olen;
	achar *name = convert::toachar(newname,strlen(newname),olen);
	if (!name) return false;
	(*val._object)[name]=newval;
	return true;
}
#endif

	/*!
	 * \brief
	 * sets the child value this[\p newname] to a copy of  \p newval.
	 * 
	 * \returns
	 * \b true if the value was added.
	 * \b false if this is not a %JSON object datatype
	 *
	 * Any previous values are deleted
	 */
bool value::addvalue(const wchar_t *newname,const value &newval){
	if (dt != datatype::_object) return false;
	if (newval.dt == datatype::_nullval) return false;
	size_t olen;
	achar *name = convert::toachar(newname,wcslen(newname),olen);
	if (!name) return false;
	(*val._object)[name]=new value(newval);
	return true;
}

	/*!
	 * \brief
	 * sets the child value this[\p newname] to a copy of  \p newval.
	 * 
	 * \returns
	 * \b true if the value was added.
	 * \b false if this is not a %JSON object datatype
	 *
	 * Any previous values are deleted
	 */
bool value::addvalue(const char *newname,const value &newval){
	if (dt != datatype::_object) return false;
	if (newval.dt == datatype::_nullval) return false;
	size_t olen;
	achar *name = convert::toachar(newname,strlen(newname),olen);
	if (!name) return false;
	(*val._object)[name]=new value(newval);
	return true;
}
	
#pragma endregion

#pragma region Getting Values / Types

	/*!
	 * \brief
	 * returns the curent datatype::dt of this value
	 * 
	 * \returns
	 * \b one of the values in JSON::dataype::dt
	 *
	 */
datatype::dt value::getdatatype(){
	return dt;
}

	/*!
	 * \brief
	 * shallow searches for a child value whose name matches \p name.  \p this must be a %JSON object
	 *
	 * If no such child value is found, a new value is created with the specified \p name, and returned instead.
	 *
	 * \returns
	 * A reference to the child value.  WARNING, if \p this, is deleted, or \p this[\p name] is deleted, then the reference will become invalid.
	 *
	 * We do not do reference counting here.
	 *
	 */
value & value::getchild(const wchar_t *name){
	if (dt!=datatype::_object && dt!=datatype::_undefined) return nullval;
	if (dt == datatype::_undefined) setobject();

	size_t olen;
	achar *rname=convert::toachar(name,wcslen(name),olen);
	
	raw_object_t::iterator dstval=(val._object)->find((achar *const)rname);
	if (dstval==val._object->end()){
		value *nval = new value();
		val._object->insert(make_pair(rname,nval));
		return *nval;
	}else{
		delete [] rname;
		return (*(*dstval).second);
	}
	delete [] rname;
	return nullval;
}

	/*!
	 * \brief
	 * shallow searches for a child value whose name matches \p name.  \p this must be a %JSON object
	 * 
	 * If no such child value is found, a new value is created with the specified \p name, and returned instead.
	 *
	 * \returns
	 * A reference to the child value.  WARNING, if \p this, is deleted, or \p this[\p name] is deleted, then the reference will become invalid.
	 *
	 * We do not do reference counting here.
	 *
	 */
value & value::getchild(const char *name){
	if (dt!=datatype::_object) return nullval;
	size_t olen;
	achar *rname=convert::toachar(name,strlen(name),olen);

	raw_object_t::iterator dstval=(val._object)->find((achar *const)rname);
	if (dstval==val._object->end()){
		value *nval = new value();
		val._object->insert(make_pair(rname,nval));
		return *nval;
	}else{
		delete [] rname;
		return (*(*dstval).second);
	}
	delete [] rname;
	return nullval;
}

/*!
	 * \brief
	 * shallow searches for a child that \p aval points to.  \p this must be a %JSON array.
	 *
	 * TODO: add object searching as well!
	 * 
	 * \returns
	 * NULL if the child cannot be found.
	 *
	 * A pointer to the child if it is found.
	 *
	 */
value *value::childexists(value *aval){
	if (dt == datatype::_array){
		raw_array_t::iterator cur = val._array->begin();
		for (;cur != val._array->end(); ++ cur){
			if ((*cur) == aval) return aval;
		}
	}
	return false;
}

	/*!
	 * \brief
	 * shallow searches for a child with the name \p name.  \p this must be a %JSON object.
	 * 
	 * \returns
	 * NULL if the child cannot be found.
	 *
	 * A pointer to the child if it is found.
	 *
	 */
#ifdef JSON_USE_WCHAR
value *value::childexists(const char *name){
	size_t olen;
	achar *tmp = convert::toachar(name,strlen(name),olen);
#else
value *value::childexists(const wchar_t *name){
	size_t olen;
	achar *tmp = convert::toachar(name,wcslen(name),olen);
#endif
	return childexists(tmp);
	delete [] tmp;
}

	/*!
	 * \brief
	 * shallow searches for a child with the name \p name.  \p this must be a %JSON object.
	 * 
	 * \returns
	 * NULL if the child cannot be found.
	 *
	 * A pointer to the child if it is found.
	 *
	 */
value *value::childexists(const achar *name){
	if (dt!=datatype::_object) return 0;
		
	raw_object_t::iterator dstval=(val._object)->find((achar *const)name);
	if (dstval==val._object->end()){
		return 0;
	}else{
		return (*dstval).second;
	}
	return 0;
}

	/*!
	 * \brief
	 * tries to look up the value at index \p ofs in a %JSON array
	 * 
	 * \returns
	 * NULL if the index is out of bounds.
	 *
	 * A pointer to the child if it is found.
	 *
	 */
value *value::childexists(size_t ofs){
	if (dt!= datatype::_array) return 0;
	if (ofs >= val._array->size()){ return 0; }
	return (*(val._array))[ofs];
}

	/*!
	 * \brief
	 * returns a bool representing the current %value of \p this
	 * 
	 * \returns
	 * returns a bool representing the current %value of \p this
	 * 
	 * Reasonable attempts will be made to cast the current %value into a bool, using the following rules:
	 *
	 * type is a literal : true if the %value is true, otherwise false.
	 *
	 * type is an array or object : true if the array contains members > 0, otherwise false.
	 *
	 * type is a Number : true if the number != 0, otherwise false.
	 *
	 * type is a string: true if the length > 0, otherwise false.
	 *
	 * type is undefined : returns value of _default 
	 *
	 */
bool value::getbool(bool _default){
	switch (dt){
		case datatype::_array:
			if (val._array->size() > 0) return true;
			return false;
		
		case datatype::_fixed_number:
			if (val._fixed_number == 0) return false;
			return true;
		
		case datatype::_literal:
			if (val._literal == literals::_true) return true;
			return false;

		case datatype::_number:
			if ((*val._number) == 0)return false;
			return true;

		case datatype::_object:
			if (val._object->size() > 0) return true;
			return false;
		
		case datatype::_unquoted_string:
		case datatype::_string:
			if (val._string->length() == 0) return false;
			return true;

	}
	return _default;
}

	/*!
	 * \brief
	 * returns a literals::literals representing the current %value of \p this
	 * 
	 * \returns
	 * returns the current %value if the dataype is literal.  Otherwise returns \p _default
	 *
	 */
JSON::literals::literals value::getliteral(JSON::literals::literals _default){
	if (dt == datatype::_literal)
			return val._literal;
	return _default;
}

	/*!
	 * \brief
	 * returns an integer representing the current %value of \p this
	 * 
	 * \returns
	 * returns an integer representing the current %value of \p this.
	 * 
	 * If datatype is fixed_number (integer), then the %value is returned

	 * If datatype is number (float), then the return is cast to integer.

	 * Otherwise, the \p _default value is returned.
	 *
	 */

aint value::getinteger(aint _default){
	switch (dt){
		case datatype::_array:
		case datatype::_literal:
		case datatype::_object:
		case datatype::_unquoted_string:
		case datatype::_string:
		case datatype::_undefined:
			return _default;
		
		case datatype::_fixed_number:
			return val._fixed_number;

		case datatype::_number:
			return (aint)(*val._number);
	}
	return _default;
}

/*!
	 * \brief
	 * returns a float representing the current %value of \p this
	 * 
	 * \returns
	 * returns a float representing the current %value of \p this.
	 * 
	 * If datatype is fixed_number (integer), then the %value is returned

	 * If datatype is number (float), then the %value is returned.

	 * Otherwise, the \p _default value is returned.
	 *
	 */
afloat value::getfloat(afloat _default){
	switch (dt){
		case datatype::_fixed_number:
			return (afloat)val._fixed_number;
	
		case datatype::_number:
			return (*val._number);

	}
	return _default;
}


/*!
	 * \brief
	 * returns a string representing the current %value of \p this
	 * 
	 * \returns
	 * returns a string representing the current %value of \p this.
	 *
	 * if the value is an array, object, undefined or a nullval, then the default is returned
	 */
astr &value::getstring(astr &dest,achar *_default){
	switch(dt){
		case datatype::_array:
		case datatype::_object:
		case datatype::_undefined:
		case datatype::_nullval:
			return dest = _default;
		default:
			return getstring(dest,false,false);

	}

}

/*!
	 * \brief
	 * fills \p dest with the string representation of the current %value of \p this
	 *
	 * If datatype is fixed_number (integer), or number (float), then \p dst will contain a string representation of the number, similar to itoa or gcvt.

	 * If datatype is string, then \p dst will contain the string.

	 * If datatype is literal, then \p dst will contain one of: L"true",L"false" or L"null"
	 *
	 * If datatype is array or object, then \p dst will contain valid JSON, while each child added recursively. [output is only valid JSON if \p enquote is true]
	 *
	 * \param dest
	 * This will be filled with the return value
	 *
	 * \param enquote
	 * If this is true, then strings and object names will be properly quoted and escaped.  passing true here will guarantee valid JSON
	 *
	 * \param newline
	 * If \p this is an object or array, then each child value will be output on a separate line.
	 *
	 * \returns
	 * a reference to dest.
	 *
	 */
std::wstring &value::getstring(std::wstring &dest,bool enquote,bool newline){
	wchar_t strnum[32];
	astr tmpstr;
	switch(dt){
		case datatype::_unquoted_string:
		case datatype::_string:
			tmpstr = *val._string;
			if (enquote)quotestring(tmpstr);
			convert::towstr(dest,tmpstr);			
			break;
		case datatype::_fixed_number:
			#ifdef JSON_NUMBER_INT
				swprintf(strnum,31,L"%.1d",val._fixed_number);
			#endif
			#ifdef JSON_NUMBER_LONG
				swprintf(strnum,31,L"%.1ld",val._fixed_number);
			#endif
			#ifdef JSON_NUMBER_i64
				swprintf(strnum,31,L"%.1lld",val._fixed_number);
			#endif
			convert::towstr(dest,strnum);
			break;
		case datatype::_number:
			swprintf(strnum,31,L"%lG",(*val._number));
			convert::towstr(dest,strnum);
			break;
		case datatype::_literal:
			switch(val._literal){
				case literals::_false:dest=L"false";break;
				case literals::_true:dest=L"true";break;
				case literals::_null:dest=L"null";break;
				default: return dest;
			}
			break;
		case datatype::_array:
			arraygetstring(tmpstr,(*val._array),enquote,newline);
			convert::towstr(dest,tmpstr);
			break;
		case datatype::_object:
			objectgetstring(tmpstr,(*val._object),enquote,newline);
			convert::towstr(dest,tmpstr);
			break;
		case datatype::_undefined:
		case datatype::_nullval:  //I'm not sure about this.  maybe we shouldn't do it.
			dest=L"null";
			break;
	}
	return dest;
}


/* \brief
	 * fills \p dest with the string representation of the current %value of \p this
	 *
	 * If datatype is fixed_number (integer), or number (float), then \p dst will contain a string representation of the number, similar to itoa or gcvt.

	 * If datatype is string, then \p dst will contain the string.

	 * If datatype is literal, then \p dst will contain one of: L"true",L"false" or L"null"
	 *
	 * If datatype is array or object, then \p dst will contain valid JSON, while each child added recursively. [output is only valid JSON if \p enquote is true]
	 *
	 * \param dest
	 * This will be filled with the return value
	 *
	 * \param enquote
	 * If this is true, then strings and object names will be properly quoted and escaped.  passing true here will guarantee valid JSON
	 *
	 * \param newline
	 * If \p this is an object or array, then each child value will be output on a separate line.
	 *
	 * \returns
	 * a reference to dest.
	 *
	 */
std::string &value::getstring(std::string &dest,bool enquote,bool newline){
	wchar_t strnum[48];
	astr tmpstr;
	switch(dt){
		case datatype::_unquoted_string:
		case datatype::_string:
			tmpstr = *val._string;
			if (enquote)quotestring(tmpstr);
			convert::tostr(dest,tmpstr);			
			break;
		case datatype::_fixed_number:
			#ifdef JSON_NUMBER_INT
				swprintf(strnum,31,L"%.1d",val._fixed_number);
			#endif
			#ifdef JSON_NUMBER_LONG
				swprintf(strnum,31,L"%.1ld",val._fixed_number);
			#endif
			#ifdef JSON_NUMBER_i64
				swprintf(strnum,31,L"%.1lld",val._fixed_number);
			#endif

			convert::tostr(dest,strnum);
			break;
		case datatype::_number:
			swprintf(strnum,31,L"%lG",(*val._number));
			convert::tostr(dest,strnum);
			break;
		case datatype::_literal:
			switch(val._literal){
				case literals::_false:dest="false";break;
				case literals::_true:dest="true";break;
				case literals::_null:dest="null";break;
				default: return dest;
			}
			break;
		case datatype::_array:
			arraygetstring(tmpstr,(*val._array),enquote,newline);
			convert::tostr(dest,tmpstr);
			break;
		case datatype::_object:
			objectgetstring(tmpstr,(*val._object),enquote,newline);
			convert::tostr(dest,tmpstr);
			break;
		case datatype::_nullval:  //I'm not sure about this.  maybe we shouldn't do it.
		case datatype::_undefined:
			dest="null";
			break;
	}
	return dest;
}

/* \brief
	 * Gets a refrence to the std::[w]string used internally to store the string. [only when datatype == _string]
	 *
	 * \returns
	 * a reference to the std::[w]string used to store the %JSON string.
	 * WARNING, this method only succeeds if the datatype of \p this is a string.
	 * 
	 * An exception will be thrown if \p this is the wrong datatype
	 */
astr &value::getrawstring(){
	if (dt != datatype::_string && dt != datatype::_unquoted_string) throw "Wrong Datatype";
	return *val._string;
}

	/* \brief
	 * Gets a refrence to the raw_array_t used internally to store the array. [only when datatype == _array]
	 *
	 * \returns
	 * a reference to the raw_array_t used to store the %JSON array.
	 * WARNING, this method only succeeds if the datatype of \p this is an array.
	 * 
	 * An exception will be thrown if \p this is the wrong datatype
	 */
raw_array_t &value::getrawarray(){
	if (dt != datatype::_array) throw "Wrong Datatype";
	return *val._array;
}

/* \brief
	 * Gets a refrence to the raw_object_t used internally to store the object. [only when datatype == _object]
	 *
	 * \returns
	 * a reference to the raw_object_t used to store the %JSON object.
	 * WARNING, this method only succeeds if the datatype of \p this is a object.
	 * 
	 * An exception will be thrown if \p this is the wrong datatype
	 */
raw_object_t &value::getrawobject(){
	if (dt != datatype::_object) throw "Wrong Datatype";
	return *val._object;
}


#pragma endregion 

#pragma region overloaded [] functions

	/*!
	 * \brief
	 * returns a reference to the array child at index \p index.
	 * 
	 * If \p this[\p index] doesn't exist, then a new value is created and returned.
	 *
	 * If datatype is not array (or undefined), then a reference to JSON::value::nullval is returned. 
	 * if dataype is 'undefined', then the value is converted into an array.
	 * To detect this, compare a pointer the returned reference to a pointer to value::nullval
	 *
	 * \returns
	 * A reference to the \p index index of the %JSON array.  
	 * Or a reference to value::nullval if the dataype is wrong.
	 *
	 */
value &value::operator[](aint index){
	if (this->dt != datatype::_array && this->dt != datatype::_undefined) return nullval;
	if (this->dt == datatype::_undefined) setarray();

	if (index >= (*this->val._array).size()){
		size_t diff = index - (*this->val._array).size() + 1;
		(*this->val._array).reserve(index+1);
		for (size_t i=0; i<diff; ++i){
			(*this->val._array).push_back(new value(datatype::_undefined));
		}
	}	
	
	return *((*this->val._array)[index]);

}

	/*!
	 * \brief
	 * returns a reference to the object child with name \p name
	 * 
	 * If \p this[\p name] doesn't exist, then a new value is created and returned.
	 *
	 * If datatype is not array, then a reference to JSON::value::nullval is returned. 
	 * To detect this, compare a pointer the returned reference to a pointer to value::nullval
	 *
	 * \returns
	 * A reference to the \p name index of the %JSON object.  
	 * Or a reference to value::nullval if the dataype is wrong.
	 *
	 */
value &value::operator[](const wchar_t *name){
	return getchild(name);
}

/*!
	 * \brief
	 * returns a reference to the object child with name \p name
	 * 
	 * If \p this[\p name] doesn't exist, then a new value is created and returned.
	 *
	 * If datatype is not array, then a reference to JSON::value::nullval is returned. 
	 * To detect this, compare a pointer the returned reference to a pointer to value::nullval
	 *
	 * \returns
	 * A reference to the \p name index of the %JSON object.  
	 * Or a reference to value::nullval if the dataype is wrong.
	 *
	 */
value &value::operator[](const char *name){
	return getchild(name);
}

/*!
	 * \brief
	 * wrapper function for value::addvalue(const value)
	 */
value &value::operator+=(const value &nval){
	if (dt != datatype::_array) return *this;

	addvalue(nval);
	return *this;
}

#pragma endregion
