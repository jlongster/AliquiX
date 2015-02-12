/* --------------------------------------------------------------
	File:			AXSceneNode.cpp
	Description:	This file implementes the scene node.

	Date:			January 2, 2006
	Author:			James Long
-------------------------------------------------------------- */

#include "..\\AXCore.h"
#include "AXScene.h"
#include "AXSceneNode.h"
#include "AXEntity.h"

using std::list;
using std::map;

AXSceneNode::AXSceneNode() : _Index(0), _ParentNode(NULL), _AttachedEntity(NULL) {
	AXMatrixIdentity(&_WorldMatrix);
	AXMatrixIdentity(&_LocalMatrix);
	Translate(0.0f, 0.0f, 0.0f);
	SetTranslationSpeed(0.0f);
	Rotate(0.0f, AXVector3(0.0f, 1.0f, 0.0f));
	SetRotationSpeed(0.0f);
}
AXSceneNode::~AXSceneNode() {
	
	if(_AttachedEntity)
		AXBaseSystem::GetSceneMgr()->RemoveEntity(_AttachedEntity->GetName());

	NodeList::iterator Iter;
	for(Iter = _ChildrenNodes.begin(); Iter != _ChildrenNodes.end(); Iter++) {
		SafeDelete(*Iter);
	}
	_ChildrenNodes.clear(); 
}

void AXSceneNode::Update() {
	if(_ParentNode) _WorldMatrix = _ParentNode->_WorldMatrix;

	AXMatrixMultiply(&_WorldMatrix, &_LocalMatrix, &_WorldMatrix);
	_WorldPosition.x = _WorldMatrix._41;
	_WorldPosition.y = _WorldMatrix._42;
	_WorldPosition.z = _WorldMatrix._43;

	if(_AttachedEntity) _AttachedEntity->Update();

	NodeList::iterator NodeIter;
	for(NodeIter = _ChildrenNodes.begin(); NodeIter != _ChildrenNodes.end(); NodeIter++) {
		(*NodeIter)->Update();
	}
}

void AXSceneNode::Execute() {
	if(_AttachedEntity) _AttachedEntity->Execute();

	NodeList::iterator NodeIter;
	for(NodeIter = _ChildrenNodes.begin(); NodeIter != _ChildrenNodes.end(); NodeIter++) {
		(*NodeIter)->Execute();
	}
}

AXSceneNode* AXSceneNode::CreateChild() {
	AXSceneNode *Node = new AXSceneNode;
	Node->_ParentNode = this;

	if(_ChildrenNodes.empty())
		Node->_Index = 0;
	else
		Node->_Index = _ChildrenNodes.back()->_Index+1;

	_ChildrenNodes.push_back(Node);

	return Node;
}

AXSceneNode* AXSceneNode::CreateChildWithEntity(AXEntity *Entity) {
	AXSceneNode *Node = new AXSceneNode;
	Node->_ParentNode = this;
	if(_ChildrenNodes.empty())
		Node->_Index = 0;
	else
		Node->_Index = _ChildrenNodes.back()->_Index+1;
	Node->AttachEntity(Entity);

	_ChildrenNodes.push_back(Node);

	return Node;
}

AXSceneNode* AXSceneNode::GetChild(std::string EntityName) {
	AXSceneNode *Child;
	NodeList::iterator Iter;
	for(Iter = _ChildrenNodes.begin(); Iter != _ChildrenNodes.end(); Iter++) {
		if((*Iter)->_AttachedEntity->GetName() == EntityName) {
			Child = (*Iter);
			return Child;
		}
	}
	LOG_DBG1("WARNING - Could not find scenenode child - name %s", EntityName.c_str());
	return NULL;
}

AXSceneNode* AXSceneNode::GetChild(unsigned int Index) {
	AXSceneNode* Child;
	NodeList::iterator Iter;
	unsigned int Count = 0;
	for(Iter = _ChildrenNodes.begin(); Iter != _ChildrenNodes.end(); Iter++) {
		if(Count == Index) {
			Child = (*Iter);
			return Child;
		}
		Count++;
	}
	LOG_DBG1("WARNING - Could not find scenenode child - index %d", Index);
	return NULL;
}

