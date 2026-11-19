// Earth & Beyond client (reconstruction)
/*
**	The decoration constructor: Decoration (0x6F9C30), for the radar's objects. The aux-data constructors are
**	gated by family in aux_roots.h: an object compiles (and numbers at start-up, the Delta templates' per-class
**	globals: workflow.md 3.4) exactly the families whose constructors it sees. The constructor's two template
**	fields, AppearsInRadar and RadarRange, number T<bool> then T<float> while the object is parsed: the client's
**	radar_system_data_cache and gutter_data objects number just these two, in this order, before <string>'s
**	guard (CRT entries 656 / 639: 0x7BE5A0, 0x7B6B70), and radar_system_data_cache reads RadarRange's value.
**	Include this header first, before anything that includes aux_roots.h (the macros must come first).
**
**	// NAME 1 [aux_decoration.h]: provisional file name (the client's headers are not named by any string)
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __AUX_DECORATION_H
#define __AUX_DECORATION_H

#include "..\shared\auxdata\delta_numbering.h"
#define AUX_CTORS_DECORATION
#include "aux_roots_decoration.h"	// the class first: its members instantiate T<bool> before T<float> (delta_interpolator.h)
#include "aux_roots.h"

#endif //__AUX_DECORATION_H
