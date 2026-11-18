// Earth & Beyond client (reconstruction)
/*
**	The ship's and the player's constructors with their families' and the Hull root's (sclient: 0x758CD0, 0x7598E0,
**	0x75A250; no light, decoration, harvestable, manufacturing lab or component root, which co.cpp's factory builds).
**	The aux-data constructors are gated by family in aux_roots.h / aux_roots_send.h: an object compiles
**	(and numbers at start-up, the Delta templates' per-class globals: workflow.md 3.4) exactly the
**	families whose constructors it sees, as the client's objects do (their initialisers from 0xBE3B44).
**	Include this header first, before anything that includes aux_roots.h (the macros must come first).
**
**	// NAME 1 [aux_ship_player.h]: provisional file name (the client's headers are not named by any string)
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __AUX_SHIP_PLAYER_H
#define __AUX_SHIP_PLAYER_H

#include "..\shared\auxdata\delta_numbering.h"
#define AUX_CTORS_SHIP
#define AUX_CTORS_SHIPINV
#define AUX_CTORS_LEGO
#define AUX_CTORS_BUFF
#define AUX_CTORS_ITEM
#define AUX_CTORS_SKILL
#define AUX_CTORS_MISSION
#define AUX_CTORS_FACTION
#define AUX_CTORS_GROUP
#define AUX_CTORS_PLAYER
#define AUX_CTORS_HULL
#include "aux_item.h"
#include "aux_buff.h"
#include "aux_roots_send.h"

#endif //__AUX_SHIP_PLAYER_H
