/* --------------------------------------------------------------
	File:			AXTaskAppState.h
	Description:	This task handles different states of the application and lets users easy switch between states.
	Date:			August 2, 2005
	Author:			James Long
-------------------------------------------------------------- */

#if !defined(AXTASKAPPSTATE_INCLUDE)
#define AXTASKAPPSTATE_INCLUDE

#include "..\\AXCommon.h"
#include "..\\AXCore\\AXUpdatableSystemComponent.h"
#include "..\\AXGraphics\\AXScene.h"
#include "..\\AXGraphics\\AXOverlay.h"

#include <string>
#include <list>
#include "boost\\smart_ptr.hpp"

class AXAppState {
	friend class AXAppStateMgr;

public:
	AXAppState(std::string Name);
	virtual ~AXAppState();

	AXScene* GetSceneManager();
	AXOverlay* GetOverlayManager();

	std::string GetName() { return _Name; }
	std::string GetPreviousStateName() { return _FromTaskName; }

	/*	This function actually executes things and updates everything */
	void Execute();

	/*	The user implements the following methods for user-defined code */
	virtual AXResult Initialize()=0;
	virtual void Shutdown()=0;

	virtual AXResult OnSwitchTo() { return AXSUCCESS; }
	virtual void OnPreExecute() {}
	virtual void OnPostExecute() {}
	virtual void OnSwitchAway() {}

	virtual void OnRenderDeviceRenew() {}
	virtual void OnInputDeviceRenew() {}

	virtual void HandleInput()=0;

private:
	AXAppState() {}

	AXScene* _SceneMgr;
	AXOverlay* _OverlayMgr;

	std::string _Name;
	std::string _FromTaskName;
	
};

class AXAppStateMgr : public AXUpdatableSystemComponent {
public:
	AXAppStateMgr();
	virtual ~AXAppStateMgr();

	AXResult Start();
	void Update();
	void Stop();

	AXResult AddAppState(boost::shared_ptr<AXAppState> AppState);
	AXResult RemoveAppState(std::string AppStateName);
	AXResult SwitchToAppState(std::string AppStateName);

	void OnRenderDeviceRenew();
	void OnInputDeviceRenew();

private:

	typedef std::list< boost::shared_ptr<AXAppState> > AppStateList;

	boost::shared_ptr<AXAppState> _CurrentState;
	boost::shared_ptr<AXAppState> _NextState;

	AppStateList _AppStateList;
};

#endif