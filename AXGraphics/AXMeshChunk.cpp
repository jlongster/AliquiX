/* --------------------------------------------------------------
	File:			AXMeshChunk.cpp
	Description:	See AXMeshChunk.h

	Date:			January 5, 2006
	Author:			James Long
-------------------------------------------------------------- */

#include "AXCore.h"
#include "AXGraphics.h"
#include "AXMeshChunk.h"

AXMeshChunk::AXMeshChunk() : _Owner(NULL), _Type(AX_PRIM_TRIANGLES), _EffectID(0),
								_NumVertices(0), _NumIndices(0), _VertexData(NULL), _Stride(0),
								_VertexSizeInBytes(0), _VertexDataSizeInBytes(0), _UploadedTypeFlags(0),
								_IndexFlag(0), _IndexData(NULL), _IndexDataSizeInBytes(0), _IndexSizeInBytes(0),
								_VertexBuffer(NULL), _IndexBuffer(NULL) {
}

AXMeshChunk::AXMeshChunk(AXPrimitiveType PrimType) : _Owner(NULL), _Type(PrimType), _EffectID(0),
								_NumVertices(0), _NumIndices(0), _VertexData(NULL), _Stride(0),
								_VertexSizeInBytes(0), _VertexDataSizeInBytes(0), _UploadedTypeFlags(0),
								_IndexFlag(0), _IndexData(NULL), _IndexDataSizeInBytes(0), _IndexSizeInBytes(0),
								_VertexBuffer(NULL), _IndexBuffer(NULL) {
}

AXMeshChunk::~AXMeshChunk() {
	FreeUploadedVertexData();
	FreeUploadedIndexData();
	FreeVertexData();
	FreeIndexData();
}

void AXMeshChunk::AttachVertexData(void* Data, AXDataType Type, AXDataFormat Format) {
	if(!Data) return;

	if(_NumVertices == 0) {
		LOG_SYS("Failed adding vertex stream: must set number of vertices first!");
		return;
	}

	for(unsigned int i=0; i<(unsigned int)_VertexDescription.size(); i++) {
		if(_VertexDescription[i].Type == Type) {
			LOG_SYS1("Failed adding vertex stream: type '%' already exists!", TypeToString(Type));
			return;
		}
	}

	AXElementDescription NewType;
	NewType.Type = Type;
	NewType.Format = Format;

	unsigned int ElementSize = GetElementSizeInBytes(Format);
	unsigned int SizeOfStream = _NumVertices*ElementSize;

	if(_Stride) {

		NewType.OffsetInBytes = _VertexSizeInBytes;
		_Stride = _VertexSizeInBytes+ElementSize;

		/*	Copy interleaved data out */
		unsigned char *DataBuffer = new unsigned char[_VertexDataSizeInBytes+SizeOfStream];

		// what's the best way to do this ? hmmm...
		for(unsigned int i=0; i<_NumVertices; i++) {
			memcpy((void*)(DataBuffer+i*(_VertexSizeInBytes+ElementSize)), _VertexData, _VertexSizeInBytes);
			memcpy((void*)(DataBuffer+i*(_VertexSizeInBytes+ElementSize)+_VertexSizeInBytes), Data, ElementSize);
		}

		SafeDeleteArr(_VertexData);
		_VertexData = (void*)DataBuffer;
	}
	else {
		NewType.OffsetInBytes = _VertexDataSizeInBytes;

		/*	Copy non-interleaved data out */
		unsigned char *DataBuffer = new unsigned char[_VertexDataSizeInBytes+SizeOfStream];
		memcpy((void*)DataBuffer, _VertexData, _VertexDataSizeInBytes);
		memcpy((void*)(DataBuffer+_VertexDataSizeInBytes), Data, SizeOfStream);
		SafeDeleteArr(_VertexData);
		_VertexData = (void*)DataBuffer;
	}

	_VertexDataSizeInBytes += SizeOfStream;
	_VertexSizeInBytes += ElementSize;
	_VertexDescription.push_back(NewType);
}
void AXMeshChunk::AttachInterleavedVertexData(void *Data, std::vector<AXElementDescription> DataDesc) {
	if(!Data) return;
	
	if(!_VertexDescription.empty()) {
		if(!_Stride) {
			LOG_SYS("Failed adding vertex stream: cannot add interleaved data to non-interleaved data!");
			return;
		}
	}

	if(_NumVertices == 0) {
		LOG_SYS("Failed adding vertex stream: must set number of vertices first!");
		return;
	}

	for(unsigned int i=0; i<(unsigned int)DataDesc.size(); i++) {
		for(unsigned int i=0; i<(unsigned int)_VertexDescription.size(); i++) {
			if(_VertexDescription[i].Type == DataDesc[i].Type) {
				LOG_SYS1("Failed adding interleaved vertex stream: it contains type '%' which already exists!", TypeToString(DataDesc[i].Type));
				return;
			}
		}
	}

	unsigned int NewElementsSize = 0;
	for(unsigned int i=0; i<(unsigned int)DataDesc.size(); i++) {
		NewElementsSize += GetElementSizeInBytes(DataDesc[i].Format);
		DataDesc[i].OffsetInBytes += _VertexSizeInBytes;
		_VertexDescription.push_back(DataDesc[i]);
	}

	/*	Copy interleaved data out */
	unsigned int SizeOfStream = NewElementsSize*_NumVertices;
	unsigned char *DataBuffer = new unsigned char[_VertexDataSizeInBytes+SizeOfStream];

	// what's the best way to do this ? hmmm...
	for(unsigned int i=0; i<_NumVertices; i++) {
		memcpy((void*)(DataBuffer+i*(_VertexSizeInBytes+NewElementsSize)), _VertexData, _VertexSizeInBytes);
		memcpy((void*)(DataBuffer+i*(_VertexSizeInBytes+NewElementsSize)+_VertexSizeInBytes), Data, NewElementsSize);
	}

	SafeDeleteArr(_VertexData);
	_VertexData = (void*)DataBuffer;

	_Stride += NewElementsSize;
	_VertexDataSizeInBytes += SizeOfStream;
	_VertexSizeInBytes += NewElementsSize;
}

