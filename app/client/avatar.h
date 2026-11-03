#ifndef AVATAR_H
#define AVATAR_H

#include "composite.h"
#include "..\shared\avatar_desc.h"

class LightClass;
class HAnimClass;
class AudibleSoundClass;
class SClientUnknown1338Class;
class PlayerNLPClass;

// avatar.cpp's model (the view's +0x194, +0x198 and +0x1A4)
class CharacterViewAvatarClass : public CompositeRenderObjClass	// NAME 1: neutral (D10)
{
public:
	CharacterViewAvatarClass(AvatarDescClass *desc, int a, int b, SClientUnknown1338Class *env_maps);	// 0x77B870 (0x384 bytes)
	virtual ~CharacterViewAvatarClass();				// 0x77C080
	virtual RenderObjClass *Clone() const;				// 0x77C870
	virtual void Set_Transform(const Matrix3D &m);		// 0x77D1E0 (the body too; +0x330 keeps it unless +0x36C)
	virtual void Set_Position(const Vector3 &v);		// 0x77D2B0 (the body too)
	virtual void Render(RenderInfoClass &rinfo);		// 0x77D420
	virtual void Get_Obj_Space_Bounding_Sphere(SphereClass &sphere) const { sphere = ObjSphere; }	// 0x77BFA0
	virtual void Get_Obj_Space_Bounding_Box(AABoxClass &box) const { box = ObjBox; }	// 0x77BFE0
	virtual void Unknown_1C4();							// 0x783B50 (per-frame update)

	void Unknown_77E3D0(int a);							// 0x77E3D0 (0x67A6C0: 1)  // NAME 1 [a]: neutral
	void Unknown_784A50(const char *name, int a, int b);	// 0x784A50 ('/walk_on' 1, 1; '/walk_off' 1, 0)  // NAME 1 [name, a, b]: neutral
	void Unknown_784C90(const char *name, int a);		// 0x784C90 ('Idle0', 1; ui_login.cpp's 'RejectedOnce', 1)  // NAME 1 [name, a]: neutral
	void Unknown_784E00();								// 0x784E00
	void Unknown_786370(bool on);						// 0x786370  // NAME 1 [on]: neutral
	bool Unknown_7863E0();								// 0x7863E0
	void Unknown_788EF0();								// 0x788EF0
	void Unknown_7896B0(int a);							// 0x7896B0 (0x67B390: 4)  // NAME 1 [a]: neutral
	void Unknown_781EA0();								// 0x781EA0 (the idle: breathe / bored / sit_breathe; sets +0x220)
	std::string Unknown_781D10(int anim);				// 0x781D10 ('%s.ABANIM%02d-%02d': skeleton, personality, anim)  // NAME 1 [anim]: neutral
	void Unknown_7831C0(const char *name);				// 0x7831C0 (the gesture animation into +0xC0, or +0xC4 to blend; 'Cannot Find Gesture Animation')  // NAME 1 [name]: neutral
	void Unknown_783620(float time);					// 0x783620 (the frames on, the body's Set_Animation)  // NAME 1 [time]: neutral

