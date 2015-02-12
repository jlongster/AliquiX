/* --------------------------------------------------------------
	File:			AXInput.cpp
	Description:	This file implements any code for the interface.

	Date:			December 12, 2005
	Author:			James Long
-------------------------------------------------------------- */

#include "AXBaseSystem.h"
#include "AXInput.h"
#include "AXKernel.h"
#include "AXLogger.h"
#include "..\\AXUtilities\\AXProfiler.h"

AXInput::AXInput() : AXUpdatableSystemComponent("Input") {
}
AXInput::~AXInput() {
}

AXResult AXInput::Start() {

	if( AXFAILED(CreateDevice()) ) {
		LOG_SYS("An error occurred while creating input device(s)!");
		return AXFAILURE;
	}

	return AXSUCCESS;
}


void AXInput::OnSuspend() {
	/*	You never want to pause the input task! */
	AXBaseSystem::GetKernel()->ResumeTask(this->GetName());
}

void AXInput::Update() {
	AXPROFILE("TaskInputUpdate");

	HandleWindowEvents();
	UpdateKeyboardState();
	UpdateMouseState();
}

void AXInput::Stop() {
	DestroyDevice();
}

void AXInput::ValidateSettings() {
}
