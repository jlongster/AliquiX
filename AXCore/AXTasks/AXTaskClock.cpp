/* --------------------------------------------------------------
	File:			AXTaskClock.cpp
	Description:	Implementation for the AXTaskClock class.  See AXTaskClock.h for details.
	Date:			July 22, 2005
	Author:			James Long
-------------------------------------------------------------- */


#include "..\\..\\AXCore.h"
#include <windows.h>

float AXTaskClock::dT = 0.0f;
float AXTaskClock::ThisFrameTime = 0;
float AXTaskClock::LastFrameTime = 0;

float AXTaskClock::TicksPerSecond;


AXTaskClock::AXTaskClock() : AXTask() {
}
AXTaskClock::~AXTaskClock() {
}

AXResult AXTaskClock::Start() {
	LARGE_INTEGER PFrequency;
	LARGE_INTEGER Time;
	QueryPerformanceFrequency(&PFrequency);
	QueryPerformanceCounter(&Time);

	TicksPerSecond = (float)PFrequency.QuadPart;
	ThisFrameTime = (float)Time.QuadPart/TicksPerSecond;
	dT = 0.000001f;

	return AXSUCCESS;
}

void AXTaskClock::Update() {
	LARGE_INTEGER Time;
	QueryPerformanceCounter(&Time);
	LastFrameTime = ThisFrameTime;
	ThisFrameTime = (float)Time.QuadPart/TicksPerSecond;
	dT = ThisFrameTime-LastFrameTime;
}

void AXTaskClock::Stop() {
}

float AXTaskClock::GetTime() {
	LARGE_INTEGER Time;
	QueryPerformanceCounter(&Time);
	return (float)Time.QuadPart/TicksPerSecond;
}