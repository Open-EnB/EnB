// Earth & Beyond client (reconstruction): app\client\camctrl_rescue.h, the rescue camera (the client's camctrl_rescue.cpp
// per its __FILE__ 0xB9E440; this header's name is provisional, NAME 3 by D24) (CC1, 2026-11-07).
#ifndef CAMCTRL_RESCUE_H  // NAME 3 [camctrl_rescue.h]: header name from camctrl_rescue.cpp (D24)
#define CAMCTRL_RESCUE_H

#include "camctrl.h"
#include "camctrl_land.h"

// The camera manager's twelfth controller (state 11; vtable 0xB04F5C, 0x10 bytes): a camera behind and beside the
// avatar, on 0x79C4A0's module
class CameraController11Class : public CameraControllerClass	// NAME 1: neutral (D10)
{
public:
	CameraController11Class(SClient *client);			// 0x7953B0
	virtual ~CameraController11Class();					// 0x795480, deleting 0x795450
	virtual void Unknown_04();							// +0x04 0x795500
	virtual void Unknown_08(Vector3 *pos, Quaternion *rot, int *mode, int *target);	// +0x08 0x795520
	virtual void Unknown_0C();							// +0x0C 0x795550
	void Unknown_795570();								// 0x795570 (places the module)

	CameraModule79C4A0Class *Unknown0C;					// +0x0C (0x79C4A0's module)
};

#endif // CAMCTRL_RESCUE_H
