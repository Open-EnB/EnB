#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __SECTORSOUND_H  // NAME 2: sectorsound.cpp is the client's; header name assumed
#define __SECTORSOUND_H

#include "ambientsoundmanager.h"

class INIClass;

// A sector's ambient sounds, from sectorsounds<sector>.ini: one channel per section that has a MaxElevation,
// and the channel whose elevation band holds the listener plays. 0x58 bytes (SClient's +0x1330: made at
// 0x750A70 for SERVER_PARAMETERS' sector, updated from 0x750B30).
class SectorSoundClass : public AmbientSoundManager  // NAME 3: class-from-file (D24) for audio/sectorsound.cpp
{
public:
	// one channel's elevation band (0xC bytes; the vector's insert is 0x43CEA0)
	struct ElevationStruct  // NAME 2: what it holds
	{
		int Channel;			// +0x00  // NAME 2: what Play_Ambient_Sounds gets
		float MaxElevation;		// +0x04  // NAME 2: the ini's MaxElevation
		float MinElevation;		// +0x08: the next lower section's MaxElevation, -9999.0 for the lowest  // NAME 2: what it holds
	};

	SectorSoundClass(unsigned sector);	// 0x43C4A0  // NAME 1 [sector]: what 0x750A70 passes; sectorsounds%d.ini's number
	~SectorSoundClass();					// 0x43C560
	void Reset();							// 0x43C630: the base's Reset, then Load_INI  // NAME 2: as BridgeSoundManager's
	void Update(int time, float elevation);	// 0x43C660: the channel of elevation's band started when the band changes, then the base's Update  // NAME 2: what it does
	void Load_INI();						// 0x43C780 ('sectorsounds%d.ini')  // NAME 2: as BridgeSoundManager's
	void Load_Section(INIClass *ini, const char *section_name, int channel);	// 0x43CB70 (FAIL_IF 'ini == NULL', l.260; section_name is the client's)  // NAME 2: as BridgeSoundManager's

	int CurrentElevation;						// +0x2C: index into Elevations, -1 none  // NAME 2: what it holds
	std::vector<ElevationStruct> Elevations;	// +0x30  // NAME 2: what it holds
	unsigned Sector;							// +0x40  // NAME 2: what it holds
	int FadeOut;								// +0x44: 500  // NAME 2: the ini's FadeOut
	float DefaultMinDelay;						// +0x48: 10.0  // NAME 2: the ini's DefaultMinDelay
	float DefaultMaxDelay;						// +0x4C: 20.0  // NAME 2: the ini's DefaultMaxDelay
	float DefaultMinPan;						// +0x50: 0.5  // NAME 2: the ini's DefaultMinPan
	float DefaultMaxPan;						// +0x54: 0.5  // NAME 2: the ini's DefaultMaxPan
};

#endif // __SECTORSOUND_H
