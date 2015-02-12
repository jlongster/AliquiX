/* --------------------------------------------------------------
	File:			AXUpdatableSystemComponent.h
	Description:	An interface that provides a system component the ability to be updated every frame by the kernel.
					(yeah, so 'updatable' isn't a real word, oh well)

	Date:			January 1, 2006
	Author:			James Long
-------------------------------------------------------------- */

#if !defined(AXUPDATABLESYSTEMCOMPONENT_INCLUDED)
#define AXUPDATABLESYSTEMCOMPONENT_INCLUDED

#include "..\\AXCommon.h"
#include "AXSystemComponent.h"
#include <string>

class AXUpdatableSystemComponent : public AXSystemComponent {
	friend class AXKernel;

public:
	AXUpdatableSystemComponent(const std::string Name) : AXSystemComponent(Name) {}
	virtual ~AXUpdatableSystemComponent() {}

	virtual AXResult Start()=0;
	virtual void OnSuspend() {}
	virtual void Update()=0;
	virtual void OnResume() {}
	virtual void Stop()=0;

	unsigned char GetState() { return _State; }
	void SetUpdatePriority(unsigned int Priority) { _UpdatePriority = Priority; }

	enum States {
		RUNNING,
		PAUSED_LOCAL,
		PAUSED_GLOBAL,
		DEAD
	};

private:
	AXUpdatableSystemComponent() {}

	unsigned int _UpdatePriority;
	unsigned char _State;
};

#endif