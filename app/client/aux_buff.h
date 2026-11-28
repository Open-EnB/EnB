// Earth & Beyond client (reconstruction)
/*
**	The buff constructors: Element (below: inline, plain in the roots' object), Buff, the Elements and Buffs arrays
**	(delta_item_array.h's DeltaDerivedItemArrayClass<T>; ui_buff: 0x561C10-0x561D30).
**	The aux-data constructors are gated by family in aux_roots.h / aux_roots_send.h: an object compiles
**	(and numbers at start-up, the Delta templates' per-class globals: workflow.md 3.4) exactly the
**	families whose constructors it sees, as the client's objects do (their initialisers from 0xBE3B44).
**	Include this header first, before anything that includes aux_roots.h (the macros must come first).
**
**	// NAME 1 [aux_buff.h]: provisional file name (the client's headers are not named by any string)
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __AUX_BUFF_H
#define __AUX_BUFF_H

#include "..\shared\auxdata\delta_numbering.h"
#define AUX_CTORS_BUFF
#include "aux_roots.h"

// Ship.BuffArray.N.Elements.N (0x707430)
#if !defined(AUX_ROOTS_ALL)
inline
#endif
DeltaElementClass::DeltaElementClass(const char *name) :
	DeltaContainer(name),
	SourceEntity("SourceEntity", ""),
	SourceObject("SourceObject", ""),
	Magnitude("Magnitude"),
	IsActive("IsActive"),
	ExpirationTime("ExpirationTime", 0)
{
	Add(&SourceEntity);
	Add(&SourceObject);
	Add(&Magnitude);
	Add(&IsActive);
	Add(&ExpirationTime);
}

#endif //__AUX_BUFF_H
