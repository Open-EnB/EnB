/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\shared\auxdata\delta_array.h (the client's __FILE__
**	string '..\shared/auxdata/delta_array.h', from the float array's constructor 0x703550, line 71)
**
**	A compound of count leaf fields DeltaTemplateClass<T>, named "0", "1", ... (sprintf "%d",
**	0xB71A6C), each made with the array's value (M1.3). The float array's vtable is 0xB00C20 (its
**	global 0xC415DC), the 20-byte one's 0xB00BC4. The client doesn't name the template (D24).
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __DELTA_ARRAY_H
#define __DELTA_ARRAY_H

#include <stdio.h>
#include "delta_template.h"
#include "..\unknown_debug.h"

template<class T> class DeltaArrayClass : public DeltaCompoundClass	// NAME 3: by convention (D24) from the client's file delta_array.h
{
public:
	DeltaArrayClass(const char *name, int count, const T &value = T());

	static int UnknownGlobal;	// the per-class global at +0x04 (float: 0xC415DC)
};

template<class T> int DeltaArrayClass<T>::UnknownGlobal;

// (the lines down to the constructor's assert hold code of the client's header not rebuilt yet:
// the assert is on line 71)























template<class T> DeltaArrayClass<T>::DeltaArrayClass(const char *name, int count, const T &value) :
	DeltaCompoundClass(name)
{
	m_Unknown04 = UnknownGlobal;
	char item_name[256];
	int i = 0;
	if (count > 0) {
		do {
			sprintf(item_name, "%d", i);
			DeltaTemplateClass<T> *item = new DeltaTemplateClass<T>(item_name, value);
			if (item == 0) {
				G_ASSERT(item);
				break;
			}
			Add(item);
		} while (++i < count);
	}
}

#endif //__DELTA_ARRAY_H
