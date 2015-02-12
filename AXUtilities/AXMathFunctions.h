/* --------------------------------------------------------------
	File:			AXMathFunctions.h
	Descriptions:	Defines a few mathematical functions.
	Date:			July 1, 2005
	Author:			James Long
-------------------------------------------------------------- */

#if !defined(AXMATHFUNCTIONS_INCLUDED)
#define AXMATHFUNCTIONS_INCLUDED

#include "math.h"

#define AX_PI (3.141592654f)

template<class T>
inline T AXClamp(T x, T lower, T higher) {
	if(x < lower) return lower;
	else if(x > higher) return higher;
	else return x;
}

template<class T>
inline T AXLerp(T x, T lower, T higher) {
	return (higher-lower)*x + lower;
}

inline float AXDegreesToRadians(float Degrees) {
	return (Degrees/180.0f)*AX_PI;
}
inline float AXRadiansToDegrees(float Radians) {
	return (Radians/AX_PI)*180.0f;
}

inline unsigned int NearestPow2Index(const unsigned int x) {
	return (unsigned int)ceil(log(double(x)) / log(2.0));
}

#endif