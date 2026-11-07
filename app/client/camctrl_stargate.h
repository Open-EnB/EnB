// Earth & Beyond client (reconstruction): app\client\camctrl_stargate.h, the stargate camera (the client's
// camctrl_stargate.cpp per its __FILE__ 0xB9E47C; this header's name is provisional, NAME 3 by D24) (CC1, 2026-11-07).
#ifndef CAMCTRL_STARGATE_H  // NAME 3 [camctrl_stargate.h]: header name from camctrl_stargate.cpp (D24)
#define CAMCTRL_STARGATE_H

#include "camctrl.h"
#include "unknown_7979a0.h"

// The camera manager's fourth controller (state 3; vtable 0xB05014, 0x1C bytes): the gate at SClient +0x13BC; phase
// 0 until 7 seconds have passed or the avatar is within half the gate's reach, then phase 1 and request 0x18
class CameraController03Class : public CameraControllerClass	// NAME 1: neutral (D10)
{
public:
	CameraController03Class(SClient *client);			// 0x795900
	virtual ~CameraController03Class();					// 0x7959D0, deleting 0x7959A0
	virtual void Unknown_04();							// +0x04 0x795A50
	virtual void Unknown_08(Vector3 *pos, Quaternion *rot, int *mode, int *target);	// +0x08 0x795A80
	virtual void Unknown_0C();							// +0x0C 0x795C60
	void Unknown_795C80(bool phase);						// 0x795C80 (places the module for the phase)  // NAME 1 [phase]: what the callers pass

	CameraModule79C8E0Class *Unknown0C;					// +0x0C (0x79C8E0's module)
	unsigned Unknown10;									// +0x10 (the client clock +0x10BC at the start)
	int Unknown14;										// +0x14 (the phase, 0 or 1)
	int Unknown18;										// +0x18
};

#endif // CAMCTRL_STARGATE_H
