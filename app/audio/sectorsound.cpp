// Earth & Beyond client (reconstruction): app\Audio\sectorsound.cpp. Notes at the end of the file.
#include <stdio.h>
#include "sectorsound.h"
#include "..\shared\g_assert.h"
#include "..\..\wwlib\ini.h"
#include "..\..\wwlib\inisup.h"

// @0x43C4A0
SectorSoundClass::SectorSoundClass(unsigned sector) :
	AmbientSoundManager(0),
	CurrentElevation(-1),
	Sector(sector)
{
	FadeOut = 500;
	DefaultMinDelay = 10.0f;
	DefaultMaxDelay = 20.0f;
	DefaultMinPan = 0.5f;
	DefaultMaxPan = 0.5f;
	Load_INI();
}

// @0x43C560
SectorSoundClass::~SectorSoundClass()
{
	AmbientSoundManager::Reset();
	Elevations.clear();
}

// @0x43C630
void SectorSoundClass::Reset()
{
	AmbientSoundManager::Reset();
	Load_INI();
}

// @0x43C660
void SectorSoundClass::Update(int time, float elevation)
{
	if (Elevations.size() != 0) {
		if (CurrentElevation == -1 || elevation > Elevations[CurrentElevation].MaxElevation || elevation < Elevations[CurrentElevation].MinElevation) {
			for (unsigned index = 0; index < Elevations.size(); index++) {
				if (elevation >= Elevations[index].MinElevation && elevation <= Elevations[index].MaxElevation) {
					CurrentElevation = index;
					Play_Ambient_Sounds(Elevations[index].Channel, FadeOut, time);
					break;
				}
			}
		}
		if (CurrentElevation != -1) {
			AmbientSoundManager::Update(time);
		}
	}
}

// @0x43C780
// NOT EXACT: stack slots only (explain.py: same length, 0.68): the client keeps ini at esp+0x18 and its copy of 'this' at esp+0x28, here 'this' is at esp+0x18 and ini at esp+0x1C
void SectorSoundClass::Load_INI()
{
	CurrentElevation = -1;
	Elevations.clear();
	INIClass ini;
	char filename[128];
	sprintf(filename, "sectorsounds%d.ini", Sector);
	ini.Load(filename);
	if (!ini.Is_Loaded()) {
		return;
	}
	if (ini.Section_Present("General")) {
		FadeOut = (int)(ini.Get_Float("General", "FadeOut", 0.5f) * 1000.0f);
		DefaultMinDelay = ini.Get_Float("General", "DefaultMinDelay", 10.0f);
		DefaultMaxDelay = ini.Get_Float("General", "DefaultMaxDelay", 20.0f);
		DefaultMinPan = ini.Get_Float("General", "DefaultMinPan", DefaultMinPan);
		DefaultMaxPan = ini.Get_Float("General", "DefaultMaxPan", DefaultMaxPan);
	}
	List<INISection *> &section_list = ini.Get_Section_List();
	int num_channels = 0;
	INISection *section;
	for (section = section_list.First_Valid(); section != NULL; section = section->Next_Valid()) {
		if (ini.Get_Float(section->Section, "MaxElevation", -9999.999f) != -9999.999f) {
			num_channels++;
		}
	}
	if (num_channels != 0) {
		Initialize(num_channels);
		int channel = 0;
		for (section = section_list.First_Valid(); section != NULL; section = section->Next_Valid()) {
			const char *section_name = section->Section;
			float max_elevation = ini.Get_Float(section_name, "MaxElevation", -9999.999f);
			if (max_elevation != -9999.999f) {
				Load_Section(&ini, section_name, channel);
				ElevationStruct elevation;
				elevation.MaxElevation = max_elevation;
				elevation.MinElevation = -9999.0f;
				elevation.Channel = channel;
				Elevations.push_back(elevation);
				channel++;
			}
		}
		for (unsigned index = 0; index < Elevations.size(); index++) {
			float min_elevation = -9999.999f;
			for (unsigned other = 0; other < Elevations.size(); other++) {
				if (index != other && Elevations[other].MaxElevation < Elevations[index].MaxElevation && min_elevation < Elevations[other].MaxElevation) {
					min_elevation = Elevations[other].MaxElevation;
				}
			}
			Elevations[index].MinElevation = min_elevation;
		}
	}
}



















































































































































// @0x43CB70
void SectorSoundClass::Load_Section(INIClass *ini, const char *section_name, int channel)
{
	FAIL_IF(ini == NULL) return;
	FAIL_IF(ini->Section_Present(section_name) == false) return;
	for (int index = 1; index <= 16; index++) {
		char entry[128];
		char name[128];
		sprintf(entry, "LoopingAmbient%d", index);
		if (ini->Is_Present(section_name, entry)) {
			ini->Get_String(section_name, entry, "ERROR", name, sizeof(name));
			Add_Looping_Sound(channel, name);
		}
		sprintf(entry, "NonLoopingAmbient%d", index);
		if (ini->Is_Present(section_name, entry)) {
			ini->Get_String(section_name, entry, "ERROR", name, sizeof(name));
			sprintf(entry, "MinDelay%d", index);
			int min_delay = (int)(ini->Get_Float(section_name, entry, DefaultMinDelay) * 1000.0f);
			sprintf(entry, "MaxDelay%d", index);
			int max_delay = (int)(ini->Get_Float(section_name, entry, DefaultMaxDelay) * 1000.0f);
			sprintf(entry, "MinPan%d", index);
			float min_pan = ini->Get_Float(section_name, entry, DefaultMinPan);
			sprintf(entry, "MaxPan%d", index);
			float max_pan = ini->Get_Float(section_name, entry, DefaultMaxPan);
			Add_Random_Sound(channel, name, min_delay, max_delay, min_pan, max_pan, false);
		}
	}
}

/*
Reconstruction notes:
sectorsound.cpp holds SectorSoundClass (derived from AmbientSoundManager): a sector's ambient sounds, read from
sectorsounds<sector>.ini, one channel per elevation band. The object is 0x43C4A0-0x43CEA0 (after musictheme.cpp's
0x43C460, before soundlib.cpp's templates); the vector's instances 0x43CE40-0x43D2A0 follow its functions.
*/
