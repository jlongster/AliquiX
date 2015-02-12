/* --------------------------------------------------------------
	File:			SceneApp.h
	Description:	Defines an application that tests the AliquiX framework.

	Date:			May 30, 2006
	Author:			James Long
-------------------------------------------------------------- */

#if !defined SCENEAPP_INCLUDED
#define SCENEAPP_INCLUDED

#include "AXCore.h"

class SceneApp : public AXBaseSystem {
public:
	SceneApp();
	virtual ~SceneApp();

	/*	The following two functions are called when the system is
		initializing */
	void OnLoadSettings();
	AXResult OnInitialize();

private:
	void _CreateGeometry();
	AXResult _CreateScene();
};

#endif
