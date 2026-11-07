// Earth & Beyond client (reconstruction)
#include <math.h>
#include <string>
#include "gd_uiback.h"
#include "unknown_signalbase.h"
#include "rendobj.h"
#include "aabox.h"
#include "wwmath.h"
#include "..\..\ww3d2\ww3d.h"
#include "..\shared\g_assert.h"

// @0x673EE0
Unknown_673EE0Class::Unknown_673EE0Class(RenderObjClass *robj, int unknown1, const LayerClass &layer, int unknown2, int unknown3) :
	Unknown_671300Class(robj, unknown1, layer, unknown2, unknown3, 3), Unknown80(0), Unknown84(0) {}
// @0x673F60
Unknown_673EE0Class::~Unknown_673EE0Class() {}
// @0x673F80
bool Unknown_673EE0Class::Unknown_2C(KernelEventClass *event, float x, float y, unsigned int mask)
{
	if (Unknown_60()) {
		if (mask & 0x11) {
			ButtonGadgetClass::Unknown_2C(event, x, y, mask);
			Unknown84 = 0;
			if (Unknown80 == 0 || Unknown80 == 2) Unknown64 = true;
			return(true);
		}
		if (mask & 0x22) {
			ButtonGadgetClass::Unknown_2C(event, x, y, mask);
			if (Unknown80 == 1) Unknown84 += WW3D::Get_Frame_Time() * 0.001f;
			return(true);
		}
		if (mask & 0x44) {
			ButtonGadgetClass::Unknown_2C(event, x, y, mask);
			Unknown84 = 0;
			if (Unknown80 == 0 || Unknown80 == 1) Unknown66 = true;
			return(true);
		}
		Unknown84 = 0;
	}
	return(false);
}
// @0x6740E0
bool Unknown_673EE0Class::Unknown_6740E0(float seconds)
{
	if (Unknown80 == 1 && Unknown84 > seconds) { Unknown84 = 0; return(true); }
	return(false);
}
// @0x674120
void Unknown_673EE0Class::Unknown_674120(bool on) { if (on) { Unknown80 = 1; } else { Unknown80 = 0; } }
// @0x674160
void Unknown_673EE0Class::Unknown_674160(bool on) { if (on) { Unknown80 = 2; } else { Unknown80 = 0; } }
// @0x6741A0
Unknown_674260Class::Unknown_674260Class(UIMeshClass *mesh, const char *name) : Unknown_675120Class(mesh, name, 0x10FF) { Unknown5C_03 = false; Unknown_65FFA0(1000); }
// @0x674260
Unknown_674260Class::Unknown_674260Class(const RefCountPtr<RenderObjClass> &model) : Unknown_675120Class(model.Peek(), 0x10FF) { Unknown5C_03 = false; Unknown_65FFA0(1000); }
// @0x6742F0
Unknown_674260Class::~Unknown_674260Class() {}
class Unknown_AFB77CClass : public Unknown_AFB794Class { public:	// NAME 1: neutral (D10): the target over a model (table 0xAFB77C); NAME 3 [render_object]: 0x6744E0's FAIL_IF; NAME 1 [position, Object]: neutral
	Unknown_AFB77CClass(const RefCountPtr<RenderObjClass> &render_object) : Object(render_object) {}
	virtual void Set_Position(const Vector3 &position) { Object->Set_Position(position); } virtual Vector3 Get_Position() { return(Object->Get_Position()); } virtual RenderObjClass *Peek_Object() { return(Object.Peek()); }
	RefCountPtr<RenderObjClass> Object; };
// @0x6744E0
static RefCountPtr<Unknown_AFB77CClass> Unknown_6744E0(const RefCountPtr<RenderObjClass> &render_object)	// NAME 1: neutral (D10)
{
	FAIL_IF (!render_object) {
		return(NULL);
	}
	return(Create_NEW(new Unknown_AFB77CClass(render_object)));
}

// @0x674310
MovableUIBackGadget::MovableUIBackGadget(const RefCountPtr<RenderObjClass> &model, const Vector3 &offset) :
	Unknown_674260Class(model),
	Unknown64(false),
	Unknown68(0, 0, 0),
	Unknown74(0, 0, 0),
	Unknown80(offset),
	Unknown8C(-320.0f, -240.0f),
	Unknown94(320.0f, 240.0f),
	Unknown9C(0, 0)
{
	Unknown60 = Unknown_6744E0(model);
	RenderObjClass *robj = Unknown60->Peek_Object();
	if (robj) {
		AABoxClass box = robj->Get_Bounding_Box();
		Unknown9C.X = box.Extent.X;
		Unknown9C.Y = box.Extent.Y;
	}
}

