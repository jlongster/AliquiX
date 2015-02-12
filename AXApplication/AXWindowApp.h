/* --------------------------------------------------------------
	File:			AXWindowApp.h
	Description:	This file defines an object that is the main controller of the program.  This
					is designed as a base object that should be derived to meet application-specific needs.
					Internal system tasks are done within the base class.
	Date:			July 3, 2005
	Author:			James Long
-------------------------------------------------------------- */

#if !defined(AXWINDOWAPP_INCLUDED)
#define AXWINDOWAPP_INCLUDED

class AXWindowApp : AXSingleton<AXWindowApp> {
public:
	AXWindowApp();
	virtual ~AXWindowApp();

	AXResult Initialize();
	void Run();
	void Shutdown();
	
	void ValidateSettings();

	virtual AXResult OnInitialize() {return AXSUCCESS;}
	virtual void OnLoadSettings() {}
	virtual void OnValidateSettings() {}
	virtual void OnRun() {}
	virtual void OnShutdown() {}

private:
};

#endif