// Earth & Beyond client (reconstruction): app\client\camera_manager.cpp (notes at the end)
#include "camera_manager.h"
#include "sclient.h"
#include "client.h"
#include "co_data.h"
#include "emote_data.h"
#include "option.h"
#include "ww3d.h"
#include "wwmath.h"
#include "..\packets\pkaction.h"
#include "..\shared\g_assert.h"
#include "co.h"
#include "camera.h"
#include "..\common\unknown_navlist.h"
#include "obbox.h"
#include "castres.h"
#include "scene.h"

// the main view's scene (mainview.cpp's 0x75ECF0, its +0x10C), as far as 0x7998B0 calls it
class MainViewSceneClass : public SimpleSceneClass	// NAME 1: neutral (D10): mainview.cpp's
{
public:
	void Unknown_75FB00(const OBBoxClass &to, const OBBoxClass &from, const Vector3 &move, CastResultStruct *result, RenderObjClass *ignore, bool ship);	// 0x75FB00 (sweeps the box through the scene; camctrl 0x796920 calls it too)  // NAME 1 [to, from, move, result, ignore, ship]: what 0x7998B0 passes
};
























// 0x798D20
CameraManagerClass::CameraManagerClass(SClient *client) :
	Client(client),
	Module(NULL),
	Requests(0),
	State(0),
	Unknown20(0.001f),
	Unknown24(0.002f),
	Unknown28(false),
	Position(0.0f, 0.0f, 0.0f),
	Orientation(true),
	FOV(1.5707964f)
{
	Module = new ChaseCameraModuleClass(Client);
	CameraControllerClass *controller;
	controller = new CameraController00Class(Client); Controllers.push_back(controller);
	controller = new CameraController01Class(Client); Controllers.push_back(controller);
	controller = new CameraController02Class(Client); Controllers.push_back(controller);
	controller = new CameraController03Class(Client); Controllers.push_back(controller);
	controller = new CameraController04Class(Client); Controllers.push_back(controller);
	controller = new CameraController05Class(Client); Controllers.push_back(controller);
	controller = new CameraController06Class(Client); Controllers.push_back(controller);
	controller = new CameraController07Class(Client); Controllers.push_back(controller);
	controller = new CameraController08Class(Client); Controllers.push_back(controller);
	controller = new CameraController09Class(Client); Controllers.push_back(controller);
	controller = new CameraController10Class(Client); Controllers.push_back(controller);
	controller = new CameraController11Class(Client); Controllers.push_back(controller);
	G_ASSERT(Controllers.size() == STATE_LAST_CONTROLLER);
	Controllers[State]->Set_Flag(0);
	Controllers[State]->Unknown_04();
}

// 0x799220
CameraManagerClass::~CameraManagerClass()
{
	if (Module) {
		delete Module;
		Module = NULL;
	}
	for (int i = 0; i < STATE_LAST_CONTROLLER; i++) {
		delete Controllers[i];
	}
	Controllers.clear();
}




// 0x7992F0
void CameraManagerClass::Unknown_7992F0()
{
	if (Client->Unknown_18() && ((SClient *)Client->Unknown_18())->Unknown_750B60()) {
		return;
	}
	if (Client->LoadingView) {
		Client->LoadingView->Unknown_692160();
	}
	int id = Client->Unknown13AC;
	FAIL_IF(id == -1) {return;}
	Client->Send_Packet(&ActionPacketClass(Client->Get_Avatar_ID(), 7, id, 0));
	Client->Unknown13AC = -1;
}


// 0x7993E0
void CameraManagerClass::Unknown_7993E0()
{
	if (Client->Unknown_18() && ((SClient *)Client->Unknown_18())->Unknown_750B60()) {
		return;
	}
	if (Client->LoadingView) {
		Client->LoadingView->Unknown_692160();
	}
	int id = Client->Unknown13BC;
	FAIL_IF(id == -1) {return;}
	Client->Send_Packet(&ActionPacketClass(Client->Get_Avatar_ID(), 19, id, 0));
	Client->Unknown13BC = -1;
}

