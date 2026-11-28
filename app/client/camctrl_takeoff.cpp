// Earth & Beyond client (reconstruction): app\client\camctrl_takeoff.cpp, the take-off camera (state 9; notes at the end)
#include "camctrl_takeoff.h"
#include "eff_o.h"
#include "camera_manager.h"
#include "sclient.h"
#include "client.h"
#include "co.h"
#include "postate.h"
#include "random.h"
#include "obbox.h"
#include "castres.h"
#include "scene.h"
#include "aabox.h"
#include "matrix3d.h"
#include "wwmath.h"
#include "..\shared\g_assert.h"
#include "..\shared\auth_product.h"
extern Random4Class Unknown_BDB1C0;				// NAME 1 [Unknown_BDB1C0]: 0xBDB1C0, the camera controllers' generator outside this file

class ENBSceneClass : public SimpleSceneClass  // NAME 5: thunk order, binary.md 5.11
{
public:
	void Unknown_75FB00(const OBBoxClass &to, const OBBoxClass &from, const Vector3 &move, CastResultStruct *result, RenderObjClass *ignore, bool ship);
};

// @0x7961B0
TakeoffCameraControllerClass::TakeoffCameraControllerClass(SpaceClientClass *client) :
	CameraControllerClass(client),
	Unknown0C(NULL),
	Unknown10(0),
	Unknown14(0)
{
	Unknown0C = new CameraModule79C8E0Class(Client);
}

// @0x796280
TakeoffCameraControllerClass::~TakeoffCameraControllerClass()
{
	if (Unknown0C) {
		delete Unknown0C;
		Unknown0C = NULL;
	}
}

// @0x796300
void TakeoffCameraControllerClass::Unknown_04()
{
	Unknown_796920();
	Unknown5C = false;
	Unknown10 = Client->Unknown10BC;
	Unknown14 = Client->Next_Local_ID();
	EffectClass *effect = Unknown_6E9490(0x170, Unknown14, 5000, Unknown10, Client, 1.0f, Vector3(0.0f, 0.0f, 0.0f), 1.0f, -1, 0);





















































	FAIL_IF(!effect) return;

	UnaryEffectClass *source = effect->Unknown_10();
	if (source) {
		source->Set_Source_Object_ID(Client->AvatarID);
	} else {
		G_ASSERT(0);
	}
	MainViewClass *view = Client->MainView;
	Client->Add_Effect(effect, (int)view->Unknown_64(), view->Unknown_7C());
}

// @0x796440
// (one *mode = 2 at the end: the client's three byte-identical tails are the compiler's copies of it; written out in each phase they took eax / ecx / edx and edi was pushed in the prologue. Each phase has its own tm, so the two Unknown_30 calls do not merge: 17 calls)
void TakeoffCameraControllerClass::Unknown_08(Vector3 *pos, Quaternion *rot, int *mode, int *target)
{
	ClientObjectClass *ship = Client->Get_Avatar();
	float distance = CameraManagerClass::Unknown_79B6B0();
	FAIL_IF(!ship) {
		return;
	}
	Unknown0C->Unknown_08(pos, rot);
	unsigned int elapsed = Client->Unknown10BC - Unknown10;
	if (elapsed < 2000) {
		float t = (float)elapsed * 0.0005f;
		Quaternion q;
		Slerp(q, Unknown3C, Unknown4C, t * t);
		Matrix3D tm = Build_Matrix3D(q);
		Vector3 p = Lerp(Unknown24, Unknown30, t);
		tm.Set_Translation(p);
		if (ship->PositionalState->Get_Type() == PositionalStateClass::CLIENT_SIDE_PROCEDURAL) {
			((Unknown718D70Class *)ship->PositionalState)->Unknown_30(Client->Unknown10BC, tm);


		}
	} else if (elapsed < 5000) { Matrix3D tm;
		if (!Unknown5C) {
			Unknown5C = true;
			
			tm.Obj_Look_At(Unknown30, Unknown18, 0.0f);
			float angle = (Unknown_BDB1C0.Get_Float() - 0.5f) * 30.0f; angle *= (WWMATH_PI / 180.0f);
			switch (Unknown_BDB1C0(0, 1)) { case 1:
				angle = WWMATH_PI - angle;
			}
			float dist = (Unknown_BDB1C0.Get_Float() * 7.0f + 10.0f) * distance;
			float s = WWMath::Sin(angle);
			float c = WWMath::Cos(angle); c *= 1.5f; c *= distance; s *= 1.5f; s *= distance;
			Vector3 offset(dist, c, s); Unknown0C->Position = tm * offset;
			Unknown0C->Unknown_04();
		}
		float t = (float)(elapsed - 2000) * 0.00033333333f;
		tm = Build_Matrix3D(Unknown4C);
		Vector3 p; Lerp(Unknown30, Unknown18, t, &p);
		tm.Set_Translation(p);
		if (ship->PositionalState->Get_Type() == PositionalStateClass::CLIENT_SIDE_PROCEDURAL) {
			((Unknown718D70Class *)ship->PositionalState)->Unknown_30(Client->Unknown10BC, tm);


		}
	} else {
		Client->CameraManager->Unknown_79B2B0(0x12);
	}
	*mode = 2;
}