// @0x674740
MovableUIBackGadget::MovableUIBackGadget(const RefCountPtr<RenderObjClass> &model, const RefCountPtr<Unknown_AFB794Class> &target) :
	Unknown_674260Class(model),
	Unknown60(target),
	Unknown64(false),
	Unknown68(0, 0, 0),
	Unknown74(0, 0, 0),
	Unknown80(0, 0, 0),
	Unknown8C(-320.0f, -240.0f),
	Unknown94(320.0f, 240.0f),
	Unknown9C(0, 0)
{
}

// @0x674860
MovableUIBackGadget::~MovableUIBackGadget()
{
}

// @0x6748F0
bool MovableUIBackGadget::Unknown_2C(KernelEventClass *event, float x, float y, unsigned int mask)
{
	if (GadgetClass::Unknown_2C(event, x, y, mask)) {
		if (Unknown60 == NULL) {
			return(true);
		}
		if (mask & 1) {
			Unknown64 = false;
			Unknown68 = Vector3(x, y, 0);
			Unknown74 = Unknown60->Get_Position();
			Unknown_68();
			return(true);
		}
		if ((mask & 4) && Unknown_BE5F24 == this) {
			Unknown64 = false;
			Unknown_6C();
			return(true);
		}
		if ((mask & 2) && Unknown_BE5F24 == this) {
			Vector3 delta(x, y, 0);
			delta -= Unknown68;
			delta.X *= 640.0f;
			delta.Y *= -480.0f;
			Vector3 position = Unknown74;
			position += delta;
			position.X = (float)floor(position.X);
			position.Y = (float)floor(position.Y);
			position.X = WWMath::Clamp(position.X, Unknown8C.X + Unknown9C.X - Unknown80.X, Unknown94.X - Unknown9C.X - Unknown80.X);
			position.Y = WWMath::Clamp(position.Y, Unknown8C.Y + Unknown9C.Y - Unknown80.Y, Unknown94.Y - Unknown9C.Y - Unknown80.Y);
			if (position != Unknown60->Get_Position()) {
				Unknown64 = true;
				Unknown60->Set_Position(position);
				Get_Signal_Map().emit(this, MovableUIBackGadgetMoved());
				return(true);
			}
			Unknown64 = false;
		}
		return(true);
	}
	return(false);
}

// @0x674C10
UIBackGadgetClass::UIBackGadgetClass(const RefCountPtr<RenderObjClass> &model) :
	Unknown_674260Class(model),
	Unknown60(0),
	Unknown64(0)
{
	Unknown_674D70();
}
// @0x674D50
UIBackGadgetClass::~UIBackGadgetClass() {}































// @0x674D70
void UIBackGadgetClass::Unknown_674D70()
{
	RenderObjClass *robj = RObj;
	FAIL_IF (!robj) { return; }
	Unknown60 = robj->Get_Bone_Index("back_tl");
	Unknown64 = robj->Get_Bone_Index("back_br");
}

// @0x674DE0
bool UIBackGadgetClass::Unknown_30(float x, float y)
{
	RenderObjClass *robj = RObj;
	FAIL_IF (!robj) {
		return(false);
	}
	x = x * 640.0f - 320.0f;
	y = 240.0f - y * 480.0f;
	const Matrix3D &top_left = robj->Get_Bone_Transform(Unknown60);
	const Matrix3D &bottom_right = robj->Get_Bone_Transform(Unknown64);
	if (top_left[0][3] > x) {
		return(false);
	}
	if (bottom_right[0][3] < x) {
		return(false);
	}
	if (top_left[1][3] < y) {
		return(false);
	}
	if (bottom_right[1][3] > y) {
		return(false);
	}
	return(true);
}

/*
**	Reconstruction notes (gd_uiback.cpp, client 0x673EE0-0x674F80; __FILE__ 0xB8D734, FAIL_IFs on the client's lines 65, 198, 207,
**	D25: the compact top and the blank lines keep them there).
**
**	The object's bounds: <string>'s initialiser (workflow.md 3.3) ends the object before at 0x673EB0 and this one at 0x674F60, with
**	none between. Its first function is the thrust button 0x673EE0, which has no string or assert of its own: it is here
**	because of those bounds, and because .rdata agrees: every gadget object's .rdata runs [first table][the 0xAC-byte block of primes]
**	[its other tables], and here that is 0xAFB498 (the button), the block 0xAFB548, then 0xAFB5F4, 0xAFB6B8, 0xAFB77C, 0xAFB794,
**	0xAFB7AC with no second block. The next block belongs to unknown_675120.cpp's table 0xAFB870.
**
**	The client's 0x6744E0 has line 65, before everything else with a line, and sits behind its only caller 0x674310: a static's
**	place in .text says nothing of its place in the source.
**
**	The rest of the object: RefCountPtr<Unknown_AFB77CClass>'s Create_NEW 0x674F00 and destructor 0x674F30, and the initialiser.
*/