void CameraManagerClass::Unknown_7994D0()
{
	int id = Client->Unknown13B8;
	FAIL_IF(id == -1) {return;}
	Client->Send_Packet(&ActionPacketClass(Client->Get_Avatar_ID(), 8, id, 0));
	Client->Unknown13B8 = -1;
}

// 0x799590
void CameraManagerClass::Unknown_799590()
{
	Client->Send_Packet(&ActionPacketClass(Client->AvatarID, 27, Client->AvatarID, 0));
}

// 0x799610
void CameraManagerClass::Unknown_799610(const Vector3 &pos, const Quaternion &rot)
{
	if (Unknown28) {
		Unknown20 = 0.005f;
		Unknown24 = 0.005f;
	} else {
		Unknown24 = 0.002f;
		float dist = (Position - pos).Length();
		float d = dist * 0.0004f;
		Unknown20 = (d * 10.0f + 1.0f) * 0.001f;
	}
	Quaternion q = rot;
	q.Make_Closest(Orientation);
	float frame = (float)WW3D::Get_Frame_Time();
	Vector3 old = Position;
	float t = frame * Unknown20;
	if (t < 1.0f) {
		Lerp(old, pos, t, &Position);
	} else {
		Position = pos;
	}
	if (Equal_Within_Epsilon(Orientation, q, 0.0001f)) {
		return;
	}


	t = frame * Unknown24;
	if (t < 1.0f) {
		Slerp(Orientation, Orientation, q, t);
	} else {
		Orientation = q;
	}
}

// 0x7998B0
void CameraManagerClass::Unknown_7998B0(int id)
{
	ClientObjectClass *object = Client->Find_Object(id);
	if (object) {






		FAIL_IF(!Client->MainView) {return;}
		CameraClass *cam = (CameraClass *)Client->MainView->Unknown_7C();
		FAIL_IF(!cam) {return;}
		Matrix3D tm = Build_Matrix3D(Orientation);
		tm.Set_Translation(Position);
		cam->Set_Transform(tm);
		MainViewSceneClass *scene = (MainViewSceneClass *)Client->MainView->Unknown_64();
		OBBoxClass box = cam->Get_Near_Clip_Bounding_Box();
		OBBoxClass moved;
		Vector3 delta = Position - object->Unknown_24();
		Matrix3D shift;
		shift.Make_Identity();
		shift.Set_Translation(-delta);
		OBBoxClass::Transform(shift, box, &moved);
		CastResultStruct result;
		bool ship = OptionClass::Instance->Get_Bool("Camera_Ship");
		scene->Unknown_75FB00(moved, box, delta, &result, object->RenderObject, ship);
		if (result.Fraction < 1.0f) {
			float fraction = result.Fraction;
			float znear, zfar;
			cam->Get_Clip_Planes(znear, zfar);
			float dist = delta.Length();
			float t = (object->Unknown_30() + znear) / dist;
			if (t > 1.0f) {
				t = 1.0f;
			}
			if (fraction < t) {
				fraction = t;
			}
			Position = object->Unknown_24() + delta * fraction;
		}
	}
}