void AXMeshChunk::Interleave() {
	if(_Stride) return;
	if(_VertexDataSizeInBytes == 0) return;

	unsigned int ByteIndex = 0;
	unsigned int Stride =  _VertexSizeInBytes;
	unsigned char *DataBuffer = new unsigned char[_VertexDataSizeInBytes];

	for(unsigned int i=0; i<(unsigned int)_VertexDescription.size(); i++) {

		unsigned int ElementSize = GetElementSizeInBytes(_VertexDescription[i].Format);
		//unsigned char *Dest = DataBuffer+ByteIndex;
		//unsigned char *Source = (unsigned char*)_VertexData+_VertexDescription[i].OffsetInBytes;

		for(unsigned int j=0; j<_NumVertices; j++) {
			unsigned int ElemOffset = _VertexDescription[i].OffsetInBytes;
			memcpy((void*)(DataBuffer+ByteIndex+j*Stride), (void*)((unsigned char*)_VertexData+ElemOffset+j*ElementSize), ElementSize);
		}

		_VertexDescription[i].OffsetInBytes = ByteIndex;

		ByteIndex += ElementSize;
	}

	SafeDeleteArr(_VertexData);
	_VertexData = DataBuffer;
	_Stride = _VertexSizeInBytes;
}

void AXMeshChunk::AttachIndexData(void* Data, AXDataFormat Format) {
	if(!Data) return;

	if(_NumIndices == 0) {
		LOG_SYS("Failed adding indices: must set number of indices first!");
		return;
	}

	if(_IndexData) {
		LOG_SYS("Failed adding indices: index data already exists!  must free first");
		return;
	}

	unsigned int ElementSize = GetElementSizeInBytes(Format);
	unsigned int SizeOfData = _NumIndices*ElementSize;

	/*	Copy data out */
	unsigned char *DataBuffer = new unsigned char[SizeOfData];
	memcpy((void*)DataBuffer, Data, SizeOfData);
	_IndexData = (void*)DataBuffer;

	_IndexDataSizeInBytes = SizeOfData;
	_IndexSizeInBytes = ElementSize;
	_IndexFormat = Format;
}

unsigned char AXMeshChunk::HasVertexData(unsigned int TypeFlags) {
	for(unsigned int i=0; i<(unsigned int)_VertexDescription.size(); i++) {
		TypeFlags &= ~_VertexDescription[i].Type;
		if(!TypeFlags) return 1;
	}

	if(TypeFlags) return 0;
	return 1;
}

unsigned int AXMeshChunk::GetVertexSize() {
	unsigned int VertexSize = 0;
	for(unsigned int i=0; i<(unsigned int)_VertexDescription.size(); i++) {
		VertexSize += GetElementSizeInBytes(_VertexDescription[i].Format);
	}
	return VertexSize;
} 

