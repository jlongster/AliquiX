/* --------------------------------------------------------------
	File:			AXSystemComponent.h
	Description:	An interface that provides basic functions that handle a system component.

	Date:			January 1, 2006
	Author:			James Long
-------------------------------------------------------------- */

#if !defined(AXSYSTEMCOMPONENT_INCLUDED)
#define AXSYSTEMCOMPONENT_INCLUDED

#include "..\\AXCommon.h"
#include "..\\AXUtilities\\AXSettings.h"
#include <string>

class AXSystemComponent {
public:
	AXSystemComponent(const std::string Name) : _Name(Name) {}
	virtual ~AXSystemComponent() {}

	virtual AXResult Initialize() { return AXSUCCESS; }
	virtual void Shutdown() {};

	AXSettings Settings;
	virtual void ValidateSettings() {}

	std::string GetName() { return _Name; }

protected:
	AXSystemComponent() {}

private:
	std::string _Name;
};

#endif