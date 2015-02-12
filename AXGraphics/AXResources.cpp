/* --------------------------------------------------------------
	File:			AXResources.cpp
	Description:	See AXResources.h.
	Date:			January 13, 2005
	Author:			James Long
-------------------------------------------------------------- */

#include "..\\AXCore.h"
#include "AXResources.h"
#include "AXGraphics.h"
#include "AXUtilities\\AXMathFunctions.h"

AXResources::AXResources() : AXSystemComponent("ResourceMgr") {
	for(unsigned int i=0; i<MAXSHADERS; i++) {
		_Shaders[i] = NULL;
	}
}
AXResources::~AXResources() {
}

AXResult AXResources::Initialize() {
	Settings.ParseSettings("resources");
	ValidateSettings();

	_CreateVideoBuffers();
	_RegisterDefaultMeshes();

	return AXSUCCESS;
}

void AXResources::Shutdown() {

	/*	Delete Meshes */
	StringMeshMap::iterator MeshIt;
	for(MeshIt = _Meshes.begin(); MeshIt != _Meshes.end(); MeshIt++) {
		SafeDelete(MeshIt->second);
	}
	_Meshes.clear();
	

	/*	Delete Shaders */
	for(unsigned int i=0; i<MAXSHADERS; i++) {
		SafeDelete(_Shaders[i]);
	}

	/*	Delete vertex buffers */
	for(unsigned int i=0; i<(unsigned int)_VertexBuffers.size(); i++) {
		VertexBufferList::iterator VBufferIt;
		for(VBufferIt = _VertexBuffers[i].begin(); VBufferIt != _VertexBuffers[i].end(); VBufferIt++) {
			SafeDelete(*VBufferIt);
		}
		_VertexBuffers[i].clear();
	}
	_VertexBuffers.clear();


	/*	Delete index buffers */
	for(unsigned int i=0; i<(unsigned int)_IndexBuffersUShort.size(); i++) {
		IndexBufferList::iterator IBufferIt;
		for(IBufferIt = _IndexBuffersUShort[i].begin(); IBufferIt != _IndexBuffersUShort[i].end(); IBufferIt++) {
			SafeDelete(*IBufferIt);
		}
		_IndexBuffersUShort[i].clear();
	}
	_IndexBuffersUShort.clear();

	for(unsigned int i=0; i<(unsigned int)_IndexBuffersUInt.size(); i++) {
		IndexBufferList::iterator IBufferIt;
		for(IBufferIt = _IndexBuffersUInt[i].begin(); IBufferIt != _IndexBuffersUInt[i].end(); IBufferIt++) {
			SafeDelete(*IBufferIt);
		}
		_IndexBuffersUInt[i].clear();
	}
	_IndexBuffersUInt.clear();

	Settings.WriteSettings();
}

void AXResources::ValidateSettings() {
	int VertexMemory = Settings.GetSetting<int>("vertexVideoMemory");
	if(VertexMemory != 8 || VertexMemory != 16 || VertexMemory != 32)
		Settings.SetSetting("vertexVideoMemory", 32);

	int IndexMemory = Settings.GetSetting<int>("indexVideoMemory");
	if(IndexMemory != 4 || IndexMemory != 8)
		Settings.SetSetting("indexVideoMemory", 8);
}

