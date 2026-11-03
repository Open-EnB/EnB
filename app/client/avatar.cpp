#include "avatar.h"
#include "tutorial_manager.h"
#include "nstrdup.h"
#include "..\audio\soundlib.h"
#include "..\..\wwaudio\AudibleSound.h"
#include "..\shared\unknown_debug.h"
#include "..\..\ww3d2\assetmgr.h"
#include "..\..\ww3d2\hanim.h"
#include "..\..\ww3d2\ww3d.h"
#include "..\audio\surfacetypes.h"
#include "..\shared\unknown_message.h"
#include "random.h"
#include "ini.h"

extern RandomClass Unknown_BDBB84;	// NAME 1 [Unknown_BDBB84]: neutral (D10), as eff_o.cpp's

// 0xB9CD28: the default skeletons by gender (0x781D10, 0x7826A0)
static const char *Unknown_B9CD28[2] = { "AMDEFAULTSKEL", "AFDEFAULTSKEL" };	// NAME 1 [Unknown_B9CD28]: neutral (D10)

void Unknown_496AE0(RenderObjClass *robj, bool make_geometry_unique);

// POC stand-in (D64) for the personality manager's entry (0x790260 builds one, 0x188 bytes, from
// personalities\Personality%02d.ini; 0x790D40 finds it by number): only what avatar.cpp asks of it,
// read from the same INI. Its vtable +0x20 is the HEADER's FigetDelay (0x77B870 halves it for the
// random part of the delay), +0x30 the BANIM entry by Tag (0x78F640 returns its +0x04 AnimNumber).
class Unknown_790260Class	// NAME 1: neutral (D10)
{
public:
	Unknown_790260Class(int personality) : FigetDelay(50), Ini(NULL)
	{
		char name[256];
		_snprintf(name, sizeof(name), "%sPersonality%02d.ini", "personalities\\", personality);
		Ini = new INIClass(name);
		if (Ini->Is_Loaded()) {
			FigetDelay = Ini->Get_Int("HEADER", "FigetDelay", 50);
		}
	}
	~Unknown_790260Class() { delete Ini; }
	int Unknown_20() { return(FigetDelay); }
	int Unknown_30(const char *tag)
	{
		if (Ini->Is_Loaded()) {
			for (int i = 0; i < 256; i++) {
				char section[32];
				char value[64];
				sprintf(section, "BANIM-%d", i);
				if (!Ini->Section_Present(section)) {
					break;
				}
				Ini->Get_String(section, "Tag", "", value, sizeof(value));
				if (stricmp(value, tag) == 0) {
					return(Ini->Get_Int(section, "AnimNumber", 0));
				}
			}
		}
		return(0);
	}
	int FigetDelay;
	INIClass *Ini;
};

// POC stand-in (D64) for nlp.cpp's class (0x78EB50, vtable 0xB04A2C): +0x0C the personality
class PlayerNLPClass
{
public:
	PlayerNLPClass() : Unknown0C(NULL) {}
	virtual ~PlayerNLPClass() { delete Unknown0C; }
	// 0x78EC00 (vtable +0x00: 0x77B870 passes the descriptor's personality and nlp): +0x0C from 0x790D40, +0x10 lexicon%02d.ini
	void Unknown_78EC00(int personality, int nlp) { if (!Unknown0C) Unknown0C = new Unknown_790260Class(personality); }
	// 0x78F640: the personality's body animation number by tag (FAIL_IF '!body_anim')
	int Unknown_78F640(const char *tag) { return(Unknown0C ? Unknown0C->Unknown_30(tag) : 0); }
	Unknown_790260Class *Unknown0C;
};

