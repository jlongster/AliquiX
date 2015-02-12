/* --------------------------------------------------------------
	File:			AXTaskRender.h
	Description:	This file defines a task that handles all the actual rendering.

					It handles 2d and 3d rendering separately.  For each rendering process,
					it creates a queue that is built as the scene updates.  Once all the
					processing is done for the scene, the Render Task hits off and
					actually renders everything (fired off in the Update function).

	Date:			August 3, 2005
	Author:			James Long
-------------------------------------------------------------- */

#if !defined(AXTASKRENDER_INCLUDE)
#define AXTASKRENDER_INCLUDE


class AXTaskRender : public AXTask {
public:
	AXTaskRender();
	virtual ~AXTaskRender();

	AXResult Start();
	void Update();
	void Stop();

private:	

};


#endif