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

// (byte-exact since +0xD8 / +0xE0..+0xE8 / +0xFC / +0x100 became ints and +0x10C an AABoxClass)
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
	UnknownA4(0.0f,
		0.0f),
	UnknownAC(0.0f,
		0.0f),
	UnknownB4(0.0f),
	UnknownB8(0.0f),
	UnknownBC(0.0f),
	UnknownC0(0.0f),
	UnknownC4(0.0f),
	UnknownC8(0.0f),
	UnknownCC(0.0f),
	UnknownD0(0.0f),
	Value(0.0f),
	UnknownD8(0),
	UnknownDC(0.0f),
	UnknownE0(0),
	UnknownE4(0),
	UnknownE8(0),
	UnknownEC(0.0f),
	UnknownF0(0.0f),
	UnknownF4(0.0f),
	UnknownF8(0.0f),
	UnknownFC(0),
	Unknown100(0),
	PressedObject(NULL),
	Unknown108(false),
	Unknown109(false),
	Unknown10C(
		Vector3(0.0f,
			0.0f, 0.0f),
		Vector3(0.0f,
			0.0f,
			0.0f)),
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

#include <math.h>
#include "..\kernel\kernel.h"
#include "..\audio\soundlib.h"
#include "unknown_signalbase.h"
#include "scene.h"
#include "vector2.h"

class GadgetSliderEvent  // NAME 5: the client's RTTI .?AVGadgetSliderEvent@@
{
public:
	GadgetSliderEvent(int value) : Value(value) {}						// NAME 1 [value]: neutral

	int Value;															// +0x00 (the slider's +0xD8)  // NAME 1: neutral (D10)
};

bool Unknown_496110(RenderObjClass *robj, float a, float b, float c);	// 0x496110 (ro_funcs.cpp, cdecl): a model's highlight  // NAME 1 [robj, a, b, c]: as ui_group.h
unsigned int Unknown_Get_Ticks();										// 0x4A3B40, the aux-data clock
extern KernelEventClass Unknown_BE5EB0;								// 0xBE5EB0 (gadget.cpp's: the last input event)  // NAME 1: neutral (D10)

extern const int Unknown_B8D430;	// 0xB8D430 (500: the first repeat's delay, ms)  // NAME 1: neutral
extern const int Unknown_B8D434;	// 0xB8D434 (100: the repeat delay, ms)  // NAME 1: neutral
extern const int Unknown_B8D438;	// 0xB8D438 (16: the thumb's least length)  // NAME 1: neutral
extern const int Unknown_B8D43C;	// 0xB8D43C (2)  // NAME 1: neutral
extern const int Unknown_B8D444;	// 0xB8D444 (1000: the hold before a drag follows another gadget)  // NAME 1: neutral
extern const int Unknown_B8D448;	// 0xB8D448 (32: the drag's margin round the bar)  // NAME 1: neutral

// 0x6705A0: the thumb animation's frame (its length past the least), emitted out of line after 0x670440
inline float GadgetSlider::Unknown_6705A0()
{
	float frame = UnknownCC - Unknown_B8D438;
	return((frame > 0.0f) ? frame : 0.0f);
}

// @0x66EDE0
void GadgetSlider::Unknown_40(bool on)
{
	GadgetClass::Unknown_40(on);
	Unknown_8C(!on);
}

// @0x66EE20
void GadgetSlider::Unknown_8C(bool on)
{
	if (UnknownC0 == UnknownC8) {
		on = true;
	}
	if (UnknownBC < (float)(Unknown_B8D438 - Unknown_B8D43C)) {
		RObjGadgetClass::Unknown_8C(true);
	} else {
		RObjGadgetClass::Unknown_8C(on);
	}
	if (UnknownBC <= 0.0f) {
		on = true;
	}
	if (MaxArrow != NULL) {
		MaxArrow->Set_Hidden(on);
	}
	if (MinArrow != NULL) {
		MinArrow->Set_Hidden(on);
	}
	Unknown108 = !on;
}

