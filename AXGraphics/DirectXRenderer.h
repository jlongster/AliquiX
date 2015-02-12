/* --------------------------------------------------------------
	File:			DirectXRenderer.h
	Description:	This file defines a class that is the rendering device of the program (using directx).

					This object will grow with time.  For now it is rather simple.  It simply
					initializes the API and provides some basic rendering functions.

	Date:			November 15, 2005
	Author:			James Long
-------------------------------------------------------------- */

#if !defined(DIRECTXRENDERER_INCLUDED)
#define DIRECTXRENDERER_INCLUDED

#include "AXRenderer.h"

#ifdef _DEBUG
#define D3D_DEBUG_INFO
#endif

#include <d3d9.h>
#include <d3dx9.h>

typedef std::map<unsigned int, IDirect3DVertexDeclaration9*> UIntVertexDeclarationMap;

class DirectXRenderer : public AXRenderer {
public:
	DirectXRenderer();
	virtual ~DirectXRenderer();

	AXResult Initialize();
	void Shutdown();

	/*	Device functions */
	AXResult CreateDevice();
	void DestroyDevice();
	IDirect3DDevice9* GetDevice() { return _D3DDevice; }

	AXResult GetResolutionList(std::vector<AXFullscreenMode> &ModeList);
	AXResult IsResolutionSupported(const AXFullscreenMode &Mode);

	void OnWindowChange();

	void Screenshot();

	void BeginRendering();
	void EndRendering();
	void ClearBuffers(const unsigned int Buffers, const AXColorRGB Color, const float Depth, const unsigned int Stencil);
	void FlipBuffers();

	void RenderPrimitives(AXPrimitiveType Type, unsigned int NumVertices);
	void RenderIndexedPrimitives(AXPrimitiveType Type, unsigned int NumVertices, unsigned int NumIndices);

	unsigned int GetRenderState(AXRenderState State);
	void SetRenderState(AXRenderState State, unsigned int Value);

	unsigned int GetDeviceCapability(AXDeviceCapability Cap);

	void SetMaterial(AXMaterial *Material);

	/*	Vertex and index management */
	unsigned int CreateVertexBuffer();
	unsigned int CreateIndexBuffer();
	AXResult AllocateVertexBuffer(unsigned int ID, unsigned int Size);
	AXResult AllocateIndexBuffer(unsigned int ID, unsigned int Size, unsigned int NumBytes);
	void DestroyVertexBuffer(unsigned int ID);
	void DestroyIndexBuffer(unsigned int ID);

	void* LockVertexBuffer(unsigned int ID, unsigned int Offset, unsigned int Size);
	void* LockIndexBuffer(unsigned int ID, unsigned int Offset, unsigned int Size);
	void UnlockVertexBuffer(unsigned int ID);
	void UnlockIndexBuffer(unsigned int ID);

	void SetVertexSource(AXVertexBuffer *Buffer);
	void SetIndexSource(AXIndexBuffer *Buffer);

	/*	Lights */
	void SetLight(unsigned int Index, AXLightEntity *Light);
	void EnableLight(unsigned int Index);
	void DisableLight(unsigned int Index);

	//unsigned int CreateOffscreenRenderTarget();
	//void DestroyOffscreenRenderTarget(const unsigned int TargetID);
	//void UseRenderTarget(const unsigned int TargetID);

	/*	Matrix operations */
	void LoadMatrix(const MatrixType Type, const AXMatrix4 *Mat);


private:

	/* DX specific device functions */
	AXResult _ResetDevice();

	D3DFORMAT _GetCurrentAdapterFormat();
	D3DFORMAT _GetBestBackBufferFormat();
	D3DFORMAT _GetBestDepthStencilFormat();
    AXResult _IsBackBufferFormatAvailable(D3DFORMAT AdapterFormat, D3DFORMAT BackBufferFormat);
	AXResult _IsDepthStencilFormatAvailable(D3DFORMAT AdapterFormat, D3DFORMAT DepthStencilFormat);
	AXResult _IsDepthStencilFormatCompatible(D3DFORMAT AdapterFormat, D3DFORMAT RenderFormat, D3DFORMAT DepthStencilFormat);
	AXResult _GetPresentationParam(D3DPRESENT_PARAMETERS *Present_Param);
	void _SetDXDefaults();

	/*	Internal states and objects */
	void _FreeInternalResources();

	IDirect3DVertexDeclaration9* _CreateVertexDeclaration(AXVertexBuffer *Buffer);

	UIntVertexDeclarationMap _VertexDeclarations;

	struct DirectXVertexBuffer {
		IDirect3DVertexBuffer9 *VertexBuffer;
		unsigned char InUse;
	};
	struct DirectXIndexBuffer {
		IDirect3DIndexBuffer9 *IndexBuffer;
		unsigned char InUse;
	};

	std::vector<DirectXVertexBuffer> _VertexBuffers;
	std::vector<DirectXIndexBuffer> _IndexBuffers;

	unsigned int _IndexBufferID;
	unsigned int _IndexBufferOffset;

	unsigned int _SetRenderStates[AX_MAXRENDERSTATES];

	/*	DirectX, device, and properties */
	IDirect3D9* _D3D;
	IDirect3DDevice9* _D3DDevice;
	D3DDEVTYPE _DeviceType;
	D3DFORMAT _BackBufferFormat;
	D3DFORMAT _DepthStencilFormat;

	D3DCAPS9 _D3DCaps;

};

#endif