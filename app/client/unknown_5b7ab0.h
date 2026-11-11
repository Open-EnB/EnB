// Earth & Beyond client (reconstruction): app\client\unknown_5b7ab0.h, the menu window of the view at SClient +0x1358
// (interim placement, D137: the client's object 0x5B7AB0-0x5B8120 has no __FILE__ string; file name provisional,
// NAME 1). Moved out of sclient.h, which includes it.

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __UNKNOWN_5B7AB0_H  // NAME 1 [unknown_5b7ab0.h]: interim placement (D137)
#define __UNKNOWN_5B7AB0_H

#include "unknown_5bb250.h"

// the interface object of the view at SClient +0x1358 (0xAC bytes, tables 0xAF30A8 / 0xAF309C / 0xAF3090): the mesh
// mu01 and its five option buttons MU01A.OPTION1..5 (ids 1..5), with the animations mu01a.mu01-idl / -on / -o1..o5
class Unknown5B7AB0Class : public Unknown5BB320Class	// NAME 1: neutral (D10)
{
public:
	Unknown5B7AB0Class(UIViewClass *view, const LayerClass &layer);	// 0x5B7AB0 (0x514E70 passes its view and a layer of the base's +0x1C / +0x20; the mesh made after the base)  // NAME 1 [view, layer]: what 0x514E70 passes
	virtual ~Unknown5B7AB0Class();						// +0x00 0x5B7BA0 (deletes the mesh first; the deleting destructor 0x5B7B70)
	virtual void Unknown_54();							// +0x54 0x5B7C40 (the five option buttons, then slot +0x10)
	virtual void Unknown_58();							// +0x58 0x5B7EE0 (the base's under a sort scope, then the idle and on animations)
	virtual void Unknown_5C();							// +0x5C 0x5B7FA0 (the base's)
	virtual void Unknown_60();							// +0x60 0x5B80B0 (the base's)
	virtual bool Unknown_68(int key);					// +0x68 0x5B7FC0 (keys 1..5: that option's animation, then the on one; false)  // NAME 1 [key]: InterfaceClass's
};

#endif //__UNKNOWN_5B7AB0_H
