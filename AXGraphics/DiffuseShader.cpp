/* --------------------------------------------------------------
	File:			DiffuseShader.cpp
	Description:	See DiffuseShader.h.

	Date:			January 18, 2006
	Author:			James Long
-------------------------------------------------------------- */

#include "AXCore.h"
#include "AXGraphics.h"
#include "DiffuseShader.h"


DiffuseShader::DiffuseShader() : AXShader() {

}

DiffuseShader::~DiffuseShader() {

}

void DiffuseShader::SetupShader() {
}

void DiffuseShader::Enter() {
	SetRenderState(AX_RS_LIGHTING, 0);
	//SetRenderState(AX_RS_CULLMODE, AX_CULL_NONE);
}

void DiffuseShader::SetParameters(AXMeshChunkEntity *ChunkEntity) {
	AXRenderer *Renderer = AXBaseSystem::GetRenderer();
	Renderer->LoadMatrix(AX_MATRIX_WORLD, GetWorldMatrix(ChunkEntity));
}

void DiffuseShader::FillCache(AXMeshChunk *Chunk) {
	if(AXFAILED(Chunk->UploadVertexData(AX_ELEM_POSITION | AX_ELEM_DIFFUSE))) {
		LOG_SYS1("Failed uploading data for mesh chunk, owner: '%s'", Chunk->GetOwner()->GetName().c_str());
		return;
	}

	Chunk->UploadIndexData();
}

void DiffuseShader::Exit() {

}

AXResult DiffuseShader::HardwareSupportsIt() {
	return AXSUCCESS;
}