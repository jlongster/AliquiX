/* --------------------------------------------------------------
	File:			AXRenderer.cpp
	Description:	Implementation for the AXRenderer class.  See AXRenderer.h for details.
	Date:			August 1, 2005
	Author:			James Long
-------------------------------------------------------------- */

#include "..\\AXCore.h"
#include "AXRenderer.h"
#include "..\\AXApplication\\AXWindow.h"
#include "AXResources.h"

// Shaders
#include "DiffuseShader.h"
#include "SimpleLightShader.h"

#include <string>

AXRenderer::AXRenderer() : AXUpdatableSystemComponent("Renderer") {
	_ClearColor = AXColorToRGB(0,0,0);
	_ClearDepth = 1.0f;
	_ClearStencil = 0;
}
AXRenderer::~AXRenderer() {
}

AXResult AXRenderer::Start() {

	/*	Create the rendering device */
	if( AXFAILED(CreateDevice()) ) {
		LOG_SYS("An error occurred while creating the rendering device!");
		return AXFAILURE;
	}

	/*	Default render queue */
	CreateRenderQueue(1000);

	/*	Shaders */
	AXResources *ResourceMgr = AXBaseSystem::GetResourceMgr();

	AXShader *Shader = new DiffuseShader;
	ResourceMgr->RegisterShader(Shader, 0);

	Shader = new SimpleLightShader;
	ResourceMgr->RegisterShader(Shader, 1);

	/*	Initial clear */
	ClearBuffers(AX_BUFFER_TARGET, _ClearColor, _ClearDepth, _ClearStencil);

	/*	Initial States */
	SetRenderState(AX_RS_CULLMODE, AX_CULL_CW);

	return AXSUCCESS;
}

void AXRenderer::Update() {
	AXPROFILE("TaskRenderUpdate");

	FlipBuffers();
	ClearBuffers(AX_BUFFER_TARGET | AX_BUFFER_DEPTH, _ClearColor, _ClearDepth, _ClearStencil);
	BeginRendering();

	unsigned int Count = (unsigned int)_RenderQueuesSorted.size();
	for(unsigned int i=0; i<Count; i++)
		_RenderQueuesSorted[i]->Execute();

	EndRendering();
}

void AXRenderer::Stop() {
	/*	Kill the renderer */
	LOG_SYS("Shutting down the renderer...");
	DestroyDevice();

	/*	Delete the render queues */
	unsigned int Count = (unsigned int)_RenderQueues.size();
	for(unsigned int i=0; i<Count; i++)
		SafeDelete(_RenderQueues[i]);
}


void AXRenderer::ValidateSettings() {
	std::string DeviceType = Settings.GetSetting<std::string>("dxDeviceType");
	if(DeviceType != "hal" && DeviceType != "ref")
		Settings.SetSetting("dxDeviceType", std::string("hal"));

	int depthBpp = Settings.GetSetting<int>("depthBpp");
	if(depthBpp != 16 && depthBpp != 32 && depthBpp != -1)
		Settings.SetSetting("depthBpp", 32);

	int screenBpp = Settings.GetSetting<int>("screenBpp");
	if(screenBpp != 16 && screenBpp != 32 && screenBpp != -1)
		Settings.SetSetting("screenBpp", 32);

	int screenshotIndex = Settings.GetSetting<int>("screenshotIndex");
	if(screenshotIndex == ERRORINT)
		Settings.SetSetting("screenshotIndex", 0);
}

AXDimension2D AXRenderer::GetScreenSize() {
	return AXBaseSystem::GetWindow()->GetScreenSize();
}

unsigned int AXRenderer::CreateRenderQueue(unsigned int Priority) {
	AXRenderQueue *NewQueue = new AXRenderQueue;

	RenderQueueVector::iterator Iter;
	for(Iter = _RenderQueuesSorted.begin(); Iter != _RenderQueuesSorted.end(); Iter++) {
		if((*Iter)->_Priority < Priority) break;
	}

    _RenderQueuesSorted.insert(Iter, NewQueue);
	_RenderQueues.push_back(NewQueue);

	return (unsigned int)_RenderQueues.size()-1;
}

void AXRenderer::AddToRenderQueue(AXMeshChunkEntity *ChunkEntity, unsigned int RenderQueueID) {
	if(RenderQueueID >= _RenderQueues.size()) return;

	_RenderQueues[RenderQueueID]->Queue(ChunkEntity);
}

