/* --------------------------------------------------------------
	File:			AXKernel.h
	Description:	The kernel is the driving mechanism for the program.
					It's a pretty simple object that simply builds tasks and then continually
					cycles through and executes them.  It also provides functionality to pause/resume
					tasks and handle other operations with tasks.

					Tasks are technically updatable system components.  If any of the system objects, like the
					renderer or the input, need to be run through each frame than they are attached here, to the kernel.

					Tasks initially added to system:
						- Name "Clock"
						- Name "Rendering"
						- Name "Input"
						- Name "SceneMgr"

	Date:			January 1, 2006
	Author:			James Long
-------------------------------------------------------------- */

#if !defined(AXKERNEL_INCLUDE)
#define AXKERNEL_INCLUDE

#include "..\\AXCommon.h"
#include "AXSystemComponent.h"
#include "AXUpdatableSystemComponent.h"

#include <list>
#include <string>

class AXKernel : public AXSystemComponent {
public:
	AXKernel();
	virtual ~AXKernel();

	void Execute();

	AXResult AddTask(AXUpdatableSystemComponent* Task);
	void RemoveTask(std::string TaskName);

	void SuspendTask(std::string TaskName);
	void ResumeTask(std::string TaskName);

	void Pause();
	void Resume();

	void KillAllTasks();

private:
	typedef std::list< AXUpdatableSystemComponent* > TaskList;
	TaskList _TaskList;
};


#endif
