/* --------------------------------------------------------------
	File:			AXMeshEntity.h
	Description:	This file defines a renderable entity.

	Date:			December 26, 2005
	Author:			James Long
-------------------------------------------------------------- */

#if !defined(AXENTITYRENDERABLE_INCLUDED)
#define AXENTITYRENDERABLE_INCLUDED

#include "AXEntity.h"
#include "AXMeshChunkEntity.h"

#include <string>
#include <list>

class AXMeshEntity : public AXEntity {
public:
	AXMeshEntity(const std::string &Name);
	virtual ~AXMeshEntity();

	void Configure(const std::string &Parameters);
	void Update();
	void Execute();

	AXResult AssignEffect(unsigned int ID);

	AXResult AssociateMesh(const std::string &Name);
	std::string GetAssociatedMesh() { return _AssociatedMesh; }

	void RenderNormals(unsigned char Flag);

	AXMeshChunkEntity* GetChunk(const unsigned int Index);
	unsigned int GetNumChunks();

private:
	typedef std::vector< AXMeshChunkEntity* > ChunkCollection;

	ChunkCollection _Chunks;
	std::string _AssociatedMesh;

	ChunkCollection _NormalChunks;
	unsigned char _RenderNormals;
};

#endif
