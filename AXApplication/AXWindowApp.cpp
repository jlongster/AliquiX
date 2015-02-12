/* --------------------------------------------------------------
	File:			AXWindowApp.cpp
	Description:	Implementation for the AXWindowApp class.  See AXWindowApp.h for details.
	Date:			June 29, 2005
	Author:			James Long
-------------------------------------------------------------- */

#include "..\\AXApplication.h"
#include "..\\AXCore.h"
#include "..\\AXGraphics.h"
#include <crtdbg.h>

/*	Global Application Objects	*/
AXWindow* const AXAppWindow = new AXWindow;
AXLogger* const AXAppLog = new AXLogger;
AXSettings* const AXAppSettings = new AXSettings;
AXKernel* const AXAppKernel = new AXKernel;
AXRenderer* const AXAppRenderer = new AXRenderer;

AXWindowApp::AXWindowApp() {
}
AXWindowApp::~AXWindowApp() {
}

AXResult AXWindowApp::Initialize() {
#ifdef _DEBUG
	/*	Check for memory leaks at the end */
	int tempflag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
	tempflag |= _CRTDBG_LEAK_CHECK_DF;
	_CrtSetDbgFlag(tempflag);
#endif

	/*	Initialize the logger */
	AXAppLog->Initialize();
	AXAppLog->Write(MESSAGE_SYS, "AliquiX Started ~ " __TIMESTAMP__);

	/*	Load the settings */
	AXAppLog->Write(MESSAGE_SYS, "Loading settings...");
	AXAppSettings->ParseFile("settings.txt", SETTING_USER);
	OnLoadSettings();
	ValidateSettings();
	

	/*	Create the window */
	AXAppLog->Write(MESSAGE_SYS, "Creating the window...");
	if( AXFAILED(AXAppWindow->Create()) ) {
		AXAppLog->Write(LOG_TYPE_USER | MESSAGE_SYS, "Could not create the window!  See the log for details.");
		return AXFAILURE;
	}

	/*	Initialize the renderer */
	AXAppLog->Write(MESSAGE_SYS, "Initializing the renderer...");
	if( AXFAILED(AXAppRenderer->Initialize()) ) {
		AXAppLog->Write(LOG_TYPE_USER | MESSAGE_SYS, "An error occurred while initializing the renderer!  See log for details.");
		return AXFAILURE;
	}

	/*	Initialize the system tasks */
	shared_ptr<AXTask> Task;
	
	Task = shared_ptr<AXTask>(new AXTaskInput);
	Task->Name = "Input";
	Task->Priority = 25;
	if( AXFAILED(AXAppKernel->AddTask(Task)) )
		return AXFAILURE;

	Task = shared_ptr<AXTask>(new AXTaskClock);
	Task->Name = "Clock";
	Task->Priority = 20;
	if( AXFAILED(AXAppKernel->AddTask(Task)) )
		return AXFAILURE;

	Task = shared_ptr<AXTask>(new AXTaskAppState);
	Task->Name = "AppStates";
	Task->Priority = 500;
	if( AXFAILED(AXAppKernel->AddTask(Task)) )
		return AXFAILURE;

	Task = shared_ptr<AXTask>(new AXTaskRender);
	Task->Name = "Renderer";
	Task->Priority = 1000;
	if( AXFAILED(AXAppKernel->AddTask(Task)) )
		return AXFAILURE;

	/*	Call user-defined event handlers */
	if( AXFAILED(OnInitialize()) ) {
		AXAppLog->Write(LOG_TYPE_USER | MESSAGE_SYS, "User defined setup failed!  See log for details.");
		return AXFAILURE;
	}

	return AXSUCCESS;
}

void AXWindowApp::Run() {
	AXAppKernel->Execute();
}

void AXWindowApp::Shutdown() {

	OnShutdown();

	AXAppRenderer->Shutdown();
	delete AXAppRenderer;

	AXAppLog->Write(MESSAGE_SYS, "Destroying the window...");
	AXAppWindow->Destroy();
	delete AXAppWindow;

	delete AXAppKernel;

	AXAppLog->Write(MESSAGE_SYS, "Saving the settings...");
	AXAppSettings->WriteToFile("settings.txt", SETTING_USER);
	delete AXAppSettings;

	AXAppLog->Shutdown();
	delete AXAppLog;
}

void AXWindowApp::ValidateSettings() {
	
	int windowedWidth = AXAppSettings->GetSetting<int>("windowedWidth");
	if(windowedWidth <= 0)
		AXAppSettings->RegisterSetting("windowedWidth", 800, SETTING_USER);

	int windowedHeight = AXAppSettings->GetSetting<int>("windowedHeight");
	if(windowedHeight <= 0)
		AXAppSettings->RegisterSetting("windowedHeight", 600, SETTING_USER);

	int fullscreenWidth = AXAppSettings->GetSetting<int>("fullscreenWidth");
	if(fullscreenWidth <= 0)
		AXAppSettings->RegisterSetting("fullscreenWidth", 800, SETTING_USER);

	int fullscreenHeight = AXAppSettings->GetSetting<int>("fullscreenHeight");
	if(fullscreenHeight <= 0)
		AXAppSettings->RegisterSetting("fullscreenHeight", 600, SETTING_USER);

	int depthBpp = AXAppSettings->GetSetting<int>("depthBpp");
	if(depthBpp != 16 && depthBpp != 32 && depthBpp != -1)
		AXAppSettings->RegisterSetting("depthBpp", 32, SETTING_USER);

	int screenBpp = AXAppSettings->GetSetting<int>("screenBpp");
	if(screenBpp != 16 && screenBpp != 32 && screenBpp != -1)
		AXAppSettings->RegisterSetting("screenBpp", 32, SETTING_USER);

	int fullscreen = AXAppSettings->GetSetting<int>("fullscreen");
	if(fullscreen < 0)
		AXAppSettings->RegisterSetting("fullscreen", 0, SETTING_USER);

	OnValidateSettings();
}
