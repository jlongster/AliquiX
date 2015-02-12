/* --------------------------------------------------------------
	File:			AXInput.h
	Description:	This file defines an interface that is the input device of the program.

	Date:			December 12, 2005
	Author:			James Long
-------------------------------------------------------------- */

#if !defined(AXINPUT_INCLUDED)
#define AXINPUT_INCLUDED

#include "..\\AXCommon.h"
#include "..\\AXCommonStruct.h"
#include "AXUpdatableSystemComponent.h"

typedef unsigned int key;
typedef unsigned int mousebutton;

class AXInput : public AXUpdatableSystemComponent {
public:
	AXInput();
	virtual ~AXInput();

	virtual AXResult CreateDevice()=0;
	virtual void DestroyDevice()=0;

	AXResult Start();
	void OnSuspend();
	void Update();
	void Stop();

	virtual unsigned char CurrentKey(key KeyQuery)=0;
	virtual unsigned char OldKey(key KeyQuery)=0;
	virtual unsigned char CurrentMouse(key KeyQuery)=0;
	virtual unsigned char OldMouse(key KeyQuery)=0;

	unsigned char KeyDown(key KeyQuery) { return (CurrentKey(KeyQuery) && !OldKey(KeyQuery)); }
	unsigned char KeyStillDown(key KeyQuery) { return (CurrentKey(KeyQuery) && OldKey(KeyQuery)); }
	unsigned char KeyUp(key KeyQuery) { return (!CurrentKey(KeyQuery) && OldKey(KeyQuery)); }
	unsigned char KeyStillUp(key KeyQuery) { return (!CurrentKey(KeyQuery) && !OldKey(KeyQuery)); } 
	unsigned char MouseDown(mousebutton ButtonQuery) { return (CurrentMouse(ButtonQuery) && !OldMouse(ButtonQuery)); }
	unsigned char MouseStillDown(mousebutton ButtonQuery) { return (CurrentMouse(ButtonQuery) && OldMouse(ButtonQuery)); }
	unsigned char MouseUp(mousebutton ButtonQuery) { return (!CurrentMouse(ButtonQuery) && OldMouse(ButtonQuery)); }
	unsigned char MouseStillUp(mousebutton ButtonQuery) { return (!CurrentMouse(ButtonQuery) && !OldMouse(ButtonQuery)); }

	virtual AXDimension2D MousePosAbs()=0;
	virtual AXDimension2D MousePosRel()=0;

	virtual AXResult UpdateKeyboardState()=0;
	virtual AXResult UpdateMouseState()=0;
	virtual AXResult HandleWindowEvents()=0;

	void ValidateSettings();

private:
};


#endif
