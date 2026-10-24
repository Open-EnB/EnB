/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\shared\auxdata\delta_template.h (the client's __FILE__
**	string, from the float field's Read 0x4A6DF0, line 110)
**
**	A leaf field holding one value of type T (the float field's vtable 0xAEAEB0 overrides slots 0,
**	1, 0x2C and 0x30 of DeltaClass's). Only what delta_money.cpp uses so far: the generic Read and
**	Write (asserts at lines 110 and later) and the rest are not rebuilt yet, so the asserts' lines
**	are not kept here yet. The client doesn't name the template (D24).
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __DELTA_TEMPLATE_H
#define __DELTA_TEMPLATE_H

#include "delta.h"

template<class T> class DeltaTemplateClass : public DeltaClass
{
public:
	virtual bool Read(LinearBufferClass &buf, int b);		// +0x2C (float: 0x4A6DF0)
	virtual bool Write(LinearBufferClass &buf);				// +0x30 (float: 0x4A6E60)

	T Value;			// +0x84, or +0x88 for an 8-aligned T (MoneyClass)
};

#endif //__DELTA_TEMPLATE_H
