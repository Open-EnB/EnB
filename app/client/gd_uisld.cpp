// Earth & Beyond client (reconstruction): app\Client\gd_uisld.cpp. Notes at the end of the file.
#include <stdlib.h>
#include <string>
#include "gd_uisld.h"
#include "rendobj.h"
#include "aabox.h"
#include "intersec.h"
#include "layer.h"
#include "..\audio\soundlib.h"
#include "..\shared\g_assert.h"

UISliderGadget::UISliderGadget(RenderObjClass *screen, const char *pull, const char *bar, const LayerClass *layer, bool horizontal, float extra) :
	Unknown_675120Class(screen->Get_Sub_Object_By_Name(bar), *layer, 0x77),
	Pull(NULL),
	Screen(screen),
	IsHorizontal(horizontal),
	Unknown0A9(false),
	Unknown0AC(0),
	Unknown0B0(0),
	Unknown0B4(0),
	Length(0),
	Unknown0BC(false),
	Unknown0C0(0, 0)
{
	Unknown_6755E0(pull, bar, horizontal, extra);
}























UISliderGadget::UISliderGadget(RenderObjClass *screen, const char *pull, const char *bar, bool horizontal, float extra) :
	Unknown_675120Class(screen->Get_Sub_Object_By_Name(bar), 0x77),
	Pull(NULL),
	Screen(screen),
	IsHorizontal(horizontal),
	Unknown0A9(false),
	Unknown0AC(0),
	Unknown0B0(0),
	Unknown0B4(0),
	Length(0),
	Unknown0BC(false),
	Unknown0C0(0, 0)
{
	Unknown_6755E0(pull, bar, horizontal, extra);
}






















// NOT EXACT: 0x6755E0 order: the client works out pull_box.Extent * 2 * extra before (bar - pull) * 2 (every form tried factors or swaps them)
void UISliderGadget::Unknown_6755E0(const char *pull, const char *bar, bool horizontal, float extra)
{
	RObj->Release_Ref();
	Screen->Add_Ref();
	Pull = Screen->Get_Sub_Object_By_Name(pull);
	RenderObjClass *screen = Screen;
	PullBone = screen->Get_Bone_Index(pull);
	PullTransform = screen->Get_Bone_Transform(PullBone);
	screen->Capture_Bone(PullBone);

	Vector3 bar_pos = RObj->Get_Position();
	AABoxClass bar_box = RObj->Get_Bounding_Box();
	Vector3 pull_pos = Pull->Get_Position();
	AABoxClass pull_box = Pull->Get_Bounding_Box();

	Unknown068.Set(0, 0, 0);
	if (horizontal) {
		Length = (pull_box.Extent.X + pull_box.Extent.X) * extra + (bar_box.Extent.X - pull_box.Extent.X) * 2.0f;
		Unknown068.X = (bar_pos.X - pull_pos.X) - Length * 0.5f;
	} else {
		Length = (pull_box.Extent.Y + pull_box.Extent.Y) * extra + (bar_box.Extent.Y - pull_box.Extent.Y) * 2.0f;
		Unknown068.Y = (bar_pos.Y - pull_pos.Y) - Length * 0.5f;
	}
	Unknown0B0 = (int)Length;
	Unknown_A4(Unknown0AC);
}























UISliderGadget::~UISliderGadget()
{
	Screen->Control_Bone(PullBone, PullTransform, false);
	Screen->Release_Bone(PullBone);
	Pull->Release_Ref();
	Screen->Release_Ref();
}























void UISliderGadget::Unknown_40(bool on)
{
	GadgetClass::Unknown_40(on);
}























bool UISliderGadget::Unknown_30(float x, float y)
{
	Unknown0C8.X = x * 640.0f - 320.0f;
	Unknown0C8.Y = (-480.0f * y) + 240.0f;
	if (!Pull->Is_Hidden()) {
		IntersectionClass intersection;
		IntersectionResultClass result;
		Unknown0BC = intersection.Intersect_Sub_Object(x, y, Layer, Pull, &result) != NULL;
		if (Unknown0BC) {
			Vector3 pos = Pull->Get_Position();
			Unknown0C0.X = Unknown0C8.X - pos.X;
			Unknown0C0.Y = Unknown0C8.Y - pos.Y;
			return(true);
		}
		Unknown0C0.X = 0;
		Unknown0C0.Y = 0;
	}
	return(RObjGadgetClass::Unknown_30(x, y));
}























bool UISliderGadget::Unknown_2C(KernelEventClass *event, float x, float y, unsigned int mask)
{
	if (!Unknown_60()) {
		return(false);
	}
	Unknown0C8.X = x * 640.0f - 320.0f;
	Unknown0C8.Y = (-480.0f * y) + 240.0f;
	if (mask & 0x11) {
		Unknown0A9 = true;
		if (!Unknown0BC) {
			Unknown_675F00(Unknown0C8);
		}
		GadgetClass::Unknown_2C(event, x, y, mask);
		Unknown_68();
		return(true);
	}
	if (Is_Held()) {
		if (mask & 0x22) {
			Unknown_675F00(Unknown0C8);
			GadgetClass::Unknown_2C(event, x, y, mask);
			return(true);
		}
		if (mask & 0x44) {
			GadgetClass::Unknown_2C(event, x, y, mask);
			Unknown_6C();
			Unknown0A9 = false;
			return(true);
		}
	}
	return(false);
}























