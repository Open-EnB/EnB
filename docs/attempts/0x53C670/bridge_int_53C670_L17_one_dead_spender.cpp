// Earth & Beyond client (reconstruction): bridge_int.cpp, the ship's bridge around the first-person camera (notes at the end)
#include <stdio.h>
#include <math.h>
#include <string>
#define __BRIDGE_INT_MEMBERS	// bridge_int.h's enums and the functions sclient.cpp does not call
#include "bridge_int.h"
#include "sclient.h"
#include "ui_mesh.h"
#include "avatar.h"
#include "co.h"
#include "gadget.h"
#include "option.h"
#include "unknown_signalbase.h"
#include "random.h"
#include "wwmath.h"
#include "..\shared\avatar_desc.h"
#include "..\shared\g_assert.h"
#include "..\..\ww3d2\rendobj.h"
#include "..\..\ww3d2\camera.h"
#include "..\..\ww3d2\scene.h"

extern RandomClass Unknown_BDBB84;	// NAME 1 [Unknown_BDBB84]: neutral (D10), as avatar.cpp's

// 0xB78CD4: the mesh's camera bone for each view (a fifth, empty entry follows the four)
static const char *CameraBoneNames[BridgeInterface::NUM_VIEWS + 1] = {	// NAME 1: what it holds
	"cam_main",
	"cam_left",
	"cam_right",
	"cam_front",
	NULL
};

// 0xB78CE8: the mesh's animation that carries the camera from one view's bone to another's, [from + to * NUM_VIEWS]
// (the model's name goes before it: 't_i_bridge.t_i_bridge-ml'), and whether it plays backwards
struct BridgeTransitionStruct	// NAME 1: what it is
{
	const char *AnimName;		// NAME 3: the FAIL_IF's '!anim.AnimName'
	bool Reverse;				// NAME 1: what 0x5B8EE0 does with it
};
static BridgeTransitionStruct Transitions[BridgeInterface::NUM_VIEWS * BridgeInterface::NUM_VIEWS] = {	// NAME 1: what it holds
	{ NULL, false }, { "-ml", true }, { "-mr", true }, { "-mf", true },
	{ "-ml", false }, { NULL, false }, { "-rl", false }, { "-lf", true },
	{ "-mr", false }, { "-rl", true }, { NULL, false }, { "-rf", true },
	{ "-mf", false }, { "-lf", false }, { "-rf", false }, { NULL, false }
};

static float HitScale = 4.0f;	// 0xB78D68: a hit's direction to the camera's offset  // NAME 1: what it is

static Matrix3D BridgeTransform;	// 0xBE5AA8: the bridge model's orientation in its scene (the constructor sets it)  // NAME 1: what it is

// NOT EXACT: order (explain.py): the client stores the listener's table before +0x04, ours after (ui_scut.h's base sets
// +0x04 in its initialiser list; the client's order is a base without the member and Client as this class's first)
// @0x53B040
BridgeInterface::BridgeInterface(ClientClass *client) :
	UnknownAEEA88Class(client),
	Mesh(NULL),
	BridgeName(NULL),
	View(VIEW_MAIN),
	ViewReached(VIEW_MAIN),
	InTransition(false),
	CameraBone(0),
	CameraTransform(true),
	HitOffset(0.0f, 0.0f, 0.0f),
	Turn(0.0f, 0.0f, 0.0f),
	Pitch(0.0f),
	Yaw(0.0f),
	Looking(false),
	Unknown79(false),
	Animation(ANIM_NONE)
{
	Mesh = new UIMeshClass(NULL, 0);
	Mesh->Observers.Add((ObserverClass *)this);
	BridgeTransform.Make_Identity();
	BridgeTransform.Rotate_X(DEG_TO_RADF(-90.0f));
	BridgeTransform.Rotate_Z(DEG_TO_RADF(90.0f));
}

// @0x53B380
BridgeInterface::~BridgeInterface()
{
	Set_Scene(RefCountPtr<RefCountClass>());
	UnknownAEEA88Class::Unknown_04((DynamicVectorClass<UnknownAEEA88Class *> *)&Mesh->Observers);
	delete Mesh;
	Mesh = NULL;
	Avatar = NULL;
}