AXResult AXResources::OnReset() {
	AXRenderer *Renderer = AXBaseSystem::GetRenderer();

	unsigned int VertexBufferSize = 0;
	unsigned int VertexPool0 = Renderer->CreateVertexBuffer();

	/*	Free vertex buffers */
	for(unsigned int i=0; i<(unsigned int)_VertexBuffers.size(); i++) {
		/*	We do it this way because FreeUploadedVertexData changes the
			order of the list and would make our iterator invalid */
		for(unsigned int j=0; j<(unsigned int)_VertexBuffers[i].size(); j++) {
			AXVertexBuffer *Buffer = _VertexBuffers[i].back();
			if(Buffer->Owner) Buffer->Owner->FreeUploadedVertexData();
			else FreeVertexBuffer(Buffer);

			Buffer->PoolID = VertexPool0;
			VertexBufferSize += Buffer->Capacity;
		}
	}

	Renderer->AllocateVertexBuffer(VertexPool0, VertexBufferSize);


	unsigned int IndexBufferSize_Pool0 = 0;
	unsigned int IndexBufferSize_Pool1 = 0;
	unsigned int IndexPool0 = Renderer->CreateIndexBuffer();
	unsigned int IndexPool1 = Renderer->CreateIndexBuffer();

	/*	Free index buffers */
	for(unsigned int i=0; i<(unsigned int)_IndexBuffersUShort.size(); i++) {
		/*	We do it this way because FreeUploadedIndexData changes the
			order of the list and would make our iterator invalid */
		for(unsigned int j=0; j<(unsigned int)_IndexBuffersUShort[i].size(); j++) {
			AXIndexBuffer *Buffer = _IndexBuffersUShort[i].back();
			if(Buffer->Owner) Buffer->Owner->FreeUploadedIndexData();
			else FreeIndexBuffer(Buffer);

			Buffer->PoolID = IndexPool0;
			IndexBufferSize_Pool0 += Buffer->Capacity;
		}
	}

	for(unsigned int i=0; i<(unsigned int)_IndexBuffersUInt.size(); i++) {
		/*	We do it this way because FreeUploadedIndexData changes the
			order of the list and would make our iterator invalid */
		for(unsigned int j=0; j<(unsigned int)_IndexBuffersUInt[i].size(); j++) {
			AXIndexBuffer *Buffer = _IndexBuffersUInt[i].back();
			if(Buffer->Owner) Buffer->Owner->FreeUploadedIndexData();
			else FreeIndexBuffer(Buffer);

			Buffer->PoolID = IndexPool1;
			IndexBufferSize_Pool1 += Buffer->Capacity;
		}
	}

	
	Renderer->AllocateIndexBuffer(IndexPool0, IndexBufferSize_Pool0, 2);
	Renderer->AllocateIndexBuffer(IndexPool1, IndexBufferSize_Pool1, 4);

	return AXSUCCESS;
}


void AXResources::RegisterMesh(AXMesh* Mesh) {
	if(!Mesh) return;
	StringMeshMap::iterator it;
	if((it = _Meshes.find(Mesh->GetName())) != _Meshes.end()) {
		LOG_SYS1("Failed registering mesh '%s'!  Name already taken", Mesh->GetName().c_str());
		return;
	}

	/* Optimize */
	for(unsigned int i=0; i<Mesh->GetNumChunks(); i++) {
		if(!Mesh->GetChunk(i)->IsInterleaved()) Mesh->GetChunk(i)->Interleave();
	}

	_Meshes[Mesh->GetName()] = Mesh;
}

AXMesh* AXResources::GetMesh(const std::string &Name) {
	StringMeshMap::iterator Iter;

	if((Iter = _Meshes.find(Name)) != _Meshes.end())
		return Iter->second;

	
	//	We need to implement loading from disk!
	LOG_SYS1("Warning - Mesh '%s' could not be found!", Name.c_str());
	return NULL;
}

AXResult AXResources::RegisterShader(AXShader *Shader, unsigned int ID) {
	if(!Shader) return AXFAILURE;
	if(ID >= MAXSHADERS) {
		LOG_SYS1("Failed registering shader, invalid id: %d", ID);
		return AXFAILURE;
	}
	if(AXFAILED(Shader->HardwareSupportsIt())) {
		return AXFAILURE;
	}

	if(_Shaders[ID] != NULL) {
		LOG_SYS1("Failed registering shader '%d'!  ID already taken", ID);
		return AXFAILURE;
	}

	Shader->_ID = ID;
	Shader->Initialize();
	_Shaders[ID] = Shader;
	return AXSUCCESS;
}

AXShader* AXResources::GetShader(unsigned int ID) {
	if(_Shaders[ID] != NULL)
		return _Shaders[ID];

	//	TODO: We need to implement loading from disk!
	LOG_SYS1("Warning - Shader '%ID' could not be found!", ID);
	return NULL;
}

unsigned int AXResources::_GetVertexBufferSizeIndex(unsigned int Size) {
	int SizeIndex = NearestPow2Index(Size)-NearestPow2Index(64*1024);
	if(SizeIndex < 0) SizeIndex = 0;
	return SizeIndex;
}

unsigned int AXResources::_GetIndexBufferSizeIndex(unsigned int Size) {
	int SizeIndex = NearestPow2Index(Size)-NearestPow2Index(32*1024);
	if(SizeIndex < 0) SizeIndex = 0;
	return SizeIndex;
}

