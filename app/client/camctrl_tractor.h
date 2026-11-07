// Earth & Beyond client (reconstruction): app\client\camctrl_tractor.h, the tractor camera (the client's
// camctrl_tractor.cpp per its __FILE__ 0xB9E504; this header's name is provisional, NAME 3 by D24) (CC1, 2026-11-07).
#ifndef CAMCTRL_TRACTOR_H  // NAME 3 [camctrl_tractor.h]: header name from camctrl_tractor.cpp (D24)
#define CAMCTRL_TRACTOR_H

#include "camctrl.h"
#include "unknown_7979a0.h"

// The camera manager's ninth controller (state 8; vtable 0xB051BC, 0x20 bytes): follows the object SClient +0x13B4
// (phase 1) until it is within reach of the avatar or 15 seconds pass (phase 2 for a second, then 3: request 0x10)
class CameraController08Class : public CameraControllerClass	// NAME 1: neutral (D10)
{
public:
	CameraController08Class(SClient *client);			// 0x797320
	virtual ~CameraController08Class();					// 0x797410, deleting 0x7973E0
	virtual void Unknown_04();							// +0x04 0x797490
	virtual void Unknown_08(Vector3 *pos, Quaternion *rot, int *mode, int *target);	// +0x08 0x7974D0
	virtual void Unknown_0C();							// +0x0C 0x797720
	void Unknown_797570();								// 0x797570 (the phases)
	void Unknown_797740();								// 0x797740 (stop and start again; inlined in the update)
	void Unknown_797770();								// 0x797770 (places the module)

	CameraModule79C8E0Class *Unknown0C;					// +0x0C (0x79C8E0's module)
	unsigned Unknown10;									// +0x10 (the client clock +0x10BC at the start)
	unsigned Unknown14;									// +0x14 (phase 2's end: the clock + 1000)
	int Unknown18;										// +0x18 (SClient +0x13B4 at the start)
	int Unknown1C;										// +0x1C (the phase, 1-3)
};

#endif // CAMCTRL_TRACTOR_H