// 0x77B870
// NOT EXACT: functional for POC (D64)
CharacterViewAvatarClass::CharacterViewAvatarClass(AvatarDescClass *desc, int a, int b, SClientUnknown1338Class *env_maps)
	: CompositeRenderObjClass(),
	  PlayerNLP(NULL),
	  Unknown204(0),
	  Unknown208(0),
	  Unknown20C(2),
	  Unknown210(0),
	  Unknown214(false),
	  Unknown215(false),
	  Unknown220(false),
	  Unknown221(false),
	  Unknown231(false),
	  Unknown232(false),
	  Unknown24C(-1),
	  HeadSource(NULL),
	  BodySource(NULL),
	  Unknown2B8(NULL),
	  Unknown2BC(NULL),
	  CollisionObj(NULL),
	  Unknown2C4(NULL),
	  HairSource(NULL),
	  Unknown2CC(NULL),
	  GoggleSource(NULL),
	  BeardSource(NULL),
	  WeaponHip(NULL),
	  WeaponUnique(NULL),
	  WeaponBack(NULL),
	  MaskObj(NULL),
	  SpeechSound(NULL),
	  Unknown2EC(NULL),
	  Unknown2F0(0.0f),
	  Unknown2F8(0),
	  Unknown308(0.0f),
	  Unknown314(0),
	  Unknown36C(false)
{
	Unknown0BC = 0;
	for (int i = 0; i < 2; i++) {
		Unknown0C0[i] = NULL;
		Unknown0C8[i] = false;
		Unknown0CC[i] = 0.0f;
		Unknown0D4[i] = 0.0f;
		Unknown0DC[i] = -1;
	}
	Unknown0E4 = 0;
	Unknown218 = 0.0f;
	Unknown21C = 0.0f;
	Unknown224 = 0;
	Unknown228 = 0.0f;
	Unknown22C = 0.0f;
	Unknown230 = true;
	Unknown233 = true;
	Unknown234 = 0;
	Unknown238 = 0;
	Unknown23C = 0;
	Unknown240 = 0;
	Unknown244 = 0;
	Unknown248 = 0;
	for (int k = 0; k < 9; k++) {
		Unknown250[k] = 0;
	}
	Unknown2F4 = true;
	Unknown330.Make_Identity();
	if (desc) {
		Desc = *desc;
	}
	Desc.Upgrade();
	if (a) {
		if (!PlayerNLP) {
			PlayerNLP = new PlayerNLPClass();
		}
		PlayerNLP->Unknown_78EC00(Desc.Personality, Desc.NLP);
		Unknown224 = Unknown_BDBB84(0, PlayerNLP->Unknown0C->Unknown_20() / 2);
		Unknown221 = true;
		Unknown220 = true;

		// 1. Build body render object
		char body_name[32];
		if (Desc.Avatar_Type == 0) {
			const char *gender = (Desc.Gender == 1) ? "AF" : "AM";
			const char *race = (Desc.Race == 1) ? "J" : ((Desc.Race == 2) ? "P" : "T");
			_snprintf(body_name, sizeof(body_name), "%s%sBODY%02d%02d",
				gender, race, (int)Desc.Body_Type, (int)Desc.Pants_Type);
		} else {
			_snprintf(body_name, sizeof(body_name), "ANPCBODY%03d", (int)Desc.Body_Type);
		}

		if (WW3DAssetManager::Get_Instance()) {
			BodySource = WW3DAssetManager::Get_Instance()->Create_Render_Obj(body_name);
		}
		FAIL_IF(BodySource == NULL) {
		}
		if (BodySource != NULL) {
			Unknown_496AE0(BodySource, true);
			Unknown2BC = BodySource->Clone();
			if (Unknown2BC) {
				Unknown2BC->Add_Ref();
				Unknown_496AE0(Unknown2BC, true);
				Unknown2BC->Set_LOD_Level(Unknown2BC->Get_LOD_Count() - 1);

				Unknown2BC->Get_Obj_Space_Bounding_Sphere(ObjSphere);
				Unknown2BC->Get_Obj_Space_Bounding_Box(ObjBox);
				Update_Obj_Space_Bounding_Volumes();
			}
		}

		// 2. Build collision object
		if (WW3DAssetManager::Get_Instance()) {
			CollisionObj = WW3DAssetManager::Get_Instance()->Create_Render_Obj("avcollision");
			if (CollisionObj) {
				Unknown_496AE0(CollisionObj, true);
				if (Unknown2BC) {
					Unknown2BC->Add_Sub_Object_To_Bone(CollisionObj, "skel_reference");
				}
			}
		}

		// 3. Build head render object
		if (Unknown2BC) {
			char head_name[32];
			if (Desc.Avatar_Type == 0) {
				const char *gender = (Desc.Gender == 1) ? "AF" : "AM";
				const char *race = (Desc.Race == 1) ? "J" : ((Desc.Race == 2) ? "P" : "T");
				_snprintf(head_name, sizeof(head_name), "%s%sHEAD%02d",
					gender, race, (int)Desc.Head_Type);
			} else {
				_snprintf(head_name, sizeof(head_name), "ANPCHEAD%03d", (int)Desc.Head_Type);
			}

			if (WW3DAssetManager::Get_Instance()) {
				HeadSource = WW3DAssetManager::Get_Instance()->Create_Render_Obj(head_name);
			}
			FAIL_IF(HeadSource == NULL) {
			}
			if (HeadSource != NULL) {
				Unknown_496AE0(HeadSource, true);
				Unknown2B8 = HeadSource->Clone();
				if (Unknown2B8) {
					Unknown2B8->Add_Ref();
					Unknown_496AE0(Unknown2B8, true);
					Unknown2BC->Add_Sub_Object_To_Bone(Unknown2B8, "K_HEAD");
				}
			}
		}

		// 4. Release creation prototypes if non-player or not retaining
		if (b == 0 || Desc.Avatar_Type != 0) {
			if (HeadSource) {
				HeadSource->Release_Ref();
				HeadSource = NULL;
			}
			if (BodySource) {
				BodySource->Release_Ref();
				BodySource = NULL;
			}
			if (HairSource) {
				HairSource->Release_Ref();
				HairSource = NULL;
			}
			if (GoggleSource) {
				GoggleSource->Release_Ref();
				GoggleSource = NULL;
			}
			if (BeardSource) {
				BeardSource->Release_Ref();
				BeardSource = NULL;
			}
		}
		if (Unknown2BC) {
			Unknown_781EA0();
		}
	} else {
		Unknown221 = false;
	}
}

