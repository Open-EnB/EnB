/*
**	Earth & Beyond client reconstruction.
**
**	Original location: e:\build\beta\G\app\shared\auxdata\auxdata.h (the client's assert string
**	'..\shared/auxdata/auxdata.h', G_ASSERT 't' on line 155: D25, D41).
**
**	The aux-data lookup the client inlines in its callers (84 of them, co.cpp's 0x6F58B0, 0x6F5E40,
**	0x6F5ED0, ...): the root's Lookup finds the named field, the field's per-class global at +0x04
**	tests its type (the bool field's 0xC416AC) and the assert catches a mismatch. The client doesn't
**	name the helper (D24).
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __AUXDATA_H
#define __AUXDATA_H

#include "delta.h"
#include "delta_template.h"
#include "lookup.h"
#include "name.h"
#include "..\unknown_debug.h"
#include "rtti.h"























































































































// The field named name under root, as a C (a delta class: DeltaTemplateClass<T>, DeltaStringClass);
// NULL when there is no root, no such field, or (asserting) a field of another type (rtti.h's test).
template<class C> C *Unknown_Find_Field(DeltaCompoundClass *root, const NameClass &name)  // NAME 1: neutral (D10)
{
	if (!root) return(NULL);
	LookupClass *lookup = root->Get_Lookup();
	DeltaClass *d = lookup->Find(name);
	if (!d) return(NULL);
	// early returns: the client's out-of-line copies 0x514B60 / 0x7079B0 (IB1)
	C *t = UnknownRTTIClass<C>::Cast(d);
	G_ASSERT(t);
	return(t);
}

#endif //__AUXDATA_H
