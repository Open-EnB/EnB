// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: unknown (PROVISIONAL, D137: a neutral name; notes in unknown_66b770.cpp)
**
**	A model gadget with highlight / hide modes (+0x60): table 0xAFA890, 0x64 bytes, constructors 0x66B770 /
**	0x66B7E0, destructor 0x66B820. The base of the button gadget 0x675240 / 0x675170 (ui_scut.cpp's
**	UIScutButtonGadgetClass), so of gd_proxy.cpp's proxy buttons too.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef UNKNOWN_66B770_H  // NAME 1 [unknown_66b770.h]: neutral (D137)
#define UNKNOWN_66B770_H

#include "gd_robj.h"

class Unknown_66B770Class : public RObjGadgetClass  // NAME 1: neutral (D10)
{
public:
	Unknown_66B770Class(RenderObjClass *robj, const LayerClass &layer, unsigned int mask, unsigned int flags);	// 0x66B770  // NAME 1 [robj, layer, mask, flags]: the base's and what it does
	Unknown_66B770Class(RenderObjClass *robj, unsigned int mask, unsigned int flags);	// 0x66B7E0  // NAME 1 [robj, mask, flags]: the base's and what it does
	virtual ~Unknown_66B770Class();										// +0x00 0x66B820 (deleting 0x66B7B0)
	virtual bool Unknown_2C(KernelEventClass *event, float x, float y, unsigned int mask);	// +0x2C 0x66B910 (presses and releases: the held gadgets, the highlight)  // NAME 1 [event, x, y, mask]: the base's
	virtual void Unknown_40(bool on);									// +0x40 0x66B840 (the modes applied, then the base's)  // NAME 1 [on]: the base's

	unsigned int Unknown60;												// +0x60: bit 0 highlight while held, bit 1 highlight while off, bit 2 hidden while off  // NAME 1: neutral (D10)
};

#endif
