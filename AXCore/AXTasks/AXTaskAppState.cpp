/* --------------------------------------------------------------
	File:			AXTaskAppState.cpp
	Description:	Implementation for the AXTaskAppState class.  See AXTaskAppState.h for details.
	Date:			August 2, 2005
	Author:			James Long
-------------------------------------------------------------- */

#include "..\\..\\AXCore.h"

shared_ptr<AXAppState> AXTaskAppState::CurrentState;
shared_ptr<AXAppState> AXTaskAppState::NextState;
list< shared_ptr<AXAppState> > AXTaskAppState::AppStateList;

AXTaskAppState::AXTaskAppState() : AXTask() {
}
AXTaskAppState::~AXTaskAppState() {
}

AXResult AXTaskAppState::Start() {
	return AXSUCCESS;
}

void AXTaskAppState::Update() {
	AXPROFILE("AppState Task");

	if(NextState) {
		CurrentState->OnSwitchAway();
		NextState->FromTaskName = CurrentState->Name;
		NextState->OnSwitchTo();
		CurrentState = NextState;
		NextState.reset();
	}

	CurrentState->HandleInput();
	CurrentState->Update();
}

void AXTaskAppState::Stop() {
	list< shared_ptr<AXAppState> >::iterator it;
	for(it = AppStateList.begin(); it != AppStateList.end();) {
		shared_ptr<AXAppState> AppState = (*it);
		it++;

		AXAppLog->Write(MESSAGE_SYS, "Shutting down and removing Application State '%s'...", AppState->Name.c_str());
		AppState->Shutdown();
		AppStateList.remove(AppState);
	}
}

AXResult AXTaskAppState::AddAppState(shared_ptr<AXAppState> AppState) {
	AXAppLog->Write(MESSAGE_SYS, "Adding and initializing Application State '%s'...", AppState->Name.c_str());

	list< shared_ptr<AXAppState> >::iterator it;
	for(it = AppStateList.begin(); it != AppStateList.end(); it++) {
		if((*it)->Name == AppState->Name) {
			AXAppLog->Write(MESSAGE_SYS, "Application State '%s' was not added because one with the same name already exists.", AppState->Name.c_str());
			return AXFAILURE;
		}
	}

	if(AppStateList.empty()) CurrentState = AppState;

	AppStateList.push_back(AppState);
	
	if( AXFAILED(AppState->Initialize()) ) {
		AXAppLog->Write(MESSAGE_SYS, "Application State '%s' initialization failed!", AppState->Name.c_str());
		return AXFAILURE;
	}

	return AXSUCCESS;
}

AXResult AXTaskAppState::RemoveAppState(string AppStateName) {
	list< shared_ptr<AXAppState> >::iterator it;
	for(it = AppStateList.begin(); it != AppStateList.end(); it++) {
		if((*it)->Name == AppStateName) {
			AXAppLog->Write(MESSAGE_SYS, "Shutting down and removing Application State '%s'...", AppStateName.c_str());
			(*it)->Shutdown();
			AppStateList.erase(it);
			return AXSUCCESS;
		}
	}

	return AXFAILURE;
}

AXResult AXTaskAppState::SwitchToAppState(string AppStateName) {
	list< shared_ptr<AXAppState> >::iterator it;
	for(it = AppStateList.begin(); it != AppStateList.end(); it++) {
		if((*it)->Name == AppStateName) {
			if(CurrentState != (*it)) NextState = (*it);
			return AXSUCCESS;
		}
	}

	AXAppLog->Write(MESSAGE_SYS, "Switching state failed: Application State '%s' does not exist!", AppStateName.c_str());
	return AXFAILURE;
}

