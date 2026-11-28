// Earth & Beyond client (reconstruction)
/*
**	The item family's constructors: Item, Effect, EquipItem (item_info, inv_data_peek: 0x529580, 0x68B280).
**	The aux-data constructors are gated by family in aux_roots.h / aux_roots_send.h: an object compiles
**	(and numbers at start-up, the Delta templates' per-class globals: workflow.md 3.4) exactly the
**	families whose constructors it sees, as the client's objects do (their initialisers from 0xBE3B44).
**	Include this header first, before anything that includes aux_roots.h (the macros must come first).
**
**	// NAME 1 [aux_item.h]: provisional file name (the client's headers are not named by any string)
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __AUX_ITEM_H
#define __AUX_ITEM_H

#include "..\shared\auxdata\delta_numbering.h"
#define AUX_CTORS_ITEM
#include "aux_roots_item.h"	// the item classes first: T<int> before T<float> (inv_render 0x68FAA0)
#include "aux_roots.h"

// Ship.Inventory.Cargo.N (0x6FA150)
inline
DeltaItemClass::DeltaItemClass(const char *name) :
	DeltaContainer(name),
	ItemTemplateID("ItemTemplateID", -1),
	StackCount("StackCount"),
	Price("Price", MoneyClass(-1)),
	AveCost("AveCost"),
	Structure("Structure", -1.0f),
	Quality("Quality", -1.0f),
	InstanceInfo("InstanceInfo", ""),
	InstanceActivatedEffectInfo("InstanceActivatedEffectInfo", ""),
	InstanceEquipEffectInfo("InstanceEquipEffectInfo", ""),
	BuilderName("BuilderName", "")
{
	Add(&ItemTemplateID);
	Add(&StackCount);
	Add(&Price);
	Add(&AveCost);
	Add(&Structure);
	Add(&Quality);
	Add(&InstanceInfo);
	Add(&InstanceActivatedEffectInfo);
	Add(&InstanceEquipEffectInfo);
	Add(&BuilderName);
}

#endif //__AUX_ITEM_H
