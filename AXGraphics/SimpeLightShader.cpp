/* --------------------------------------------------------------
	File:			SimpleLightShader.cpp
	Description:	See SimpleLightShader.h.

	Date:			May 30, 2006
	Author:			James Long
-------------------------------------------------------------- */

#include "AXCore.h"
#include "AXGraphics.h"
#include "SimpleLightShader.h"


SimpleLightShader::SimpleLightShader() : AXShader() {

}

SimpleLightShader::~SimpleLightShader() {

}

void SimpleLightShader::SetupShader() {
	_NumLights = AXBaseSystem::GetRenderer()->GetDeviceCapability(AX_DC_NUMLIGHTS);
	if(_NumLights > 8) _NumLights = 8;
}

void SimpleLightShader::Enter() {
	SetRenderState(AX_RS_LIGHTING, 1);
	//SetRenderState(AX_RS_CULLMODE, AX_CULL_NONE);
}

void SimpleLightShader::SetParameters(AXMeshChunkEntity *ChunkEntity) {
	AXScene *Scene = AXBaseSystem::GetSceneMgr();
	AXRenderer *Renderer = AXBaseSystem::GetRenderer();

	/*	Load the object's matrix */
	Renderer->LoadMatrix(AX_MATRIX_WORLD, GetWorldMatrix(ChunkEntity));

	LightVector Lights;
	Scene->GetLights(&Lights, GetWorldPosition(ChunkEntity), _NumLights);

	for(unsigned int i=0; i<Lights.size(); i++) {
		Lights[i]->Enable();
	}

	Renderer->SetMaterial(&ChunkEntity->Attributes);
}

void SimpleLightShader::FillCache(AXMeshChunk *Chunk) {
	if(AXFAILED(Chunk->UploadVertexData(AX_ELEM_POSITION | AX_ELEM_DIFFUSE | AX_ELEM_NORMAL))) {
		LOG_SYS1("Failed uploading data for mesh chunk, owner: '%s'", Chunk->GetOwner()->GetName().c_str());
		return;
	}

	Chunk->UploadIndexData();
}

void SimpleLightShader::Exit() {

}

AXResult SimpleLightShader::HardwareSupportsIt() {
	return AXSUCCESS;
}