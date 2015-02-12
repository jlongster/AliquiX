/* --------------------------------------------------------------
	File:			AXMeshChunkEntity.h
	Description:	A Mesh is actually divided into any number chunks.  The mesh is broken up according to shaders
					and properties.  The chunks are what is actually passed to the renderer for rendering.

	Date:			January 5, 2006
	Author:			James Long
-------------------------------------------------------------- */

#if !defined(AXMESHCHUNKENTITY_INCLUDED)
#define AXMESHCHUNKENTITY_INCLUDED

#include "AXMeshChunk.h"
#include "AXMaterial.h"
#include "AXShader.h"

#include <string>

class AXMeshChunkEntity {
	friend class AXMeshEntity;

public:
	AXMeshChunkEntity();
	virtual ~AXMeshChunkEntity();

	AXResult AssignEffect(unsigned int ID);
	unsigned int GetAssignedEffect() { return _EffectID; }

	AXMeshEntity* GetOwner() { return _ParentMeshEntity; }
	AXMeshChunk* GetMeshChunk() { return _MeshChunkInstance; }

	void SelectShader();
	void PrepareShader();
	void ReleaseShader();

	void Render();

	AXMaterial Attributes;

private:

	void _LinkMeshChunk(AXMeshChunk *Chunk);

	AXMeshEntity* _ParentMeshEntity;

	unsigned int _EffectID;
	AXMeshChunk* _MeshChunkInstance;
	AXShader* _Shader;

};

#endif