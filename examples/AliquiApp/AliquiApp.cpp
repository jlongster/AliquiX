/* --------------------------------------------------------------
	File:			AliquiX.cpp
	Description:	The main entry point for the AliquiX application.
					This should only include a main function that creates an
					application and runs it.
	Date:			May 11, 2006
	Author:			James Long
-------------------------------------------------------------- */

#include "AliquiX.h"
#include "SceneApp.h"
#include <crtdbg.h>

int main(int argc, char* argv[]) {
	SceneApp MainApp;

	//_CrtSetBreakAlloc(817);

	if( AXFAILED(MainApp.Initialize()) ) {
		LOG_USER("The application failed to initialize.  Please read the above information for errors.");
		MainApp.Shutdown();
		return 1;
	}

	MainApp.Run();
	MainApp.Shutdown();

	return 0;
}

