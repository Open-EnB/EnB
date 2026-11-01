// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Client\charview.h (PROVISIONAL, after charview.cpp's __FILE__)
**
**	charview.cpp's CharacterViewClass: the front end's view (login, galaxy and character select, the
**	character and ship creators), 0x514 bytes, vtable 0xAFBD5C (24 slots). LoginTaskClass makes it
**	(Create_Character_View 0x769D60) and hands it the step; its +0x08 builds the step's screen.
*/



#if defined(_MSC_VER)
#pragma once
#endif

#ifndef CHARVIEW_H  // NAME 3 [charview.h]: the header of charview.cpp (its __FILE__ 'e:\build\beta\G\app\Client\charview.cpp')
#define CHARVIEW_H

#include "sclient.h"
#include "..\shared\avatar_desc.h"
#include "..\shared\shipcust_desc.h"
#include "vector2.h"
#include "vector3.h"
#include "vector4.h"
#include <string>
#include <deque>

class LoginTaskClass;
class CameraClass;
class SimpleSceneClass;
class LayerClass;
class RenderObjClass;
class DazzleLayerClass;
class LightClass;
class HAnimClass;
class AudibleSoundClass;
class SClientUnknown1338Class;		// sclient.cpp's: env_map_manager.cpp's 'envmaps.ini' (0x24 bytes)
#include "rendobj.h"

// avatar.cpp's model (the view's +0x194, +0x198 and +0x1A4), as far as charview.cpp and ui_login.cpp call it
class CharacterViewAvatarClass : public RenderObjClass	// NAME 1: neutral (D10)
{
public:
	CharacterViewAvatarClass(AvatarDescClass *desc, int a, int b, SClientUnknown1338Class *env_maps);	// 0x77B870 (0x384 bytes; 0x67A6C0: the narrator's description, 1, 0, NULL; 0x67BF20 passes +0x3A4)  // NAME 1 [desc, a, b, env_maps]: what the callers pass
	virtual RenderObjClass *Clone() const;				// (avatar.cpp's)
	virtual void Render(RenderInfoClass &rinfo);		// (avatar.cpp's)
	void Unknown_1C4();									// the client's slot +0x1C4 (every frame)
	void Unknown_77E3D0(int a);							// 0x77E3D0 (0x67A6C0: 1)  // NAME 1 [a]: neutral
	void Unknown_784A50(const char *name, int a, int b);	// 0x784A50 ('/walk_on' 1, 1; '/walk_off' 1, 0)  // NAME 1 [name, a, b]: neutral
	void Unknown_784C90(const char *name, int a);		// 0x784C90 ('Idle0', 1; ui_login.cpp's 'RejectedOnce', 1)  // NAME 1 [name, a]: neutral
	void Unknown_784E00();								// 0x784E00
	void Unknown_786370(bool on);						// 0x786370  // NAME 1 [on]: neutral
	bool Unknown_7863E0();								// 0x7863E0
	void Unknown_788EF0();								// 0x788EF0
	void Unknown_7896B0(int a);							// 0x7896B0 (0x67B390: 4)  // NAME 1 [a]: neutral
	char Unknown_Pad[0x214 - sizeof(RenderObjClass)];
	bool Unknown214;									// +0x214 (0x67A440 sets it)
	char Unknown215[0x220 - 0x215];
	bool Is_Unknown220() { return(Unknown220); }		// (ui_login.cpp)  // NAME 1: neutral (D10)
	bool Unknown220;									// +0x220 (ui_login.cpp's 0x5ADA50: the narrator's welcome waits for it)
	char Unknown221[0x231 - 0x221];
	bool Unknown231;									// +0x231 (0x67A440 clears it)
	bool Unknown232;									// +0x232 (0x67A440 sets it)
};

// The view's modes (+0x1AC): the FAIL_IF 'mode < CHARACTER_POPUP_MESSAGE || mode >= CHARACTER_LASTVIEWMODE' (0x67A370)
// and the screens 0x679160 makes for them
enum CharacterViewModeType	// NAME 1 [CharacterViewModeType]: neutral (D10); NAME 3 [CHARACTER_POPUP_MESSAGE, CHARACTER_LASTVIEWMODE]: the FAIL_IF's
{
	CHARACTER_POPUP_MESSAGE = 0,						// 0x5DB670's screen
	CHARACTER_LASTVIEWMODE = 15
};