unsigned int AXMeshChunk::GetVertexSize(unsigned int TypeFlags) {
	unsigned int VertexSize = 0;
	for(unsigned int i=0; i<(unsigned int)_VertexDescription.size(); i++) {
		if(_VertexDescription[i].Type & TypeFlags)
			VertexSize += GetElementSizeInBytes(_VertexDescription[i].Format);
	}
	return VertexSize;
}

unsigned int AXMeshChunk::GetIndexSize() {
	return GetElementSizeInBytes(_IndexFormat);
}

AXResult AXMeshChunk::GetVertexData(void *Buffer) {
	unsigned int Types = 0;
	unsigned int VertexSize = 0;
	for(unsigned int i=0; i<(unsigned int)_VertexDescription.size(); i++) {
		Types |= _VertexDescription[i].Type;
		VertexSize += GetElementSizeInBytes(_VertexDescription[i].Format);
	}
	return GetVertexData(Types, VertexSize, Buffer);
}

AXResult AXMeshChunk::GetVertexData(unsigned int TypeFlags, unsigned int VertexSize, void *Buffer) {
	if(VertexSize == 0)
		VertexSize = GetVertexSize(TypeFlags);

	unsigned int BufferSize = VertexSize*_NumVertices;

	if(BufferSize == _VertexDataSizeInBytes) {
		/*	They requested the whole thing */
		memcpy(Buffer, _VertexData, BufferSize);
	}

	/*	Is it interleaved or not? */
	if(_Stride) {
		/*	They only requested some of the data */
		/*	Optimized copy */
		unsigned int LocalByteOffset = 0;
		unsigned int VideoByteOffset = 0;
		unsigned int SizeToCopy = 0;
		for(unsigned int i=0; i<(unsigned int)_VertexDescription.size(); i++) {
			unsigned int ElemSize = GetElementSizeInBytes(_VertexDescription[i].Format);

			if(TypeFlags & _VertexDescription[i].Type) {
				SizeToCopy += ElemSize;
			}
			else {
				if(SizeToCopy) {
					for(unsigned int j=0; j<_NumVertices; j++)
						memcpy((unsigned char*)Buffer+j*VertexSize+VideoByteOffset, (unsigned char*)_VertexData+j*_VertexSizeInBytes+LocalByteOffset, SizeToCopy);
				}
				VideoByteOffset += SizeToCopy;
				LocalByteOffset += SizeToCopy + ElemSize;
				SizeToCopy = 0;
			}
		}
		if(SizeToCopy) {
			for(unsigned int j=0; j<_NumVertices; j++)
				memcpy((unsigned char*)Buffer+j*VertexSize+VideoByteOffset, (unsigned char*)_VertexData+j*_VertexSizeInBytes+LocalByteOffset, SizeToCopy);
		}
	}
	else {
		unsigned int LocalByteOffset = 0;
		unsigned int VideoByteOffset = 0;
		unsigned int SizeToCopy = 0;
		for(unsigned int i=0; i<(unsigned int)_VertexDescription.size(); i++) {
			unsigned int ElemDataSize = GetElementSizeInBytes(_VertexDescription[i].Format)*_NumVertices;

			if(TypeFlags & _VertexDescription[i].Type) {
				SizeToCopy += ElemDataSize;
			}
			else {
				if(SizeToCopy) memcpy((unsigned char*)Buffer+VideoByteOffset, (unsigned char*)_VertexData+LocalByteOffset, SizeToCopy);
				VideoByteOffset += SizeToCopy;
				LocalByteOffset += SizeToCopy + ElemDataSize;
				SizeToCopy = 0;
			}
			if(SizeToCopy) memcpy((unsigned char*)Buffer+VideoByteOffset, (unsigned char*)_VertexData+LocalByteOffset, SizeToCopy);
		}
	}

	return AXSUCCESS;
}

AXResult AXMeshChunk::GetIndexData(unsigned int IndexSize, void *Buffer) {
	unsigned int BufferSize = IndexSize*_NumIndices;
	memcpy(Buffer, _IndexData, BufferSize);
	return AXSUCCESS;
}