// 0x77C080
// NOT EXACT: functional for POC (D64)
CharacterViewAvatarClass::~CharacterViewAvatarClass()
{
	Unknown_784E00();
	if (PlayerNLP) {
		delete PlayerNLP;
		PlayerNLP = NULL;
	}
	for (int a = 0; a < 2; a++) {
		if (Unknown0C0[a]) {
			Unknown0C0[a]->Release_Ref();
			Unknown0C0[a] = NULL;
		}
	}
	if (Unknown2BC) {
		if (WeaponHip) {
			Unknown2BC->Remove_Sub_Object(WeaponHip);
			WeaponHip->Release_Ref();
			WeaponHip = NULL;
		}
		if (WeaponUnique) {
			Unknown2BC->Remove_Sub_Object(WeaponUnique);
			WeaponUnique->Release_Ref();
			WeaponUnique = NULL;
		}
		if (WeaponBack) {
			Unknown2BC->Remove_Sub_Object(WeaponBack);
			WeaponBack->Release_Ref();
			WeaponBack = NULL;
		}
		if (CollisionObj) {
			Unknown2BC->Remove_Sub_Object(CollisionObj);
			CollisionObj->Release_Ref();
			CollisionObj = NULL;
		}
		if (Unknown2B8) {
			Unknown2BC->Remove_Sub_Object(Unknown2B8);
			Unknown2B8->Release_Ref();
			Unknown2B8 = NULL;
		}
		if (MaskObj) {
			Unknown2BC->Remove_Sub_Object(MaskObj);
			MaskObj->Release_Ref();
			MaskObj = NULL;
		}
		int head_bone = Unknown2BC->Get_Bone_Index("K_HEAD");
		Unknown2BC->Release_Bone(head_bone);
		Unknown2BC->Remove();
		Unknown2BC->Release_Ref();
		Unknown2BC = NULL;
	}
	if (Unknown2B8) {
		Unknown2B8->Release_Ref();
		Unknown2B8 = NULL;
	}
	if (HeadSource) {
		HeadSource->Release_Ref();
		HeadSource = NULL;
	}
	if (BodySource) {
		BodySource->Release_Ref();
		BodySource = NULL;
	}
	if (CollisionObj) {
		CollisionObj->Release_Ref();
		CollisionObj = NULL;
	}
	if (HairSource) {
		HairSource->Release_Ref();
		HairSource = NULL;
	}
	if (GoggleSource) {
		GoggleSource->Release_Ref();
		GoggleSource = NULL;
	}
	if (BeardSource) {
		BeardSource->Release_Ref();
		BeardSource = NULL;
	}
	if (WeaponHip) {
		WeaponHip->Release_Ref();
		WeaponHip = NULL;
	}
	if (WeaponUnique) {
		WeaponUnique->Release_Ref();
		WeaponUnique = NULL;
	}
	if (WeaponBack) {
		WeaponBack->Release_Ref();
		WeaponBack = NULL;
	}
	if (MaskObj) {
		MaskObj->Release_Ref();
		MaskObj = NULL;
	}
}