// The mesh's processes report here: a transition started (1: the camera rides the main bone while it plays), ended (3:
// the camera is at the view asked for), anything else clears the mesh's flag bit 0 (Set_View sets it)
// @0x53B500
void BridgeInterface::Unknown_00(void *source, int *event)
{
	if (source != (SubjectClass *)Mesh) return;
	if (event[1] == 1) {
		InTransition = true;
		Update_Camera_Bone();
	} else if (event[1] == 3) {
		InTransition = false;
		ViewReached = View;
		Update_Camera_Bone();
	} else {
		Mesh->Unknown70 &= ~1;
	}
}

// @0x53B670
void BridgeInterface::Get_Hit(const Vector3 &hit)
{
	if (Scene) {
		HitOffset = hit * HitScale;
		HitOffset.Z = 0.0f;
		if (hit.X != 0.0f || hit.Y != 0.0f) {
			Play_Animation(ANIM_GET_HIT, 1.0f, 0, -1);
		}
	}
}

// @0x53B710
void BridgeInterface::Start_Warp()
{
	Play_Animation(ANIM_START_WARP, 1.0f, 0, -1);
}

// @0x53B740
void BridgeInterface::Stop_Hard()
{
	Play_Animation(ANIM_STOP_HARD, 1.0f, 0, -1);
}

// @0x53B770
void BridgeInterface::Lose_Control()
{
	Play_Animation(ANIM_LOSE_CONTROL, 1.0f, 0, -1);
}

//PAD
// The player's own avatar arrived: the race's bridge model, and a second avatar of that description (personality 8, or
// 9 when its gender is not 0) for the chair. The bridge leaves its scene meanwhile.
// @0x53B7A0
void BridgeInterface::Set_Avatar(AvatarDescClass *desc)
{
	RefCountPtr<RefCountClass> scene = Scene;
	if (Scene) {
		Set_Scene(RefCountPtr<RefCountClass>());
	}
	switch (desc->Race) {
		case 1:
			BridgeName = "j_i_bridge";
			break;
		case 2:
			BridgeName = "p_i_bridge";
			break;
		default:
#line 221
			G_ASSERT(false);
		case 0:
			BridgeName = "t_i_bridge";
			break;
	}
	Mesh->Unknown_5B8850(BridgeName, 1);
	if (Mesh->InterfaceMesh) {
		AvatarDescClass pilot(*desc);
		pilot.Personality = pilot.Gender ? 9 : 8;
		Avatar = Create_NEW(new CharacterViewAvatarClass(&pilot, false, false, NULL));
		Avatar->Unknown_77E610();
		EnvironmentMapManagerClass *env_maps = ((SClient *)Peek_Client()->Unknown_18())->EnvironmentMapManager;
		Avatar->Unknown_780810(env_maps);
		Avatar->Unknown_77E3D0(1);
		Avatar->Unknown215 = true;
		Avatar->Unknown_1C4();
	} else {
//PAD
#line 250
		G_ASSERT(false);
		Avatar = NULL;
	}
	if (scene) {
		Set_Scene(scene);
	}
	Update_Camera_Bone();
}

