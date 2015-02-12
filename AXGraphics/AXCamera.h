/* --------------------------------------------------------------
	File:			AXCamera.h
	Description:	Implements a simple camera working with quaternions.

	Date:			February 12, 2005
	Author:			James Long
-------------------------------------------------------------- */

#if !defined(AXCAMERA_INCLUDED)
#define AXCAMERA_INCLUDED

#include "AXCore.h"
#include "AXEntity.h"
#include "..\\AXUtilities\\AXMath.h"
#include "..\\AXUtilities\\AXMath.h"
#include "..\\AXUtilities\\AXMath.h"

class AXCamera : public AXEntity {
public:
	AXCamera(const std::string &Name);
	virtual ~AXCamera();

	void Configure(const std::string &Parameters);
	void Update();
	void Execute();

	AXVector3* GetWorldPosition();

	void Activate() { _Activated = 1; }
	void Deactivate() { _Activated = 0; }

private:
	void _RotateXAxis(float angle);
	void _RotateYAxis(float angle);
	void _RotateZAxis(float angle);

	AXVector3 _GetAxisX();
	AXVector3 _GetAxisY();
	AXVector3 _GetAxisZ();

	unsigned char _Activated;

	float _Speed;

	AXVector3 _WorldPosition;
	AXVector3 _Position;

	AXQuaternion _QuatView;
	AXMatrix4 _MatHelper, _MatTrans, _MatRot;

	AXInput *_Input;

};

#endif