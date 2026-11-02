// Earth & Beyond client (reconstruction): app\client\unknown_fontsound.cpp (PROVISIONAL name). Notes at the end of the file.
#include <windows.h>
#include <mmsystem.h>
#include "unknown_fontsound.h"
#include "..\..\wwaudio\WWAudio.h"
#include "..\..\wwaudio\AudibleSound.h"
#include <string.h>

// NOT EXACT: (D64) the client's has an EH frame with no state stores around the same seven stores
Unknown_6BCFB0Class::Unknown_6BCFB0Class(void)
{
}

// NOT EXACT: (D64) Clear's loop as `while (--index >= 0)` (dec / js, 0.80); the client has no state -1 store
// before the member's destructor
Unknown_6BCFB0Class::~Unknown_6BCFB0Class(void)
{
	int index = Sounds.Count();
	while (--index >= 0) {
		Remove(Sounds[index]);
	}
	Sounds.Reset_Active();
}

void Unknown_6BCFB0Class::Clear(void)
{
	for (int index = Sounds.Count() - 1; index >= 0; index--) {
		Remove(Sounds[index]);
	}
	Sounds.Reset_Active();
}

// NOT EXACT: (D64) the client stores Duration before the fcomp of seconds and places the Add branch after the
// epilogue; this form schedules the store after it (0.81)
void Unknown_6BCFB0Class::Play(StringClass name, float volume, float seconds)
{
	if (strlen(name) != 0) {
		PlayingSoundStruct * sound = new PlayingSoundStruct;
		if (sound != NULL) {
			sound->Sound = WWAudioClass::Get_Instance()->Create_Sound_Effect(name);
			if (sound->Sound != NULL) {
				sound->Sound->Set_Priority(0.5f);
				sound->Sound->Set_Loop_Count(1);
				sound->Sound->Set_Volume(volume);
				sound->Sound->Play();
				sound->Duration = seconds * 1000.0f;
				if (seconds < 0.0f) {
					sound->Sound->Release_Ref();
					sound->Sound = NULL;
				} else {
					Sounds.Add(sound);
					sound->Time = timeGetTime();
					return;
				}
			}
			delete sound;
		}
	}
}

void Unknown_6BCFB0Class::Update(void)
{
	for (int index = Sounds.Count() - 1; index >= 0; index--) {
		PlayingSoundStruct * sound = Sounds[index];
		if (sound != NULL) {
			unsigned long now = timeGetTime();
			int elapsed = now - sound->Time;
			sound->Time = now;
			if (sound->Duration < 60000) {
				sound->Duration -= elapsed;
			}
			if (sound->Duration < 0) {
				Remove(sound);
			}
		}
	}
}

void Unknown_6BCFB0Class::Remove(PlayingSoundStruct * sound)
{
	if (sound != NULL) {
		if (sound->Sound != NULL) {
			sound->Sound->Stop();
			sound->Sound->Release_Ref();
			sound->Sound = NULL;
		}
		Sounds.Delete(sound);
		delete sound;
	}
}

/*
**	Reconstruction notes (unknown_fontsound.cpp)
**
**	The client's object 0x6BCFB0-0x6BDA3F between font32bit.cpp (asserts up to 0x6BCDE0, its inline members to
**	0x6BCFA0) and fontdatabase.cpp (0x6BDA40); no __FILE__, so the file name is provisional (FNT1). The
**	members, then DynamicVectorClass<PlayingSoundStruct *> / VectorClass<PlayingSoundStruct *>'s instances
**	0x6BD5F0-0x6BD9E0 (vtables 0xAFEA18 / 0xAFEA34) and the <string> guard 0x6BDA10.
*/
