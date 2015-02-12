/* --------------------------------------------------------------
	File:			AXCamera.cpp
	Description:	See AXCamera.h.

	Date:			February 13, 2005
	Author:			James Long
-------------------------------------------------------------- */


#include "AXCamera.h"
#include "..\\AXGraphics.h"
#include "..\\AXCore.h"
#include "..\\AXUtilities\\AXStringFunctions.h"

AXCamera::AXCamera(const std::string &Name) : AXEntity(Name) {
}

AXCamera::~AXCamera() {
}

void AXCamera::Configure(const std::string &Parameters) {
	_Speed = AXStringConvertToFloat(Parameters);
	_Position = AXVector3(0.0f, 0.0f, 0.0f);
	AXQuaternionIdentity(&_QuatView);
	AXMatrixIdentity(&_MatHelper);
	_RotateXAxis(0.0f);
	_Input = AXBaseSystem::GetInput();

	_Activated = 0;
}

void AXCamera::Update() {
	if(_Activated) {
		if(_Input->MouseStillDown(AX_MOUSE0)) {
			AXDimension2D MouseChange = _Input->MousePosRel();
			_RotateYAxis(MouseChange.x * _Speed/2500.0f);
			_RotateXAxis(MouseChange.y * _Speed/3500.0f);
		}

		if(_Input->KeyStillDown(AX_W)) {
			_Position += _GetAxisZ()*(_Speed*AXClock::dT);
		}
		if(_Input->KeyStillDown(AX_S)) {
			_Position += (-_GetAxisZ())*(_Speed*AXClock::dT);
		}
		if(_Input->KeyStillDown(AX_A)) {
			_Position += (-_GetAxisX())*(_Speed*AXClock::dT);
		}
		if(_Input->KeyStillDown(AX_D)) {
			_Position += _GetAxisX()*(_Speed*AXClock::dT);
		}
	}
}

void AXCamera::Execute() {
	if(_Activated) {
		AXMatrixTranslation(&_MatTrans, -_Position.x, -_Position.y, -_Position.z);
		AXMatrixRotationQuaternion(&_MatRot, &AXQuaternion(-_QuatView.x,-_QuatView.y,-_QuatView.z,_QuatView.w));
		AXMatrixMultiply(&_MatRot, &_MatTrans, &_MatRot);

		AXMatrix4 *WorldMatrix = GetSceneNode()->GetWorldMatrix();
		AXMatrixInverse(WorldMatrix, NULL, WorldMatrix);
		AXMatrixMultiply(&_MatRot, WorldMatrix, &_MatRot);

		//D3DXMatrixIdentity(&MatHelper);
		//Position.x = -(MatHelper[8]*Speed + MatHelper[0]*Speed);
		//Position.y = -(MatHelper[9]*Speed + MatHelper[1]*Speed);
		//Position.z = -(MatHelper[10]*Speed + MatHelper[2]*Speed);

		AXBaseSystem::GetRenderer()->LoadMatrix(AX_MATRIX_VIEW, &_MatRot);
	}
	_WorldPosition = _Position+*(GetSceneNode()->GetWorldPosition());
}

AXVector3* AXCamera::GetWorldPosition() {
	return &_WorldPosition;
}

void AXCamera::_RotateXAxis(float angle) {
	AXQuaternion QuatRot;
	AXQuaternionRotationAxis(&QuatRot, &_GetAxisX(), angle);
	_QuatView *= QuatRot;
	AXQuaternionNormalize(&_QuatView, &_QuatView);
	AXMatrixRotationQuaternion(&_MatHelper, &_QuatView);
}

void AXCamera::_RotateYAxis(float angle) {
	AXQuaternion QuatRot;
	AXQuaternionRotationAxis(&QuatRot, &AXVector3(0.0f, 1.0f, 0.0f), angle);
	_QuatView *= QuatRot;
	AXQuaternionNormalize(&_QuatView, &_QuatView);
	AXMatrixRotationQuaternion(&_MatHelper, &_QuatView);
}

void AXCamera::_RotateZAxis(float angle) {
	AXQuaternion QuatRot;
	AXQuaternionRotationAxis(&QuatRot, &_GetAxisZ(), angle);
	_QuatView *= QuatRot;
	AXQuaternionNormalize(&_QuatView, &_QuatView);
	AXMatrixRotationQuaternion(&_MatHelper, &_QuatView);
}

AXVector3 AXCamera::_GetAxisX() {
	return AXVector3(_MatHelper._11, _MatHelper._12, _MatHelper._13);
}
AXVector3 AXCamera::_GetAxisY() {
	return AXVector3(_MatHelper._21, _MatHelper._22, _MatHelper._23);
}
AXVector3 AXCamera::_GetAxisZ() {
	return AXVector3(_MatHelper._31, _MatHelper._32, _MatHelper._33);
}
