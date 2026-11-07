// Earth & Beyond client (reconstruction): app\client\unknown_798ae0.h. PROVISIONAL file name (D137): the camera
// controller of state 0 (the object 0x798AE0-0x798D0F has no __FILE__, assert or string) (CM1, 2026-11-07).
#ifndef UNKNOWN_798AE0_H  // NAME 1 [unknown_798ae0.h]: PROVISIONAL (D137)
#define UNKNOWN_798AE0_H

#include "camctrl.h"

// The camera manager's first controller (state 0; vtable 0xB05318, 0x14 bytes): asks for request 0x16 once the avatar
// has been there for 15 seconds, or at once when it started with flag 0 set
class CameraController00Class : public CameraControllerClass	// NAME 1: neutral (D10)
{
public:
	CameraController00Class(SClient *client);			// 0x798AE0
	virtual ~CameraController00Class();					// 0x798B40, deleting 0x798B10
	virtual void Unknown_04();							// +0x04 0x798B60
	virtual void Unknown_08(void *arg1, void *arg2, int *new_state, int arg4);	// +0x08 0x798BA0
	virtual void Unknown_0C();							// +0x0C 0x798CD0 (empty)

	unsigned Unknown0C;									// +0x0C (the client clock +0x10BC when the avatar was seen)
	bool Unknown10;										// +0x10 (an avatar was there)
	bool Unknown11;										// +0x11 (flag 0 was set at the start)
};

#endif // UNKNOWN_798AE0_H
