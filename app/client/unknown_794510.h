// Earth & Beyond client (reconstruction): app\client\unknown_794510.h. PROVISIONAL file name (D137): the camera
// controller of state 1 (the object 0x794510-0x7946BF has no __FILE__, assert or string) (CC1, 2026-11-07).
#ifndef UNKNOWN_794510_H  // NAME 1 [unknown_794510.h]: PROVISIONAL (D137)
#define UNKNOWN_794510_H

#include "camctrl.h"

// The camera manager's second controller (state 1; vtable 0xB04DF8, 0x10 bytes): drives the manager's chase module
// on the avatar; stays while the options' CameraMode is set, else hands back state 2
class CameraController01Class : public CameraControllerClass	// NAME 1: neutral (D10)
{
public:
	CameraController01Class(SClient *client);			// 0x794510
	virtual ~CameraController01Class();					// 0x794570, deleting 0x794540
	virtual void Unknown_04();							// +0x04 0x794590
	virtual void Unknown_08(Vector3 *pos, Quaternion *rot, int *mode, int *target);	// +0x08 0x7945E0 (target: the followed id)
	virtual void Unknown_0C();							// +0x0C 0x794670

	bool Unknown0C;										// +0x0C (true from the constructor and the start; the first update clears it)
};

#endif // UNKNOWN_794510_H