// The scene the bridge is drawn in (the main view's, while the camera is inside the ship; NULL takes the bridge out).
// The mesh and the avatar leave the old scene; in the new one the mesh takes the bridge's orientation and the avatar
// sits on its 'main_chair' bone. The gadgets' flashes are listened to only while in a scene, and the race's bridge
// sounds play only there.
// @0x53BBC0
void BridgeInterface::Set_Scene(const RefCountPtr<RefCountClass> &scene)
{
	if (Scene.Peek() == scene.Peek()) return;
	RenderObjClass *bridge = Mesh->InterfaceMesh;
	if (Scene) {
		if (Avatar) {
			Avatar->Remove();
		}
		if (bridge) {
			Mesh->Unknown_5B92B0();
		}
		GadgetClass::Get_Signal_Map().disconnect((GadgetClass *)NULL, this, &BridgeInterface::On_Gadget_Flash, FlashingGadgetEvent(NULL));
	}
	Scene = scene;
	if (Scene && bridge) {
		Mesh->Unknown_5B9290(Peek_Scene());
		bridge->Set_Transform(BridgeTransform);
		if (Avatar) {
			bridge->Add_Sub_Object_To_Bone(Avatar.Peek(), "main_chair");
		}
		GadgetClass::Get_Signal_Map().Add((GadgetClass *)NULL, this, &BridgeInterface::On_Gadget_Flash, FlashingGadgetEvent(NULL));
	}
	Mesh->Unknown_5B9930(-1);
	ViewReached = View;
	InTransition = false;
	if (Scene) {
		if (Avatar && Peek_Client()) {
			Sounds.Play_Sounds(Avatar->Desc.Race, Peek_Client()->Unknown10BC);
		}
	} else {
		Sounds.Unknown_437070();
	}
}

//PAD
// One frame, while the bridge is in a scene. The camera goes onto the view's bone of the mesh (in the main view turned
// by Yaw and Pitch, which decay unless the pointer set them this frame) and is moved by the last hit's offset; the
// bridge model leans by the ship's turn rates, smoothed.
// NOT EXACT: order (explain.py; the same length, 1866 bytes): the client multiplies the rows' Y terms of the
// translation first and keeps the second and third rotations' sine and cosine in the other two stack slots
// @0x53BD40
void BridgeInterface::Do_Frame(CameraClass *camera)
{
#line 315
	FAIL_IF(!camera) return;
	RenderObjClass *bridge = Mesh->InterfaceMesh;

#line 318
	FAIL_IF(!bridge) return;
	bridge->Set_Transform(BridgeTransform);
	bridge->Set_Sub_Object_Transforms_Dirty(true);
	Mesh->Unknown_5B9410();
	Update_Avatar();
//PAD
	ClientObjectClass *pship = Peek_Client()->Get_Ship();
#line 332
	FAIL_IF(!pship) return;

	// the camera, relative to the main view's
	CameraTransform = bridge->Get_Bone_Transform(CameraBone);
	if (View == VIEW_MAIN) {
		CameraTransform.Pre_Rotate_Y(Yaw);
		CameraTransform.Pre_Rotate_X(Pitch);
	}
	CameraTransform.Translate(HitOffset);
	if (Looking) {
		Looking = false;
	} else {
		Yaw *= 0.85f;
		Pitch *= 0.85f;
	}
	Matrix3D tm = camera->Get_Transform();
	Matrix3D view;
	Matrix3D::Multiply(tm, CameraTransform, &view);
	camera->Set_Transform(view);

	// the bridge leans with the ship's turn (not while the PDA is open)
	Vector3 turn = pship->Unknown98->Unknown_14(Peek_Client()->Unknown10BC);
	if (View == VIEW_MAIN) {
		turn *= 50.0f;
	} else {
		turn.Set(0.0f, 0.0f, 0.0f);
	}
	Vector3 smooth = Lerp(Turn, turn, 0.2f);
	Turn = smooth;
	Matrix3D lean(true);
	lean.Pre_Rotate_X(smooth.Y);
	lean.Pre_Rotate_Y(-smooth.Z);
	lean.Pre_Rotate_Z(smooth.Z);
	lean.Rotate_X(DEG_TO_RADF(-90.0f));
	lean.Rotate_Z(DEG_TO_RADF(90.0f));
	bridge->Set_Transform(lean);
//PAD
	ClientClass *Client = Peek_Client();	// NAME 3: the FAIL_IF's (the listener's +0x04)
#line 404
	FAIL_IF(Client == NULL) return;
	Sounds.Stop_Sound(Client->Unknown10BC);
}

