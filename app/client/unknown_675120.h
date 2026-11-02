// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: unknown (PROVISIONAL, D137: a neutral name; notes in unknown_675120.cpp)
**
**	The model gadgets that stay out of the scene (their model is part of a screen's model already): slots
**	+0x74 / +0x78 inline, the model not put into the layer's scene, only the flash stopped.
**	Unknown_675120Class over RObjGadgetClass (table 0xAFB870, 0x60 bytes; gd_resize.cpp's drag handle derives
**	from it) and ButtonGadgetClass over unknown_66b770.h's gadget with modes (table 0xAFB934, 0x64 bytes): the
**	UI's buttons (ui_login.cpp's, ui_manu.cpp's, ui_scut.cpp's, ui_avsel.cpp's, ui_group.cpp's 0x675240 /
**	0x675170 / 0x675310). Both over a model, or over a UIMeshClass's sub-object by name.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef UNKNOWN_675120_H  // NAME 1 [unknown_675120.h]: neutral (D137)
#define UNKNOWN_675120_H

#include "gd_robj.h"
#include "unknown_66b770.h"

class UIMeshClass;

// The model gadget without a scene (0x60 bytes; its slots +0x74 / +0x78 the inline 0x51ADC0 / 0x51ADE0)
class Unknown_675120Class : public RObjGadgetClass  // NAME 1: neutral (D10)
{
public:
	Unknown_675120Class(UIMeshClass *mesh, const char *name, const LayerClass &layer, unsigned int mask);	// 0x674F90 (the mesh's sub-object 0x5B8B40)  // NAME 1 [mesh, name, layer, mask]: the base's and what it does
	Unknown_675120Class(RenderObjClass *robj, const LayerClass &layer, unsigned int mask);	// 0x675050 (RObjGadgetClass 0x66BDF0)  // NAME 1 [robj, layer, mask]: the base's
	Unknown_675120Class(UIMeshClass *mesh, const char *name, unsigned int mask);	// 0x675090  // NAME 1 [mesh, name, mask]: the base's and what it does
	Unknown_675120Class(RenderObjClass *robj, unsigned int mask);		// 0x675120 (RObjGadgetClass 0x66BEC0)  // NAME 1 [robj, mask]: the base's
	virtual ~Unknown_675120Class();										// +0x00 0x675150 (deleting 0x675020)
	virtual void Unknown_74() {}										// +0x74 0x51ADC0 (no scene to enter)
	virtual void Unknown_78() { Unknown_6607D0(); }						// +0x78 0x51ADE0 (only the flash stopped)
};

// The button gadget: the gadget with modes without a scene (0x64 bytes; its slots +0x74 / +0x78 the inline 0x58E5B0 / 0x58E5D0)
class ButtonGadgetClass : public Unknown_66B770Class  // NAME 1 [ButtonGadgetClass]: neutral (D10): what the ui_* files make of it
{
public:
	ButtonGadgetClass(UIMeshClass *mesh, const char *name, const LayerClass &layer, unsigned int mask, unsigned int flags);	// 0x675170 (the mesh's sub-object 0x5B8B40)  // NAME 1 [mesh, name, layer, mask, flags]: the base's and what it does
	ButtonGadgetClass(RenderObjClass *robj, const LayerClass &layer, unsigned int mask, unsigned int flags);	// 0x675240 (the screens pass 0x55, 3)  // NAME 1 [robj, layer, mask, flags]: the base's
	ButtonGadgetClass(UIMeshClass *mesh, const char *name, unsigned int mask, unsigned int flags);	// 0x675280  // NAME 1 [mesh, name, mask, flags]: the base's and what it does
	ButtonGadgetClass(RenderObjClass *robj, unsigned int mask, unsigned int flags);	// 0x675310 (ui_group.cpp's icons)  // NAME 1 [robj, mask, flags]: the base's
	virtual ~ButtonGadgetClass();										// +0x00 0x675350 (deleting 0x675210)
	virtual void Unknown_74() {}										// +0x74 0x58E5B0 (no scene to enter)
	virtual void Unknown_78() { Unknown_6607D0(); }						// +0x78 0x58E5D0 (only the flash stopped)
};

#endif
