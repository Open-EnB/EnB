// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\input\input.h (the client's assert string
**	'e:\build\beta\G\app\input\input.h', TrackMouseCounter, used by w32kbd.cpp's 0x42F3A0)
**
**	The input layer (input.cpp): one event queue (0xBDA968) that the keyboard and mouse handlers
**	fill from the window's messages, the keyboard (0xBDAA70), the mouse (0xBDAA74) and a joystick
**	placeholder (0xBDAA78). Every class here is named neutrally (D10): the client keeps no RTTI
**	for them.
*/



#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __INPUT_H
#define __INPUT_H

#include <windows.h>
#include "..\kernel\kernel.h"

#define INPUT_KEY_COUNT	0x103	// NAME 1 [INPUT_KEY_COUNT]: neutral (D10): the key tables' size (input.cpp, w32kbd.cpp)

// A ring of events (vtable 0xAE7184; Head == Tail: empty, one slot stays free)
class InputQueueClass	// NAME 1 [InputQueueClass]: neutral (D10)
{
public:
	InputQueueClass(unsigned int size) : Head(0), Tail(0), Size(size) { Buffer = new KernelEventClass[size]; }
	virtual ~InputQueueClass() { delete Buffer; }									// 0x42F1C0 (scalar deleting)
	virtual bool Add(KernelEventClass &event)										// 0x42F2F0: false when full  // NAME 1 [Add]: what it does
	{
		unsigned int next = (Tail + 1) % Size;
		if (next == Head) {
			return false;
		}
		Buffer[Tail] = event;
		Tail = next;
		return true;
	}
	bool Peek(KernelEventClass &event)	// inline only: the oldest event, kept  // NAME 1 [Peek]: what it does
	{
		if (Head == Tail) {
			return false;
		}
		event = Buffer[Head];
		return true;
	}
	bool Get(KernelEventClass &event)	// inline only: the oldest event, taken  // NAME 1 [Get]: what it does
	{
		if (Head == Tail) {
			return false;
		}
		event = Buffer[Head];
		Head = (Head + 1) % Size;
		return true;
	}

	KernelEventClass *Buffer;		// +0x04  // NAME 2: what it holds
	unsigned int Head;				// +0x08: the next event to take  // NAME 2: what it holds
	unsigned int Tail;				// +0x0C: the next free slot  // NAME 2: what it holds
	unsigned int Size;				// +0x10: 0xFF  // NAME 2: what it holds
};

// The keyboard's queue (0x420 bytes, vtable 0xAE7178): a key's release takes its press's flags
class KeyQueueClass : public InputQueueClass	// NAME 1 [KeyQueueClass]: neutral (D10)
{
public:
	KeyQueueClass() : InputQueueClass(0xFF) { memset(KeyStates, 0, sizeof(KeyStates)); }
	virtual bool Add(KernelEventClass &event);										// 0x42EEC0

	unsigned int KeyStates[INPUT_KEY_COUNT];	// +0x14: 0x10000 | the press's flags while a key is down  // NAME 2: what it holds
};

// The keyboard handler's base (vtable 0xAE7190, 4 bytes)
class KeyboardClass		// NAME 1 [KeyboardClass]: neutral (D10)
{
public:
	KeyboardClass(KeyQueueClass *queue) { Queue = queue; }
	virtual ~KeyboardClass() {}														// 0x42F060 (scalar deleting)
	virtual void Unknown_04() = 0;	// NAME 1 [Unknown_04]: neutral (D10)
	virtual void Unknown_08() = 0;	// NAME 1 [Unknown_08]: neutral (D10)
	virtual bool Unknown_0C(int unknown1, int unknown2) = 0;	// NAME 1 [Unknown_0C, unknown1, unknown2]: neutral (D10)
	virtual bool Set_Repeat(bool enable) = 0;	// +0x10: 0x42EE00 counts its callers  // NAME 1 [Set_Repeat, enable]: 0x42EE00's 'KeyRepeatCount'

	static KeyQueueClass *Queue;	// 0xBDAAC4  // NAME 2: what it holds
};

// The keyboard for DIRECTX_KEYBOARD=T (vtable 0xAE7160): it does nothing
class InputUnknownAE7160Class : public KeyboardClass	// NAME 1 [InputUnknownAE7160Class]: neutral (D10)
{
public:
	InputUnknownAE7160Class(KeyQueueClass *queue) : KeyboardClass(queue) {}
	virtual void Unknown_04() {}													// 0x42EF90
	virtual void Unknown_08() {}													// 0x42EFB0
	virtual bool Unknown_0C(int unknown1, int unknown2) { return false; }			// 0x42EFD0  // NAME 1 [unknown1, unknown2]: neutral (D10)
	virtual bool Set_Repeat(bool enable) { return false; }							// 0x42EFF0  // NAME 1 [enable]: as KeyboardClass's
};

