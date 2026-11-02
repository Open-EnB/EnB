// Earth & Beyond client (reconstruction): an interface with a gauge of the avatar's height (client 0x515640-0x515A53).
// File and header names PROVISIONAL (D137): no string; between smouse.cpp (to 0x51562F, its last static
// initializer) and the dynawin.ini file at 0x515A60 (its INIClass's initializer, then 0x515AB0), before
// dynamic_window_background.cpp (0x515F00).
#ifndef UNKNOWN_515640_H
#define UNKNOWN_515640_H

#include "intrface.h"
#include "vector2.h"
#include "vector4.h"

// What +0x84 points to (0x2C bytes, constructor 0x686010: two 0xF4-byte elements; destructor 0x686350), as far
// as this file calls it
class Unknown686010Class  // NAME 1: neutral (D10)
{
public:
	Unknown686010Class(UIViewClass *view, const Vector4 &color, const Vector2 &pos, const Vector2 &size, int unknown, float unknown2);	// 0x686010  // NAME 1 [view, color, pos, size, unknown, unknown2]: what 0x515840 passes (the view, green 0 / 0.6 / 0 / 1, 0.96 / 0.1, 0.03 / 0.3, 1, 2.4)
	~Unknown686010Class();																// 0x686350
	void Unknown_6865E0(float value, int unknown);										// 0x6865E0: the level, 0..1  // NAME 1 [value, unknown]: what 0x515980 passes (the height's fraction, 0)

	char Unknown00[0x2C];
};

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
