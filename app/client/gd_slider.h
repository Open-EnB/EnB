// Earth & Beyond client (reconstruction)
/*
**	Original location: e:\build\beta\G\app\Client\gd_slider.h (name assumed from gd_slider.cpp, whose
**	__FILE__ is 'e:\build\beta\G\app\Client\gd_slider.cpp'; the client holds no string of this header)
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef GD_SLIDER_H
#define GD_SLIDER_H

#include "always.h"
#include "gd_robj.h"
#include "vector3.h"

struct KernelEventClass;
class HAnimClass;

class GadgetSlider : public RObjGadgetClass	// NAME 5: the client's RTTI .?AVGadgetSlider@@
{
public:
	GadgetSlider(bool arg, LayerClass *layer = NULL);	// 0x66DFC0
	virtual ~GadgetSlider();							// +0x00 0x66E1D0 (deleting 0x66E1A0)
	virtual RenderObjClass *Unknown_24(Vector3 *position, float *frame, bool indicator);	// +0x24 0x66F8E0
	virtual bool Unknown_2C(KernelEventClass *event, float x, float y, unsigned int mask);	// +0x2C 0x66F330
	virtual bool Unknown_30(float x, float y);			// +0x30 0x66EF00
	virtual void Unknown_40(bool on);					// +0x40 0x66EDE0
	virtual void Unknown_74();							// +0x74 0x66F850
	virtual void Unknown_78();							// +0x78 0x66F8A0
	virtual void Unknown_8C(bool on);					// +0x8C 0x66EE20
	virtual void Unknown_A4(float val, bool arg2);		// +0xA4 0x66FC70

	void Unknown_66E3A0();								// 0x66E3A0
	void Unknown_66E460(const RefCountPtr<RenderObjClass> &object, bool is_horizontal, int bone_idx);	// 0x66E460
	bool Unknown_66FD80(bool force = false);			// 0x66FD80

	RefCountPtr<RenderObjClass> SliderObject;			// +0x60  // NAME 2 [SliderObject]: FAIL_IF '!SliderObject'
	RefCountPtr<RenderObjClass> MinArrow;				// +0x64  // NAME 2 [MinArrow]: FAIL_IF '!MinArrow'
	RefCountPtr<RenderObjClass> MaxArrow;				// +0x68  // NAME 2 [MaxArrow]: FAIL_IF '!MaxArrow'
	RefCountPtr<RenderObjClass> Thumb;					// +0x6C  // NAME 2 [Thumb]: FAIL_IF '!Thumb'
	RefCountPtr<RenderObjClass> WheelRegion;			// +0x70  // NAME 2 [WheelRegion]: FAIL_IF '!WheelRegion'
	RefCountPtr<HAnimClass> ThumbAnim;					// +0x74  // NAME 2 [ThumbAnim]: FAIL_IF '!ThumbAnim'
	int MinBoneIdx;										// +0x78  // NAME 2 [MinBoneIdx]: G_ASSERT 'MinBoneIdx'
	int MaxBoneIdx;										// +0x7C  // NAME 2 [MaxBoneIdx]: G_ASSERT 'MaxBoneIdx'
	float Unknown80;									// +0x80
	bool IsHorizontal;									// +0x84
	Vector3 MinBonePos;									// +0x88
	Vector3 MaxBonePos;									// +0x94
	float UnknownA0;									// +0xA0
	float UnknownA4;									// +0xA4
	float UnknownA8;									// +0xA8
	float UnknownAC;									// +0xAC
	float UnknownB0;									// +0xB0
	float UnknownB4;									// +0xB4
	float UnknownB8;									// +0xB8
	float UnknownBC;									// +0xBC
	float UnknownC0;									// +0xC0
	float UnknownC4;									// +0xC4
	float UnknownC8;									// +0xC8
	float UnknownCC;									// +0xCC
	float UnknownD0;									// +0xD0
	float Value;										// +0xD4
	float UnknownD8;									// +0xD8
	float UnknownDC;									// +0xDC
	float UnknownE0;									// +0xE0
	float UnknownE4;									// +0xE4
	float UnknownE8;									// +0xE8
	float UnknownEC;									// +0xEC
	float UnknownF0;									// +0xF0
	float UnknownF4;									// +0xF4
	float UnknownF8;									// +0xF8
	float UnknownFC;									// +0xFC
	float Unknown100;									// +0x100
	RefCountPtr<RenderObjClass> Unknown104;				// +0x104
	bool Unknown108;									// +0x108
	bool Unknown109;									// +0x109
	int Unknown10C;										// +0x10C
	int Unknown110;										// +0x110
	int Unknown114;										// +0x114
	int Unknown118;										// +0x118
	int Unknown11C;										// +0x11C
	int Unknown120;										// +0x120
	bool Unknown124;									// +0x124
};

#endif
