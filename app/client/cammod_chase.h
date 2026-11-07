// Earth & Beyond client (reconstruction): app\client\cammod_chase.h, the chase camera module (the client's
// cammod_chase.cpp per its __FILE__ 0xB9E658; this header's name by convention, D24). Laid out from the constructor
// 0x79BB80 (0x38 bytes, CameraManagerClass's new) (CM1, 2026-11-07).
#ifndef CAMMOD_CHASE_H  // NAME 3 [cammod_chase.h]: header name from cammod_chase.cpp (D24)
#define CAMMOD_CHASE_H

#include "cammod.h"
#include "..\shared\accel.h"

class Vector3;
class Quaternion;
class ClientObjectClass;

// The camera manager's +0x04 (vtable 0xB054E4)
class ChaseCameraModuleClass : public CameraModuleClass	// NAME 2: the FAIL_IF '!CameraManager->Get_Player_Chase_Camera_Module()' (sclient.cpp 0x744C10)
{
public:
	ChaseCameraModuleClass(SClient *client);			// 0x79BB80
	virtual ~ChaseCameraModuleClass() {}				// 0x79BCC0, deleting 0x79BC90
	virtual void Unknown_04();							// +0x04 0x79BCE0 (shows the object, then the base's)
	virtual void Unknown_08(Vector3 *des_pos, Quaternion *des_quat);	// +0x08 0x79BD50  // NAME 2 [des_pos, des_quat]: 0x79BFC0's FAIL_IFs
	virtual void Unknown_0C();							// +0x0C 0x79BDC0 (hides the object, then the base's)
	void Set_Heading(float heading);					// 0x79BE00  // NAME 2: the manager's (sclient.cpp)
	void Set_Distance(float distance);					// 0x79BE30  // NAME 2: the manager's
	void Set_Tilt(float tilt);							// 0x79BE60  // NAME 2: the manager's
	void Unknown_79BE90(bool arg);						// 0x79BE90  // NAME 1 [arg]: neutral
	void Set_Elevation_Offset(float offset);			// 0x79BEC0  // NAME 2: the manager's
	void Unknown_79BEF0(bool hidden);					// 0x79BEF0 (SClient::Init passes CameraMode == 0)  // NAME 2 [hidden]: what it passes to 0x6F3BF0
	void Unknown_79BF40(int id);						// 0x79BF40 (the followed object's id)  // NAME 1 [id]: neutral
	void Unknown_79BFC0(ClientObjectClass *obj, Quaternion *des_quat, Vector3 *des_pos);	// 0x79BFC0 (NOT EXACT)  // NAME 2 [des_quat, des_pos, obj]: its FAIL_IFs

	float Heading;										// +0x08  // NAME 2: sclient.cpp's
	float Distance;										// +0x0C  // NAME 2: sclient.cpp's
	float Tilt;											// +0x10  // NAME 2: sclient.cpp's
	bool Unknown14;										// +0x14 (true; 0x79BFC0 keeps the object's up when clear)
	float ElevationOffset;								// +0x18  // NAME 2: sclient.cpp's
	bool Unknown1C;										// +0x1C (passed to the object's 0x6F3BF0 while active)
	int ObjectID;										// +0x20 (-1: none)  // NAME 1: neutral
	AccelClass Unknown24;								// +0x24 (0x79BFC0: steered toward the object's 0x6F3ED0 angle / 4)
	bool Unknown34;										// +0x34 (set while active, slots +0x04 / +0x0C; sclient.cpp 0x744C10 reads it)
};

#endif // CAMMOD_CHASE_H
