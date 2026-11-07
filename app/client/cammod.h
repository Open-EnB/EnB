// Earth & Beyond client (reconstruction): app\client\cammod.h, the camera modules' base. PROVISIONAL file name (D137):
// the object 0x79B9E0-0x79BB6F has no __FILE__; named after its neighbour cammod_chase.cpp (CM1, 2026-11-07).
#ifndef CAMMOD_H  // NAME 1 [cammod.h]: PROVISIONAL (D137)
#define CAMMOD_H

class SClient;
class SignalMap;
class Vector3;
class Quaternion;

// What a module emits on the modules' signal map when it starts (true) or stops (false)
class CameraModuleSignal	// NAME 5: the client's typeid name .?AVCameraModuleSignal@@ (0xB9ACA8)
{
public:
	CameraModuleSignal(bool on) : On(on) {}				// NAME 1 [on]: neutral
	bool On;											// +0x00  // NAME 1: neutral (D10)
};

// The camera modules' base (vtable 0xB054F8, 8 bytes; its deleting destructor 0x79BC60 is emitted in cammod_chase.cpp)
class CameraModuleClass	// NAME 5: the client's typeid name .?AVCameraModuleClass@@ (0xB9ABE8)
{
public:
	CameraModuleClass(SClient *client) : Client(client) {}
	virtual ~CameraModuleClass() {}
	virtual void Unknown_04();							// +0x04 0x79BA50 (emits CameraModuleSignal(true))
	virtual void Unknown_08(Vector3 *des_pos, Quaternion *des_quat) = 0;	// +0x08 (the camera manager passes where to put the camera)  // NAME 2 [des_pos, des_quat]: cammod_chase.cpp's FAIL_IFs
	virtual void Unknown_0C();							// +0x0C 0x79BAD0 (emits CameraModuleSignal(false))

	static SignalMap &Get_Signal_Map();					// 0x79B9E0 (the function-local static 0xBE86A0, guard 0xBE8698)  // NAME 1: neutral, what it returns

	SClient *Client;									// +0x04  // NAME 2: what it holds (SClient::Init's this)
};

#endif // CAMMOD_H
