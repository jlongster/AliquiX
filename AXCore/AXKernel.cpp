/* --------------------------------------------------------------
	File:			AXKernel.cpp
	Description:	Implementation for the AXKernel class.  See AXKernel.h for details.
	Date:			July 21, 2005
	Author:			James Long
-------------------------------------------------------------- */

#include "AXBaseSystem.h"
#include "AXKernel.h"
#include "AXLogger.h"
#include "..\\AXUtilities\\AXProfiler.h"
#include <crtdbg.h>

#include "AXUpdatableSystemComponent.h"

using std::string;

AXKernel::AXKernel() : AXSystemComponent("Kernel") {
	_TaskList.clear();
}
AXKernel::~AXKernel() {
}

void AXKernel::Execute() {
	LOG_SYS("Executing the kernel...");

	TaskList::iterator it;

	while(!_TaskList.empty()) {
		{	// Start profile sample
			AXPROFILE("Main loop");

			for(it = _TaskList.begin(); it != _TaskList.end();) {
				AXUpdatableSystemComponent* Task = *it;
				it++;

				if(Task->_State == AXUpdatableSystemComponent::DEAD) {
					LOG_SYS1("Stopping and removing task '%s'...", Task->GetName().c_str());
					Task->Stop();
					_TaskList.remove(Task);
				}
				else if(Task->_State == AXUpdatableSystemComponent::RUNNING) Task->Update();
			}
		}

#ifdef _DEBUG
		_CrtCheckMemory();
		AXProfileSample::Output();
#endif

	}
}

AXResult AXKernel::AddTask(AXUpdatableSystemComponent* Task) {
	LOG_SYS1("Adding and starting task '%s'...", Task->GetName().c_str());

	/*	We aren't using std::find because AXTask is not equality testable */
	TaskList::iterator it;
	for(it = _TaskList.begin(); it != _TaskList.end(); it++) {
		if((*it)->GetName() == Task->GetName()) {
			LOG_SYS("Task was not added because a task with the same name already exists.");
			return AXFAILURE;
		}
	}


	if( AXFAILED(Task->Start()) ) {
		LOG_SYS1("Task %s failed to start!\n", Task->GetName().c_str());	
		return AXFAILURE;
	}

	for(it = _TaskList.begin(); it != _TaskList.end(); it++) {
		if((*it)->_UpdatePriority < Task->_UpdatePriority) break;
	}
    _TaskList.insert(it, Task);

	Task->_State = AXUpdatableSystemComponent::RUNNING;

	return AXSUCCESS;
}

void AXKernel::RemoveTask(string TaskName) {
	TaskList::iterator it;
	for(it = _TaskList.begin(); it != _TaskList.end(); it++) {
		AXUpdatableSystemComponent* Task = (*it);
		if(Task->GetName() == TaskName) {
			Task->_State = AXUpdatableSystemComponent::DEAD;
		}
	}
}


void AXKernel::SuspendTask(string TaskName) {
	TaskList::iterator it;
	for(it = _TaskList.begin(); it != _TaskList.end(); it++) {
		AXUpdatableSystemComponent* Task = (*it);
		if(Task->GetName() == TaskName) {
			if(Task->_State == AXUpdatableSystemComponent::RUNNING) {
				Task->_State = AXUpdatableSystemComponent::PAUSED_LOCAL;
				Task->OnSuspend();
			}
			break;
		}
	}
}

void AXKernel::ResumeTask(string TaskName) {
	TaskList::iterator it;
	for(it = _TaskList.begin(); it != _TaskList.end(); it++) {
		AXUpdatableSystemComponent* Task = (*it);
		if(Task->GetName() == TaskName) {
			if(Task->_State == AXUpdatableSystemComponent::PAUSED_LOCAL) {
				Task->_State = AXUpdatableSystemComponent::RUNNING;
				Task->OnResume();
			}
			break;
		}
	}
}

void AXKernel::Pause() {
	TaskList::iterator it;
	for(it = _TaskList.begin(); it != _TaskList.end(); it++) {
		AXUpdatableSystemComponent* Task = (*it);
		if(Task->_State == AXUpdatableSystemComponent::RUNNING) {
			Task->_State = AXUpdatableSystemComponent::PAUSED_GLOBAL;
			Task->OnSuspend();
		}
	}
}

void AXKernel::Resume() {
	TaskList::iterator it;
	for(it = _TaskList.begin(); it != _TaskList.end(); it++) {
		AXUpdatableSystemComponent* Task = (*it);
		if(Task->_State == AXUpdatableSystemComponent::PAUSED_GLOBAL) {
			Task->_State = AXUpdatableSystemComponent::RUNNING;
			Task->OnResume();
		}
	}
}

void AXKernel::KillAllTasks() {
	TaskList::iterator it;
	for(it = _TaskList.begin(); it != _TaskList.end(); it++) {
		AXUpdatableSystemComponent* Task = (*it);
		Task->_State = AXUpdatableSystemComponent::DEAD;
	}
}
