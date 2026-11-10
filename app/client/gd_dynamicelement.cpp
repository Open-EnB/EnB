// Earth & Beyond client (reconstruction): interim app\client\gd_dynamicelement.cpp (D137; no __FILE__). Notes at the end of the file.
#include "gd_dynamicelement.h"
#include "dynamic_window_render_object.h"

DynamicElementGadgetClass::DynamicElementGadgetClass(DynamicWindowElementClass *element) :
	GadgetClass(0x55),
	Element(element)
{
}

bool DynamicElementGadgetClass::Unknown_30(float x, float y)
{
	AABoxClass box;
	Element->Get_Bounds(box);
	x = x * 640.0f - 320.0f;
	y = 240.0f - y * 480.0f;
	if (x >= box.Center.X - box.Extent.X && x <= box.Extent.X + box.Center.X &&
		y >= box.Center.Y - box.Extent.Y && y <= box.Extent.Y + box.Center.Y) {
		return(true);
	}
	return(false);
}

void DynamicElementGadgetClass::Unknown_64(AABoxClass &box)
{
	Element->Get_Bounds(box);
}

/*
**	Reconstruction notes (DW1, 2026-11-10)
**
**	Client 0x6686F0-0x66884F, no __FILE__ and no RTTI: the constructor 0x6686F0 (GadgetClass with 0x55, the
**	element at +0x30), the deleting destructor 0x668720 and the destructor 0x668750 (inline, empty), the hit
**	test 0x668770 (slot +0x30: gd_dynamicwindow.cpp's 0x668C80 without the model checks and the highlight) and
**	the bounds 0x668840 (slot +0x64). Callers: dynamic_window_toggle.cpp's constructor 0x51DEB0 and the item
**	examination window's rows (0x533450, 0x535CB0, 0x538350).
*/
