// Earth & Beyond client (reconstruction)
/*
**	The player's constructors with its families' (skill, item, mission, faction, group: ui_faction, ui_missn 0x58E9F0, 0x5C6F10).
**	The aux-data constructors are gated by family in aux_roots.h / aux_roots_send.h: an object compiles
**	(and numbers at start-up, the Delta templates' per-class globals: workflow.md 3.4) exactly the
**	families whose constructors it sees, as the client's objects do (their initialisers from 0xBE3B44).
**	Include this header first, before anything that includes aux_roots.h (the macros must come first).
**
**	// NAME 1 [aux_player.h]: provisional file name (the client's headers are not named by any string)
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __AUX_PLAYER_H
#define __AUX_PLAYER_H

#include "..\shared\auxdata\delta_numbering.h"
#define AUX_CTORS_SKILL
#define AUX_CTORS_ITEM
#define AUX_CTORS_MISSION
#define AUX_CTORS_FACTION
#define AUX_CTORS_GROUP
#define AUX_CTORS_PLAYER
#include "aux_roots_send.h"

#endif //__AUX_PLAYER_H
