/* --------------------------------------------------------------
	File:			Win32Window.h
	Description:	This is the declaration for the Win32 version of the window.
					It implements AXWindow.
	Date:			December 12, 2005
	Author:			James Long
-------------------------------------------------------------- */

#if !defined(WIN32WINDOW_INCLUDED)
#define WIN32WINDOW_INCLUDED

#include "..\\AXCommon.h"
#include "AXWindow.h"
#include <windows.h>

// The resource number of icon to be used (should use this constant in app)
const static int AXICONRES = 101;

class Win32Window : public AXWindow {
public:
	Win32Window();
	virtual ~Win32Window();

	AXResult Initialize();
	AXResult Recreate();
	void Shutdown();

	AXResult ToggleFullscreen();

	static LRESULT CALLBACK WindowProc(HWND hWnd, unsigned int message, WPARAM wParam, LPARAM lParam);

	HWND GetHWND() { return _hWnd; }
	HINSTANCE GetHINSTANCE() { return _hInstance; }

private:
	AXResult _RegisterClass();
	AXResult _MakeWindow();

	MSG _msg;

	HINSTANCE _hInstance;
	HWND _hWnd;
};

#endif
