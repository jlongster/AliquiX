/* --------------------------------------------------------------
	File:			AXSceneNode.h
	Description:	This file defines an abstract scene node object.

	Date:			December 18, 2005
	Author:			James Long
-------------------------------------------------------------- */


#if !defined(AXSCENENODE_INCLUDED)
#define AXSCENENODE_INCLUDED

class AXEntity;

#include "..\\AXUtilities\\AXMath.h"
#include "..\\AXUtilities\\AXMath.h"

#include <string>
#include <list>

class AXSceneNode {
	friend class AXScene;

public:
	AXSceneNode();
	virtual ~AXSceneNode();

	void Update();
	void Execute();

	unsigned int GetIndex() { return _Index; }

	AXSceneNode* CreateChild();
	AXSceneNode* CreateChildWithEntity(AXEntity* Entity);
	AXSceneNode* GetChild(std::string EntityName);
	AXSceneNode* GetChild(unsigned int Index);
	//boost::shared_ptr<AXSceneNode> RemoveChild(std::string EntityName);
	//boost::shared_ptr<AXSceneNode> RemoveChild(unsigned int Index);
	void RemoveAndDestroyChild(std::string EntityName);
	void RemoveAndDestroyChild(unsigned int Index);
	void RemoveAndDestroyChildren();

	AXEntity* AttachEntity(AXEntity *Entity);
	AXEntity* GetAttachedEntity();
	AXEntity* DetachEntity();

	/*	These haven't fully been implemented yet.
		TODO: Figure out what kind of animation system I need */
	void Translate(float x, float y, float z);
	void SetTranslation(AXVector3 Start, AXVector3 Direction);
	void SetTranslationSpeed(float Speed);
	void Rotate(float Angle, AXVector3 Axis);
	void SetRotationSpeed(float Speed);
	void Scale(float x, float y, float z);

	AXVector3* GetWorldPosition() { return &_WorldPosition; }
	AXMatrix4* GetWorldMatrix() { return &_WorldMatrix; }

	void Debug();

private:

	void _DisplayChildren(unsigned int Level=0);

	AXSceneNode* _ParentNode;
	unsigned int _Index;

	typedef std::list< AXSceneNode* > NodeList;
	NodeList _ChildrenNodes;

	AXEntity* _AttachedEntity;

	float _RotationSpeed;
	float _RotationAngle;
	AXVector3 _RotationAxis;
	float _TranslationSpeed;
	AXVector3 _Translation;
	AXVector3 _TranslationDir;

	AXMatrix4 _WorldMatrix;
	AXMatrix4 _LocalMatrix;
	AXVector3 _WorldPosition;
};

#endif