// @0x796850
void TakeoffCameraControllerClass::Unknown_0C()
{
	Unknown0C->Unknown_0C();
	ClientObjectClass *ship = Client->Get_Avatar();
	FAIL_IF(!ship) {
		return;
	}
	PositionalStateClass *state = ship->PositionalState;
	if (state && state->Unknown_2C()) {
		ship->PositionalState = state->Unknown_28();
		delete state;
	}
	EffectClass *effect = Client->Get_Effect(Unknown14);
	if (effect) {
		effect->Unknown_6E6E30();
	}
}

// @0x7968F0
bool TakeoffCameraControllerClass::Unknown_14()
{
	return(Client->Unknown10BC - Unknown10 > 2000);
}
// @0x796920
// NOT EXACT: shape: the retry counter's slot, loop entry and scheduling; every call, literal and member offset is the client's; functional
void TakeoffCameraControllerClass::Unknown_796920()
{
	ClientObjectClass *ship = Client->Get_Avatar();
	FAIL_IF(!ship) {
		return;
	}
	RenderObjClass *ship_ro = ship->RenderObject;
	FAIL_IF(!ship_ro) {
		return;
	}

	Unknown24 = ship->Unknown_24();
	Unknown30 = Vector3(Unknown24.X, Unknown24.Y, (Unknown_BDB1C0.Get_Float() * 500.0f + 100.0f) + Unknown24.Z);
	Unknown3C = Build_Quaternion(ship->Unknown_28(true));

	int i = 0; while (true) {
		if (i >= 10) {
			Unknown18 = Vector3(Unknown30.X, Unknown30.Y, Unknown30.Z + 5000.0f);
			break;
		}
		++i; float pitch = Unknown_BDB1C0.Get_Float() * 30.0f; pitch += 30.0f; pitch *= (WWMATH_PI / 180.0f);
		float yaw = (Unknown_BDB1C0.Get_Float() * 315.0f + 45.0f) * (WWMATH_PI / 180.0f);
		yaw += ship->Unknown_28(true).Get_Z_Rotation();
		float c_p = WWMath::Cos(pitch);
		float s_p = WWMath::Sin(pitch);
		Matrix3D rotation(true); rotation.Rotate_Z(yaw);
		Vector3 dir = rotation * Vector3(c_p, 0.0f, s_p);
		Vector3 offset = dir * 5000.0f;
		Unknown18 = Unknown30 + offset;

		AABoxClass model_box; ship_ro->Get_Obj_Space_Bounding_Box(model_box); // NAME 2: 0x796b98
		OBBoxClass local_box(Vector3(0.0f, 0.0f, 0.0f), model_box.Extent); // NAME 2: identity basis and model box, 0x796bd8
		OBBoxClass start_box, end_box; // NAME 2: 0x796da1 and 0x796e42
		rotation.Make_Identity(); rotation = ship->Unknown_28(true); // NAME 2: 0x796cc2
		rotation.Set_Translation(Unknown30);
		OBBoxClass::Transform(rotation, local_box, &start_box);
		rotation.Set_Translation(Unknown18);
		OBBoxClass::Transform(rotation, local_box, &end_box);
		CastResultStruct result;
		((ENBSceneClass *)Client->MainView->Unknown_64())->Unknown_75FB00(start_box, end_box, dir * 1000000.0f, &result, ship_ro, false);
		if (result.Fraction >= 1.0f) {
			break;
		}
	}

	Matrix3D lookat;
	lookat.Obj_Look_At(Unknown30, Unknown18, 0.0f);
	Unknown4C = Build_Quaternion(lookat);

	float distance = CameraManagerClass::Unknown_79B6B0();
	float angle = Unknown_BDB1C0.Get_Float() * 360.0f; angle *= (WWMATH_PI / 180.0f);


	lookat.Obj_Look_At((Unknown24 + Unknown30) * 0.5f, Unknown30, 0.0f);
	float s = WWMath::Sin(angle);
	float c = WWMath::Cos(angle); s = s * distance * 1.5f; c = c * distance * 1.5f;
	Vector3 camera_position = lookat * Vector3(0.0f, c, s); { CameraModule79C8E0Class *module = Unknown0C; module->Position.X = camera_position.X; module->Position.Y = camera_position.Y; module->Position.Z = camera_position.Z; }
	Unknown0C->Unknown14 = true;
	Unknown0C->Unknown18 = Client->AvatarID;
	Unknown0C->Unknown_04();

	ship->PositionalState = new Unknown718D70Class(Client->Unknown10BC, ship->Unknown_28(true), ship->PositionalState);
}

/*
** Reconstruction notes (CC1, 2026-11-07)
**
** The object 0x7961A0-0x79730F: after camctrl_stargate.cpp's <string> initialiser 0x796180, ending with its own at
** 0x797300. Its own __FILE__ 0xB9E4BC (e:\build\beta\G\app\Client\camctrl_takeoff.cpp, FAIL_IFs l.106 / 190, G_ASSERT
** l.112; 0x796440 l.124, 0x796920 l.214, 218): the start and the stop have land's shape (effect 0x170 for five seconds).
*/
