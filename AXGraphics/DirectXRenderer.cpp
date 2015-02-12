/* --------------------------------------------------------------
	File:			DirectXRenderer.cpp
	Description:	Implementation for the DirectXRenderer class.  See DirectXRenderer.h for details.
	Date:			August 1, 2005
	Author:			James Long
-------------------------------------------------------------- */

#include "..\\AXCore.h"
#include "..\\AXApplication.h"
#include "..\\AXUtilities\\AXMath.h"
#include "..\\AXUtilities\\AXStringFunctions.h"
#include "..\\AXApplication\\Win32Window.h"
#include "AXResources.h"

#include "DirectXRenderer.h"

#include <dxerr9.h>
#include <string>
#include <crtdbg.h>

DirectXRenderer::DirectXRenderer() : AXRenderer(), _D3D(NULL), _D3DDevice(NULL) {
}
DirectXRenderer::~DirectXRenderer() {
}

AXResult DirectXRenderer::Initialize() {
	if(_D3D) {
		LOG_SYS("Failure to init, already initialized!");
		return AXFAILURE;
	}

	Settings.ParseSettings("renderer");
	ValidateSettings();

	LOG_SYS("Initializing Direct3d...");
	if( (_D3D = Direct3DCreate9( D3D_SDK_VERSION )) == NULL ) {
		LOG_SYS("Creating D3D failed!");
		return AXFAILURE;
	}

	for(unsigned int i=0; i<AX_MAXRENDERSTATES; i++) {
		_SetRenderStates[i] = (unsigned int)-1;
	}

	return AXSUCCESS;
}

void DirectXRenderer::Shutdown() {
	SafeRelease(_D3D);

	Settings.WriteSettings();
}

AXResult DirectXRenderer::CreateDevice() {
	if(_D3DDevice) return AXSUCCESS;

	LOG_SYS("Creating Direct3d device...");

    D3DPRESENT_PARAMETERS D3DPresParam;
	std::string DevType = Settings.GetSetting<std::string>("dxDeviceType");
	int VertexProcessingType = 0;

	/*	Get the appropriate device type according the settings */
	if(DevType == "ref")
		_DeviceType = D3DDEVTYPE_REF;
	else
		_DeviceType = D3DDEVTYPE_HAL;

	/*	Fill in the device caps */
	_D3D->GetDeviceCaps(D3DADAPTER_DEFAULT, _DeviceType, &_D3DCaps);

	/*	Fill in the presentation parameter structure will all the correct values
		according to settings, supported formats etc.
		This also sets the internal format variables of this renderer, so we know
		the formats of buffers later on */
	if( AXFAILED(_GetPresentationParam(&D3DPresParam)) ) {
		LOG_SYS("An error occurred while getting presentation parameters!");
		return AXFAILURE;
	}

	/*	Figure out if we can do hardware processing, or if we must do software.
		We have to call GetDeviceCaps to figure this out. */
	if(_D3DCaps.VertexProcessingCaps != 0) {
		LOG_SYS("Using hardware vertex processing...");
		VertexProcessingType |= D3DCREATE_HARDWARE_VERTEXPROCESSING;
	}
	else {
		LOG_SYS("Using software vertex processing...");
		VertexProcessingType |= D3DCREATE_SOFTWARE_VERTEXPROCESSING;
	}

	/*	Actually create the device with all the parameters that we've made. */
	HRESULT hr;

	/*	If we are using DirectX, we know that we are on a Win32 machine.  We need to get the HWND of the window,
		and very carefully send the AXAppWindow object down the class heirarchy */
	Win32Window *Window = dynamic_cast<Win32Window*>(AXBaseSystem::GetWindow());
	if(!Window) {
		LOG_SYS("A serious error has occurred while trying to retrieve the Win32 window.");
		return AXFAILURE;
	}

	hr = _D3D->CreateDevice(D3DADAPTER_DEFAULT, _DeviceType, Window->GetHWND(), VertexProcessingType, &D3DPresParam, &_D3DDevice);
	if(FAILED(hr)) {
		LOG_SYS1("Cannot create device! (%s)", DXGetErrorString9(hr));
		return AXFAILURE;
	}
	LOG_SYS("Creating Direct3d device successful!");

	this->_IndexBufferID = (unsigned int)-1;
	this->_IndexBufferOffset = 0;

	_SetDXDefaults();


	return AXSUCCESS;
}

void DirectXRenderer::DestroyDevice() {
	if(!_D3DDevice)
		return;

	_FreeInternalResources();

	LOG_SYS("Releasing the rendering device...");
	SafeRelease(_D3DDevice);
}

AXResult DirectXRenderer::_ResetDevice() {
	LOG_SYS("Resetting Direct3d device...");
	if(!_D3DDevice) {
		LOG_SYS("Cannot reset device, not created yet!");
		return AXFAILURE;
	}

	_FreeInternalResources();

    D3DPRESENT_PARAMETERS D3DPresParam; 
	if( AXFAILED(_GetPresentationParam(&D3DPresParam)) ) {
		LOG_SYS("An error occurred while getting presentation parameters!");
		return AXFAILURE;
	}

	HRESULT hr;
	hr = _D3DDevice->Reset(&D3DPresParam);
	if(FAILED(hr)) {
		LOG_SYS1("Failed to reset the device: %s", DXGetErrorString9(hr));
		return AXFAILURE;
	}
	LOG_SYS("Resetting Direct3d device successful!");

	/*	Tell the resource manager that is can request things again */
	LOG_SYS("Resetting resources...");
	if(AXFAILED(AXBaseSystem::GetResourceMgr()->OnReset())) {
		LOG_SYS("Failed resetting resources!");
		return AXFAILURE;
	}

	_SetDXDefaults();

	return AXSUCCESS;
}

