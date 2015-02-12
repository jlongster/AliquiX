/* --------------------------------------------------------------
	File:			AXMath.h
	Description:	Defines math objects.

					IMPORTANT: All this file does is include the directx math library.  I don't have
					time to write my own or even abstract this out
	Date:			May 30, 2005
	Author:			James Long
-------------------------------------------------------------- */

#if !defined(AXMATH_INCLUDED)
#define AXMATH_INCLUDED

#ifdef _DEBUG
#define D3D_DEBUG_INFO
#endif

#ifdef WIN32
#include <d3dx9math.h>
#else
#error Quaternion library requires DirectX
#endif

#include "AXMathFunctions.h"

typedef D3DXVECTOR2 AXVector2;
typedef D3DXVECTOR3 AXVector3;
typedef D3DXVECTOR4 AXVector4;
typedef D3DXMATRIX AXMatrix4;
typedef D3DXQUATERNION AXQuaternion;

/*	Vector */

inline float AXVector2Length(AXVector2 *Vec) {
	return D3DXVec2Length(Vec);
}
inline float AXVector3Length(AXVector3 *Vec) {
	return D3DXVec3Length(Vec);
}
inline float AXVector4Length(AXVector4 *Vec) {
	return D3DXVec4Length(Vec);
}

inline AXVector2* AXVector2Normalize(AXVector2 *Out, AXVector2 *Vec) {
	return D3DXVec2Normalize(Out, Vec);
}

inline AXVector3* AXVector3Normalize(AXVector3 *Out, AXVector3 *Vec) {
	return D3DXVec3Normalize(Out, Vec);
}

inline AXVector4* AXVector4Normalize(AXVector4 *Out, AXVector4 *Vec) {
	return D3DXVec4Normalize(Out, Vec);
}

inline AXVector2* AXVector2Subtract(AXVector2 *Out, AXVector2 *Vec1, AXVector2 *Vec2) {
	return D3DXVec2Subtract(Out, Vec1, Vec2);
}

inline AXVector3* AXVector3Subtract(AXVector3 *Out, AXVector3 *Vec1, AXVector3 *Vec2) {
	return D3DXVec3Subtract(Out, Vec1, Vec2);
}

inline AXVector4* AXVector4Subtract(AXVector4 *Out, AXVector4 *Vec1, AXVector4 *Vec2) {
	return D3DXVec4Subtract(Out, Vec1, Vec2);
}

inline AXVector4* AXVector2Transform(AXVector4 *Out, AXVector2 *Vec, AXMatrix4 *Mat) {
	return D3DXVec2Transform(Out, Vec, Mat);
}

inline AXVector4* AXVector3Transform(AXVector4 *Out, AXVector3 *Vec, AXMatrix4 *Mat) {
	return D3DXVec3Transform(Out, Vec, Mat);
}

inline AXVector4* AXVector4Transform(AXVector4 *Out, AXVector4 *Vec, AXMatrix4 *Mat) {
	return D3DXVec4Transform(Out, Vec, Mat);
}

inline float AXVector2Dot(AXVector2 *Out, AXVector2 *Vec) {
	return D3DXVec2Dot(Out, Vec);
}

inline float AXVector2Dot(AXVector3 *Out, AXVector3 *Vec) {
	return D3DXVec3Dot(Out, Vec);
}

inline float AXVector2Dot(AXVector4 *Out, AXVector4 *Vec) {
	return D3DXVec4Dot(Out, Vec);
}

/*	Matrix */

inline AXMatrix4* AXMatrixIdentity(AXMatrix4 *Out) {
	return D3DXMatrixIdentity(Out);
}

inline AXMatrix4* AXMatrixTranslation(AXMatrix4 *Out, float x, float y, float z) {
	return D3DXMatrixTranslation(Out, x, y, z);
}

inline AXMatrix4* AXMatrixRotationAxis(AXMatrix4 *Out, AXVector3 *Axis, float Angle) {
	return D3DXMatrixRotationAxis(Out, Axis, Angle);
}

inline AXMatrix4* AXMatrixRotationQuaternion(AXMatrix4 *Out, AXQuaternion *Quat) {
	return D3DXMatrixRotationQuaternion(Out, Quat);
}

inline AXMatrix4* AXMatrixMultiply(AXMatrix4 *Out, AXMatrix4 *Mat1, AXMatrix4 *Mat2) {
	return D3DXMatrixMultiply(Out, Mat1, Mat2);
}

inline AXMatrix4* AXMatrixScale(AXMatrix4 *Out, float x, float y, float z) {
	return D3DXMatrixScaling(Out, x, y, z);
}

inline AXMatrix4* AXMatrixInverse(AXMatrix4 *Out, float *Det, AXMatrix4 *Mat) {
	return D3DXMatrixInverse(Out, Det, Mat);
}

/*	Quaternion */

inline AXQuaternion* AXQuaternionIdentity(AXQuaternion *Out) {
	return D3DXQuaternionIdentity(Out);
}

inline AXQuaternion* AXQuaternionNormalize(AXQuaternion *Out, AXQuaternion *Quat) {
	return D3DXQuaternionNormalize(Out, Quat);
}

inline AXVector3* AXQuaternionTransformVector(AXVector3 *Out, AXVector3 *Vec, AXQuaternion *Quat) {
	AXMatrix4 MatRot;
	AXMatrixRotationQuaternion(&MatRot, Quat);

	Out->x = Vec->x * MatRot._11 + Vec->y * MatRot._21 + Vec->z * MatRot._31 + MatRot._41; 
	Out->y = Vec->x * MatRot._12 + Vec->y * MatRot._22 + Vec->z * MatRot._32 + MatRot._42;
	Out->z = Vec->x * MatRot._13 + Vec->y * MatRot._23 + Vec->z * MatRot._33 + MatRot._43;

	return Out;
}

inline AXQuaternion* AXQuaternionRotationAxis(AXQuaternion *Quat, AXVector3 *Axis, float Angle) {
	return D3DXQuaternionRotationAxis(Quat, Axis, Angle);
}

#endif