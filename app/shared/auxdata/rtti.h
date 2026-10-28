/*
**	Earth & Beyond client reconstruction.
**
**	Original location: e:\build\beta\G\app\shared\auxdata\rtti.h (the client's assert string
**	'..\shared/auxdata/rtti.h', G_ASSERT 'b' on line 32: D25, D41).
**
**	The aux data's type test: a delta object as a T when its per-class global at +0x04 is T's,
**	else NULL. Inlined in most callers, called out of line where a caller's budget runs out:
**	0x5469C0 for DeltaStringClass (global 0xBE3A9C), 0x709B30 for the class with global 0xC4158C.
**	The client doesn't name it (D24).
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __RTTI_H
#define __RTTI_H

#include "delta.h"
#include "..\unknown_debug.h"








template<class T> class UnknownRTTIClass { public: static T *Cast(DeltaClass *b)  // NAME 1: neutral (D10)
{
	G_ASSERT(b);
	if (!b) return(NULL);
	if (b->m_Unknown04 != T::UnknownGlobal) return(NULL);
	return((T *)b);
} };

#endif //__RTTI_H