// What the view holds five of at +0x1D4 (0x10 bytes): the animation of the model of the same type (+0x1B8)
struct CharacterViewUnknown1D4Struct	// NAME 1: neutral (D10)
{
	CharacterViewUnknown1D4Struct() : Anim(NULL), Frame(0), Mode(0), Unknown0C(false), Unknown0D(false) {}
	HAnimClass *Anim;									// +0x00 (0x67AD30 gets it by name; the model's Set_Animation)  // NAME 2: what it holds
	float Frame;										// +0x04 (0x67AD30: -1)  // NAME 1: what it holds
	int Mode;											// +0x08 (0x67ADD0: 1, 0x67AE20 / 0x67AE60: 0)  // NAME 1: Set_Animation's mode
	bool Unknown0C;										// +0x0C (0x67AEA0 tests and clears it)
	bool Unknown0D;										// +0x0D (0x67AE60 sets it, 0x67AE20 clears it)
};

// What the view holds at +0x510: an AudibleSoundClass released by the destructor  // NAME 1: neutral (D10)
class CharacterViewSoundClass	// NAME 1: neutral (D10)
{
public:
	CharacterViewSoundClass() : Sound(NULL) {}
	~CharacterViewSoundClass();							// (inline in 0x678970)
	AudibleSoundClass *Sound;							// +0x00  // NAME 1: neutral
};

class CharacterViewClass : public UIViewClass	// NAME 1 [CharacterViewClass]: the task's member CharacterView (tsk_login.cpp's FAIL_IFs '!CharacterView')
{
public:
	CharacterViewClass(LoginTaskClass *task, int mode);	// 0x678450 (Create_Character_View 0x769D60 passes its Step)  // NAME 2 [mode]: the FAIL_IF's 'mode'
	// +0x00 the implicit destructor 0x678970 (it stores no vtable; deleting 0x678820)
	virtual void Unknown_04();							// +0x04 0x678B70 (the constructor calls it: the scenes, cameras, music, env maps)
	virtual void Unknown_08();							// +0x08 0x679160: the mode's screen
	virtual int Unknown_0C(bool on);					// +0x0C 0x679930 (LoginTaskClass::Render passes true): the frame
	virtual void Unknown_10();							// +0x10 0x679510 Shutdown (LoginTaskClass::Delete_Character_View)
	virtual void Unknown_18(int on);					// +0x18 0x67A210 (the screen's +0x58 when on, +0x5C when not)  // NAME 1 [on]: what it selects
	virtual int Unknown_28() { return((int)this); }		// +0x28 0x6787E0 (the screens' views give the character view)
	virtual int Unknown_5C() { return(0); }				// +0x5C 0x678800