// 0x77C870
// NOT EXACT: functional for POC (D64)
RenderObjClass *CharacterViewAvatarClass::Clone() const
{
	return new CharacterViewAvatarClass((AvatarDescClass *)&Desc, 1, 0, NULL);
}

// 0x77D1E0
void CharacterViewAvatarClass::Set_Transform(const Matrix3D &m)
{
	if (Unknown2BC) {
		Unknown2BC->Set_Transform(m);
		RenderObjClass::Set_Transform(m);
		if (!Unknown36C) {
			Unknown330 = m;
		}
	}
}

// 0x77D2B0
void CharacterViewAvatarClass::Set_Position(const Vector3 &v)
{
	if (Unknown2BC) {
		Unknown2BC->Set_Position(v);
		RenderObjClass::Set_Position(v);
	}
}

// 0x77D420
void CharacterViewAvatarClass::Render(RenderInfoClass &rinfo)
{
	if (Unknown2BC) {
		Set_Transform(Unknown330);
		Unknown2BC->Render(rinfo);
	}
}

// 0x77E3D0
void CharacterViewAvatarClass::Unknown_77E3D0(int a)
{
	Unknown204 = a;
}

// 0x784A50
// NOT EXACT: functional for POC (D64)
void CharacterViewAvatarClass::Unknown_784A50(const char *name, int a, int b)
{
	if (Unknown2F8 == 0 && Unknown308 == 0.0f) {
		Unknown214 = (a != 0);
		if (a) {
			Unknown24C = -1;
		}
		Unknown220 = false;
		Unknown215 = (b != 0);
	}
}

// 0x784C90
// NOT EXACT: functional for POC (D64)
void CharacterViewAvatarClass::Unknown_784C90(const char *name, bool a)
{
	if (PlayerNLP == NULL) {
		Unknown_Assert_Failed("PlayerNLP", __FILE__, __LINE__);
		Unknown_4BC260("Avatar Animation", "", __FILE__, __LINE__, "Invalid NLP pointer");
		return;
	}
	if (!a) {
		if (TutorialManagerClass::Instance->Is_Completed(name)) {
			return;
		}
		TutorialManagerClass::Instance->Set_Completed(name);
	}
	if (SpeechSound) {
		SpeechSound->Stop(1);
		SpeechSound->Release_Ref();
		SpeechSound = NULL;
	}
	TutorialManagerClass::Instance->Stop_Step();
	Unknown208 = 0;
	Unknown214 = true;
	if (Unknown2EC) {
		delete Unknown2EC;
		Unknown2EC = NULL;
	}
	TutorialManagerClass::Instance->Start_Step(name);
	Unknown2F0 = 60.0f;
	Unknown2EC = nstrdup(name);
}

