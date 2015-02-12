/* --------------------------------------------------------------
	File:			AXTaskAppState.h
	Description:	This task handles different states of the application and lets users easy switch between states.
	Date:			August 2, 2005
	Author:			James Long
-------------------------------------------------------------- */

#if !defined(AXTASKAPPSTATE_INCLUDE)
#define AXTASKAPPSTATE_INCLUDE

class AXTaskAppState;

class AXAppState {
	friend AXTaskAppState;

public:
	AXAppState() : Name("task_name") {}
	virtual ~AXAppState() {}

	virtual AXResult Initialize()=0;
	virtual void Shutdown()=0;

	virtual void OnSwitchTo() {}
	virtual void Update()=0;
	virtual void OnSwitchAway() {}

	virtual void HandleInput()=0;

	string Name;

private:
	string FromTaskName;
};

class AXTaskAppState : public AXTask {
public:
	AXTaskAppState();
	virtual ~AXTaskAppState();

	AXResult Start();
	void Update();
	void Stop();

	static AXResult AddAppState(shared_ptr<AXAppState> AppState);
	static AXResult RemoveAppState(string AppStateName);
	static AXResult SwitchToAppState(string AppStateName);


private:
	static shared_ptr<AXAppState> CurrentState;
	static shared_ptr<AXAppState> NextState;
	static list< shared_ptr<AXAppState> > AppStateList;
};

#endif