// Earth & Beyond client (reconstruction)
/*
**	The mission classes ahead of the player's tree (ui_line_display: 0x5AA540, 0x5AADA0).
**	The aux-data constructors are gated by family in aux_roots.h / aux_roots_send.h: an object compiles
**	(and numbers at start-up, the Delta templates' per-class globals: workflow.md 3.4) exactly the
**	families whose constructors it sees, as the client's objects do (their initialisers from 0xBE3B44).
**	Include this header first, before anything that includes aux_roots.h (the macros must come first).
**	ui_line_display's initialisers number T<bool> before T<int> and I<MissionStage> before I<Skill>: its
**	object parsed MissionStage and Mission before the skill classes (aux_player.h includes them after them).
**
**	// NAME 1 [aux_mission.h]: provisional file name (the client's headers are not named by any string)
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __AUX_MISSION_H
#define __AUX_MISSION_H

#include "..\shared\auxdata\delta_numbering.h"
#define AUX_CTORS_MISSION
#include "aux_roots_mission.h"	// MissionStage, Mission: T<bool>, then T<int> (ui_line_display 0x5AA540)

#endif //__AUX_MISSION_H
