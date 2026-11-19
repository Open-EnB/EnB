// Earth & Beyond client (reconstruction)
/*
**	The ship's constructors with its families' (ship inventory, item, lego, buff: ship_stats_display 0x6A3C30).
**	The aux-data constructors are gated by family in aux_roots.h / aux_roots_send.h: an object compiles
**	(and numbers at start-up, the Delta templates' per-class globals: workflow.md 3.4) exactly the
**	families whose constructors it sees, as the client's objects do (their initialisers from 0xBE3B44).
**	Include this header first, before anything that includes aux_roots.h (the macros must come first).
**
**	// NAME 1 [aux_ship.h]: provisional file name (the client's headers are not named by any string)
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __AUX_SHIP_H
#define __AUX_SHIP_H

#include "..\shared\auxdata\delta_numbering.h"
#define AUX_CTORS_SHIP
#define AUX_CTORS_SHIPINV
#define AUX_CTORS_LEGO
#define AUX_CTORS_BUFF
#define AUX_CTORS_ITEM		// (aux_item.h's: before the class headers it gates)
#include "aux_roots.h"		// aux_roots.h's class order (T<float> first: ship_stats_display 0x6A3C30)
#include "aux_item.h"
#include "aux_buff.h"

#endif //__AUX_SHIP_H