AXVertexBuffer* AXResources::CreateVertexBuffer(unsigned int Size) {
	if(Size < 0) return NULL;

	unsigned int SizeIndex = _GetVertexBufferSizeIndex(Size);

	if(SizeIndex >= 6) {
		LOG_SYS("Failed creating vertex buffer, size exceeds maximum!");
		return NULL;
	}

	AXVertexBuffer *Buffer = _VertexBuffers[SizeIndex].front();
	if(Buffer->Owner) {
		Buffer->Owner->FreeUploadedVertexData();
	}

	_VertexBuffers[SizeIndex].erase(_VertexBuffers[SizeIndex].begin());
	_VertexBuffers[SizeIndex].push_back(Buffer);

	return Buffer;
}


void AXResources::FreeVertexBuffer(AXVertexBuffer *Buffer) {
	if(!Buffer) return;
	if(Buffer->Buffer) return;  // check to see if it's locked

	unsigned int SizeIndex = _GetVertexBufferSizeIndex(Buffer->Capacity);

	Buffer->Owner = NULL;
	Buffer->BufferDesc.clear();
	Buffer->NumFilled = 0;
	Buffer->Stride = 0;

	VertexBufferList::iterator it = _VertexBuffers[SizeIndex].begin();
	while(it != _VertexBuffers[SizeIndex].end()) {
		if((*it) == Buffer) break;
		it++;
	}
	_VertexBuffers[SizeIndex].erase(it);
	_VertexBuffers[SizeIndex].insert(_VertexBuffers[SizeIndex].begin(), Buffer);
}

void* AXResources::LockVertexBuffer(AXVertexBuffer *Buffer) {
	if(!Buffer) return NULL;
	if(Buffer->Buffer) return Buffer->Buffer;
	Buffer->Buffer = AXBaseSystem::GetRenderer()->LockVertexBuffer(Buffer->PoolID, Buffer->Offset, Buffer->Capacity);
	return Buffer->Buffer;
}
void AXResources::UnlockVertexBuffer(AXVertexBuffer *Buffer) {
	if(!Buffer) return;
	if(Buffer->Buffer) {
		AXBaseSystem::GetRenderer()->UnlockVertexBuffer(Buffer->PoolID);
		Buffer->Buffer = NULL;
	}
}

AXIndexBuffer* AXResources::CreateIndexBuffer(unsigned int Size, AXDataFormat Format) {
	if(Size < 0) return NULL;

	unsigned int SizeIndex = _GetIndexBufferSizeIndex(Size);

	if(SizeIndex >= 6) {
		LOG_SYS("Failed creating vertex buffer, size exceeds maximum!");
		return NULL;
	}

	IndexBufferList *List = NULL;
	if(Format == AX_FORMAT_USHORT1) {
		List = &_IndexBuffersUShort[SizeIndex];
	}
	else if(Format == AX_FORMAT_UINT1) {
		List = &_IndexBuffersUInt[SizeIndex];
	}

	if(!List) return NULL;

	AXIndexBuffer *Buffer = List->front();
	
	if(Buffer->Owner) {
		Buffer->Owner->FreeUploadedIndexData();
	}

	List->erase(List->begin());
	List->push_back(Buffer);

	return Buffer;
}


void AXResources::FreeIndexBuffer(AXIndexBuffer *Buffer) {
	if(!Buffer) return;
	if(Buffer->Buffer) return;  // check to see if it's locked

	unsigned int SizeIndex = _GetIndexBufferSizeIndex(Buffer->Capacity);

	Buffer->Owner = NULL;
	Buffer->NumFilled = 0;

	IndexBufferList *List = NULL;
	if(Buffer->IndexFormat == AX_FORMAT_USHORT1) {
		List = &_IndexBuffersUShort[SizeIndex];
	}
	else if(Buffer->IndexFormat == AX_FORMAT_UINT1) {
		List = &_IndexBuffersUInt[SizeIndex];
	}
	if(!List) return;

	IndexBufferList::iterator it = List->begin();
	while(it != List->end()) {
		if((*it) == Buffer) break;
		it++;
	}
	List->erase(it);
	List->insert(List->begin(), Buffer);
}

