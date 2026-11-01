// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\input\w32mouse.h (name assumed from w32mouse.cpp, whose
**	__FILE__ is 'e:\build\beta\G\app\input\w32mouse.cpp')
**
**	The mouse handler for the window's messages (vtable 0xAE7248, 4 bytes; the client's string
**	'Win32MouseHandlerClass: capturing mouse input.'). Only what input.cpp uses so far;
**	w32mouse.cpp is not rebuilt yet.
*/



#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __W32MOUSE_H	// NAME 3 [w32mouse.h]: header name assumed from the client's w32mouse.cpp
#define __W32MOUSE_H

#include "input.h"

class Win32MouseHandlerClass : public MouseClass
{
public:
	Win32MouseHandlerClass(KeyQueueClass *queue);									// 0x430D90
	virtual ~Win32MouseHandlerClass();												// 0x430EB0 (scalar deleting)
	virtual void Unknown_04();														// 0x430F50
	virtual void Unknown_08();														// 0x4310E0
	virtual void Unknown_0C(int unknown1, int unknown2);							// 0x431900  // NAME 1 [unknown1, unknown2]: neutral (D10)
	virtual void Get_Position(float *x, float *y);									// 0x431950
	virtual void Unknown_14(int unknown1, int unknown2);							// 0x431C30  // NAME 1 [unknown1, unknown2]: neutral (D10)
	virtual void Set_Position(float x, float y);									// 0x431B70
	virtual void Update();															// 0x431AA0
};

#endif //__W32MOUSE_H