// NOT EXACT: shape (similarity 0.81): -480 * y folds to a subtraction; the client's early returns keep their own epilogues
// @0x66EF00
bool GadgetSlider::Unknown_30(float x, float y)
{
	if (Thumb == NULL) {
		return(false);
	}
	unsigned int last = Unknown100;
	UnknownE0 = 0;
	UnknownEC = x * 640.0f - 320.0f;
	UnknownF0 = -480.0f * y + 240.0f;
	unsigned int now = Unknown_Get_Ticks();
	Unknown100 = now;
	if (Unknown_BE5EB0.Key != 0x101 && Unknown_BE5EB0.Key != 0x102) {
		if (!Unknown108) {
			return(false);
		}
		if (UnknownEC < UnknownA4.X) {
			return(false);
		}
		if (UnknownEC > UnknownAC.X) {
			return(false);
		}
		if (UnknownF0 < UnknownA4.Y) {
			return(false);
		}
		if (UnknownF0 > UnknownAC.Y) {
			return(false);
		}
		Unknown100 = last;
		if (IsHorizontal) {
			if (UnknownEC < UnknownB4) {
				UnknownE0 = 1;
			} else if (UnknownEC < UnknownDC) {
				UnknownE0 = 2;
			} else if (UnknownEC < UnknownCC + UnknownDC && !Thumb->Is_Hidden()) {
				UnknownE0 = 3;
			} else if (UnknownEC < UnknownB8) {
				UnknownE0 = 4;
			} else {
				UnknownE0 = 5;
			}
		} else {
			if (UnknownF0 > UnknownB4) {
				UnknownE0 = 1;
			} else if (UnknownF0 > UnknownDC) {
				UnknownE0 = 2;
			} else if (UnknownF0 > UnknownDC - UnknownCC && !Thumb->Is_Hidden()) {
				UnknownE0 = 3;
			} else if (UnknownF0 > UnknownB8) {
				UnknownE0 = 4;
			} else {
				UnknownE0 = 5;
			}
		}
		if (Unknown_BE5F24 == this) {
			return(true);
		}
		if (Unknown_BE5F24 == NULL) {
			return(true);
		}
		if (!Unknown_BE5F24->Unknown_10()) {
			return(true);
		}
		if (Unknown_B8D444 + Unknown100 >= now) {
			return(true);
		}
		switch (UnknownE0) {
			case 1:
			case 5:
				return(true);
		}
		Unknown_670440();
		return(true);
	}

	if (WheelRegion == NULL && !Unknown109) {
		return(false);
	}
	const AABoxClass &box = Unknown109 ? Unknown10C : WheelRegion->Get_Bounding_Box();
	if (UnknownEC < box.Center.X - box.Extent.X) {
		return(false);
	}
	if (box.Extent.X + box.Center.X < UnknownEC) {
		return(false);
	}
	if (UnknownF0 < box.Center.Y - box.Extent.Y) {
		return(false);
	}
	if (UnknownF0 > box.Center.Y + box.Extent.Y) {
		return(false);
	}
	UnknownE0 = 6;
	return(true);
}





















































