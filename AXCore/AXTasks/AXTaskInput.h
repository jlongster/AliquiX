/* --------------------------------------------------------------
	File:			AXTaskInput.h
	Description:	This file defines a task that handles all input to the application.  It drives
					the AXInput object created by the factory.

	Date:			July 22, 2005
	Author:			James Long
-------------------------------------------------------------- */

#if !defined(AXTASKINPUT_INCLUDE)
#define AXTASKINPUT_INCLUDE

class AXTaskInput : public AXTask {
public:
	AXTaskInput();
	virtual ~AXTaskInput();

	AXResult Start();
	void OnSuspend();
	void Update();
	void Stop();

private:
};


#endif