	// the inline accessors ui_login.cpp reads the view through
	ViewUnknown34Class *Get_Unknown034() { return(Unknown34); }	// NAME 1: neutral (D10)
	CharacterViewAvatarClass *Get_Unknown198() { return(NarratorRObj); }	// NAME 1: neutral (D10)
	int Get_Unknown1B0() { return(Unknown1B0); }		// NAME 1: neutral (D10)
	RenderObjClass *Peek_Model() { return(ViewRObjs[0]); }	// NAME 1: neutral (D10), as ui_avatr.cpp's
	void Unknown_67A250();								// 0x67A250 (every +0x1CC set: 0x67A9B0)
	void Unknown_67A290(const Vector3 &position, const Vector3 &rotation, float fov);	// 0x67A290 (the camera's: +0x4C, +0x58, +0x64)  // NAME 1 [position, rotation, fov]: what it stores
	void Unknown_67A300();								// 0x67A300 (LoginTaskClass::Run: the screen off and released)
	void Unknown_67A370(int mode);						// 0x67A370 (the FAIL_IF 'mode < CHARACTER_POPUP_MESSAGE ...'): the mode's screen  // NAME 2 [mode]: the FAIL_IF's
	void Unknown_67A3F0();								// 0x67A3F0 (+0x194 released)
	void Unknown_67AB20(int type, const char *name);	// 0x67AB20 (the G_ASSERT 'ViewRObjs[type]'): the model of the type by name  // NAME 2 [type]: the assert's; NAME 1 [name]: what ui_login.cpp passes ('FE01A')
	void Unknown_67BBE0();								// 0x67BBE0 (0x67AB20, type 4: the two lights into +0x84)
	void Unknown_67A440(AvatarDescClass *desc, bool copy);	// 0x67A440 (the G_ASSERT 'AvatarRObj': ui_avatr.cpp's player avatar, from desc or from a fresh description of its race, profession and gender)  // NAME 1 [desc, copy]: what it does with them
	void Unknown_67A670();								// 0x67A670 (+0x198 released)
	void Unknown_67A8D0(bool off);						// 0x67A8D0 (the narrator walks off, or on)  // NAME 1 [off]: what it selects
	void Unknown_67A960();								// 0x67A960 (the narrator's Idle0)
	void Unknown_67ACE0(int type);						// 0x67ACE0 (the type's animation released)  // NAME 2 [type]: as 0x67AB20's
	bool Unknown_67AD30(int type, const char *name);	// 0x67AD30 (the type's animation by name: true when found)  // NAME 2 [type]: as 0x67AB20's; NAME 1 [name]: what ui_login.cpp passes
	void Unknown_67ADD0(int type);						// 0x67ADD0 (the animation started on the model, mode 1)  // NAME 2 [type]: as 0x67AB20's
	void Unknown_67AE20(int type);						// 0x67AE20  // NAME 2 [type]: as 0x67AB20's
	void Unknown_67AE60(int type);						// 0x67AE60  // NAME 2 [type]: as 0x67AB20's
	bool Unknown_67AEA0(int type);						// 0x67AEA0 (+0x0C tested and cleared)  // NAME 2 [type]: as 0x67AB20's
	void Unknown_67AED0(int type, bool first);			// 0x67AED0 (the animation set at its first frame, or its last)  // NAME 2 [type]: as 0x67AB20's; NAME 1 [first]: what it selects
	void Unknown_67B090();								// 0x67B090 (the narrator's walk; inlined in the frame)
	void Unknown_67B120();								// 0x67B120 (the narrator's idle; inlined in the frame)
	void Unknown_67B1A0(const char *name);				// 0x67B1A0 (the music: the old one stopped, name looped)  // NAME 1 [name]: what Play_2D_Sound gets
	void Unknown_67B220();								// 0x67B220 (the music stopped)
	void Unknown_67B320();								// 0x67B320 (the two avatars of +0x1A4 off the platform and released)
	void Unknown_67B390(int race, int profession);		// 0x67B390 (the cached male and female avatars of +0x3B8 onto the platform)  // NAME 1 [race, profession]: the cache's indices
	void Unknown_67B750();								// 0x67B750 (the lights of +0x3A8 released)
	void Unknown_67BB90();								// 0x67BB90 (the lights of +0x3B0 released)
	void Unknown_67C200(Vector2 *offsets, int size, int which);	// 0x67C200 (FAIL_IF 'size > MAX_OFFSET': +0x400's offsets out)  // NAME 3 [size]: the FAIL_IF's; NAME 1 [offsets, which]: neutral
	void Unknown_67C270(const Vector2 *offsets, int size, int which);	// 0x67C270 (FAIL_IF 'size > MAX_OFFSET': +0x400's offsets in)  // NAME 3 [size]: the FAIL_IF's; NAME 1 [offsets, which]: neutral
	void Unknown_67C2E0(Vector2 *out, int index);		// 0x67C2E0 (+0x4A0)  // NAME 1 [out, index]: neutral
	void Unknown_67C310(const Vector2 *in, int index);	// 0x67C310 (+0x4A0)  // NAME 1 [in, index]: neutral
	void Unknown_67C340(Vector3 *out, int index);		// 0x67C340 (+0x4B0)  // NAME 1 [out, index]: neutral
	void Unknown_67C380(const Vector3 *in, int index);	// 0x67C380 (+0x4B0)  // NAME 1 [in, index]: neutral
	void Unknown_67B270();								// 0x67B270 ('Are you sure you want to quit?')
	bool Unknown_67B2B0();								// 0x67B2B0 (the quit box answered: yes kills the kernel, no returns true)
	void Unknown_67A6C0();								// 0x67A6C0 (the G_ASSERT 'NarratorRObj': the narrator made, walking on)
	void Unknown_67B4F0(bool on);						// 0x67B4F0 (the model of type 4's 'platform' shown, or hidden)  // NAME 1 [on]: what it selects
	void Unknown_67C440(int index, bool now);			// 0x67C440 (+0x4D4 the colour index of 0xBE6018's three; with now, +0x4C8 too)  // NAME 1 [index, now]: what they select
	void Unknown_67C7B0();								// 0x67C7B0 (the dialog stopped, the queue +0x4E0 emptied)
	void Unknown_67A9B0(int type);						// 0x67A9B0 (ui_login.cpp: the destructor passes 0)  // NAME 2 [type]: 0x67AB20's G_ASSERT 'ViewRObjs[type]'
	void Unknown_67AF70(float time);					// 0x67AF70 (the frame passes the frame time in seconds)  // NAME 1 [time]: what the frame passes
	void Unknown_67BEB0();								// 0x67BEB0 (Shutdown: the eighteen avatars of +0x3B8 released)
	void Unknown_67BF20();								// 0x67BF20 (Unknown_04: avatar.cpp's models)
	void Unknown_67C530();								// 0x67C530 (the frame)
	void Unknown_67CBD0();								// 0x67CBD0 (the frame, first: the next queued dialog when none plays)
	// 0x67CEF0: the deque's pop_front, out of line (Shutdown)

