// Earth & Beyond client (reconstruction)
/*
**	The harvestable's constructor with the skill and item families' (ui_mfd_extended_context_harvestable: 0x5C0FD0).
**	The aux-data constructors are gated by family in aux_roots.h / aux_roots_send.h: an object compiles
**	(and numbers at start-up, the Delta templates' per-class globals: workflow.md 3.4) exactly the
**	families whose constructors it sees, as the client's objects do (their initialisers from 0xBE3B44).
**	Include this header first, before anything that includes aux_roots.h (the macros must come first).
**
**	// NAME 1 [aux_harvestable.h]: provisional file name (the client's headers are not named by any string)
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __AUX_HARVESTABLE_H
#define __AUX_HARVESTABLE_H

#include "..\shared\auxdata\delta_numbering.h"
#define AUX_CTORS_HARVESTABLE
#define AUX_CTORS_SKILL
#define AUX_CTORS_ITEM
#include "aux_roots_skill.h"	// the skill classes, then the item's (ui_mfd_extended_context_harvestable)
#include "aux_roots_item.h"
#include "aux_roots_send.h"

#endif //__AUX_HARVESTABLE_H
