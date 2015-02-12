/* --------------------------------------------------------------
	File:			AXResources.h
	Description:	This is a basic resource management class.  I may refactor this part of the system
					if I feel like it needs more flexibility, but right now it basically loads, stores,
					and saves different resources.
	Date:			January 13, 2005
	Author:			James Long
-------------------------------------------------------------- */


#if !defined(AXRESOURCES_INCLUDED)
#define AXRESOURCES_INCLUDED

#include "..\\AXCommon.h"
#include "..\\AXCore\\AXSystemComponent.h"
#include "AXVideoBuffers.h"
#include "AXMesh.h"
#include "AXShader.h"

#include <string>
#include <list>
#include <map>

#define MAXSHADERS 255

class AXResources : public AXSystemComponent {
public:
	AXResources();
	virtual ~AXResources();

	AXResult Initialize();
	void Shutdown();

	void ValidateSettings();

	/*	This is called when the system is reset and everything in video memory is lost.
		We will need to recreate all the resources. */
	AXResult OnReset();



	/* --------------------------------------
		Meshes
	-------------------------------------- */

	/*	You only need to call this function if you are creating a mesh programmatically.  If the mesh is saved on the
		hard disk, it will automatically be loaded and registered when it is requested.  */
	void RegisterMesh(AXMesh* Mesh);

	/*	Returns the mesh object.  If it's not found, it tries to load it from the disk, otherwise it returns NULL.
		
		Note that you should pass in the whole name for a file, such as "box.m2d" and that the mesh will be named exactly
		that.  However, if a mesh has been manually loaded, its name is whatever it was registered to. */
	AXMesh* GetMesh(const std::string &Name);


	
	/* --------------------------------------
		Shaders
	-------------------------------------- */

	/*	Registers a shader.  The system will register default shaders, such as simple position, diffuse, etc., but you
		can also add your custom shaders.  This will fail if the hardware can't run the shader. */
	AXResult RegisterShader(AXShader* Shader, unsigned int ID);

	/*	Gets a shader according to its name. */
	AXShader* GetShader(unsigned int ID);


	/* --------------------------------------
		Buffers
	-------------------------------------- */
	AXVertexBuffer* CreateVertexBuffer(unsigned int Size);
	void FreeVertexBuffer(AXVertexBuffer *Buffer);
	void* LockVertexBuffer(AXVertexBuffer *Buffer);
	void UnlockVertexBuffer(AXVertexBuffer *Buffer);

	AXIndexBuffer* CreateIndexBuffer(unsigned int Size, AXDataFormat Format);
	void FreeIndexBuffer(AXIndexBuffer *Buffer);
	void* LockIndexBuffer(AXIndexBuffer *Buffer);
	void UnlockIndexBuffer(AXIndexBuffer *Buffer);



	void DebugMesh(const std::string &Name);
	void Debug();

private:
	typedef std::map< std::string, AXMesh* > StringMeshMap;
	typedef std::map< std::string, AXShader* > StringShaderMap;
	typedef std::list< AXVertexBuffer* > VertexBufferList;
	typedef std::list< AXIndexBuffer* > IndexBufferList;
	typedef std::vector< VertexBufferList > VertexBuffer2DArray;
	typedef std::vector< IndexBufferList > IndexBuffer2DArray;

	void _CreateVideoBuffers();
	void _RegisterDefaultMeshes();

	unsigned int _GetVertexBufferSizeIndex(unsigned int Size);
	unsigned int _GetIndexBufferSizeIndex(unsigned int Size);

	VertexBuffer2DArray _VertexBuffers;
	IndexBuffer2DArray _IndexBuffersUShort;
	IndexBuffer2DArray _IndexBuffersUInt;

	StringMeshMap _Meshes;
	AXShader* _Shaders[MAXSHADERS];

};

#endif
