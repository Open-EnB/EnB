#include "avatar.h"
#include "tutorial_manager.h"
#include "nstrdup.h"
#include "..\audio\soundlib.h"
#include "..\..\wwaudio\AudibleSound.h"
#include "..\shared\unknown_debug.h"
#include "..\..\ww3d2\assetmgr.h"

void Unknown_496AE0(RenderObjClass *robj, bool make_geometry_unique);

class PlayerNLPClass
{
public:
	PlayerNLPClass() {}
	virtual ~PlayerNLPClass() {}
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
	  Unknown314(0)
{
	Unknown330.Make_Identity();
	if (desc) {
		Desc = *desc;
	}
	Desc.Upgrade();
	if (a) {
		if (!PlayerNLP) {
			PlayerNLP = new PlayerNLPClass();
		}
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
void CharacterViewAvatarClass::Unknown_784C90(const char *name, int a)
{
	if (!PlayerNLP) {
		// In the client, asserts PlayerNLP != 0
	}
	if (!a) {
		if (TutorialManagerClass::Instance && !TutorialManagerClass::Instance->Is_Completed(name)) {
			TutorialManagerClass::Instance->Set_Completed(name);
		}
	}
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
	if (TutorialManagerClass::Instance) {
		TutorialManagerClass::Instance->Start_Step(name);
	}
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

// 0x783B50
// NOT EXACT: functional for POC (D64)
void CharacterViewAvatarClass::Unknown_1C4()
{
	// Per-frame avatar update. When walk-on completes (or if no animation assets exist in POC),
	// it calls 0x781EA0 which sets Unknown220 = true.
	if (Unknown208 == 0) {
		Unknown220 = true;
	}
}

void CharacterViewAvatarClass::Unknown_788EF0()
{
}

void CharacterViewAvatarClass::Unknown_7896B0(int a)
{
}