// NOT EXACT: shape (similarity 0.56): the client places the repeat tail before the held / wheel branches
// @0x66F330
bool GadgetSlider::Unknown_2C(KernelEventClass *event, float x, float y, unsigned int mask)
{
	if (!Unknown_60()) {
		return(false);
	}
	unsigned int now = Unknown_Get_Ticks();
	int delay = Unknown_B8D434;
	if (mask & 0x11) {
		GadgetClass::Unknown_2C(event, x, y, mask);
		Unknown_68();
		delay = Unknown_B8D430;
		UnknownE4 = UnknownE0;
		UnknownFC = now;
		UnknownF4 = Value;
		if (IsHorizontal) {
			UnknownF8 = UnknownEC - Thumb->Get_Position().X;
		} else {
			UnknownF8 = UnknownF0 - Thumb->Get_Position().Y;
		}
		switch (UnknownE4) {
			default:
				FAIL_IF(PressedObject) {
					PressedObject = NULL;
				}
				break;
			case 5:
				PressedObject = MaxArrow;
				break;
			case 1:
				PressedObject = MinArrow;
				break;
		}
		if (PressedObject != NULL) {
			Vector3 color = Unknown_9C();
			Unknown_496110(PressedObject.Peek(), color.X, color.Y, color.Z);
		}
	} else if (Unknown_BE5F24 == this || Unknown_BE5F28 == this) {
		if (mask & 0x44) {
			GadgetClass::Unknown_2C(event, x, y, mask);
			Unknown_6C();
			if (PressedObject != NULL) {
				Vector3 color = Unknown_A0();
				Unknown_496110(PressedObject.Peek(), color.X, color.Y, color.Z);
				PressedObject = NULL;
			}
			return(true);
		}
		if (mask & 0x22) {
			Unknown_30(x, y);
			GadgetClass::Unknown_2C(event, x, y, mask);
		}
	} else if (mask & 0x1000) {
		if (event->Key == 0x101) {
			Unknown_A4(Value - 1.0f, false);
		} else if (event->Key == 0x102) {
			Unknown_A4(Value + 1.0f, false);
		} else {







			G_ASSERT(false);
			return(false);
		}
		GadgetClass::Unknown_2C(event, x, y, mask);
		return(true);
	}

	if (UnknownE4 == 3) {
		Unknown_670440();
		return(true);
	}
	if (UnknownE4 == UnknownE0 && UnknownFC <= now) {
		UnknownFC += delay;
		switch (UnknownE0) {
			case 1:
				Unknown_A4(Value - 1.0f, false);
				break;
			case 2:
				Unknown_A4(Value - UnknownC8, false);
				break;
			case 4:
				Unknown_A4(Value + UnknownC8, false);
				break;
			case 5:
				Unknown_A4(Value + 1.0f, false);
				break;
		}
	}
	return(true);
}

// @0x66F850
void GadgetSlider::Unknown_74()
{
	if (MinArrow != NULL) {
		MinArrow->Add(Layer.Peek_Scene());
	}
	if (MaxArrow != NULL) {
		MaxArrow->Add(Layer.Peek_Scene());
	}
	RObjGadgetClass::Unknown_74();
}

// @0x66F8A0
void GadgetSlider::Unknown_78()
{
	RObjGadgetClass::Unknown_78();
	if (MinArrow != NULL) {
		MinArrow->Remove();
	}
	if (MaxArrow != NULL) {
		MaxArrow->Remove();
	}
}

// @0x66F8E0
RenderObjClass *GadgetSlider::Unknown_24(Vector3 *position, float *frame, bool indicator)
{
	RefCountPtr<RenderObjClass> object;
	int part = UnknownE0;
	if (UnknownE8 == part) {
		UnknownE8 = 0;
	}
	for (int i = 0; i < 2; i++) {
		switch (part) {
			case 5:
				if (!MaxArrow->Is_Hidden()) {
					object = MaxArrow;
				}
				break;
			case 3:
				if (!Thumb->Is_Hidden()) {
					object = Thumb;
					*frame = Unknown_6705A0();
				}
				break;
			case 1:
				if (!MinArrow->Is_Hidden()) {
					object = MinArrow;
				}
				break;
		}
		if (object != NULL) {
			*position = object->Get_Position();
			goto done;
		}
		part = UnknownE8;
	}
	*position = Vector3(0.0f, 0.0f, 0.0f);
done:
	return(object.Peek());
}

// (always.h's MAX reloads +0xBC where WWMath::Max's by-value copy spilled it)
// @0x66FA90
void GadgetSlider::Unknown_66FA90(float total, float page, bool a)
{
	if (a || total != UnknownC0 || page != UnknownC8) {
		if (total < 0.0001f) {
			UnknownC0 = 0.0f;
			UnknownC8 = 0.0f;
			float least = Unknown_B8D438;
			UnknownCC = MAX(UnknownBC, least);
		} else if (page < 0.0001f) {
			UnknownC8 = 0.0f;
			UnknownC0 = total;
			UnknownCC = Unknown_B8D438;
		} else {
			UnknownC0 = total;
			UnknownC8 = page;
			if (Unknown124) {
				UnknownC4 = total + 1.0f;
			} else if (total >= page) {
				UnknownC4 = total - page;
			} else {
				UnknownC4 = 0.0f;
				UnknownC0 = page;
			}
			float thumb = page / UnknownC0 * UnknownBC;
			float least = Unknown_B8D438;
			if (thumb <= least) {
				thumb = least;
			}
			UnknownCC = thumb;
		}
		UnknownD0 = WWMath::Max(0.0f, UnknownBC - UnknownCC);
		Unknown_8C(false);
		Unknown_A4(Value, true);
	}
}

