/* --------------------------------------------------------------
	File:			DirectXInput.h
	Description:	This file declares a class that implements input using DirectX.

	Date:			December 12, 2005
	Author:			James Long
-------------------------------------------------------------- */

#if !defined(DIRECTXINPUT_INCLUDED)
#define DIRECTXINPUT_INCLUDED

#if !defined(DIRECTINPUT_VERSION)
#define DIRECTINPUT_VERSION 0x0800
#endif

#include "..\\AXCommon.h"
#include "..\\AXCommonStruct.h"
#include "AXInput.h"

#include <windows.h>
#include <dinput.h>

static const int KEYCOUNT = 256;

class DirectXInput : public AXInput {
public:
	DirectXInput();
	virtual ~DirectXInput();

	AXResult Initialize();
	void Shutdown();

	AXResult CreateDevice();
	void DestroyDevice();

	unsigned char CurrentKey(key KeyQuery) { return _KeyState[KeyQuery] & 0x80; }
	unsigned char OldKey(key KeyQuery) { return _LastKeyState[KeyQuery] & 0x80; }
	unsigned char CurrentMouse(mousebutton ButtonQuery) { return (_MouseState.rgbButtons[ButtonQuery] & 0x80); }
	unsigned char OldMouse(mousebutton ButtonQuery) { return (_LastMouseState.rgbButtons[ButtonQuery] & 0x80); }

	AXDimension2D MousePosAbs() { return _MousePosAbs; }
	AXDimension2D MousePosRel() { return _MousePosRel; }

	AXResult UpdateKeyboardState();
	AXResult UpdateMouseState();
	AXResult HandleWindowEvents();

private:
	unsigned char* _LastKeyState;
	unsigned char* _KeyState;

	DIMOUSESTATE _LastMouseState;
	DIMOUSESTATE _MouseState;

	AXDimension2D _MousePosAbs;
	AXDimension2D _MousePosRel;

	// DirectInput Objects
	IDirectInput8* _DXInput;
	IDirectInputDevice8* _DXKeyboard;
	IDirectInputDevice8* _DXMouse;

	// Keep these since we use them often
	HWND _AppHWND;
	HINSTANCE _AppHINSTANCE;

};


#endif