// 0x784E00
void CharacterViewAvatarClass::Unknown_784E00()
{
	if (SpeechSound) {
		SpeechSound->Stop(1);
		SpeechSound->Release_Ref();
		SpeechSound = NULL;
	}
	if (TutorialManagerClass::Instance) {
		TutorialManagerClass::Instance->Stop_Step();
	}
	Unknown208 = 0;
	Unknown214 = true;
	if (Unknown2EC) {
		delete[] Unknown2EC;
		Unknown2EC = NULL;
	}
}

// 0x786370
void CharacterViewAvatarClass::Unknown_786370(bool on)
{
	if (Unknown2BC) {
		Unknown2BC->Set_Hidden(on);
	}
	if (Unknown2B8) {
		Unknown2B8->Set_Hidden(on);
	}
}

// 0x7863E0
bool CharacterViewAvatarClass::Unknown_7863E0()
{
	if (Unknown2BC) {
		if (Unknown2BC->Is_Hidden()) {
			return(true);
		}
	}
	return(false);
}

// 0x781D10
// NOT EXACT: functional for POC (D64)
std::string CharacterViewAvatarClass::Unknown_781D10(int anim)
{
	char name[256];
	const char *skeleton;
	if (Desc.Skeleton.size() == 0) {
		skeleton = Unknown_B9CD28[Desc.Gender];
	} else {
		skeleton = Desc.Skeleton.c_str();
	}
	_snprintf(name, 0xFF, "%s.ABANIM%02d-%02d", skeleton, (int)Desc.Personality, anim);
	return(std::string(name));
}

// 0x7831C0
// NOT EXACT: functional for POC (D64)
void CharacterViewAvatarClass::Unknown_7831C0(const char *name)
{
	int i = 1;
	if (Unknown0C0[0] == NULL || Unknown215) {
		i = 0;
		if (Unknown0C0[1]) {
			Unknown0C0[1]->Release_Ref();
		}
		Unknown0C0[1] = NULL;
		Unknown0C8[1] = false;
		Unknown0CC[1] = 0.0f;
		Unknown0D4[1] = 0.0f;
		Unknown0DC[1] = -1;
	}
	if (Unknown0C0[i]) {
		Unknown0C0[i]->Release_Ref();
	}
	Unknown0C0[i] = NULL;
	Unknown0C8[i] = false;
	Unknown0CC[i] = 0.0f;
	Unknown0D4[i] = 0.0f;
	Unknown0DC[i] = -1;
	Unknown0C0[i] = WW3DAssetManager::Get_Instance()->Get_HAnim(name);
	if (Unknown0C0[i] == NULL) {
		Unknown_4BC260("Avatar Animation", "", __FILE__, __LINE__, "Cannot Find Gesture Animation file %s for gender %d, persoanlity %d",
			name, Desc.Gender, (int)Desc.Personality);
	} else if (i == 1) {
		Unknown218 = 0.5f;
		float rate = Unknown0C0[1]->Get_Frame_Rate();
		if (rate > 0.0001f && (float)Unknown0C0[1]->Get_Num_Frames() / rate < Unknown218) {
			Unknown218 = (float)Unknown0C0[1]->Get_Num_Frames() / rate;
		}
		Unknown21C = Unknown218;
	} else {
		Unknown218 = 0.0f;
		Unknown21C = 0.0f;
	}
	Unknown215 = false;
}