// @0x66FC70
void GadgetSlider::Unknown_A4(float val, bool arg2)
{
	if (val < 0.0f) {
		val = 0.0f;
	} else if (val > UnknownC4) {
		val = UnknownC4;
	}
	if (arg2 || Value != val) {
		int index = (int)floor(val);
		if (UnknownD8 != index) {
			UnknownD8 = index;
			SoundLibraryClass::Play_2D_Sound("slider_bar_1.wav", 2);
			Get_Signal_Map().emit(this, GadgetSliderEvent(UnknownD8));
		}
		Value = val;
		Unknown_670300();
	}
}




































































































// NOT EXACT: frame (similarity 0.58): the client keeps the low bone's X on the FPU stack, ours spills it (0x1C frame vs 0x18)
// @0x66FD80
bool GadgetSlider::Unknown_66FD80(bool force)
{
	FAIL_IF(!SliderObject) return(false);

	const Matrix3D &min_tm = SliderObject->Get_Bone_Transform(MinBoneIdx);
	const Matrix3D &max_tm = SliderObject->Get_Bone_Transform(MaxBoneIdx);
	Vector3 min_pos = min_tm.Get_Translation();
	Vector3 max_pos = max_tm.Get_Translation();
	if (!force && min_pos == MinBonePos && max_pos == MaxBonePos) {
		return(false);
	}
	MinBonePos = min_pos;
	MaxBonePos = max_pos;
	if (IsHorizontal) {
		UnknownA0 = min_pos.Y;
		UnknownA4 = Vector2(WWMath::Min(min_pos.X, max_pos.X), min_pos.Y - Unknown80 * 0.5f);
		UnknownAC = Vector2(WWMath::Max(min_pos.X, max_pos.X), Unknown80 * 0.5f + UnknownA0);
		UnknownB4 = UnknownA4.X + Unknown80;
		UnknownB8 = UnknownAC.X - Unknown80;
		UnknownBC = UnknownB8 - UnknownB4;
		if (UnknownBC < 1.0f) {
			float mid = UnknownB4 - UnknownBC * 0.5f;
			UnknownBC = 0.0f;
			UnknownB4 = mid;
			UnknownB8 = mid;
		}
		MinArrow->Set_Position(Vector3(UnknownB4 - Unknown80, UnknownA0, 0.0f));
		MaxArrow->Set_Position(Vector3(UnknownB8 + Unknown80, UnknownA0, 0.0f));
	} else {
		UnknownA0 = min_pos.X;
		UnknownA4 = Vector2(min_pos.X - Unknown80 * 0.5f, WWMath::Min(min_pos.Y, max_pos.Y));
		UnknownAC = Vector2(Unknown80 * 0.5f + UnknownA0, WWMath::Max(min_pos.Y, max_pos.Y));
		UnknownB4 = UnknownAC.Y - Unknown80;
		UnknownB8 = UnknownA4.Y + Unknown80;
		UnknownBC = UnknownB4 - UnknownB8;
		if (UnknownBC < 1.0f) {
			float mid = UnknownBC * 0.5f + UnknownB4;
			UnknownBC = 0.0f;
			UnknownB4 = mid;
			UnknownB8 = mid;
		}
		MinArrow->Set_Position(Vector3(UnknownA0, UnknownB4 + Unknown80, 0.0f));
		MaxArrow->Set_Position(Vector3(UnknownA0, UnknownB8 - Unknown80, 0.0f));
	}
	Unknown_8C(false);
	Unknown_66FA90(UnknownC0, UnknownC8, true);
	return(true);
}

// @0x670230
void GadgetSlider::Unknown_670230(const AABoxClass &box, bool force)
{
	Unknown109 = true;
	Unknown10C = box;
	Unknown_66FD80(force);
}

// @0x6702A0
bool GadgetSlider::Unknown_6702A0()
{
	UnknownE8 = 1;
	Unknown_6603A0(false, NULL);
	return(true);
}

