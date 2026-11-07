// Earth & Beyond client (reconstruction): app\client\camctrl_land.h, the landing camera (the client's camctrl_land.cpp per
// its __FILE__ 0xB9E374; this header's name is provisional, NAME 3 by D24) (CC1, 2026-11-07).
#ifndef CAMCTRL_LAND_H  // NAME 3 [camctrl_land.h]: header name from camctrl_land.cpp (D24)
#define CAMCTRL_LAND_H

#include "camctrl.h"
#include "cammod.h"
#include "matrix3d.h"

// The camera module the landing camera drives (constructor 0x79C4A0, 0x44 bytes: one of the cammod objects after
// cammod_chase.cpp's; not rebuilt; placed here, PROVISIONAL, D137), laid out from camctrl_land.cpp's 0x793CE0 stores
class CameraModule79C4A0Class : public CameraModuleClass	// NAME 1: neutral (D10)
{
public:
	CameraModule79C4A0Class(SClient *client);			// 0x79C4A0
	virtual void Unknown_08(Vector3 *des_pos, Quaternion *des_quat);	// +0x08

	Matrix3D Unknown08;									// +0x08 (0x793CE0: the camera's transform)
	float Unknown38;									// +0x38 (0x793CE0: +-0.00002 at random)
	int Unknown3C;										// +0x3C (0x793CE0: 2)
	int Unknown40;										// +0x40: not seen
};

// The camera manager's sixth controller (state 5; vtable 0xB04D4C, 0x18 bytes): the landing at the target SClient
// +0x13B8, with effect 0x16A on the avatar; after four seconds request 9
class CameraController05Class : public CameraControllerClass	// NAME 1: neutral (D10)
{
public:
	CameraController05Class(SClient *client);			// 0x793920
	virtual ~CameraController05Class();					// 0x793A10, deleting 0x7939E0
	virtual void Unknown_04();							// +0x04 0x793A90
	virtual void Unknown_08(Vector3 *pos, Quaternion *rot, int *mode, int *target);	// +0x08 0x793BC0
	virtual void Unknown_0C();							// +0x0C 0x793C40
	virtual bool Unknown_14() { return(true); }			// +0x14 0x7939C0
	void Unknown_793CE0();								// 0x793CE0 (places the module, moves the ship along a spline)

	CameraModule79C4A0Class *Unknown0C;					// +0x0C (0x79C4A0's module)
	unsigned Unknown10;									// +0x10 (the client clock +0x10BC at the start)
	unsigned Unknown14;									// +0x14 (the effect's id: the client's +0x113C counted down)
};

#endif // CAMCTRL_LAND_H
