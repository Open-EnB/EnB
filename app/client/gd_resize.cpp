// Earth & Beyond client (reconstruction): app\Client\gd_resize.cpp (__FILE__ 0xB8D340). Notes at the end of the file.
#include "gd_resize.h"
#include "rendobj.h"
#include "wwmath.h"
#include "..\shared\unknown_debug.h"

ResizeGadgetClass::ResizeGadgetClass(const RefCountPtr<RenderObjClass> &robj) :
	Unknown_675120Class(robj.Peek(), 0x77),
	Unknown60(0.0f, 0.0f),
	Unknown68(0.0f, 0.0f),
	Unknown70(0.0f, 0.0f),
	Unknown78(0.0f, 0.0f),
	Unknown80(0.0f, 0.0f),
	Unknown88(false)
{
}

ResizeGadgetClass::~ResizeGadgetClass()
{
}








































// NOT EXACT: shape (2 rows): the client loads the origin first for the x offset (fld +0x60, fsubr); tried pos - origin in place, a Vector2 difference, -origin + pos
bool ResizeGadgetClass::Unknown_2C(KernelEventClass *event, float x, float y, unsigned int mask)
{
	FAIL_IF(!Peek_Render_Obj()) {
		return(false);
	}
	if (!GadgetClass::Unknown_2C(event, x, y, mask)) {
		return(false);
	}
	Vector2 pos(x * 640.0f - 320.0f, 240.0f - y * 480.0f);
	if (mask & 0x10) {
		Unknown_68();
		return(true);
	}
	if ((mask & 0x40) && Unknown_BE5F28 == this) {
		Unknown_6C();
		return(true);
	}
	if (mask & 1) {
		Vector3 robjpos = RObj->Get_Position();
		Unknown80.Set(robjpos.X, robjpos.Y);
		Unknown80 -= pos;
		Unknown88 = true;
		Unknown_68();
	} else if ((mask & 4) && Unknown_BE5F24 == this) {
		Unknown88 = false;
		Unknown_6C();
	} else if (!(mask & 2) || Unknown_BE5F24 != this) {
		return(false);
	}
	Vector2 delta(-Unknown60.X + pos.X, Unknown60.Y - pos.Y);
	Unknown68.X = WWMath::Clamp(delta.X, Unknown70.X, Unknown78.X);
	Unknown68.Y = WWMath::Clamp(delta.Y, Unknown70.Y, Unknown78.Y);
	return(true);
}

void ResizeGadgetClass::Unknown_40(bool on)
{
	Unknown_8C(!on);
	GadgetClass::Unknown_40(on);
}

void ResizeGadgetClass::Unknown_66BDA0()
{
	if (RObj) {
		Vector3 robjpos = RObj->Get_Position();
		Unknown60 = Vector2(robjpos.X, robjpos.Y);
	}
}



/*
**	Reconstruction notes (GAD4, 2026-11-01). Client 0x66BA50-0x66BDD1 (table 0xAFA954; __FILE__ 0xB8D340, FAIL_IF
**	'!Peek_Render_Obj()' on the client's line 64 in 0x66BB10). gd_robj.cpp follows at 0x66BDF0. The file starts at
**	the constructor 0x66BA50: the model gadget with modes before it (0x66B770-0x66BA10, unknown_66b770.cpp) cannot
**	be the start of this file, since its five functions would not fit above line 64 (D137 placement there).
**
**	The drag (0x66BB10): the pointer to 640 x 480 coordinates around the centre; a press takes the model's offset
**	from it and holds the gadget (slot +0x68), a release lets go (+0x6C); every accepted event clamps the pointer's
**	offset from the origin +0x60 to the corners +0x70 / +0x78 into +0x68 (WWMath::Clamp inlined). Its caller is
**	0x5723B0. Unknown_675120Class's own file (0x675020-0x675160) is not rebuilt.
*/
