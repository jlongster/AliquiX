/* --------------------------------------------------------------
	File:			AXEntity.h
	Description:	This file defines an "entity" which is a base object that others in the system
					will derive from.

	Date:			December 23, 2005
	Author:			James Long
-------------------------------------------------------------- */

#if !defined(AXENTITY_INCLUDED)
#define AXENTITY_INCLUDED

#include "AXSceneNode.h"

#include <string>

class AXEntity {
	friend class AXScene;
	friend class AXSceneNode;

public:
	AXEntity(std::string Name) : _Name(Name), _AssociatedSceneNode(NULL) {}
	virtual ~AXEntity() {}

	std::string GetName() { return _Name; }
	AXSceneNode* GetSceneNode() { return _AssociatedSceneNode; }

	virtual void Configure(const std::string &Parameters)=0;
	virtual void Update()=0;
	virtual void Execute()=0;

	std::string GetType() { return _Type; }

private:
	AXEntity() {}

	AXSceneNode* _AssociatedSceneNode;

	std::string _Name;
	std::string _Type;
};


#endif