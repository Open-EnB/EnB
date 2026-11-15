// Earth & Beyond client (reconstruction): a gauge drawn with screen lines (client 0x686010-0x68672F).
// File and header names PROVISIONAL (D137): no string; between the action entries' object (0x685E80 / 0x685F90, after
// fontsys_cursor.cpp) and intrlist.cpp (0x686740); the object ends with <string>'s guarded static 0x686710.
#ifndef UNKNOWN_686010_H
#define UNKNOWN_686010_H

#include "vector2.h"
#include "vector4.h"

class UIViewClass;
class Line2DClass;

// What Unknown515640Class's +0x84 points to (0x2C bytes, no table): a frame of three lines open on one side and a
// fourth line across it at the level (0..1) of the frame's height or width, as the type says
class Unknown686010Class  // NAME 1: neutral (D10)
{
public:
	Unknown686010Class(UIViewClass *view, const Vector4 &color, const Vector2 &pos, const Vector2 &size, int type, float width);	// 0x686010  // NAME 1 [view, color, pos, size, type, width]: what it does with them (0x515840 passes the view, green 0 / 0.6 / 0 / 1, 0.96 / 0.1, 0.03 / 0.3, 1, 2.4)
	~Unknown686010Class();																// 0x686350
	void Unknown_6863E0(const Vector2 &pos, const Vector2 &size, int type, float width);	// 0x6863E0: the frame placed (a type 0 keeps the one it has)  // NAME 1 [pos, size, type, width]: what it does with them
	bool Unknown_6865E0(float level, float width);										// 0x6865E0: the level, 0..1 (-1 keeps it; false when unchanged)  // NAME 1 [level, width]: what it does with them

	UIViewClass *View;									// +0x00  // NAME 1: what it holds
	float Level;										// +0x04 (0..1)  // NAME 1: what 0x6865E0 clamps
	Vector2 Position;									// +0x08  // NAME 1: what 0x6863E0 stores
	Vector2 Size;										// +0x10  // NAME 1: what 0x6863E0 stores
	int Type;											// +0x18 (1-4: the frame's open side and the level's direction)  // NAME 1: neutral
	Line2DClass *Frame[3];								// +0x1C  // NAME 1: what they draw
	Line2DClass *Bar;									// +0x28 (the color's first three components rotated)  // NAME 1: what it draws
};

#endif
