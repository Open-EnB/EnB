// Earth & Beyond client (reconstruction): app\client\ship_equip_pip.h, the ship's PIP on the PDA's equipment window (the
// client's ship_equip_pip.cpp per its asserts, 'e:\build\beta\G\app\Client\ship_equip_pip.cpp' at 0xB79614; this
// header's name is provisional, NAME 1, D137). Laid out from the constructor 0x541DE0 and the destructor 0x542180 (O18,
// 2026-11-13).
#ifndef SHIP_EQUIP_PIP_H  // NAME 1 [ship_equip_pip.h]: provisional header name for ship_equip_pip.cpp
#define SHIP_EQUIP_PIP_H

#include "pip_scene.h"
#include "matrix3d.h"

class LightClass;
class ProxyRObj;

// The ship's PIP (0xB0 bytes, table 0xAEF058: the destructor only; ui_pda_equip.cpp makes it, gd_ship_equip_pip.cpp
// turns and zooms it): the ship's model through a proxy in the PIP's scene, a light turned 30 degrees off the camera,
// the camera on a trackball at a distance scaled by CameraManagerClass::Unknown_79B6B0
class ShipEquipPipClass : public PipSceneClass	// NAME 3: class-from-file (D24) for app\Client\ship_equip_pip.cpp
{
public:
	ShipEquipPipClass(SClient *client);					// 0x541DE0 (FAIL_IF '!scene' l.96)  // NAME 1 [client]: what the window passes
	virtual ~ShipEquipPipClass();						// +0x00 0x542180 (deleting 0x542150)
	void Unknown_542250(const RefCountPtr<RenderObjClass> &robj);	// 0x542250 (FAIL_IF '!camera' l.177): the ship's model (ui_pda_equip.cpp's 0x5D2B90), the camera eased to the unit's distance  // NAME 1 [Unknown_542250, robj]: neutral (D10)
	void Unknown_542A10(const Vector2 &point, bool on);	// 0x542A10 (FAIL_IF '!camera' l.234): gd_ship_equip_pip.cpp's drag from the point  // NAME 1 [Unknown_542A10, point, on]: neutral (D10)
	void Unknown_542EE0(float zoom);					// 0x542EE0 (FAIL_IF '!camera' l.282): gd_ship_equip_pip.cpp's wheel, +-0.05  // NAME 1 [Unknown_542EE0, zoom]: what the wheel passes
	void Unknown_543200(Vector2 *dest_mouse, Vector2 point);	// 0x543200 (FAIL_IF '!dest_mouse' l.318): the point to -1..1  // NAME 1 [Unknown_543200, point]: neutral (D10); [dest_mouse]: the FAIL_IF's
	void Unknown_543270(const Matrix3D &tm);			// 0x543270 (FAIL_IF '!camera' l.340; no callers: the others inline it): the camera's and the light's transform  // NAME 1 [Unknown_543270, tm]: neutral (D10)

	RefCountPtr<LightClass> Light;						// +0x34  // NAME 2: what it holds
	RefCountPtr<ProxyRObj> Proxy;						// +0x38 (in the scene; proxies the ship's model)  // NAME 2: what it holds
	Vector2 LastMouse;									// +0x3C (Unknown_543200's last point)  // NAME 2: what it holds
	float Distance;										// +0x44 (the camera's from the model)  // NAME 2: what it holds
	float Unknown48;									// +0x48 (CameraManagerClass::Unknown_79B6B0 when last placed)  // NAME 1: neutral (D10)
	float Unknown4C;									// +0x4C (the ease: 1 down to 0 over a second)  // NAME 1: neutral (D10)
	Matrix3D Unknown50;									// +0x50 (the ease's start)  // NAME 1: neutral (D10)
	Matrix3D Unknown80;									// +0x80 (the ease's end)  // NAME 1: neutral (D10)
};

#endif // SHIP_EQUIP_PIP_H
