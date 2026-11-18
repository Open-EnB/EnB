// Earth & Beyond client (reconstruction)
/*
**	The group's constructors first, then the ship's with its families' (ship inventory, item, lego, buff):
**	radar_system numbers the group's types first (its initialisers 0x7BDE30 / 0x7BDEA0: T<int>, T<FormationState>,
**	T<bool>, then the ship's; I<GroupMember> first), as the group classes parse before aux_roots.h's here.
**	The aux-data constructors are gated by family in aux_roots.h / aux_roots_send.h: an object compiles
**	(and numbers at start-up, the Delta templates' per-class globals: workflow.md 3.4) exactly the
**	families whose constructors it sees, as the client's objects do (their initialisers from 0xBE3B44).
**	Include this header first, before anything that includes aux_roots.h (the macros must come first).
**
**	// NAME 1 [aux_group_ship.h]: provisional file name (the client's headers are not named by any string)
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __AUX_GROUP_SHIP_H
#define __AUX_GROUP_SHIP_H

#include "..\shared\auxdata\delta_numbering.h"
#define AUX_CTORS_GROUP
#define AUX_CTORS_SHIP
#define AUX_CTORS_SHIPINV
#define AUX_CTORS_LEGO
#define AUX_CTORS_BUFF
#include "aux_roots_group.h"
#include "aux_item.h"
#include "aux_buff.h"

#endif //__AUX_GROUP_SHIP_H
