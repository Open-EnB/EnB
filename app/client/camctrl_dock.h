// Earth & Beyond client (reconstruction): app\client\camctrl_dock.h, the docking camera (the client's camctrl_dock.cpp per
// its __FILE__ 0xB9E248; this header's name is provisional, NAME 3 by D24) (CC1, 2026-11-07).
#ifndef CAMCTRL_DOCK_H  // NAME 3 [camctrl_dock.h]: header name from camctrl_dock.cpp (D24)
#define CAMCTRL_DOCK_H

#include "camctrl.h"
#include "unknown_7979a0.h"

// The camera manager's seventh controller (state 6; vtable 0xB04B1C, 0x1C bytes): docking (flag 0) and undocking
// (flag 1) at the target SClient +0x13AC, laid out from dock.ini's section for the target's render object
class CameraController06Class : public CameraControllerClass	// NAME 1: neutral (D10): state 6, 'Camera_Dock'
{
public:
	CameraController06Class(SClient *client);			// 0x792320
	virtual ~CameraController06Class();					// 0x7924E0, deleting 0x7924B0
	virtual void Unknown_04();							// +0x04 0x792560
	virtual void Unknown_08(Vector3 *pos, Quaternion *rot, int *mode, int *target);	// +0x08 0x792590
	virtual void Unknown_0C();							// +0x0C 0x7926B0
	virtual bool Unknown_14() { return(true); }			// +0x14 0x792490
	void Unknown_792750();								// 0x792750 (reads dock.ini, places the module and moves the ship)

	CameraModule79C8E0Class *Unknown0C;					// +0x0C (0x79C8E0's module)
	unsigned Unknown10;									// +0x10 (the client clock +0x10BC at the start)
	bool Unknown14;										// +0x14 (dock.ini named a bone)
	float Unknown18;									// +0x18 (dock.ini's Duration, in ms)
};

#endif // CAMCTRL_DOCK_H
