// Earth & Beyond client (reconstruction): app\Audio\bridgesounds.cpp. Notes at the end of the file.
#include "bridgesounds.h"

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
void BridgeSoundManager::Stop_Sound(int channel)
{
	AmbientSoundManager::Stop_Sound(channel);
}

/*
Reconstruction notes:
bridgesounds.cpp holds BridgeSoundManager (derived from AmbientSoundManager)
which loads bridgesounds.ini for bridge ambient audio loops and delays.
Confirmed object boundary 0x436A60-0x4370A0.
*/
