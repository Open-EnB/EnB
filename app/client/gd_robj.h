// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Client\gd_robj.h (name assumed from gd_robj.cpp, whose
**	__FILE__ is 'e:\build\beta\G\app\Client\gd_robj.cpp'; the client holds no string of this header)
**
**	gd_robj.cpp's gadget over a model: GadgetClass with the model (+0x30, G_ASSERT 'RObj'), a font style
**	shown with it (+0x34), its own layer (+0x38) and four flags (+0x5C). Table 0xAFAAA0 (41 slots),
**	constructors 0x66BDF0 (with a layer) / 0x66BEC0, destructor 0x66BF60, 0x60 bytes. gd_resize.cpp's
**	gadget derives from it ('!Peek_Render_Obj()' l.64); gadget.h's GadgetUnknown08Struct (slot +0x08's
**	answer, its model at +0x30) is this class seen from outside.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef GD_ROBJ_H  // NAME 3 [gd_robj.h]: header name assumed from the client's gd_robj.cpp
#define GD_ROBJ_H

#include "always.h"
#include "layer.h"
#include "matrix3d.h"
#include "gadget.h"

class RenderObjClass;
class FontStyleClass;

class RObjGadgetClass : public GadgetClass  // NAME 3 [RObjGadgetClass]: class-from-file (D24) for app\Client\gd_robj.cpp
{
public:
	RObjGadgetClass(RenderObjClass *robj, const LayerClass &layer, unsigned int mask);	// 0x66BDF0  // NAME 1 [robj, layer, mask]: what it does with them
	RObjGadgetClass(RenderObjClass *robj, unsigned int mask);			// 0x66BEC0 (an empty layer)  // NAME 1 [robj, mask]: what it does with them
	virtual ~RObjGadgetClass();											// +0x00 0x66BF60 (deleting 0x66BE90)
	virtual GadgetUnknown08Struct *Unknown_08() { return((GadgetUnknown08Struct *)this); }	// +0x08 0x51AD20 (itself)
	virtual RenderObjClass *Unknown_24(Vector3 *position, float *frame, bool indicator);	// +0x24 0x66C530 (the model and its position, while bit 3)
	virtual void Unknown_28(LayerClass *layer);							// +0x28 0x66C5B0 (the layer replaced, the model moved with it)
	virtual bool Unknown_30(float x, float y);							// +0x30 0x66C4C0 (the pointer on the model: ro_funcs.cpp's pick)
	virtual void Unknown_54(GadgetListManagerClass *manager, InterfaceClass *iface);	// +0x54 0x66C660 (slot +0x74, then the base's)
	virtual void Unknown_58(GadgetListManagerClass *manager);			// +0x58 0x66C690 (slot +0x78, then the base's)
	virtual void Unknown_64(AABoxClass &box);							// +0x64 0x66C7D0 (G_ASSERT 'RObj' l.250: the model's bounding box)
	virtual void Unknown_74();											// +0x74 0x66C5F0 (the model into the layer's scene)
	virtual void Unknown_78();											// +0x78 0x66C630 (the flash stopped, the model out of its scene)
	virtual void Unknown_7C(int arg) {}									// +0x7C 0x51AD40  // NAME 1 [arg]: neutral
	virtual void Unknown_80(int arg) {}									// +0x80 0x51AD60  // NAME 1 [arg]: neutral
	virtual void Unknown_84(int arg) {}									// +0x84 0x51AD80  // NAME 1 [arg]: neutral
	virtual bool Unknown_88() { return(false); }						// +0x88 0x51ADA0
	virtual void Unknown_8C(bool on);									// +0x8C 0x66C3F0 (the model and the style hidden)  // NAME 1 [on]: neutral
	virtual void Unknown_90(int arg);									// +0x90 0x66C470 (ro_funcs.cpp's 0x495FD0 on the model, index 0)  // NAME 1 [arg]: neutral
	virtual void Set_Render_Obj(RenderObjClass *robj);					// +0x94 0x66C000 (the flags carried over to the new model)  // NAME 2: as Peek_Render_Obj; NAME 1 [robj]: neutral
	virtual void Unknown_98(FontStyleClass *style);						// +0x98 0x66C1E0 (+0x34, referenced)  // NAME 1 [style]: its type
	virtual Vector3 Unknown_9C();										// +0x9C 0x66C860 (0.35 grey: the highlight's colour)
	virtual Vector3 Unknown_A0();										// +0xA0 0x66C890 (white: the colour without it)

	RenderObjClass *Peek_Render_Obj() { return(RObj); }					// (inline: gd_resize.cpp's FAIL_IF '!Peek_Render_Obj()')  // NAME 3: the FAIL_IF's
	void Unknown_66C220(float r, float g, float b);						// 0x66C220 (ro_funcs.cpp's 0x496110 on the model, unless bit 10)  // NAME 1 [r, g, b]: neutral
	void Unknown_66C260(const Vector3 &color);							// 0x66C260 (the same from a vector)  // NAME 1 [color]: neutral
	void Unknown_66C2A0(int arg);										// 0x66C2A0 (ro_funcs.cpp's 0x496240 on the model)  // NAME 1 [arg]: neutral
	void Unknown_66C2D0();												// 0x66C2D0 (the highlight on: slot +0x9C, the style's 0x6CE910)
	void Unknown_66C360();												// 0x66C360 (the highlight off: slot +0xA0)
	void Unknown_66C4A0();												// 0x66C4A0 (ro_funcs.cpp's 0x4960C0 on the model, index 0)
	const Matrix3D &Unknown_66C6C0();									// 0x66C6C0 (the model's transform, or a static identity)

	RenderObjClass *RObj;												// +0x30 (G_ASSERT 'RObj' l.250)  // NAME 3: the assert's
	FontStyleClass *Unknown34;											// +0x34 (slot +0x98; hidden and highlighted with the model)
	LayerClass Layer;													// +0x38 (0x24 bytes; slot +0x28 sets it)  // NAME 1: its type
	unsigned char Unknown5C_00 : 1;										// +0x5C bit 0: highlighted (0x66C2D0 / 0x66C360)
	unsigned char Unknown5C_01 : 1;										// +0x5C bit 1: in the layer's scene (slots +0x74 / +0x78)
	unsigned char Unknown5C_02 : 1;										// +0x5C bit 2: hidden (slot +0x8C)
	unsigned char Unknown5C_03 : 1;										// +0x5C bit 3 (the constructors set it; slot +0x24 answers only with it)
};

#endif
