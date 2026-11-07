// Earth & Beyond client (reconstruction): app\client\camctrl_emote.h, the emote camera (the client's camctrl_emote.cpp per
// its __FILE__ 0xB9E2D8; this header's name is provisional, NAME 3 by D24) (CC1, 2026-11-07).
#ifndef CAMCTRL_EMOTE_H  // NAME 3 [camctrl_emote.h]: header name from camctrl_emote.cpp (D24)
#define CAMCTRL_EMOTE_H

#include "camctrl.h"
#include "unknown_7979a0.h"

// The camera manager's eleventh controller (state 10; vtable 0xB04BEC, 0x10 bytes): holds the camera where it is,
// at least four times 0x79B6B0's distance from the avatar
class CameraController10Class : public CameraControllerClass	// NAME 1: neutral (D10)
{
public:
	CameraController10Class(SClient *client);			// 0x7930E0
	virtual ~CameraController10Class();					// 0x7931B0, deleting 0x793180
	virtual void Unknown_04();							// +0x04 0x793230
	virtual void Unknown_08(Vector3 *pos, Quaternion *rot, int *mode, int *target);	// +0x08 0x793250
	virtual void Unknown_0C();							// +0x0C 0x793280
	void Unknown_7932A0();								// 0x7932A0 (places the module)

	CameraModule79C8E0Class *Unknown0C;					// +0x0C (0x79C8E0's module)
};

#endif // CAMCTRL_EMOTE_H
