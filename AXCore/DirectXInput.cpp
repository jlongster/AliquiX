/* --------------------------------------------------------------
	File:			DirectXInput.cpp
	Description:	This file implements input using DirectX.  See DirectXInput.h for details.

	Date:			December 12, 2005
	Author:			James Long
-------------------------------------------------------------- */

#include "AXBaseSystem.h"
#include "DirectXInput.h"
#include "AXLogger.h"
#include <dxerr9.h>

#include "..\\AXApplication\\Win32Window.h"

DirectXInput::DirectXInput() : AXInput() {
	_MousePosAbs = AXDimension2D(-1, -1);

	_LastKeyState = NULL;
	_KeyState = NULL;

	_DXInput = NULL;
	_DXKeyboard = NULL;
	_DXMouse = NULL;
}
DirectXInput::~DirectXInput() {
}

AXResult DirectXInput::Initialize() {
	/*	Initialize the variables */
	_KeyState = new unsigned char[KEYCOUNT];
	_LastKeyState = new unsigned char[KEYCOUNT];

	LOG_SYS("Initializing DirectX input...");

    /*	Create a Direct Input object
		If we are using DirectX, we know that we are on a Win32 machine.  We need to get the HWND of the window,
		and very carefully send the AXAppWindow object down the class heirarchy */
	Win32Window *Window = dynamic_cast<Win32Window*>(AXBaseSystem::GetWindow());
	if(!Window) {
		LOG_SYS("A serious error has occurred while trying to retrieve the Win32 window.");
		return AXFAILURE;
	}

	HRESULT hr = DirectInput8Create(Window->GetHINSTANCE(), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&_DXInput, NULL);
	if(FAILED(hr)) {
		LOG_SYS1("Failed to create Direct Input: %s", DXGetErrorString9(hr));
		return AXFAILURE;
	}

	return AXSUCCESS;
}

void DirectXInput::Shutdown() {
	SafeDeleteArr(_KeyState);
	SafeDeleteArr(_LastKeyState);

	LOG_SYS("Shutting down input and releasing devices...");

	if(_DXKeyboard) {
		_DXKeyboard->Unacquire();
		SafeRelease(_DXKeyboard);
	}

	if(_DXMouse) {
		_DXMouse->Unacquire();
		SafeRelease(_DXMouse);
	}

	SafeRelease(_DXInput);
}

AXResult DirectXInput::CreateDevice() {
	HRESULT hr;

	LOG_SYS("Creating input devices...");

	/*	If we are using DirectX, we know that we are on a Win32 machine.  We need to get the HWND and HINSTANCE
		of the window, so very carefully send the AXAppWindow object down the class heirarchy */
	Win32Window *Window = dynamic_cast<Win32Window*>(AXBaseSystem::GetWindow());
	if(!Window) {
		LOG_SYS("A serious error has occurred while trying to retrieve the Win32 window.");
		return AXFAILURE;
	}

	_AppHINSTANCE = Window->GetHINSTANCE();
	_AppHWND = Window->GetHWND();

	if(!_DXKeyboard) {
		/* --------------------------------------------------
			Keyboard
		-------------------------------------------------- */
		hr = _DXInput->CreateDevice(GUID_SysKeyboard, &_DXKeyboard, NULL);
		if(FAILED(hr)) {
			LOG_SYS1("Failed to create Direct Input device (keyboard): %s", DXGetErrorString9(hr));
			return AXFAILURE;
		}

		/*	Set the device's data format to be standard keyboard */
		hr = _DXKeyboard->SetDataFormat(&c_dfDIKeyboard);
		if(FAILED(hr)) {
			LOG_SYS1("Failed to set data format (keyboard): %s", DXGetErrorString9(hr));
			return AXFAILURE;
		}

		/* Set the cooperative level to be background and non-exclusive */
		int KeyboardCoopFlags = DISCL_NONEXCLUSIVE;

#ifdef _DEBUG
		KeyboardCoopFlags |= DISCL_BACKGROUND;
#else
		KeyboardCoopFlags |= DISCL_FOREGROUND;
#endif

		hr = _DXKeyboard->SetCooperativeLevel(_AppHWND, KeyboardCoopFlags);
		if(FAILED(hr)) {
			LOG_SYS1("Failed to set cooperative level (keyboard): %s", DXGetErrorString9(hr));
			return AXFAILURE;
		}

		/* Acquire it and get the initial state */
		_DXKeyboard->Acquire();
	}

	if(!_DXMouse) {
		/* --------------------------------------------------
			Mouse
		-------------------------------------------------- */
		hr = _DXInput->CreateDevice(GUID_SysMouse, &_DXMouse, NULL);
		if(FAILED(hr)) {
			LOG_SYS1("Failed to create Direct Input device (mouse): %s", DXGetErrorString9(hr));
			return AXFAILURE;
		}

		/*	Set the device's data format to be standard mouse */
		hr = _DXMouse->SetDataFormat(&c_dfDIMouse);
		if(FAILED(hr)) {
			LOG_SYS1("Failed to set data format (mouse): %s", DXGetErrorString9(hr));
			return AXFAILURE;
		}

		/* Set the cooperative level to be background and non-exclusive */
		int MouseCoopFlags = DISCL_NONEXCLUSIVE;

#ifdef _DEBUG
		MouseCoopFlags |= DISCL_BACKGROUND;
#else
		MouseCoopFlags |= DISCL_FOREGROUND;
#endif

		hr = _DXMouse->SetCooperativeLevel(_AppHWND, MouseCoopFlags);
		if(FAILED(hr)) {
			LOG_SYS1("Failed to set cooperative level (mouse): %s", DXGetErrorString9(hr));
			return AXFAILURE;
		}

		_DXMouse->Acquire();
	}

	return AXSUCCESS;
}

