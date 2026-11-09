// Earth & Beyond client (reconstruction): the starbase avatar, 0x78A910-0x78BEA0 (PROVISIONAL file, D137: no
// __FILE__; after avatar.cpp's object, before unknown_creator's): the constructor 0x78A910 (over avatar.cpp's
// 0x77B870; tables 0xB04808 / 0xB047F0 / 0xB047EC), the destructor, the idle animations and walk points
// (0x78ACE0-0x78BB60; TG3, OD1). Notes at the end.
#include <stdio.h>
#include <vector>
#include <string>
#include "avatar.h"
#include "..\..\ww3d2\assetmgr.h"
#include "..\..\ww3d2\hanim.h"
#include "..\..\ww3d2\hmorphanim.h"
#include "..\..\ww3d2\ww3d.h"
#include "..\..\wwlib\ref_ptr.h"
#include "matrix3d.h"
#include "random.h"

extern RandomClass Unknown_BDBB84;	// NAME 1 [Unknown_BDBB84]: neutral (D10), as eff_o.cpp's

static const char *Unknown_B9D55C[2] = { "AMDEFAULTSKEL", "AFDEFAULTSKEL" };	// NAME 1 [Unknown_B9D55C]: neutral (D10): this object's own copy of avatar.cpp's 0xB9CD28

// One of the avatar's idle animations (0x2C bytes; 0x78BB60 picks the one whose range holds a roll)
struct Unknown78A910AnimStruct	// NAME 1: neutral (D10)
{
	Unknown78A910AnimStruct(int anim, int min, int max, const char *model, const char *motion)	// (0x78AD20 inlines it)  // NAME 1 [anim, min, max, model, motion]: neutral
		: Anim(anim), Min(min), Max(max), Model(""), Motion("")
	{
		if (model) {
			Model = model;
		}
		if (motion) {
			Motion = motion;
		}
	}

	int Anim;											// +0x00 (the ABANIM number)  // NAME 1: neutral
	int Min;											// +0x04  // NAME 1: neutral
	int Max;											// +0x08  // NAME 1: neutral
	std::string Model;									// +0x0C (a model added to the body)  // NAME 1: neutral
	std::string Motion;									// +0x1C (its animation)  // NAME 1: neutral
};

// The starbase avatar (0x3E4 bytes), declared as far as the rebuilt functions use it; the constructor's stores
class Unknown78A910AvatarClass : public CharacterViewAvatarClass	// NAME 1: neutral (D10)
{
public:
	Unknown78A910AvatarClass(int a, AvatarDescClass *desc, EnvironmentMapManagerClass *env_maps);	// 0x78A910 (ui_starbase.cpp's 0x642740)  // NAME 1 [a]: neutral
	virtual ~Unknown78A910AvatarClass();				// 0x78AA80
	virtual void Unknown_1C4();							// 0x78ACE0 (0x78B660 when +0x384 is 3, then the base's)

	void Unknown_78AD20(int anim, int min, int max, const char *model, const char *motion);	// 0x78AD20 (an idle animation added; +0x3C0 the largest top)  // NAME 1: neutral  // NAME 1 [anim, min, max, model, motion]: neutral
	void Unknown_78B010(const Matrix3D &tm);			// 0x78B010 (a walk point to +0x398; +0x3BA set)  // NAME 1: neutral  // NAME 1 [tm]: neutral
	void Unknown_78B290(const Matrix3D &tm);			// 0x78B290 (a walk point to +0x3A8)  // NAME 1: neutral  // NAME 1 [tm]: neutral
	void Unknown_78B530(int a, int b, int c, int d);	// 0x78B530 (+0x3C4..+0x3D0)  // NAME 1: neutral  // NAME 1 [a, b, c, d]: neutral
	void Unknown_78B570(bool a);						// 0x78B570 (+0x3B8 = !a; the idle animation's model dropped; ui_starbase.cpp's 0x638DA0 / 0x63D2F0)  // NAME 1: neutral  // NAME 1 [a]: neutral
	void Unknown_78B610();								// 0x78B610 (+0x3D4 cleared, +0x3D8 a roll between +0x3C4 and +0x3C8)  // NAME 1: neutral
	void Unknown_78B660();								// 0x78B660 (the idle: after +0x3D8 seconds an animation or a walk)  // NAME 1: neutral
	void Unknown_Inline_Release()						// (the destructor, 0x78B570 and 0x78B660 inline it: the destructor keeps std::copy out of line only with it)  // NAME 1: neutral (D10)
	{
		if (Unknown3DC) {
			Unknown3DC->Set_Animation();
			Unknown3DC->Remove();
			Unknown3DC = NULL;
		}
		if (Unknown3E0) {
			Unknown3E0 = NULL;
		}
	}
	void Unknown_78B810();								// 0x78B810 (0x78B660, mode +0x3BA set)
	void Unknown_78BA20();								// 0x78BA20 (0x78B660, else; 0x78B810 when the two lists differ in size)
	void Unknown_78BB60();								// 0x78BB60 (0x78B660: an idle animation by a roll)

