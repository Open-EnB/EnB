// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\client\emote_data.h (name assumed from emote_data.cpp)
**
**	A client object's emote data (vtable 0xB075CC), bound to the object through co_data.cpp;
**	declared as far as co_data.cpp needs it (its constructor and size).
*/



#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __EMOTE_DATA_H  // NAME 3 [emote_data.h]: header name assumed from the client's emote_data.cpp
#define __EMOTE_DATA_H

#include "co_data.h"

class EmoteDataClass : public ClientObjectDataClass  // NAME 5: the client's typeid name .PAVEmoteDataClass@@ (co_data.cpp's registry)
{
public:
	EmoteDataClass(ClientObjectClass *co);			// 0x7E62F0 (emote_data.cpp)
	bool Unknown_7E7A40();							// 0x7E7A40 (co.cpp's per-frame update tests it for the player's object)
	void Unknown_7E76E0();							// 0x7E76E0 (after the camera call 0x79B2B0(0x14))
	void Unknown_7E6860();							// 0x7E6860 (every frame, for every object with emote data)
	bool Unknown_7E7A70();							// 0x7E7A70 (co.cpp's engine sounds 0x6FDBD0: true turns the thrust on)

	char Unknown08[0x48 - 0x08];		// +0x08: not rebuilt (0x48 bytes: co_data.cpp's new)
};

#endif //__EMOTE_DATA_H
