/* --------------------------------------------------------------
	File:			AXTaskRender.cpp
	Description:	Implementation for the AXTaskRender class.  See AXTaskRender.h for details.
	Date:			August 3, 2005
	Author:			James Long
-------------------------------------------------------------- */

#include "..\\..\\AXApplication.h"
#include "..\\..\\AXCore.h"
#include "..\\..\\AXGraphics.h"
#include <dxerr9.h>

AXTaskRender::AXTaskRender() {
}
AXTaskRender::~AXTaskRender() {
}

AXResult AXTaskRender::Start() {

	/*	Create the renderer instance and initialize */
	AXAppRenderer = AXBaseSystem::CreateRendererInstance();
	if( AXFAILED(AXAppRenderer->Initialize()) ) {
		AXAppLog->Write(MESSAGE_SYS, "An error occurred while initializing the renderer!");
		return AXFAILURE;
	}

	/*	Create and initialize the window (this must be done after the renderer is initialized) */
	AXAppWindow = AXBaseSystem::CreateWindowInstance();
	if( AXFAILED(AXAppWindow->Create()) ) {
		AXAppLog->Write(MESSAGE_SYS, "An error occurred while creating the window!");
		return AXFAILURE;
	}

	/*	Create the rendering device */
	if( AXFAILED(AXAppRenderer->CreateDevice()) ) {
		AXAppLog->Write(MESSAGE_SYS, "An error occurred while creating the rendering device!");
		return AXFAILURE;
	}

	return AXSUCCESS;
}

void AXTaskRender::Update() {
	AXPROFILE("TaskRenderUpdate");

	// Begin the scene
	//AXAppRenderer->GetRenderDevice()->BeginScene();

	// Rendering of scene objects happens here

	// End the scene
	//AXAppRenderer->GetRenderDevice()->EndScene();

	AXAppRenderer->FlipBuffers();
	AXAppRenderer->ClearBuffers(BUFFER_TARGET | BUFFER_DEPTH, AXColorToRGBA(0,0,0,0), 1.0f, 0);
}

void AXTaskRender::Stop() {
}
