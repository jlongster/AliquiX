/* --------------------------------------------------------------
	File:			AXMaterial.h
	Description:	Defines properties for an instances mesh chunk.

	Date:			January 18, 2006
	Author:			James Long
-------------------------------------------------------------- */

#if !defined(AXMATERIAL_INCLUDED)
#define AXMATERIAL_INCLUDED

#include "..\\AXUtilities\\AXMath.h"
#include "AXColor.h"
#include "AXRenderStates.h"

struct AXMaterial {
	AXMaterial() {
		Diffuse = AXColorToARGB(1.0f, 1.0f, 1.0f, 1.0f);
		Specular = AXColorToARGB(1.0f, 1.0f, 1.0f, 1.0f);
		Shininess = 0.0f;
		Opacity = 1.0f;

		DiffuseSource = AX_COLORSOURCE_VERTEX;
		SpecularSource = AX_COLORSOURCE_VERTEX;
	}

	AXColorARGBf Diffuse;
	AXColorARGBf Specular;
	float Shininess;
	float Opacity;
	//unsigned int *textures;

	AXColorSource DiffuseSource;
	AXColorSource SpecularSource;
};

#endif