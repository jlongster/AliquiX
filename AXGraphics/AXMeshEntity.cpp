/* --------------------------------------------------------------
	File:			AXMeshEntity.cpp
	Description:	See AXMeshEntity.h.

	Date:			December 26, 2005
	Author:			James Long
-------------------------------------------------------------- */

#include "AXMeshEntity.h"
#include "..\\AXCore.h"
#include "AXResources.h"
#include "AXRenderer.h"

AXMeshEntity::AXMeshEntity(const std::string &Name) : AXEntity(Name) {
}
AXMeshEntity::~AXMeshEntity() {
	for(unsigned int i=0; i<_Chunks.size(); i++)
		SafeDelete(_Chunks[i]);
}

void AXMeshEntity::Configure(const std::string &Parameters) {
	AssociateMesh(Parameters);
}

void AXMeshEntity::Update() {
}

void AXMeshEntity::Execute() {
	unsigned int NumChunks = (unsigned int)_Chunks.size();
	for(unsigned int i=0; i<NumChunks; i++) {
		AXBaseSystem::GetRenderer()->AddToRenderQueue(_Chunks[i]);
	}

	if(_RenderNormals) {
		unsigned int NumNormalChunks = (unsigned int)_NormalChunks.size();
		for(unsigned int i=0; i<NumNormalChunks; i++) {
			AXBaseSystem::GetRenderer()->AddToRenderQueue(_NormalChunks[i]);
		}
	}
}

AXResult AXMeshEntity::AssignEffect(unsigned int ID) {
	unsigned int NumChunks = GetNumChunks();
	for(unsigned int i=0; i<NumChunks; i++) {
		if(AXFAILED(_Chunks[i]->AssignEffect(ID)))
			return AXFAILURE;
	}
	return AXSUCCESS;
}

AXResult AXMeshEntity::AssociateMesh(const std::string &Name) {
	AXMesh* Mesh = AXBaseSystem::GetResourceMgr()->GetMesh(Name);
	if(!Mesh) {
		LOG_SYS1("Warning - created empty entity since mesh '%s' does not exist", Name.c_str());
		return AXFAILURE;
	}

	_AssociatedMesh = Name;
	_Chunks.clear();
	
	LOG_DBG1("Linking to Mesh '%s'...", Name.c_str());
	unsigned int NumChunks = Mesh->GetNumChunks();
	for(unsigned int i=0; i<NumChunks; i++) {
		LOG_DBG1("Chunk %d...", i);
		AXMeshChunkEntity *ChunkEntity= new AXMeshChunkEntity;

		AXMeshChunk *Chunk = Mesh->GetChunk(i);
		ChunkEntity->_LinkMeshChunk(Chunk);
		ChunkEntity->_ParentMeshEntity = this;
		ChunkEntity->Attributes = Chunk->Attributes;
		ChunkEntity->AssignEffect(Chunk->GetAssignedEffect());

		_Chunks.push_back(ChunkEntity);
	}

	return AXSUCCESS;
}

void AXMeshEntity::RenderNormals(unsigned char Flag) {
	_RenderNormals = Flag;
	AXMesh* Mesh = AXBaseSystem::GetResourceMgr()->GetMesh(_AssociatedMesh);

	if(_RenderNormals && _NormalChunks.empty()) {
        Mesh->CalculateNormalChunks();

		unsigned int NumChunks = Mesh->GetNumChunks();
		for(unsigned int i=0; i<NumChunks; i++) {
			AXMeshChunkEntity *ChunkEntity= new AXMeshChunkEntity;

			AXMeshChunk *Chunk = Mesh->GetNormalChunk(i);
			ChunkEntity->_LinkMeshChunk(Chunk);
			ChunkEntity->_ParentMeshEntity = this;
			ChunkEntity->Attributes = Chunk->Attributes;
			ChunkEntity->AssignEffect(Chunk->GetAssignedEffect());

			_NormalChunks.push_back(ChunkEntity);
		}
	}
	else if(!_RenderNormals && !_NormalChunks.empty()) {
		for(unsigned int i=0; i<_NormalChunks.size(); i++)
			SafeDelete(_NormalChunks[i]);
		_NormalChunks.clear();
	}
}

AXMeshChunkEntity* AXMeshEntity::GetChunk(const unsigned int Index) {
	if(Index >= 0 && Index < (unsigned int)_Chunks.size()) return _Chunks[Index];
	return NULL;
}

unsigned int AXMeshEntity::GetNumChunks() {
	return (unsigned int)_Chunks.size();
}