	int Unknown384;										// +0x384 (the constructor's first argument; 3: 0x78ACE0 runs 0x78B660)
	std::vector<Unknown78A910AnimStruct> Unknown388;	// +0x388 (the idle animations)
	std::vector<Matrix3D> Unknown398;					// +0x398 (walk points)  // NAME 1: neutral
	std::vector<Matrix3D> Unknown3A8;					// +0x3A8 (walk points)  // NAME 1: neutral
	bool Unknown3B8;									// +0x3B8
	bool Unknown3B9;									// +0x3B9 (set: walk through the points between)
	bool Unknown3BA;									// +0x3BA (0x78B660: 0x78B810, else 0x78BA20)
	bool Unknown3BB;									// +0x3BB (1 from the constructor: point 1 at most)
	int Unknown3BC;										// +0x3BC (the point it stands at)
	int Unknown3C0;										// +0x3C0 (the idle roll's top)
	int Unknown3C4;										// +0x3C4 (5 from the constructor; the delay roll's bottom)
	int Unknown3C8;										// +0x3C8 (15 from the constructor; the delay roll's top)
	int Unknown3CC;										// +0x3CC (33 from the constructor; rolls above it play an animation)
	int Unknown3D0;										// +0x3D0 (67 from the constructor)
	float Unknown3D4;									// +0x3D4
	float Unknown3D8;									// +0x3D8 (1.0 from the constructor)
	RefCountPtr<RenderObjClass> Unknown3DC;				// +0x3DC (the idle animation's model)
	RefCountPtr<HAnimClass> Unknown3E0;					// +0x3E0 (its animation)
};

// 0x78A910
Unknown78A910AvatarClass::Unknown78A910AvatarClass(int a, AvatarDescClass *desc, EnvironmentMapManagerClass *env_maps)
	: CharacterViewAvatarClass(desc, true, false, env_maps),
	  Unknown384(a),
	  Unknown3B8(false),
	  Unknown3B9(false),
	  Unknown3BA(false),
	  Unknown3BB(true),
	  Unknown3BC(0),
	  Unknown3C0(0),
	  Unknown3C4(5),
	  Unknown3C8(15),
	  Unknown3CC(33),
	  Unknown3D0(67),
	  Unknown3D4(0.0f),
	  Unknown3D8(1.0f)
{
}

// 0x78AA80
Unknown78A910AvatarClass::~Unknown78A910AvatarClass()
{
	Unknown_Inline_Release();
	Unknown388.clear();
	Unknown3A8.clear();
}

// 0x78ACE0
void Unknown78A910AvatarClass::Unknown_1C4()
{
	if (Unknown204) {
		if (Unknown384 == 3) {
			Unknown_78B660();
		}
		CharacterViewAvatarClass::Unknown_1C4();
	}
}

// 0x78AD20
void Unknown78A910AvatarClass::Unknown_78AD20(int anim, int min, int max, const char *model, const char *motion)
{
	Unknown78A910AnimStruct entry(anim, min, max, model, motion);	// NAME 1 [entry]: neutral
	Unknown388.push_back(entry);
	if (Unknown3C0 < max) {
		Unknown3C0 = max;
	}
}

// 0x78B010
void Unknown78A910AvatarClass::Unknown_78B010(const Matrix3D &tm)
{
	Unknown398.push_back(tm);
	Unknown3BA = true;
}

// 0x78B290
void Unknown78A910AvatarClass::Unknown_78B290(const Matrix3D &tm)
{
	Unknown3A8.push_back(tm);
}

// 0x78B530
void Unknown78A910AvatarClass::Unknown_78B530(int a, int b, int c, int d)
{
	Unknown3C4 = a;
	Unknown3C8 = b;
	Unknown3CC = c;
	Unknown3D0 = d;
}

// 0x78B570
void Unknown78A910AvatarClass::Unknown_78B570(bool a)
{
	Unknown3B8 = !a;
	Unknown_Inline_Release();
}

// 0x78B610
void Unknown78A910AvatarClass::Unknown_78B610()
{
	Unknown3D4 = 0.0f;
	Unknown3D8 = (float)Unknown_BDBB84(Unknown3C4, Unknown3C8);
}

// 0x78B660
void Unknown78A910AvatarClass::Unknown_78B660()
{
	if (Unknown220 && !Unknown3B8) {
		Unknown_Inline_Release();
		Unknown3D4 += (float)WW3D::Get_Frame_Time() * 0.001f;
		if (Unknown3D4 > Unknown3D8) {
			int roll = Unknown_BDBB84(0, Unknown3CC + Unknown3D0);
			if (roll > Unknown3CC && (!Unknown3BB || Unknown3CC <= 0)) {
				Unknown_78BB60();
			} else if (Unknown3BA) {
				Unknown_78B810();
			} else {
				Unknown_78BA20();
			}
			Unknown3D4 = 0.0f;
			Unknown3D8 = (float)Unknown_BDBB84(Unknown3C4, Unknown3C8);
			Unknown3BB = false;
		}
	}
}