AXResult AXMeshChunk::UploadVertexData(unsigned int TypeFlags) {
	if(TypeFlags & ~_UploadedTypeFlags) {
		AXPROFILE("Uploading vertex data");

		LOG_SYS2("Mesh chunk for mesh '%s' not uploaded, uploading with flags 0x%x...", _Owner->GetName().c_str(), TypeFlags);
		AXResources *ResourceMgr = AXBaseSystem::GetResourceMgr();
		
		void *Buffer = NULL;
		unsigned int BufferSize = 0;
		unsigned int VertexSize = 0;

		for(unsigned int i=0; i<(unsigned int)_VertexDescription.size(); i++) {
			if(_VertexDescription[i].Type & TypeFlags) {
				BufferSize += _NumVertices*GetElementSizeInBytes(_VertexDescription[i].Format);
				VertexSize += GetElementSizeInBytes(_VertexDescription[i].Format);
			}
		}

		if(_VertexBuffer && BufferSize <= _VertexBuffer->Capacity) {
			Buffer = ResourceMgr->LockVertexBuffer(_VertexBuffer);
			_VertexBuffer->BufferDesc.clear();
		
			if(!Buffer) {
				LOG_SYS("Failed uploading data, Could not lock vertex buffer!");
				return AXFAILURE;
			}
		}
		else {
			if(_VertexBuffer) FreeUploadedVertexData();

			_VertexBuffer = ResourceMgr->CreateVertexBuffer(BufferSize);

			if(!_VertexBuffer) {
				LOG_SYS("Failed uploading data, Could not create vertex buffer!");
				return AXFAILURE;
			}

			_VertexBuffer->Owner = this;

			Buffer = ResourceMgr->LockVertexBuffer(_VertexBuffer);
		}
		
		if(!Buffer) {
			FreeUploadedVertexData();
			return AXFAILURE;
		}


		_VertexBuffer->Stride = VertexSize;
		_VertexBuffer->NumFilled = BufferSize;
		_UploadedTypeFlags = TypeFlags;
		for(unsigned int i=0; i<(unsigned int)_VertexDescription.size(); i++) {
			if(TypeFlags & _VertexDescription[i].Type)
				_VertexBuffer->BufferDesc.push_back(_VertexDescription[i]);
		}

		if(BufferSize == _VertexDataSizeInBytes) {
			/*	They requested the whole thing */
			memcpy(Buffer, _VertexData, BufferSize);
		}

		/*	Is it interleaved or not? */
		if(_Stride) {
			/*	They only requested some of the data */
			/*	Optimized copy */
			unsigned int LocalByteOffset = 0;
			unsigned int VideoByteOffset = 0;
			unsigned int SizeToCopy = 0;
			for(unsigned int i=0; i<(unsigned int)_VertexDescription.size(); i++) {
				unsigned int ElemSize = GetElementSizeInBytes(_VertexDescription[i].Format);

				if(TypeFlags & _VertexDescription[i].Type) {
					SizeToCopy += ElemSize;
				}
				else {
					if(SizeToCopy) {
						for(unsigned int j=0; j<_NumVertices; j++)
							memcpy((unsigned char*)Buffer+j*VertexSize+VideoByteOffset, (unsigned char*)_VertexData+j*_VertexSizeInBytes+LocalByteOffset, SizeToCopy);
					}
					VideoByteOffset += SizeToCopy;
					LocalByteOffset += SizeToCopy + ElemSize;
					SizeToCopy = 0;
				}
			}
			if(SizeToCopy) {
				for(unsigned int j=0; j<_NumVertices; j++)
					memcpy((unsigned char*)Buffer+j*VertexSize+VideoByteOffset, (unsigned char*)_VertexData+j*_VertexSizeInBytes+LocalByteOffset, SizeToCopy);
			}
		}
		else {
			unsigned int LocalByteOffset = 0;
			unsigned int VideoByteOffset = 0;
			unsigned int SizeToCopy = 0;
			for(unsigned int i=0; i<(unsigned int)_VertexDescription.size(); i++) {
				unsigned int ElemDataSize = GetElementSizeInBytes(_VertexDescription[i].Format)*_NumVertices;

				if(TypeFlags & _VertexDescription[i].Type) {
					SizeToCopy += ElemDataSize;
				}
				else {
					if(SizeToCopy) memcpy((unsigned char*)Buffer+VideoByteOffset, (unsigned char*)_VertexData+LocalByteOffset, SizeToCopy);
					VideoByteOffset += SizeToCopy;
					LocalByteOffset += SizeToCopy + ElemDataSize;
					SizeToCopy = 0;
				}
				if(SizeToCopy) memcpy((unsigned char*)Buffer+VideoByteOffset, (unsigned char*)_VertexData+LocalByteOffset, SizeToCopy);
			}

			/*	Force interleave */
			//unsigned int VideoByteOffset = 0;
			//for(unsigned int i=0; i<(unsigned int)_VertexDescription.size(); i++) {
			//	if(TypeFlags & _VertexDescription[i].Type) {
			//		unsigned int ElemSize = GetElementSizeInBytes(_VertexDescription[i].Format);
			//		for(unsigned int j=0; j<_NumVertices; j++)
			//			memcpy((unsigned char*)Buffer+j*VertexSize+VideoByteOffset, (unsigned char*)_VertexData+_VertexDescription[i].OffsetInBytes+j*ElemSize, ElemSize);
			//		VideoByteOffset += ElemSize;
			//	}
			//}
		}

		ResourceMgr->UnlockVertexBuffer(_VertexBuffer);
	}

	return AXSUCCESS;
}

