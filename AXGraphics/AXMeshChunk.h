/* --------------------------------------------------------------
	File:			AXMeshChunk.h
	Description:	A discrete chunk of a mesh.  This may be instanced
					by using AXMeshChunkEntity.

	Date:			January 5, 2006
	Author:			James Long
-------------------------------------------------------------- */

#if !defined(AXMESHCHUNK_INCLUDED)
#define AXMESHCHUNK_INCLUDED

#include "..\\AXCommon.h"
#include "AXMaterial.h"
#include "AXElement.h"
#include "AXVideoBuffers.h"
#include <vector>
#include <string>

//	AXMesh depends on AXMeshChunk too
class AXMesh;

enum AXPrimitiveType {
	AX_PRIM_TRIANGLES = 1,
	AX_PRIM_QUADS = 2,
	AX_PRIM_LINES = 3,
	AX_PRIM_TRIANGLESTRIP = 4,
	AX_PRIM_QUADSTRIP = 5,
	AX_PRIM_LINESTRIP = 6
};

class AXMeshChunk {
	friend AXMesh;

public:
	AXMeshChunk();
	AXMeshChunk(AXPrimitiveType PrimType);
	virtual ~AXMeshChunk();

	AXMesh* GetOwner() { return _Owner; }

	void AssignDefaultEffect(unsigned int EffectID) { _EffectID = EffectID; }
	unsigned int GetAssignedEffect() { return _EffectID; }

	AXPrimitiveType GetPrimitiveType() { return _Type; }

	void SetNumVertices(unsigned int NumVertices) { _NumVertices = NumVertices; }
	unsigned int GetNumVertices() { return _NumVertices; }

	void SetNumIndices(unsigned int NumIndices) { _NumIndices = NumIndices; }
	unsigned int GetNumIndices() { return _NumIndices; }

	void SetDataSources();
	void Draw();


	/*	Dealing with data */

	void AttachVertexData(void* Data, AXDataType Type, AXDataFormat Format);
	void AttachInterleavedVertexData(void *Data, std::vector<AXElementDescription> DataDesc);

	template<typename T>
	void AttachVertexData(std::vector<T> Data, AXDataType Type, AXDataFormat Format) {
		unsigned int NumElements = GetElementCount(Format);
		unsigned int NumData = (unsigned int)Data.size();
		unsigned int NumVertices = NumData/NumElements;

		if(_NumVertices == 0) SetNumVertices(NumVertices);
		else if(_NumVertices != NumVertices) return;

		AttachVertexData((void*)(&Data[0]), Type, Format);
	}

	void Interleave();
	unsigned char IsInterleaved() { if(_Stride) { return 1; } else { return 0; } }

	void AttachIndexData(void* Data, AXDataFormat Format);
	
	template<typename T>
	void AttachIndexData(std::vector<T> Data, AXDataFormat Format) {
		unsigned int NumElements = GetElementCount(Format);
		unsigned int NumData = (unsigned int)Data.size();
		unsigned int NumIndices = NumData/NumElements;

		if(_NumIndices == 0) SetNumIndices(NumIndices);
		else if(_NumIndices != NumIndices) return;

		AttachIndexData((void*)(&Data[0]), Format);
	}

	unsigned char HasVertexData(unsigned int TypeFlags);
	unsigned int GetVertexSize();
	unsigned int GetVertexSize(unsigned int TypeFlags);
	unsigned int GetIndexSize();
	AXResult GetVertexData(void *Buffer);
	AXResult GetVertexData(unsigned int TypeFlags, unsigned int VertexSize, void *Buffer);
	AXResult GetIndexData(unsigned int IndexSize, void *Buffer);

	AXResult UploadVertexData(unsigned int TypeFlags);
	AXResult UploadIndexData();

	void FreeVertexData();
	void FreeUploadedVertexData();
	void FreeIndexData();
	void FreeUploadedIndexData();

	/*	TODO: add ability to modify mesh */

	AXMaterial Attributes;

	void Debug();

private:

	unsigned int _EffectID;

	AXMesh *_Owner;

	AXPrimitiveType _Type;

	/*	Vertex data */
	void *_VertexData;
	unsigned int _NumVertices;
	unsigned int _Stride;
	unsigned int _VertexDataSizeInBytes;
	unsigned int _VertexSizeInBytes;
	DataDescription _VertexDescription;

	/*	Index data */
	void *_IndexData;
	unsigned int _NumIndices;
	unsigned int _IndexDataSizeInBytes;
	unsigned int _IndexSizeInBytes;
	AXDataFormat _IndexFormat;

	/*	Video memory info */
	unsigned int _UploadedTypeFlags;
	unsigned int _IndexFlag;
	AXVertexBuffer *_VertexBuffer;
	AXIndexBuffer *_IndexBuffer;

	/* For debugging */
	void _DebugPrint(void *Data, AXDataFormat Format);
};

#endif


