/* --------------------------------------------------------------
	File:			AXClock.cpp
	Description:	Implementation for the AXClock class.  See AXClock.h for details.
	Date:			July 22, 2005
	Author:			James Long
-------------------------------------------------------------- */


#include "AXClock.h"
#include <windows.h>

float AXClock::dT = 0.0f;
float AXClock::ThisFrameTime = 0;
float AXClock::LastFrameTime = 0;

float AXClock::_TicksPerSecond;


AXClock::AXClock() : AXUpdatableSystemComponent("Clock") {
}
AXClock::~AXClock() {
}

AXResult AXClock::Start() {
	LARGE_INTEGER PFrequency;
	LARGE_INTEGER Time;
	QueryPerformanceFrequency(&PFrequency);
	QueryPerformanceCounter(&Time);

	_TicksPerSecond = (float)PFrequency.QuadPart;
	ThisFrameTime = (float)Time.QuadPart/_TicksPerSecond;
	dT = 0.000001f;

	return AXSUCCESS;
}

void AXClock::Update() {
	LARGE_INTEGER Time;
	QueryPerformanceCounter(&Time);
	LastFrameTime = ThisFrameTime;
	ThisFrameTime = (float)Time.QuadPart/_TicksPerSecond;
	dT = ThisFrameTime-LastFrameTime;
}

void AXClock::Stop() {
}

float AXClock::GetTime() {
	LARGE_INTEGER Time;
	QueryPerformanceCounter(&Time);
	return (float)Time.QuadPart/_TicksPerSecond;
}