AXResult DirectXRenderer::GetResolutionList(std::vector<AXFullscreenMode> &ModeList) {
	D3DFORMAT AdapterFormat = _GetCurrentAdapterFormat();
	D3DDISPLAYMODE DisplayInformation;
	AXFullscreenMode Mode;

	ModeList.clear();
	unsigned int NumModes = _D3D->GetAdapterModeCount(D3DADAPTER_DEFAULT, AdapterFormat);
	for(unsigned int i=0; i<NumModes; i++) {
		if( FAILED(_D3D->EnumAdapterModes(D3DADAPTER_DEFAULT, AdapterFormat, i, &DisplayInformation)) )
			return AXFAILURE;

		Mode.RefreshRate = DisplayInformation.RefreshRate;
		Mode.Resolution = AXDimension2D(DisplayInformation.Width, DisplayInformation.Height);

		ModeList.push_back(Mode);
	}

	return AXSUCCESS;
}

AXResult DirectXRenderer::IsResolutionSupported(const AXFullscreenMode &Mode) {
	std::vector<AXFullscreenMode> ModeList;
	
	if( AXFAILED(GetResolutionList(ModeList)) )
		return AXFAILURE;

	for(unsigned int i=0; i<ModeList.size(); i++) {
		if(ModeList[i].Resolution.x == Mode.Resolution.x && ModeList[i].Resolution.y == Mode.Resolution.y && ModeList[i].RefreshRate == Mode.RefreshRate) {
				return AXSUCCESS;
		}
	}

	return AXFAILURE;
}

void DirectXRenderer::_FreeInternalResources() {
	/*	Free resources */
	for(unsigned int i=0; i<(unsigned int)_VertexBuffers.size(); i++) {
		DestroyVertexBuffer(i);
	}
	_VertexBuffers.clear();
	for(unsigned int i=0; i<(unsigned int)_IndexBuffers.size(); i++) {
		DestroyIndexBuffer(i);
	}
	_IndexBuffers.clear();
	
	UIntVertexDeclarationMap::iterator it;
	for(it = _VertexDeclarations.begin(); it != _VertexDeclarations.end(); it++) {
		SafeRelease(it->second);
	}
	_VertexDeclarations.clear();

	this->_IndexBufferID = (unsigned int)-1;
	this->_IndexBufferOffset = 0;
}

/* -----------------------------------
	Events
----------------------------------- */
void DirectXRenderer::OnWindowChange() {
	if(!_D3DDevice)
		return;
	
	//	Some properties of the window have changed,
	//	so we need to reset the device
	if(AXFAILED(_ResetDevice())) {
		LOG_SYS("An error occurred while resetting the device!");
	}
}

void DirectXRenderer::Screenshot() {
	if(!_D3DDevice)
		return;

	LOG_SYS("Taking screenshot...");

	AXDimension2D ScreenSize;
	IDirect3DSurface9* BackBuffer;

	ScreenSize = GetScreenSize();
	//_D3DDevice->CreateOffscreenPlainSurface(ScreenSize.x, ScreenSize.y, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &BackBuffer, NULL);

	HRESULT hr = _D3DDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &BackBuffer);

	if(FAILED(hr))	{
		LOG_SYS("Failed taking screenshot!");
		SafeRelease(BackBuffer);
		return;
	}

	int Index = Settings.GetSetting<int>("screenshotIndex") + 1;
	Settings.SetSetting("screenshotIndex", Index);

	std::string FileName = std::string("Screenshot") + AXStringConvertFromInt(Index) + std::string(".jpg");
	D3DXSaveSurfaceToFile(FileName.c_str(), D3DXIFF_BMP, BackBuffer, NULL, NULL);

	SafeRelease(BackBuffer);
}

/* -----------------------------------
	Rendering functions and data functions
----------------------------------- */

void DirectXRenderer::RenderPrimitives(AXPrimitiveType Type, unsigned int NumVertices) {
	D3DPRIMITIVETYPE DXType;
	unsigned int NumPrimitives = 0;

	if(Type == AX_PRIM_TRIANGLES) {
		DXType = D3DPT_TRIANGLELIST;
		NumPrimitives = NumVertices/3;
	}
	else if(Type == AX_PRIM_TRIANGLESTRIP) {
		DXType = D3DPT_TRIANGLESTRIP;
		NumPrimitives = NumVertices-2;
	}
	else if(Type == AX_PRIM_LINES) {
		DXType = D3DPT_LINELIST;
		NumPrimitives = NumVertices/2;
	}
	else if(Type == AX_PRIM_LINESTRIP) {
		DXType = D3DPT_LINELIST;
		NumPrimitives = NumVertices-1;
	}
	// Other types aren't supported yet

	_D3DDevice->DrawPrimitive(DXType, 0, NumPrimitives);
}

void DirectXRenderer::RenderIndexedPrimitives(AXPrimitiveType Type, unsigned int NumVertices, unsigned int NumIndices) {
	D3DPRIMITIVETYPE DXType;
	unsigned int NumPrimitives = 0;

	if(Type == AX_PRIM_TRIANGLES) {
		DXType = D3DPT_TRIANGLELIST;
		NumPrimitives = NumIndices/3;
	}
	else if(Type == AX_PRIM_TRIANGLESTRIP) {
		DXType = D3DPT_TRIANGLESTRIP;
		NumPrimitives = NumIndices-2;
	}

	HRESULT hr = _D3DDevice->DrawIndexedPrimitive(DXType, 0, 0, NumVertices, _IndexBufferOffset, NumPrimitives);
	if(FAILED(hr)) {
		LOG_SYS1("DrawIndexedPrimitive failed! Error: %s", DXGetErrorString9(hr));
	}
}

