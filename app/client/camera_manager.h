// Earth & Beyond client (reconstruction): app\client\camera_manager.h, the camera manager (the client's camera_manager.cpp
// per its __FILE__ 0xB9E5C0; this header's name is provisional, NAME 3 by D24). Laid out from the constructor 0x798D20
// (0x4C bytes, SClient::Init's new) and the destructor 0x799220 (CM1, 2026-11-07).
#ifndef CAMERA_MANAGER_H  // NAME 3 [camera_manager.h]: header name from camera_manager.cpp (D24)
#define CAMERA_MANAGER_H

#include <vector>
#include "vector3.h"
#include "quat.h"

class SClient;

// The camera controllers' base (vtable 0xB04B40: its destructor 0x792460, slots +0x04 .. +0x0C pure, +0x10 .. +0x18
// 0x7923E0 / 0x792410 / 0x792430; its file not rebuilt). The manager holds one per state, STATE_LAST_CONTROLLER of them.
class CameraControllerClass	// NAME 1: neutral (D10)
{
public:
	virtual ~CameraControllerClass();
	virtual void Unknown_04() = 0;						// +0x04 (the constructor's, after +0x10(false): the state's start)
	virtual void Unknown_08() = 0;						// +0x08
	virtual void Unknown_0C() = 0;						// +0x0C
	virtual void Unknown_10(bool arg);					// +0x10 0x7923E0 (the constructor passes false)  // NAME 1 [arg]: neutral
	virtual bool Unknown_14();							// +0x14 0x792410 (0x79B690 asks the current state's)
	virtual void Unknown_18();							// +0x18 0x792430
};

// The twelve controllers in the manager's order (their files not rebuilt: the camctrl_*.cpp objects 0x792320-0x798D10;
// declared as far as the constructor's new needs them: the size and the constructor)
#define CAMERA_CONTROLLER(name, size, ctor) \
	class name : public CameraControllerClass { public: name(SClient *client); void Unknown_04(); void Unknown_08(); void Unknown_0C(); char Unknown04[size - 4]; };
CAMERA_CONTROLLER(CameraController00Class, 0x14, 0x798AE0)	// NAME 1: neutral (D10): vtable 0xB05318
CAMERA_CONTROLLER(CameraController01Class, 0x10, 0x794510)	// NAME 1: neutral (D10)
CAMERA_CONTROLLER(CameraController02Class, 0x1C, 0x7979A0)	// NAME 1: neutral (D10)
CAMERA_CONTROLLER(CameraController03Class, 0x1C, 0x795900)	// NAME 1: neutral (D10)
CAMERA_CONTROLLER(CameraController04Class, 0x14, 0x793520)	// NAME 1: neutral (D10)
CAMERA_CONTROLLER(CameraController05Class, 0x18, 0x793920)	// NAME 1: neutral (D10)
CAMERA_CONTROLLER(CameraController06Class, 0x1C, 0x792320)	// NAME 1: neutral (D10): state 6, 'Camera_Dock'
CAMERA_CONTROLLER(CameraController07Class, 0x18, 0x7946D0)	// NAME 1: neutral (D10)
CAMERA_CONTROLLER(CameraController08Class, 0x20, 0x797320)	// NAME 1: neutral (D10)
CAMERA_CONTROLLER(CameraController09Class, 0x60, 0x7961B0)	// NAME 1: neutral (D10)
CAMERA_CONTROLLER(CameraController10Class, 0x10, 0x7930E0)	// NAME 1: neutral (D10)
CAMERA_CONTROLLER(CameraController11Class, 0x10, 0x7953B0)	// NAME 1: neutral (D10)
#undef CAMERA_CONTROLLER

// The manager's +0x04 (cammod_chase.cpp: constructor 0x79BB80, 0x38 bytes, vtable 0xB054F8)
class ChaseCameraModuleClass	// NAME 2: the FAIL_IF '!CameraManager->Get_Player_Chase_Camera_Module()' (sclient.cpp 0x744C10)
{
public:
	ChaseCameraModuleClass(SClient *client);			// 0x79BB80
	virtual ~ChaseCameraModuleClass();					// 0x79BCC0
	void Set_Heading(float heading);					// 0x79BE00  // NAME 2: the manager's (sclient.cpp)
	void Set_Distance(float distance);					// 0x79BE30  // NAME 2: the manager's
	void Set_Tilt(float tilt);							// 0x79BE60  // NAME 2: the manager's
	void Unknown_79BE90(bool arg);						// 0x79BE90  // NAME 1 [arg]: neutral
	void Set_Elevation_Offset(float offset);			// 0x79BEC0  // NAME 2: the manager's
	void Unknown_79BEF0(bool arg);						// 0x79BEF0  // NAME 1 [arg]: neutral

