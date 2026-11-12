// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Client\bridge_int.h (PROVISIONAL, D137: bridge_int.cpp's header; its __FILE__ 0xB78DF0)
*/
#ifndef BRIDGE_INT_H
#define BRIDGE_INT_H

#include "ui_scut.h"
#include "matrix3d.h"
#include "vector3.h"
#include "..\..\wwlib\ref_ptr.h"
#include "..\audio\bridgesounds.h"

class ClientClass;
class UIMeshClass;
class CameraClass;
class SimpleSceneClass;
class AvatarDescClass;
class CharacterViewAvatarClass;
class FlashingGadgetEvent;
class GadgetClass;

// bridge_int.cpp's interface (0xC4 bytes, table 0xAEEA7C over the listener's 0xAEEA88; SClient's +0x128C, made by its
// Init and deleted by its Shutdown): the ship's bridge around the first-person camera. It owns the bridge's mesh (the
// race's 't_i_bridge' / 'j_i_bridge' / 'p_i_bridge' model) and listens to its animations, seats the player's avatar on
// the mesh's 'main_chair' bone, puts both into the main view's bridge scene while the camera is inside the ship, and
// each frame places the camera on one of the mesh's camera bones: the main one, or the PDA page's after the
// transition animation between two of them has played.
class BridgeInterface : public UnknownAEEA88Class	// NAME 5: the client's RTTI (the receiver in MemberCallback<GadgetClass, BridgeInterface, void (BridgeInterface::*)(FlashingGadgetEvent *, GadgetClass *), FlashingGadgetEvent>, the type descriptor 0xB78F28)
{
public:
	// What the avatar plays in its chair (+0x7C). The high word is the rank: a playing animation is not replaced by
	// one of a higher rank number. Each name is the animation's tag in the avatar's personality.
	enum {
		ANIM_LOSE_CONTROL		= 0x10000,	// 'lose_control'  // NAME 2: the tag's
		ANIM_STOP_HARD			= 0x20000,	// 'stop_hard'  // NAME 2: the tag's
		ANIM_START_WARP			= 0x20001,	// 'start_warp'  // NAME 2: the tag's
		ANIM_GET_HIT			= 0x30000,	// 'get_hit'  // NAME 2: the tag's
		ANIM_LEFT_TURN_SMALL	= 0x40000,	// 'left_turn_small'  // NAME 2: the tag's
		ANIM_RIGHT_TURN_SMALL	= 0x40001,	// 'right_turn_small'  // NAME 2: the tag's
		ANIM_USE_CONTROLS		= 0x50000,	// 'use_controls%d' (1..4)  // NAME 2: the tag's
		ANIM_NONE				= 0x60000	// nothing (the gesture stopped)  // NAME 1: what it stands for
	};

	// Where the camera sits (+0x18 / +0x1C): the index into the camera bones' names 0xB78CD4 and the transition
	// animations 0xB78CE8 (pdaint.cpp passes them)
	enum {
		VIEW_MAIN = 0,		// the PDA closed  // NAME 1: what pdaint.cpp passes it for
		VIEW_CARGO,			// NAME 1: what pdaint.cpp passes it for
		VIEW_CHARACTER,		// NAME 1: what pdaint.cpp passes it for
		VIEW_STAR_MAP,		// NAME 1: what pdaint.cpp passes it for
		NUM_VIEWS			// NAME 1: the count
	};

	BridgeInterface(ClientClass *client);				// 0x53B040  // NAME 3 [client]: the member's
	~BridgeInterface();									// 0x53B380
	virtual void Unknown_00(void *source, int *event);	// +0x00 0x53B500: the mesh's animation started (1: the camera back to the main bone), ended (3: the view reached) or else (the mesh's flag bit 0 cleared)

