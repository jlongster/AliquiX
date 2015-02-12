/* --------------------------------------------------------------
	File:			AXLogger.cpp
	Description:	Implementation for the AXLogger class.  See AXLogger.h for details.
	Date:			June 29, 2005
	Author:			James Long
-------------------------------------------------------------- */

#include "AXLogger.h"

AXLogger::AXLogger() : AXSystemComponent("Logger") {
}
AXLogger::~AXLogger() {
}

AXResult AXLogger::Initialize() {

	Settings.ParseSettings("logger");
	ValidateSettings();

	// Open the log file(s)
	_FileLog.open("log.txt");

#ifdef _DEBUG
	_ProfileLog.open("logProfile.txt");
#endif

	ValidateSettings();

	return AXSUCCESS;
}

void AXLogger::ValidateSettings() {
	int logApplication = Settings.GetSetting<int>("logApplicationMessages");
	if(logApplication != 0 || logApplication != 1)
		Settings.SetSetting("logApplicationMessages", 1);

	int logSystem = Settings.GetSetting<int>("logSystemMessages");
	if(logSystem != 0 || logApplication != 1)
		Settings.SetSetting("logSystemMessages", 1);
}

void AXLogger::Shutdown() {
	// Close the log file(s)
	_FileLog.close();

#ifdef _DEBUG
	_ProfileLog.close();
#endif

	Settings.WriteSettings();

}

void AXLogger::Write(int target, const char *FunctionName, int LineNumber, const char* Msg, ...) {
	va_list args;

	va_start(args, Msg);
	vsprintf(_Temp_Buf, Msg, args);
	va_end(args);

	// Detect the log type and log accordingly
	if(target & LOG_TYPE_USER) {
		printf("AX User (%s %d)> %s\n", FunctionName, LineNumber, _Temp_Buf);
		_FileLog << "\n Dear user: " << _Temp_Buf << "\n\n";
#ifdef _DEBUG
		_FileLog.flush();
#endif
	}

	if(target & LOG_TYPE_APPLICATION && Settings.GetSetting<int>("logApplicationMessages")) {
		printf("AX Application (%s %d)> %s\n", FunctionName, LineNumber, _Temp_Buf);
		_FileLog << "AX Application (" << FunctionName << " " << LineNumber<< ")> " << _Temp_Buf << "\n";
#ifdef _DEBUG
		_FileLog.flush();
#endif
	}

	if(target & LOG_TYPE_SYSTEM && Settings.GetSetting<int>("logSystemMessages")) {
		printf("AX System (%s %d)> %s\n", FunctionName, LineNumber, _Temp_Buf);
		_FileLog << "AX System (" << FunctionName << " " << LineNumber << ")> " << _Temp_Buf << "\n";
#ifdef _DEBUG
		_FileLog.flush();
#endif
	}

	if(target & LOG_TYPE_DEBUG) {
#ifdef _DEBUG
		printf("*** AX Debug (%s %d)> %s\n", FunctionName, LineNumber, _Temp_Buf);
		_FileLog << "AX Debug (" << FunctionName << " " << LineNumber << ")> " << _Temp_Buf << "\n";
		_FileLog.flush();
#endif
	}

	if(target & LOG_TYPE_PROFILE) {
#ifdef _DEBUG
		_ProfileLog << _Temp_Buf << "\n";
#endif
	}

}
