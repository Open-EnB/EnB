// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Client\acticon.h (name assumed from acticon.cpp, whose __FILE__ is
**	'e:\build\beta\G\app\Client\acticon.cpp'; the client holds no string of this header)
**
**	The cockpit's warp button (ui_cpit.cpp makes it over its 'WARP' model and 'WARP_BAR' texture): unknown_671300.h's
**	button with a state, its bar's texture scrolled as a warp charges or cools and its model pulsed while it waits,
**	the model flashed white on a state change. Table 0xAFBB58 (44 slots: Unknown_671300Class's, +0xAC its own),
**	constructor 0x676000, 0x94 bytes.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef ACTICON_H  // NAME 3 [acticon.h]: header name assumed from the client's acticon.cpp
#define ACTICON_H

#include "unknown_671300.h"
#include "..\..\wwlib\ref_ptr.h"

class ActIconClass : public Unknown_671300Class  // NAME 3: class-from-file (D24) for app\Client\acticon.cpp
{
public:
	ActIconClass(RenderObjClass *robj, RenderObjClass *bar, const LayerClass &layer);	// 0x676000 (the base's 0x671300 with the bar, 0, 0, 3)  // NAME 1 [robj, layer]: the base's; NAME 2 [bar]: ui_cpit.cpp's 'WARP_BAR'
	// +0x00 0x676100 (deleting 0x6760D0): the implicit destructor (Bar released; no table store)
	virtual void Unknown_AC();							// +0xAC 0x6762B0 (every frame: the flash fades, the bar scrolls by the state)

	void Set_State(int state, unsigned int end_time);	// 0x676190 (ui_cpit.cpp: 5 incapacitated, 0 idle, 4 / 1 until a time, 2 flashed)  // NAME 2: what it does; NAME 1 [state, end_time]: what ui_cpit.cpp passes

	RefCountPtr<RenderObjClass> Bar;					// +0x80 (the constructor's second model; slot +0xAC's 0x496110 highlight on it)  // NAME 2: ui_cpit.cpp's 'WARP_BAR'
	int State;											// +0x84 (Set_State's: 0 idle, 1 / 4 timed, 2 / 3 flashed, 5 off)  // NAME 2: what Set_State stores
	float Duration;										// +0x88 (the seconds to end_time)  // NAME 1: what Set_State stores
	unsigned int EndTime;								// +0x8C (Set_State's sync time)  // NAME 1: what Set_State stores
	float Flash;										// +0x90 (1 on a flash, down to 0 by the frame time)  // NAME 1: what slot +0xAC does with it
};

#endif
