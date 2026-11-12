#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __BRIDGESOUNDS_H  // NAME 2: bridgesounds.cpp is the client's; header name assumed
#define __BRIDGESOUNDS_H

#include "ambientsoundmanager.h"

class INIClass;

class BridgeSoundManager : public AmbientSoundManager  // NAME 2: bridgesounds.cpp is the client's; class name assumed
{
public:
	BridgeSoundManager();
	~BridgeSoundManager();
	void Reset(int time);
	void Play_Sounds(int channel, int time);  // NAME 2: function name assumed
	void Stop_Sound(int channel);  // NAME 2: function name assumed
	void Unknown_437070();  // 0x437070: AmbientSoundManager's 0x435FA0 with m_fade_out (bridge_int.cpp: the bridge left its scene)
	void Load_INI();  // NAME 2: function name assumed
	void Load_Section(INIClass *ini, const char *section, int race);  // NAME 2: function name assumed

	int m_fade_out;     // 0x2c  // NAME 2: from bridgesounds.ini "FadeOut"
	float m_min_delay;  // 0x30  // NAME 2: from bridgesounds.ini "DefaultMinDelay"
	float m_max_delay;  // 0x34  // NAME 2: from bridgesounds.ini "DefaultMaxDelay"
	float m_min_pan;    // 0x38  // NAME 2: from bridgesounds.ini "DefaultMinPan"
	float m_max_pan;    // 0x3c  // NAME 2: from bridgesounds.ini "DefaultMaxPan"
	int m_channel;      // 0x40  // NAME 1: race / sound channel index
};

#endif // __BRIDGESOUNDS_H
