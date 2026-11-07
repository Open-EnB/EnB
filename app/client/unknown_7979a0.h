// Earth & Beyond client (reconstruction): app\client\unknown_7979a0.h. PROVISIONAL file name (D137): the camera
// controller of state 2 (the object 0x7979A0-0x798ACF has no __FILE__ or assert) (CM1, 2026-11-07).
#ifndef UNKNOWN_7979A0_H  // NAME 1 [unknown_7979a0.h]: PROVISIONAL (D137)
#define UNKNOWN_7979A0_H

#include "camctrl.h"
#include "cammod.h"

// The camera module this controller drives (constructor 0x79C8E0, 0x1C bytes, vtable 0xB05594: the object after
// cammod_chase.cpp's; not rebuilt)
class CameraModule79C8E0Class : public CameraModuleClass	// NAME 1: neutral (D10)
{
public:
	CameraModule79C8E0Class(SClient *client);			// 0x79C8E0
	virtual void Unknown_08(Vector3 *des_pos, Quaternion *des_quat);	// +0x08
	char Unknown08[0x1C - 0x08];						// +0x08: not rebuilt
};

// The camera manager's third controller (state 2; vtable 0xB05268, 0x1C bytes)
class CameraController02Class : public CameraControllerClass	// NAME 1: neutral (D10)
{
public:
	CameraController02Class(SClient *client);			// 0x7979A0
	virtual ~CameraController02Class();					// 0x797A90, deleting 0x797A60
	virtual void Unknown_04();							// +0x04 0x797B10
	virtual void Unknown_08(Vector3 *pos, Quaternion *rot, int *mode, int *target);	// +0x08 0x797B50 (not rebuilt)
	virtual void Unknown_0C();							// +0x0C 0x798130 (stops the phase's module)
	void Unknown_7981B0();								// 0x7981B0 (not rebuilt)

	CameraModuleClass *Unknown0C;						// +0x0C (0x79C8E0's module)
	int Unknown10;										// +0x10 (the phase, 0-4; 2 uses the manager's chase module)
	int Unknown14;										// +0x14 (-1; the start sets the client clock + 2000)
	int Unknown18;										// +0x18
};

#endif // UNKNOWN_7979A0_H