// NOT EXACT: register, inline budget (explain.py): the client keeps &anim in ebx (saved on entry, so its early returns share one epilogue) and destroys the first temporary string through _Tidy; ours spills anim and expands that _Tidy inline
// @0x53C670
bool BridgeInterface::Set_View(int view)
{
	if (ViewReached != View) return(false);
	if (View == view) return(false);
	if (Scene && OptionClass::Instance->Get_Bool("Camera_Bridge")) {
		BridgeTransitionStruct &anim = Transitions[View + view * NUM_VIEWS];
//PAD
#line 416
		FAIL_IF(!anim.AnimName) return(false);
		std::string name = std::string(BridgeName) + "." + BridgeName + anim.AnimName;
		Mesh->Unknown70 |= 1;
		Mesh->Unknown_5B8EE0(0, view, name.c_str(), false, 1, anim.Reverse, 0, -1);
		name.size();
	} else {
		View = view;
		ViewReached = view;
		Update_Camera_Bone();
	}
	View = view;
	return(true);
}

// @0x53C980
void BridgeInterface::Set_Pitch(float pitch)
{
	if (View == ViewReached && View == VIEW_MAIN) {
		Pitch = WWMath::Clamp(pitch, -0.2f, 0.4f);
		Looking = true;
	}
}

// @0x53CA00
void BridgeInterface::Set_Yaw(float yaw)
{
	if (View == ViewReached && View == VIEW_MAIN) {
		Yaw = WWMath::Clamp(yaw, -0.25f, 0.25f);
		Looking = true;
	}
}

//PAD
// The bone the camera sits on: the view's, or the main one while a transition plays (its animation moves that bone)
// and always with the option 'Camera_Bridge' off. The client inlines it into Unknown_00, Set_Avatar and Set_View.
// @0x53CA80
void BridgeInterface::Update_Camera_Bone()
{
	RenderObjClass *bridge = Mesh->InterfaceMesh;
#line 462
	FAIL_IF(!bridge) return;
	if (OptionClass::Instance->Get_Bool("Camera_Bridge")) {
		if (InTransition) {
			CameraBone = bridge->Get_Bone_Index("cam_main");
		} else {
			CameraBone = bridge->Get_Bone_Index(CameraBoneNames[View]);
		}
	} else {
		CameraBone = bridge->Get_Bone_Index("cam_main");
	}
}

// The avatar's frame: a finished animation is forgotten; while the ship rolls the avatar leans into the turn, the
// animation's frame following the turn's strength, and straightens when it stops.
// NOT EXACT: shape (explain.py): the same instructions in another block order (the client puts the compare with
// Animation right after the left turn's assignment and the right turn's test after it, the frame code last)
// @0x53CB40
void BridgeInterface::Update_Avatar()
{
	Avatar->Unknown_1C4();
	if (Animation != ANIM_NONE && !Avatar->Unknown_789D40()) {
		Animation = ANIM_NONE;
	}
	int anim;
	if (Turn.Z > 0.0001f) {
		anim = ANIM_LEFT_TURN_SMALL;
	} else if (Turn.Z < -0.0001f) {
		anim = ANIM_RIGHT_TURN_SMALL;
	} else {
		if (Animation == ANIM_LEFT_TURN_SMALL || Animation == ANIM_RIGHT_TURN_SMALL) {
			Avatar->Unknown_789C10(0.0f, 0);
		}
		return;
	}
	if (anim != Animation) {
		Play_Animation(anim, 0.0f, 2, -1);
		return;
	}
	if (Animation == ANIM_LEFT_TURN_SMALL || Animation == ANIM_RIGHT_TURN_SMALL) {
		float frame = (float)fabs(Turn.Z) + Turn.Y * 0.5f;
		if (0.0f > frame) frame = 0.0f;
		Avatar->Unknown_789C10(WWMath::Clamp(frame * 5.7295775f, 0.0f, 1.0f), 2);
	}
}

//PAD
// A gadget flashed (the signal map's callback, for every sender): the avatar works the controls when the gadget under
// the pointer has a flash
// @0x53CCD0
void BridgeInterface::On_Gadget_Flash(FlashingGadgetEvent *event, GadgetClass *sender)
{
	if (Scene) {
#line 521
		FAIL_IF(!event) return;

		GadgetClass *gadget = event->Gadget;

#line 525
		FAIL_IF(!gadget) return;
		if (gadget->Flash && gadget->Is_Unknown2C_02()) {
			Play_Animation(ANIM_USE_CONTROLS, 1.0f, 0, -1);
		}
	}
}