RenderObjClass *UISliderGadget::Unknown_24(Vector3 *position, float *frame, bool indicator)
{
	RenderObjClass *retval = Unknown0BC ? Pull : RObj;
	FAIL_IF(!retval) return(NULL);
	*position = retval->Get_Position();
	return(retval);
}

void UISliderGadget::Unknown_8C(bool on)
{
	RObjGadgetClass::Unknown_8C(on);
	if (Pull != NULL) {
		Pull->Set_Hidden(on);
	}
	Unknown_A4(Unknown0B4);
}

void UISliderGadget::Unknown_675CF0(int count, int a, int b)
{
	Unknown0AC = count;
	Unknown0B0 = a;
	Unknown_A4(b);
}

void UISliderGadget::Unknown_A4(int top)
{
	if (Unknown0B4 != top) {
		SoundLibraryClass::Play_2D_Sound("SR_rollover14b.wav", 2);
	}
	Unknown0B4 = top;
	int range = abs(Unknown0B0 - Unknown0AC);
	float offset;
	if (range != 0) {
		if (Unknown0AC < Unknown0B0) {
			if (top < Unknown0AC) {
				Unknown0B4 = Unknown0AC;
			} else if (top > Unknown0B0) {
				Unknown0B4 = Unknown0B0;
			}
			offset = (float)(Unknown0B4 - Unknown0AC) / range * Length;
		} else {
			if (top > Unknown0AC) {
				Unknown0B4 = Unknown0AC;
			} else if (top < Unknown0B0) {
				Unknown0B4 = Unknown0B0;
			}
			offset = (float)(Unknown0B0 - Unknown0B4 + range) / range * Length;
		}
		Pull->Set_Hidden(Unknown5C_02);
	} else {
		Unknown0B4 = Unknown0AC;
		offset = 0;
		Pull->Set_Hidden(true);
	}
	if (!Pull->Is_Hidden()) {
		Matrix3D tm(Unknown068);
		if (IsHorizontal) {
			tm[0][3] += offset;
		} else {
			tm[1][3] += offset;
		}
		Screen->Control_Bone(PullBone, tm, false);
	}
}
// NOT EXACT: 0x675F00 order: the client loads Length * 0.5f before IsHorizontal
void UISliderGadget::Unknown_675F00(Vector2 point)
{
	Vector3 pos = RObj->Get_Position();
	float half = Length * 0.5f;
	float start;
	float value;
	if (IsHorizontal) {
		start = pos.X - half;
		value = point.X;
	} else {
		start = pos.Y - half;
		value = point.Y;
	}
	int range = abs(Unknown0B0 - Unknown0AC);
	int rows = (int)((value - start) / Length * range + 0.5);
	if (Unknown0AC < Unknown0B0) {
		Unknown_A4(Unknown0AC + rows);
	} else {
		Unknown_A4(Unknown0AC - rows);
	}
}

/*
**	Reconstruction notes (gd_uisld.cpp, client 0x675370-0x675FEF; __FILE__ 0xB8D78C, FAIL_IF '!retval' on the client's line 293,
**	D25: the blank lines between the definitions above it keep it there).
**
**	The object: from the scroll bar's first constructor 0x675370 (right after unknown_675120.cpp's last function 0x675350) to
**	<string>'s guarded ctype initialiser 0x675FD0, which ends it (workflow.md 3.3). Its 16 functions: the two constructors,
**	the table's three inline getters 0x675460 / 0x675480 / 0x6754A0 and the deleting destructor 0x6754C0, the constructors'
**	helper 0x6755E0, the destructor, the slots +0x40 / +0x30 / +0x2C / +0x24 / +0x8C / +0xA4 and the two members 0x675CF0 /
**	0x675F00. Table 0xAFBA80 over unknown_675120.h's 0xAFB870, with four new slots +0xA4..+0xB0.
**
**	The class name: none in the client (no RTTI, no string); D24's class-from-file for gd_uisld.cpp, spelled as the sibling
**	gd_uiback.cpp's UIBackGadget (RTTI .?AVMovableUIBackGadget@@'s base). 'uisld' read as 'UI slider' (gd_slider.cpp holds
**	the client's GadgetSlider, a different class).
**
**	Forms the bytes settled: the pointer at +0xC8 and the pull offset at +0xC0 are Vector2s (slot +0x2C passes +0xC8 whole to
**	0x675F00 from memory; the constructors' exception state is set where the Vector2 at +0xC0 is built); the y scale is
**	(-480.0f * y) + 240.0f as in gd_slider.cpp (y * -480.0f + 240.0f compiles to fsubr); 0x6755E0 holds the screen in a local
**	after Pull is set (edi across three virtual calls) and copies both bounding boxes (its 0x48-byte frame).
**
**	The screen's model: its +0x70 is Get_Sub_Object_By_Name, +0xB0 Get_Bone_Index, +0xB4 Get_Bone_Transform(int), +0xBC
**	Capture_Bone, +0xC0 Release_Bone, +0xC8 Control_Bone, +0xF4 Get_Bounding_Box, +0x180 Is_Hidden, +0x184 Set_Hidden.
*/
