// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location unknown: a one-function object, client 0x4E0700-0x4E082F (it ends with <string>'s
**	guarded ctype initialiser 0x4E0810; the object before ends with 0x4E06D0's), with no string of its
**	own; the provisional name stays (D137).
*/



#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __UNKNOWN_4E0700_H  // NAME 1: provisional file name (D137)
#define __UNKNOWN_4E0700_H

#include "attrib_dir.h"

int Detach_Owner_Mods(AttribDir *dir, AttribModIterator mods, void *owner, int keep);	// 0x4E0700: detaches owner's mods (and unless keep deletes them), returns the count  // NAME 2: what it does

#endif //__UNKNOWN_4E0700_H
