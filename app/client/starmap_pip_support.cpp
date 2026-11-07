// Earth & Beyond client (reconstruction): a line drawn on the star map PIP (notes at the end)
#include "starmap_pip_support.h"
#include "..\shared\g_assert.h"
#include "vector3.h"


StarmapPipSupportClass::StarmapPipSupportClass() :
	Unknown_466920Class(Vector3(0.0f, 0.0f, 0.0f), Vector3(1.0f, 1.0f, 1.0f), 2.0f, 0.8f, 0.8f, 0.8f, 0.8f),
	Unknown10C(0.0f, 0.0f),
	Unknown114(1.0f)
{
}


StarmapPipSupportClass::~StarmapPipSupportClass()
{






































































































































	G_ASSERT(!Peek_Scene());
}

void StarmapPipSupportClass::Unknown_7C3C60(const Vector2 &arg1, float arg2)
{
	Unknown10C = arg1;
	Unknown114 = arg2;
}

void StarmapPipSupportClass::Unknown_7C3CA0(float total_range, float arg1, float arg2, bool arg3)
{
	FAIL_IF(!total_range) return;
	float a = arg1 / total_range;
	if (1.0f < a) {
		a = 1.0f;
	}
	a = a * Unknown114;
	float b = arg2 / total_range;
	if (1.0f < b) {
		b = 1.0f;
	}
	b = b * Unknown114;
	if (arg3) {
		float x = b + Unknown10C.X;
		float y = Unknown10C.Y - 1.0f;
		Reset(Vector3(x, y, 0.0f), Vector3(x + a, y, 0.0f));
	} else {
		float y = Unknown10C.Y - b;
		float x = Unknown10C.X + 1.0f;
		Reset(Vector3(x, y, 0.0f), Vector3(x, y - a, 0.0f));
	}
}

/*
**	Reconstruction notes
**
**	The object's window is 0x7C3AC0 to 0x7C3EB3: __FILE__ 0xB9F758 on the asserts at lines 151 and 162, the literals
**	0xB9F748 ('!Peek_Scene()') and 0xB9F798 ('!total_range') beside it. 0x7C3A90 before it forwards to FontSys and is called
**	only from starmap_pip.cpp's window (0x7C1770, 0x7C2340). 0x7C3F10 up (table 0xB07008) reaches target_find.cpp's
**	literals: 'idx >= MAX_VISIBLE_GROUP_MEMBERS' at 0xB9F7AC is its FAIL_IF (0x7C43A0, __FILE__ 0xB9F7D4), not this file's.
*/
