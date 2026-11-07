// Earth & Beyond client (reconstruction): app\client\camctrl.h, the camera controllers' base. PROVISIONAL file name
// (D137): the base's code 0x7923E0-0x792460 sits before camctrl_dock.cpp's (0x7926B0) with no __FILE__ of its own
// (CM1, 2026-11-07).
#ifndef CAMCTRL_H  // NAME 1 [camctrl.h]: PROVISIONAL (D137)
#define CAMCTRL_H

class SClient; class Vector3; class Quaternion;

// The camera controllers' base (vtable 0xB04B40: its deleting destructor 0x792460, slots +0x04 .. +0x0C pure,
// +0x10 .. +0x18 0x7923E0 / 0x792410 / 0x792430, all inline: the client's copies are the COMDATs of the first
// camctrl object, camctrl_dock.cpp's). The camera manager holds one per state.
class CameraControllerClass	// NAME 1: neutral (D10)
{
public:
	CameraControllerClass(SClient *client) : Client(client), Flags(0) {}
	virtual ~CameraControllerClass() {}
	virtual void Unknown_04() = 0;						// +0x04 (the state's start: the manager's constructor calls it after +0x10(0))
	virtual void Unknown_08(Vector3 *pos, Quaternion *rot, int *mode, int *target) = 0;	// +0x08 (the manager 0x799E40 passes its position, orientation, the mode (1 eases, 2 sets; 2 by default) and an object ID for 0x7998B0 (-1 none))  // NAME 1 [pos, rot, mode, target]: what 0x799E40 does with them
	virtual void Unknown_0C() = 0;						// +0x0C
	virtual void Set_Flag(int bit) { Flags |= 1 << bit; }	// +0x10 0x7923E0  // NAME 1: what it does
	virtual bool Unknown_14() { return(false); }		// +0x14 0x792410 (0x79B690 asks the current state's)
	virtual void Clear_Flag(int bit) { Flags &= ~(1 << bit); }	// +0x18 0x792430  // NAME 1: what it does

	bool Get_Flag(int bit) volatile { return((Flags >> bit) & 1); }	// (0x797B50's shr / test al,1 tests; volatile: camctrl_dock.cpp reads Flags afresh at each test)  // NAME 1: what it does

	SClient *Client;									// +0x04  // NAME 2: what it holds (the manager's)
	unsigned Flags;										// +0x08  // NAME 1: neutral
};

#endif // CAMCTRL_H
