// Earth & Beyond client (reconstruction): app\Client\ctrngadg.cpp (__FILE__ 0xB76294). Notes at the end of the file.
#include <stddef.h>
#include <math.h>
#include "ctrngadg.h"
#include "sclient.h"
#include "tasks\tsk_login.h"
#include "..\kernel\kernel.h"
#include "..\shared\g_assert.h"
#include "..\..\ww3d2\ww3d.h"	// WW3D::Get_Frame_Time (0xBFAE3C - 0xBFAE40, read inline)

bool Unknown_42EC60(int button, int arg);							// 0x42EC60 (a key down: input.cpp's)  // NAME 1 [button, arg]: neutral
bool Unknown_42EE90();												// 0x42EE90 (input.cpp: the window has the focus)
void Unknown_42ED20(float x, float y);								// 0x42ED20 (input.cpp: the pointer placed, 0..1)  // NAME 1 [x, y]: input.h's


#include "bridge_int.h"	// BridgeInterface (SClient +0x128C)












#include "camera_manager.h"














extern unsigned int Unknown_BE5F34;										// 0xBE5F34 (gadget.cpp's: the event mask)  // NAME 1: neutral (D10)

// A value set for a scope and restored after it (the slot calls run with 0xBE5F34 set to 1)
template <class T> class CtrnValueHolderClass	// NAME 1: neutral (D10)
{
public:
	CtrnValueHolderClass(T &value, T temp) : Value(value), Old(value) { value = temp; }	// NAME 1 [value, temp]: neutral
	~CtrnValueHolderClass() { Value = Old; }
	T &Value;
	T Old;
};

// @0x50A850
CtrnGadgetClass::CtrnGadgetClass(int x, int y, int width, int height, LineDisplayClientClass *client) :
	RectangleGadgetClass(x, y, width, height, 0x1080),
	Client(client),
	Unknown44(true),
	Unknown48(0),
	Unknown4C(0)
{
}

// @0x50AB30
bool CtrnGadgetClass::Unknown_30(float x, float y)
{
	if (RectangleGadgetClass::Unknown_30(x, y)) {
		if (!Unknown44) return(true);
		if (!Unknown_42EC60(0x12, 0) && (Unknown_42EC60(0x10, 0) || Unknown_42EC60(0x11, 0))) return(true);	// VK_MENU; VK_SHIFT, VK_CONTROL
	}
	return(false);
}











