void DirectXRenderer::BeginRendering() {
	_D3DDevice->BeginScene();
}

void DirectXRenderer::EndRendering() {
	_D3DDevice->EndScene();
}

void DirectXRenderer::ClearBuffers(const unsigned int Buffers, const AXColorRGB Color, const float Depth, const unsigned int Stencil) {
	unsigned int BufferFlags = 0;
	if(Buffers & AX_BUFFER_TARGET) BufferFlags |= D3DCLEAR_TARGET;
	if(Buffers & AX_BUFFER_DEPTH) BufferFlags |= D3DCLEAR_ZBUFFER;
	_D3DDevice->Clear(0, NULL, BufferFlags, D3DCOLOR_XRGB(AXColorR(Color), AXColorG(Color), AXColorB(Color)), Depth, Stencil);
}

void DirectXRenderer::FlipBuffers() {
	AXPROFILE("Flip buffers");
	HRESULT hr;
	hr = _D3DDevice->Present(NULL, NULL, NULL, NULL);
	if(FAILED(hr)) {
		hr = _D3DDevice->TestCooperativeLevel();
		if(hr == D3DERR_DEVICENOTRESET) _ResetDevice();
	}
}

void DirectXRenderer::LoadMatrix(const MatrixType Type, const AXMatrix4 *Mat) {
	switch(Type) {
		case AX_MATRIX_VIEW: _D3DDevice->SetTransform(D3DTS_VIEW, Mat); break;
		case AX_MATRIX_WORLD: _D3DDevice->SetTransform(D3DTS_WORLD, Mat); break;
		case AX_MATRIX_PROJECTION: _D3DDevice->SetTransform(D3DTS_PROJECTION, Mat); break;
	}
}

unsigned int DirectXRenderer::GetRenderState(AXRenderState State) {
	DWORD V = 0;
	switch(State) {
		case AX_RS_LIGHTING: _D3DDevice->GetRenderState(D3DRS_LIGHTING, &V); break;
		case AX_RS_ZENABLE: _D3DDevice->GetRenderState(D3DRS_ZENABLE, &V); break;
		case AX_RS_ZWRITE: _D3DDevice->GetRenderState(D3DRS_ZWRITEENABLE, &V); break;
		case AX_RS_CULLMODE: 
			_D3DDevice->GetRenderState(D3DRS_CULLMODE, &V);
			if(V == D3DCULL_NONE) V = AX_CULL_NONE;
			else if(V == D3DCULL_CCW) V = AX_CULL_CCW;
			else if(V == D3DCULL_CW) V = AX_CULL_CW;
			break;
		case AX_RS_FILLMODE:
			_D3DDevice->GetRenderState(D3DRS_FILLMODE, &V);
			if(V == D3DFILL_SOLID) V = AX_FILL_SOLID;
			else if(V == D3DFILL_WIREFRAME) V = AX_FILL_WIREFRAME;
			else if(V == D3DFILL_POINT) V = AX_FILL_POINT;
			break;
		case AX_RS_DIFFUSESOURCE:
			_D3DDevice->GetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, &V);
			if(V == D3DMCS_COLOR1) V = AX_COLORSOURCE_VERTEX;
			else if(V == D3DMCS_MATERIAL) V = AX_COLORSOURCE_MATERIAL;
			break;
	}
	return (unsigned int)V;
}

void DirectXRenderer::SetRenderState(AXRenderState State, unsigned int Value) {
	switch(State) {
		case AX_RS_LIGHTING: _D3DDevice->SetRenderState(D3DRS_LIGHTING, Value); break;
		case AX_RS_ZENABLE: _D3DDevice->SetRenderState(D3DRS_ZENABLE, Value); break;
		case AX_RS_ZWRITE: _D3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, Value); break;
		case AX_RS_CULLMODE: 
			if(Value == AX_CULL_NONE)
				_D3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
			else if(Value == AX_CULL_CCW)
				_D3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
			else if(Value == AX_CULL_CW)
				_D3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
			break;
		case AX_RS_FILLMODE: 
			if(Value == AX_FILL_SOLID) _D3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
			else if(Value == AX_FILL_WIREFRAME) _D3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
			else if(Value == AX_FILL_POINT) _D3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_POINT);
			break;
		case AX_RS_DIFFUSESOURCE:
			if(Value == AX_COLORSOURCE_VERTEX) _D3DDevice->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_COLOR1);
			else _D3DDevice->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_MATERIAL);
			break;
	}
	_SetRenderStates[State] = Value;
}

void DirectXRenderer::SetMaterial(AXMaterial *Material) {
	D3DMATERIAL9 Mat;
	ZeroMemory(&Mat, sizeof(Mat));
	Mat.Ambient.a = 1.0f;
	Mat.Ambient.r = 1.0f;
	Mat.Ambient.g = 1.0f;
	Mat.Ambient.b = 1.0f;

	Mat.Diffuse.a = AXColorA(Material->Diffuse);
	Mat.Diffuse.r = AXColorR(Material->Diffuse);
	Mat.Diffuse.g = AXColorG(Material->Diffuse);
	Mat.Diffuse.b = AXColorB(Material->Diffuse);

	Mat.Specular.a = AXColorA(Material->Specular);
	Mat.Specular.r = AXColorR(Material->Specular);
	Mat.Specular.g = AXColorG(Material->Specular);
	Mat.Specular.b = AXColorB(Material->Specular);

	Mat.Power = Material->Shininess;

	unsigned int CurDiffuseSource = GetRenderState(AX_RS_DIFFUSESOURCE);
	unsigned int CurSpecularSource = GetRenderState(AX_RS_SPECULARSOURCE);
	if(Material->DiffuseSource != CurDiffuseSource) {
		SetRenderState(AX_RS_DIFFUSESOURCE, Material->DiffuseSource);
	}
	if(Material->SpecularSource != CurSpecularSource) {
		SetRenderState(AX_RS_DIFFUSESOURCE, Material->SpecularSource);
	}

	_D3DDevice->SetMaterial(&Mat);
}