void* AXResources::LockIndexBuffer(AXIndexBuffer *Buffer) {
	if(!Buffer) return NULL;
	if(Buffer->Buffer) return Buffer->Buffer;
	Buffer->Buffer = AXBaseSystem::GetRenderer()->LockIndexBuffer(Buffer->PoolID, Buffer->Offset, Buffer->Capacity);
	return Buffer->Buffer;
}
void AXResources::UnlockIndexBuffer(AXIndexBuffer *Buffer) {
	if(!Buffer) return;
	if(Buffer->Buffer) {
		AXBaseSystem::GetRenderer()->UnlockIndexBuffer(Buffer->PoolID);
		Buffer->Buffer = NULL;
	}
}

void AXResources::DebugMesh(const std::string &Name) {
	StringMeshMap::iterator Iter;

	if((Iter = _Meshes.find(Name)) != _Meshes.end()) {
		LOG_DBG1("Debugging Mesh '%s'...", Name.c_str());
		Iter->second->Debug();
	}
}

void AXResources::Debug() {
	LOG_DBG("Debugging resources...");
	StringMeshMap::iterator Iter;
	for(Iter = _Meshes.begin(); Iter != _Meshes.end(); Iter++) {
		LOG_DBG1("Registered Mesh '%s'", Iter->first.c_str());
		Iter->second->Debug();
	}

	LOG_DBG("Printing out video buffer...");
	for(unsigned int i=0; i<(unsigned int)_VertexBuffers.size(); i++) {
		VertexBufferList::iterator it;
		for(it = _VertexBuffers[i].begin(); it != _VertexBuffers[i].end(); it++) {
			LOG_DBG2("Capacity: %d, Owner: %x", (*it)->Capacity, (*it)->Owner);
		}
	}

	LOG_DBG("Printing out index buffers...");
	for(unsigned int i=0; i<(unsigned int)_IndexBuffersUShort.size(); i++) {
		IndexBufferList::iterator it;
		for(it = _IndexBuffersUShort[i].begin(); it != _IndexBuffersUShort[i].end(); it++) {
			LOG_DBG2("Capacity: %d, Owner: %x", (*it)->Capacity, (*it)->Owner);
		}
	}
	
	for(unsigned int i=0; i<(unsigned int)_IndexBuffersUInt.size(); i++) {
		IndexBufferList::iterator it;
		for(it = _IndexBuffersUInt[i].begin(); it != _IndexBuffersUInt[i].end(); it++) {
			LOG_DBG2("Capacity: %d, Owner: %x", (*it)->Capacity, (*it)->Owner);
		}
	}
}

