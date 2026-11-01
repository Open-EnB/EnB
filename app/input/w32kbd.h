// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\input\w32kbd.h (name assumed from w32kbd.cpp, whose
**	__FILE__ is 'e:\build\beta\G\app\input\w32kbd.cpp')
**
**	The keyboard handler for the window's messages (vtable 0xAE7228, 4 bytes; its state is in
**	w32kbd.cpp's statics). Only what input.cpp uses so far; w32kbd.cpp is not rebuilt yet.
*/



#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __W32KBD_H	// NAME 3 [w32kbd.h]: header name assumed from the client's w32kbd.cpp
#define __W32KBD_H

#include "input.h"
#include "winmsg.h"

class Win32KeyboardHandlerClass : public KeyboardClass	// NAME 1 [Win32KeyboardHandlerClass]: after w32mouse.cpp's Win32MouseHandlerClass
{
public:
	Win32KeyboardHandlerClass(KeyQueueClass *queue);								// 0x430490
	virtual ~Win32KeyboardHandlerClass();											// 0x430600 (scalar deleting)
	virtual void Unknown_04();														// 0x430700
	virtual void Unknown_08();														// 0x430750
	virtual bool Unknown_0C(int unknown1, int unknown2);							// 0x430CA0  // NAME 1 [unknown1, unknown2]: neutral (D10)
	virtual bool Set_Repeat(bool enable);											// 0x4306D0  // NAME 1 [enable]: as KeyboardClass's
};


#endif //__W32KBD_H
