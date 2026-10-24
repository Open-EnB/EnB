/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\shared\kv_list.h (name assumed from kv_list.cpp; no
**	assert names it)
**
**	The list an encoder's pairs go to. Only what kv_encoder_base.cpp uses so far. The client
**	doesn't name the class (D24).
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __KV_LIST_H
#define __KV_LIST_H

class KVPairClass;

class KVListClass
{
public:
	void Add(KVPairClass *item);			// 0x4B6760 (the assert's name, line 23)
};

#endif //__KV_LIST_H
