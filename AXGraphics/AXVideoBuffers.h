/* --------------------------------------------------------------
	File:			AXResources.h
	Description:	Represents vertex and index buffers.

	Date:			May 23, 2005
	Author:			James Long
-------------------------------------------------------------- */

#if !defined(AXVIDEOBUFFERS_INCLUDED)
#define AXVIDEOBUFFERS_INCLUDED

#include "AXElement.h"

class AXMeshChunk;


struct AXVertexBuffer {
	unsigned int PoolID;
	unsigned long Offset;
	unsigned long NumFilled;
	unsigned long Capacity;

	DataDescription BufferDesc;
	unsigned int Stride;
	void *Buffer;

	AXMeshChunk *Owner;
};

struct AXIndexBuffer {
	unsigned int PoolID;
	unsigned long Offset;
	unsigned long NumFilled;
	unsigned long Capacity;

	AXDataFormat IndexFormat;
	void *Buffer;

	AXMeshChunk *Owner;
};


#endif