// The mouse handler's base (vtable 0xAE71A8, 4 bytes)
class MouseClass		// NAME 1 [MouseClass]: neutral (D10)
{
public:
	MouseClass(KeyQueueClass *queue) { Queue = queue; }
	virtual ~MouseClass() {}														// 0x42F220 (scalar deleting)
	virtual void Unknown_04() = 0;	// NAME 1 [Unknown_04]: neutral (D10)
	virtual void Unknown_08() = 0;	// NAME 1 [Unknown_08]: neutral (D10)
	virtual void Get_Pixel_Position(int *x, int *y) = 0;	// +0x0C: in the client area  // NAME 1 [Get_Pixel_Position, x, y]: what 0x431900 does
	virtual void Get_Position(float *x, float *y) = 0;		// +0x10: 0..1 of the window  // NAME 1 [Get_Position, x, y]: what 0x42ECF0's callers do with it
	virtual void Set_Pixel_Position(int x, int y) = 0;		// +0x14  // NAME 1 [Set_Pixel_Position, x, y]: what 0x431C30 does
	virtual void Set_Position(float x, float y) = 0;		// +0x18: 0x42ED80 centres it (0.5, 0.5)  // NAME 1 [Set_Position, x, y]: what 0x42ED80 does with it
	virtual void Update() = 0;								// +0x1C: 0x42EDB0, once a frame  // NAME 1 [Update]: what its callers do with it

	static KeyQueueClass *Queue;	// 0xBDAAC8  // NAME 2: what it holds
};

// The mouse for DIRECTX_MOUSE=T (vtable 0xAE7134, one virtual more than the base): it does nothing
class InputUnknownAE7134Class : public MouseClass	// NAME 1 [InputUnknownAE7134Class]: neutral (D10)
{
public:
	InputUnknownAE7134Class(KeyQueueClass *queue) : MouseClass(queue) {}
	virtual void Unknown_04() {}													// 0x42F090
	virtual void Unknown_08() {}													// 0x42F0B0
	virtual void Get_Pixel_Position(int *x, int *y) {}							// 0x42F0D0
	virtual void Get_Position(float *x, float *y) {}								// 0x42F0F0
	virtual void Set_Pixel_Position(int x, int y) {}								// 0x42F110
	virtual void Set_Position(float x, float y) {}									// 0x42F130
	virtual void Unknown_20() {}													// 0x42F150 (slot +0x20, its own)  // NAME 1 [Unknown_20]: neutral (D10)
	virtual void Update() {}														// 0x42F170
};

// What DIRECTX_JOYSTICK makes (1 byte, no virtuals; deleted with operator delete alone)
class InputUnknownBDAA78Class	// NAME 1 [InputUnknownBDAA78Class]: neutral (D10)
{
};

extern InputQueueClass *Unknown_BDA968;				// the event queue (a KeyQueueClass)  // NAME 1 [Unknown_BDA968]: neutral (D10)
extern unsigned char Unknown_BDA96C[INPUT_KEY_COUNT];	// 0x42EB80's key state for ToAscii  // NAME 1 [Unknown_BDA96C]: neutral (D10)
extern KeyboardClass *Unknown_BDAA70;					// NAME 1 [Unknown_BDAA70]: neutral (D10)
extern MouseClass *Unknown_BDAA74;						// NAME 1 [Unknown_BDAA74]: neutral (D10)
extern InputUnknownBDAA78Class *Unknown_BDAA78;		// NAME 1 [Unknown_BDAA78]: neutral (D10)
extern HWND Unknown_BDAA7C;							// the window Init was given  // NAME 1 [Unknown_BDAA7C]: neutral (D10)
extern int TrackMouseCounter;							// 0xBDAA80 (the client's assert in input.h, 0x42F3A0): > 0, 0x42E8A0 makes a pointer event when the queue is empty
extern int KeyRepeatCount;								// 0xBDAA84 (the client's assert 'KeyRepeatCount')

int Unknown_42E390(int unknown1, int unknown2);			// 0x42E390: WM_DESTROY's handler: PostQuitMessage(0)  // NAME 1 [Unknown_42E390, unknown1, unknown2]: neutral (D10)
int Unknown_42E3B0(int unknown1, int unknown2);			// 0x42E3B0: the window to the top (unknown2) or the bottom, no caller  // NAME 1 [Unknown_42E3B0, unknown1, unknown2]: neutral (D10)
void Unknown_42E4A0(HWND window);						// 0x42E4A0: start-up (..\data\input.ini)  // NAME 1 [Unknown_42E4A0, window]: neutral (D10)
void Unknown_42E7B0();									// 0x42E7B0: shut-down  // NAME 1 [Unknown_42E7B0]: neutral (D10)
void Unknown_42E8A0(KernelEventClass *event);			// 0x42E8A0: takes the next event (waits for one)  // NAME 1 [Unknown_42E8A0, event]: neutral (D10)
bool Unknown_42EA80(KernelEventClass *event);			// 0x42EA80: the next event, kept  // NAME 1 [Unknown_42EA80, event]: neutral (D10)
char Unknown_42EB80(KernelEventClass *event);			// 0x42EB80: a key event's character (ToAscii), 0 for none  // NAME 1 [Unknown_42EB80, event]: neutral (D10)
void Unknown_42ECF0(float *x, float *y);				// 0x42ECF0: the pointer's position, 0..1  // NAME 1 [Unknown_42ECF0, x, y]: neutral (D10)
void Unknown_42ED20(float x, float y);					// 0x42ED20  // NAME 1 [Unknown_42ED20, x, y]: neutral (D10)
void Unknown_42ED80();									// 0x42ED80: the pointer to the centre  // NAME 1 [Unknown_42ED80]: neutral (D10)
void Unknown_42EDB0();									// 0x42EDB0: the mouse's frame update  // NAME 1 [Unknown_42EDB0]: neutral (D10)
bool Unknown_42EE00(bool enable);						// 0x42EE00: key repeat on / off, counted  // NAME 1 [Unknown_42EE00, enable]: neutral (D10)
bool Unknown_42EE90();									// 0x42EE90: the window has the focus  // NAME 1 [Unknown_42EE90]: neutral (D10)

#endif //__INPUT_H