AXResult AXMeshChunk::UploadIndexData() {
	if(!_IndexFlag && _IndexData) {
		LOG_SYS1("Index data for a chunk from mesh '%s' not uploaded, uploading...", _Owner->GetName().c_str());
		AXResources *ResourceMgr = AXBaseSystem::GetResourceMgr();
		
		void *Buffer = NULL;
		unsigned int BufferSize = _IndexDataSizeInBytes;

		if(_IndexBuffer && BufferSize <= _IndexBuffer->Capacity) {
			Buffer = ResourceMgr->LockIndexBuffer(_IndexBuffer);
		
			if(!Buffer) {
				LOG_SYS("Failed uploading data, Could not lock index buffer!");
				return AXFAILURE;
			}
		}
		else {
			if(_IndexBuffer) FreeUploadedIndexData();

			_IndexBuffer = ResourceMgr->CreateIndexBuffer(BufferSize, _IndexFormat);

			if(!_IndexBuffer) {
				LOG_SYS("Failed uploading data, Could not create index buffer!");
				return AXFAILURE;
			}

			_IndexBuffer->Owner = this;

			Buffer = ResourceMgr->LockIndexBuffer(_IndexBuffer);
		}
		
		if(!Buffer) {
			FreeUploadedIndexData();
			return AXFAILURE;
		}


		_IndexBuffer->NumFilled = BufferSize;
		_IndexFlag = 1;

		/*	Copy the data */
		memcpy(Buffer, _IndexData, BufferSize);

		ResourceMgr->UnlockIndexBuffer(_IndexBuffer);
	}

	return AXSUCCESS;
}

void AXMeshChunk::FreeVertexData() {
	SafeDeleteArr(_VertexData);
	_VertexDescription.clear();
	_Stride = 0;
	_VertexDataSizeInBytes = 0;
	_VertexSizeInBytes = 0;
}

void AXMeshChunk::FreeIndexData() {
	SafeDeleteArr(_IndexData);
	_NumIndices = 0;
	_IndexDataSizeInBytes = 0;
	_IndexSizeInBytes = 0;
	_IndexFormat = (AXDataFormat)0;
}


void AXMeshChunk::FreeUploadedVertexData() {
	if(_VertexBuffer) {
		LOG_SYS1("Freeing vertex memory for a mesh chunk from '%s'", _Owner->GetName().c_str());
		AXBaseSystem::GetResourceMgr()->FreeVertexBuffer(_VertexBuffer);
		_VertexBuffer = NULL;
		_UploadedTypeFlags = 0;
	}
}

void AXMeshChunk::FreeUploadedIndexData() {
	if(_IndexBuffer) {
		LOG_SYS1("Freeing index memory for a mesh chunk from '%s'", _Owner->GetName().c_str());
		AXBaseSystem::GetResourceMgr()->FreeIndexBuffer(_IndexBuffer);
		_IndexBuffer = NULL;
		_IndexFlag = 0;
	}
}

void AXMeshChunk::SetDataSources() {
	AXBaseSystem::GetRenderer()->SetVertexSource(_VertexBuffer);
	AXBaseSystem::GetRenderer()->SetIndexSource(_IndexBuffer);
}

void AXMeshChunk::Draw() {
	if(_NumVertices == 0) return;

	if(_NumIndices)
		AXBaseSystem::GetRenderer()->RenderIndexedPrimitives(_Type, _NumVertices, _NumIndices);
	else
		AXBaseSystem::GetRenderer()->RenderPrimitives(_Type, _NumVertices);
}


