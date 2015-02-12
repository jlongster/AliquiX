/* --------------------------------------------------------------
	File:			DiffuseShader.h
	Description:	This shader renders the object simply with its diffuse color.
					This is the most basic shader.

	Date:			January 18, 2006
	Author:			James Long
-------------------------------------------------------------- */

#if !defined(DIFFUSESHADER_INCLUDED)
#define DIFFUSESHADER_INCLUDED

#include "AXShader.h"

class DiffuseShader : public AXShader {
public:
	DiffuseShader();
	virtual ~DiffuseShader();

	void SetupShader();
	void Enter();
	void SetParameters(AXMeshChunkEntity *ChunkEntity);
	void FillCache(AXMeshChunk *Chunk);
	void Exit();

	AXResult HardwareSupportsIt();
};

#endif