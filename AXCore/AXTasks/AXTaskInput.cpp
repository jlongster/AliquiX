/* --------------------------------------------------------------
	File:			AXTaskInput.cpp
	Description:	Implementation for the AXTaskInput class.  See AXTaskInput.h for details.
	Date:			July 22, 2005
	Author:			James Long
-------------------------------------------------------------- */


#include "..\\..\\AXCore.h"
#include "..\\..\\AXApplication.h"

AXTaskInput::AXTaskInput() : AXTask() {
}
AXTaskInput::~AXTaskInput() {
}

AXResult AXTaskInput::Start() {

	AXAppInput = AXBaseSystem::CreateInputInstance();
	if( AXFAILED(AXAppInput->Initialize()) ) {
		AXAppLog->Write(MESSAGE_SYS, "An error occurred while initializing input!");
		return AXFAILURE;
	}

	if( AXFAILED(AXAppInput->CreateDevice()) ) {
		AXAppLog->Write(MESSAGE_SYS, "An error occurred while creating input device(s)!");
		return AXFAILURE;
	}

	return AXSUCCESS;
}


void AXTaskInput::OnSuspend() {
	/*	You never want to pause the input task! */
	AXAppKernel->ResumeTask(this->Name);
}

void AXTaskInput::Update() {
	AXPROFILE("TaskInputUpdate");

	AXAppInput->UpdateKeyboardState();
	AXAppInput->UpdateMouseState();

}

void AXTaskInput::Stop() {

}
