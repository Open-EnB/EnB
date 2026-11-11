// Earth & Beyond client (reconstruction): app\client\dynamic_window_move.cpp, a dynamic window's move gadget (the client's
// file per its asserts, 'e:\build\beta\G\app\Client\dynamic_window_move.cpp' at 0xB76C00). Notes at the end of the file.
#include "dynamic_window_move.h"
#include "ui_dynamicwindow.h"
#include "rendobj.h"
#include "..\shared\g_assert.h"

// 0x51A820
RefCountPtr<DynamicWindowMoveClass> DynamicWindowMoveClass::Create(UIDynamicWindowClass *window)
{
	return(Create_NEW(new DynamicWindowMoveClass(window)));
}

// 0x51A940
void DynamicWindowMoveClass::Set_Position(const Vector3 &position)
{
	Vector2 center(position.X, position.Y);	// NAME 1 [center]: what Unknown_58A580 sets
	Window->Unknown_58A580(center);
}

// 0x51A980
Vector3 DynamicWindowMoveClass::Get_Position()
{
	Vector2 center;	// NAME 1 [center]: Get_Center's
	Window->Get_Center(center);
	return(Vector3(center.X, center.Y, 0.0f));
}

// 0x51A9C0
GadgetClass *Unknown_51A9C0(UIDynamicWindowClass *window)
{
	RefCountPtr<RenderObjClass> robj = window->Get_Render_Object();
	RefCountPtr<Unknown_AFB794Class> move = DynamicWindowMoveClass::Create(window);
	GadgetClass *gadget = window->Unknown84_04();























	FAIL_IF(!robj) return(NULL);
	FAIL_IF(!move) return(NULL);
	FAIL_IF(!gadget) return(NULL);
	return(new DynamicWindowMoveGadgetClass(robj, move, gadget));
}

// 0x51AE00
DynamicWindowMoveGadgetClass::DynamicWindowMoveGadgetClass(const RefCountPtr<RenderObjClass> &model, const RefCountPtr<Unknown_AFB794Class> &target, GadgetClass *gadget) :
	MovableUIBackGadget(model, target),
	UnknownA4(gadget)
{
	Unknown_65FFA0(1000);
}

/*
** Reconstruction notes (O6, run 2026-11-10b).
**
** The object 0x51A820-0x51AF3F: DynamicWindowMoveClass (table 0xAED610: Create 0x51A820, Peek_Object 0x51A8A0, the
** constructor 0x51A8C0, the destructor 0x51A920 / deleting 0x51A8F0, Set_Position 0x51A940, Get_Position 0x51A980),
** the factory 0x51A9C0 (FAIL_IF lines 58-60; ui_item_examination.cpp's caller), the inline slots 0x51AD20-0x51ADE0,
** DynamicWindowMoveGadgetClass (table 0xAED628: the constructor 0x51AE00, deleting destructor 0x51AE90, destructor
** 0x51AEC0, slot +0x30 0x51AEE0) and RefCountPtr's destructor 0x51AF10.
*/
