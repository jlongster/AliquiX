/* --------------------------------------------------------------
	File:			AXRenderStates.h
	Description:	Defines render states (will be expanded later)

	Date:			January 5, 2006
	Author:			James Long
-------------------------------------------------------------- */

#if !defined(AXRENDERSTATES_INCLUDED)
#define AXRENDERSTATES_INCLUDED

#define AX_MAXRENDERSTATES 256

/*	Render states */
enum AXRenderState {
	AX_RS_LIGHTING = 1,
	AX_RS_ZENABLE = 2,
	AX_RS_ZWRITE = 3,
	AX_RS_CULLMODE = 4,
	AX_RS_FILLMODE = 5,
	AX_RS_DIFFUSESOURCE = 6,
	AX_RS_SPECULARSOURCE = 7
};

enum AXCullMode {
	AX_CULL_NONE = 1,
	AX_CULL_CW = 2,
	AX_CULL_CCW = 3,
};

enum AXFillMode {
	AX_FILL_POINT = 1,
	AX_FILL_WIREFRAME = 2,
	AX_FILL_SOLID = 3
};

enum AXColorSource {
	AX_COLORSOURCE_MATERIAL = 1,
	AX_COLORSOURCE_VERTEX = 2
};

#endif