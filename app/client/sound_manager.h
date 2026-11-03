// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: unknown (PROVISIONAL: app\Client\sound_manager.h beside sound_manager.cpp, D137)
**
**	SoundManagerClass: sound_manager.cpp's per-channel sounds (0x340 bytes; WinMain 0x526DE0 makes it
**	into 0xBE82A4). NUM_CHANNELS from the asserts 'channel >= NUM_CHANNELS' (0x75AE30, 0x75B0D0) and the
**	constructor's 16-element arrays.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef SOUND_MANAGER_H  // NAME 3 [sound_manager.h]: beside the client's sound_manager.cpp
#define SOUND_MANAGER_H

#include <deque>

class AudibleSoundClass;

#define NUM_CHANNELS	16	// the asserts' name; the constructor's array counts

// A queued sound (4 bytes, new'd by 0x75AE30): its name, nstrdup'd
struct SoundManagerQueueEntryStruct  // NAME 1: neutral (D10)
{
	SoundManagerQueueEntryStruct(const char *name);
	~SoundManagerQueueEntryStruct() { if (Name) { delete [] Name; Name = NULL; } }	// inline in 0x75AC50 / 0x75AB70

	char *Name;																		// +0x00  // NAME 2: what 0x75AE30 stores (nstrdup of its name)
};

class SoundManagerClass	// NAME 3: class-from-file (D24) for app\client\sound_manager.cpp
{
public:
	SoundManagerClass();															// 0x75AB10
	~SoundManagerClass();															// 0x75AC50
	void Unknown_75AE30(const char *name, int channel);								// 0x75AE30 (queued on the channel)  // NAME 1 [name, channel]: the assert's channel
	void Unknown_75B0D0(const char *name, int channel);								// 0x75B0D0 (the channel's sound stopped, name played)  // NAME 1 [name, channel]: the assert's channel
	void Unknown_75B170(int channel);												// 0x75B170  // NAME 1 [channel]: neutral
	void Unknown_75B2B0(int channel);												// 0x75B2B0  // NAME 1 [channel]: neutral
	void Unknown_75B320();															// 0x75B320

	std::deque<SoundManagerQueueEntryStruct *> Queues[NUM_CHANNELS];				// +0x000 (0x30 bytes each; 0x75AB50 / 0x75AB70)  // NAME 2: what they hold
	AudibleSoundClass *Sounds[NUM_CHANNELS];										// +0x300 (Play_2D_Sound's, 0x75B0D0)  // NAME 2: what they hold
};

#endif //SOUND_MANAGER_H
