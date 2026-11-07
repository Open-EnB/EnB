// Earth & Beyond client (reconstruction): app\client\camctrl_entersector.h, the sector-entry camera (the client's
// camctrl_entersector.cpp per its __FILE__ 0xB9E330; this header's name is provisional, NAME 3 by D24) (CC1, 2026-11-07).
#ifndef CAMCTRL_ENTERSECTOR_H  // NAME 3 [camctrl_entersector.h]: header name from camctrl_entersector.cpp (D24)
#define CAMCTRL_ENTERSECTOR_H

#include "camctrl.h"
#include "unknown_7979a0.h"

// The camera manager's fifth controller (state 4; vtable 0xB04C98, 0x14 bytes): a camera beside the avatar at a random
// angle, for five seconds, then request 7
class CameraController04Class : public CameraControllerClass	// NAME 1: neutral (D10)
{
public:
	CameraController04Class(SClient *client);			// 0x793520
	virtual ~CameraController04Class();					// 0x7935F0, deleting 0x7935C0
	virtual void Unknown_04();							// +0x04 0x793670
	virtual void Unknown_08(Vector3 *pos, Quaternion *rot, int *mode, int *target);	// +0x08 0x7936A0
	virtual void Unknown_0C();							// +0x0C 0x793720
	void Unknown_793740();								// 0x793740 (places the module)

	CameraModule79C8E0Class *Unknown0C;					// +0x0C (0x79C8E0's module)
	unsigned Unknown10;									// +0x10 (the client clock +0x10BC at the start)
};

#endif // CAMCTRL_ENTERSECTOR_H
