/* --------------------------------------------------------------
	File:			AXEntity.cpp
	Description:	This file defines an "entity" which is a base object that others in the system
					will derive from.

	Date:			December 26, 2005
	Author:			James Long
-------------------------------------------------------------- */

#include "AXEntity.h"

AXEntity::AXEntity() {
}
AXEntity::AXEntity(std::string Name) : _Name(Name) {
}
AXEntity::AXEntity(std::string Name, std::string Parameters) : _Name(Name) {
	Configure(Parameters);
}
AXEntity::~AXEntity() {
}

std::string AXEntity::GetName() {
	return _Name;
}