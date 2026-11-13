// Earth & Beyond client (reconstruction): app\client\shortcut_range_gauge.h, the range gauge on a shortcut's slot (the
// client's shortcut_range_gauge.cpp per its assert, 'e:\build\beta\G\app\Client\shortcut_range_gauge.cpp' at 0xB91270;
// this header's name is provisional, NAME 1, D137). Laid out from the constructor 0x6A3D60 and the destructor 0x6A3F10
// (O18, 2026-11-13).
#ifndef SHORTCUT_RANGE_GAUGE_H  // NAME 1 [shortcut_range_gauge.h]: provisional header name for shortcut_range_gauge.cpp
#define SHORTCUT_RANGE_GAUGE_H

#include "..\..\wwlib\ref_ptr.h"

class RenderObjClass;
class AudibleSoundClass;
class Unknown4A13F0Class;		// uvcontrl.h's frame mapper

// The range gauge (0x18 bytes, no table; inv_render.cpp's and skill_render.cpp's slots make it over their mesh): the
// 'if_targ' model on the slot's bone, its frame (0..15) from the target's distance against the range, turned while in
// range, and the 'weapon_range_green_1.wav' sound
class ShortcutRangeGaugeClass	// NAME 3: class-from-file (D24) for app\Client\shortcut_range_gauge.cpp
{
public:
	ShortcutRangeGaugeClass(RenderObjClass *robj);		// 0x6A3D60 (FAIL_IF '!RenderObject' l.48)  // NAME 1 [robj]: what the slots pass
	~ShortcutRangeGaugeClass();							// 0x6A3F10
	void Unknown_6A4020(float distance, float range, int state, bool arg);	// 0x6A4020: hidden unless state and a range; the frame and the turn from distance / range  // NAME 1 [Unknown_6A4020, distance, range, state, arg]: neutral (D10)
	void Unknown_6A43D0(int mode);						// 0x6A43D0: the sound for mode (1: none)  // NAME 1 [Unknown_6A43D0, mode]: neutral (D10)
	void Unknown_6A4500();								// 0x6A4500 (no callers: inlined by the others): the sound stopped  // NAME 1: neutral (D10)

	RefCountPtr<RenderObjClass> RenderObject;			// +0x00 ('if_targ')  // NAME 5: the FAIL_IF '!RenderObject'
	Unknown4A13F0Class *Unknown04;						// +0x04 (its frames)  // NAME 1: neutral (D10)
	int Frame;											// +0x08 (-1 until set; 0..15)  // NAME 2: what Unknown04 gets
	float Angle;										// +0x0C (the turn about z, below pi / 2)  // NAME 2: what it holds
	RefCountPtr<AudibleSoundClass> Sound;				// +0x10  // NAME 2: what it holds
	int SoundMode;										// +0x14 (Unknown_6A43D0's mode)  // NAME 2: what it holds
};

#endif // SHORTCUT_RANGE_GAUGE_H
