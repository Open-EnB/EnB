// Earth & Beyond client (reconstruction): app\client\unknown_62a7c0.cpp, the shield gadget. No string names the
// file (no asserts of its own): the name is an interim one (D137). Notes at the end.
#include "unknown_62a7c0.h"
#include "co.h"
#include "client.h"
#include "rendobj.h"
#include "vector2.h"
#include "wwmath.h"
#include "..\shared\uvcontrl.h"
#include "..\shared\auxdata\auxdata.h"
#include "..\shared\auxdata\delta_interpolator.h"

template<class C> inline C *Unknown_Find_Valid(DeltaCompoundClass *root, const NameClass &name) { C *t = Unknown_Find_Field<C>(root, name); if (t && t->Unknown70) return(t); return(NULL); }	// NAME 1: neutral (D10): as co.cpp's

// @0x62A7C0
Unknown62A7C0Class::Unknown62A7C0Class(RenderObjClass *robj) :
	Object(NULL),
	Percent(0.0f)
{
	RenderObjClass *shield = robj->Get_Sub_Object_By_Name("shield01");
	Shield[0] = new UVControllerClass(shield, 0, 1);
	shield->Release_Ref();
	shield = robj->Get_Sub_Object_By_Name("shield02");
	Shield[1] = new UVControllerClass(shield, 0, 1);
	shield->Release_Ref();
	shield = robj->Get_Sub_Object_By_Name("shield03");
	Shield[2] = new UVControllerClass(shield, 0, 1);
	shield->Release_Ref();
	Shield[0]->Unknown_4A13A0(Vector2(0.0f, -0.29f));
	Shield[1]->Unknown_4A13A0(Vector2(0.0f, -0.29f));
	Shield[2]->Unknown_4A13A0(Vector2(0.0f, -0.29f));
}

// @0x62A9D0
Unknown62A7C0Class::~Unknown62A7C0Class()
{
	delete Shield[0];
	delete Shield[1];
	delete Shield[2];
}

// @0x62AA20
// NOT EXACT (0.59, attempts.py 0x62AA20): the client holds the folded -0.29f in one float temporary (fld / fst) for the three offsets, ours stores it as immediates
void Unknown62A7C0Class::Unknown_62AA20(ClientObjectClass *co)
{
	if (co != Object) {
		Object = co;
		Unknown_62ADE0(0.0f);
	}
}

// @0x62AAE0
void Unknown62A7C0Class::Unknown_00()
{
	ClientObjectClass *co = Object;
	if (co != NULL) {
		DeltaCompoundClass *aux = (DeltaCompoundClass *)co->AuxData;
		if (aux != NULL) {
			DurationClass now((unsigned)co->Client->Unknown10BC);
			NameClass name("ShieldPercent");
			DeltaInterpolatorClass *percent = Unknown_Find_Valid<DeltaInterpolatorClass>(aux, name);
			if (percent) {
				Unknown_62ADE0(percent->Get_Value(now));
			}
		}
	}
}

// @0x62ADE0
void Unknown62A7C0Class::Unknown_62ADE0(float percent)
{
	if (percent != Percent) {
		Shield[2]->Unknown_4A13A0(Vector2(0.0f, -0.71f * (WWMath::Clamp(percent, 0.0f, 1.0f / 3.0f) * 3.0f) - 0.29f));
		Shield[1]->Unknown_4A13A0(Vector2(0.0f, -0.71f * ((WWMath::Clamp(percent, 1.0f / 3.0f, 2.0f / 3.0f) - 1.0f / 3.0f) * 3.0f) - 0.29f));
		Shield[0]->Unknown_4A13A0(Vector2(0.0f, -0.71f * ((WWMath::Clamp(percent, 2.0f / 3.0f, 1.0f) - 2.0f / 3.0f) * 3.0f) - 0.29f));
		Percent = percent;
	}
}

/*
** Reconstruction notes
** - The object: 0x62A7C0-0x62AF34, after ui_shipcust.cpp's (to 0x62A760); its table 0xAF7AD0 has one slot (0x62AAE0) and
**   the floats after it (0xAF7AD4-0xAF7AE8: -0.29, 2/3, 0.29, -0.71, 3, 1/3) are the file's literals. Nothing in the
**   client constructs it (no caller of 0x62A7C0 or of its thunk): a gadget left in, the shield meshes 'shield01' to
**   'shield03' of a model scrolled by UVControllerClass (uvcontrl.cpp) after the object's ShieldPercent.
** - Each shield shows a third: shield03 (+0x0C) 0 to 1/3, shield02 1/3 to 2/3, shield01 2/3 to 1, each its third times 3
**   as a V offset from -0.29 (empty) to -1.0 (full).
** - The offset's form: -0.71f * (third * 3.0f) - 0.29f keeps the client's two multiplies (fmul 3.0, fmul -0.71);
**   third * 3.0f * -0.71f (or 3.0 as a double) has VC6 fold the constants into one. 0x62ADE0 is defined last (the
**   client's order); VC6 still inlines it into 0x62AA20 (with 0) and 0x62AAE0.
** - NOT EXACT: 0x62AA20 (0x62ADE0 inlined with 0.0f; the client keeps the folded -0.29f in one float temporary, ours
**   stores immediates; tried a local offset, a float-returning helper: both exact elsewhere, no change here).
*/
