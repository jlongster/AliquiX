/* --------------------------------------------------------------
	File:			AXScene.cpp
	Description:	See AXScene.h

	Date:			December 19, 2005
	Author:			James Long
-------------------------------------------------------------- */

#include "..\\AXCore.h"
#include "AXScene.h"
#include "..\\AXUtilities\\AXStringFunctions.h"

/*	Default entity types:
		- Mesh (AXMeshEntity)
		- Light (AXLightEntity)
		- Camera (AXCamera)
*/
#include "AXMeshEntity.h"
#include "AXLightEntity.h"
#include "AXCamera.h"

using std::string;

AXScene::AXScene() : AXUpdatableSystemComponent("SceneMgr"), _RootSceneNode(NULL), _CurrentCamera(NULL) {
}
AXScene::~AXScene() {
}

AXResult AXScene::Initialize() {
	RegisterEntityType<AXMeshEntity>("Mesh");
	RegisterEntityType<AXLightEntity>("Light");
	RegisterEntityType<AXCamera>("Camera");
	
	_RootSceneNode = new AXSceneNode;

	return AXSUCCESS;
}

void AXScene::Shutdown() {
	for(unsigned int i=0; i<(unsigned int)_Listeners.size(); i++) {
		SafeDelete(_Listeners[i]);
	}

	if(_RootSceneNode) {
		_RootSceneNode->RemoveAndDestroyChildren();
		SafeDelete(_RootSceneNode);
	}

	StringEntityMap::iterator EntityIt;
	for(EntityIt = _EntityMap.begin(); EntityIt != _EntityMap.end(); EntityIt++) {
		SafeDelete(EntityIt->second);
	}
}

AXResult AXScene::Start() {
	return AXSUCCESS;
}

void AXScene::Update() {
	for(unsigned int i=0; i<(unsigned int)_Listeners.size(); i++) {
		_Listeners[i]->Listen(this);
	}

	_RootSceneNode->Update();
	_RootSceneNode->Execute();
}

void AXScene::Stop() {
}

AXSceneNode* AXScene::GetRoot() {
	return _RootSceneNode;
}

void AXScene::ClearScene() {
	_RootSceneNode->RemoveAndDestroyChildren();
}

void AXScene::AddSceneListener(AXSceneListener *Listener) {
	_Listeners.push_back(Listener);
}

AXEntity* AXScene::CreateEntity(const string &Type, const string &Name, const string &Parameters) {
	AXEntity* Entity;
	StringEntityFuncMap::iterator Iter = _EntityRegistrar.find(Type);

	if(Iter != _EntityRegistrar.end()) {
		LOG_DBG3("Creating entity -- type:%s, name:%s, param:%s", Type.c_str(), Name.c_str(), Parameters.c_str());
		Entity = (this->*(Iter->second))(Name, Parameters);

		if(Entity != NULL) {
			Entity->_Type = Type;
		}
		else {
			LOG_DBG("WARNING -- Failed creating entity, name already taken");
		}

		return Entity;
	}
	
	LOG_DBG("WARNING -- Failed creating entity, type doesn't exist");
	return NULL;
}

AXEntity* AXScene::GetEntity(const string &Name) {
	StringEntityMap::iterator Iter = _EntityMap.find(Name);
	if(Iter != _EntityMap.end()) return Iter->second;
	else return NULL;
}

void AXScene::RemoveEntity(const string &Name) {
	StringEntityMap::iterator it;
	if((it = _EntityMap.find(Name)) != _EntityMap.end()) {
		SafeDelete(it->second);
		_EntityMap.erase(Name);
	}
}

AXLightEntity* AXScene::CreateLight(const std::string &Name) {
	AXLightEntity* Light = dynamic_cast<AXLightEntity*>(CreateEntity("Light", Name, ""));
	if(!Light) {
		LOG_SYS("man what is wrong with this system");
		return NULL;
	}

	Light->Index = (unsigned int)_LightList.size();
	_LightList.push_back(Light);

	return Light;
}

void AXScene::GetLights(LightVector *Lights, AXVector3 *WorldPosition, unsigned int MaxLights) {
	for(unsigned int i=0; i<_LightList.size(); i++) {
		_LightList[i]->CacheEffect(WorldPosition);
		_LightList[i]->Disable();
	}

	for(unsigned int i=0; i<_LightList.size(); i++) {
		LOG_SYS2("Cached effect for light '%s': %f", _LightList[i]->GetName().c_str(), _LightList[i]->CachedEffect);
	}

	std::sort(_LightList.begin(), _LightList.end(), DescendingLightSort());
	for(unsigned int i=0; i<_LightList.size() && i<MaxLights; i++) {
		if(_LightList[i]->CachedEffect > 0.0001f)
			Lights->push_back(_LightList[i]);
	}
}

AXCamera* AXScene::CreateCamera(const std::string &Name, float Speed) {
	AXCamera* Camera = dynamic_cast<AXCamera*>(CreateEntity("Camera", Name, AXStringConvertFromFloat(Speed)));
	if(!Camera) 
		LOG_SYS("something is seriously wrong...");

	return Camera;
}

AXResult AXScene::ActivateCamera(const std::string &Name) {
	AXCamera *Camera = dynamic_cast<AXCamera*>(GetEntity(Name));
	if(!Camera) {
		LOG_SYS1("Failed activating camera '%s': Camera doesn't exist.", Name.c_str());
		return AXFAILURE;
	}

	if(_CurrentCamera) {
		_CurrentCamera->Deactivate();
	}

	_CurrentCamera = Camera;
	_CurrentCamera->Activate();

	return AXSUCCESS;
}

AXVector3* AXScene::GetViewerPosition() {
	return _CurrentCamera->GetWorldPosition();
}

void AXScene::Debug() {
	LOG_DBG("Debugging root scenenode and attached entities...");
	_RootSceneNode->Debug();
}

