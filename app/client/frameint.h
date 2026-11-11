// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Client\frameint.h (PROVISIONAL, D137: frameint.cpp's header; its __FILE__ 0xB790A4)
*/
#ifndef FRAMEINT_H
#define FRAMEINT_H

#include "intrface.h"

class SimpleSceneClass;

// frameint.cpp's interface (0x90 bytes, tables 0xAEED2C / 0xAEED20 / 0xAEED14): the frame rate (the main view's
// 0x50C690 makes it over its +0x24 scene and registers it). Every frame it smooths the rate, keeps its lowest and
// highest, shows the memory statistics or the free texture memory as the client's display mode asks, and reports
// 'Frame Rate = %4.0f' under 'Render'; its key (0x54, modifier 0x400) resets the lowest and highest.
class FrameInterfaceClass : public InterfaceClass	// NAME 3: class-from-file (D24) for frameint.cpp, as backint.cpp's BackInterfaceClass
{
public:
	FrameInterfaceClass(UIViewClass *view, SimpleSceneClass *scene);	// 0x53E970  // NAME 1 [view, scene]: what 0x50C690 passes
	virtual ~FrameInterfaceClass();						// 0x53EA70 (deleting 0x53EA40)
	virtual void Unknown_54();							// +0x54 0x53EAA0 (empty)
	virtual void Unknown_58();							// +0x58 0x53EAC0 (the base's)
	virtual void Unknown_5C();							// +0x5C 0x53EAE0 (the base's)
	virtual void Unknown_60();							// +0x60 0x53EB00 (the frame: the rate, the statistics, Message_Stat)
	virtual bool Unknown_68(int key);					// +0x68 0x53F2A0 (key 0: the lowest and highest reset; G_ASSERT line 299 otherwise)  // NAME 1 [key]: InterfaceClass's

	float FrameRate;									// +0x84 (nine tenths the last, one tenth this frame's)  // NAME 2: 'Frame Rate = %4.0f'
	float MinFrameRate;									// +0x88 (starts at 99.4)  // NAME 1: what it keeps
	float MaxFrameRate;									// +0x8C (starts at 0)  // NAME 1: what it keeps
};

#endif
