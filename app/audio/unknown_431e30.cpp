// Earth & Beyond client (reconstruction): an unnamed audio object at 0x431E30 (PROVISIONAL). Notes at the end of the file.
#include <stdio.h>
#include "unknown_431e30.h"
#include "..\..\wwlib\ini.h"
#include "..\..\wwlib\argv.h"
#include "..\..\wwaudio\WWAudio.h"
#include "..\..\wwaudio\AudibleSound.h"
#include "..\client\option.h"

Unknown431E30Class *Unknown431E30Class::_Instance = NULL;

// @0x431E30
Unknown431E30Class::Unknown431E30Class() :
	Sound(NULL),
	Track(0),
	State(1),
	PlayingState(1),
	Paused(false),
	Sector(0)
{
	Load_INI();
	Is_Active();
}

// @0x432080
Unknown431E30Class::~Unknown431E30Class()
{
	Stop();
	if (_Instance == this) {
		_Instance = NULL;
	}
	Tracks.clear();
	SectorMusic.clear();
}

// @0x4322B0
void Unknown431E30Class::Load_INI()
{
	char entry[32];
	char section[32];
	char name[128];
	INIClass ini;
	ini.Load("ambient.ini");
	Tracks.clear();
	for (int index = 0; ; index++) {
		_snprintf(entry, sizeof(entry), "SOUND_%d", index);
		if (!ini.Is_Present("General", entry)) {
			break;
		}
		ini.Get_String("General", entry, "Destination.MP3", name, sizeof(name));
		Tracks.push_back(std::string(name));
	}
	SectorMusic.clear();
	int music = 0;
	while (true) {
		_snprintf(section, sizeof(section), "Music-%03d", music);
		if (!ini.Section_Present(section)) {
			break;
		}
		int sector = ini.Get_Int(section, "Sector", 0);
		int track = ini.Get_Int(section, "DefaultMusic", 1);
		SectorMusic[sector] = track; SectorMusic.size();	// a dead read: it sends operator[] (0x433410) out of line, as the client has it (workflow.md 3.1.2, inline budget)
		music++;
	}
	float volume = OptionClass::Instance->Get_Float("Sound_Music") * 0.01F;
	WWAudioClass::Get_Instance()->Set_Music_Volume(volume);
}

// @0x4325F0
void Unknown431E30Class::Set_Paused(bool paused)
{
	Paused = paused;
}

// @0x432610
void Unknown431E30Class::Play(int track, bool loop)
{
	ArgvClass args(false, false);
	if (args.Find("-NOMUSIC") == NULL && Is_Active()) {
		Stop();
		if (track < Tracks.size()) {
			float volume = OptionClass::Instance->Get_Float("Sound_Music") * 0.01F;
			WWAudioClass::Get_Instance()->Set_Music_Volume(volume);
			Track = track;
			Sound = WWAudioClass::Get_Instance()->Create_Sound_Effect(Tracks[track].c_str());
			if (Sound != NULL) {
				Sound->Set_Type(AudibleSoundClass::TYPE_MUSIC);
				if (loop) {
					Sound->Set_Loop_Count(0);
				}
				Sound->Play(true);
			}
		}
	}
}

// @0x4327D0
void Unknown431E30Class::Stop()
{
	if (Sound != NULL) {
		if (Is_Active()) {
			Sound->Stop(true);
		}
		Sound->Release_Ref();
		Sound = NULL;
		PlayingState = 0;
	}
}

// @0x432840
void Unknown431E30Class::Resume()
{
	if (Sound != NULL && !Sound->Is_Playing()) {
		Sound->Play(true);
	}
}

// @0x432880
int Unknown431E30Class::Is_Playing()
{
	if (Sound != NULL && Sound->Is_Playing()) {
		return 1;
	}
	return 0;
}

// @0x4328B0
// The lookup's const_iterator: the client converts end() through _Hashtable_const_iterator(const iterator &) 0x432C00, called out of line here; the three dead size() reads after the Get_Sector_Music call are budget devices (no bytes, D272) that keep that conversion out of line as the client
void Unknown431E30Class::Update(bool flag)
{
	float volume = WWAudioClass::Get_Instance()->Get_Music_Volume();
	if (Paused) {
		if (volume > 0.05) {
			WWAudioClass::Get_Instance()->Set_Music_Volume(volume - 0.04);
			return;
		}
		if (Sound != NULL) {
			Stop();
			PlayingState = MUSIC_PAUSED;
		}
		return;
	}
	if (State != PlayingState) {
		if (volume > 0.05) {
			WWAudioClass::Get_Instance()->Set_Music_Volume(volume - 0.02);
			return;
		}
		Stop();
		if (State != 0) {
			int track = State;
			if (State == MUSIC_SECTOR) {
				track = Get_Sector_Music(flag); if (0) { SectorMusic.size(); } if (0) { SectorMusic.size(); } if (0) { SectorMusic.size(); }
			}
			Play(track, true);
		}
		PlayingState = State;
		return;
	}
	volume = WWAudioClass::Get_Instance()->Get_Sound_Effects_Volume();
	float target = OptionClass::Instance->Get_Float("Sound_Music") * 0.01F;
	if (volume != target) {
		WWAudioClass::Get_Instance()->Set_Music_Volume(target);
	}
}

// @0x432B00
int Unknown431E30Class::Get_Sector_Music(bool flag)
{
	std::hash_map<int, int>::const_iterator it = SectorMusic.find(Sector);
	if (it == SectorMusic.end()) {
		return flag ? 2 : 1;
	}
	return (*it).second;
}

/*
Reconstruction notes:
The object 0x431E30-0x433CE0 (after w32window.cpp, before ambientsoundmanager.cpp; its SGI hash_map<int, int> and
vector<string> instances follow its functions, to the _Construct 0x433C80 and the <string> initialiser 0x433CB0) holds a music player no code constructs: ambient.ini's tracks, a sector's
DefaultMusic, fades through WWAudio's music volume. It names no file (no assert); the class and file names are
provisional. Update compares WWAudio's +0x14 with the option (read here as Get_Sound_Effects_Volume: the field
after m_MusicVolume).
*/