// 0x799DA0
bool CameraManagerClass::Unknown_799DA0(int *new_state, int state)
{
	FAIL_IF(!new_state) {return(false);}
	bool result = true;
	if (State != state && Unknown_79B4F0(state)) {
		*new_state = state;
	} else {
		result = false;
	}
	if (State == 10 && *new_state != -1) {
		EmoteDataClass *emote = Get_Emote_Data(Client->Get_Avatar(), false);
		if (emote) {
			emote->Unknown_7E7700();
		}
	}
	return(result);
}
// NOT EXACT: 0x799E40 (frame order: the client puts target at -0x10 above the 12-byte block locals and Sqrt's float above the avatar position temporary, ours the reverse; bit 15's Length2 keeps Y and Z on the FPU in the client)
// 0x799E40
void CameraManagerClass::Unknown_799E40()
{
	int new_state = -1;
	bool ease = false;
	int mode;
	int target;
	if (Unknown_79B330(0)) {
		if (!Unknown_799DA0(&new_state, 4)) {
			Unknown_799DA0(&new_state, 0);
			Controllers[0]->Set_Flag(0);
		}
	} else if (Unknown_79B330(1)) {
		Unknown_799DA0(&new_state, 0);
	} else if (Unknown_79B330(2)) {
		NavigationListClass &path = (NavigationListClass &)Client->MainView->Radar->Unknown_7BD960();
		bool warp = true;
		if (path.Is_Last()) {
			ClientObjectClass *dest = Client->Find_Object(path.Get_Current());
			if (dest) {
				Vector3 dest_pos = dest->Unknown_24();
				float dist = (dest_pos - Client->Get_Avatar()->Unknown_24()).Length();
				if (dist - (dest->Unknown_30() + 75.0f) < 15000.0f) {
					warp = false;
				}
			}
		}
		if (warp) {
			Unknown_799DA0(&new_state, 2);
		}
	} else if (Unknown_79B330(3)) {
		if (State == 2) {
			Controllers[2]->Set_Flag(0);
		}
	} else if (Unknown_79B330(4)) {
		if (State == 2) {
			Controllers[2]->Set_Flag(1);
		}
	} else if (Unknown_79B330(5)) {
		if (State == 2) {
			ease = true;
			Unknown_799DA0(&new_state, 1);
		}
	} else if (Unknown_79B330(6)) {
		if (!Client->Unknown_74D730() && State != 3) {
			if (State != 0) {
				if (!Unknown_799DA0(&new_state, 3)) {
					Unknown_799DA0(&new_state, 0);
					Unknown_7993E0();
				}
			}
		} else {
			Unknown_799DA0(&new_state, 0);
			Unknown_7993E0();
		}
	} else if (Unknown_79B330(24)) {
		Unknown_799DA0(&new_state, 0);
		Unknown_7993E0();
	} else if (Unknown_79B330(7)) {
		Unknown_799DA0(&new_state, 1);
	} else if (Unknown_79B330(8)) {
		if (!Client->Unknown_74D730() && State != 5) {
			if (State != 0) {
				if (!Unknown_799DA0(&new_state, 5)) {
					Unknown_799DA0(&new_state, 0);
					Unknown_7994D0();
				}
			}
		} else {
			Unknown_799DA0(&new_state, 0);
			Unknown_7994D0();
		}
	} else if (Unknown_79B330(9)) {
		Unknown_799DA0(&new_state, 0);
		Unknown_7994D0();
	} else if (Unknown_79B330(10)) {
		if (!Client->Unknown_74D730() && State != 6) {
			if (State != 0) {
				if (Unknown_799DA0(&new_state, 6)) {
					Controllers[6]->Set_Flag(0);
				} else {
					Unknown_799DA0(&new_state, 0);
					Unknown_7992F0();
				}
			}
		} else {
			Unknown_799DA0(&new_state, 0);
			Unknown_7992F0();
		}
	} else if (Unknown_79B330(11)) {
		Unknown_799DA0(&new_state, 0);
		Unknown_7992F0();
	} else if (Unknown_79B330(12)) {
		if (Unknown_799DA0(&new_state, 6)) {
			Controllers[6]->Set_Flag(1);
		} else {
			Unknown_799DA0(&new_state, 0);
			Controllers[0]->Set_Flag(0);
		}
	} else if (Unknown_79B330(13)) {
		if (!Unknown_799DA0(&new_state, 7)) {
			Unknown_799DA0(&new_state, 0);
			Controllers[0]->Set_Flag(0);
		}
	} else if (Unknown_79B330(14)) {
		Unknown_799DA0(&new_state, 1);
	} else if (Unknown_79B330(15)) {
		if (!Client->Unknown_74D730()) {
			ClientObjectClass *target = Client->Find_Object(Client->Unknown13B4);
			ClientObjectClass *avatar = Client->Get_Avatar();
			if (target && avatar) {
				Vector3 target_pos = target->Unknown_24(); Vector3 diff = target_pos - avatar->Unknown_24();
				float radius = target->Unknown_30();
				if (diff.Length2() > (radius * radius + 5625.0f) * 2.0f) {
					Unknown_799DA0(&new_state, 8);
				}
			}
		}
	} else if (Unknown_79B330(16)) {
		ease = true;
		Unknown_799DA0(&new_state, 1);
	} else if (Unknown_79B330(17)) {
		if (State != 0 && !Unknown_799DA0(&new_state, 9)) {
			Unknown_799DA0(&new_state, 0);
			Unknown_799590();
		}
	} else if (Unknown_79B330(18)) {
		Unknown_799DA0(&new_state, 0);
		Unknown_799590();
	} else if (Unknown_79B330(19)) {
		Unknown_799DA0(&new_state, 1);
	} else if (Unknown_79B330(22)) {
		Unknown_799DA0(&new_state, 1);
	} else if (Unknown_79B330(20)) {
		if (State == 1) {
			Unknown_799DA0(&new_state, 10);
		}
	} else if (Unknown_79B330(21)) {
		if (State == 10) {
			Unknown_799DA0(&new_state, 1);
			ease = true;
		}
	} else if (Unknown_79B330(23)) {
		Unknown_799DA0(&new_state, 11);
	} else if (Unknown_79B330(25)) {
		Unknown_799DA0(&new_state, 1);
	}
























































































	if (new_state != -1) {
		FAIL_IF(new_state == State) {return;}
		Controllers[State]->Unknown_0C();
		State = new_state;
		Controllers[State]->Unknown_04();
	}
	Vector3 pos = Position;
	Quaternion rot = Orientation;
	mode = 2;
	target = -1;
	Controllers[State]->Unknown_08(&pos, &rot, &mode, &target);
	if (ease) {
		mode = 1;
	}
	if ((Position - pos).Length2() > 6250000.0f) {
		mode = 2;
	}
	if (!Unknown_79B4F0(2) && Client->Get_Avatar() && Client->Get_Avatar()->Unknown_6FC970()) {
		mode = 2;
	}
	switch (mode) {
		case 1:
			Unknown_799610(pos, rot);
			break;
		case 2:
			Position = pos;
			Orientation = rot;
			break;
	}
	if (target != -1) {
		Unknown_7998B0(target);
	}
	Matrix3D tm(Orientation, Position);













	CameraClass *cam = (CameraClass *)Client->MainView->Unknown_7C();
	FAIL_IF(!cam) {return;}
	cam->Set_Transform(tm);
	cam->Set_View_Plane(FOV, -1.0f);
}









































