	Vector3 Unknown04C;									// +0x4C the camera's position (0x67A290; the frame translates the model's CAMERA bone by it)
	Vector3 Unknown058;									// +0x58 the camera's rotation in degrees (the frame: X, then Rotate_Y, Rotate_Z)
	float Unknown064;									// +0x64 the camera's field of view in degrees (45; 0x67A290's Set_View_Plane)
	CameraClass *Unknown068;							// +0x68 the 3-D camera (ui_login.cpp's Peek_Camera)
	SimpleSceneClass *Unknown06C;						// +0x6C the 3-D scene
	LayerClass *Unknown070;								// +0x70 a layer of +0x6C / +0x68
	LayerClass *Unknown074;								// +0x74 a layer of the base's +0x1C / +0x20: what the screens are made over
	CameraClass *Unknown078;							// +0x78 the backdrop's camera (rendered first, with +0x7C)
	SimpleSceneClass *Unknown07C;						// +0x7C
	CameraClass *Unknown080;							// +0x80 (rendered after the 3-D scene, with +0x84)
	SimpleSceneClass *Unknown084;						// +0x84
	AvatarDescClass Unknown088;							// +0x88
	CharacterViewAvatarClass *AvatarRObj;				// +0x194 (0x67A440's G_ASSERT 'AvatarRObj')  // NAME 3: the assert's
	CharacterViewAvatarClass *NarratorRObj;				// +0x198 (0x67A6C0's G_ASSERT 'NarratorRObj')  // NAME 3: the assert's
	bool Unknown19C;									// +0x19C
	bool Unknown19D;									// +0x19D
	bool Unknown19E;									// +0x19E
	float Unknown1A0;									// +0x1A0 the platform's angle (the frame adds 0.2 per second)
	CharacterViewAvatarClass *Unknown1A4[2];			// +0x1A4 (avatar.cpp's, as +0x194)
	int Mode;											// +0x1AC (0x67A370's FAIL_IF 'mode')  // NAME 2: the FAIL_IF's
	int Unknown1B0;										// +0x1B0 the mode before (0x622180's screen tests it for 5)
	InterfaceClass *Unknown1B4;							// +0x1B4 the mode's screen (0x679160)
	RenderObjClass *ViewRObjs[5];						// +0x1B8 (0x67AB20's G_ASSERT 'ViewRObjs[type]'; +0x1C8 has the 'platform' bone)  // NAME 3: the assert's
	bool Unknown1CC[5];									// +0x1CC (tsk_login.cpp sets [1]..[4]; 0x67A250 clears them)
	CharacterViewUnknown1D4Struct Unknown1D4[5];		// +0x1D4
	AudibleSoundClass *Unknown224;						// +0x224 'fe_music.mp3'
	bool Unknown228;									// +0x228
	int Unknown22C;										// +0x22C (the frame counts it down before the narrator's next animation)
	DazzleLayerClass *Unknown230;						// +0x230
	int Unknown234;										// +0x234
	int Unknown238;										// +0x238
	int Unknown23C;										// +0x23C
	int Unknown240;										// +0x240
	ShipCustDescClass Unknown244;						// +0x244
	SClientUnknown1338Class *Unknown3A4;				// +0x3A4 'envmaps.ini' (ui_login.cpp's)
	RenderObjClass *Unknown3A8[2];						// +0x3A8 (removed with the model of type 1 or 2)
	LightClass *Unknown3B0[2];							// +0x3B0 the two lights of the model of type 4 (0x67BBE0)
	CharacterViewAvatarClass *Unknown3B8[3][3][2];		// +0x3B8 an avatar for each race, profession and gender (0x67BF20, avatar.cpp's 0x77B870)
	Vector2 Unknown400[10][2];							// +0x400 ten pairs of texture offsets (0.5 each; 0x67C200 / 0x67C270 by the pair's index)
	Vector2 Unknown4A0[2];								// +0x4A0 (0.5, 0.5)
	Vector3 Unknown4B0[2];								// +0x4B0 (0.5, 0.5, 1)
	Vector3 Unknown4C8;									// +0x4C8 (Unknown_04: the colour at 0xBE6018)
	Vector3 Unknown4D4;									// +0x4D4 (likewise)
	std::deque<std::string> Unknown4E0;					// +0x4E0 (the destructor pops it)
	CharacterViewSoundClass Unknown510;					// +0x510
};

#endif //CHARVIEW_H
