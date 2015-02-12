/* --------------------------------------------------------------
	File:			AXMesh.cpp
	Description:	See AXMesh.h.

	Date:			January 5, 2006
	Author:			James Long
-------------------------------------------------------------- */

#include "AXMesh.h"
#include "AXMeshChunk.h"
#include "..\\AXCore.h"

AXMesh::AXMesh() {
}
AXMesh::AXMesh(const std::string &Name) {
	_Name = Name;
}
AXMesh::~AXMesh() {
	FreeChunks();
	FreeNormalChunks();
}

std::string AXMesh::GetName() {
	return _Name;
}

void AXMesh::AddChunk(AXMeshChunk *Chunk) {
	Chunk->_Owner = this;
	_Chunks.push_back(Chunk);
}

AXMeshChunk* AXMesh::GetChunk(const unsigned int Index) {
	if(Index >= _Chunks.size()) return NULL;
	else return _Chunks[Index];
}

unsigned int AXMesh::GetNumChunks() {
	return (unsigned int)_Chunks.size();
}

void AXMesh::FreeChunks() {
	for(unsigned int i=0; i<_Chunks.size(); i++)
		SafeDelete(_Chunks[i]);
	_Chunks.clear();
}

void AXMesh::CalculateNormalChunks() {
	if(!_NormalChunks.empty()) return;
	for(unsigned int i=0; i<(unsigned int)_Chunks.size(); i++) {
		AXMeshChunk *Chunk = _Chunks[i];
		if(!Chunk->HasVertexData(AX_ELEM_NORMAL)) continue;

		LOG_SYS1("Calculating normals chunks of mesh '%d' for display...", i);

		unsigned int NumVertices;
		unsigned int NumIndices;
		float *Position = NULL;
		float *Normals = NULL;
		
		NumVertices = Chunk->GetNumVertices();
		NumIndices = Chunk->GetNumIndices();
		Position = new float[NumVertices*3];
		Normals = new float[NumVertices*3];
		Chunk->GetVertexData(AX_ELEM_POSITION, sizeof(float)*3, Position);
		Chunk->GetVertexData(AX_ELEM_NORMAL, sizeof(float)*3, Normals);


		FloatData Lines;

		if(NumIndices) {
			unsigned int IndexSize = Chunk->GetIndexSize();

			if(IndexSize == 2) {
				unsigned short *Index = new unsigned short[NumIndices];
				Chunk->GetIndexData(IndexSize, (void*)Index);

				for(unsigned int i=0; i<NumIndices; i++) {
					unsigned int FloatIndex = Index[i]*3;
					DataAdd3(Lines, Position[ FloatIndex ], Position[ FloatIndex+1 ], Position[ FloatIndex+2 ]);
					DataAdd3(Lines, Position[ FloatIndex ]+Normals[ FloatIndex ], Position[ FloatIndex+1 ]+Normals[ FloatIndex+1 ], Position[ FloatIndex+2 ]+Normals[ FloatIndex+2 ]);
				}

				SafeDeleteArr(Index);
			}
			else if(IndexSize == 4) {
				unsigned int *Index = new unsigned int[NumIndices];
				Chunk->GetIndexData(IndexSize, (void*)Index);

				for(unsigned int i=0; i<NumIndices; i++) {
					unsigned int FloatIndex = Index[i]*3;
					DataAdd3(Lines, Position[ FloatIndex ], Position[ FloatIndex+1 ], Position[ FloatIndex+2 ]);
					DataAdd3(Lines, Position[ FloatIndex ]+Normals[ FloatIndex ], Position[ FloatIndex+1 ]+Normals[ FloatIndex+1 ], Position[ FloatIndex+2 ]+Normals[ FloatIndex+2 ]);
				}

				SafeDeleteArr(Index);
			}
		}
		else {
			for(unsigned int i=0; i<NumVertices*3; i+=3) {
				DataAdd3(Lines, Position[i], Position[i+1], Position[i+2]);
				DataAdd3(Lines, Position[i]+Normals[i], Position[i+1]+Normals[i+1], Position[i+2]+Normals[i+2]);
			}
		}

		AXMeshChunk *NormalChunk = new AXMeshChunk(AX_PRIM_LINES);
		NormalChunk->AttachVertexData(Lines, AX_ELEM_POSITION, AX_FORMAT_FLOAT3);
		NormalChunk->AssignDefaultEffect(0);

		NormalChunk->_Owner = this;
		_NormalChunks.push_back(NormalChunk);

		SafeDeleteArr(Position);
		SafeDeleteArr(Normals);
	}
}

void AXMesh::FreeNormalChunks() {
	for(unsigned int i=0; i<(unsigned int)_NormalChunks.size(); i++) {
		SafeDelete(_NormalChunks[i]);
	}
	_NormalChunks.clear();
}

AXMeshChunk* AXMesh::GetNormalChunk(const unsigned int Index) {
	if(Index >= _NormalChunks.size()) return NULL;
	else return _NormalChunks[Index];
}

AXResult AXMesh::LoadFromFile(std::string FileName) {
	return AXFAILURE;
}

void AXMesh::Debug() {
	LOG_DBG2("Mesh '%s': %d Chunks", _Name.c_str(), GetNumChunks());

	//LOG_DBG1("Displaying chunks for mesh '%s'...", _Name.c_str());
	//for(unsigned int i=0; i<_Chunks.size(); i++) {
	//	_Chunks[i]->Debug();
	//}
}