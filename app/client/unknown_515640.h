// Earth & Beyond client (reconstruction): an interface with a gauge of the avatar's height (client 0x515640-0x515A53).
// File and header names PROVISIONAL (D137): no string; between smouse.cpp (to 0x51562F, its last static
// initializer) and the dynawin.ini file at 0x515A60 (its INIClass's initializer, then 0x515AB0), before
// dynamic_window_background.cpp (0x515F00).
#ifndef UNKNOWN_515640_H
#define UNKNOWN_515640_H

#include "intrface.h"
#include "vector2.h"
#include "vector4.h"
#include "unknown_686010.h"

// What +0x84 points to: Unknown686010Class (unknown_686010.h, 0x686010-0x68672F)

// The interface (table 0xAED2B4; 0xAED2A8 at +0x04, 0xAED29C at +0x38): InterfaceClass with slots +0x58 /
// +0x5C / +0x60 of its own. Nothing constructs it in client.exe (0x515640 has no caller and no reference).
class Unknown515640Class : public InterfaceClass  // NAME 1: neutral (D10)
{
public:
	Unknown515640Class(UIViewClass *view, int unknown);					// 0x515640  // NAME 1 [view, unknown]: InterfaceClass's
	virtual ~Unknown515640Class();										// 0x5157A0 (scalar deleting 0x515770; thunks 0x515A40 / 0x515A50)
	virtual void Unknown_58();											// +0x58 0x515840: the gauge made, then InterfaceClass's
	virtual void Unknown_5C();											// +0x5C 0x515930: the gauge deleted, then InterfaceClass's
	virtual void Unknown_60();											// +0x60 0x515980: the gauge set to the avatar's height within the client's bounds

	Unknown686010Class *Unknown84;										// +0x84
};

#endif
