// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\shared\auxdata\permstr.h (name assumed from permstr.cpp)
**
**	An interned string: one pointer into permstr.cpp's hash table (s_Table), so equal strings
**	compare by pointer. The class name is the client's ("PermString: %d slots, ..." in 0x4ADFF0).
**	The table is made and freed by a counted initialiser that every file including this header
**	holds (the client's objects each construct one: 0x4A60A0 at 0xBE3A90, 0x4AE3B0 at 0xBE3B40).
**	The classes are permstring.h's: kv_pair.h takes them without the static (see there).
*/



#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __PERMSTR_H  // NAME 3 [permstr.h]: header name assumed from the client's permstr.cpp
#define __PERMSTR_H

#include "permstring.h"	// PermString and PermStringInitClass

static PermStringInitClass PermStringInit;  // NAME 2: describes what it does

#endif //__PERMSTR_H