// @0x6702D0
bool GadgetSlider::Unknown_6702D0()
{
	UnknownE8 = 5;
	Unknown_6603A0(false, NULL);
	return(true);
}
















































// @0x670300
void GadgetSlider::Unknown_670300()
{
	FAIL_IF(!Thumb) return;

	Thumb->Set_Animation(ThumbAnim.Peek(), Unknown_6705A0(), 0);
	float offset;
	if (UnknownC4 > 0.0f) {
		offset = Value / UnknownC4 * UnknownD0;
	} else {
		offset = 0.0f;
	}
	if (IsHorizontal) {
		UnknownDC = offset + UnknownB4;
		Thumb->Set_Position(Vector3(UnknownDC, UnknownA0, 0.0f));
	} else {
		UnknownDC = UnknownB4 - offset;
		Thumb->Set_Position(Vector3(UnknownA0, UnknownDC, 0.0f));
	}
}

// @0x670440
void GadgetSlider::Unknown_670440()
{
	if (UnknownD0 > 0.0001f) {
		if (UnknownE0 == 0 && (UnknownEC < UnknownA4.X - Unknown_B8D448 || Unknown_B8D448 + UnknownAC.X < UnknownEC
			|| UnknownF0 < UnknownA4.Y - Unknown_B8D448 || Unknown_B8D448 + UnknownAC.Y < UnknownF0)) {
			Unknown_A4(UnknownF4, false);
			return;
		}
		float pos;
		if (IsHorizontal) {
			pos = (UnknownEC - UnknownF8) - UnknownB4;
		} else {
			pos = UnknownB4 - (UnknownF0 - UnknownF8);
		}
		if (pos < 0.0001f) {
			Unknown_A4(0.0f, false);
			return;
		}
		Unknown_A4(pos / UnknownD0 * UnknownC4, false);
		return;
	}
	Unknown_A4(0.0f, false);
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
**	0x66DFC0 (constructor): byte-exact (CS14 2026-11-05: the members 0x66EF00 / 0x66F330 / 0x66FC70 read as ints typed
**	int, the wheel box +0x10C an AABoxClass).
**
**	CS14 (2026-11-05): 0x66EDE0-0x670440, the slots and members the POC linked as gen_stubs stand-ins (the class
**	screen's skill slider and every combo box's list: not drawn, no hover, no click). FAIL_IF 'PressedObject' line 436,
**	G_ASSERT 'false' line 479, FAIL_IF '!SliderObject' line 733, FAIL_IF '!Thumb' line 854. Byte-exact: 0x66EDE0 (+0x40),
**	0x66F850 (+0x74), 0x66F8A0 (+0x78), 0x66F8E0 (+0x24: a switch with the cases in the client's order 5, 3, 1),
**	0x66FC70 (+0xA4: GadgetSliderEvent carries the floor of the value), 0x670230, 0x6702A0, 0x6702D0, 0x670300 (the
**	inline 0x6705A0 as a ?:), 0x670440. 0x6705A0 (inline, emitted out of line by the client with no callers) and
**	0x6705E0 (`*this = 0`, no callers) are not written. The constants 0xB8D430-0xB8D448 are ints in .data that the
**	code reads (fild), not folded: defined here, after their uses. gadget.cpp's last input event 0xBE5EB0 is no longer
**	static (0x66EF00 reads its key: 0x101 / 0x102 are the wheel's).
**	0x66EE20 (+0x8C): byte-exact with the base call in an if / else (the client cross-jumps the two calls).
**	+0xA4 and +0xAC are Vector2s (0x66FD80 stores each through a Vector2 temporary: 0.51 -> 0.58; the constructor stays exact).
**	NOT EXACT (D64): 0x66EF00 (0.81), 0x66F330 (0.56), 0x66FA90 (0.77), 0x66FD80 (0.58).
*/

const int Unknown_B8D430 = 500;
const int Unknown_B8D434 = 100;
const int Unknown_B8D438 = 16;
const int Unknown_B8D43C = 2;
const int Unknown_B8D440 = 2;
const int Unknown_B8D444 = 1000;
const int Unknown_B8D448 = 32;
