/* --------------------------------------------------------------
	File:			Win32Window.cpp
	Description:	Implementation for the Win32 version of the window.  See Win32Window.h and AXWindow.h for details.
	Date:			June 29, 2005
	Author:			James Long
-------------------------------------------------------------- */

#include "..\\AXCore.h"
#include "Win32Window.h"
#include "..\\AXGraphics\\AXRenderer.h"
#include "..\\AXGraphics\\AXResources.h"
#include <windows.h>

Win32Window::Win32Window() : AXWindow(), _hWnd(NULL), _hInstance(NULL) {
}
Win32Window::~Win32Window() {
}

AXResult Win32Window::Initialize() {

	Settings.ParseSettings("window");
	ValidateSettings();

	if( AXFAILED(_RegisterClass()) ) {
		AXLog->Write(MESSAGE_SYS, "Could not register the window class!");
		return AXFAILURE;
	}

	if( AXFAILED(_MakeWindow()) ) {
		Shutdown();
		AXLog->Write(MESSAGE_SYS, "Could not create the window!");
		return AXFAILURE;
	}

	return AXSUCCESS;
}

AXResult Win32Window::Recreate() {
	if(_hWnd) {
		DestroyWindow(_hWnd);
		_hWnd = NULL;
	}

	ValidateSettings();

	if( AXFAILED(_MakeWindow()) ) {
		Shutdown();
		AXLog->Write(MESSAGE_SYS, "Could not create the window!");
		return AXFAILURE;
	}

	return AXSUCCESS;
}

void Win32Window::Shutdown() {
	if(_hWnd) {
		DestroyWindow(_hWnd);
		_hWnd = NULL;
	}

	UnregisterClass("aliquix_program", _hInstance);
	_hInstance = NULL;

	Settings.WriteSettings();
}

AXResult Win32Window::ToggleFullscreen() {
	if(Settings.GetSetting<int>("fullscreen"))
		Settings.SetSetting("fullscreen", 0);
	else
		Settings.SetSetting("fullscreen", 1);


	AXBaseSystem::GetInput()->DestroyDevice();
	AXBaseSystem::GetRenderer()->DestroyDevice();
	if( AXFAILED(Recreate()) ) {
		AXLog->Write(MESSAGE_APP, "Failed to recreate window after toggling fullscreen!");
		return AXFAILURE;
	}
	if( AXFAILED(AXBaseSystem::GetRenderer()->CreateDevice()) ) {
		return AXFAILURE;
	}
	if( AXFAILED(AXBaseSystem::GetInput()->CreateDevice()) ) {
		return AXFAILURE;
	}

	AXBaseSystem::GetResourceMgr()->OnReset();

	return AXSUCCESS;
}

AXResult Win32Window::_RegisterClass() {

	_hInstance = GetModuleHandle(NULL);

	WNDCLASS wc;

	wc.style = CS_CLASSDC;
	wc.lpfnWndProc = (WNDPROC) Win32Window::WindowProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = _hInstance;
	wc.hIcon = LoadIcon(_hInstance, MAKEINTRESOURCE(AXICONRES));
	wc.hCursor = LoadCursor(_hInstance, IDC_ARROW);
	wc.hbrBackground = NULL;
	wc.lpszMenuName = NULL;
	wc.lpszClassName = "aliquix_program";

	if(!RegisterClass(&wc)) return AXFAILURE;

	return AXSUCCESS;
}

AXResult Win32Window::_MakeWindow() {

	std::string AppName = AXBaseSystem::Instance().Settings.GetSetting<std::string>("name");
	RECT windowRect;
	int WindowFlags = 0;

	windowRect.left = 0;
	windowRect.top = 0;

	if(Settings.GetSetting<int>("fullscreen")) {
		windowRect.right = (long)Settings.GetSetting<int>("fullscreenWidth");
		windowRect.bottom = (long)Settings.GetSetting<int>("fullscreenHeight");

		WindowFlags = WS_POPUP;
	}
	else {
		windowRect.right = (long)Settings.GetSetting<int>("windowedWidth");
		windowRect.bottom = (long)Settings.GetSetting<int>("windowedHeight");
		
		WindowFlags = WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
		AdjustWindowRect(&windowRect, WindowFlags, FALSE);
	}


	_hWnd = CreateWindow("aliquix_program", AppName.c_str(), WindowFlags, 0, 0,
		windowRect.right-windowRect.left, windowRect.bottom-windowRect.top, NULL, NULL, _hInstance, NULL);

	if(!_hWnd) return AXFAILURE;

	ShowWindow(_hWnd, SW_SHOW);
	//SetForegroundWindow(_hWnd);
	//SetFocus(_hWnd);

	return AXSUCCESS;
}

LRESULT CALLBACK Win32Window::WindowProc(HWND hWnd, unsigned int message, WPARAM wParam, LPARAM lParam) {
	switch(message) {
		case WM_CLOSE:
			AXBaseSystem::GetKernel()->KillAllTasks();
			return 0;
		case WM_ACTIVATE:
			if(!HIWORD(wParam))
				AXBaseSystem::GetKernel()->Resume();
			else
				AXBaseSystem::GetKernel()->Pause();
			break;
		case WM_SYSCOMMAND:
			switch (wParam) {
				case SC_SCREENSAVE:
				case SC_MONITORPOWER:
				return 0;
			}
			break;
		case WM_SIZE:
			// This shouldn't happen in fullscreen, but just to be safe
			if(!AXBaseSystem::GetWindow()->Settings.GetSetting<int>("fullscreen")) {
				AXBaseSystem::GetWindow()->Settings.SetSetting("windowedWidth", (int)LOWORD(lParam));
				AXBaseSystem::GetWindow()->Settings.SetSetting("windowedHeight", (int)HIWORD(lParam));
				AXBaseSystem::GetRenderer()->OnWindowChange();
			}
			break;
		case WM_SETCURSOR:
			SetCursor(NULL);
			
			break;
		default:
			break;
	}
	
	return DefWindowProc(hWnd, message, wParam, lParam);
}

