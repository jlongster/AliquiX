/* --------------------------------------------------------------
	File:			AXScene.h
	Description:	This file defines an class that manages a scene.

	Date:			December 19, 2005
	Author:			James Long
-------------------------------------------------------------- */

#if !defined(AXSCENE_INCLUDED)
#define AXSCENE_INCLUDED

#include "..\\AXCore\\AXUpdatableSystemComponent.h"

#include "AXSceneNode.h"
#include "AXEntity.h"
#include "AXLightEntity.h"
#include "AXCamera.h"

#include <string>
#include <map>
#include <vector>

typedef AXEntity* (AXScene::*EntityCreatorPtr)(std::string, std::string);
typedef std::map<std::string, AXEntity* > StringEntityMap;
typedef std::map<std::string, EntityCreatorPtr> StringEntityFuncMap;
typedef std::vector< AXLightEntity* > LightVector;

class AXSceneListener {
public:
	AXSceneListener() {}
	virtual ~AXSceneListener() {}

	virtual void Listen(AXScene *Scene)=0;
};

class AXScene : public AXUpdatableSystemComponent {
public:
	AXScene();
	virtual ~AXScene();

	AXResult Initialize();
	void Shutdown();

	AXResult Start();
	void Update();
	void Stop();

	/*	Functions dealing with entities */
	template<class Type> void RegisterEntityType(std::string Name) { _EntityRegistrar[Name] = &AXScene::_EntityCreator<Type>; }
	AXEntity* CreateEntity(const std::string &Type, const std::string &Name, const std::string &Parameters);
	AXEntity* GetEntity(const std::string &Name);
	void RemoveEntity(const std::string &Name);

	/*	Scene functions */
	AXSceneNode* GetRoot();
	void ClearScene();
	void AddSceneListener(AXSceneListener *Listener);

	void Debug();

	/*	Light management */
	AXLightEntity* CreateLight(const std::string &Name);
	void GetLights(LightVector *Lights, AXVector3 *WorldPosition, unsigned int MaxLights);

	/*	Camera management */
	AXCamera* CreateCamera(const std::string &Name, float Speed);
	AXResult ActivateCamera(const std::string &Name);
	AXVector3* GetViewerPosition();

private:

	template<class Type>
	AXEntity* _EntityCreator(const std::string Name, const std::string Parameters) {
		if(_EntityMap.find(Name) != _EntityMap.end()) return NULL;

		Type* Entity = new Type(Name);
		Entity->Configure(Parameters);
		_EntityMap[Name] = Entity;

		return Entity;
	}

	AXSceneNode* _RootSceneNode;
	StringEntityMap _EntityMap;
	StringEntityFuncMap _EntityRegistrar;

	/*	Listeners */
	typedef std::vector<AXSceneListener*> ListenerVector;
	ListenerVector _Listeners;


	/*	Lights */
	LightVector _LightList;

	/*	Camera */
	AXCamera *_CurrentCamera;
};


#endif
