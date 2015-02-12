/* --------------------------------------------------------------
	File:			AXElement.cpp
	Description:	See AXElement.h.

	Date:			May 23, 2006
	Author:			James Long
-------------------------------------------------------------- */

#include "AXElement.h"

unsigned int GetElementSizeInBytes(AXDataFormat Format) {
	if(Format == AX_FORMAT_FLOAT1) return sizeof(float);
	else if(Format == AX_FORMAT_FLOAT2) return sizeof(float)*2;
	else if(Format == AX_FORMAT_FLOAT3) return sizeof(float)*3;
	else if(Format == AX_FORMAT_FLOAT4) return sizeof(float)*4;
	else if(Format == AX_FORMAT_UINT1) return sizeof(unsigned int);
	else if(Format == AX_FORMAT_SHORT1) return sizeof(short);
	else if(Format == AX_FORMAT_USHORT1) return sizeof(unsigned short);
	else if(Format == AX_FORMAT_SHORT2) return sizeof(short)*2;
	else if(Format == AX_FORMAT_SHORT4) return sizeof(short)*4;
	else if(Format == AX_FORMAT_UBYTE4) return sizeof(unsigned char)*4;
	return 0;
}

unsigned int GetElementCount(AXDataFormat Format) {
	if(Format == AX_FORMAT_FLOAT1) return 1;
	else if(Format == AX_FORMAT_FLOAT2) return 2;
	else if(Format == AX_FORMAT_FLOAT3) return 3;
	else if(Format == AX_FORMAT_FLOAT4) return 4;
	else if(Format == AX_FORMAT_UINT1) return 1;
	else if(Format == AX_FORMAT_SHORT1) return 1;
	else if(Format == AX_FORMAT_USHORT1) return 1;
	else if(Format == AX_FORMAT_SHORT2) return 2;
	else if(Format == AX_FORMAT_SHORT4) return 4;
	else if(Format == AX_FORMAT_UBYTE4) return 4;
	return 0;
}

const char* FormatToString(AXDataFormat Format) {
	if(Format == AX_FORMAT_FLOAT1) return "float1";
	else if(Format == AX_FORMAT_FLOAT2) return "float2";
	else if(Format == AX_FORMAT_FLOAT3) return "float3";
	else if(Format == AX_FORMAT_FLOAT4) return "float4";
	else if(Format == AX_FORMAT_UINT1) return "uint1";
	else if(Format == AX_FORMAT_SHORT1) return "short1";
	else if(Format == AX_FORMAT_USHORT1) return "ushort1";
	else if(Format == AX_FORMAT_SHORT2) return "short2";
	else if(Format == AX_FORMAT_SHORT4) return "short4";
	else if(Format == AX_FORMAT_UBYTE4) return "ubyte4";
	return "default";
}

const char* TypeToString(AXDataType Type) {
	if(Type == AX_ELEM_POSITION) return "position";
	else if(Type == AX_ELEM_DIFFUSE) return "diffuse";
	else if(Type == AX_ELEM_NORMAL) return "normal";
	else if(Type == AX_ELEM_TEXCOORD0) return "texcoord0";
	else if(Type == AX_ELEM_TEXCOORD1) return "texcoord1";
	else if(Type == AX_ELEM_TEXCOORD2) return "texcoord2";
	else if(Type == AX_ELEM_TEXCOORD3) return "texcoord3";
	else if(Type == AX_ELEM_TEXCOORD4) return "texcoord4";
	else if(Type == AX_ELEM_TEXCOORD5) return "texcoord5";
	else if(Type == AX_ELEM_TEXCOORD6) return "texcoord6";
	else if(Type == AX_ELEM_TEXCOORD7) return "texcoord7";
	return "default";
}

