/* --------------------------------------------------------------
	File:			AXShader.h
	Description:	A base shader which should be inherited to implement custom shaders.

					At some point I would like to make this much more robust, where shaders
					can be linked and it would be easy to set up effects that require multiple
					passes.

	Date:			May 28, 2006
	Author:			James Long
-------------------------------------------------------------- */

#if !defined(AXSHADER_INCLUDED)
#define AXSHADER_INCLUDED

#include "AXRenderStates.h"
#include "..\\AXUtilities\\AXMath.h"
#include "..\\AXUtilities\\AXMath.h"
#include <map>

class AXRenderer;

/*	Because of dependencies, let the order of includes be sorted out elsewhere */
class AXMeshChunk;
class AXMeshChunkEntity;

class AXShader {
	friend class AXResources;

public:
	AXShader();
	virtual ~AXShader();
	
	void Initialize();
	unsigned int GetUID() { return _ID; }
	void ResetRenderStates();

	virtual void SetupShader()=0;
	virtual void Enter()=0;
	virtual void SetParameters(AXMeshChunkEntity *ChunkEntity)=0;
	virtual void FillCache(AXMeshChunk *Chunk)=0;
	virtual void Exit()=0;

	//virtual int GetRenderFactor(unsigned char ID)=0;
	virtual AXResult HardwareSupportsIt()=0;

	/*	Functions common to shaders */
	void SetRenderState(AXRenderState State, unsigned int Value);
	AXMatrix4* GetWorldMatrix(AXMeshChunkEntity *ChunkEntity);
	AXVector3* GetWorldPosition(AXMeshChunkEntity *ChunkEntity);
private:
	typedef std::map<AXRenderState,unsigned int> RenderStateMap;

	AXRenderer *_Renderer;
	RenderStateMap _PrevStates;

	unsigned int _ID;
	//std::vector<int> _SupportedEffects;
	//std::vector<int> _EffectRenderFactors;
};

#endif