	int Unknown04;										// +0x04
	float Heading;										// +0x08  // NAME 2: sclient.cpp's
	float Distance;										// +0x0C  // NAME 2: sclient.cpp's
	float Tilt;											// +0x10  // NAME 2: sclient.cpp's
	bool Unknown14;										// +0x14
	float ElevationOffset;								// +0x18  // NAME 2: sclient.cpp's
	bool Unknown1C;										// +0x1C
	int Unknown20[(0x34 - 0x20) / 4];					// +0x20
	bool Unknown34;										// +0x34 (sclient.cpp 0x744C10 asks the main view's slot +0x78 only when set)
};

class CameraManagerClass	// NAME 3: class-from-file (D24) for camera_manager.cpp
{
public:
	enum {
		STATE_LAST_CONTROLLER = 12						// NAME 5: the constructor's assert
	};

	CameraManagerClass(SClient *client);				// 0x798D20
	~CameraManagerClass();								// 0x799220
	void Unknown_7992F0();								// 0x7992F0 (ACTION 7 for SClient +0x13AC)
	void Unknown_7993E0();								// 0x7993E0 (ACTION 19 for SClient +0x13BC)
	void Unknown_7994D0();								// 0x7994D0 (ACTION 8 for SClient +0x13B8)
	void Unknown_799590();								// 0x799590 (ACTION 27 for the avatar)
	void Unknown_799610(const Vector3 &pos, const Quaternion &rot);	// 0x799610 (eases Position / Orientation toward them)  // NAME 1 [pos, rot]: what they are
	void Unknown_7998B0();								// 0x7998B0 (not rebuilt)
	bool Unknown_799DA0(int *new_state, int state);		// 0x799DA0  // NAME 2 [new_state]: its FAIL_IF; NAME 1 [state]: neutral
	void Unknown_799E40();								// 0x799E40 (SClient's Process_AI; not rebuilt)
	void Unknown_79B2B0(int request);					// 0x79B2B0 (sets the request's bit in Requests)  // NAME 1 [request]: neutral
	bool Unknown_79B330(int request);					// 0x79B330 (tests and clears it)  // NAME 1 [request]: neutral
	void Set_Heading(float heading); float Get_Heading();	// 0x79B370, 0x79B390  // NAME 2: sclient.cpp's
	void Set_Distance(float distance); float Get_Distance();	// 0x79B3B0, 0x79B3D0  // NAME 2: sclient.cpp's
	void Set_Tilt(float tilt); float Get_Tilt();		// 0x79B3F0, 0x79B410  // NAME 2: sclient.cpp's
	void Unknown_79B430(bool arg); bool Unknown_79B450();	// 0x79B430, 0x79B450  // NAME 1 [arg]: neutral
	void Set_Elevation_Offset(float offset); float Get_Elevation_Offset();	// 0x79B470, 0x79B490  // NAME 2: sclient.cpp's
	void Unknown_79B4B0(bool first_mode); bool Unknown_79B4D0();	// 0x79B4B0, 0x79B4D0  // NAME 1 [first_mode]: sclient.cpp's
	bool Unknown_79B4F0(int state);						// 0x79B4F0 (the option allowing the state)  // NAME 1 [state]: neutral
	bool Unknown_79B600();								// 0x79B600 (state 5, 6, 7 or 9)
	bool Unknown_79B630();								// 0x79B630 (state 6)
	bool Unknown_79B650();								// 0x79B650 (state 3)
	bool Unknown_79B670();								// 0x79B670 (state 1)
	bool Unknown_79B690();								// 0x79B690 (the current controller's +0x14)
	static float Unknown_79B6B0();						// 0x79B6B0 (75: the camera controllers', the radar's and the UI's)
	ChaseCameraModuleClass *Get_Player_Chase_Camera_Module() { return(Module); }	// NAME 5: sclient.cpp's FAIL_IF

	SClient *Client;									// +0x00  // NAME 2: what it holds
	ChaseCameraModuleClass *Module;						// +0x04  // NAME 1: neutral
	unsigned Requests;									// +0x08 (one bit per request, 0x79B2B0 / 0x79B330)  // NAME 1: neutral
	std::vector<CameraControllerClass *> Controllers;	// +0x0C (allocator), +0x10 .. +0x18  // NAME 5: the constructor's assert
	int State;											// +0x1C (the index into Controllers)  // NAME 2: the FAIL_IF 'new_state == State'
	float Unknown20;									// +0x20 (Position's ease rate)
	float Unknown24;									// +0x24 (Orientation's ease rate)
	bool Unknown28;										// +0x28
	Vector3 Position;									// +0x2C  // NAME 1: neutral
	Quaternion Orientation;								// +0x38  // NAME 1: neutral
	float FOV;											// +0x48  // NAME 2: sclient.cpp's
};

#endif // CAMERA_MANAGER_H
