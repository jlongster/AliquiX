/* --------------------------------------------------------------
	File:			SceneApp.cpp
	Description:	See SceneApp.h.

	Date:			May 30, 2006
	Author:			James Long
-------------------------------------------------------------- */

#include "AliquiX.h"
#include "AXUtilities\\AXStringFunctions.h"
#include "SceneApp.h"

/*	Handles input */
class InputListener : public AXSceneListener {
public:
	InputListener() : AXSceneListener() {}
	virtual ~InputListener() {}

	/*	Fires off every time a scene is updated, which is every frame */
	void Listen(AXScene *Scene) {
		AXInput *Input = AXBaseSystem::GetInput();
		if(Input->KeyDown(AX_ESCAPE)) {
			AXBaseSystem::GetKernel()->KillAllTasks();
		}

		if(Input->KeyDown(AX_F1)) {
			AXBaseSystem::GetWindow()->ToggleFullscreen();
		}

		if(Input->KeyDown(AX_F2)) {
			if(AXBaseSystem::GetRenderer()->GetRenderState(AX_RS_FILLMODE) == AX_FILL_SOLID)
				AXBaseSystem::GetRenderer()->SetRenderState(AX_RS_FILLMODE, AX_FILL_WIREFRAME);
			else
				AXBaseSystem::GetRenderer()->SetRenderState(AX_RS_FILLMODE, AX_FILL_SOLID);
		}

		if(Input->KeyDown(AX_F3)) {
			AXBaseSystem::GetRenderer()->Screenshot();
		}

		if(Input->KeyDown(AX_F5)) {
			//	Debug resources and scene
			AXBaseSystem::GetResourceMgr()->Debug();
			AXBaseSystem::GetSceneMgr()->Debug();
		}

		if(Input->KeyStillDown(AX_O)) {
			AXMeshEntity *Box = dynamic_cast<AXMeshEntity*>(Scene->GetEntity("RootBox"));
			Box->RenderNormals(1);
		}

		if(Input->KeyStillDown(AX_L)) {
			AXMeshEntity *Box = dynamic_cast<AXMeshEntity*>(Scene->GetEntity("RootBox"));
			Box->RenderNormals(0);
		}

		//AXEntity *Light = Scene->GetEntity("Light1");
		//Light->GetSceneNode()->Rotate(D3DX_PI/2.0f*AXClock::dT, AXVector3(1.0f, 0.0f, 0.0f));
	}
};

SceneApp::SceneApp() : AXBaseSystem() {
}
SceneApp::~SceneApp() {
}

void SceneApp::OnLoadSettings() {
	/*	Customizable settings:
			- "name" : name of the application
	*/

	Settings.SetSetting("name", std::string("Scene Test"));
}

AXResult SceneApp::OnInitialize() {
	/*	Custom initializing here */

	AXBaseSystem::GetRenderer()->SetClearColor(AXColorToRGB(20,20,34));

	_CreateGeometry();

	if(AXFAILED(_CreateScene())) {
		LOG_APP("Failed creating the scene!");
		return AXFAILURE;
	}

	return AXSUCCESS;
}

void SceneApp::_CreateGeometry() {

	//unsigned int GridSize = 256;

	//FloatData Position;
	//UIntData Color;
	//UShortData Indices;
	//for(unsigned int i=0; i<GridSize; i++) {
	//	for(unsigned int j=0; j<GridSize; j++) {
	//		DataAdd3(Position, (float)i, 0.0f, (float)j);
	//		DataAdd1(Color, AXColorToARGB(255, 50, 90, 100));
	//	}
	//}

	//for(unsigned int i=0; i<GridSize-1; i++) {
	//	for(unsigned int j=0; j<GridSize-1; j++) {
	//		DataAdd1(Indices, i*GridSize+j);
	//		DataAdd1(Indices, i*GridSize+j+1);
	//		DataAdd1(Indices, (i+1)*GridSize+j);

	//		DataAdd1(Indices, (i+1)*GridSize+j);
	//		DataAdd1(Indices, i*GridSize+j+1);
	//		DataAdd1(Indices, (i+1)*GridSize+j+1);
	//	}
	//}

	//AXMeshChunk *TerrainChunk = new AXMeshChunk(AX_PRIM_TRIANGLES);
	//TerrainChunk->AttachVertexData(Position, AX_ELEM_POSITION, AX_FORMAT_FLOAT3);
	//TerrainChunk->AttachVertexData(Color, AX_ELEM_DIFFUSE, AX_FORMAT_UINT1);
	//TerrainChunk->AttachIndexData(Indices, AX_FORMAT_USHORT1);
	////TerrainChunk->Attributes.DiffuseSource = AX_COLORSOURCE_MATERIAL;
	////TerrainChunk->Attributes.Diffuse = AXColorToARGB(1.0f, 0.5f, 0.85f, 0.92f);

	//AXMesh *Terrain = new AXMesh("Terrain");
	//Terrain->AddChunk(TerrainChunk);

	//AXBaseSystem::GetResourceMgr()->RegisterMesh(Terrain);

	/*	Axes */

	FloatData Position;
	UIntData Color;
	
	DataAdd3(Position, 0.0f, 0.0f, 0.0f);
	DataAdd3(Position, 1.0f, 0.1f, 0.0f);
	DataAdd3(Position, 1.0f, -0.1f, 0.0f);
	DataAdd3(Position, 0.0f, 0.0f, 0.0f);
	DataAdd3(Position, 1.0f, -0.1f, 0.0f);
	DataAdd3(Position, 1.0f, 0.1f, 0.0f);

	DataAdd3(Position, 0.0f, 0.0f, 0.0f);
	DataAdd3(Position, 0.0f, 1.0f, 0.1f);
	DataAdd3(Position, 0.0f, 1.0f, -0.1f);
	DataAdd3(Position, 0.0f, 0.0f, 0.0f);
	DataAdd3(Position, 0.0f, 1.0f, -0.1f);
	DataAdd3(Position, 0.0f, 1.0f, 0.1f);

	DataAdd3(Position, 0.0f, 0.0f, 0.0f);
	DataAdd3(Position, 0.1f, 0.0f, 1.0f);
	DataAdd3(Position, -0.1f, 0.0f, 1.0f);
	DataAdd3(Position, 0.0f, 0.0f, 0.0f);
	DataAdd3(Position, -0.1f, 0.0f, 1.0f);
	DataAdd3(Position, 0.1f, 0.0f, 1.0f);

	for(unsigned int i=0; i<6; i++)
		DataAdd1(Color, AXColorToARGB(255,255,0,0));
	
	for(unsigned int i=0; i<6; i++)
		DataAdd1(Color, AXColorToARGB(255,0,255,0));

	for(unsigned int i=0; i<6; i++)
		DataAdd1(Color, AXColorToARGB(255,0,0,255));

	AXMeshChunk *AxesChunk = new AXMeshChunk(AX_PRIM_TRIANGLES);
	AxesChunk->AttachVertexData(Position, AX_ELEM_POSITION, AX_FORMAT_FLOAT3);
	AxesChunk->AttachVertexData(Color, AX_ELEM_DIFFUSE, AX_FORMAT_UINT1);
	AxesChunk->AssignDefaultEffect(0);

	AXMesh *Axes = new AXMesh("Axes");
	Axes->AddChunk(AxesChunk);

	AXBaseSystem::GetResourceMgr()->RegisterMesh(Axes);


	/*	Wall */

	Position.clear();

	DataAdd3(Position, -1.0f, 1.0f, 1.0f);
	DataAdd3(Position, -1.0f, -1.0f, 1.0f);
	DataAdd3(Position, 1.0f, 1.0f, 1.0f);
	DataAdd3(Position, 1.0f, -1.0f, 1.0f);

	AXMeshChunk *WallChunk = new AXMeshChunk(AX_PRIM_TRIANGLESTRIP);
	WallChunk->AttachVertexData(Position, AX_ELEM_POSITION, AX_FORMAT_FLOAT3);

	AXMesh *Wall = new AXMesh("Wall");
	Wall->AddChunk(WallChunk);

	AXBaseSystem::GetResourceMgr()->RegisterMesh(Wall);

}

