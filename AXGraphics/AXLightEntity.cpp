/* --------------------------------------------------------------
	File:			AXLightEntity.cpp
	Description:	See AXLightEntity.h.

	Date:			May 11, 2005
	Author:			James Long
-------------------------------------------------------------- */

#include "AXLightEntity.h"
#include "AXGraphics.h"

AXLightEntity::AXLightEntity(const std::string &Name) : AXEntity(Name) {
}
AXLightEntity::~AXLightEntity() {
}

void AXLightEntity::Configure(const std::string &Parameters) {
	Range = 100.0f;
	Brightness = 1.0f;
	Attenuation0 = 0.0f;
	Attenuation1 = 1.0f;
	Attenuation2 = 0.0f;

	Diffuse = AXColorToARGB(1.0f, 1.0f, 1.0f, 1.0f);
	Specular = AXColorToARGB(1.0f, 1.0f, 1.0f, 1.0f);
	Ambient = AXColorToARGB(0.0f, 0.0f, 0.0f, 0.0f);

	Position = AXVector3(0.0f, 0.0f, 0.0f);
	Direction0 = AXVector3(1.0f, 0.0f, 0.0f);

	Phi = AX_PI/2.0f;
	Theta = AX_PI/4.0f;
	Falloff = 1.0f;

	Dynamic = 0;
	CachedEffect = 0.0f;

	_IsSet = 0;
}

void AXLightEntity::Update() {
	if(Dynamic) {
		AXSceneNode *Node = GetSceneNode();

		Position = *(Node->GetWorldPosition());

		if(Type == AX_LIGHT_SPOT) {
			AXMatrix4 Mat = *(Node->GetWorldMatrix());
			Mat._41 = 0;
			Mat._42 = 0;
			Mat._43 = 0;
			AXVector3Transform(&_TempVec4, &Direction0, &Mat);
			Direction1.x = _TempVec4.x;
			Direction1.y = _TempVec4.y;
			Direction1.z = _TempVec4.z;
		}

		SaveProperties();
	}
	
	if(!_IsSet) {
		AXSceneNode *Node = GetSceneNode();

		if(!Dynamic) {
			Position = *(Node->GetWorldPosition());
			if(Type == AX_LIGHT_SPOT) {
				AXMatrix4 Mat = *(Node->GetWorldMatrix());
				Mat._41 = 0;
				Mat._42 = 0;
				Mat._43 = 0;
				AXVector3Transform(&_TempVec4, &Direction0, &Mat);
				Direction1.x = _TempVec4.x;
				Direction1.y = _TempVec4.y;
				Direction1.z = _TempVec4.z;
			}
		}

		if(Node) {
			if(Type != AX_LIGHT_DIRECTIONAL) {
				AXMeshEntity *Mesh = dynamic_cast<AXMeshEntity*>(AXBaseSystem::GetSceneMgr()->CreateEntity("Mesh", GetName()+"Box", "Box"));
				Mesh->AssignEffect(0);
				AXSceneNode *LightBox = Node->CreateChildWithEntity(Mesh);
				LightBox->Scale(0.1f, 0.1f, 0.1f);

				if(Type == AX_LIGHT_SPOT) {
					AXMeshEntity *MeshDir = dynamic_cast<AXMeshEntity*>(AXBaseSystem::GetSceneMgr()->CreateEntity("Mesh", GetName()+"BoxDir", "Box"));
					MeshDir->AssignEffect(0);
					AXSceneNode *BoxDir = Node->CreateChildWithEntity(MeshDir);
					BoxDir->Scale(0.05f, 0.05f, 0.05f);

					AXVector3 NormalizedDir;
					AXVector3Normalize(&NormalizedDir, &Direction0);
					BoxDir->Translate(NormalizedDir.x*0.3f, NormalizedDir.y*0.3f, NormalizedDir.z*0.3f);
				}
			}
		}
		if(!Dynamic) SaveProperties();
		_IsSet = 1;
	}
}

void AXLightEntity::Execute() {
}

void AXLightEntity::SaveProperties() {
	AXBaseSystem::GetRenderer()->SetLight(Index, this);

	if(Type == AX_LIGHT_SPOT) {
		_cosTheta = cosf(Theta);
		_cosPhi = cosf(Phi);
	}
}

void AXLightEntity::Enable() {
	AXBaseSystem::GetRenderer()->EnableLight(Index);
}

void AXLightEntity::Disable() {
	AXBaseSystem::GetRenderer()->DisableLight(Index);
}

void AXLightEntity::CacheEffect(AXVector3 *WorldPosition) {
	if(Type == AX_LIGHT_DIRECTIONAL) {
		CachedEffect = Brightness;
	}
	else {
		AXVector3 *l1pos = GetSceneNode()->GetWorldPosition();

		_TempVec.x = l1pos->x - WorldPosition->x;
		_TempVec.y = l1pos->y - WorldPosition->y;
		_TempVec.z = l1pos->z - WorldPosition->z;
		float l1distance = AXVector3Length(&_TempVec);

		std::string Name = GetName();

		if(l1distance >= Range) {
			CachedEffect = 0.0f;
		}
		else if(Type == AX_LIGHT_POINT) {
			CachedEffect = Brightness/(Attenuation0 + Attenuation1*l1distance + Attenuation2*l1distance*l1distance);
		}
		else if(Type == AX_LIGHT_SPOT) {
			//AXVector3 LightToPos;
			//AXVector3Subtract(&LightToPos, WorldPosition, l1pos);
			//AXVector3Normalize(&LightToPos, &LightToPos);
			//float cosAngle = AXVector2Dot(&LightToPos, &Direction1);
			//CachedEffect = AXClamp((cosAngle-_cosPhi)/(_cosTheta-_cosPhi), 0.0f, 1.0f);
			CachedEffect = 1.0f/l1distance;
		}
	}
}

