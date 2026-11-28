// Earth & Beyond client (reconstruction)
/*
**	The manufacturing lab's constructors with the item family's (ui_manu, ui_refinery, ui_analyze: 0x5B6FD0, 0x5EEC10, 0x546540).
**	The aux-data constructors are gated by family in aux_roots.h / aux_roots_send.h: an object compiles
**	(and numbers at start-up, the Delta templates' per-class globals: workflow.md 3.4) exactly the
**	families whose constructors it sees, as the client's objects do (their initialisers from 0xBE3B44).
**	Include this header first, before anything that includes aux_roots.h (the macros must come first).
**
**	// NAME 1 [aux_manu.h]: provisional file name (the client's headers are not named by any string)
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __AUX_MANU_H
#define __AUX_MANU_H

#include "..\shared\auxdata\delta_numbering.h"
#define AUX_CTORS_MANU
#define AUX_CTORS_ITEM		// (aux_item.h's: before the class headers it gates)
#include "aux_roots_manu.h"	// the item classes, then the manufacturing ones (ui_manu: 0xC41630 before T<bool>)
#include "aux_item.h"

// ManufacturingLab.PrimaryCategories.N (0x6FB1E0)
inline
DeltaPrimaryCategoryClass::DeltaPrimaryCategoryClass(const char *name) :
	DeltaContainer(name),
	Name("Name", ""),
	Categories("Categories", 5)
{
	Add(&Name);
	Add(&Categories);
}

#endif //__AUX_MANU_H
