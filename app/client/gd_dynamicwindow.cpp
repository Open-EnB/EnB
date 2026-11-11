// Earth & Beyond client (reconstruction): app\Client\gd_dynamicwindow.cpp (__FILE__ 0xB8CF58). Notes at the end of the file.
#include "gd_dynamicwindow.h"
#include "dynamic_window_class.h"
#include "rendobj.h"
#include "aabox.h"
#include "..\shared\g_assert.h"

bool Unknown_496110(RenderObjClass *robj, float a, float b, float c);	// 0x496110 (ro_funcs.cpp, cdecl): a model's highlight  // NAME 1 [robj, a, b, c]: as ui_group.h

DynamicWindowGadgetClass::DynamicWindowGadgetClass(const RefCountPtr<DynamicWindowRenderObjectClass> &object, unsigned int mask, unsigned int flags, bool highlight) :
	ButtonGadgetClass(NULL, mask, flags),
	DynamicObject(),
	Highlight(highlight)
{
	Set_Dynamic_Object(object);
}

DynamicWindowGadgetClass::DynamicWindowGadgetClass(DynamicWindowRenderObjectClass *object, unsigned int mask, unsigned int flags, bool highlight) :
	ButtonGadgetClass(NULL, mask, flags),
	DynamicObject(),
	Highlight(highlight)
{
	Set_Dynamic_Object(Create_Peek(object));
}

DynamicWindowGadgetClass::~DynamicWindowGadgetClass()
{
	Set_Render_Obj(NULL);
	DynamicObject = NULL;
}

void DynamicWindowGadgetClass::Set_Dynamic_Object(const RefCountPtr<DynamicWindowRenderObjectClass> &object)
{
	DynamicObject = object;
	if (!DynamicObject) {
		Set_Render_Obj(NULL);
		return;
	}
	Set_Render_Obj(DynamicObject->Get_Render_Object().Peek());
	if (Highlight) {
		Unknown_496110(RObj, 0.6f, 0.6f, 0.6f);
	}
}















bool DynamicWindowGadgetClass::Unknown_30(float x, float y)
{
	FAIL_IF(!DynamicObject) {
		return(false);
	}
	if (RObj == NULL || RObj->Is_Hidden() || !RObj->Peek_Scene()) {
		return(false);
	}
	if (Highlight) {
		Unknown_496110(RObj, 0.6f, 0.6f, 0.6f);
	}
	bool inside = false;
	AABoxClass box;
	DynamicObject->Get_Bounds(box);
	x = x * 640.0f - 320.0f;
	y = 240.0f - y * 480.0f;
	if (x >= box.Center.X - box.Extent.X && x <= box.Extent.X + box.Center.X &&
		y >= box.Center.Y - box.Extent.Y && y <= box.Extent.Y + box.Center.Y) {
		if (Highlight) {
			Unknown_496110(RObj, 1.0f, 1.0f, 1.0f);
		}
		inside = true;
	}
	return(inside);
}

/*
**	Reconstruction notes (MSG2, 2026-11-02)
**
**	Client 0x668860-0x668E1F: the constructors 0x668860 (over a RefCountPtr) / 0x6689C0 (over a pointer, through
**	Create_Peek 0x58B490), slot +0x24 0x668970 (inline: no model to report), the deleting destructor 0x668990,
**	the destructor 0x668B20, Set_Dynamic_Object 0x668BE0 (inlined into both constructors: /Ob2), the hit test
**	0x668C80 (FAIL_IF '!DynamicObject' on line 61; the pointer in the window's Get_Bounds box, in the 640 x 480
**	screen's centred coordinates) and a header static's initializer 0x668E00. 0x668840 before it is the
**	previous file's (table 0xAF9FAC). Every function byte-exact (appfile.py match).
*/
