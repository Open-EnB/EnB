/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\shared\auxdata\delta_item_array.h (the client's __FILE__
**	string '..\shared/auxdata/delta_item_array.h', from 0x701D70, line 60)
**
**	A compound of count compound fields T(name), named "0", "1", ... (M1.3): the item arrays
**	(Cargo, Equipped: vtable 0xB00F54, global 0xC41624, elements 0xB0111C). The client doesn't name
**	the template (D24).
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __DELTA_ITEM_ARRAY_H
#define __DELTA_ITEM_ARRAY_H

#include <stdio.h>
#include "delta.h"
#include "..\unknown_debug.h"

template<class T> class DeltaItemArrayClass : public DeltaCompoundClass	// NAME 3: by convention (D24) from the client's file delta_item_array.h
{
public:
	DeltaItemArrayClass(const char *name, unsigned int count);

	static int UnknownGlobal;	// the per-class global at +0x04 (0xB00F54's: 0xC41624)
};

template<class T> int DeltaItemArrayClass<T>::UnknownGlobal;

// (the lines down to the constructor's assert hold code of the client's header not rebuilt yet:
// the assert is on line 60)












template<class T> DeltaItemArrayClass<T>::DeltaItemArrayClass(const char *name, unsigned int count) :
	DeltaCompoundClass(name)
{
	m_Unknown04 = UnknownGlobal;
	for (unsigned int i = 0; i < count; i++) {
		char item_name[256];
		sprintf(item_name, "%d", i);
		T *item = new T(item_name);
		if (item != 0) {
			Add(item);
		} else {
			G_ASSERT(item);
			break;
		}
	}
}

#endif //__DELTA_ITEM_ARRAY_H
