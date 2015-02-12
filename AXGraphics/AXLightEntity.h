/* --------------------------------------------------------------
	File:			AXLightEntity.h
	Description:	A simple class that holds properties for a light.

	Date:			May 11, 2005
	Author:			James Long
-------------------------------------------------------------- */

#if !defined(AXLIGHTENTITY_INCLUDED)
#define AXLIGHTENTITY_INCLUDED

#include "AXEntity.h"
#include "AXColor.h"
#include "AXUtilities\\AXMath.h"

#include <string>

enum AXLightType {
	AX_LIGHT_POINT,
	AX_LIGHT_SPOT,
	AX_LIGHT_DIRECTIONAL
};

class AXLightEntity : public AXEntity {
public:
	AXLightEntity(const std::string &Name);
	virtual ~AXLightEntity();

	void Configure(const std::string &Parameters);
	void Update();
	void Execute();

	void SaveProperties();
	void Enable();
	void Disable();

	unsigned int Index;

	void CacheEffect(AXVector3 *WorldPosition);
	float CachedEffect;

	/* Light properties */
	AXLightType Type;
	AXColorARGBf Diffuse;
	AXColorARGBf Specular;
	AXColorARGBf Ambient;
	AXVector3 Position;
	AXVector3 Direction0;
	AXVector3 Direction1;
	float Range;
	float Attenuation0;
	float Attenuation1;
	float Attenuation2;
	float Brightness;
	float Falloff;
	float Phi;
	float Theta;
	unsigned char Dynamic;

private:
	unsigned char _IsSet;

	/*	Cache data for quick lookups */
	AXVector3 _TempVec;
	AXVector4 _TempVec4;
	float _cosTheta;
	float _cosPhi;

	float _CalculateAttenuation(float distance);
};

struct DescendingLightSort {
	unsigned char operator()(AXLightEntity* &l1, AXLightEntity* &l2) {

		// the amount should already be calculated for optimization
		if(l1->CachedEffect > l2->CachedEffect)
			return 1;
		else
			return 0;
	}
};


#endif
