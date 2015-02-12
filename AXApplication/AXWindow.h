/* --------------------------------------------------------------
	File:			AXWindow.h
	Description:	This file defines a class that handles the window and all events associated with it.

					Notice I said THE window.  This program doesn't support multiple windows.
	Date:			June 29, 2005
	Author:			James Long
-------------------------------------------------------------- */

#if !defined(AXWINDOW_INCLUDED)
#define AXWINDOW_INCLUDED

#include "..\\AXCommon.h"
#include "..\\AXCommonStruct.h"
#include "..\\AXCore\\AXSystemComponent.h"

class AXWindow : public AXSystemComponent {
public:
	AXWindow() : AXSystemComponent("Window") {}
	virtual ~AXWindow() {}

	virtual AXResult Recreate()=0;

	virtual AXResult ToggleFullscreen()=0;

	/*	These are the settings for the window
		It's public which allows you to add/change settings at will.  The ValidateSettings()
		function will make sure everything is there in a correct format so nothing goes
		wrong when the window uses the settings. */
	AXSettings Settings;
	void ValidateSettings();

	/*	Gets the current screen size */
	AXDimension2D GetScreenSize();

private:
};

#endif