// The tag's body animation onto the avatar as its gesture, unless it plays already or one of a lower rank number
// does. 'get_hit' and 'use_controls' have four numbered variants (variant 1..4, else a random one).
// NOT EXACT: shape (explain.py): the same instructions in another block order (the client's numbered code follows
// 'get_hit' and runs into the call, 'use_controls' jumps back to it; the goto is the nearest form, not the client's)
// @0x53CD60
void BridgeInterface::Play_Animation(int anim, float frame, int flags, int variant)
{
	if (!Avatar) return;
	const char *name = NULL;
	if (anim == Animation) return;
	if ((anim & ~0xFFFF) > (Animation & ~0xFFFF)) return;
	const char *base;
	char buffer[64];
	switch (anim) {
		case ANIM_STOP_HARD:
			name = "stop_hard";
			break;
		case ANIM_LOSE_CONTROL:
			name = "lose_control";
			break;
		case ANIM_START_WARP:
			name = "start_warp";
			break;
		case ANIM_GET_HIT:
			base = "get_hit";
		numbered:
			if (variant < 1 || variant > 4) variant = Unknown_BDBB84(1, 4);
			sprintf(buffer, "%s%d", base, variant);
			name = buffer;
			break;
		case ANIM_LEFT_TURN_SMALL:
			name = "left_turn_small";
			break;
		case ANIM_NONE:
			Stop_Animation();
			return;
		case ANIM_USE_CONTROLS:
			base = "use_controls";
			goto numbered;
		case ANIM_RIGHT_TURN_SMALL:
			name = "right_turn_small";
			break;
	}
	Avatar->Unknown_7899F0(name, frame, flags);
	Animation = anim;
}

// @0x53CEE0
void BridgeInterface::Stop_Animation()
{
	if (Animation != ANIM_NONE) {
		Avatar->Unknown_789C10(-1.0f, 0);
		Animation = ANIM_NONE;
	}
}

// @0x53CF20
void BridgeInterface::Reset_Sounds(int time)
{
	Sounds.Reset(time);
}

/*
**	Reconstruction notes (from the retail client.exe, SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Client\bridge_int.cpp (its __FILE__ 0xB78DF0: G_ASSERT 'false' on lines
**	221 / 250, FAIL_IFs '!camera' 315, '!bridge' 318, '!pship' 332, 'Client == NULL' 404, '!anim.AnimName' 416,
**	'!bridge' 462, '!event' 521, '!gadget' 525). Client 0x53B040-0x53D23E: the constructor, the listener's slot +0x04
**	(0x53B310, ui_scut.h's inline, first emitted here), the destructor, the functions above in the client's order,
**	AvatarDescClass's destructor 0x53BB50, then the instances this file is the first to use: Create_NEW 0x53CF50 and
**	~RefCountPtr 0x53CF80 of CharacterViewAvatarClass, the signal map's disconnect 0x53CFB0 and Add 0x53D030 for
**	MemberCallback<GadgetClass, BridgeInterface, ..., FlashingGadgetEvent>, that callback's members 0x53D0D0-0x53D1C0,
**	<string>'s guarded initialiser 0x53D1F0 and the callback's deleting destructor 0x53D220.
**	The class name is the client's (the callback's RTTI, type descriptor 0xB78F28); the member and function names
**	are from their use (NAME 1 in bridge_int.h) except where an assert or an animation tag gives one.
**	SClient's Init makes the one instance (+0x128C); 0x74C3B0 gives it the player's avatar, 0x744C10 the scene,
**	Do_Frame the frame; pdaint.cpp asks for the PDA's views, ctrngadg.cpp turns the view, ui_cpit.cpp plays the
**	warp's and the incapacitance's animations, 0x6E2CC0 a hit.
**	Update_Camera_Bone 0x53CA80 and Stop_Animation 0x53CEE0 have no callers in the client: defined after their
**	uses, every call is inline (/Ob2) and the bodies stay.
*/
