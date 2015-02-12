 /* --------------------------------------------------------------
	File:			AXRenderQueue.cpp
	Description:	See AXRenderQueue.h

	Date:			January 12, 2006
	Author:			James Long
-------------------------------------------------------------- */

#include "..\\AXCore.h"
#include "AXRenderQueue.h"
#include <algorithm>

struct AscendingChunkSort {
	unsigned char operator()(AXMeshChunkEntity *m1, AXMeshChunkEntity *m2) {
			/*	TODO:	For now we only sort according to effects.  We should really sort by several
				parameters, such as shader, depth, priority, and anything else. */
			if(m1->GetAssignedEffect() < m2->GetAssignedEffect())
				return 1;
			else
				return 0;
	}
};

AXRenderQueue::AXRenderQueue() {
}
AXRenderQueue::~AXRenderQueue() {
}

void AXRenderQueue::Queue(AXMeshChunkEntity *MeshChunk) {
	_RenderQueue.push_back(MeshChunk);
}

void AXRenderQueue::Execute() {
	AXPROFILE("RenderQueue Execution");

	std::sort(_RenderQueue.begin(), _RenderQueue.end(), AscendingChunkSort());

	unsigned int CurrentShader = (unsigned int)-1;
	unsigned int Count = (unsigned int)_RenderQueue.size();
	for(unsigned int i=0; i<Count; i++) {
		unsigned int Shader = _RenderQueue[i]->GetAssignedEffect();
		if(CurrentShader != Shader) {
			if(i>0) _RenderQueue[i-1]->ReleaseShader();
            _RenderQueue[i]->SelectShader();
			CurrentShader = Shader;
		}

		_RenderQueue[i]->PrepareShader();
		_RenderQueue[i]->Render();

		if(i == Count-1) {
			_RenderQueue[i]->ReleaseShader();
		}
	}
	_RenderQueue.clear();
}