// 0x78B810
void Unknown78A910AvatarClass::Unknown_78B810()
{
	if (Unknown398.size() != Unknown3A8.size()) {
		Unknown_78BA20();
	}
	int last = Unknown3A8.size() - 1;
	int index = Unknown_BDBB84(0, last);
	if (Unknown3BB) {
		index = (last > 1) ? 1 : last;
	}
	if (Unknown3BC != index) {
		if (Unknown3B9) {
			int i;
			if (index > Unknown3BC) {
				for (i = Unknown3BC + 1; i <= index; i++) {
					Unknown_788730(Unknown398[i], false);
					if (i == index) {
						Unknown_788730(Unknown3A8[i], false);
					}
				}
			} else {
				for (i = Unknown3BC - 1; i >= index; i--) {
					Unknown_788730(Unknown398[i], false);
					if (i == index) {
						Unknown_788730(Unknown3A8[i], false);
					}
				}
			}
		} else {
			Unknown_788730(Unknown398[index], true);
			Unknown_788730(Unknown3A8[index], true);
		}
		Unknown3BC = index;
	}
}

// 0x78BA20
void Unknown78A910AvatarClass::Unknown_78BA20()
{
	int last = Unknown3A8.size() - 1;
	int index = Unknown_BDBB84(0, last);
	if (Unknown3BB) {
		index = (last > 1) ? 1 : last;
	}
	if (Unknown3BC != index) {
		if (Unknown3B9) {
			int i;
			if (index > Unknown3BC) {
				for (i = Unknown3BC + 1; i <= index; i++) {
					Unknown_788730(Unknown3A8[i], false);
				}
			} else {
				for (i = Unknown3BC - 1; i >= index; i--) {
					Unknown_788730(Unknown3A8[i], false);
				}
			}
		} else {
			Unknown_788730(Unknown3A8[index], true);
		}
		Unknown3BC = index;
	}
}

// 0x78BB60
void Unknown78A910AvatarClass::Unknown_78BB60()
{
	int count = Unknown388.size();
	int roll = Unknown_BDBB84(0, Unknown3C0);
	for (int i = 0; i < count; i++) {
		int min = Unknown388[i].Min;	// NAME 1: neutral
		int max = Unknown388[i].Max;	// NAME 1: neutral
		if (roll >= min && roll <= max) {
			char name[64];
			_snprintf(name, sizeof(name), "%s.ABANIM%02d-%02d", Unknown_B9D55C[Desc.Gender], (int)Desc.Personality, Unknown388[i].Anim);
			Unknown_7831C0(name);
			if (!Unknown388[i].Model.empty() && !Unknown388[i].Motion.empty()) {
				Unknown3DC = Create_Get(WW3DAssetManager::Get_Instance()->Create_Render_Obj(Unknown388[i].Model.c_str()));
				BodyObj->Add_Sub_Object(Unknown3DC.Peek());
				Unknown3E0 = Create_Get(WW3DAssetManager::Get_Instance()->Get_HAnim(Unknown388[i].Motion.c_str()));
				Unknown3DC->Set_Animation(Unknown3E0.Peek(), 0.0f, 2);
			}
			if (Unknown0BC && (Unknown0C0[0] || Unknown0C0[1])) {
				Unknown0BC->Release_Keys();
				Unknown_782CA0();
				Unknown_782FF0("");
				Unknown_783470();
			}
			Unknown208 = 1;
			Unknown220 = false;
			return;
		}
	}
}

/*
**	Reconstruction notes (unknown_78a910.cpp)
**
**	* The object: the constructor 0x78A910 follows avatar.cpp's last template copies (0x78A3E0-0x78A900)
**	  and this file has its own copy of avatar.cpp's skeleton-name table (0xB9D55C against 0xB9CD28):
**	  another object, the starbase avatar (ui_starbase.cpp's 0x642740 constructs it; 0x638DA0 / 0x63D2F0
**	  call 0x78B570). The file's name is unknown (PROVISIONAL, D137).
**	* 0x78B810, 0x78BA20 and 0x78BB60 exact (TG3). 0x78BB60 reads an entry's Min and Max into locals
**	  before comparing (the client's roll stays in memory) and tests the strings with empty().
**	* 0x78A910-0x78B660 exact (OD1), with the compiler's own 0x78AA50, 0x78AF80 and the template copies after
**	  0x78BEA0. The constructor initialises every member in its list (all stores precede the vtables), so
**	  +0x3C4..+0x3D0 are four members, not an array. 0x78AD20 keeps its entry in a named local (+0x3C0 is
**	  compared before the entry dies). The destructor keeps std::copy<Unknown78A910AnimStruct *> out of
**	  line (0x78C4B0, operator= inlined in it) only when the release of +0x3DC / +0x3E0 is an inline helper
**	  (Unknown_Inline_Release, also inlined by 0x78B570 and 0x78B660); written out, copy inlines.
*/
