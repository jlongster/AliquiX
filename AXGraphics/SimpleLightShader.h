/* --------------------------------------------------------------
	File:			SimpleLightShader.h
	Description:	This shader renders the object with its diffuse color and enable lights.

	Date:			May 30, 2006
	Author:			James Long
-------------------------------------------------------------- */

#if !defined(SIMPLELIGHTSHADER_INCLUDED)
#define SIMPLELIGHTSHADER_INCLUDED

#include "AXShader.h"

class SimpleLightShader : public AXShader {
public:
	SimpleLightShader();
	virtual ~SimpleLightShader();

	void SetupShader();
	void Enter();
	void SetParameters(AXMeshChunkEntity *ChunkEntity);
	void FillCache(AXMeshChunk *Chunk);
	void Exit();

	AXResult HardwareSupportsIt();
private:
	unsigned int _NumLights;
};

#endif