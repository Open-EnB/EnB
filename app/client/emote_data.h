// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\client\emote_data.h (name assumed from emote_data.cpp)
**
**	A client object's emote data (vtable 0xB075CC, 0x48 bytes), bound to the object through co_data.cpp:
**	the queue of emotes NOTIFY_EMOTE 162 adds and the one playing (emote_data.cpp, 0x7E62F0-0x7E7E60).
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __EMOTE_DATA_H  // NAME 3 [emote_data.h]: header name assumed from the client's emote_data.cpp
#define __EMOTE_DATA_H

#include "co_data.h"
#include <deque>
#include "..\..\wwlib\ref_ptr.h"

class Matrix3D;
class Sound3DClass;

class EmoteDataClass : public ClientObjectDataClass  // NAME 5: the client's typeid name .PAVEmoteDataClass@@ (co_data.cpp's registry)
{
public:
	EmoteDataClass(ClientObjectClass *co);			// 0x7E62F0
	virtual ~EmoteDataClass();						// +0x04 0x7E63A0 (0x7E6370)

	void Unknown_7E6650(int emote);					// 0x7E6650 (NOTIFY_EMOTE 162: queues the packet's emote)  // NAME 1 [emote]: the packet's Emote
	void Unknown_7E6860();							// 0x7E6860 (every frame, for every object with emote data)
	Matrix3D Unknown_7E70A0(float t);				// 0x7E70A0 (the emote's transform at t, 0 to 1)  // NAME 1 [t]: neutral
	void Unknown_7E7620();							// 0x7E7620 (ends the emote; uncalled)
	void Unknown_7E76E0();							// 0x7E76E0 (after the camera call 0x79B2B0(0x14))
	void Unknown_7E7700();							// 0x7E7700 (camera_manager.cpp 0x799DA0: entering a state from state 10)
	void Unknown_7E7780();							// 0x7E7780 (puts a type 14 state over the object's positional state)
	void Unknown_7E79D0();							// 0x7E79D0 (takes the type 14 state off again)
	bool Unknown_7E7A40();							// 0x7E7A40 (co.cpp's per-frame update tests it for the player's object)
	bool Unknown_7E7A70();							// 0x7E7A70 (co.cpp's engine sounds 0x6FDBD0: true turns the thrust on)

	std::deque<int> Queue;							// +0x08 (the emotes still to play)  // NAME 2: what it holds
	unsigned int StartTime;							// +0x38 (the client's time when the emote began)  // NAME 2: what it holds
	int Emote;										// +0x3C (the playing emote, 7 none)  // NAME 2: what it holds
	bool Unknown40;									// +0x40 (true from the constructor and 0x7E76E0, false from 0x7E7700)
	RefCountPtr<Sound3DClass> Sound;				// +0x44 (the emote's 3D sound)  // NAME 2: what it holds
};

#endif //__EMOTE_DATA_H