unsigned int DirectXRenderer::GetDeviceCapability(AXDeviceCapability Cap) {
	/*	to be fleshed out */
	switch(Cap) {
		case AX_DC_NUMLIGHTS:	return _D3DCaps.MaxActiveLights;
	}

	return 0;
}

/* -----------------------------------
	Vertex and index management
----------------------------------- */

unsigned int DirectXRenderer::CreateVertexBuffer() {
	for(unsigned int i=0; i<_VertexBuffers.size(); i++) {
		if(_VertexBuffers[i].InUse = 0) {
			_VertexBuffers[i].VertexBuffer = NULL;
			_VertexBuffers[i].InUse = 1;
			return i;
		}
	}

	DirectXVertexBuffer TempBuffer;
	TempBuffer.VertexBuffer = NULL;
	TempBuffer.InUse = 1;
	_VertexBuffers.push_back(TempBuffer);

	return (unsigned int)_VertexBuffers.size()-1;
}

unsigned int DirectXRenderer::CreateIndexBuffer() {
	for(unsigned int i=0; i<_IndexBuffers.size(); i++) {
		if(_IndexBuffers[i].InUse = 0) {
			_IndexBuffers[i].IndexBuffer = NULL;
			_IndexBuffers[i].InUse = 1;
			return i;
		}
	}

	DirectXIndexBuffer TempBuffer;
	TempBuffer.IndexBuffer = NULL;
	TempBuffer.InUse = 1;
	_IndexBuffers.push_back(TempBuffer);

	return (unsigned int)_IndexBuffers.size()-1;
}

AXResult DirectXRenderer::AllocateVertexBuffer(unsigned int ID, unsigned int Size) {
	if(ID >= (unsigned int)_VertexBuffers.size()) {
		LOG_SYS1("Failed creating DX vertex buffer, invalid ID: %d", ID);
		return AXFAILURE;
	}
	if(_VertexBuffers[ID].VertexBuffer != NULL) {
		SafeRelease(_VertexBuffers[ID].VertexBuffer);
	}

	HRESULT hr = _D3DDevice->CreateVertexBuffer(Size, D3DUSAGE_WRITEONLY, 0, D3DPOOL_DEFAULT, &_VertexBuffers[ID].VertexBuffer, NULL);
	if(FAILED(hr)) {
		LOG_SYS2("Failed creating DX vertex buffer, size: %d: %s", Size, DXGetErrorString9(hr));
		return AXFAILURE;
	}
	return AXSUCCESS;
}

void DirectXRenderer::DestroyVertexBuffer(unsigned int ID) {
	SafeRelease(_VertexBuffers[ID].VertexBuffer);
	_VertexBuffers[ID].InUse = 0;
}


AXResult DirectXRenderer::AllocateIndexBuffer(unsigned int ID, unsigned int Size, unsigned int NumBytes) {
	if(ID >= (unsigned int)_IndexBuffers.size()) {
		LOG_SYS1("Failed creating DX index buffer, invalid ID: %d", ID);
		return AXFAILURE;
	}
	if(_IndexBuffers[ID].IndexBuffer != NULL) {
		SafeRelease(_IndexBuffers[ID].IndexBuffer);
	}

	HRESULT hr;
	if(NumBytes == 2)
		hr = _D3DDevice->CreateIndexBuffer(Size, D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &_IndexBuffers[ID].IndexBuffer, NULL);
	else
		hr = _D3DDevice->CreateIndexBuffer(Size, D3DUSAGE_WRITEONLY, D3DFMT_INDEX32, D3DPOOL_DEFAULT, &_IndexBuffers[ID].IndexBuffer, NULL);

	if(FAILED(hr)) {
		LOG_SYS3("Failed creating DX index buffer, size: %d  numbytes: %d.  Error: %s", Size, NumBytes, DXGetErrorString9(hr));
		return AXFAILURE;
	}
	return AXSUCCESS;
}

void DirectXRenderer::DestroyIndexBuffer(unsigned int ID) {
	SafeRelease(_IndexBuffers[ID].IndexBuffer);
	_IndexBuffers[ID].InUse = 0;
}

void* DirectXRenderer::LockVertexBuffer(unsigned int ID, unsigned int Offset, unsigned int Size) {
	void* Buffer;
	HRESULT hr = _VertexBuffers[ID].VertexBuffer->Lock(Offset, Size, &Buffer, 0);
	if(FAILED(hr)) {
		LOG_SYS1("Failed locking DX vertex buffer: %s", DXGetErrorString9(hr));
		return NULL;
	}
	return Buffer;
}


void DirectXRenderer::UnlockVertexBuffer(unsigned int ID) {
	_VertexBuffers[ID].VertexBuffer->Unlock();
}

void* DirectXRenderer::LockIndexBuffer(unsigned int ID, unsigned int Offset, unsigned int Size) {
	void* Buffer;
	HRESULT hr = _IndexBuffers[ID].IndexBuffer->Lock(Offset, Size, &Buffer, 0);
	if(FAILED(hr)) {
		LOG_SYS1("Failed locking DX index buffer:", DXGetErrorString9(hr));
		return NULL;
	}
	return Buffer;
}
void DirectXRenderer::UnlockIndexBuffer(unsigned int ID) {
	_IndexBuffers[ID].IndexBuffer->Unlock();
}


