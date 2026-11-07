// Earth & Beyond client (reconstruction): app\client\camctrl_pland.h, the planet-landing camera (the client's
// camctrl_pland.cpp per its __FILE__ 0xB9E3B8; this header's name is provisional, NAME 3 by D24) (CC1, 2026-11-07).
#ifndef CAMCTRL_PLAND_H  // NAME 3 [camctrl_pland.h]: header name from camctrl_pland.cpp (D24)
#define CAMCTRL_PLAND_H

#include "camctrl.h"
#include "unknown_7979a0.h"

// The camera manager's eighth controller (state 7; vtable 0xB04EA8, 0x18 bytes): the landing on a planet, from
// landingpattern.ini, with effect 0x16A on the avatar; after four seconds request 14
class CameraController07Class : public CameraControllerClass	// NAME 1: neutral (D10)
{
public:
	CameraController07Class(SClient *client);			// 0x7946D0
	virtual ~CameraController07Class();					// 0x7947C0, deleting 0x794790
	virtual void Unknown_04();							// +0x04 0x794840
	virtual void Unknown_08(Vector3 *pos, Quaternion *rot, int *mode, int *target);	// +0x08 0x794970
	virtual void Unknown_0C();							// +0x0C 0x7949F0
	virtual bool Unknown_14() { return(true); }			// +0x14 0x794770
	void Unknown_794A90();								// 0x794A90 (landingpattern.ini: places the module, moves the ship along a spline; not rebuilt)

	CameraModule79C8E0Class *Unknown0C;					// +0x0C (0x79C8E0's module)
	unsigned Unknown10;									// +0x10 (the client clock +0x10BC at the start)
	unsigned Unknown14;									// +0x14 (the effect's id: the client's +0x113C counted down)
};

#endif // CAMCTRL_PLAND_H
