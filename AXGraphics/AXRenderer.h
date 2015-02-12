/* --------------------------------------------------------------
	File:			AXRenderer.h
	Description:	This file defines an interface that is the rendering device of the program.

	Date:			August 1, 2005
	Author:			James Long
-------------------------------------------------------------- */

#if !defined(AXRENDERER_INCLUDED)
#define AXRENDERER_INCLUDED

#include "..\\AXCommon.h"
#include "..\\AXCommonStruct.h"
#include "..\\AXCore\\AXUpdatableSystemComponent.h"
#include "..\\AXUtilities\\AXMath.h"
#include "AXRenderStates.h"
#include "AXDeviceCapabilities.h"
#include "AXRenderQueue.h"
#include "AXLightEntity.h"
#include "AXColor.h"
#include "AXMaterial.h"
#include "AXMesh.h"

#include <vector>

/*	This specifies the on-screen back buffer */
static const unsigned int AXRENDERTARGET_DEFAULT = 0;

/*	Enum defining type of matrices */
enum MatrixType {
	AX_MATRIX_VIEW=1,
	AX_MATRIX_WORLD=2,
	AX_MATRIX_PROJECTION=4,
	AX_MATRIX_NORMAL=8
};

/*	Enum representing special buffers */
enum BufferType {
	AX_BUFFER_TARGET=1,
	AX_BUFFER_DEPTH=2
};

enum RenderQueues {
	RQ_DEFAULT=0
};


typedef std::vector<AXRenderQueue*> RenderQueueVector;


class AXRenderer : public AXUpdatableSystemComponent {
public:
	AXRenderer();
	virtual ~AXRenderer();

	/*	Functions dealing with whatever rendering device/method the API uses.
		These signal the API to either create a new one or destroy it. */
	virtual AXResult CreateDevice()=0;
	virtual void DestroyDevice()=0;
	
	/*	Process functions since this is an updatable system component */
	AXResult Start();
	void Update();
	void Stop();

	/*	These are the settings for the renderer (very important!).
		It's public which allows you to add/change settings at will.  The ValidateSettings()
		function will make sure everything is there in a correct format so nothing goes
		wrong when the renderer uses the settings. */
	AXSettings Settings;
	void ValidateSettings();

	/*	The rendering API must support some way of retrieving valid fullscreen modes
		so that we know how to validate them. */
	virtual AXResult GetResolutionList(std::vector<AXFullscreenMode> &ModeList)=0;
	virtual AXResult IsResolutionSupported(const AXFullscreenMode &Mode)=0;

	/*	This function simply interfaces to AXAppWindow's GetScreenSize function so that we don't have to be
		dependant on the application classes */
	AXDimension2D GetScreenSize();

	/*	Events that the renderer should care about */
	virtual void OnWindowChange()=0;  // Called when anything about the window changes (size, etc.)

	/*	Screenshot */
	virtual void Screenshot()=0;

	/*	Generic rendering functions */
	virtual void BeginRendering()=0;
	virtual void EndRendering()=0;
	virtual void ClearBuffers(const unsigned int Buffers, const AXColorRGB Color, const float Depth, const unsigned int Stencil)=0;
	virtual void FlipBuffers()=0;

	virtual void RenderPrimitives(AXPrimitiveType Type, unsigned int NumVertices)=0;
	virtual void RenderIndexedPrimitives(AXPrimitiveType Type, unsigned int NumVertices, unsigned int NumIndices)=0;

	virtual unsigned int GetRenderState(AXRenderState State)=0;
	virtual void SetRenderState(AXRenderState State, unsigned int Value)=0;

	/*	Hardware support */
	virtual unsigned int GetDeviceCapability(AXDeviceCapability Cap)=0;
	
	virtual void SetMaterial(AXMaterial *Material)=0;

	/*	Vertex and index management */
	virtual unsigned int CreateVertexBuffer()=0;
	virtual unsigned int CreateIndexBuffer()=0;
	virtual AXResult AllocateVertexBuffer(unsigned int ID, unsigned int Size)=0;
	virtual AXResult AllocateIndexBuffer(unsigned int ID, unsigned int Size, unsigned int NumBytes)=0;
	virtual void DestroyVertexBuffer(unsigned int ID)=0;
	virtual void DestroyIndexBuffer(unsigned int ID)=0;

	virtual void* LockVertexBuffer(unsigned int ID, unsigned int Offset, unsigned int Size)=0;
	virtual void* LockIndexBuffer(unsigned int ID, unsigned int Offset, unsigned int Size)=0;
	virtual void UnlockVertexBuffer(unsigned int ID)=0;
	virtual void UnlockIndexBuffer(unsigned int ID)=0;

	virtual void SetVertexSource(AXVertexBuffer *Buffer)=0;
	virtual void SetIndexSource(AXIndexBuffer *Buffer)=0;

	/*	Lights */
	virtual void SetLight(unsigned int Index, AXLightEntity *Light)=0;
	virtual void EnableLight(unsigned int Index)=0;
	virtual void DisableLight(unsigned int Index)=0;

	/*	Changes to properties */
	void SetClearColor(AXColorRGB Color) { _ClearColor = Color; }
	void SetClearDepth(float Depth) { _ClearDepth = Depth; }
	void SetClearStencil(unsigned int Stencil) { _ClearStencil = Stencil; }

	//virtual unsigned int CreateOffscreenRenderTarget()=0;
	//virtual void DestroyOffscreenRenderTarget(const unsigned int TargetID)=0;
	//virtual void UseRenderTarget(const unsigned int TargetID)=0;

	/*	Render queue operations */
	unsigned int CreateRenderQueue(unsigned int Priority);
	void AddToRenderQueue(AXMeshChunkEntity *ChunkEntity, unsigned int RenderQueueID = RQ_DEFAULT);

	/*	Matrix operations */
	virtual void LoadMatrix(const MatrixType Type, const AXMatrix4 *Mat)=0;

protected:
	/*	These are our own matrices the renderer should work with, and then convert them appropriately and load
		them in itself */
	AXMatrix4 _Matrix_World;
	AXMatrix4 _Matrix_View;
	AXMatrix4 _Matrix_Projection;

	/*	Keeps track of current state */
	AXColorRGB _ClearColor;
	float _ClearDepth;
	unsigned int _ClearStencil;

	/*	For efficiency, keep two arrays of the render queues.  One keeps the pointers
		stored in the same index as its ID for quick lookups.  The other keeps them
		sorted by priority for rendering. */
	RenderQueueVector _RenderQueues;
	RenderQueueVector _RenderQueuesSorted;

};

#endif