void DirectXRenderer::SetVertexSource(AXVertexBuffer *Buffer) {
	if(!Buffer) return;
	if(!Buffer->Stride) {
		LOG_SYS("Warning - failed setting vertex source, data is not interleaved!");
		return;
	};

	unsigned int Size = 0;
	unsigned int Types = 0;
	unsigned int NumElements = (unsigned int)Buffer->BufferDesc.size();
	for(unsigned int i=0; i<NumElements; i++) {
		Types |= Buffer->BufferDesc[i].Type;
		Size += GetElementSizeInBytes(Buffer->BufferDesc[i].Format);
	}

	IDirect3DVertexDeclaration9 *Decl = NULL;

	UIntVertexDeclarationMap::iterator it = _VertexDeclarations.find(Types);
	if(it == _VertexDeclarations.end())
		Decl = _CreateVertexDeclaration(Buffer);
	else
		Decl = it->second;

	HRESULT hr = _D3DDevice->SetVertexDeclaration(Decl);
	if(FAILED(hr)) {
		LOG_SYS1("Failed setting DX vertex declaration: %s", DXGetErrorString9(hr));
	}


	hr = _D3DDevice->SetStreamSource(0, _VertexBuffers[Buffer->PoolID].VertexBuffer, Buffer->Offset, Buffer->Stride);
	if(FAILED(hr)) {
		LOG_SYS1("Failed setting DX stream source: %s", DXGetErrorString9(hr));
	}
}


void DirectXRenderer::SetIndexSource(AXIndexBuffer *Buffer) {
	if(!Buffer) return;

	if(Buffer->IndexFormat == AX_FORMAT_USHORT1) {
		_IndexBufferOffset = Buffer->Offset/2;
	}
	else if(Buffer->IndexFormat = AX_FORMAT_UINT1) {
		_IndexBufferOffset = Buffer->Offset/4;
	}

	if(_IndexBufferID != Buffer->PoolID) {
		HRESULT hr = _D3DDevice->SetIndices(_IndexBuffers[Buffer->PoolID].IndexBuffer);
		if(FAILED(hr)) {
			LOG_SYS1("Failed setting DX index buffer: %s", DXGetErrorString9(hr));
		}
		else {
			_IndexBufferID = Buffer->PoolID;
		}
	}
}

/* -----------------------------------
	Lights
----------------------------------- */

void DirectXRenderer::SetLight(unsigned int Index, AXLightEntity *Light) {
	D3DLIGHT9 D3DLight;
	ZeroMemory(&D3DLight, sizeof(D3DLight));

	switch(Light->Type) {
		case AX_LIGHT_POINT:		D3DLight.Type = D3DLIGHT_POINT; break;
		case AX_LIGHT_DIRECTIONAL:	D3DLight.Type = D3DLIGHT_DIRECTIONAL; break;
		case AX_LIGHT_SPOT:			D3DLight.Type = D3DLIGHT_SPOT; break;
		default:
			LOG_SYS("Set light failed, invalid light type!");
			return;
	}

	D3DLight.Diffuse.r = Light->Diffuse.r;
	D3DLight.Diffuse.g = Light->Diffuse.g;
	D3DLight.Diffuse.b = Light->Diffuse.b;
	D3DLight.Diffuse.a = Light->Diffuse.a;

	D3DLight.Specular.r = Light->Specular.r;
	D3DLight.Specular.g = Light->Specular.g;
	D3DLight.Specular.b = Light->Specular.b;
	D3DLight.Specular.a = Light->Specular.a;

	D3DLight.Ambient.r = Light->Ambient.r;
	D3DLight.Ambient.g = Light->Ambient.g;
	D3DLight.Ambient.b = Light->Ambient.b;
	D3DLight.Ambient.a = Light->Ambient.a;

	if(Light->Type != AX_LIGHT_DIRECTIONAL) {
		D3DLight.Position.x = Light->Position.x;
		D3DLight.Position.y = Light->Position.y;
		D3DLight.Position.z = Light->Position.z;

		if(Light->Range < 0)
			D3DLight.Range = 0;
		else
			D3DLight.Range = Light->Range;

		D3DLight.Attenuation0 = Light->Attenuation0/Light->Brightness;
		D3DLight.Attenuation1 = Light->Attenuation1/Light->Brightness;
		D3DLight.Attenuation2 = Light->Attenuation2/Light->Brightness;
	}

	if(Light->Type != AX_LIGHT_POINT) {
		D3DLight.Direction.x = Light->Direction1.x;
		D3DLight.Direction.y = Light->Direction1.y;
		D3DLight.Direction.z = Light->Direction1.z;
	}

	if(Light->Type == AX_LIGHT_SPOT) {
		D3DLight.Falloff = Light->Falloff;
		D3DLight.Theta = Light->Theta;
		D3DLight.Phi = Light->Phi;
	}

	_D3DDevice->SetLight(Index, &D3DLight);
}

void DirectXRenderer::EnableLight(unsigned int Index) {
	_D3DDevice->LightEnable(Index, 1);
}

void DirectXRenderer::DisableLight(unsigned int Index) {
	_D3DDevice->LightEnable(Index, 0);
}


/* -----------------------------------
	Various functions
----------------------------------- */

