/* --------------------------------------------------------------
	File:			AXBaseSystem.cpp
	Description:	Implementation for the AXBaseSystem class.  See AXBaseSystem.h for details.
	Date:			June 29, 2005
	Author:			James Long
-------------------------------------------------------------- */

#include "AXBaseSystem.h"

#include "AXLogger.h"
#include "AXKernel.h"
#include "AXClock.h"
#include "AXInput.h"

#include "..\\AXApplication\\AXWindow.h"
#include "..\\AXGraphics\\AXRenderer.h"
#include "..\\AXGraphics\\AXResources.h"
#include "..\\AXGraphics\\AXScene.h"

#include <crtdbg.h>

#include <string>


#ifdef WIN32
#include "..\\AXApplication\\Win32Window.h"
#include "..\\AXGraphics\\DirectXRenderer.h"
#include "..\\AXCore\\DirectXInput.h"
#endif

AXWindow* AXBaseSystem::_Window = NULL;
AXLogger* AXBaseSystem::_Logger = NULL;
AXKernel* AXBaseSystem::_Kernel = NULL;
AXRenderer* AXBaseSystem::_Renderer = NULL;
AXInput* AXBaseSystem::_Input = NULL;
AXScene* AXBaseSystem::_SceneMgr = NULL;
AXClock* AXBaseSystem::_Clock = NULL;
AXResources *AXBaseSystem::_ResourceMgr = NULL;

AXBaseSystem::AXBaseSystem() : AXSingleton<AXBaseSystem>() {
}
AXBaseSystem::~AXBaseSystem() {
}

AXResult AXBaseSystem::Initialize() {
#ifdef _DEBUG
	/*	Check for memory leaks at the end */
	int tempflag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
	tempflag |= _CRTDBG_LEAK_CHECK_DF;
	_CrtSetDbgFlag(tempflag);
#endif

	/*	Parse system settings */
	Settings.ParseSettings("system");
	ValidateSettings();
	OnLoadSettings();


	/* --------------------------------------------
		Create and initialize system components
	-------------------------------------------- */

	AXSystemComponent *Component;

	/*	Logger */
	Component = AXBaseSystem::CreateLoggerInstance();
	if(!Component) return AXFAILURE;
	if(AXFAILED(Component->Initialize())) return AXFAILURE;
	AXBaseSystem::GetLogger()->Write(MESSAGE_SYS, "AliquiX Started ~ " __TIMESTAMP__);

	/*	Kernel */
	Component = AXBaseSystem::CreateKernelInstance();
	if(!Component) {
		LOG_SYS("An error occurred while creating the kernel!");
		return AXFAILURE;
	}
	if(AXFAILED(Component->Initialize())) {
		LOG_SYS("Kernel initialization failed!");
		return AXFAILURE;
	}

	/*	Renderer */
	Component = AXBaseSystem::CreateRendererInstance();
	if(!Component) {
		LOG_SYS("An error occurred while creating the renderer!");
		return AXFAILURE;
	}
	if(AXFAILED(Component->Initialize())) {
		LOG_SYS("Renderer initialization failed!");
		return AXFAILURE;
	}

	/*	Window */
	Component = AXBaseSystem::CreateWindowInstance();
	if(!Component) {
		LOG_SYS("An error occurred while creating the window instance!");
		return AXFAILURE;
	}
	if(AXFAILED(Component->Initialize())) {
		LOG_SYS("Creating the window failed!");
	}

	/*	Input */
	Component = AXBaseSystem::CreateInputInstance();
	if(!Component) {
		LOG_SYS("An error occurred while creating the input!");
		return AXFAILURE;
	}
	if(AXFAILED(Component->Initialize())) {
		LOG_SYS("Input initialization failed!");
		return AXFAILURE;
	}

	/*	SceneMgr */
	Component = AXBaseSystem::CreateSceneMgrInstance();
	if(!Component) {
		LOG_SYS("An error occurred while creating the scene manager!");
		return AXFAILURE;
	}
	if(AXFAILED(Component->Initialize())) {
		LOG_SYS("Scene Manager initialization failed!");
		return AXFAILURE;
	}

	/*	Clock */
	Component = AXBaseSystem::CreateClockInstance();
	if(!Component) {
		LOG_SYS("An error occurred while creating the clock!");
		return AXFAILURE;
	}
	if(AXFAILED(Component->Initialize())) {
		LOG_SYS("Clock initialization failed!");
		return AXFAILURE;
	}

	/*	ResourceMgr */
	Component = AXBaseSystem::CreateResourceMgrInstance();
	if(!Component) {
		LOG_SYS("An error occurred while creating the resource manager!");
		return AXFAILURE;
	}


	/* ---------------------------------------------------
		Attach updatable system components to the kernel
	--------------------------------------------------- */

	AXKernel* Kernel = AXBaseSystem::GetKernel();

	AXUpdatableSystemComponent* Process = AXBaseSystem::GetRenderer();
	Process->SetUpdatePriority(1000);
	if( AXFAILED(Kernel->AddTask(Process)) )
		return AXFAILURE;
	
	Process = AXBaseSystem::GetInput();
	Process->SetUpdatePriority(500);
	if( AXFAILED(Kernel->AddTask(Process)) )
		return AXFAILURE;

	Process = AXBaseSystem::GetClock();
	Process->SetUpdatePriority(20);
	if( AXFAILED(Kernel->AddTask(Process)) )
		return AXFAILURE;

	Process = AXBaseSystem::GetSceneMgr();
	Process->SetUpdatePriority(25);
	if( AXFAILED(Kernel->AddTask(Process)) )
		return AXFAILURE;

	/* ------------------------------------------------
		Resources depends on everything being created,
		so we initialize it last.
	------------------------------------------------ */
	if(AXFAILED(AXBaseSystem::GetResourceMgr()->Initialize())) {
		LOG_SYS("Clock initialization failed!");
		return AXFAILURE;
	}

	/*	Call user-defined event handlers */
	if( AXFAILED(OnInitialize()) ) {
		LOG_SYS("User defined setup failed!");
		return AXFAILURE;
	}

	return AXSUCCESS;
}

