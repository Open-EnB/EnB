// Earth & Beyond client (reconstruction): app\client\camctrl_takeoff.h, the take-off camera (the client's
// camctrl_takeoff.cpp per its __FILE__ 0xB9E4BC; this header's name is provisional, NAME 3 by D24) (CC1, 2026-11-07).
#ifndef CAMCTRL_TAKEOFF_H  // NAME 3 [camctrl_takeoff.h]: header name from camctrl_takeoff.cpp (D24)
#define CAMCTRL_TAKEOFF_H

#include "camctrl.h"
#include "unknown_7979a0.h"
#include "quat.h"

// The camera manager's tenth controller (state 9; vtable 0xB05100, 0x60 bytes): the take-off, with effect 0x170 on the
// avatar for five seconds; +0x14 says yes after two
class CameraController09Class : public CameraControllerClass	// NAME 1: neutral (D10)
{
public:
	CameraController09Class(SClient *client);			// 0x7961B0
	virtual ~CameraController09Class();					// 0x796280, deleting 0x796250
	virtual void Unknown_04();							// +0x04 0x796300
	virtual void Unknown_08(Vector3 *pos, Quaternion *rot, int *mode, int *target);	// +0x08 0x796440
	virtual void Unknown_0C();							// +0x0C 0x796850
	virtual bool Unknown_14();							// +0x14 0x7968F0 (two seconds since the start)
	void Unknown_796920();								// 0x796920 (places the module, the ship's path)

	CameraModule79C8E0Class *Unknown0C;					// +0x0C (0x79C8E0's module)
	unsigned Unknown10;									// +0x10 (the client clock +0x10BC at the start)
	unsigned Unknown14;									// +0x14 (the effect's id: Next_Local_ID)
	Vector3 Unknown18;									// +0x18
	Vector3 Unknown24;									// +0x24
	Vector3 Unknown30;									// +0x30
	Quaternion Unknown3C;								// +0x3C
	Quaternion Unknown4C;								// +0x4C
	bool Unknown5C;										// +0x5C (the start clears it)
};

#endif // CAMCTRL_TAKEOFF_H