IDirect3DVertexDeclaration9* DirectXRenderer::_CreateVertexDeclaration(AXVertexBuffer *Buffer) {
	if(!Buffer->Stride) {
		LOG_SYS("Warning - failed creating vertex declaration, data is not interleaved!");
		return NULL;
	};

	unsigned int TypeFlags = 0;
	unsigned int NumElements = (unsigned int)Buffer->BufferDesc.size();

	/*	Now we need to create the vertex declaration to tell DirectX what's in the buffer */
	D3DVERTEXELEMENT9 *Elements = new D3DVERTEXELEMENT9[NumElements+1];
	unsigned int ElementIndex = 0;
	unsigned int ByteOffset = 0;

	for(unsigned int j=0; j<NumElements; j++) {
		AXElementDescription Elem = Buffer->BufferDesc[j];

		D3DDECLUSAGE DXUsage;
		unsigned int TypeIndex = 0;
		if(Elem.Type == AX_ELEM_POSITION) {
			TypeFlags |= AX_ELEM_POSITION;
			DXUsage = D3DDECLUSAGE_POSITION;
		}
		else if(Elem.Type == AX_ELEM_NORMAL) {
			TypeFlags |= AX_ELEM_NORMAL;
			DXUsage = D3DDECLUSAGE_NORMAL;
		}
		else if(Elem.Type == AX_ELEM_DIFFUSE) {
			TypeFlags |= AX_ELEM_DIFFUSE;
			DXUsage = D3DDECLUSAGE_COLOR;
		}

		else if(Elem.Type == AX_ELEM_TEXCOORD0) {
			TypeFlags |= AX_ELEM_TEXCOORD0;
			DXUsage = D3DDECLUSAGE_TEXCOORD;
			TypeIndex = 0;
		}
		else if(Elem.Type == AX_ELEM_TEXCOORD1) {
			TypeFlags |= AX_ELEM_TEXCOORD1;
			DXUsage = D3DDECLUSAGE_TEXCOORD;
			TypeIndex = 1;
		}
		else if(Elem.Type == AX_ELEM_TEXCOORD2) {
			TypeFlags |= AX_ELEM_TEXCOORD2;
			DXUsage = D3DDECLUSAGE_TEXCOORD;
			TypeIndex = 2;
		}
		else if(Elem.Type == AX_ELEM_TEXCOORD3) {
			TypeFlags |= AX_ELEM_TEXCOORD3;
			DXUsage = D3DDECLUSAGE_TEXCOORD;
			TypeIndex = 3;
		}
		else if(Elem.Type == AX_ELEM_TEXCOORD4) {
			TypeFlags |= AX_ELEM_TEXCOORD4;
			DXUsage = D3DDECLUSAGE_TEXCOORD;
			TypeIndex = 4;
		}
		else if(Elem.Type == AX_ELEM_TEXCOORD5) {
			TypeFlags |= AX_ELEM_TEXCOORD5;
			DXUsage = D3DDECLUSAGE_TEXCOORD;
			TypeIndex = 5;
		}
		else if(Elem.Type == AX_ELEM_TEXCOORD6) {
			TypeFlags |= AX_ELEM_TEXCOORD6;
			DXUsage = D3DDECLUSAGE_TEXCOORD;
			TypeIndex = 6;
		}
		else if(Elem.Type == AX_ELEM_TEXCOORD7) {
			TypeFlags |= AX_ELEM_TEXCOORD7;
			DXUsage = D3DDECLUSAGE_TEXCOORD;
			TypeIndex = 7;
		}

		/*	Map our types to DirectX's types */
		D3DDECLTYPE DXType = D3DDECLTYPE_UNUSED;
		AXDataFormat AXFormat = Elem.Format;
		switch(AXFormat) {
			case AX_FORMAT_FLOAT1: DXType = D3DDECLTYPE_FLOAT1; break;
			case AX_FORMAT_FLOAT2: DXType = D3DDECLTYPE_FLOAT2; break;
			case AX_FORMAT_FLOAT3: DXType = D3DDECLTYPE_FLOAT3; break;
			case AX_FORMAT_FLOAT4: DXType = D3DDECLTYPE_FLOAT4; break;
			case AX_FORMAT_UINT1:  DXType = D3DDECLTYPE_D3DCOLOR; break;
			case AX_FORMAT_UBYTE4: DXType = D3DDECLTYPE_UBYTE4; break;
			case AX_FORMAT_SHORT2: DXType = D3DDECLTYPE_SHORT2; break;
			case AX_FORMAT_SHORT4: DXType = D3DDECLTYPE_SHORT4; break;
			default: DXType = D3DDECLTYPE_UNUSED;
		}

		D3DVERTEXELEMENT9 TempElem = { 0, ByteOffset, DXType, D3DDECLMETHOD_DEFAULT, DXUsage, TypeIndex };
		Elements[ElementIndex++] = TempElem;
		ByteOffset += GetElementSizeInBytes(Elem.Format);
	}

	D3DVERTEXELEMENT9 TempElem = D3DDECL_END();
	Elements[ElementIndex++] = TempElem;

	IDirect3DVertexDeclaration9 *Declaration;
	HRESULT hr = _D3DDevice->CreateVertexDeclaration(Elements, &Declaration);

	SafeDelete(Elements);

	if(FAILED(hr)) {
		LOG_SYS2("Failed creating vertex declaration!  Types: %x  Error: %s", TypeFlags, DXGetErrorString9(hr));
		return NULL;
	}
	
	_VertexDeclarations[TypeFlags] = Declaration;
	return Declaration;
}

/* -----------------------------------
	Private internal functions
----------------------------------- */
void DirectXRenderer::_SetDXDefaults() {
	AXDimension2D ScreenSize = GetScreenSize();

	/*	Default Matrices */
	AXMatrix4 TempMatrix;
	D3DXMatrixIdentity(&TempMatrix);
	LoadMatrix(AX_MATRIX_WORLD, &TempMatrix);

	D3DXMatrixLookAtLH(&TempMatrix, &AXVector3(0.0f, 0.0f, 0.0f), &AXVector3(0.0f, 0.0f, 1.0f), &AXVector3(0.0f, 1.0f, 0.0f));
	LoadMatrix(AX_MATRIX_VIEW, &TempMatrix);

	D3DXMatrixPerspectiveFovLH(&TempMatrix, 45.0f, (float)ScreenSize.x/(float)ScreenSize.y, 0.5f, 1000.0f);
	LoadMatrix(AX_MATRIX_PROJECTION, &TempMatrix);

	for(unsigned int i=0; i<AX_MAXRENDERSTATES; i++) {
		if(_SetRenderStates[i] != (unsigned int)-1) {
			SetRenderState((AXRenderState)i, _SetRenderStates[i]);
		}
	}
}