// 0x79B2B0
void CameraManagerClass::Unknown_79B2B0(int request)
{
	switch (request) {
		case 0:
		case 1:
		case 12:
		case 13:
			Requests &= ~((1 << 0) | (1 << 1) | (1 << 12) | (1 << 13));
			break;
		case 2:
		case 3:
		case 4:
			Requests &= ~((1 << 2) | (1 << 3) | (1 << 4) | (1 << 5));
			break;
	}
	Requests |= 1 << request;
}

// 0x79B330
bool CameraManagerClass::Unknown_79B330(int request)
{
	bool set = (Requests & (1 << request)) != 0;
	Requests &= ~(1 << request);

	return(set);
}

void CameraManagerClass::Set_Heading(float heading) { Module->Set_Heading(heading); }				// 0x79B370
float CameraManagerClass::Get_Heading() { return(Module->Heading); }								// 0x79B390
void CameraManagerClass::Set_Distance(float distance) { Module->Set_Distance(distance); }			// 0x79B3B0
float CameraManagerClass::Get_Distance() { return(Module->Distance); }								// 0x79B3D0
void CameraManagerClass::Set_Tilt(float tilt) { Module->Set_Tilt(tilt); }							// 0x79B3F0
float CameraManagerClass::Get_Tilt() { return(Module->Tilt); }										// 0x79B410
void CameraManagerClass::Unknown_79B430(bool arg) { Module->Unknown_79BE90(arg); }					// 0x79B430
bool CameraManagerClass::Unknown_79B450() { return(Module->Unknown14); }							// 0x79B450
void CameraManagerClass::Set_Elevation_Offset(float offset) { Module->Set_Elevation_Offset(offset); }	// 0x79B470
float CameraManagerClass::Get_Elevation_Offset() { return(Module->ElevationOffset); }				// 0x79B490
void CameraManagerClass::Unknown_79B4B0(bool first_mode) { Module->Unknown_79BEF0(first_mode); }	// 0x79B4B0
bool CameraManagerClass::Unknown_79B4D0() { return(Module->Unknown1C); }							// 0x79B4D0
#pragma auto_inline(off)
// NOT EXACT: 0x79B4F0 (the default arm: the client keeps true in bl across the assert, so its return is not merged with case 0's mov al,1)
bool CameraManagerClass::Unknown_79B4F0(int state)
{
	switch (state) {
		case 2:
			return(OptionClass::Instance->Get_Bool("Camera_Warp"));
		case 3:
			return(OptionClass::Instance->Get_Bool("Camera_Gate"));
		case 5:
		case 7:
		case 9:
			return(OptionClass::Instance->Get_Bool("Camera_Land"));
		case 8:
			return(OptionClass::Instance->Get_Bool("Camera_Loot"));
		case 6:
			return(OptionClass::Instance->Get_Bool("Camera_Dock"));
		case 0:
		case 1:
		case 10:
		case 11:
			return(true);
		case 4:
			return(false);
		default:
			G_ASSERT(0);
			return(true);
	}
}
#pragma auto_inline(on)
// 0x79B600
bool CameraManagerClass::Unknown_79B600()
{
	if (State == 5 || State == 6 || State == 7 || State == 9) {
		return(true);
	}
	return(false);
}