// 0x781EA0
// NOT EXACT: functional for POC (D64)
void CharacterViewAvatarClass::Unknown_781EA0()
{
	int delay = PlayerNLP->Unknown0C->Unknown_20();
	Unknown220 = true;
	if (SpeechSound) {
		SpeechSound->Stop(1);
		SpeechSound->Release_Ref();
		SpeechSound = NULL;
	}
	char name[32];
	if (Unknown210 != 0) {
		_snprintf(name, 0x20, "sit_breathe");
	} else if ((float)(delay + Unknown224) < Unknown22C) {
		if (Unknown244 > 3) {
			_snprintf(name, 0x20, "bored");
			Unknown244 = 0;
		} else {
			_snprintf(name, 0x20, "bored%d", Unknown_BDBB84(1, 4));
			Unknown244++;
		}
		Unknown22C = 0.0f;
		Unknown224 = Unknown_BDBB84(0, delay / 2);
		Unknown250[0] = 0;
		Unknown250[2] = 0;
		Unknown250[5] = 0;
		Unknown250[6] = 0;
	} else if (Unknown231) {
		_snprintf(name, 0x20, "stand_still");
	} else if (Unknown314 != 1) {
		_snprintf(name, 0x20, "breathe1");
		Unknown314 = 1;
	} else {
		int n = 1;
		for (;;) {
			if ((float)Unknown_BDBB84(0, 1000) * 0.001f < 0.7f) {
				break;
			}
			n++;
			if (n == 4) {
				break;
			}
		}
		_snprintf(name, 0x20, "breathe%d", n);
		Unknown314 = n;
	}
	std::string anim = Unknown_781D10(PlayerNLP->Unknown_78F640(name));
	Unknown_7831C0(anim.c_str());
	// The client then (+0xBC set, a gesture animation loaded) releases the face animation's keys and
	// starts its own (HMorphAnimClass::Release_Keys, 0x782FF0, 0x782CA0, 0x783470): not built yet.
}

// 0x783620
// NOT EXACT: functional for POC (D64)
void CharacterViewAvatarClass::Unknown_783620(float time)
{
	for (int i = 0; i < 2; i++) {
		if (Unknown0C0[i] && !Unknown0C8[i]) {
			float rate = Unknown0C0[i]->Get_Frame_Rate();
			float last = (float)(Unknown0C0[i]->Get_Num_Frames() - 1);
			bool done = true;
			if (Unknown0DC[i] != -1) {
				float target;
				if ((Unknown0DC[i] & 1) == 0) {
					target = last * Unknown0D4[i];
				} else {
					target = Unknown0D4[i];
				}
				if (target <= last) {
					last = target;
					if (target < 0.0f) {
						last = 0.0f;
					}
				}
				done = (Unknown0DC[i] & 2) == 0;
				if (Unknown0DC[i] & 4) {
					Unknown0CC[i] = last;
				}
			}
			if (last < Unknown0CC[i]) {
				Unknown0CC[i] -= rate * time;
				if (Unknown0CC[i] <= last) {
					Unknown0CC[i] = last;
					Unknown0C8[i] = done;
				}
			} else {
				if (Unknown0CC[i] < last) {
					Unknown0CC[i] += rate * time;
				}
				if (last <= Unknown0CC[i]) {
					Unknown0CC[i] = last;
					Unknown0C8[i] = done;
				}
			}
			if (Unknown0C0[1] == NULL && Unknown20C != 2) {
				float part = Unknown0CC[i] / last;
				if ((Unknown238 == 0 && part > 0.1f) || (Unknown238 == 1 && part > 0.6f)) {
					if (!Unknown2F4) {
						Unknown_442930(Unknown23C, Unknown330);
					} else {
						Unknown_4428E0(Unknown23C, Unknown330);
					}
					Unknown238++;
					if (Unknown238 == 1 && part > 0.6f) {
						Unknown238 = 2;
					}
				}
			}
		}
	}
	if (Unknown0C0[0] == NULL) {
		return;
	}
	if (Unknown0C0[1] == NULL) {
		Unknown2BC->Set_Animation(Unknown0C0[0], Unknown0CC[0], 0);
		// The client then sets +0x214 once the frame passes +0x24C while the NLP's queue (+0x2A4)
		// has an entry: not built yet.
		return;
	}
	Unknown21C -= time;
	FAIL_IF(Unknown218 < 0.0001f) {
		Unknown218 = 0.5f;
	}
	float t = Unknown21C / Unknown218;
	if (t < 0.0f) {
		t = 0.0f;
	} else if (t > 1.0f) {
		t = 1.0f;
	}
	t = 1.0f - t;
	float blend;
	if (t < 0.5f) {
		blend = 1.0f - (float)cos(t * 3.1415927f);
	} else {
		blend = (float)sin((t - 0.5f) * 3.1415927f) + 1.0f;
	}
	Unknown2BC->Set_Animation(Unknown0C0[0], Unknown0CC[0], Unknown0C0[1], Unknown0CC[1], blend * 0.5f);
	if (Unknown21C >= 0.0f) {
		return;
	}
	Unknown21C = 0.0f;
	Unknown218 = 0.0f;
	if (Unknown0C0[0]) {
		Unknown0C0[0]->Release_Ref();
	}
	Unknown0C0[0] = Unknown0C0[1];
	Unknown0CC[0] = Unknown0CC[1];
	Unknown0D4[0] = Unknown0D4[1];
	Unknown0DC[0] = Unknown0DC[1];
	Unknown0C8[0] = false;
	Unknown0C0[1] = NULL;
	Unknown0C8[1] = false;
	Unknown0CC[1] = 0.0f;
	Unknown0D4[1] = 0.0f;
	Unknown0DC[1] = -1;
}