AXResult DirectXRenderer::_GetPresentationParam(D3DPRESENT_PARAMETERS *Present_Param) {
    ZeroMemory(Present_Param, sizeof(D3DPRESENT_PARAMETERS));
	D3DFORMAT BackBufferFormat;
	D3DFORMAT DepthStencilFormat;

	/*	We do quite different things if we are in fullscreen or not */
	if(AXBaseSystem::GetWindow()->Settings.GetSetting<int>("fullscreen")) {
		Present_Param->BackBufferWidth = (unsigned int)AXBaseSystem::GetWindow()->Settings.GetSetting<int>("fullscreenWidth");
		Present_Param->BackBufferHeight = (unsigned int)AXBaseSystem::GetWindow()->Settings.GetSetting<int>("fullscreenHeight");
		Present_Param->FullScreen_RefreshRateInHz = (unsigned int)AXBaseSystem::GetWindow()->Settings.GetSetting<int>("fullscreenRefreshRate");
		Present_Param->Windowed = 0;
	}
	else {
		Present_Param->Windowed = 1;
	}

	/*	Get the back buffer format */
	BackBufferFormat = _GetBestBackBufferFormat();
	if(BackBufferFormat == (D3DFORMAT)0) {
		LOG_SYS("A supported back buffer format could not be found!");
		return AXFAILURE;
	}

	_BackBufferFormat = BackBufferFormat;

	/*	Get the depth stencil format */
	DepthStencilFormat = _GetBestDepthStencilFormat();
	if(DepthStencilFormat == (D3DFORMAT)0) {
		LOG_SYS("A supported depth stencil format could not be found!");
		return AXFAILURE;
	}

	_DepthStencilFormat = DepthStencilFormat;

	/*	Fill in the last of the required values */
	Present_Param->BackBufferFormat = BackBufferFormat;
	Present_Param->EnableAutoDepthStencil = 1;
	Present_Param->AutoDepthStencilFormat = DepthStencilFormat;
	Present_Param->PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;
	Present_Param->BackBufferCount = 2;
    Present_Param->SwapEffect = D3DSWAPEFFECT_DISCARD;

	return AXSUCCESS;
}

D3DFORMAT DirectXRenderer::_GetCurrentAdapterFormat() {
	if(AXBaseSystem::GetWindow()->Settings.GetSetting<int>("fullscreen")) {
		return D3DFMT_X8R8G8B8;
	}
	else {
		D3DDISPLAYMODE dm;
		_D3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &dm);
		return dm.Format;
	}
}

D3DFORMAT DirectXRenderer::_GetBestBackBufferFormat() {
	D3DFORMAT AdapterFormat;
	D3DFORMAT BackBufferFormat;
	int ForcedBackBufferBpp;

	AdapterFormat = _GetCurrentAdapterFormat();
	ForcedBackBufferBpp = Settings.GetSetting<int>("screenBpp");

	/* This gets the closest back buffer format to the adapter format */
	if(ForcedBackBufferBpp == -1 && !AXBaseSystem::GetWindow()->Settings.GetSetting<int>("fullscreen")) {
		if(AdapterFormat == D3DFMT_R5G6B5 || AdapterFormat == D3DFMT_X1R5G5B5)
			ForcedBackBufferBpp = 16;
	}
	
	/*	Figure out an appropriate screen format.  This just goes through and tries every format.
		Obviously we check the best formats first and go down from there.  That way when it finds a supported
		format, we know its the best we can get with this hardware. */
	if( ForcedBackBufferBpp != 16 &&
		AXSUCCEEDED(_IsBackBufferFormatAvailable(AdapterFormat, D3DFMT_A8R8G8B8)) ) {
		LOG_SYS("32-bit screen format found: A8R8G8B8");
		BackBufferFormat = D3DFMT_A8R8G8B8;
	}
	else if( ForcedBackBufferBpp != 16 &&
			 AXSUCCEEDED(_IsBackBufferFormatAvailable(AdapterFormat, D3DFMT_A2R10G10B10)) ) {
		LOG_SYS("32-bit screen format found: A2R10G10B10");
		BackBufferFormat = D3DFMT_A2R10G10B10;
	}
	else if( ForcedBackBufferBpp != 16 &&
			 AXSUCCEEDED(_IsBackBufferFormatAvailable(AdapterFormat, D3DFMT_X8R8G8B8)) ) {
		LOG_SYS("32-bit screen format found: X8R8G8B8");
		BackBufferFormat = D3DFMT_X8R8G8B8;
	}
	else if (ForcedBackBufferBpp == 32) {
		LOG_SYS("Could not find a supported 32-bit back buffer format!  Try changing the setting to 16-bit.");
		return (D3DFORMAT)0;
	}
	else if( AXSUCCEEDED(_IsBackBufferFormatAvailable(AdapterFormat, D3DFMT_R5G6B5)) ) {
		LOG_SYS("16-bit screen format found: R5G6B5");
		BackBufferFormat = D3DFMT_R5G6B5;
	}
	else if( AXSUCCEEDED(_IsBackBufferFormatAvailable(AdapterFormat, D3DFMT_X1R5G5B5)) ) {
		LOG_SYS("16-bit screen format found: X1R5G5B5");
		BackBufferFormat = D3DFMT_X1R5G5B5;
	}
	else {
		LOG_SYS("16-bit back buffer format could not be found.  If you set 16-bits in the settings, try changing it to 32-bits.");
		return (D3DFORMAT)0;
	}

	return BackBufferFormat;

}