bool CameraManagerClass::Unknown_79B630() { return(State == 6 ? true : false); }					// 0x79B630
bool CameraManagerClass::Unknown_79B650() { return(State == 3 ? true : false); }					// 0x79B650
bool CameraManagerClass::Unknown_79B670() { return(State == 1 ? true : false); }					// 0x79B670
bool CameraManagerClass::Unknown_79B690() { return(Controllers[State]->Unknown_14()); }	// 0x79B690
float CameraManagerClass::Unknown_79B6B0() { return(75.0f); }						// 0x79B6B0

/*
** Reconstruction notes (CM1, 2026-11-07)
**
** The object: camera_manager.cpp's __FILE__ 0xB9E5C0 (asserts on lines 76-677). It starts at the constructor 0x798D20,
** after the camctrl object 0x798AE0-0x798D0F ends with <string>'s initialiser 0x798CF0, and ends with its own at 0x79B9B0,
** after std::vector<CameraControllerClass *>'s destructor 0x79B6D0 (the constructor's and destructor's unwinding) and
** insert(it, n, x) 0x79B710. 0x79B9E0 onward is the next object. asserts.txt gives the constructor's assert string
** 0xB9E58C to camctrl_tractor.cpp (the preceding __FILE__): it is this file's (0x7990D4 pushes it with 0xB9E5C0).
**
** 0x7998B0 (lines 188 / 190) and 0x799E40 (lines 237 x 23, 489, 535: it inlines 0x799DA0) rebuilt NOT EXACT (CS1,
** 2026-11-07). 0x799E40's inlining: 0x79B330 inline for its first 17 requests and 0x799DA0 for 23 sites only with
** 0x79B330 written without a bit local (with it, the requests 15 and 16 lose theirs to the budget); the 'bit' local
** form is exact out of line too. 0x7998B0's shifted box needs Set_Translation (Matrix3D(-delta) copies through the FPU).
*/
