/* --------------------------------------------------------------
	File:			AXTaskClock.h
	Description:	This file defines a task that keeps track of time for the application

					It comes with several static functions and variables that you
					can call/read to query info on time.

	Date:			July 31, 2005
	Author:			James Long
-------------------------------------------------------------- */

#if !defined(AXTASKCLOCK_INCLUDE)
#define AXTASKCLOCK_INCLUDE


class AXTaskClock : public AXTask {
public:
	AXTaskClock();
	virtual ~AXTaskClock();

	AXResult Start();
	void Update();
	void Stop();


	static float dT;
	static float ThisFrameTime;
	static float LastFrameTime;
	static float GetTime();

private:
	static float TicksPerSecond;
};


#endif