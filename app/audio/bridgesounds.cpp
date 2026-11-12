// Earth & Beyond client (reconstruction): app\Audio\bridgesounds.cpp. Notes at the end of the file.
#include <stdio.h>
#include "bridgesounds.h"
#include "..\shared\g_assert.h"
#include "..\..\wwlib\ini.h"

// @0x436A60
// NOT EXACT: register-only (explain.py: same length, 0.74): the client holds the 0 of m_channel in ecx and loads 0.5 into eax before it; here eax holds both in turn
BridgeSoundManager::BridgeSoundManager() :
	AmbientSoundManager(3)
{
	m_channel = 0;
	m_fade_out = 500;
	m_min_delay = 10.0f;
	m_max_delay = 20.0f;
	m_min_pan = 0.5f;
	m_max_pan = 0.5f;
	Load_INI();
}

// @0x436AF0
BridgeSoundManager::~BridgeSoundManager()
{
	AmbientSoundManager::Reset();
}

// @0x436B60
void BridgeSoundManager::Reset(int time)
{
	AmbientSoundManager::Reset();
	AmbientSoundManager::Initialize(3);
	Load_INI();
	AmbientSoundManager::Play_Ambient_Sounds(m_channel, m_fade_out, time);
}

// @0x436BA0
void BridgeSoundManager::Play_Sounds(int channel, int time)
{
	AmbientSoundManager::Play_Ambient_Sounds(channel, m_fade_out, time);
}

// @0x436BD0
void BridgeSoundManager::Update(int time)
{
	AmbientSoundManager::Update(time);
}

// @0x436BF0
void BridgeSoundManager::Load_INI()
{
	INIClass ini;
	char filename[128];
	sprintf(filename, "bridgesounds.ini");
	ini.Load(filename);
	if (ini.Is_Loaded()) {
		if (ini.Section_Present("General")) {
			m_fade_out = (int)(ini.Get_Float("General", "FadeOut", 0.5f) * 1000.0f);
			m_min_delay = ini.Get_Float("General", "DefaultMinDelay", 10.0f);
			m_max_delay = ini.Get_Float("General", "DefaultMaxDelay", 20.0f);
			m_min_pan = ini.Get_Float("General", "DefaultMinPan", m_min_pan);
			m_max_pan = ini.Get_Float("General", "DefaultMaxPan", m_max_pan);
		}
		Load_Section(&ini, "Terran", 0);
		Load_Section(&ini, "Jenquai", 1);
		Load_Section(&ini, "Progen", 2);
	}
}













































































































































// @0x436DC0
void BridgeSoundManager::Load_Section(INIClass *ini, const char *section, int race)
{
	FAIL_IF(ini == NULL) return;
	if (ini->Section_Present(section)) {
		for (int index = 1; index <= 16; index++) {
			char entry[128];
			char name[128];
			sprintf(entry, "LoopingAmbient%d", index);
			if (ini->Is_Present(section, entry)) {
				ini->Get_String(section, entry, "ERROR", name, sizeof(name));
				Add_Looping_Sound(race, name);
			}
			sprintf(entry, "NonLoopingAmbient%d", index);
			if (ini->Is_Present(section, entry)) {
				ini->Get_String(section, entry, "ERROR", name, sizeof(name));
				sprintf(entry, "MinDelay%d", index);
				int min_delay = (int)(ini->Get_Float(section, entry, m_min_delay) * 1000.0f);
				sprintf(entry, "MaxDelay%d", index);
				int max_delay = (int)(ini->Get_Float(section, entry, m_max_delay) * 1000.0f);
				sprintf(entry, "MinPan%d", index);
				float min_pan = ini->Get_Float(section, entry, m_min_pan);
				sprintf(entry, "MaxPan%d", index);
				float max_pan = ini->Get_Float(section, entry, m_max_pan);
				Add_Random_Sound(race, name, min_delay, max_delay, min_pan, max_pan, false);
			}
		}
	}
}

/*
Reconstruction notes:
bridgesounds.cpp holds BridgeSoundManager (derived from AmbientSoundManager)
which loads bridgesounds.ini for bridge ambient audio loops and delays.
Confirmed object boundary 0x436A60-0x4370A0. Not rebuilt yet: 0x437070 (Stop_Sounds with the fade-out; no caller found).
*/
