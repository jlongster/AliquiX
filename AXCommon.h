/* --------------------------------------------------------------
	File:			AXCommon.h
	Description:	This file defines some types that should be used throughout the whole program.
					It includes return types, failure and succeed macros, helpful memory management macros, and more.
	Date:			June 29, 2005
	Author:			James Long
------------------------------------------------------------- */

#if !defined(AXCOMMON_INCLUDED)
#define AXCOMMON_INCLUDED

enum AXResult {
	AXSUCCESS = 0,
	AXFAILURE = 1,
	AXFILENOTFOUND = 2
};

#define AXSUCCEEDED(x)	((AXResult)x == 0)
#define AXFAILED(x)		((AXResult)x >  0)

#ifndef NULL
#define NULL 0
#endif

#define SafeDelete(x)		if(x) { delete x; x = NULL; }
#define SafeDeleteArr(x)	if(x) { delete[] x; x = NULL; }
#define SafeRelease(x)		if(x) { x->Release(); x = NULL; }

#define ERRORINT 0xFFFFFFFF
#define ERRORFLOAT static_cast<float>(0xFFFFFFFF)

#endif