//shared_ptr<AXSceneNode> AXSceneNode::RemoveChild(string EntityName) {
//	shared_ptr<AXSceneNode>	Child;
//	NodeList::iterator Iter;
//	for(Iter = _ChildrenNodes.begin(); Iter != _ChildrenNodes.end(); Iter++) {
//		if((*Iter)->_AttachedEntity->GetName() == EntityName) {
//			Child = (*Iter);
//			_ChildrenNodes.erase(Iter);
//			return Child;
//		}
//	}
//	LOG_LOG_DBG1("WARNING - Could not find scenenode child to remove - name %s", EntityName.c_str());
//	return Child;
//}
//
//shared_ptr<AXSceneNode> AXSceneNode::RemoveChild(unsigned int Index) {
//	shared_ptr<AXSceneNode>	Child;
//	NodeList::iterator Iter;
//	for(Iter = _ChildrenNodes.begin(); Iter != _ChildrenNodes.end(); Iter++) {
//		if((*Iter)->_Index == Index) {
//			Child = (*Iter);
//			_ChildrenNodes.erase(Iter);
//			return Child;
//		}
//	}
//	LOG_LOG_DBG1("WARNING - Could not find scenenode child to remove - index %d", Index);
//	return Child;
//}

void AXSceneNode::RemoveAndDestroyChild(std::string EntityName) {
	NodeList::iterator Iter;
	for(Iter = _ChildrenNodes.begin(); Iter != _ChildrenNodes.end(); Iter++) {
		AXSceneNode *Node = *Iter;
		if(Node->_AttachedEntity->GetName() == EntityName) {
			SafeDelete(Node);
			_ChildrenNodes.erase(Iter);
			return;
		}
	}
	LOG_DBG1("WARNING - Could not find scenenode child to remove and destroy - name %s", EntityName.c_str());
}

void AXSceneNode::RemoveAndDestroyChild(unsigned int Index) {
	NodeList::iterator Iter;
	unsigned int Count = 0;
	for(Iter = _ChildrenNodes.begin(); Iter != _ChildrenNodes.end(); Iter++) {
		AXSceneNode *Node = *Iter;
		if(Count == Index) {
			SafeDelete(Node);
			_ChildrenNodes.erase(Iter);
			return;
		}
		Count++;
	}
	LOG_DBG1("WARNING - Could not find scenenode child to remove and destroy - index %d", Index);
}

void AXSceneNode::RemoveAndDestroyChildren() {
	NodeList::iterator Iter;
	for(Iter = _ChildrenNodes.begin(); Iter != _ChildrenNodes.end(); Iter++) {
		AXSceneNode *Node = *Iter;
		SafeDelete(Node);
	}
	_ChildrenNodes.clear();
}

AXEntity* AXSceneNode::AttachEntity(AXEntity *Entity) {
	if(!Entity) return NULL;
	if(Entity->_AssociatedSceneNode) {
		LOG_SYS1("Failed attached entity '%s', it's already attached to a scene node!", Entity->GetName().c_str());
		return NULL;
	}

	Entity->_AssociatedSceneNode = this;
	_AttachedEntity = Entity;
	return Entity;
}

AXEntity* AXSceneNode::GetAttachedEntity() {
	return _AttachedEntity;
}

AXEntity* AXSceneNode::DetachEntity() {
	AXEntity* Entity = _AttachedEntity;
	_AttachedEntity = NULL;
	return Entity;
}

void AXSceneNode::Translate(float x, float y, float z) {
	AXMatrix4 Translate;
	AXMatrixTranslation(&Translate, x, y, z);
	AXMatrixMultiply(&_LocalMatrix, &_LocalMatrix, &Translate);
}

void AXSceneNode::SetTranslation(AXVector3 Start, AXVector3 Direction) {
	_Translation = Start;
	_TranslationDir = Direction;
}

void AXSceneNode::SetTranslationSpeed(float Speed) {
	_TranslationSpeed = 0.0f;
}

void AXSceneNode::Rotate(float Angle, AXVector3 Axis) {
	AXMatrix4 Rotate;
	AXMatrixRotationAxis(&Rotate, &Axis, Angle);
	AXMatrixMultiply(&_LocalMatrix, &_LocalMatrix, &Rotate);
}

void AXSceneNode::SetRotationSpeed(float Speed) {
	_RotationSpeed = 0.0f;
}

void AXSceneNode::Scale(float x, float y, float z) {
	AXMatrix4 Scale;
	AXMatrixScale(&Scale, x, y, z);
	AXMatrixMultiply(&_LocalMatrix, &_LocalMatrix, &Scale);
}

void AXSceneNode::Debug() {
	LOG_DBG("Showing children of called scenenode...");
	_DisplayChildren(1);
}

void AXSceneNode::_DisplayChildren(unsigned int Level) {
	NodeList::iterator Iter;
	for(Iter = _ChildrenNodes.begin(); Iter != _ChildrenNodes.end(); Iter++) {
		std::string Message;
		for(unsigned int j=0; j<Level; j++)
			Message += " ";

		Message += "Child %d";

		if((*Iter)->_AttachedEntity)
			Message += " (Entity: " + (*Iter)->_AttachedEntity->GetName() + ") ";
		LOG_DBG1(Message.c_str(), (*Iter)->_Index);
		(*Iter)->_DisplayChildren(Level+1);
	}
}

