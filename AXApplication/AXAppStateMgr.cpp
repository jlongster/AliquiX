/* --------------------------------------------------------------
	File:			AXAppStateMgr.cpp
	Description:	Implementation for the AXAppStateMgr class.  See AXAppStateMgr.h for details.
	Date:			August 2, 2005
	Author:			James Long
-------------------------------------------------------------- */

#include "AXAppStateMgr.h"
#include "..\\AXCore.h"

// Entity types
#include "..\\AXGraphics\\AXMeshEntity.h"
#include "..\\AXGraphics\\AXCamera.h"

#include "boost\\smart_ptr.hpp"

using std::string;
using boost::shared_ptr;

/*	AppState functions */
AXAppState::AXAppState(std::string Name) : _Name(Name), _SceneMgr(NULL), _OverlayMgr(NULL) {
}
AXAppState::~AXAppState() {
	SafeDelete(_SceneMgr);
	SafeDelete(_OverlayMgr);
}

AXScene* AXAppState::GetSceneManager() {
	if(!_SceneMgr) {
		_SceneMgr = new AXScene;

		//	Register default entity types
		//_SceneMgr->RegisterEntityType<AXMeshEntity>("Mesh");
		_SceneMgr->RegisterEntityType<AXCamera>("Camera");
	}
	return _SceneMgr;
}

AXOverlay* AXAppState::GetOverlayManager() {
	if(!_OverlayMgr) _OverlayMgr = new AXOverlay;
	return _OverlayMgr;
}

void AXAppState::Execute() {
	AXPROFILE("Executing Current AppState");

	if(_SceneMgr) {
		_SceneMgr->GetScene()->Update();
		_SceneMgr->GetScene()->Execute();
	}
	if(_OverlayMgr) {
		// ... when implemented ...
	}
}

/*	AppStateMgr definitions */
AXAppStateMgr::AXAppStateMgr() : AXUpdatableSystemComponent("AppStateMgr") {
}
AXAppStateMgr::~AXAppStateMgr() {
}

AXResult AXAppStateMgr::Start() {
	return AXSUCCESS;
}

void AXAppStateMgr::Update() {
	AXPROFILE("AppState Task");

	if(_NextState) {
		_CurrentState->OnSwitchAway();
		_NextState->_FromTaskName = _CurrentState->_Name;
		_CurrentState = _NextState;
		_NextState.reset();
	}

	_CurrentState->OnPreExecute();
	_CurrentState->Execute();
	_CurrentState->OnPostExecute();
	_CurrentState->HandleInput();
}

void AXAppStateMgr::Stop() {

	if(_CurrentState) _CurrentState->OnSwitchAway();

	AppStateList::iterator it;
	for(it = _AppStateList.begin(); it != _AppStateList.end();) {
		shared_ptr<AXAppState> AppState = (*it);
		it++;

		LOG_SYS1("Shutting down and removing Application State '%s'...", AppState->_Name.c_str());
		AppState->Shutdown();
		_AppStateList.remove(AppState);
	}
}

AXResult AXAppStateMgr::AddAppState(shared_ptr<AXAppState> AppState) {
	LOG_SYS1("Adding and initializing Application State '%s'...", AppState->_Name.c_str());

	AppStateList::iterator it;
	for(it = _AppStateList.begin(); it != _AppStateList.end(); it++) {
		if((*it)->_Name == AppState->_Name) {
			LOG_SYS1("Application State '%s' was not added because one with the same name already exists.", AppState->_Name.c_str());
			return AXFAILURE;
		}
	}
	
	if( AXFAILED(AppState->Initialize()) ) {
		LOG_SYS1("Application State '%s' initialization failed!", AppState->_Name.c_str());
		AppState->Shutdown();
		return AXFAILURE;
	}

	if(_AppStateList.empty()) {
		_CurrentState = AppState;
		if(AXFAILED(_CurrentState->OnSwitchTo())) {
			LOG_SYS("Switching to first state failed - the first state must succeed!");
			return AXFAILURE;
		}
	}

	_AppStateList.push_back(AppState);


	return AXSUCCESS;
}

AXResult AXAppStateMgr::RemoveAppState(string AppStateName) {
	if(AppStateName == _CurrentState->_Name) {
		LOG_SYS1("Removing state failed: Cannot remove the current Application State '%s'!", AppStateName.c_str());		
		return AXFAILURE;
	}
	AppStateList::iterator it;
	for(it = _AppStateList.begin(); it != _AppStateList.end(); it++) {
		if((*it)->_Name == AppStateName) {
			AXLog->Write(MESSAGE_SYS, "Shutting down and removing Application State '%s'...", AppStateName.c_str());
			(*it)->Shutdown();
			_AppStateList.erase(it);
			return AXSUCCESS;
		}
	}

	LOG_SYS1("Removing state failed: Application State '%s' does not exist!", AppStateName.c_str());

	return AXFAILURE;
}

AXResult AXAppStateMgr::SwitchToAppState(string AppStateName) {
	LOG_SYS1("Switching to Application State '%s'...", AppStateName.c_str());
	if(_CurrentState->_Name == AppStateName) return AXSUCCESS;
	AppStateList::iterator it;
	for(it = _AppStateList.begin(); it != _AppStateList.end(); it++) {
		if((*it)->_Name == AppStateName) {
			if(_CurrentState != (*it)) _NextState = (*it);

			if(AXFAILED(_NextState->OnSwitchTo())) {
				LOG_SYS1("Switching to state '%s' failed!", _NextState->_Name.c_str());
				_NextState.reset();
				return AXFAILURE;
			}

			return AXSUCCESS;
		}
	}

	LOG_SYS1("Switching state failed: Application State '%s' does not exist!", AppStateName.c_str());
	return AXFAILURE;
}

void AXAppStateMgr::OnInputDeviceRenew() {
	AppStateList::iterator it;
	for(it = _AppStateList.begin(); it != _AppStateList.end(); it++) {
		(*it)->OnInputDeviceRenew();
	}
}

void AXAppStateMgr::OnRenderDeviceRenew() {
	AppStateList::iterator it;
	for(it = _AppStateList.begin(); it != _AppStateList.end(); it++) {
		(*it)->OnRenderDeviceRenew();
		(*it)->GetSceneManager()->OnRenderDeviceRenew();
	}
}