D3DFORMAT DirectXRenderer::_GetBestDepthStencilFormat() {
	D3DFORMAT AdapterFormat;
	D3DFORMAT BackBufferFormat;
	D3DFORMAT DepthStencilFormat;
	int ForcedDepthStencilBpp;

	AdapterFormat = _GetCurrentAdapterFormat();
	BackBufferFormat = _BackBufferFormat;
	ForcedDepthStencilBpp = Settings.GetSetting<int>("depthBpp");

	/*	Go through and test all the formats */
	if( ForcedDepthStencilBpp != 16 &&
		AXSUCCEEDED(_IsDepthStencilFormatAvailable(AdapterFormat, D3DFMT_D24S8)) &&
		AXSUCCEEDED(_IsDepthStencilFormatCompatible(AdapterFormat, BackBufferFormat, D3DFMT_D24S8)) ) {
		DepthStencilFormat = D3DFMT_D24S8;
		LOG_SYS("Best 32-bit depth stencil buffer found: D24S8");
	}	

	else if( ForcedDepthStencilBpp != 16 &&
			 AXSUCCEEDED(_IsDepthStencilFormatAvailable(AdapterFormat, D3DFMT_D24X4S4)) &&
			 AXSUCCEEDED(_IsDepthStencilFormatCompatible(AdapterFormat, BackBufferFormat, D3DFMT_D24X4S4)) ) {
		DepthStencilFormat = D3DFMT_D24X4S4;
		LOG_SYS("Best 32-bit depth stencil buffer found: D24X4S4");
	}

	else if( ForcedDepthStencilBpp != 16 &&
			 AXSUCCEEDED(_IsDepthStencilFormatAvailable(AdapterFormat, D3DFMT_D32)) &&
			 AXSUCCEEDED(_IsDepthStencilFormatCompatible(AdapterFormat, BackBufferFormat, D3DFMT_D32)) ) {
		DepthStencilFormat = D3DFMT_D32;
		LOG_SYS("Best 32-bit depth stencil buffer found: D32");
	}

	else if( ForcedDepthStencilBpp != 16 &&
			 AXSUCCEEDED(_IsDepthStencilFormatAvailable(AdapterFormat, D3DFMT_D24X8)) &&
			 AXSUCCEEDED(_IsDepthStencilFormatCompatible(AdapterFormat, BackBufferFormat, D3DFMT_D24X8)) ) {
		DepthStencilFormat = D3DFMT_D24X8;
		LOG_SYS("Best 32-bit depth stencil buffer found: D24X8");
	}

	else if(ForcedDepthStencilBpp == 32) {
		LOG_SYS("Could not find supported 32-bit format for depth stencil!  Try changing the 'depthBpp' setting to 16-bit, or taking it out altogether.");
		return (D3DFORMAT)0;
	}

	else if( AXSUCCEEDED(_IsDepthStencilFormatAvailable(AdapterFormat, D3DFMT_D15S1)) &&
			 AXSUCCEEDED(_IsDepthStencilFormatCompatible(AdapterFormat, BackBufferFormat, D3DFMT_D15S1)) ) {
		DepthStencilFormat = D3DFMT_D15S1;
		LOG_SYS("Best 16-bit depth stencil buffer found: D15S1");
	}

	else if( AXSUCCEEDED(_IsDepthStencilFormatAvailable(AdapterFormat, D3DFMT_D16)) &&
			AXSUCCEEDED(_IsDepthStencilFormatCompatible(AdapterFormat, BackBufferFormat, D3DFMT_D16)) ) {
		DepthStencilFormat = D3DFMT_D16;
		LOG_SYS("Best 16-bit depth stencil buffer found: D16");
	}

	else if(ForcedDepthStencilBpp == 16) {
		LOG_SYS("Could not find supported 16-bit format for depth stencil!  Try changing the 'depthBpp' setting to 32-bit, or taking it out altogether.");
		return (D3DFORMAT)0;
	}
	else {
		LOG_SYS("Could not find supported format for depth stencil!");
		return (D3DFORMAT)0;
	}

	return DepthStencilFormat;
}

AXResult DirectXRenderer::_IsBackBufferFormatAvailable(D3DFORMAT AdapterFormat, D3DFORMAT BackBufferFormat) {
	HRESULT hr = _D3D->CheckDeviceFormat(D3DADAPTER_DEFAULT, _DeviceType, AdapterFormat, D3DUSAGE_RENDERTARGET, D3DRTYPE_SURFACE, BackBufferFormat);
	if(FAILED(hr))
		return AXFAILURE;
	return AXSUCCESS;
}

AXResult DirectXRenderer::_IsDepthStencilFormatAvailable(D3DFORMAT AdapterFormat, D3DFORMAT DepthStencilFormat) {
	HRESULT hr = _D3D->CheckDeviceFormat(D3DADAPTER_DEFAULT, _DeviceType, AdapterFormat, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, DepthStencilFormat);
	if(FAILED(hr))
		return AXFAILURE;
	return AXSUCCESS;
}

AXResult DirectXRenderer::_IsDepthStencilFormatCompatible(D3DFORMAT AdapterFormat, D3DFORMAT RenderFormat, D3DFORMAT DepthStencilFormat) {
	HRESULT hr = _D3D->CheckDepthStencilMatch(D3DADAPTER_DEFAULT, _DeviceType, AdapterFormat, RenderFormat, DepthStencilFormat);
	if(FAILED(hr))
		return AXFAILURE;
	return AXSUCCESS;
}

