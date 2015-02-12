/* --------------------------------------------------------------
	File:			AXShader.cpp
	Description:	See AXShader.h.

	Date:			May 28, 2006
	Author:			James Long
-------------------------------------------------------------- */

#include "AXGraphics.h"
#include "AXShader.h"

AXShader::AXShader() {
}
AXShader::~AXShader() {
}

void AXShader::Initialize() {
	_Renderer = AXBaseSystem::GetRenderer();
	SetupShader();
}

void AXShader::ResetRenderStates() {
	RenderStateMap::iterator it;
	for(it = _PrevStates.begin(); it != _PrevStates.end(); it++) {
		_Renderer->SetRenderState(it->first, it->second);
	}
}

void AXShader::SetRenderState(AXRenderState State, unsigned int Value) {
	unsigned int CurState = _Renderer->GetRenderState(State);
	if(CurState != Value) {
		_PrevStates[State] = CurState;
		_Renderer->SetRenderState(State, Value);
	}
}

AXMatrix4* AXShader::GetWorldMatrix(AXMeshChunkEntity *ChunkEntity) {
	return ChunkEntity->GetOwner()->GetSceneNode()->GetWorldMatrix();
}

AXVector3* AXShader::GetWorldPosition(AXMeshChunkEntity *ChunkEntity) {
	return ChunkEntity->GetOwner()->GetSceneNode()->GetWorldPosition();
}