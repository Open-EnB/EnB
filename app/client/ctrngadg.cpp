// Earth & Beyond client (reconstruction): app\Client\ctrngadg.cpp (__FILE__ 0xB76294). Notes at the end of the file.
#include <stddef.h>
#include "ctrngadg.h"

bool Unknown_42EC60(int button, int arg);							// 0x42EC60 (a key down: input.cpp's)  // NAME 1 [button, arg]: neutral

// @0x50A850
CtrnGadgetClass::CtrnGadgetClass(int x, int y, int width, int height, LineDisplayClientClass *client) :
	InterfaceDeactivationGadgetClass(x, y, width, height, 0x1080),
	Client(client),
	Unknown44(true),
	Unknown48(0),
	Unknown4C(0)
{
}

// @0x50AB30
bool CtrnGadgetClass::Unknown_30(float x, float y)
{
	if (InterfaceDeactivationGadgetClass::Unknown_30(x, y)) {
		if (!Unknown44) return(true);
		if (!Unknown_42EC60(0x12, 0) && (Unknown_42EC60(0x10, 0) || Unknown_42EC60(0x11, 0))) return(true);	// VK_MENU; VK_SHIFT, VK_CONTROL
	}
	return(false);
}

/*
**	Reconstruction notes
**
**	The file: ctrngadg.cpp's __FILE__ (0xB76294) is cited by the event slot 0x50ABC0 ('false', line 162: an
**	event that is neither 0x101 nor 0x102 with mask bit 0x1000). Its object sits between dbgmgr.obj (to
**	0x50A810) and mainview.obj: the constructor 0x50A850, GadgetClass's one-line slots 0x50A8C0-0x50AAC0 (its
**	table 0xAECDE4 is the first in the link to need them), the deleting destructor 0x50AAE0 and the implicit
**	destructor 0x50AB10 (a jmp to GadgetClass's 0x65EE60), then the hit test 0x50AB30 (slot +0x30) before the
**	event slot 0x50ABC0 (+0x2C). The cockpit's set-up (ui_cpit.cpp 0x57DD20) is the one constructor caller.
**	0x50ABC0 (out of DG's claim, not written yet): with the window focused (input.cpp 0x42EE90) and the task
**	focused (SClient 0x74B5E0), a pointer move (+0x48 / +0x4C its last x, y) turns the camera
**	(camera_manager.cpp's 0x79B370-0x79B410, the pitch clamped to +-1.4835 = 85 degrees, scaled by the frame
**	time and 0.1) or the bridge's view (bridge_int.cpp 0x53CA00 / 0x53C980) while Shift or Ctrl is down; the
**	wheel (0x101 / 0x102) zooms by exp(+-0.5) between 0.5 and 10. Slots +0x68 / +0x6C run with gadget.cpp's
**	0xBE5F34 set to 1 and restored (a scoped value holder: the EH states 0-3).
*/