void AXMeshChunk::Debug() {
	if(_Stride) {
		LOG_DBG("Debugging vertex stream, interleaved format:");
		for(unsigned int i=0; i<(unsigned int)_VertexDescription.size(); i++) {
			LOG_DBG2("     %s, %s", TypeToString(_VertexDescription[i].Type), FormatToString(_VertexDescription[i].Format));
		}
		for(unsigned int i=0; i<_NumVertices; i++) {
			LOG_DBG1("Vertex %d:", i);
			for(unsigned int j=0; j<(unsigned int)_VertexDescription.size(); j++) {
				unsigned int ElemOffset = _VertexDescription[j].OffsetInBytes;
				_DebugPrint((unsigned char*)_VertexData+i*_Stride+ElemOffset, _VertexDescription[j].Format);
			}
		}
	}
	else {
		LOG_DBG("Debugging vertex stream, non-interleaved format:");
		for(unsigned int j=0; j<(unsigned int)_VertexDescription.size(); j++) {
			LOG_DBG2("     %s, %s", TypeToString(_VertexDescription[j].Type), FormatToString(_VertexDescription[j].Format));
			for(unsigned int i=0; i<_NumVertices; i++)
				_DebugPrint((unsigned char*)_VertexData+i*GetElementSizeInBytes(_VertexDescription[j].Format)+_VertexDescription[j].OffsetInBytes, _VertexDescription[j].Format);
		}
	}
}

void AXMeshChunk::_DebugPrint(void *Data, AXDataFormat Format) {
	if(Format == AX_FORMAT_FLOAT1) {
		LOG_DBG1("Float %f", (reinterpret_cast<float*>(Data))[0]);
	}
	else if(Format == AX_FORMAT_FLOAT2) {
		LOG_DBG1("Float %f", (reinterpret_cast<float*>(Data))[0]);
		LOG_DBG1(" Float %f", (reinterpret_cast<float*>(Data))[1]);
	}
	else if(Format == AX_FORMAT_FLOAT3) {
		LOG_DBG1("Float %f", (reinterpret_cast<float*>(Data))[0]);
		LOG_DBG1(" Float %f", (reinterpret_cast<float*>(Data))[1]);
		LOG_DBG1("  Float %f", (reinterpret_cast<float*>(Data))[2]);
	}
	else if(Format == AX_FORMAT_FLOAT4) {
		LOG_DBG1("Float %f", (reinterpret_cast<float*>(Data))[0]);
		LOG_DBG1(" Float %f", (reinterpret_cast<float*>(Data))[1]);
		LOG_DBG1("  Float %f", (reinterpret_cast<float*>(Data))[2]);
		LOG_DBG1("   Float %f", (reinterpret_cast<float*>(Data))[3]);
	}
	else if(Format == AX_FORMAT_UINT1) {
		LOG_DBG1("UInt 0x%x", (reinterpret_cast<unsigned int*>(Data))[0]);
	}
	else if(Format == AX_FORMAT_SHORT1) {
		LOG_DBG1("Short %d", (reinterpret_cast<short*>(Data))[0]);
	}
	else if(Format == AX_FORMAT_USHORT1) {
		LOG_DBG1("UShort %d", (reinterpret_cast<unsigned short*>(Data))[0]);
	}
	else if(Format == AX_FORMAT_SHORT2) {
		LOG_DBG1("Short %d", (reinterpret_cast<unsigned short*>(Data))[0]);
		LOG_DBG1(" Short %d", (reinterpret_cast<unsigned short*>(Data))[1]);
	}
	else if(Format == AX_FORMAT_SHORT4) {
		LOG_DBG1("Short %d", (reinterpret_cast<unsigned short*>(Data))[0]);
		LOG_DBG1(" Short %d", (reinterpret_cast<unsigned short*>(Data))[1]);
		LOG_DBG1("  Short %d", (reinterpret_cast<unsigned short*>(Data))[2]);
		LOG_DBG1("   Short %d", (reinterpret_cast<unsigned short*>(Data))[3]);
	}
	else if(Format == AX_FORMAT_UBYTE4) {
		LOG_DBG1("UByte %d", (reinterpret_cast<unsigned char*>(Data))[0]);
		LOG_DBG1(" UByte %d", (reinterpret_cast<unsigned char*>(Data))[1]);
		LOG_DBG1("  UByte %d", (reinterpret_cast<unsigned char*>(Data))[2]);
		LOG_DBG1("   UByte %d", (reinterpret_cast<unsigned char*>(Data))[3]);
	}
}



