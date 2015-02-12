/* --------------------------------------------------------------
	File:			AXWindow.cpp
	Description:	Implementation for the AXWindow class.  See AXWindow.h for details.
	Date:			June 29, 2005
	Author:			James Long
-------------------------------------------------------------- */

#include "..\\AXCore\\AXBaseSystem.h"
#include "..\\AXCore\\AXLogger.h"
#include "..\\AXGraphics\\AXRenderer.h"
#include "AXWindow.h"

void AXWindow::ValidateSettings() {
	int windowedWidth = Settings.GetSetting<int>("windowedWidth");
	if(windowedWidth <= 0)
		Settings.SetSetting("windowedWidth", 800);

	int windowedHeight = Settings.GetSetting<int>("windowedHeight");
	if(windowedHeight <= 0)
		Settings.SetSetting("windowedHeight", 600);

	int fullscreen = Settings.GetSetting<int>("fullscreen");
	if(fullscreen != 0 && fullscreen != 1)
		Settings.SetSetting("fullscreen", 0);

	if(Settings.GetSetting<int>("fullscreen")) {
		/*	Make sure the fullscreen resolution is valid. */	
		AXFullscreenMode Mode;
		Mode.Resolution.x = Settings.GetSetting<int>("fullscreenWidth");
		Mode.Resolution.y = Settings.GetSetting<int>("fullscreenHeight");
		Mode.RefreshRate = Settings.GetSetting<int>("fullscreenRefreshRate");

		if( AXFAILED(AXBaseSystem::GetRenderer()->IsResolutionSupported(Mode)) ) {
			/*	The resolution test failed. */
			LOG_SYS("Invalid fullscreen resolution!  Defaulting to 800x600 60Hz.");

			Settings.SetSetting("fullscreenWidth", 800);
			Settings.SetSetting("fullscreenHeight", 600);
			Settings.SetSetting("fullscreenRefreshRate", 60);
		}
	}
}

AXDimension2D AXWindow::GetScreenSize() {
	if(Settings.GetSetting<int>("fullscreen"))
		return AXDimension2D(Settings.GetSetting<int>("fullscreenWidth"), Settings.GetSetting<int>("fullscreenHeight"));
	else
		return AXDimension2D(Settings.GetSetting<int>("windowedWidth"), Settings.GetSetting<int>("windowedHeight"));
}




