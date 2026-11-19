// Earth & Beyond client (reconstruction)
/*
**	The shake's constructor alone (Ship.Shake: ForceX, ForceY, ForceZ, Damage), for the objects that read a shake:
**	parsing it numbers T<float> (the Delta templates' per-class globals, workflow.md 3.4) by itself, before <string>'s
**	guard, as the client's cameraeffects and bridge_int objects do (CRT entries 533 / 212: 0x6E46F0, 0x53D0A0). The
**	aux-data constructors are gated by family in aux_roots.h (the shake's: SHIP or SHAKE).
**	Include this header first, before anything that includes aux_roots.h (the macros must come first).
**
**	// NAME 1 [aux_shake.h]: provisional file name (the client's headers are not named by any string)
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __AUX_SHAKE_H
#define __AUX_SHAKE_H

#include "..\shared\auxdata\delta_numbering.h"
#define AUX_CTORS_SHAKE
#include "aux_roots.h"

#endif //__AUX_SHAKE_H
