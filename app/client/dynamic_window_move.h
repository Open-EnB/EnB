// Earth & Beyond client (reconstruction): app\client\dynamic_window_move.h, a dynamic window's move gadget (the client's
// dynamic_window_move.cpp, 'e:\build\beta\G\app\Client\dynamic_window_move.cpp' at 0xB76C00; this header's name is
// provisional, NAME 1). The object is 0x51A820-0x51AF3F.
#ifndef DYNAMIC_WINDOW_MOVE_H  // NAME 1 [dynamic_window_move.h]: provisional header name for dynamic_window_move.cpp
#define DYNAMIC_WINDOW_MOVE_H

#include "gd_uiback.h"

class UIDynamicWindowClass;

// What MovableUIBackGadget moves for a dynamic window: the window's center (0xC bytes, table 0xAED610)
class DynamicWindowMoveClass : public Unknown_AFB794Class  // NAME 3: class-from-file (D24) for app\Client\dynamic_window_move.cpp
{
public:
	DynamicWindowMoveClass(UIDynamicWindowClass *window) : Window(window) {}				// 0x51A8C0  // NAME 1 [window]: neutral
	virtual ~DynamicWindowMoveClass() {}														// +0x04 0x51A920 (deleting 0x51A8F0)
	virtual void Set_Position(const Vector3 &position);										// +0x08 0x51A940 (the window's Unknown_58A580)
	virtual Vector3 Get_Position();															// +0x0C 0x51A980 (the window's Get_Center, z 0)
	virtual RenderObjClass *Peek_Object() { return(0); }										// +0x10 0x51A8A0
	static RefCountPtr<DynamicWindowMoveClass> Create(UIDynamicWindowClass *window);		// 0x51A820  // NAME 1 [Create]: what it does

	UIDynamicWindowClass *Window;															// +0x08  // NAME 1: neutral (D10)
};

// The gadget that moves it: answers the pointer as the window's own gadget does (0xA8 bytes, table 0xAED628)
class DynamicWindowMoveGadgetClass : public MovableUIBackGadget  // NAME 1: neutral (D10)
{
public:
	DynamicWindowMoveGadgetClass(const RefCountPtr<RenderObjClass> &model, const RefCountPtr<Unknown_AFB794Class> &target, GadgetClass *gadget);	// 0x51AE00  // NAME 1 [gadget]: neutral
	virtual bool Unknown_30(float x, float y) { return(UnknownA4->Unknown_30(x, y)); }		// +0x30 0x51AEE0

	GadgetClass *UnknownA4;																	// +0xA4 (the window's gadget)  // NAME 1: neutral (D10)
};

GadgetClass *Unknown_51A9C0(UIDynamicWindowClass *window);									// 0x51A9C0 (FAIL_IF '!robj', '!move', '!gadget': the window's move gadget)  // NAME 1 [window]: neutral

#endif
