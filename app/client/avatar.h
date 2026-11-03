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

	char Unknown_Pad0[0xF4 - sizeof(CompositeRenderObjClass)];	// +0xA4
	PlayerNLPClass *PlayerNLP;							// +0xF4 (0x77B870: 0x78EB50)  // NAME 2: PlayerNLP != 0 assert
	AvatarDescClass Desc;								// +0xF8 (0x10C bytes)
	int Unknown204;										// +0x204 (0x77E3D0 sets it)
	int Unknown208;										// +0x208
	int Unknown20C;										// +0x20C
	int Unknown210;										// +0x210
	bool Unknown214;									// +0x214 (0x67A440 sets it)
	bool Unknown215;									// +0x215
	char Unknown_Pad216[0x220 - 0x216];
	bool Is_Unknown220() { return(Unknown220); }		// (ui_login.cpp)  // NAME 1: neutral (D10)
	bool Unknown220;									// +0x220 (ui_login.cpp's 0x5ADA50: narrator welcome waits for it)
	bool Unknown221;									// +0x221
	char Unknown_Pad222[0x231 - 0x222];
	bool Unknown231;									// +0x231 (0x67A440 clears it)
	bool Unknown232;									// +0x232 (0x67A440 sets it)
	void Unknown_Inline_231() { Unknown231 = false; Unknown214 = true; }	// NAME 1: neutral (D10)
	char Unknown_Pad233[0x24C - 0x233];
	int Unknown24C;										// +0x24C
	char Unknown_Pad250[0x2B0 - 0x250];
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
	char Unknown_Pad2F4[0x2F8 - 0x2F4];
	int Unknown2F8;										// +0x2F8 (tested in 0x784A50)
	char Unknown_Pad2FC[0x308 - 0x2FC];
	float Unknown308;									// +0x308 (tested in 0x784A50)
	char Unknown_Pad30C[0x314 - 0x30C];
	int Unknown314;										// +0x314
	char Unknown_Pad318[0x330 - 0x318];
	Matrix3D Unknown330;								// +0x330  // NAME 1: neutral (D10)
	char Unknown_Pad360[0x384 - 0x360];					// +0x360 (total size 0x384)
};

#endif
