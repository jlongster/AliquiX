/* --------------------------------------------------------------
	File:			AXElement.h
	Description:	An element is a part of the vertex data that
					represents one property of the vertex.

	Date:			May 23, 2006
	Author:			James Long
-------------------------------------------------------------- */

#if !defined(AXELEMENT_INCLUDED)
#define AXELEMENT_INCLUDED

#include <vector>

typedef std::vector<float> FloatData;
typedef std::vector<unsigned int> UIntData;
typedef std::vector<unsigned short> UShortData;
typedef std::vector<unsigned char> UByteData;

#define DataAdd1(D,x) D.push_back(x);
#define DataAdd2(D,x,y) D.push_back(x); D.push_back(y);
#define DataAdd3(D,x,y,z) D.push_back(x); D.push_back(y); D.push_back(z);
#define DataAdd4(D,x,y,z,w) D.push_back(x); D.push_back(y); D.push_back(z); D.push_back(w);

enum AXDataType {
	AX_ELEM_POSITION = 0x1,
	AX_ELEM_DIFFUSE = 0x2,
	AX_ELEM_NORMAL = 0x4,
	AX_ELEM_TEXCOORD0 = 0x8,
	AX_ELEM_TEXCOORD1 = 0x10,
	AX_ELEM_TEXCOORD2 = 0x20,
	AX_ELEM_TEXCOORD3 = 0x40,
	AX_ELEM_TEXCOORD4 = 0x80,
	AX_ELEM_TEXCOORD5 = 0x100,
	AX_ELEM_TEXCOORD6 = 0x200,
	AX_ELEM_TEXCOORD7 = 0x400,
};

enum AXDataFormat {
	AX_FORMAT_FLOAT1 = 1,
	AX_FORMAT_FLOAT2 = 2,
	AX_FORMAT_FLOAT3 = 3,
	AX_FORMAT_FLOAT4 = 4,
	AX_FORMAT_UINT1 = 5,
	AX_FORMAT_SHORT1 = 6,
	AX_FORMAT_USHORT1 = 7,
	AX_FORMAT_SHORT2 = 8,
	AX_FORMAT_SHORT4 = 9,
	AX_FORMAT_UBYTE4 = 10,
};

struct AXElementDescription {
	AXDataType Type;
	AXDataFormat Format;
	unsigned int OffsetInBytes;
};

typedef std::vector<AXElementDescription> DataDescription;


const char* TypeToString(AXDataType Type);
const char* FormatToString(AXDataFormat Format);
unsigned int GetElementSizeInBytes(AXDataFormat Format);
unsigned int GetElementCount(AXDataFormat Format);

#endif
