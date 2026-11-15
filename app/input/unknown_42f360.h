// Earth & Beyond client (reconstruction): the drag tracker of constructor 0x42F360 (notes in unknown_42f360.cpp)
#if defined(_MSC_VER)
#pragma once
#endif

#ifndef UNKNOWN_42F360_H  // NAME 1 [unknown_42f360.h]: neutral (D137)
#define UNKNOWN_42F360_H

#include "input.h"
#include "vector2.h"

// A mouse button's press, click and drag tracker (0x24 bytes, table 0xAE71D0: five empty virtuals for a subclass); the
// client keeps no caller of it
class InputUnknownAE71D0Class	// NAME 1 [InputUnknownAE71D0Class]: neutral (D10)
{
public:
	InputUnknownAE71D0Class(unsigned short button, float threshold, unsigned int click_time);	// 0x42F360  // NAME 2 [button, threshold, click_time]: what Handle_Event does with them
	bool Handle_Event(KernelEventClass *event);					// 0x42F3A0: true when the event is the button's  // NAME 2 [Handle_Event]: what it does

	virtual void Press(const Vector2 &pos) {}					// +0x00 0x42F7E0 (the button down, from idle)  // NAME 2: when it is called
	virtual void Click(const Vector2 &pos) {}					// +0x04 0x42F800 (a second press released within the click time)  // NAME 2: when it is called
	virtual void Drag_Begin(const Vector2 &start) {}			// +0x08 0x42F820 (moved past the threshold)  // NAME 2: when it is called
	virtual void Drag(const Vector2 &min, const Vector2 &max) {}	// +0x0C 0x42F840 (the rectangle from the press)  // NAME 2: when it is called
	virtual void Drag_End(const Vector2 &min, const Vector2 &max) {}	// +0x10 0x42F860 (released while dragging)  // NAME 2: when it is called

	unsigned short Button;			// +0x04 the event key it follows  // NAME 2: what it holds
	int State;						// +0x08 0 idle, 1 pressed, 2 held, 3 pressed again, 4 dragging  // NAME 2: what it holds
	bool Active;					// +0x0C the press is live (false after a release)  // NAME 2: what it holds
	Vector2 Start;					// +0x10 the press's position  // NAME 2: what it holds
	float Threshold;				// +0x18 the drag distance, squared  // NAME 2: what it holds
	unsigned int ClickTime;			// +0x1C  // NAME 2: what it holds
	unsigned int PressTime;			// +0x20 the press event's time  // NAME 2: what it holds
};

#endif
