// Earth & Beyond client (reconstruction): app\Client\gd_slider.cpp. Notes at the end of the file.
#include <stdio.h>
#include "always.h"
#include "..\shared\unknown_debug.h"
#include "..\..\wwlib\ref_ptr.h"
#include "rendobj.h"
#include "aabox.h"
#include "assetmgr.h"
#include "hanim.h"
#include "gd_slider.h"

// NOT EXACT: order-only (similarity 0.89): member store order
// @0x66DFC0
GadgetSlider::GadgetSlider(bool arg, LayerClass *layer) :
	RObjGadgetClass(NULL, 0x1077),
	SliderObject(NULL),
	MinArrow(NULL),
	MaxArrow(NULL),
	Thumb(NULL),
	WheelRegion(NULL),
	ThumbAnim(NULL),
	Unknown80(0.0f),
	MinBoneIdx(-1),
	MaxBoneIdx(-1),
	IsHorizontal(false),
	MinBonePos(0.0f, 0.0f, 0.0f),
	MaxBonePos(0.0f, 0.0f, 0.0f),
	UnknownA0(0.0f),
	UnknownA4(0.0f),
	UnknownA8(0.0f),
	UnknownAC(0.0f),
	UnknownB0(0.0f),
	UnknownB4(0.0f),
	UnknownB8(0.0f),
	UnknownBC(0.0f),
	UnknownC0(0.0f),
	UnknownC4(0.0f),
	UnknownC8(0.0f),
	UnknownCC(0.0f),
	UnknownD0(0.0f),
	Value(0.0f),
	UnknownD8(0.0f),
	UnknownDC(0.0f),
	UnknownE0(0.0f),
	UnknownE4(0.0f),
	UnknownE8(0.0f),
	UnknownEC(0.0f),
	UnknownF0(0.0f),
	UnknownF4(0.0f),
	UnknownF8(0.0f),
	UnknownFC(0.0f),
	Unknown100(0.0f),
	Unknown104(NULL),
	Unknown108(false),
	Unknown109(false),
	Unknown10C(0),
	Unknown110(0),
	Unknown114(0),
	Unknown118(0),
	Unknown11C(0),
	Unknown120(0),
	Unknown124(arg)
{
	if (layer != NULL) {
		Unknown_28(layer);
	}
}

// @0x66E1D0
GadgetSlider::~GadgetSlider()
{
	Unknown_66E3A0();
}

// @0x66E3A0
void GadgetSlider::Unknown_66E3A0()
{
	Set_Render_Obj(NULL);
	SliderObject = NULL;
	MaxArrow = NULL;
	MinArrow = NULL;
	Thumb = NULL;
	WheelRegion = NULL;
}


bool Unknown_4967F0(RenderObjClass *robj, int delta, RenderObjClass *base_robj);
void Unknown_496AE0(RenderObjClass *robj, bool make_geometry_unique);
RefCountPtr<RenderObjClass> Unknown_497300(RenderObjClass *robj);

extern const int Unknown_B8D440;	// 0xB8D440 (2, .rdata: read by fisub, not folded: defined at the end of the file)  // NAME 1: neutral














































// @0x66E460
void GadgetSlider::Unknown_66E460(const RefCountPtr<RenderObjClass> &object, bool is_horizontal, int bone_idx)
{
	Unknown_66E3A0();
	if (object.Peek() == NULL) {
		return;
	}

	char buf[128];
	sprintf(buf, "slider_%02d_min", bone_idx);
	MinBoneIdx = object->Get_Bone_Index(buf);
	G_ASSERT(MinBoneIdx);

	sprintf(buf, "slider_%02d_max", bone_idx);
	MaxBoneIdx = object->Get_Bone_Index(buf);
	G_ASSERT(MaxBoneIdx);
	IsHorizontal = is_horizontal;
	if (IsHorizontal) {
		ThumbAnim = Create_Get(WW3DAssetManager::Get_Instance()->Get_HAnim("slider_hor.slider_hor-1"));
		Thumb = Create_NEW(WW3DAssetManager::Get_Instance()->Create_Render_Obj("slider_hor"));
		MinArrow = Create_NEW(WW3DAssetManager::Get_Instance()->Create_Render_Obj("slider_lt"));
		MaxArrow = Create_NEW(WW3DAssetManager::Get_Instance()->Create_Render_Obj("slider_rt"));
	} else {
		ThumbAnim = Create_Get(WW3DAssetManager::Get_Instance()->Get_HAnim("slider_vert.slider_vert-1"));
		Thumb = Create_NEW(WW3DAssetManager::Get_Instance()->Create_Render_Obj("slider_vert"));
		MinArrow = Create_NEW(WW3DAssetManager::Get_Instance()->Create_Render_Obj("slider_up"));
		MaxArrow = Create_NEW(WW3DAssetManager::Get_Instance()->Create_Render_Obj("slider_dn"));
	}
	sprintf(buf, "slider_%02d_regio", bone_idx);
	WheelRegion = Create_Get(object->Get_Sub_Object_By_Name(buf));
	if (WheelRegion == NULL) {
		WheelRegion = (object->Get_Container() != NULL) ? Unknown_497300(object->Get_Container()) : object;
	}





	FAIL_IF(!ThumbAnim) return;
	FAIL_IF(!Thumb) return;
	FAIL_IF(!MaxArrow) return;
	FAIL_IF(!MinArrow) return;
	FAIL_IF(!WheelRegion) return;

	Unknown_496AE0(Thumb.Peek(), true);
	Unknown_496AE0(MaxArrow.Peek(), true);
	Unknown_496AE0(MinArrow.Peek(), true);

	Unknown_4967F0(Thumb.Peek(), -1, object.Peek());
	Unknown_4967F0(MaxArrow.Peek(), 0, Thumb.Peek());
	Unknown_4967F0(MinArrow.Peek(), 0, Thumb.Peek());

	const AABoxClass &box = MinArrow->Get_Bounding_Box();
	if (IsHorizontal) {
		Unknown80 = box.Extent.X * 2.0f - Unknown_B8D440;
	} else {
		Unknown80 = box.Extent.Y * 2.0f - Unknown_B8D440;
	}

	SliderObject = object;
	Set_Render_Obj(Thumb.Peek());
	Unknown_66FD80(true);
}

/*
**	Reconstruction notes (SLD, 2026-11-03). Client 0x66DFC0-0x66EBE4 (table 0xAFAF78, 42 slots;
**	__FILE__ 'e:\build\beta\G\app\Client\gd_slider.cpp', G_ASSERT 'MinBoneIdx' line 149, 'MaxBoneIdx' line 153,
**	FAIL_IF '!ThumbAnim' line 176, '!Thumb' line 177, '!MaxArrow' line 178, '!MinArrow' line 179, '!WheelRegion' line 180).
**	0x66E1A0 (scalar deleting destructor): byte-exact.
**	0x66E1D0 (~GadgetSlider): byte-exact.
**	0x66E3A0 (Unknown_66E3A0): byte-exact.
**	0x66E460 (Unknown_66E460): byte-exact (NEX 2026-11-03: is_horizontal is a bool, ThumbAnim a RefCountPtr<HAnimClass>
**	through Get_HAnim (Create_Get 0x57B6B0), the render objects through Create_NEW (0x47D2E0), the wheel region a ?:
**	with a conditional temporary, FAIL_IF returns, and the arrow width subtracts the not-folded int 0xB8D440).
**	NOT EXACT (D64): constructor 0x66DFC0 (order-only, similarity 0.89).
*/

const int Unknown_B8D440 = 2;