void DirectXInput::DestroyDevice() {
	if(_DXKeyboard) {
		_DXKeyboard->Unacquire();
		SafeRelease(_DXKeyboard);
	}

	if(_DXMouse) {
		_DXMouse->Unacquire();
		SafeRelease(_DXMouse);
	}
}

AXResult DirectXInput::HandleWindowEvents() {
	MSG msg;
	/*	Take messages out of the queue so that the window will actually respond to user interaction */
	while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return AXSUCCESS;
}

AXResult DirectXInput::UpdateKeyboardState() {
	HRESULT hr;

	memcpy(_LastKeyState, _KeyState, sizeof(unsigned char)*KEYCOUNT);
	hr = _DXKeyboard->GetDeviceState(sizeof(unsigned char)*KEYCOUNT, (void*)_KeyState);

	if(FAILED(hr)) {
		// LOG_SYS("Failed getting device state (keyboard): %s", DXGetErrorString9(hr));
		// LOG_SYS("Attempting to re-acquire.");

		hr = _DXKeyboard->Acquire();
		if(hr == DIERR_INPUTLOST)
			hr = _DXKeyboard->Acquire();

		if(FAILED(hr)) {
			LOG_SYS1("Couldn't acquire device!  (keyboard): %s", DXGetErrorString9(hr));
			return AXFAILURE;
		}
		else {
			// LOG_SYS("Successfully re-acquired device (keyboard)!");
			_DXKeyboard->GetDeviceState(sizeof(unsigned char)*KEYCOUNT, (void*)_KeyState);
		}
	}

	return AXSUCCESS;
}

AXResult DirectXInput::UpdateMouseState() {
	HRESULT hr;

	_LastMouseState = _MouseState;
	hr = _DXMouse->GetDeviceState(sizeof(DIMOUSESTATE), (void*)&_MouseState);

	if(FAILED(hr)) {
		// LOG_SYS("Failed getting device state (mouse): %s", DXGetErrorString9(hr));
		// LOG_SYS("Attempting to re-acquire.");

		hr = _DXMouse->Acquire();
		if(hr == DIERR_INPUTLOST)
			hr = _DXMouse->Acquire();

		if(FAILED(hr)) {
			LOG_SYS1("Couldn't acquire device!  (mouse): %s", DXGetErrorString9(hr));
			return AXFAILURE;
		}
		else {
			// LOG_SYS("Successfully re-acquired device (mouse)!");
			_DXMouse->GetDeviceState(sizeof(DIMOUSESTATE), (void*)&_MouseState);
		}
	}

	_MousePosRel = AXDimension2D(_MouseState.lX, _MouseState.lY);

	// Do some funky Windows stuff to figure out the position of the cursor
	POINT p;
	GetCursorPos(&p);
	ScreenToClient(_AppHWND, &p);

	_MousePosAbs = AXDimension2D(p.x, p.y);

	return AXSUCCESS;
}