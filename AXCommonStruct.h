/* --------------------------------------------------------------
	File:			AXCommonStruct.h
	Description:	This file defines some structs that may be used frequently throughout the app.

	Date:			December 31, 2005
	Author:			James Long
------------------------------------------------------------- */

#if !defined(AXCOMMONSTRUCT_INCLUDED)
#define AXCOMMONSTRUCT_INCLUDED

struct AXDimension2D {
	AXDimension2D() : x(0), y(0) {}
	AXDimension2D(int X, int Y) : x(X), y(Y) {}
	int x;
	int y;
};

struct AXDimension3D {
	AXDimension3D() : x(0), y(0), z(0) {}
	AXDimension3D(int X, int Y, int Z) : x(X), y(Y), z(Z) {}
	int x;
	int y;
	int z;
};

/*	Structure defining the fullscreen modes */
struct AXFullscreenMode {
	AXDimension2D Resolution;
	unsigned int RefreshRate;
};

#endif
