#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __STARBASESOUND_H  // NAME 2: starbasesound.cpp is the client's; header name assumed
#define __STARBASESOUND_H

#include "ambientsoundmanager.h"

class INIClass;
class UIMeshClass;
class RenderObjClass;

// A starbase's ambient sounds, from starbasesounds<n>.ini: five channels, one per room ('Hangar' ... : the
// section-name table 0xB6F73C). 0x4C bytes (the starbase view's +0x1F4; the view is 0x240).
class StarbaseSoundClass : public AmbientSoundManager  // NAME 3: class-from-file (D24) for audio/starbasesound.cpp
{
public:
	enum {
		NUM_ROOMS = 5,	// NAME 2: the channel count 0x441910 / 0x441A50 pass
	};

	StarbaseSoundClass(int starbase);		// 0x441910 (the view's constructor passes its second argument)  // NAME 2 [starbase]: starbasesounds%d.ini's number
	~StarbaseSoundClass();				// 0x4419B0: the ini deleted
	void Set_Room(int room);				// 0x441A30 (the starbase view's 0x69D030 passes its interface index): the base's Set_Pending_Channel  // NAME 2: what it does
	void Reset(int room);					// 0x441A50 (0x6A2480): the base's Reset and Initialize, Load_INI, Set_Pending_Channel  // NAME 2: as BridgeSoundManager's
	void Update(int time, UIMeshClass *mesh);	// 0x441A90 (FAIL_IF 'mesh == NULL', l.155; 0x69C8F0: the client's +0x10BC, the starbase interface's +0x84)  // NAME 2: what it does
	void Load_INI();						// 0x441C40 ('starbasesounds%d.ini')  // NAME 2: as BridgeSoundManager's
	void Load_Section(int room, const char *section_name);	// 0x441DD0 (0x6A24C0): the room's 2-D sounds  // NAME 2: as BridgeSoundManager's
	void Load_Section(int room, RenderObjClass *rendobj, const char *section_name);	// 0x4420E0 (0x6A24C0, 0x6A2500): the room's sounds at rendobj's bones  // NAME 2: as BridgeSoundManager's

	int Unknown2C;				// +0x2C: 0 (the constructor)
	int Starbase;				// +0x30  // NAME 2: what it holds
	int FadeOut;				// +0x34: 500  // NAME 2: the ini's FadeOut
	float DefaultMinDelay;		// +0x38: 10.0  // NAME 2: the ini's DefaultMinDelay
	float DefaultMaxDelay;		// +0x3C: 20.0  // NAME 2: the ini's DefaultMaxDelay
	float DefaultMinPan;		// +0x40: 0.5  // NAME 2: the ini's DefaultMinPan
	float DefaultMaxPan;		// +0x44: 0.5  // NAME 2: the ini's DefaultMaxPan
	INIClass *INI;				// +0x48: Load_INI's, kept for Load_Section  // NAME 2: what it holds
};

#endif // __STARBASESOUND_H