void AXResources::_CreateVideoBuffers() {
	AXRenderer *Renderer = AXBaseSystem::GetRenderer();

	int VertexMemory = Settings.GetSetting<int>("vertexVideoMemory");
	int IndexMemory = Settings.GetSetting<int>("indexVideoMemory");

	/*	Create the abstract video memory buffers
		we have 6 different sizes, the largest being 8 megs,
		I may tweak this later once I can run tests.
		We will fill up the video memory until the specified limit is reached */

	/*	Vertex Memory */

	unsigned int VertexPool0 = Renderer->CreateVertexBuffer();

	unsigned int NumBuffers = VertexMemory/2/((2*1024)/1024);
	unsigned int VertexBufferOffset = 0;

	for(unsigned int i=64*1024; i<=2*1024*1024; i*=2) {
		VertexBufferList BufferVector;
		for(unsigned int j=0; j<NumBuffers; j++) {
			AXVertexBuffer *Buffer = new AXVertexBuffer;
			Buffer->Capacity = i;
			Buffer->NumFilled = 0;
			Buffer->Offset = VertexBufferOffset;
			Buffer->Buffer = NULL;
			Buffer->Owner = NULL;
			Buffer->Stride = 0;

			Buffer->PoolID = VertexPool0;

			BufferVector.push_back(Buffer);
			VertexBufferOffset += i;
		}

		_VertexBuffers.push_back(BufferVector);
	}

	unsigned int VertexBufferSize = VertexBufferOffset;

	Renderer->AllocateVertexBuffer(VertexPool0, VertexBufferSize);


	/*	Index Memory */

	unsigned int IndexPool0 = Renderer->CreateIndexBuffer();
	unsigned int IndexPool1 = Renderer->CreateIndexBuffer();

	NumBuffers = IndexMemory/2/(1024/1024)/2;
	unsigned int IndexBufferOffset_Pool0 = 0;
	unsigned int IndexBufferOffset_Pool1 = 0;

	/*	Create Index Buffers using shorts (2 bytes) */
	for(unsigned int i=32*1024; i<=1024*1024; i*=2) {
		IndexBufferList BufferVector;
		for(unsigned int j=0; j<NumBuffers; j++) {
			AXIndexBuffer *Buffer = new AXIndexBuffer;
			Buffer->Capacity = i;
			Buffer->NumFilled = 0;
			Buffer->Buffer = NULL;
			Buffer->Owner = NULL;

			Buffer->PoolID = IndexPool0;
			Buffer->Offset = IndexBufferOffset_Pool0;
			Buffer->IndexFormat = AX_FORMAT_USHORT1;
			IndexBufferOffset_Pool0 += i;

			BufferVector.push_back(Buffer);
		}
		_IndexBuffersUShort.push_back(BufferVector);
	}

	/*	Create Index Buffers using ints (4 bytes) */
	for(unsigned int i=32*1024; i<=1024*1024; i*=2) {
		IndexBufferList BufferVector;
		for(unsigned int j=0; j<NumBuffers; j++) {
			AXIndexBuffer *Buffer = new AXIndexBuffer;
			Buffer->Capacity = i;
			Buffer->NumFilled = 0;
			Buffer->Buffer = NULL;
			Buffer->Owner = NULL;

			Buffer->PoolID = IndexPool1;
			Buffer->Offset = IndexBufferOffset_Pool1;
			Buffer->IndexFormat = AX_FORMAT_UINT1;
			IndexBufferOffset_Pool1 += i;

			BufferVector.push_back(Buffer);
		}

		_IndexBuffersUInt.push_back(BufferVector);
	}

	unsigned int IndexBufferSize_Pool0 = IndexBufferOffset_Pool0;
	unsigned int IndexBufferSize_Pool1 = IndexBufferOffset_Pool1;

	Renderer->AllocateIndexBuffer(IndexPool0, IndexBufferSize_Pool0, 2);
	Renderer->AllocateIndexBuffer(IndexPool1, IndexBufferSize_Pool1, 4);


}

