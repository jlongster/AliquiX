/* --------------------------------------------------------------
	File:			AXStringFunctions.cpp
	Description:	Implementation for the AXStringFunctions class.  See AXStringFunctions.h for details.
	Date:			July 1, 2005
	Author:			James Long
-------------------------------------------------------------- */

#include "AXStringFunctions.h"

#include "..\\AXCommon.h"
#include "boost\\lexical_cast.hpp"

using std::string;

unsigned char AXStringIsInt(const string &s) {
	try {
		boost::lexical_cast<int>(s);
		return 1;
	}
	catch(boost::bad_lexical_cast &) {
		return 0;
	}
}

unsigned char AXStringIsFloat(const string &s) {
	try {
		boost::lexical_cast<float>(s);
		return 1;
	}
	catch(boost::bad_lexical_cast &) {
		return 0;
	}
}

int AXStringConvertToInt(const string &s) {
	try {
		int temp = boost::lexical_cast<int>(s);
		return temp;
	}
	catch(boost::bad_lexical_cast &) {
		return ERRORINT;
	}
}

float AXStringConvertToFloat(const string &s) {
	try {
		float temp = boost::lexical_cast<float>(s);
		return temp;
	}
	catch(boost::bad_lexical_cast &) {
		return ERRORFLOAT;
	}
}

boost::any AXStringConvertToAny(const string &s) {
	int Int_Value;
	float Float_Value;

	if(	(Int_Value = AXStringConvertToInt(s)) != ERRORINT )
		return static_cast<boost::any>(Int_Value);
	else if( (Float_Value = AXStringConvertToFloat(s)) != ERRORFLOAT )
		return static_cast<boost::any>(Float_Value);
	else
		return static_cast<boost::any>(s);
}

string AXStringConvertFromInt(const int &n) {
	return boost::lexical_cast<string>(n);
}

string AXStringConvertFromFloat(const float &n) {
	return boost::lexical_cast<string>(n);
}

string AXStringConvertFromAny(const boost::any &value) {
	try {
		int Int_Value = boost::any_cast<int>(value);
		return boost::lexical_cast<string>(Int_Value);
	}
	catch(boost::bad_any_cast &) {
	}

	try {
		float Float_Value = boost::any_cast<float>(value);
		return boost::lexical_cast<string>(Float_Value);
	}
	catch(boost::bad_any_cast &) {
	}

	try {
		string String_Value = boost::any_cast<string>(value);
		return String_Value;
	}
	catch(boost::bad_any_cast &) {
	}

	return "";
}

