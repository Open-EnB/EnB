// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\shared\auxdata\delta_item_array.h (the client's __FILE__
**	string '..\shared/auxdata/delta_item_array.h', from 0x701D70, line 60)
**
**	A compound of count compound fields T(name), named "0", "1", ... (M1.3): the item arrays
**	(Cargo, Equipped: vtable 0xB00F54, global 0xC41624, elements 0xB0111C). The client doesn't name
**	the template (D24). The loop: a do-while behind a count test (`jae exit; jmp top`, 0x6F9ED0).
*/



#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __DELTA_ITEM_ARRAY_H
#define __DELTA_ITEM_ARRAY_H

#include <stdio.h>
#include "delta.h"
#include "..\g_assert.h"

template<class T> class DeltaItemArrayClass : public DeltaCompoundClass	// NAME 3: by convention (D24) from the client's file delta_item_array.h
{
public:
	DeltaItemArrayClass(const char *name, unsigned int count);
	~DeltaItemArrayClass();

	static int UnknownGlobal;	// the per-class global at +0x04 (0xB00F54's: 0xC41624)
};

// UnknownGlobal: defined in delta_numbering.h

// (the lines down to the constructor's assert hold code of the client's header not rebuilt yet:
// the assert is on line 60)

template<class T> DeltaItemArrayClass<T>::~DeltaItemArrayClass()
{
	for (unsigned int i = 0; i < Children.size(); i++) {
		delete Children[i];
	}
}




template<class T> DeltaItemArrayClass<T>::DeltaItemArrayClass(const char *name, unsigned int count) :
	DeltaCompoundClass(name)
{
	Unknown04 = UnknownGlobal;
	char item_name[256];
	unsigned int i = 0;
	if (count > 0) {
		do {
			sprintf(item_name, "%d", i);
			T *item = new T(item_name);
			if (item == 0) {
				G_ASSERT(item);
				break;
			}
			Add(item);
		} while (++i < count);
	}
}

// An item array with a vtable and per-class global of its own (the Buff's Elements 0xB01EBC, Ship's BuffArray
// 0xB00B0C): the client builds the base, then stores the two (0x707292, 0x6F73DA). A template: the client
// numbers its globals (0xC416A4, 0xC41600) as template statics, at an object's end (ui_buff 0x561CB0, co
// 0x707A20), and the bases' (0xC41694, 0xC41570) a round later (0x561D00, 0x709B80). Placement interim (D137).
template<class T> class DeltaDerivedItemArrayClass : public DeltaItemArrayClass<T>	// NAME 1: neutral (no client name)
{
public:
	DeltaDerivedItemArrayClass(const char *name, unsigned int count);
	~DeltaDerivedItemArrayClass() {}	// user-declared: Ship's destructor stores 0xB00B0C first (0x6F8EA9)

	static int UnknownGlobal;	// the per-class global at +0x04 (delta_numbering.h)
};

template<class T> DeltaDerivedItemArrayClass<T>::DeltaDerivedItemArrayClass(const char *name, unsigned int count) :
	DeltaItemArrayClass<T>(name, count)
{
	Unknown04 = UnknownGlobal;
}

#endif //__DELTA_ITEM_ARRAY_H