void AXBaseSystem::Run() {
	AXBaseSystem::GetKernel()->Execute();
}

void AXBaseSystem::Shutdown() {
	/*	Let the user do any needed actions */
	OnShutdown();

	/*	Erase any settings here that you don't want logged */
	Settings.SetSetting("name", std::string(""));

	/*	Write the settings */
	Settings.WriteSettings();


	/* --------------------------------------------
		Shutdown and delete system components
	-------------------------------------------- */

	AXSystemComponent* Component;

	/*	ResourceMgr */
	Component = AXBaseSystem::GetResourceMgr();
	if(Component) {
		Component->Shutdown();
		SafeDelete(Component);
	}

	/*	Clock */
	Component = AXBaseSystem::GetClock();
	if(Component) {
		Component->Shutdown();
		SafeDelete(Component);
	}

	/*	SceneMgr */
	Component = AXBaseSystem::GetSceneMgr();
	if(Component) {
		Component->Shutdown();
		SafeDelete(Component);
	}

	/*	Input */
	Component = AXBaseSystem::GetInput();
	if(Component) {
		Component->Shutdown();
		SafeDelete(Component);
	}

	/*	Window */
	Component = AXBaseSystem::GetWindow();
	if(Component) {
		Component->Shutdown();
		SafeDelete(Component);
	}

	/*	Renderer */
	Component = AXBaseSystem::GetRenderer();
	if(Component) {
		Component->Shutdown();
		SafeDelete(Component);
	}

	/*	Kernel	*/
	Component = AXBaseSystem::GetKernel();
	if(Component) {
		Component->Shutdown();
		SafeDelete(Component);
	}

	/*	Logger */
	Component = AXBaseSystem::GetLogger();
	if(Component) {
		Component->Shutdown();
		SafeDelete(Component);
	}

}

void AXBaseSystem::ValidateSettings() {
	std::string Renderer = Settings.GetSetting<std::string>("renderer");
	if(Renderer != "directx")
		Settings.SetSetting("renderer", std::string("directx"));

	std::string Name = Settings.GetSetting<std::string>("name");
	if(Name == "defaultstring")
		Settings.SetSetting("name", "AliquiX App");
}



AXWindow* AXBaseSystem::CreateWindowInstance() {
	/*	Available Windows:
			- Win32Window (provides function to create and handle a Win32 Window)
	*/

#ifdef WIN32
	_Window = new Win32Window();
#else
	// Windows for other systems may be supported in the future, which would be created here
#endif

	return _Window;
}

AXLogger* AXBaseSystem::CreateLoggerInstance() {
	/*	Available Loggers:
			- AXLogger (handles four different levels of log messages, logs to screen and file)
	*/

	_Logger = new AXLogger;
	return _Logger;
}

AXKernel* AXBaseSystem::CreateKernelInstance() {
	/*	Available Kernels:
			- AXKernel (provides functions for adding, pausing, resuming, etc. updatable system components)
	*/

	_Kernel = new AXKernel;
	return _Kernel;
}
AXRenderer* AXBaseSystem::CreateRendererInstance() {
	/*	Available Renderers:
			- DirectXRenderer (provides rendering and associated functions using DirectX)

		OpenGL renderer is not yet supported, but possibly some time in the future
	*/

	if(AXBaseSystem::InstancePtr() == NULL) return NULL;

	//	This setting should already be validated
	std::string RendererName = AXBaseSystem::InstancePtr()->Settings.GetSetting<std::string>("renderer");
	if(RendererName == "opengl") {
		//	Hopefully will be supported sometime
	}
#ifdef WIN32
	else if(RendererName == "directx") {
		_Renderer = new DirectXRenderer();
	}
#endif

	return _Renderer;
}

AXInput* AXBaseSystem::CreateInputInstance() {
	/*	Available Input's:
			- DirectXInput (provides functions to query input using DirectInput)
	*/
#ifdef WIN32
	_Input = new DirectXInput();
#else
	// Input for other systems may be supported in the future, which would be created here
#endif 

	return _Input;
}

AXScene* AXBaseSystem::CreateSceneMgrInstance() {
	/*	Available SceneMgr's:
			- AXScene (provides simple functions to create a scene)
	*/
	_SceneMgr = new AXScene;
	return _SceneMgr;
}

AXClock* AXBaseSystem::CreateClockInstance() {
	/*	Available Clocks:
			- AXClock (provides a GetTime function and keeps track of time elapsed since last frame), uses Win32 functions
	*/
	_Clock = new AXClock;
	return _Clock;
}

AXResources* AXBaseSystem::CreateResourceMgrInstance() {
	/*	Available ResourceMgr's:
			- AXResources (provides management for Meshes)
	*/

	_ResourceMgr = new AXResources;
	return _ResourceMgr;
}
