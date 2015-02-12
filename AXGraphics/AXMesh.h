/* --------------------------------------------------------------
	File:			AXMesh.h
	Description:	A Mesh is any sort of renderable geometry.

					The system will take care of loading meshes and creating the mesh chunks.
					However, you may want to construct a mesh programmatically.  In that case,
					you can construct the chunks manually.  You would do that with something
					like this:

						FloatData position;
						DataAdd(position, 5.0f, 5.0f, 0.0f);

						FloatData texcoord;
						DataAdd(texcoord, 1.0f, 1.0f);

						AXMeshChunk* BoxChunk1 = new AXMeshChunk;
						BoxChunk1->AttachStream(position, AX_ELEM_POSITION, AX_FORMAT_FLOAT);
						BoxChunk1->AttachStream(texcoord, AX_ELEM_TEXCOORD0, AX_FORMAT_INT);

						AXMesh *Box = new AXMesh("Box");
						Box->AddChunk(BoxChunk1);

						AXBaseSystem::GetResourceMgr()->RegisterMesh(Box);

					Note that we do not used smart pointers when creating the chunks and mesh.
					This is because we do not need the performance penalty with
					such a simple owning scenario, and these pointers will be called a lot.
					The Mesh takes all ownership of the chunks and will delete them, and the chunks will
					clean up their streams.

	Date:			May 31, 2006
	Author:			James Long
-------------------------------------------------------------- */

#if !defined(AXMESH_INCLUDED)
#define AXMESH_INCLUDED

#include "..\\AXCommon.h"
#include "AXMeshChunk.h"

#include <string>
#include <vector>

class AXMesh {
public:
	AXMesh();
	AXMesh(const std::string &Name);
	virtual ~AXMesh();

	std::string GetName();

	unsigned int GetNumChunks();

	void AddChunk(AXMeshChunk *Chunk);
	AXMeshChunk* GetChunk(const unsigned int Index);

	void CalculateNormalChunks();
	AXMeshChunk* GetNormalChunk(const unsigned int Index);

	void FreeChunks();
	void FreeNormalChunks();

	AXResult LoadFromFile(std::string FileName);

	void Debug();

public:
	std::string _Name;
	std::vector<AXMeshChunk*> _Chunks;
	std::vector<AXMeshChunk*> _NormalChunks;
};

#endif

