// Earth & Beyond client (reconstruction): app\Client\sound_manager.cpp (__FILE__ 0xB9AE38). Notes at the end of the file.
#include <string.h>
#include "sound_manager.h"
#include "..\audio\soundlib.h"
#include "..\..\wwaudio\AudibleSound.h"
#include "..\..\wwlib\nstrdup.h"
#include "..\shared\unknown_debug.h"

inline SoundManagerQueueEntryStruct::SoundManagerQueueEntryStruct(const char *name) :
	Name(nstrdup(name))
{
}

// 0x75AB10
SoundManagerClass::SoundManagerClass()
{
	for (int i = 0; i < NUM_CHANNELS; i++) {
		Sounds[i] = NULL;
	}
}

// 0x75AC50
SoundManagerClass::~SoundManagerClass()
{
	for (int i = 0; i < NUM_CHANNELS; i++) {
		if (Sounds[i]) {
			Sounds[i]->Stop(true);
			Sounds[i]->Release_Ref(); Sounds[i] = NULL;
		}
		while (!Queues[i].empty()) {
			delete Queues[i].front();
			Queues[i].pop_front();
		}
	}
}





















// 0x75AE30
void SoundManagerClass::Unknown_75AE30(const char *name, int channel)
{
	FAIL_IF(channel >= NUM_CHANNELS) return;
	Queues[channel].push_back(new SoundManagerQueueEntryStruct(name));
}




// 0x75B0D0
void SoundManagerClass::Unknown_75B0D0(const char *name, int channel)
{
	FAIL_IF(channel >= NUM_CHANNELS) return;
	if (Sounds[channel]) {
		Sounds[channel]->Stop(true);
		Sounds[channel]->Release_Ref(); Sounds[channel] = NULL;
	}
	Sounds[channel] = SoundLibraryClass::Play_2D_Sound(name, 1);
}

// 0x75B170
void SoundManagerClass::Unknown_75B170(int channel)
{
	while (!Queues[channel].empty()) {
		delete Queues[channel].front();
		Queues[channel].pop_front();
	}
}

// 0x75B2B0
void SoundManagerClass::Unknown_75B2B0(int channel)
{
	if (Sounds[channel]) {
		Sounds[channel]->Stop(true);
		Sounds[channel]->Release_Ref(); Sounds[channel] = NULL;
	}
}

// 0x75B320
void SoundManagerClass::Unknown_75B320()
{
	for (int i = 0; i < NUM_CHANNELS; i++) {
		if (Sounds[i] && !Sounds[i]->Is_Playing()) {
			Sounds[i]->Release_Ref(); Sounds[i] = NULL;
		}
		if (Sounds[i] == NULL && !Queues[i].empty()) {
			SoundManagerQueueEntryStruct *entry = Queues[i].front();
			Sounds[i] = SoundLibraryClass::Play_2D_Sound(entry->Name, 1);
			delete entry;
			Queues[i].pop_front();
		}
	}
}

/*
**	Reconstruction notes
**
**	client.exe: SoundManagerClass 0x75AB10-0x75B460 (its deque instances after it). The constructor runs
**	the eh vector constructor over Queues (0x75AB50 / 0x75AB70: std::deque's) and zeroes Sounds; the
**	destructor stops and releases each channel's sound and deletes its queued entries.
**	All seven byte-exact (appfile.py match): 0x75AB10, 0x75AC50, 0x75AE30, 0x75B0D0, 0x75B170 (the channel's
**	queue emptied), 0x75B2B0 (the channel's sound stopped), 0x75B320 (finished sounds released, the next
**	queued one played; AudibleSoundClass vtable +0xC4 is Is_Playing). File placement from the asserts'
**	__FILE__ (0xB9AE38); the header beside it is PROVISIONAL (D137). The other files' local declarations
**	(main_client.cpp, sclient.cpp, cpacket.cpp, ui_cpit.cpp: char Unknown00[0x340]) are left as they are.
*/
