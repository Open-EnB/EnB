// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: unknown (PROVISIONAL, D137: a neutral name; notes in unknown_51c800.cpp)
**
**	A dynamic window element over a font style's text: DynamicWindowRenderObjectClass with the text as its
**	model (table 0xAED85C, 0x4C bytes). The message box's headers and option texts, the tooltip's lines.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef UNKNOWN_51C800_H  // NAME 1 [unknown_51c800.h]: neutral (D137)
#define UNKNOWN_51C800_H

#include "dynamic_window_render_object.h"

class FontSys;

class Unknown51C800Class : public DynamicWindowRenderObjectClass	// NAME 1: neutral (D10)
{
public:
	Unknown51C800Class(FontSys *text, unsigned int flags);	// 0x51C800  // NAME 1 [text, flags]: what the callers pass (6)
	// (implicit destructor: 0x51C8F0, deleting 0x51C8C0)
	virtual void Set_Render_Object(const RefCountPtr<RenderObjClass> &robj) { DynamicWindowRenderObjectClass::Set_Render_Object(robj); }	// +0x58 0x51C910
};

#endif
