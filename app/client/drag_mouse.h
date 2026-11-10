// Earth & Beyond client (reconstruction): app\client\drag_mouse.h, a drag of the software mouse (the client's
// drag_mouse.cpp per its asserts, 'e:\build\beta\G\app\Client\drag_mouse.cpp' at 0xB8FC14; this header's name is
// provisional, NAME 1, D137). Laid out from the constructor 0x684F20 (table 0xAFC3D4).
#ifndef DRAG_MOUSE_H  // NAME 1 [drag_mouse.h]: provisional header name for drag_mouse.cpp
#define DRAG_MOUSE_H

class LoginUnknown18Class;	// tsk_login.h's software mouse (smouse.cpp)

// drag_mouse.cpp's drag (0x14 bytes): the mouse holds it while a button is down (smouse.cpp's 0x515510 / 0x5155A0)
class SMouseDragClass	// NAME 2: smouse.cpp's name for what LoginUnknown18Class +0x2C holds
{
public:
	enum { STATE_INACTIVE = 0 };						// NAME 3: the asserts 'State == STATE_INACTIVE', 'State != STATE_INACTIVE'
	SMouseDragClass(LoginUnknown18Class *mouse);		// 0x684F20 (G_ASSERT 'Mouse' l.45)  // NAME 3 [mouse]: the assert's
	virtual ~SMouseDragClass();							// +0x00 0x684FB0 (deleting 0x684F70)
	virtual void Begin();								// +0x04 0x684FD0 (G_ASSERT 'State == STATE_INACTIVE' l.58: State 1, the mouse's position kept)  // NAME 2: what it does
	virtual bool Drop();								// +0x08 0x6850E0 (G_ASSERT 'State != STATE_INACTIVE' l.93: State 3 when dragging, else +0x0C)  // NAME 2: what it does
	virtual bool Cancel();								// +0x0C 0x685150 (State 0, off the mouse)  // NAME 2: what it does
	virtual bool Update(float x, float y) = 0;			// +0x10 (drag_slot_data.cpp's 0x685380)  // NAME 2: what it does; NAME 1 [x, y]: the pointer's (0..1)
	virtual float Threshold();							// +0x14 0x685040 (0.0009: the squared distance a drag starts at)  // NAME 2: what it returns
	bool Update_Drag(float x, float y);					// 0x685060 (State 2 once past Threshold; the mouse hidden while dragging)  // NAME 2: what it does; NAME 1 [x, y]: as Update's
	void Unknown_685190();								// 0x685190 (smouse.cpp: let go, +0x0C while State is set)

	LoginUnknown18Class *Mouse;							// +0x04  // NAME 3: the G_ASSERT's
	float X;											// +0x08 (the mouse's position at Begin)  // NAME 2: what it holds
	float Y;											// +0x0C  // NAME 2: what it holds
	int State;											// +0x10 (0 inactive, 1 pressed, 2 dragging, 3 dropped)  // NAME 3: the asserts'
};

#endif // DRAG_MOUSE_H