AXResult SceneApp::_CreateScene() {
	AXScene *Scene = AXBaseSystem::GetSceneMgr();
	Scene->AddSceneListener(new InputListener);

	AXSceneNode *Root = Scene->GetRoot();

	Root->CreateChildWithEntity(Scene->CreateEntity("Mesh", "Axes", "Axes"));

	//AXMeshEntity *Box = dynamic_cast<AXMeshEntity*>(Scene->CreateEntity("Mesh", "Box0", "Box"));
	//Box->AssignEffect(1);

	//Root->CreateChildWithEntity(Box);


	AXEntity *Box = Scene->CreateEntity("Mesh", "RootBox", "Box");
	AXSceneNode *BoxNode = Root->CreateChildWithEntity(Box);

	AXLightEntity *Light1 = Scene->CreateLight("RootLight");
	Light1->Type = AX_LIGHT_SPOT;
	Light1->Direction0 = AXVector3(1.0f, -1.0f, 0.0f);
	Light1->Diffuse = AXColorToARGB(1.0f, 0.9f, 0.6f, 0.5f);
	Light1->Theta = AX_PI/1.5f;
	Light1->Phi = AX_PI/1.1f;
	Light1->Brightness = 5.0f;
	Light1->Dynamic = 1;
	AXSceneNode *Light1Node = Root->CreateChildWithEntity(Light1);


	AXSceneNode *LastLight = Light1Node;
	AXSceneNode *LastBox = BoxNode;
	LastBox->Translate(0.0f, 0.0f, 5.0f);
	LastLight->Translate(0.0f, 5.0f, 5.0f);
	for(unsigned int i=0; i<50; i++) {		
		std::string Name = "Box" + AXStringConvertFromInt(i);
		AXEntity *Box = Scene->CreateEntity("Mesh", Name, "Box");

		AXSceneNode *RotatingBox;
		RotatingBox = LastBox->CreateChildWithEntity(Box);

		RotatingBox->Rotate(AX_PI/15.0f, AXVector3(1.0f, 0.0f, 0.0f));
		RotatingBox->Translate(5.0f, 0.0f, 0.0f);

		LastBox = RotatingBox;

		if(i%5 == 0) {
			AXLightEntity *Light1 = Scene->CreateLight("Light" + Name);
			Light1->Type = AX_LIGHT_SPOT;
			Light1->Direction0 = AXVector3(-1.0f, -1.0f, 0.0f);
			Light1->Diffuse = AXColorToARGB(1.0f, 0.2f, i/50.0f, i/100.0f);
			Light1->Theta = AX_PI/1.5f;
			Light1->Phi = AX_PI/1.1f;

			if(i==5) Light1->Brightness = 15.0f;
			else Light1->Brightness = 5.0f;

			Light1->Dynamic = 1;

			AXSceneNode *LightNode;
			LightNode = LastLight->CreateChildWithEntity(Light1);
			LightNode->Translate(25.0f, 0.0f, 0.0f);

			LastLight = LightNode;
		}
	}

	AXEntity *Camera = Scene->CreateCamera("Player", 50.0f);
	AXSceneNode *CameraNode = Root->CreateChildWithEntity(Camera);
	CameraNode->Translate(0.0f, 0.0f, -5.0f);

	Scene->ActivateCamera("Player");

	return AXSUCCESS;
}

