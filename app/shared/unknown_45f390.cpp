#include <string.h>
#include "unknown_45f390.h"
#include "..\audio\soundlib.h"
#include "..\..\wwaudio\Sound3D.h"

Unknown45F390Class::Unknown45F390Class(RenderStateListClass *list, int id, int link_id, unsigned int flags, const char *sound, int message) :
	Unknown45D840Class(list, id, link_id, flags, message),
	SoundName(NULL),
	Sound(NULL),
	Unknown38(false)
{
	Flags |= 0x8000;
	if (sound != NULL) {
		SoundName = new char[strlen(sound) + 1];
		strcpy(SoundName, sound);
	}
}

Unknown45F390Class::Unknown45F390Class(RenderStateListClass *list, const Unknown45F390Class &source) :
	Unknown45D840Class(list, source),
	SoundName(NULL),
	Sound(NULL),
	Unknown38(false)
{
	const char *sound = source.SoundName;
	if (sound != NULL) {
		SoundName = new char[strlen(sound) + 1];
		strcpy(SoundName, sound);
	}
}

RenderStateClass *Unknown45F390Class::Unknown_04(RenderStateListClass *list)
{
	return(new Unknown45F390Class(list, *this));
}

Unknown45F390Class::~Unknown45F390Class()
{
	Unknown_45FA30();
	Unknown_0C();
	if (SoundName != NULL) {
		delete [] SoundName;
		SoundName = NULL;
	}
}

void Unknown45F390Class::Unknown_14(RsTrackClass *owner)
{
	Unknown_457E10();
	Unknown_45FB40();
	owner->Unknown_1D4((int)this, Message, (int)Sound);
}

void Unknown45F390Class::Unknown_18(RsTrackClass *owner)
{
	if (!Test_Flag(3)) {
		Unknown_45FA30();
		RenderStateClass::Unknown_18(owner);
	}
}

void Unknown45F390Class::Unknown_45F890(const char *sound)
{
	Unknown_45FA30();
	if (SoundName != NULL) {
		delete [] SoundName;
		SoundName = NULL;
	}
	if (sound != NULL) {
		SoundName = new char[strlen(sound) + 1];
		strcpy(SoundName, sound);
		Unknown_45FB40();
	}
}

void Unknown45F390Class::Unknown_45F960(const char *sound)
{
	Unknown_45FA30();
	if (SoundName != NULL) {
		delete [] SoundName;
		SoundName = NULL;
	}
	if (sound != NULL) {
		SoundName = new char[strlen(sound) + 1];
		strcpy(SoundName, sound);
	}
}

void Unknown45F390Class::Unknown_45FA30()
{
	if (Sound != NULL) {
		if (!Test_Flag(17)) {
			Sound->Stop(true);
			Sound->Remove_From_Scene();
		}
		Sound->Release_Ref();
		Sound = NULL;
	}
}

void Unknown45F390Class::Unknown_0C()
{
	Unknown_45FA30();
}

bool Unknown45F390Class::Unknown_10()
{
	Unknown_45FB40();
	return(Sound != NULL);
}

void Unknown45F390Class::Unknown_45FB40()
{
	if (Sound == NULL) {
		if (Test_Flag(15)) {
			return;
		}
		Matrix3D tm(true);
		Sound = SoundLibraryClass::Play_3D_Sound(SoundName, tm, 0, 1.0f);
		if (Sound != NULL) {
			if (Test_Flag(18)) {
				Sound->Set_Loop_Count(1);
			} else {
				Sound->Set_Loop_Count(Test_Flag(3));
			}
		}
	} else {
		Sound->Seek(0);
	}
	Unknown38 = false;
}

void Unknown45F390Class::Unknown_30(RsTrackClass *owner, float seconds)
{
	if (Sound != NULL) {
		if (!Unknown38) {
			Sound->Set_Transform(owner->Get_Transform());
			Sound->Add_To_Scene(true);
			Unknown38 = true;
		}
		if (!Sound->Is_Playing()) {
			if (Test_Flag(3)) {
				Flags |= 5;
				Unknown_4584B0(owner, 0, Link);
				Unknown_45FA30();
			} else {
				Unknown_457E90();
			}
		} else if (!Test_Flag(14)) {
			Sound->Set_Transform(owner->Get_Transform());
		}
	}
}


/*
** unknown_45f390.cpp: reconstruction notes
**
** The client's window 0x45F390-0x45FD9F, between rs_robj.cpp (ends 0x45F343) and rs_track.cpp (starts
** 0x45FDA0, RsTrackClass's constructor): the 'sound' render state, vtable 0xAE821C (slots +0x00 0x45F4A0,
** +0x04 0x45F5B0, +0x0C 0x45FAA0, +0x10 0x45FB10, +0x14 0x45F7D0, +0x18 0x45F810, +0x30 0x45FC50; +0x08 and
** the rest are RenderStateClass's). It derives from the message state (unknown_45d840.cpp: its constructors
** 0x45D840 / 0x45D8C0, its destructor 0x45D990) and hands RsTrackClass::Unknown_1D4 itself, the message and
** the sound when activated. No assert, no __FILE__ string: the directory is known (app\shared, between two
** of its files), the name is neutral (D10, D137); the alphabetical link order of app\shared puts it between
** rs_robj.cpp and rs_track.cpp (rs_sound.cpp would fit, unconfirmed). 0x45FA30 has no caller: inlined
** everywhere, emitted out of line.
*/