// NOT EXACT: shape, 0.69 (explain.py): the release block is laid out after the Ctrl test, the wheel's default gets its own epilogue, 0 for +0x12B4 from ebx (see attempts.csv)
// @0x50ABC0
bool CtrnGadgetClass::Unknown_2C(KernelEventClass *event, float x, float y, unsigned int mask)
{
	if (!Unknown_42EE90() || !((SClient *)Client)->Unknown_74B5E0()) return(false);
	bool alt = Unknown_42EC60(0x12, 0);					// VK_MENU
	bool shift = !alt && Unknown_42EC60(0x10, 0);		// VK_SHIFT
	bool ctrl = !alt && Unknown_42EC60(0x11, 0);		// VK_CONTROL
	if (Unknown48 == x && Unknown4C == y) return(false);
	Unknown48 = x;
	Unknown4C = y;
	if (shift || ctrl) {
		SClient *sclient = (SClient *)((ClientClass *)Client)->Unknown_18();
		sclient->Unknown12B0 = sclient->Unknown10BC;
		sclient->Unknown12B4 = 0;
	}
	float speed = WW3D::Get_Frame_Time() * 0.1f;
	if (((SClient *)Client)->CameraMode == 0) {
		speed *= 0.5f;
		BridgeInterface *bridge = ((SClient *)Client)->Bridge;
		if (!bridge || !bridge->Scene.Peek() || !ctrl) goto release;
		{
			if (Unknown44) {
				x = bridge->Yaw;
				y = bridge->Pitch;
				Unknown44 = false;
				CtrnValueHolderClass<unsigned int> holder(Unknown_BE5F34, 1);
				Unknown_68();
			} else {
				x = (0.5f - x) * speed + bridge->Yaw;
				y = (y - 0.5f) * speed + bridge->Pitch;
				if (y < -1.4835299f) y = -1.4835299f;
				else if (y > 1.4835299f) y = 1.4835299f;
			}
			bridge->Set_Yaw(x);
			bridge->Set_Pitch(y);
		}
	} else {
		if (mask & 0x1000) {
			float zoom;
			switch (event->Key) {
				case 0x101: zoom = -0.5f; break;
				case 0x102: zoom = 0.5f; break;
				default:
					G_ASSERT(false);
					return(false);
			}
			zoom = ((SClient *)Client)->CameraManager->Get_Distance() * (float)exp(zoom);
			if (zoom < 0.5f) zoom = 0.5f;
			else if (zoom > 10.0f) zoom = 10.0f;
			((SClient *)Client)->CameraManager->Set_Distance(zoom);
			((SClient *)Client)->CameraManager->Unknown28 = true;
			return(true);
		}
		if (shift) {
			if (Unknown44) {
				Unknown44 = false;
				y = 0;
				CtrnValueHolderClass<unsigned int> holder(Unknown_BE5F34, 1);
				Unknown_68();
			} else {
				y = (y - 0.5f) * speed;
			}
			float zoom = ((SClient *)Client)->CameraManager->Get_Distance() * (float)exp(y);
			if (zoom < 0.5f) zoom = 0.5f;
			else if (zoom > 10.0f) zoom = 10.0f;
			((SClient *)Client)->CameraManager->Set_Distance(zoom);
			((SClient *)Client)->CameraManager->Unknown28 = true;
		} else {
			if (!ctrl) goto release;
			if (Unknown44) {
				x = ((SClient *)Client)->CameraManager->Get_Heading();
				y = ((SClient *)Client)->CameraManager->Get_Tilt();
				Unknown44 = false;
				CtrnValueHolderClass<unsigned int> holder(Unknown_BE5F34, 1);
				Unknown_68();
			} else {
				x = (0.5f - x) * speed + ((SClient *)Client)->CameraManager->Get_Heading();
				y = (y - 0.5f) * speed + ((SClient *)Client)->CameraManager->Get_Tilt();
				if (y < -1.4835299f) y = -1.4835299f;
				else if (y > 1.4835299f) y = 1.4835299f;
			}
			((SClient *)Client)->CameraManager->Set_Heading(x);
			((SClient *)Client)->CameraManager->Set_Tilt(y);
			((SClient *)Client)->CameraManager->Unknown28 = true;
		}
	}
	Unknown_42ED20(0.5f, 0.5f);
	((SClient *)((ClientClass *)Client)->Unknown_18())->Mouse->Unknown_5153B0();
	return(true);
release:
	((SClient *)Client)->CameraManager->Unknown28 = false;
	((SClient *)((ClientClass *)Client)->Unknown_18())->Mouse->Unknown_515400();
	Unknown44 = true;
	CtrnValueHolderClass<unsigned int> holder(Unknown_BE5F34, 1);
	Unknown_6C();
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
**	0x50ABC0 (functional, NOT EXACT): with the window focused (input.cpp 0x42EE90) and the task
**	focused (SClient 0x74B5E0), a pointer move (+0x48 / +0x4C its last x, y) turns the camera
**	(camera_manager.cpp's 0x79B370-0x79B410, the pitch clamped to +-1.4835 = 85 degrees, scaled by the frame
**	time and 0.1) or the bridge's view (bridge_int.cpp 0x53CA00 / 0x53C980) while Shift or Ctrl is down; the
**	wheel (0x101 / 0x102) zooms by exp(+-0.5) between 0.5 and 10. Slots +0x68 / +0x6C run with gadget.cpp's
**	0xBE5F34 set to 1 and restored (a scoped value holder: the EH states 0-3).
**	After it come the holder's destructor 0x50B2C0 (out of line for the EH unwind handlers 0xAAA8A0...) and
**	the object's static initialisers: 0x50B2E0 (the STL's ctype<unsigned short>::id guard, as 0x42B2F0) and
**	permstr.h's PermStringInit 0x50B310 / its atexit destructor 0x50B340; mainview.obj starts at 0x50B360.
**	Forms of 0x50ABC0 tried (attempts.csv): the key flags as alt / !alt && Shift / !alt && Ctrl (the client's
**	shared byte), a success flag (true or false first: Shift's and Ctrl's camera stores merge), goto release
**	(kept: the Ctrl block falls into the tail as the client's, but the release block moves up), else-goto (the
**	release block and the wheel's default as the client's, the stores merge), an inline zoom helper (worse).
*/
