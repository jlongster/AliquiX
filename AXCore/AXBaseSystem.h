/* --------------------------------------------------------------
	File:			AXBaseSystem.h
	Description:	This file defines an object that is the main controller of the program.  This
					is designed as a base object that should be derived to meet application-specific needs.
					Internal system tasks are done within the base class.

	Date:			July 3, 2005
	Author:			James Long
-------------------------------------------------------------- */

#if !defined(AXBASESYSTEM_INCLUDED)
#define AXBASESYSTEM_INCLUDED

#include "..\\AXCommon.h"
#include "..\\AXUtilities\\AXSingleton.h"
#include "..\\AXUtilities\\AXSettings.h"

class AXWindow;
class AXLogger;
class AXKernel;
class AXRenderer;
class AXInput;
class AXScene;
class AXClock;
class AXResources;

class AXBaseSystem : public AXSingleton<AXBaseSystem> {
public:
	AXBaseSystem();
	virtual ~AXBaseSystem();

	AXResult Initialize();
	void Run();
	void Shutdown();

	virtual AXResult OnInitialize() { return AXSUCCESS; }
	virtual void OnShutdown() {}
	virtual void OnLoadSettings() {}

	AXSettings Settings;
	void ValidateSettings();

	/*	System Components require a create and a get method */
	/*	System Components:
			- Window
			- Logger
			- Kernel
			- Resources
			- Renderer	(Updatable)
			- Input		(Updatable)
			- Scene		(Updatable)
			- Clock		(Updatable)

		An updatable system component means that it is attached to the kernel, which drives it
		on a per-frame basis (gives it a chance to update). */

	static AXWindow* CreateWindowInstance();
	static AXLogger* CreateLoggerInstance();
	static AXKernel* CreateKernelInstance();
	static AXRenderer* CreateRendererInstance();
	static AXInput* CreateInputInstance();
	static AXScene* CreateSceneMgrInstance();
	static AXClock* CreateClockInstance();
	static AXResources* CreateResourceMgrInstance();

	static AXWindow* GetWindow() { return _Window; }
	static AXLogger* GetLogger() { return _Logger; }
	static AXKernel* GetKernel() { return _Kernel; }
	static AXRenderer* GetRenderer() { return _Renderer; }
	static AXInput* GetInput() { return _Input; }
	static AXScene* GetSceneMgr() { return _SceneMgr; }
	static AXClock* GetClock() { return _Clock; }
	static AXResources* GetResourceMgr() { return _ResourceMgr; }

	
private:
	static AXWindow* _Window;
	static AXLogger* _Logger;
	static AXKernel* _Kernel;
	static AXRenderer* _Renderer;
	static AXInput* _Input;
	static AXScene* _SceneMgr;
	static AXClock* _Clock;
	static AXResources* _ResourceMgr;
};

/*	Helpful logging macros */
#define AXLog AXBaseSystem::GetLogger()

#define LOG_USER(x) AXLog->Write(MESSAGE_USER, x);
#define LOG_USER1(x,y) AXLog->Write(MESSAGE_USER, x, y);
#define LOG_USER2(x,y,z) AXLog->Write(MESSAGE_USER, x, y, z);
#define LOG_USER3(x,y,z,w) AXLog->Write(MESSAGE_USER, x, y, z, w);

#define LOG_APP(x) AXLog->Write(MESSAGE_USER, x);
#define LOG_APP1(x,y) AXLog->Write(MESSAGE_USER, x, y);
#define LOG_APP2(x,y,z) AXLog->Write(MESSAGE_USER, x, y, z);
#define LOG_APP3(x,y,z,w) AXLog->Write(MESSAGE_USER, x, y, z, w);

#define LOG_SYS(x) AXLog->Write(MESSAGE_SYS, x);
#define LOG_SYS1(x,y) AXLog->Write(MESSAGE_SYS, x, y);
#define LOG_SYS2(x,y,z) AXLog->Write(MESSAGE_SYS, x, y, z);
#define LOG_SYS3(x,y,z,w) AXLog->Write(MESSAGE_SYS, x, y, z, w);

#ifdef _DEBUG
#define LOG_DBG(x) AXLog->Write(MESSAGE_DBG, x);
#define LOG_DBG1(x,y) AXLog->Write(MESSAGE_DBG, x, y);
#define LOG_DBG2(x,y,z) AXLog->Write(MESSAGE_DBG, x, y, z);
#define LOG_DBG3(x,y,z,w) AXLog->Write(MESSAGE_DBG, x, y, z, w);
#else
#define LOG_DBG(x)
#define LOG_DBG1(x,y)
#define LOG_DBG2(x,y,z)
#define LOG_DBG3(x,y,z,w)
#endif

#endif
