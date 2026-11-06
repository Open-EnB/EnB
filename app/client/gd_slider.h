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
#include "vector2.h"
#include "vector3.h"
#include "aabox.h"

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
	void Unknown_66FA90(float total, float page, bool a);	// 0x66FA90 (ui_class.cpp's 0x576360: the skills' count, 8, 1)  // NAME 1 [total, page, a]: neutral
	bool Unknown_66FD80(bool force = false);			// 0x66FD80 (the bars laid out from the two bones; FAIL_IF '!SliderObject')
	void Unknown_670230(const AABoxClass &box, bool force);	// 0x670230 (the wheel box set, then 0x66FD80)  // NAME 1 [box, force]: neutral
	bool Unknown_6702A0();								// 0x6702A0 (the flash on the up arrow)
	bool Unknown_6702D0();								// 0x6702D0 (the flash on the down arrow)
	void Unknown_670300();								// 0x670300 (the thumb animated and placed; FAIL_IF '!Thumb')
	float Unknown_6705A0();								// 0x6705A0 (inline: the thumb animation's frame, never below 0)
	void Unknown_670440();								// 0x670440 (the value from the dragged thumb)

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
	Vector2 UnknownA4;									// +0xA4 (the bar's low corner: 0x66FD80 stores it as one Vector2)
	Vector2 UnknownAC;									// +0xAC (the high corner)
	float UnknownB4;									// +0xB4
	float UnknownB8;									// +0xB8
	float UnknownBC;									// +0xBC
	float UnknownC0;									// +0xC0
	float UnknownC4;									// +0xC4
	float UnknownC8;									// +0xC8
	float UnknownCC;									// +0xCC
	float UnknownD0;									// +0xD0
	float Value;										// +0xD4
	float Get_Value() const { return(Value); }			// inline: ui_starbase.cpp 0x646F70 / 0x6474C0 copy it through the FPU (fld / fstp), not a plain mov  // NAME 1 [Get_Value]: neutral
	int UnknownD8;										// +0xD8 (floor of Value: 0x66FC70 compares it with ftol's int)
	float UnknownDC;									// +0xDC
	int UnknownE0;										// +0xE0 (the part under the pointer, 1..6: 0x66EF00)
	int UnknownE4;										// +0xE4 (the part pressed: 0x66F330)
	int UnknownE8;										// +0xE8 (the flashed part: 0x6702A0 / 0x6702D0)
	float UnknownEC;									// +0xEC
	float UnknownF0;									// +0xF0
	float UnknownF4;									// +0xF4
	float UnknownF8;									// +0xF8
	unsigned int UnknownFC;								// +0xFC (the next repeat's ticks: 0x66F330)
	unsigned int Unknown100;							// +0x100 (the ticks at the last move: 0x66EF00)
	RefCountPtr<RenderObjClass> PressedObject;			// +0x104  // NAME 2 [PressedObject]: FAIL_IF 'PressedObject'
	bool Unknown108;									// +0x108
	bool Unknown109;									// +0x109
	AABoxClass Unknown10C;								// +0x10C (the wheel box while +0x109: 0x670230, 0x66EF00)
	bool Unknown124;									// +0x124
};

#endif