// 0x783B50
// NOT EXACT: functional for POC (D64)
void CharacterViewAvatarClass::Unknown_1C4()
{
	if (Unknown204 == 0) {
		return;
	}
	if (Unknown0E4 == (int)WW3D::Get_Frame_Count()) {
		return;
	}
	Unknown0E4 = WW3D::Get_Frame_Count();
	if (Unknown2BC == NULL || Unknown2B8 == NULL || PlayerNLP == NULL) {
		Unknown_4BC260("Avatar Animation", "", __FILE__, __LINE__, "Invalid avatar objects (%x %x %x)", Unknown2BC, Unknown2B8, PlayerNLP);
		return;
	}
	// The client first calls 0x788900 (the path to walk, +0x320) and 0x7897D0 (+0x380): not built yet.
	float time = (float)WW3D::Get_Frame_Time() * 0.001f;
	if (time > 0.25f) {
		time = 0.25f;
	}
	// The client then moves +0x330 by the movement flags +0x2F8 and the speed +0x308 (both 0 here:
	// nothing sets them yet): not built yet.
	if (!Unknown36C) {
		Set_Transform(Unknown330);
	}
	if (!Unknown231 && !Unknown232) {
		if (Unknown208 == 0 && Unknown210 == 0 && Unknown230) {
			Unknown22C += time;
		}
	} else {
		Unknown22C = 0.0f;
	}
	// The client then (+0x233) steps the blinks (0x781410, 0x781570) 20 times a second: not built yet.
	if (Unknown2EC && (Unknown2BC == NULL || !Unknown2BC->Is_Hidden())) {
		Unknown2F0 -= time;
		if (Unknown2F0 < 0.0f) {
			if (TutorialManagerClass::Instance) {
				TutorialManagerClass::Instance->Start_Step(Unknown2EC);
			}
			Unknown2F0 = 60.0f;
		}
	}
	Unknown_783620(time);
	// The client then calls 0x7854B0 (time), the face: not built yet.
	if (Unknown0C0[1] == NULL) {
		bool next = false;
		if (Unknown214) {
			Unknown214 = false;
			Unknown248 = 0;
			next = true;
		} else if (Unknown0C0[0] == NULL || Unknown0C8[0]) {
			next = true;
		}
		if (next) {
			switch (Unknown208) {
				case 0:
					if (Unknown204 == 2) {
						Unknown204 = 0;
						break;
					}
					// The client asks TutorialManagerClass::Next_Line for a narrator (Desc.Avatar_Type != 0)
					// and speaks its line through the NLP (0x7861B0, 0x784700, 0x782200): not built yet.
					Unknown_781EA0();
					break;
				case 1:
				case 2:
				case 3:
					// 0x782200 / 0x7826A0 / 0x782850 (the NLP's queue, walking, sitting): not built yet.
					Unknown208 = 0;
					break;
			}
		}
	}
	// The client then shows +0x2E4 (the mask) or the head by 0x8190D0's 0x819280: not built yet.
}

void CharacterViewAvatarClass::Unknown_788EF0()
{
}

void CharacterViewAvatarClass::Unknown_7896B0(int a)
{
}
