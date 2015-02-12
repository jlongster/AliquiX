/* --------------------------------------------------------------
	File:			AXStringFunctions.h
	Descriptions:	Defines a few functions that help with string parsing/analyzation.
	Date:			July 1, 2005
	Author:			James Long
-------------------------------------------------------------- */

#if !defined(AXSTRINGFUNCTIONS_INCLUDED)
#define AXSTRINGFUNCTIONS_INCLUDED

#include <string>
#include "boost\\any.hpp"

unsigned char AXStringIsInt(const std::string &s);
unsigned char AXStringIsFloat(const std::string &s);

int AXStringConvertToInt(const std::string &s);
float AXStringConvertToFloat(const std::string &s);
boost::any AXStringConvertToAny(const std::string &s);

std::string AXStringConvertFromInt(const int &n);
std::string AXStringConvertFromFloat(const float &n);

std::string AXStringConvertFromAny(const boost::any &value);



#endif