void AXResources::_RegisterDefaultMeshes() {
	/*	Box with normals */

	FloatData Position;
	FloatData Normals;

	DataAdd3(Position, -1.0f, 1.0f, 1.0f);
	DataAdd3(Position, 1.0f, 1.0f, 1.0f);
	DataAdd3(Position, -1.0f, -1.0f, 1.0f);
	DataAdd3(Position, 1.0f, 1.0f, 1.0f);
	DataAdd3(Position, 1.0f, -1.0f, 1.0f);
	DataAdd3(Position, -1.0f, -1.0f, 1.0f);

	DataAdd3(Position, 1.0f, 1.0f, 1.0f);
	DataAdd3(Position, 1.0f, 1.0f, -1.0f);
	DataAdd3(Position, 1.0f, -1.0f, 1.0f);
	DataAdd3(Position, 1.0f, 1.0f, -1.0f);
	DataAdd3(Position, 1.0f, -1.0f, -1.0f);
	DataAdd3(Position, 1.0f, -1.0f, 1.0f);

	DataAdd3(Position, 1.0f, 1.0f, -1.0f);
	DataAdd3(Position, -1.0f, 1.0f, -1.0f);
	DataAdd3(Position, 1.0f, -1.0f, -1.0f);
	DataAdd3(Position, -1.0f, 1.0f, -1.0f);
	DataAdd3(Position, -1.0f, -1.0f, -1.0f);
	DataAdd3(Position, 1.0f, -1.0f, -1.0f);

	DataAdd3(Position, -1.0f, 1.0f, -1.0f);
	DataAdd3(Position, -1.0f, 1.0f, 1.0f);
	DataAdd3(Position, -1.0f, -1.0f, -1.0f);
	DataAdd3(Position, -1.0f, 1.0f, 1.0f);
	DataAdd3(Position, -1.0f, -1.0f, 1.0f);
	DataAdd3(Position, -1.0f, -1.0f, -1.0f);

	DataAdd3(Position, -1.0f, 1.0f, -1.0f);
	DataAdd3(Position, 1.0f, 1.0f, -1.0f);
	DataAdd3(Position, -1.0f, 1.0f, 1.0f);
	DataAdd3(Position, 1.0f, 1.0f, -1.0f);
	DataAdd3(Position, 1.0f, 1.0f, 1.0f);
	DataAdd3(Position, -1.0f, 1.0f, 1.0f);

	DataAdd3(Position, -1.0f, -1.0f, 1.0f);
	DataAdd3(Position, 1.0f, -1.0f, 1.0f);
	DataAdd3(Position, -1.0f, -1.0f, -1.0f);
	DataAdd3(Position, 1.0f, -1.0f, 1.0f);
	DataAdd3(Position, 1.0f, -1.0f, -1.0f);
	DataAdd3(Position, -1.0f, -1.0f, -1.0f);

	DataAdd3(Normals, 0.0f, 0.0f, 1.0f);
	DataAdd3(Normals, 0.0f, 0.0f, 1.0f);
	DataAdd3(Normals, 0.0f, 0.0f, 1.0f);
	DataAdd3(Normals, 0.0f, 0.0f, 1.0f);
	DataAdd3(Normals, 0.0f, 0.0f, 1.0f);
	DataAdd3(Normals, 0.0f, 0.0f, 1.0f);

	DataAdd3(Normals, 1.0f, 0.0f, 0.0f);
	DataAdd3(Normals, 1.0f, 0.0f, 0.0f);
	DataAdd3(Normals, 1.0f, 0.0f, 0.0f);
	DataAdd3(Normals, 1.0f, 0.0f, 0.0f);
	DataAdd3(Normals, 1.0f, 0.0f, 0.0f);
	DataAdd3(Normals, 1.0f, 0.0f, 0.0f);

	DataAdd3(Normals, 0.0f, 0.0f, -1.0f);
	DataAdd3(Normals, 0.0f, 0.0f, -1.0f);
	DataAdd3(Normals, 0.0f, 0.0f, -1.0f);
	DataAdd3(Normals, 0.0f, 0.0f, -1.0f);
	DataAdd3(Normals, 0.0f, 0.0f, -1.0f);
	DataAdd3(Normals, 0.0f, 0.0f, -1.0f);

	DataAdd3(Normals, -1.0f, 0.0f, 0.0f);
	DataAdd3(Normals, -1.0f, 0.0f, 0.0f);
	DataAdd3(Normals, -1.0f, 0.0f, 0.0f);
	DataAdd3(Normals, -1.0f, 0.0f, 0.0f);
	DataAdd3(Normals, -1.0f, 0.0f, 0.0f);
	DataAdd3(Normals, -1.0f, 0.0f, 0.0f);

	DataAdd3(Normals, 0.0f, 1.0f, 0.0f);
	DataAdd3(Normals, 0.0f, 1.0f, 0.0f);
	DataAdd3(Normals, 0.0f, 1.0f, 0.0f);
	DataAdd3(Normals, 0.0f, 1.0f, 0.0f);
	DataAdd3(Normals, 0.0f, 1.0f, 0.0f);
	DataAdd3(Normals, 0.0f, 1.0f, 0.0f);

	DataAdd3(Normals, 0.0f, -1.0f, 0.0f);
	DataAdd3(Normals, 0.0f, -1.0f, 0.0f);
	DataAdd3(Normals, 0.0f, -1.0f, 0.0f);
	DataAdd3(Normals, 0.0f, -1.0f, 0.0f);
	DataAdd3(Normals, 0.0f, -1.0f, 0.0f);
	DataAdd3(Normals, 0.0f, -1.0f, 0.0f);

	AXMeshChunk *BoxChunk = new AXMeshChunk(AX_PRIM_TRIANGLES);
	BoxChunk->AttachVertexData(Position, AX_ELEM_POSITION, AX_FORMAT_FLOAT3);
	BoxChunk->AttachVertexData(Normals, AX_ELEM_NORMAL, AX_FORMAT_FLOAT3);
	BoxChunk->Attributes.DiffuseSource = AX_COLORSOURCE_MATERIAL;
	BoxChunk->AssignDefaultEffect(1);

	AXMesh *Box = new AXMesh("Box");
	Box->AddChunk(BoxChunk);

	RegisterMesh(Box);
}

