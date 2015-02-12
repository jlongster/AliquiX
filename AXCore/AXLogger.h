/* --------------------------------------------------------------
	File:			AXLogger.h
	Description:	This file defines a class that logs messages.  Depending on the type of message,
					it logs it to the appropriate output.
					For now, it logs everything to both the console and the file.  I think it's important
					to separate the types of messages, however, so later I can come back and change the type of
					outputs for each type of message.

					You specify the type of message by passing in MESSAGE_USER, MESSAGE_APP, or MESSAGE_SYSTEM
					when you write the message.  A line with the type of message, calling function, and message
					will appear in the appropriate output.  You are allowed printf style formatting.

					If you want to combine message targets, you have to use the LOG_TYPE_* type to specify the ouputs,
					but the last output channel should use the MESSAGE_* macro.  This is because the MESSAGE_* macros includes
					other parameters that the Write function requires.

					7/30/05	- Added MESSAGE_PROFILE message type

	Date:			June 29, 2005
	Author:			James Long
-------------------------------------------------------------- */


#if !defined(AXLOGGER_INCLUDE)
#define AXLOGGER_INCLUDE

#include "AXSystemComponent.h"

#include <fstream>
#include <stdarg.h>

// The different log types
enum LogType {
	LOG_TYPE_USER = 1,
	LOG_TYPE_APPLICATION = 2,
	LOG_TYPE_SYSTEM = 4,
	LOG_TYPE_DEBUG = 8,
	LOG_TYPE_PROFILE = 16
};

// Define some macros to make logging much easier
#define MESSAGE_USER	LOG_TYPE_USER, __FUNCTION__, __LINE__
#define MESSAGE_APP		LOG_TYPE_APPLICATION, __FUNCTION__, __LINE__
#define MESSAGE_SYS		LOG_TYPE_SYSTEM, __FUNCTION__, __LINE__
#define MESSAGE_DBG		LOG_TYPE_DEBUG, __FUNCTION__, __LINE__
#define MESSAGE_PROFILE	LOG_TYPE_PROFILE, "", 0

class AXLogger : public AXSystemComponent {
public:
	AXLogger();
	virtual ~AXLogger();

	AXResult Initialize();
	void Shutdown();

	void Write(int target, const char* FunctionName, int LineNumber, const char* Msg, ...);

	void ValidateSettings();

private:
	char _Temp_Buf[5012];
	std::ofstream _FileLog;
	std::ofstream _ProfileLog;
};

#endif