	// +0xBC: sizeof(CompositeRenderObjClass)
	int Unknown0BC;										// +0xBC (0x781EA0: the face animation)
	HAnimClass *Unknown0C0[2];							// +0xC0 (0x7831C0: the gesture animation, [1] the one blended to)
	bool Unknown0C8[2];									// +0xC8 (0x783620: the frame reached)
	char Unknown_Pad0CA[0xCC - 0xCA];
	float Unknown0CC[2];								// +0xCC (0x783620: the frame)
	float Unknown0D4[2];								// +0xD4 (0x783620: the frame to stop at)
	int Unknown0DC[2];									// +0xDC (0x783620: flags, -1 none)
	int Unknown0E4;										// +0xE4 (0x783B50: the WW3D frame count of the last update)
	char Unknown_Pad0E8[0xF4 - 0xE8];					// +0xE8 (0x784A50: the NLP's command stream)
	PlayerNLPClass *PlayerNLP;							// +0xF4 (0x77B870: 0x78EB50)  // NAME 2: PlayerNLP != 0 assert
	AvatarDescClass Desc;								// +0xF8 (0x10C bytes)
	int Unknown204;										// +0x204 (0x77E3D0 sets it)
	int Unknown208;										// +0x208
	int Unknown20C;										// +0x20C
	int Unknown210;										// +0x210
	bool Unknown214;									// +0x214 (0x67A440 sets it)
	bool Unknown215;									// +0x215
	char Unknown_Pad216[0x218 - 0x216];
	float Unknown218;									// +0x218 (0x7831C0: the blend time, 0.5)
	float Unknown21C;									// +0x21C (0x783620: the blend time left)
	bool Is_Unknown220() { return(Unknown220); }		// (ui_login.cpp)  // NAME 1: neutral (D10)
	bool Unknown220;									// +0x220 (ui_login.cpp's 0x5ADA50: narrator welcome waits for it)
	bool Unknown221;									// +0x221
	char Unknown_Pad222[0x224 - 0x222];
	int Unknown224;										// +0x224 (0x781EA0: random part of the fidget delay)
	float Unknown228;									// +0x228
	float Unknown22C;									// +0x22C (0x783B50: the time idle)
	bool Unknown230;									// +0x230 (0x77B870 sets it)
	bool Unknown231;									// +0x231 (0x67A440 clears it)
	bool Unknown232;									// +0x232 (0x67A440 sets it)
	void Unknown_Inline_231() { Unknown231 = false; Unknown214 = true; }	// NAME 1: neutral (D10)
	bool Unknown233;									// +0x233 (0x77B870 sets it)
	int Unknown234;										// +0x234
	int Unknown238;										// +0x238 (0x783620: footsteps played)
	int Unknown23C;										// +0x23C (0x783620: the surface type)
	int Unknown240;										// +0x240
	int Unknown244;										// +0x244 (0x781EA0: the bored count)
	int Unknown248;										// +0x248
	int Unknown24C;										// +0x24C
	int Unknown250[9];									// +0x250 (0x77B870 clears them; 0x781EA0 +0x250, +0x258, +0x264, +0x268)
	char Unknown_Pad274[0x2B0 - 0x274];
	RenderObjClass *HeadSource;							// +0x2B0  // NAME 2: '!HeadSource || !BodyObj' assert
	RenderObjClass *BodySource;							// +0x2B4  // NAME 1: neutral (D10)
	RenderObjClass *Unknown2B8;							// +0x2B8 (HeadObj, 0x786370 sets hidden)
	RenderObjClass *Unknown2BC;							// +0x2BC (BodyObj, 0x786370 / 0x7863E0)
	RenderObjClass *CollisionObj;						// +0x2C0  // NAME 1: neutral (D10)
	RenderObjClass *Unknown2C4;							// +0x2C4
	RenderObjClass *HairSource;							// +0x2C8  // NAME 1: neutral (D10)
	RenderObjClass *Unknown2CC;							// +0x2CC
	RenderObjClass *GoggleSource;						// +0x2D0  // NAME 1: neutral (D10)
	RenderObjClass *BeardSource;						// +0x2D4  // NAME 1: neutral (D10)
	RenderObjClass *WeaponHip;							// +0x2D8  // NAME 1: neutral (D10)
	RenderObjClass *WeaponUnique;						// +0x2DC  // NAME 1: neutral (D10)
	RenderObjClass *WeaponBack;							// +0x2E0  // NAME 1: neutral (D10)
	RenderObjClass *MaskObj;							// +0x2E4  // NAME 1: neutral (D10)
	AudibleSoundClass *SpeechSound;						// +0x2E8 (0x784E00 / 0x784C90)  // NAME 2: plays speech sound
	char *Unknown2EC;									// +0x2EC (nstrdup in 0x784C90, delete in 0x784E00)
	float Unknown2F0;									// +0x2F0 (60.0f in 0x784C90)
	bool Unknown2F4;									// +0x2F4 (0x77B870 sets it; 0x783620: running footsteps)
	char Unknown_Pad2F5[0x2F8 - 0x2F5];
	int Unknown2F8;										// +0x2F8 (tested in 0x784A50)
	char Unknown_Pad2FC[0x308 - 0x2FC];
	float Unknown308;									// +0x308 (tested in 0x784A50)
	char Unknown_Pad30C[0x314 - 0x30C];
	int Unknown314;										// +0x314
	char Unknown_Pad318[0x330 - 0x318];
	Matrix3D Unknown330;								// +0x330  // NAME 1: neutral (D10)
	char Unknown_Pad360[0x36C - 0x360];					// +0x360
	bool Unknown36C;									// +0x36C (0x77B870 clears it; set: 0x77D1E0 leaves +0x330 alone)
	char Unknown_Pad36D[0x384 - 0x36D];					// +0x36D (total size 0x384)
};

#endif
