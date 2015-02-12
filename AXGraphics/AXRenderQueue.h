/* --------------------------------------------------------------
	File:			AXRenderQueue.h
	Description:	This object enables the system to queue rendering requests and render the whole scene all at one time.
					This way it is able to sort and render things efficiently.

	Date:			January 11, 2006
	Author:			James Long
-------------------------------------------------------------- */

#if !defined(AXRENDERQUEUE_INCLUDED)
#define AXRENDERQUEUE_INCLUDED

#include "AXMeshChunkEntity.h"

#include <vector>

class AXRenderQueue {
	friend class AXRenderer;

public:
	AXRenderQueue();
	virtual ~AXRenderQueue();

	void Queue(AXMeshChunkEntity *MeshChunk);
	void Execute();

private:
	unsigned int _Priority;

	typedef std::vector< AXMeshChunkEntity* > MeshChunkVector;
	MeshChunkVector _RenderQueue;
};

#endif