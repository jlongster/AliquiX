/* --------------------------------------------------------------
	File:			AXColor.h
	Description:	This file contains macros and inline functions that abstract dealing with RGB and RGBA colors.
					It supports unsigned char compaction as well as floating point colors.
	Date:			June 29, 2005
	Author:			James Long
-------------------------------------------------------------- */


#if !defined(AXCOLOR_INCLUDED)
#define AXCOLOR_INCLUDED

typedef unsigned int AXColorRGBi;
typedef unsigned int AXColorRGBAi;
typedef unsigned int AXColorARGBi;

struct AXColorRGBf {
	AXColorRGBf() : r(0), g(0), b(0) {}
	AXColorRGBf(float fr, float fg, float fb) : r(fr), g(fg), b(fb) {}
	float r;
	float g;
	float b;
};
struct AXColorARGBf {
	AXColorARGBf() : a(0), r(0), g(0), b(0) {}
	AXColorARGBf(float fa, float fr, float fg, float fb) : a(fa), r(fr), g(fg), b(fb) {}
	float a;
	float r;
	float g;
	float b;
};

typedef AXColorRGBi AXColorRGB;
typedef AXColorRGBAi AXColorRGBA;
typedef AXColorARGBi AXColorARGB;
typedef AXColorARGB AXColor;

inline AXColorRGB AXColorToRGB(int r, int g, int b) {
	return (((unsigned char)r << 16) | ((unsigned char)g << 8) | (unsigned char)b);
}
inline AXColorRGBA AXColorToRGBA(int r, int g, int b, int a) {
	return (((unsigned char)r << 24) | ((unsigned char)g << 16) | ((unsigned char)b << 8) | (unsigned char)a);
}
inline AXColorARGB AXColorToARGB(int a, int r, int g, int b) {
	return (((unsigned char)a << 24) | ((unsigned char)r << 16) | ((unsigned char)g << 8) | (unsigned char)b);
}
inline AXColorRGBA AXColorToRGBA(AXColorARGB Color) {
	return ((Color&0xFF000000)>>24 | (Color&0xFF)<<24);
}
inline AXColorARGB AXColorToARGB(AXColorRGBA Color) {
	return ((Color&0xFF000000)>>24 | (Color&0xFF)<<24);
}
inline AXColorRGBf AXColorToRGB(float r, float g, float b) {
	return AXColorRGBf(r,g,b);
}
inline AXColorARGBf AXColorToARGB(float a, float r, float g, float b) {
	return AXColorARGBf(a,r,g,b);
}

inline unsigned char AXColorA(AXColorRGBAi Color) {
	return (unsigned char)((Color & 0xff000000) >> 24);
}
inline unsigned char AXColorR(AXColorRGBi Color) {
	return (unsigned char)((Color & 0x00ff0000) >> 16);
}
inline unsigned char AXColorG(AXColorRGBi Color) {
	return (unsigned char)((Color & 0x0000ff00) >> 8);
}
inline unsigned char AXColorB(AXColorRGBi Color) {
	return (unsigned char)(Color & 0x000000ff);
}

inline float AXColorR(AXColorRGBf Color) {
	return Color.r;
}
inline float AXColorG(AXColorRGBf Color) {
	return Color.g;
}
inline float AXColorB(AXColorRGBf Color) {
	return Color.b;
}
inline float AXColorR(AXColorARGBf Color) {
	return Color.r;
}
inline float AXColorG(AXColorARGBf Color) {
	return Color.g;
}
inline float AXColorB(AXColorARGBf Color) {
	return Color.b;
}
inline float AXColorA(AXColorARGBf Color) {
	return Color.a;
}

inline AXColorRGBi AXColorFloatToInt(AXColorRGBf &Color) {
	return AXColorToRGB((unsigned char)(Color.r*255), (unsigned char)(Color.g*255), (unsigned char)(Color.b*255));
}
inline AXColorARGBi AXColorFloatToInt(AXColorARGBf &Color) {
	return AXColorToARGB((unsigned char)(Color.a*255), (unsigned char)(Color.r*255), (unsigned char)(Color.g*255), (unsigned char)(Color.b*255));
}

inline AXColorARGBf AXColorIntToFloat(AXColorARGB &Color) {
	return AXColorToARGB((float)((Color & 0xff000000) >> 24), (float)((Color & 0x00ff0000) >> 16), (float)((Color & 0x0000ff00) >> 8), (float)(Color & 0x000000ff));
}


#endif