	void Get_Hit(const Vector3 &hit);					// 0x53B670: the camera thrown by a hit, 'get_hit'  // NAME 2: the tag's; [hit]: NAME 1, what it is
	void Start_Warp();									// 0x53B710 (0x5821C0: the warp done)  // NAME 2: the tag's
	void Stop_Hard();									// 0x53B740 (0x5821C0: waiting for the warp)  // NAME 2: the tag's
	void Lose_Control();								// 0x53B770 (0x584690, first)  // NAME 2: the tag's
	void Set_Avatar(AvatarDescClass *desc);				// 0x53B7A0 (0x74C3B0: the player's own avatar arrived): the race's bridge model and a second avatar of that description for the chair  // NAME 1: what it does; [desc]: what it is
	void Set_Scene(const RefCountPtr<RefCountClass> &scene);	// 0x53BBC0 (0x744C10: the main view's bridge scene, or NULL when the camera is outside): the mesh and the avatar out of the old scene and into the new one  // NAME 1: what it does; [scene]: what it is (a RefCountClass as 0x744C10 wraps it: its Create_Peek is the client's 0x514960)
	void Do_Frame(CameraClass *camera);					// 0x53BD40 (SClient's Do_Frame, while there is a scene): the processes, the avatar, the bridge's transform and the camera's (FAIL_IF '!camera')  // NAME 1: its caller's; NAME 3 [camera]: the FAIL_IF's
	bool Set_View(int view);							// 0x53C670: false while a transition plays or when already there  // NAME 1: what it does; [view]: what it is
	void Set_Pitch(float pitch);						// 0x53C980 (ctrngadg.cpp: the view turned by the pointer, -0.2 .. 0.4)  // NAME 1: what it sets; [pitch]: what the gadget passes
	void Set_Yaw(float yaw);							// 0x53CA00 (the same, -0.25 .. 0.25)  // NAME 1: what it sets; [yaw]: what the gadget passes
	void Update_Camera_Bone();							// 0x53CA80 (line 462's FAIL_IF '!bridge'; inline in its four callers): CameraBone for the view  // NAME 1: what it does
	void Update_Avatar();								// 0x53CB40: the avatar's frame, the turn animations from the ship's roll  // NAME 1: what it does
	void On_Gadget_Flash(FlashingGadgetEvent *event, GadgetClass *sender);	// 0x53CCD0 (the signal map's callback while in a scene): 'use_controls'  // NAME 1: what it handles; NAME 3 [event]: the FAIL_IF's '!event'; NAME 1 [sender]: the callback's second argument
	void Play_Animation(int anim, float frame, int flags, int variant);	// 0x53CD60: the tag's body animation onto the avatar (variant 1..4 of 'use_controls', else random)  // NAME 1: what it does; [anim, variant]: what they are; [frame, flags]: 0x789C10's
	void Stop_Animation();								// 0x53CEE0  // NAME 1: what it does
	void Reset_Sounds(int time);						// 0x53CF20: BridgeSoundManagerClass::Reset  // NAME 1: what it does; [time]: bridgesounds.h's

	bool Is_Busy() { return(ViewReached != View); }		// (inline: pdaint.cpp's 0x693CA0 and 0x695780 compare +0x1C with +0x18)  // NAME 1: neutral
	SimpleSceneClass *Peek_Scene() { return((SimpleSceneClass *)Scene.Peek()); }	// NAME 1: what it returns
	ClientClass *Peek_Client() { return(Client); }	// the listener's +0x04 (bridge_int.cpp's FAIL_IF 'Client == NULL' l.404 names it Client)  // NAME 1: what it returns

	ClientClass *Client;
	UIMeshClass *Mesh;									// +0x08: the bridge (its model is the FAIL_IFs' 'bridge')  // NAME 1: what it is
	const char *BridgeName;								// +0x0C: the race's model name  // NAME 1: what it holds
	RefCountPtr<CharacterViewAvatarClass> Avatar;		// +0x10: the player in the chair  // NAME 1: what it is
	RefCountPtr<RefCountClass> Scene;					// +0x14: the SimpleSceneClass the bridge is drawn in (NULL: not drawn)  // NAME 1: what it is
	int View;											// +0x18: the view asked for  // NAME 1: what it holds
	int ViewReached;									// +0x1C: the view the camera is at (differs while the transition plays)  // NAME 1: what it holds
	bool InTransition;									// +0x20: the transition animation is playing (the camera rides the main bone)  // NAME 1: what it holds
	int CameraBone;										// +0x24: the mesh's bone the camera sits on  // NAME 1: what it holds
	Matrix3D CameraTransform;							// +0x28: that bone's transform, turned by Yaw / Pitch and moved by HitOffset (mainview.cpp's 0x50D520 draws the bridge through it)  // NAME 1: what it holds
	Vector3 HitOffset;									// +0x58: the camera's offset in its own space after a hit  // NAME 1: what it holds
	Vector3 Turn;										// +0x64: the ship's turn rates, smoothed (the bridge leans by them)  // NAME 1: what it holds
	float Pitch;										// +0x70: the view's pitch  // NAME 1: what it holds
	float Yaw;											// +0x74: the view's yaw  // NAME 1: what it holds
	bool Looking;										// +0x78: Pitch / Yaw were set this frame (else they decay)  // NAME 1: what it holds
	bool Unknown79;										// +0x79
	int Animation;										// +0x7C: what the avatar plays (ANIM_*)  // NAME 1: what it holds
	BridgeSoundManagerClass Sounds;							// +0x80  // NAME 1: what it is
};

#endif
