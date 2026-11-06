// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: unknown (PROVISIONAL, D137: a neutral name; the gadget's file is not known)
**
**	The gadget constructor 0x674260 makes (0x60 bytes, over Unknown_675120Class) over a screen's model: as far as
**	ui_bio.cpp's bio tab (gadget 2) and ui_cinfo.cpp's character info window (gadget 6) make it. ui_cpit.cpp
**	declares it locally over GadgetClass (UICpitGadget674260Class): that file does not include unknown_675120.h.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef UNKNOWN_674260_H  // NAME 1 [unknown_674260.h]: neutral (D137)
#define UNKNOWN_674260_H

#include "unknown_675120.h"
#include "..\..\wwlib\ref_ptr.h"

class RenderObjClass;

class Unknown_674260Class : public Unknown_675120Class	// NAME 1: neutral (D10)
{
public:
	Unknown_674260Class(const RefCountPtr<RenderObjClass> &model);	// 0x674260 (0x60 bytes)  // NAME 1 [model]: what the windows pass
	virtual ~Unknown_674260Class();										// 0x6742F0 (deleting 0x674230; out of line next to the constructor: ui_ptrade.cpp's credits button 0x5E4040 calls it)
};

#endif
