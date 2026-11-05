#include "avatar.h"
#include "tutorial_manager.h"
#include "nstrdup.h"
#include "..\audio\soundlib.h"
#include "..\..\wwaudio\AudibleSound.h"
#include "..\shared\unknown_debug.h"
#include "..\..\ww3d2\assetmgr.h"
#include "..\..\ww3d2\hanim.h"
#include "..\..\ww3d2\hmorphanim.h"
#include "..\..\ww3d2\ww3d.h"
#include "..\..\ww3d2\mesh.h"
#include "..\..\ww3d2\meshmdl.h"
#include "..\..\ww3d2\htree.h"
#include "..\..\ww3d2\hlod.h"
#include "..\..\ww3d2\matinfo.h"
#include "..\..\ww3d2\vertmaterial.h"
#include "..\..\ww3d2\texture.h"
#include "..\..\ww3d2\mapper.h"
#include "..\..\ww3d2\scene.h"
#include "..\..\wwlib\ref_ptr.h"
#include "..\audio\surfacetypes.h"
#include "..\shared\unknown_message.h"
#include "random.h"
#include "ini.h"
#include "nlp.h"
#include "env_map_manager.h"
#include "..\packets\pkcolorization.h"

extern RandomClass Unknown_BDBB84;	// NAME 1 [Unknown_BDBB84]: neutral (D10), as eff_o.cpp's

// 0xB9CD28: the default skeletons by gender (0x781D10, 0x7826A0)
static const char *Unknown_B9CD28[2] = { "AMDEFAULTSKEL", "AFDEFAULTSKEL" };	// NAME 1 [Unknown_B9CD28]: neutral (D10)

// 0xB9CD30: the gender prefixes, 0xB9CD38 the race letters (0x77DA20, 0x783470)
static const char *Unknown_B9CD30[2] = { "AM", "AF" };	// NAME 1 [Unknown_B9CD30]: neutral (D10)
static const char *Unknown_B9CD38[5] = { "T", "J", "P", "L", "R" };	// NAME 1 [Unknown_B9CD38]: neutral (D10)

// 0xB9CD44: the eye sides (0x7804A0)
static const char *Unknown_B9CD44[2] = { "L", "R" };	// NAME 1 [Unknown_B9CD44]: neutral (D10)

void Unknown_496AE0(RenderObjClass *robj, bool make_geometry_unique);
void Unknown_496DB0(RenderObjClass *robj);
bool Unknown_495FD0(RenderObjClass *robj, TextureClass *tex, int index);	// 0x495FD0 (ro_funcs.cpp): the texture onto the mesh's stage index
void Unknown_4BC000(const char *sender, const char *category, const char *file, int line, const char *format, ...);	// NAME 1 [Unknown_4BC000]: 0x4BC000, E&B's message reporter (tcpconn.cpp)	// 0x496DB0 (ro_funcs: every LOD to its last, the sub-objects too)
// 0xBFAE52 is WW3D::OverbrightModifyOnLoad (MeshModelClass::post_process 0x9BCC40 reads it): the avatar's models
// (body 0x77E9D0, head 0x77F000, weapon 0x77E0E0, accessory 0x77F960) load overbright (MODULATE2X), as FE_ROOM does
static inline PlayerNLPClass *NLP_Of(CharacterViewAvatarClass *avatar) { return(avatar->PlayerNLP); } // NAME 1: neutral (D10)
void Unknown_80FCE0(RenderObjClass *robj, std::vector<ColorizationRegionStruct> &regions, EnvironmentMapManagerClass *env_maps,
	const Vector3 *skin);	// 0x80FCE0 (shiputils.cpp)

// 0x77B7D0
// 0..1 eased: a half cosine below 0.5, a half sine above (0x576AC0's planet turn, sbaseview 0x69ED80); interim placement (D137)
float Unknown_77B7D0(float t)	// NAME 1 [Unknown_77B7D0, t]: neutral (D10)
{
	if (t >= 1.0f) {
		t = 1.0f;
	} else if (t <= 0.0f) {
		t = 0.0f;
	}
	if (t < 0.5f) {
		return((float)(1.0 - cos(t * WWMATH_PI)) * 0.5f);
	}
	return((float)(sin((t - 0.5f) * WWMATH_PI) + 1.0) * 0.5f);
}

// 0x77B870
// NOT EXACT: functional for POC (D64)
CharacterViewAvatarClass::CharacterViewAvatarClass(AvatarDescClass *desc, int a, int b, EnvironmentMapManagerClass *env_maps)
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
	  HeadObj(NULL),
	  BodyObj(NULL),
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
	Unknown250 = 0.0f;
	Unknown254 = 0.0f;
	Unknown258 = 0.0f;
	Unknown25C = 0.0f;
	Unknown260 = 0.0f;
	Unknown264 = 0.0f;
	Unknown268 = 0.0f;
	Unknown26C = 0;
	Unknown270 = 0;
	Unknown29C = false;
	Unknown29D = false;
	Unknown2F4 = true;
	Unknown2F5 = true;
	Unknown2FC = 1.0f;
	Unknown300 = 1.0f;
	Unknown304 = 1.0f;
	Unknown30C = 0.0f;
	Unknown310 = 0.0f;
	Unknown330.Make_Identity();
	Unknown360.Set(0.0f, 0.0f, 0.0f);
	Unknown370 = NULL;
	Unknown374 = NULL;
	Unknown378 = -100.0f;
	Unknown37C = 0.0f;
	Unknown380 = 0;
	if (desc) {
		Desc = *desc;
	}
	for (int w = 0; w < 5; w++) {
		Unknown274[w] = Desc.Body_Weight[w];
	}
	for (w = 0; w < 5; w++) {
		Unknown288[w] = Desc.Head_Weight[w];
	}
	Desc.Upgrade();
	if (a) {
		if (!PlayerNLP) {
			PlayerNLP = new PlayerNLPClass();
		}
		PlayerNLP->Unknown_78EC00(Desc.Personality, Desc.NLP);
		Unknown224 = Unknown_BDBB84(0, PlayerNLP->Personality->Get_Fidget_Delay() / 2);
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

		bool making = WW3D::Is_Overbright_Modify_On_Load_Enabled();		// 0x77E9D0: the body loads overbright
		if (!making) {
			WW3D::Enable_Overbright_Modify_On_Load(true);
		}
		if (WW3DAssetManager::Get_Instance()) {
			BodySource = WW3DAssetManager::Get_Instance()->Create_Render_Obj(body_name);
		}
		if (!making) {
			WW3D::Enable_Overbright_Modify_On_Load(false);
		}
		FAIL_IF(BodySource == NULL) {
		}
		if (BodySource != NULL) {
			Unknown_496AE0(BodySource, true);
			BodyObj = BodySource->Clone();
			if (BodyObj) {
				BodyObj->Add_Ref();
				Unknown_496AE0(BodyObj, true);
				Unknown_496DB0(BodyObj);		// 0x77E9D0: every LOD level to its highest, the sub-objects' too

				BodyObj->Get_Obj_Space_Bounding_Sphere(ObjSphere);
				BodyObj->Get_Obj_Space_Bounding_Box(ObjBox);
				Update_Obj_Space_Bounding_Volumes();
			}
		}

		// 2. Build collision object
		if (WW3DAssetManager::Get_Instance()) {
			CollisionObj = WW3DAssetManager::Get_Instance()->Create_Render_Obj("avcollision");
			if (CollisionObj) {
				Unknown_496AE0(CollisionObj, true);
				if (BodyObj) {
					BodyObj->Add_Sub_Object_To_Bone(CollisionObj, "skel_reference");
				}
			}
		}

		// 3. Build head render object
		if (BodyObj) {
			char head_name[32];
			if (Desc.Avatar_Type == 0) {
				const char *gender = (Desc.Gender == 1) ? "AF" : "AM";
				const char *race = (Desc.Race == 1) ? "J" : ((Desc.Race == 2) ? "P" : "T");
				_snprintf(head_name, sizeof(head_name), "%s%sHEAD%02d",
					gender, race, (int)Desc.Head_Type);
			} else {
				_snprintf(head_name, sizeof(head_name), "ANPCHEAD%03d", (int)Desc.Head_Type);
			}

			bool making = WW3D::Is_Overbright_Modify_On_Load_Enabled();	// 0x77F000: the head loads overbright
			if (!making) {
				WW3D::Enable_Overbright_Modify_On_Load(true);
			}
			if (WW3DAssetManager::Get_Instance()) {
				HeadSource = WW3DAssetManager::Get_Instance()->Create_Render_Obj(head_name);
			}
			if (!making) {
				WW3D::Enable_Overbright_Modify_On_Load(false);
			}
			FAIL_IF(HeadSource == NULL) {
			}
			if (HeadSource != NULL) {
				Unknown_496AE0(HeadSource, true);
				HeadObj = HeadSource->Clone();
				if (HeadObj) {
					Unknown_496AE0(HeadObj, true);
					BodyObj->Add_Sub_Object_To_Bone(HeadObj, "K_HEAD");
				}
			}

			// 0x77DA20's face animation: three channels (mouth, brows, eyelids) morphing the head
			// skeleton's poses
			HAnimClass *anims[3];
			char anim_name[32];
			for (int i = 0; i < 3; i++) {
				_snprintf(anim_name, 0x20, "%sHSKEL.%sHANIM%02d", Unknown_B9CD30[Desc.Gender], Unknown_B9CD30[Desc.Gender], i);
				anims[i] = WW3DAssetManager::Get_Instance()->Get_HAnim(anim_name);
			}
			if (anims[0] == NULL || anims[1] == NULL || anims[2] == NULL) {
				Unknown_4BC260("Avatar Creation", "", __FILE__, __LINE__, "Missing required Anim files for Avatar head");
			} else {
				Unknown0BC = new HMorphAnimClass();
				Unknown0BC->Create_New_Morph(3, anims);
			}

			// 0x77DA20's accessories (player avatars only): each one's meshes join the head
			// prototype's LOD models, then the head object is cloned again (0x77F750)
			if (Desc.Avatar_Type == 0 && HeadSource) {
				// the weapons on the body's bones (+0x2D8 the unique one, +0x2DC the hip's, +0x2E0 the back's)
				if (Desc.Weapon_Unique_Num) {
					if (Desc.Gender == 0) {
						Unknown_77E0E0(0, "WEAPLEG", Desc.Weapon_Unique_Num, "WEAPON01", &WeaponHip);
					} else {
						Unknown_77E0E0(0, "WEAPARM", Desc.Weapon_Unique_Num, "WEAPON04", &WeaponHip);
					}
				}
				if (Desc.Weapon_Hip_Num) {
					Unknown_77E0E0(0, "WEAPHIP", Desc.Weapon_Hip_Num, "WEAPON02", &WeaponUnique);
				}
				if (Desc.Weapon_Back_Num) {
					Unknown_77E0E0(2, "WEAPBAK", Desc.Weapon_Back_Num, "WEAPON03", &WeaponBack);
				}
				Unknown_77F960("HAIR", Desc.Hair_Num, &HairSource);
				Unknown_77FFA0(Desc.Hair_Color, false);
				Unknown_77F960("EAR", Desc.Ear_Num, &Unknown2CC);
				Unknown_77F960("GOGGLE", Desc.Goggle_Num, &GoggleSource);
				Unknown_77F960("BEARD", Desc.Beard_Num, &BeardSource);
				char beard_name[64];
				for (int beard = 0; beard < 5; beard++) {
					_snprintf(beard_name, 0x40, "BEARD%d", beard);
					RenderObjClass *robj = HeadSource->Get_Sub_Object_By_Name(beard_name);
					if (robj) {
						Unknown_77FEC0(&robj, Desc.Beard_Color, 0);
						robj->Release_Ref();
					}
				}
				Unknown_77F750();
			}
		}

		// The client normalises the morph weights and shows them at once (player avatars only)
		if (Desc.Avatar_Type == 0) {
			float *weights = Desc.Body_Weight;
			float sum = 0.0f;
			Unknown29C = true;
			int i;
			for (i = 0; i < 5; i++) {
				G_ASSERT(weights[i]>=0.0f && weights[i]<=1.0f);
				sum += weights[i];
			}
			if (sum > 0.0f) {
				for (i = 0; i < 5; i++) {
					weights[i] = weights[i] / sum;
				}
			}
			weights = Desc.Head_Weight;
			sum = 0.0f;
			Unknown29D = true;
			for (i = 0; i < 5; i++) {
				G_ASSERT(weights[i]>=0.0f && weights[i]<=1.0f);
				sum += weights[i];
			}
			if (sum > 0.0f) {
				for (i = 0; i < 5; i++) {
					weights[i] = weights[i] / sum;
				}
			}
			Unknown_781410(1, 0);
			Unknown_781570(1, 0);
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
		if (BodyObj) {
			Unknown_781EA0();
		}
		Unknown_780810(env_maps);
	} else {
		Unknown221 = false;
	}
}

// 0x77C080
// NOT EXACT: functional for POC (D64)
CharacterViewAvatarClass::~CharacterViewAvatarClass()
{
	unsigned int i;
	for (i = 0; i < Unknown2A0.size(); i++) {
		if (Unknown2A0[i]) {
			delete Unknown2A0[i];
		}
	}
	Unknown2A0.clear();
	Unknown_784E00();
	if (PlayerNLP) {
		delete PlayerNLP;
		PlayerNLP = NULL;
	}
	Unknown_7885D0(false);
	Unknown_7885D0(true);
	if (Unknown370) {
		Unknown370->Set_Animation();
		Unknown370->Remove();
		Unknown370->Release_Ref();
		Unknown370 = NULL;
	}
	if (Unknown374) {
		Unknown374->Release_Ref();
		Unknown374 = NULL;
	}
	if (Unknown0BC) {
		Unknown0BC->Free_Morph();
		Unknown0BC->Release_Ref();
		Unknown0BC = NULL;
	}
	for (int a = 0; a < 2; a++) {
		if (Unknown0C0[a]) {
			Unknown0C0[a]->Release_Ref();
			Unknown0C0[a] = NULL;
		}
	}
	if (BodyObj) {
		if (WeaponHip) {
			BodyObj->Remove_Sub_Object(WeaponHip);
			WeaponHip->Release_Ref();
			WeaponHip = NULL;
		}
		if (WeaponUnique) {
			BodyObj->Remove_Sub_Object(WeaponUnique);
			WeaponUnique->Release_Ref();
			WeaponUnique = NULL;
		}
		if (WeaponBack) {
			BodyObj->Remove_Sub_Object(WeaponBack);
			WeaponBack->Release_Ref();
			WeaponBack = NULL;
		}
		if (CollisionObj) {
			BodyObj->Remove_Sub_Object(CollisionObj);
			CollisionObj->Release_Ref();
			CollisionObj = NULL;
		}
		if (HeadObj) {
			BodyObj->Remove_Sub_Object(HeadObj);
			HeadObj->Release_Ref();
			HeadObj = NULL;
		}
		if (MaskObj) {
			BodyObj->Remove_Sub_Object(MaskObj);
			MaskObj->Release_Ref();
			MaskObj = NULL;
		}
		int head_bone = BodyObj->Get_Bone_Index("K_HEAD");
		BodyObj->Release_Bone(head_bone);
		BodyObj->Remove();
		BodyObj->Release_Ref();
		BodyObj = NULL;
	}
	if (HeadObj) {
		HeadObj->Release_Ref();
		HeadObj = NULL;
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
	if (Unknown2C4) {
		Unknown2C4->Release_Ref();
		Unknown2C4 = NULL;
	}
	if (Unknown2CC) {
		Unknown2CC->Release_Ref();
		Unknown2CC = NULL;
	}
}

// 0x77C870
// through the implicit copy constructor (0x77C8F0, out of line)
RenderObjClass *CharacterViewAvatarClass::Clone() const
{
	return new CharacterViewAvatarClass(*this);
}

// 0x77D1E0
void CharacterViewAvatarClass::Set_Transform(const Matrix3D &m)
{
	if (BodyObj) {
		BodyObj->Set_Transform(m);
		RenderObjClass::Set_Transform(m);
		if (!Unknown36C) {
			Unknown330 = m;
		}
	}
}

// 0x77D2B0
void CharacterViewAvatarClass::Set_Position(const Vector3 &v)
{
	if (BodyObj) {
		BodyObj->Set_Position(v);
		RenderObjClass::Set_Position(v);
	}
}

// 0x77D3E0 (the body's LODs, when the avatar is not hidden)
void CharacterViewAvatarClass::Prepare_LOD(CameraClass &camera)
{
	if (Is_Not_Hidden_At_All() && BodyObj) {
		BodyObj->Prepare_LOD(camera);
	}
}

// 0x77D420
void CharacterViewAvatarClass::Render(RenderInfoClass &rinfo)
{
	if (BodyObj) {
		Set_Transform(Unknown330);
		BodyObj->Render(rinfo);
	}
}

// 0x77D460 (the body is the one sub-object: CompositeRenderObjClass's bounds come from it)
RenderObjClass *CharacterViewAvatarClass::Get_Sub_Object(int index) const
{
	if (index != 0) {
		return(NULL);
	}
	BodyObj->Add_Ref();
	return(BodyObj);
}

// 0x77D9F0
// NOT EXACT: shape (the client loads +0x234 again in each branch)
float CharacterViewAvatarClass::Unknown_77D9F0(bool a)
{
	if (a) {
		return(Unknown234 + 1.0f);
	}
	return(Unknown234);
}



















































// 0x77E0E0: the weapon model made into *slot and put on the body's bone; kind 0 '<gender><race><part><num>',
// 1 'a<race><part><num>', 2 'a<part><num>'
// NOT EXACT: functional for POC (D64), literal (__LINE__), relocation (the client's slot is a RefCountPtr)
void CharacterViewAvatarClass::Unknown_77E0E0(int kind, const char *part, int num, const char *bone, RenderObjClass **slot)
{
	if (*slot) {
		BodyObj->Remove_Sub_Object(*slot);
		(*slot)->Release_Ref();
		*slot = NULL;
	}
	char name[32];
	if (kind == 0) {
		_snprintf(name, 0x20, "%s%s%s%02d", Unknown_B9CD30[Desc.Gender], Unknown_B9CD38[Desc.Race], part, num);
	} else if (kind == 1) {
		_snprintf(name, 0x20, "a%s%s%02d", Unknown_B9CD38[Desc.Race], part, num);
	} else {
		_snprintf(name, 0x20, "a%s%02d", part, num);
	}
	bool making = WW3D::Is_Overbright_Modify_On_Load_Enabled();
	if (!making) {
		WW3D::Enable_Overbright_Modify_On_Load(true);
	}
	*slot = WW3DAssetManager::Get_Instance()->Create_Render_Obj(name);
	if (!making) {
		WW3D::Enable_Overbright_Modify_On_Load(false);
	}
	if (*slot) {
		Unknown_496AE0(*slot, true);
		BodyObj->Add_Sub_Object_To_Bone(*slot, bone);
		return;
	}
	Unknown_4BC000("Avatar Creation", "", __FILE__, __LINE__, "Avatar asset file does not exists %s", name);
}

// 0x77E3D0
void CharacterViewAvatarClass::Unknown_77E3D0(int a)
{
	Unknown204 = a;
}





































































// 0x77E830: the body morph weights, normalised, into Desc.Body_Weight (+0x29C: step them, 0x781410)
// (the G_ASSERT on the client's line 725)
void CharacterViewAvatarClass::Unknown_77E830(const float *weights)
{
	float sum = 0.0f;
	Unknown29C = true;
	int i;
	for (i = 0; i < 5; i++) {
		G_ASSERT(weights[i]>=0.0f && weights[i]<=1.0f);
		sum += weights[i];
	}
	if (sum > 0.0f) {
		for (i = 0; i < 5; i++) {
			Desc.Body_Weight[i] = weights[i] / sum;
		}
	}
}






// 0x77E900: the head morph weights, normalised, into Desc.Head_Weight (+0x29D: step them, 0x781570)
// (the G_ASSERT on the client's line 748)
void CharacterViewAvatarClass::Unknown_77E900(const float *weights)
{
	float sum = 0.0f;
	Unknown29D = true;
	int i;
	for (i = 0; i < 5; i++) {
		G_ASSERT(weights[i]>=0.0f && weights[i]<=1.0f);
		sum += weights[i];
	}
	if (sum > 0.0f) {
		for (i = 0; i < 5; i++) {
			Desc.Head_Weight[i] = weights[i] / sum;
		}
	}
}

// 0x77E9D0: Body_Type / Pants_Type's body '<gender><race>BODY%02d%02d' ('ANPCBODY%03d' for an NPC) loaded
// again: the head, weapons and collision objects come off the old one and onto the new one; +0x234 is
// K_HEAD's height above skel_reference; morph: the body weights shown at once (0x781410)
// NOT EXACT: functional for POC (D64), literal (__LINE__), inline-budget (the client's RefCountPtr temporaries)
void CharacterViewAvatarClass::Unknown_77E9D0(int body, int pants, bool morph)
{
	char body_name[32];
	Desc.Pants_Type = (char)pants;
	Desc.Body_Type = (char)body;
	if (BodyObj) {
		if (HeadObj) BodyObj->Remove_Sub_Object(HeadObj);
		if (WeaponUnique) BodyObj->Remove_Sub_Object(WeaponUnique);
		if (WeaponHip) BodyObj->Remove_Sub_Object(WeaponHip);
		if (WeaponBack) BodyObj->Remove_Sub_Object(WeaponBack);
		if (CollisionObj) BodyObj->Remove_Sub_Object(CollisionObj);
		if (Unknown2C4) BodyObj->Remove_Sub_Object(Unknown2C4);
		BodyObj->Release_Bone(BodyObj->Get_Bone_Index("K_Head"));
		BodyObj->Remove();
		BodyObj->Release_Ref();
		BodyObj = NULL;
	}
	if (BodySource) {
		BodySource->Release_Ref();
		BodySource = NULL;
	}
	bool making = WW3D::Is_Overbright_Modify_On_Load_Enabled();
	if (!making) {
		WW3D::Enable_Overbright_Modify_On_Load(true);
	}
	if (Desc.Avatar_Type == 0) {
		_snprintf(body_name, 0x20, "%s%sBODY%02d%02d", Unknown_B9CD30[Desc.Gender], Unknown_B9CD38[Desc.Race], (int)Desc.Body_Type, (int)Desc.Pants_Type);
	} else {
		_snprintf(body_name, 0x20, "ANPCBODY%03d", (int)Desc.Body_Type);
	}
	BodySource = WW3DAssetManager::Get_Instance()->Create_Render_Obj(body_name);
	if (!making) {
		WW3D::Enable_Overbright_Modify_On_Load(false);
	}
	if (BodySource == NULL) {
		Unknown_4BC260("Avatar Creation", "", __FILE__, 826, "Error loading up the body asset: %s", body_name);
		return;
	}
	Unknown_496AE0(BodySource, true);
	BodyObj = BodySource->Clone();
	Unknown_496AE0(BodyObj, true);
	Unknown_496DB0(BodyObj);
	Set_Name(BodyObj->Get_Name());
	Set_Base_Model_Name(BodyObj->Get_Base_Model_Name());
	Update_Obj_Space_Bounding_Volumes();
	if (HeadObj) BodyObj->Add_Sub_Object_To_Bone(HeadObj, "K_HEAD");
	if (WeaponUnique) BodyObj->Add_Sub_Object_To_Bone(WeaponUnique, "WEAPON02");
	if (WeaponBack) BodyObj->Add_Sub_Object_To_Bone(WeaponBack, "WEAPON03");
	if (CollisionObj) BodyObj->Add_Sub_Object_To_Bone(CollisionObj, "skel_reference");
	if (Unknown2C4) BodyObj->Add_Sub_Object_To_Bone(Unknown2C4, "skel_reference");
	if (Desc.Gender == 0 && WeaponHip) {
		BodyObj->Add_Sub_Object_To_Bone(WeaponHip, "WEAPON01");
	} else if (WeaponHip) {
		BodyObj->Add_Sub_Object_To_Bone(WeaponHip, "WEAPON04");
	}
	float head = BodyObj->Get_Bone_Transform("K_HEAD").Get_Z_Translation();
	Unknown234 = head - BodyObj->Get_Bone_Transform("skel_reference").Get_Z_Translation();
	if (morph) {
		Unknown_781410(1, 0);
	}
}

// 0x77F000: Head_Type's head loaded again as the head prototype ('<gender><race>HEAD%02d', 'ANPCHEAD%03d'
// for an NPC); the accessories' meshes come off the old one and onto the new one (0x789D80), the hair
// colour onto its hair and beard, then the head object is cloned again (0x77F750)
// NOT EXACT: functional for POC (D64), literal (__LINE__), inline-budget (the client's RefCountPtr temporaries)
void CharacterViewAvatarClass::Unknown_77F000(int num)
{
	char name[64];
	char head_name[32];
	int i;
	Desc.Head_Type = (char)num;
	if (BodyObj == NULL) {
		return;
	}
	if (HeadSource) {
		const char *parts[4] = { "HAIR", "EAR", "GOGGLE", "BEARD" };
		RenderObjClass *slots[4] = { HairSource, Unknown2CC, GoggleSource, BeardSource };
		for (int p = 0; p < 4; p++) {
			if (slots[p] && HeadSource) {
				for (i = 0; i < 5; i++) {
					_snprintf(name, 0x40, "%s%d", parts[p], i);
					RenderObjClass *robj = HeadSource->Get_Sub_Object_By_Name(name);
					if (robj) {
						HeadSource->Remove_Sub_Object(robj);
						robj->Release_Ref();
					}
				}
			}
		}
		HeadSource->Release_Ref();
		HeadSource = NULL;
	}
	bool making = WW3D::Is_Overbright_Modify_On_Load_Enabled();
	if (!making) {
		WW3D::Enable_Overbright_Modify_On_Load(true);
	}
	if (Desc.Avatar_Type == 0) {
		_snprintf(head_name, 0x20, "%s%sHEAD%02d", Unknown_B9CD30[Desc.Gender], Unknown_B9CD38[Desc.Race], (int)Desc.Head_Type);
	} else {
		_snprintf(head_name, 0x20, "ANPCHEAD%03d", (int)Desc.Head_Type);
	}
	HeadSource = WW3DAssetManager::Get_Instance()->Create_Render_Obj(head_name);
	if (!making) {
		WW3D::Enable_Overbright_Modify_On_Load(false);
	}
	if (HeadSource == NULL) {
		Unknown_4BC260("Avatar Creation", "", __FILE__, 924, "Error loading up the head asset: %s", head_name);
		return;
	}
	Unknown_496AE0(HeadSource, true);
	if (HairSource) {
		Unknown_789D80(&HairSource, "HAIR");
		for (i = 0; i < 5; i++) {
			_snprintf(name, 0x40, "HAIR%d", i);
			RenderObjClass *robj = HeadSource->Get_Sub_Object_By_Name(name);
			if (robj) {
				Unknown_77FEC0(&robj, Desc.Hair_Color, 0);
				robj->Release_Ref();
			}
		}
		Unknown_7802F0(Desc.Hair_Color, false);
	}
	if (Unknown2CC) {
		Unknown_789D80(&Unknown2CC, "EAR");
	}
	if (GoggleSource) {
		Unknown_789D80(&GoggleSource, "GOGGLE");
	}
	if (BeardSource) {
		Unknown_789D80(&BeardSource, "BEARD");
		Desc.Beard_Color = Desc.Hair_Color;
		for (i = 0; i < 5; i++) {
			_snprintf(name, 0x40, "BEARD%d", i);
			RenderObjClass *robj = HeadSource->Get_Sub_Object_By_Name(name);
			if (robj) {
				Unknown_77FEC0(&robj, Desc.Hair_Color, 0);
				robj->Release_Ref();
			}
		}
	}
	Unknown_77F750();
}















































// 0x77F750: the head object cloned again from the head prototype (with the accessories it now
// holds) and put on the body's K_HEAD bone
// NOT EXACT: functional for POC (D64), literal (__LINE__ of the FAIL_IF; the client's members are RefCountPtrs)
void CharacterViewAvatarClass::Unknown_77F750()
{
	FAIL_IF(!HeadSource || !BodyObj) {
		return;
	}
	if (HeadObj) {
		BodyObj->Remove_Sub_Object(HeadObj);
		HeadObj->Release_Ref();
		HeadObj = NULL;
	}
	HeadObj = HeadSource->Clone();
	Unknown_496AE0(HeadObj, true);
	Unknown_496DB0(HeadObj);
	if (Desc.Avatar_Type == 0) {
		Unknown_780F30();
		Unknown_7804A0(Desc.Eye_Color);
		Unknown_7857D0(Desc.Head_Texture_Num);
		Unknown_7859F0(Desc.Tattoo_Texture_Num);
		Unknown_785C00(Desc.Tattoo_Offset);
	}
	BodyObj->Add_Sub_Object_To_Bone(HeadObj, "K_HEAD");
}







// 0x77F960: the accessory '<gender><race><part><num>' (AFTHAIR03) made into *slot; the meshes
// '<part>0'..'<part>4' of the head prototype are removed first, the new ones added by 0x789D80
// NOT EXACT: functional for POC (D64), literal (__LINE__), relocation (the client's slot is a RefCountPtr)
void CharacterViewAvatarClass::Unknown_77F960(const char *part, int num, RenderObjClass **slot)
{
	FAIL_IF(!HeadSource) {
		return;
	}
	char name[64];
	if (HeadSource && part) {
		for (int i = 0; i < 5; i++) {
			_snprintf(name, 0x40, "%s%d", part, i);
			RenderObjClass *robj = HeadSource->Get_Sub_Object_By_Name(name);
			if (robj) {
				HeadSource->Remove_Sub_Object(robj);
				robj->Release_Ref();
			}
		}
	}
	if (*slot) {
		(*slot)->Release_Ref();
		*slot = NULL;
	}
	bool making = WW3D::Is_Overbright_Modify_On_Load_Enabled();
	if (!making) {
		WW3D::Enable_Overbright_Modify_On_Load(true);
	}
	char model[32];
	_snprintf(model, 0x20, "%s%s%s%02d", Unknown_B9CD30[Desc.Gender], Unknown_B9CD38[Desc.Race], part, num);
	RenderObjClass *proto = WW3DAssetManager::Get_Instance()->Create_Render_Obj(model);
	if (!making) {
		WW3D::Enable_Overbright_Modify_On_Load(false);
	}
	if (proto) {
		*slot = proto->Clone();
		Unknown_496AE0(*slot, true);
		Unknown_789D80(slot, part);
		proto->Release_Ref();
	}
}

// 0x77FC20: Hair_Num's hair ('HAIR'), Hair_Color onto it, then (rebuild) the head object cloned again
// NOT EXACT: functional for POC (D64), not matched yet (the client's part name may be a shared string)
void CharacterViewAvatarClass::Unknown_77FC20(int num, bool rebuild)
{
	Desc.Hair_Num = (char)num;
	Unknown_77F960("HAIR", num, &HairSource);
	Unknown_77FFA0(Desc.Hair_Color, false);
	if (rebuild) {
		Unknown_77F750();
	}
}

// 0x77FC90: Ear_Num's ear pieces ('EAR'), then (rebuild) the head object cloned again
void CharacterViewAvatarClass::Unknown_77FC90(int num, bool rebuild)
{
	Desc.Ear_Num = (char)num;
	Unknown_77F960("EAR", num, &Unknown2CC);
	if (rebuild) {
		Unknown_77F750();
	}
}

// 0x77FCE0: Goggle_Num's glasses ('GOGGLE'), then (rebuild) the head object cloned again
void CharacterViewAvatarClass::Unknown_77FCE0(int num, bool rebuild)
{
	Desc.Goggle_Num = (char)num;
	Unknown_77F960("GOGGLE", num, &GoggleSource);
	if (rebuild) {
		Unknown_77F750();
	}
}

// 0x77FD30: Beard_Num's beard ('BEARD'), Beard_Color onto it, then (rebuild) the head object cloned again
// NOT EXACT: functional for POC (D64), not matched yet (the client's part name may be a shared string)
void CharacterViewAvatarClass::Unknown_77FD30(int num, bool rebuild)
{
	Desc.Beard_Num = (char)num;
	Unknown_77F960("BEARD", num, &BeardSource);
	Unknown_7802F0(Desc.Beard_Color, false);
	if (rebuild) {
		Unknown_77F750();
	}
}

// 0x77FEC0: the colour onto the mesh's vertex material (ambient and diffuse)
// NOT EXACT: functional for POC (D64), inline-budget (the client's RefCountPtr calls)
void CharacterViewAvatarClass::Unknown_77FEC0(RenderObjClass **robj, const Vector3 &color, int index)
{
	if (*robj) {
		MaterialInfoClass *info = (*robj)->Get_Material_Info();
		if (info) {
			VertexMaterialClass *material = info->Peek_Vertex_Material(index);
			if (material) {
				material->Set_Ambient(color);
				material->Set_Diffuse(color);
			}
		}
		Unknown_496AE0(*robj, true);
		if (info) {
			info->Release_Ref();
		}
	}
}

// 0x77FFA0: the hair colour onto the head prototype's HAIR0..4 and BEARD0..4 meshes; rebuild: 0x77F750
// NOT EXACT: functional for POC (D64), inline-budget (the client's RefCountPtr calls; 0x77FEC0 inline)
void CharacterViewAvatarClass::Unknown_77FFA0(const Vector3 &color, bool rebuild)
{
	char name[64];
	int i;
	Desc.Hair_Color = color;
	for (i = 0; i < 5; i++) {
		_snprintf(name, 0x40, "HAIR%d", i);
		RenderObjClass *robj = HeadSource->Get_Sub_Object_By_Name(name);
		if (robj) {
			Unknown_77FEC0(&robj, color, 0);
			robj->Release_Ref();
		}
	}
	Unknown_7802F0(color, false);
	if (rebuild) {
		Unknown_77F750();
	}
}

// 0x7802F0: the beard colour onto the head prototype's BEARD0..4 meshes; rebuild: 0x77F750
// NOT EXACT: functional for POC (D64), inline-budget (the client's RefCountPtr calls; 0x77FEC0 inline)
void CharacterViewAvatarClass::Unknown_7802F0(const Vector3 &color, bool rebuild)
{
	char name[64];
	Desc.Beard_Color = color;
	for (int i = 0; i < 5; i++) {
		_snprintf(name, 0x40, "BEARD%d", i);
		RenderObjClass *robj = HeadSource->Get_Sub_Object_By_Name(name);
		if (robj) {
			Unknown_77FEC0(&robj, color, 0);
			robj->Release_Ref();
		}
	}
	if (rebuild) {
		Unknown_77F750();
	}
}

// 0x7804A0: the eye colour onto the head object's LEYE010..4 and REYE010..4 meshes (vertex material 1)
// NOT EXACT: functional for POC (D64), inline-budget (the client's RefCountPtr calls; 0x77FEC0 inline)
void CharacterViewAvatarClass::Unknown_7804A0(const Vector3 &color)
{
	Desc.Eye_Color = color;
	if (HeadObj) {
		for (int side = 0; side < 2; side++) {
			for (int i = 0; i < 5; i++) {
				char name[32];
				_snprintf(name, 0x20, "%sEYE01%d", Unknown_B9CD44[side], i);
				RenderObjClass *robj = HeadObj->Get_Sub_Object_By_Name(name);
				if (robj) {
					Unknown_77FEC0(&robj, color, 1);
					robj->Release_Ref();
				}
			}
		}
	}
}

// 0x780680: a colour by index: 0 Skin_Color, 1 / 2 the shirt's primary / secondary, 3 / 4 the pants'
void CharacterViewAvatarClass::Unknown_780680(int index, Vector3 color)
{
	switch (index) {
		case 0: Desc.Skin_Color = color; break;
		case 1: Desc.Shirt_Primary_Color = color; break;
		case 2: Desc.Shirt_Secondary_Color = color; break;
		case 3: Desc.Pants_Primary_Color = color; break;
		case 4: Desc.Pants_Secondary_Color = color; break;
	}
}

// 0x780790: a metal by index: 1 / 2 the shirt's primary / secondary, 3 / 4 the pants'
void CharacterViewAvatarClass::Unknown_780790(int index, int metal)
{
	switch (index) {
		case 1: Desc.Shirt_Primary_Metal = metal; break;
		case 2: Desc.Shirt_Secondary_Metal = metal; break;
		case 3: Desc.Pants_Primary_Metal = metal; break;
		case 4: Desc.Pants_Secondary_Metal = metal; break;
	}
}

// 0x780810: two colorization regions from the description, shirt (primary, secondary) and pants (primary, secondary):
// a half's metal and its colour, white when the metal is an environment map (>= 0); then shiputils.cpp's 0x80FCE0 on
// the body and the head with the skin colour
// NOT EXACT: functional for POC (D64), inline-budget (the client's vector insert and 0x780AC0 copies)
void CharacterViewAvatarClass::Unknown_780810(EnvironmentMapManagerClass *env_maps)
{
	if (env_maps) {
		std::vector<ColorizationRegionStruct> regions;
		ColorizationRegionStruct region;
		region.Unknown00.Unknown00 = Desc.Shirt_Primary_Metal;
		region.Unknown00.Unknown04 = (Desc.Shirt_Primary_Metal < 0) ? Desc.Shirt_Primary_Color : Vector3(1.0f, 1.0f, 1.0f);
		region.Unknown10.Unknown00 = Desc.Shirt_Secondary_Metal;
		region.Unknown10.Unknown04 = (Desc.Shirt_Secondary_Metal < 0) ? Desc.Shirt_Secondary_Color : Vector3(1.0f, 1.0f, 1.0f);
		regions.insert(regions.end(), 1, region);
		region.Unknown00.Unknown00 = Desc.Pants_Primary_Metal;
		region.Unknown00.Unknown04 = (Desc.Pants_Primary_Metal < 0) ? Desc.Pants_Primary_Color : Vector3(1.0f, 1.0f, 1.0f);
		region.Unknown10.Unknown00 = Desc.Pants_Secondary_Metal;
		region.Unknown10.Unknown04 = (Desc.Pants_Secondary_Metal < 0) ? Desc.Pants_Secondary_Color : Vector3(1.0f, 1.0f, 1.0f);
		regions.insert(regions.end(), 1, region);
		Unknown_80FCE0(BodyObj, regions, env_maps, &Desc.Skin_Color);
		Unknown_80FCE0(HeadObj, regions, env_maps, &Desc.Skin_Color);
	}
}


















































































































// 0x780AF0
// NOT EXACT: functional for POC (D64), literal (__LINE__)
void CharacterViewAvatarClass::Unknown_780AF0()
{
	FAIL_IF(!BodyObj || !BodySource) { return; }
	Vector3 scale(0.0f, 0.0f, 0.0f); int i;
	for (i = 0; i < 5; i++) {
		float w = Unknown274[i]; Vector3 morph;
		Desc.Get_Body_Morph(morph, i);
		scale.X += w * morph.X; scale.Y += w * morph.Y;
		scale.Z += w * morph.Z;
	}
	if (scale.X <= 0.0f || scale.Y <= 0.0f || scale.Z <= 0.0f) {
		Unknown_4BC260("Avatar Morphing", "", __FILE__, __LINE__, "Encountered bad scale values");
		return;
	}
	const char *names[] = { "ARM0", "ARM1", "ARM2", "ARM3", "ARM4", "SHIRT0", "SHIRT1", "SHIRT2", "SHIRT3", "SHIRT4",
		"PANT0", "PANT1", "PANT2", "PANT3", "PANT4", NULL };
	for (i = 0; names[i] != NULL; i++) {
		RefCountPtr<RenderObjClass> obj = Create_Get(BodySource->Get_Sub_Object_By_Name(names[i]));
		if (obj != NULL && obj->Class_ID() == RenderObjClass::CLASSID_MESH) {
			RefCountPtr<MeshClass> source = Create_Get((MeshClass *)BodySource->Get_Sub_Object_By_Name(names[i]));
			RefCountPtr<MeshClass> dest = Create_Get((MeshClass *)BodyObj->Get_Sub_Object_By_Name(names[i]));
			Unknown_7812A0(source.Peek(), dest.Peek());
			dest->Scale(scale.Y, scale.Z, scale.X);
		}
	}
	Vector3 tree_scale(scale.Z, scale.X, scale.Y);
	HTreeClass *tree = HTreeClass::Alter_Avatar_HTree(BodySource->Get_HTree(), tree_scale);
	((HLodClass *)BodyObj)->Set_HTree(tree);
	if (tree) {
		delete tree;
	}
}




















// 0x780F30
// NOT EXACT: functional for POC (D64), literal (__LINE__)
void CharacterViewAvatarClass::Unknown_780F30()
{
	FAIL_IF(HeadObj == 0 || HeadSource == 0) { return; }
	Vector3 scale(0.0f, 0.0f, 0.0f);
	int i;
	for (i = 0; i < 5; i++) {
		float w = Unknown288[i];
		Vector3 morph;
		Desc.Get_Head_Morph(morph, i);
		scale.X += w * morph.X;
		scale.Y += w * morph.Y;
		scale.Z += w * morph.Z;
	}
	if (scale.X <= 0.0f || scale.Y <= 0.0f || scale.Z <= 0.0f) {
		Unknown_4BC260("Avatar Morphing", "", __FILE__, __LINE__, "Encountered bad head scale values");
		return;
	}
	for (i = 0; i < HeadObj->Get_Num_Sub_Objects(); i++) {
		RefCountPtr<RenderObjClass> obj = Create_Get(HeadSource->Get_Sub_Object(i));
		if (obj->Class_ID() == RenderObjClass::CLASSID_MESH) {
			RefCountPtr<MeshClass> source = Create_Get((MeshClass *)HeadSource->Get_Sub_Object(i));
			RefCountPtr<MeshClass> dest = Create_Get((MeshClass *)HeadObj->Get_Sub_Object(i));
			Unknown_7812A0(source.Peek(), dest.Peek());
			dest->Scale(scale.X, scale.Y, scale.Z);
		}
	}
	HTreeClass *tree = HTreeClass::Alter_Avatar_HTree(HeadSource->Get_HTree(), scale);
	((HLodClass *)HeadObj)->Set_HTree(tree);
	if (tree) {
		delete tree;
	}
}














// 0x7812A0
// (the FAIL_IF on the client's line 1415)
void CharacterViewAvatarClass::Unknown_7812A0(MeshClass *source_mesh, MeshClass *dest_mesh)
{
	FAIL_IF(!source_mesh || !dest_mesh) {
		return;
	}
	RefCountPtr<MeshModelClass> source = Create_Get(source_mesh->Get_Model());
	RefCountPtr<MeshModelClass> dest = Create_Get(dest_mesh->Get_Model());
	int count = source->Get_Vertex_Count();
	Vector3 *from = source->Get_Vertex_Array();
	Vector3 *to = dest->Get_Vertex_Array();
	for (int i = 0; i < count; i++) {
		to[i] = from[i];
	}
}











// 0x781410
// (1 - keep computed once, before the loop)
void CharacterViewAvatarClass::Unknown_781410(int now, int steps)
{
	if (BodyObj == NULL) {
		Unknown_4BC260("Avatar Morphing", "", __FILE__, __LINE__, "Missing main avatar body object");
		Unknown29C = false;
		Unknown29D = false;
		return;
	}
	float keep = 1.0f;
	bool moving = false;
	for (; steps > 0; steps--) {
		keep *= 0.75f;
	}
	float rest = 1.0f - keep;
	for (int i = 0; i < 5; i++) {
		if (now == 1 || fabs(Unknown274[i] - Desc.Body_Weight[i]) < 0.0001) {
			Unknown274[i] = Desc.Body_Weight[i];
		} else {
			moving = true;
			Unknown274[i] = keep * Unknown274[i] + rest * Desc.Body_Weight[i];
		}
	}
	if (!moving) {
		Unknown29C = false;
	}
	Unknown_780AF0();
	Matrix3D head = BodyObj->Get_Bone_Transform("K_HEAD");
	Unknown234 = head[2][3] - BodyObj->Get_Bone_Transform("skel_reference")[2][3];
}
















// 0x781570
// NOT EXACT: functional for POC (D64), literal (__LINE__)
void CharacterViewAvatarClass::Unknown_781570(int now, int steps)
{
	if (HeadObj == NULL) {
		Unknown_4BC260("Avatar Morphing", "", __FILE__, __LINE__, "Missing main avatar head object");
		Unknown29D = false;
		return;
	}
	float keep = 1.0f;
	bool moving = false;
	for (; steps > 0; steps--) {
		keep *= 0.75f;
	}
	float rest = 1.0f - keep;
	for (int i = 0; i < 5; i++) {
		if (now == 1 || fabs(Unknown288[i] - Desc.Head_Weight[i]) < 0.0001) {
			Unknown288[i] = Desc.Head_Weight[i];
		} else {
			moving = true;
			Unknown288[i] = keep * Unknown288[i] + rest * Desc.Head_Weight[i];
		}
	}
	if (!moving) {
		Unknown29D = false;
	}
	Unknown_780F30();
}

// 0x781880
// (push_back: VC6 vector's insert inlined)
void CharacterViewAvatarClass::Unknown_781880(char anim, const char *text, int mood)
{
	Unknown2A0.push_back(new Unknown_AvatarQueueStruct(anim, text, mood));
}

// 0x781D10
// one _snprintf per branch (tail-merged at the format)
std::string CharacterViewAvatarClass::Unknown_781D10(int anim)
{
	char name[256];
	if (Desc.Skeleton.size() != 0) {
		_snprintf(name, 0xFF, "%s.ABANIM%02d-%02d", Desc.Skeleton.c_str(), (int)Desc.Personality, anim);
	} else {
		_snprintf(name, 0xFF, "%s.ABANIM%02d-%02d", Unknown_B9CD28[Desc.Gender], (int)Desc.Personality, anim);
	}



	return(std::string(name));
}

// 0x781EA0
// NOT EXACT: functional for POC (D64), relocation (0x78F640 a POC stand-in; the face animation calls not built)
void CharacterViewAvatarClass::Unknown_781EA0()
{
	int delay = PlayerNLP->Personality->Get_Fidget_Delay();
	Unknown220 = true;
	if (SpeechSound) {
		SpeechSound->Stop(1);
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
		Unknown250 = 0.0f;
		Unknown258 = 0.0f;
		Unknown264 = 0.0f;
		Unknown268 = 0.0f;
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
	if (Unknown0BC && (Unknown0C0[0] || Unknown0C0[1])) {
		Unknown0BC->Release_Keys();
		Unknown_782FF0("");
		Unknown_782CA0();
		Unknown_783470();
	}
}

// 0x782200
// NOT EXACT: functional for POC (D64)
void CharacterViewAvatarClass::Unknown_782200()
{
	std::string name;
	Unknown220 = false;
	Unknown250 = 0.0f;
	Unknown258 = 0.0f;
	Unknown264 = 0.0f;
	Unknown268 = 0.0f;
	if (Unknown248 > 0) {
		name = Unknown_781D10(PlayerNLP->Unknown_78EE00(Unknown248, Unknown210 != 0));
		Unknown_7831C0(name.c_str());
		int frames;
		if (Unknown0C0[1] == NULL) {
			frames = Unknown0C0[0]->Get_Num_Frames();
		} else {
			frames = Unknown0C0[1]->Get_Num_Frames();
		}
		Unknown248 -= frames;
		if (Unknown248 < 0) {
			Unknown248 = 0;
		}
		return;
	}
	Unknown_AvatarQueueStruct *entry;
	if (Unknown2A0.size() != 0 && (entry = Unknown2A0[0]) != NULL) {
		int anim;
		if (Unknown210 != 0) {
			int delay = PlayerNLP->Personality->Get_Speech_Delay();
			int phonemes[256];
			int count = Unknown_BE856C->Phonemes.Unknown_791400(entry->Text, phonemes, 0xFF);
			anim = PlayerNLP->Unknown_78EE00(count * delay, true);
			name = Unknown_781D10(anim);
		} else {
			anim = entry->Anim;
			if (entry->Mood >= 0) {
				Desc.Mood_Type = entry->Mood;
			}
			Unknown22C = 0.0f;
			Unknown244 = 0;
			name = Unknown_781D10(anim);
		}
		Unknown_7831C0(name.c_str());
		if (Unknown0BC && (Unknown0C0[0] || Unknown0C0[1])) {
			Unknown0BC->Release_Keys();
			Unknown_782FF0(entry->Text);
			name = Unknown_781D10(anim);
			if (!Unknown_7829B0(name.c_str())) {
				Unknown_782CA0();
			}
			Unknown_783470();
		}
		unsigned int i;
		for (i = 0; i < Unknown2A0.size(); i++) {
			if (Unknown2A0[i] == entry) {
				Unknown2A0.erase(Unknown2A0.begin() + i);
				delete entry;
				break;
			}
		}
	}
	if ((Unknown2A0.size() == 0 || Unknown2A0[0] == NULL) && Unknown248 == 0) {
		Unknown208 = 0;
	}
}

// 0x7826A0
// NOT EXACT: functional for POC (D64)
void CharacterViewAvatarClass::Unknown_7826A0()
{
	Unknown220 = false;
	Unknown22C = 0.0f;
	Unknown244 = 0;
	Unknown238 = 0;
	if (Unknown20C != 2) {
		if (Unknown20C == 0) {
			Unknown20C = 1;
		} else {
			Unknown215 = true;
		}
	}
	char name[32];
	int anim;
	if ((Unknown2F8 >> 1) & 1) {
		anim = PlayerNLP->Unknown_78F640("walk_backward");
	} else if (!Unknown2F4) {
		anim = PlayerNLP->Unknown_78F640("walk_forward");
	} else {
		anim = PlayerNLP->Unknown_78F640("run_forward");
	}
	_snprintf(name, 0x20, "%s.ABANIM%02d-%02d", Unknown_B9CD28[Desc.Gender], (int)Desc.Personality, anim);
	Unknown_7831C0(name);
	if (Unknown0BC && (Unknown0C0[0] || Unknown0C0[1])) {
		Unknown0BC->Release_Keys();
		Unknown_782CA0();
		Unknown_782FF0("");
		Unknown_783470();
	}
}

// 0x782850
// NLP_Of: the client loads ecx before the argument
void CharacterViewAvatarClass::Unknown_782850()
{
	char tag[16];
	char name[32];
	Unknown220 = false;
	if (Unknown210 == 1) {
		strcpy(tag, "sit_down");
		Unknown210 = 3;
	} else if (Unknown210 == 2) {
		strcpy(tag, "stand_up");
		Unknown210 = 0;
	}
	int anim = NLP_Of(this)->Unknown_78F640(tag);
	_snprintf(name, 0x20, "%s.ABANIM%02d-%02d", Unknown_B9CD28[Desc.Gender], (int)Desc.Personality, anim);
	Unknown_7831C0(name);
	if (Unknown0BC && (Unknown0C0[0] || Unknown0C0[1])) {
		Unknown0BC->Release_Keys();
		Unknown_782CA0();
		Unknown_782FF0("");
		Unknown_783470();
	}
	Unknown208 = 0;
}

// 0x7829B0
// NOT EXACT: functional for POC (D64)
bool CharacterViewAvatarClass::Unknown_7829B0(const char *name)
{
	static char *moods[] = { "Mormal", "Angry", "Happy", "Puzzled", NULL };	// 0xB9CD4C
	static char *brows[] = { "Open1", "Close1", "Open2", "Close2", "Wink", NULL };	// 0xB9CD60
	static char *eyes[] = { "Straight", "Left", "Fullleft", "Right", "Fullright", NULL };	// 0xB9CD78
	INIClass ini(name);
	if (!ini.Is_Loaded()) {
		return(false);
	}
	int frames;
	if (Unknown0C0[1]) {
		frames = Unknown0C0[1]->Get_Num_Frames();
	} else if (Unknown0C0[0]) {
		frames = Unknown0C0[0]->Get_Num_Frames();
	} else {
		return(false);
	}
	if (frames <= 2) {
		return(false);
	}
	Unknown0BC->Insert_Morph_Key(1, 0, Unknown240);
	Unknown0BC->Insert_Morph_Key(2, 0, 0);
	int mood = 0;
	char section[64];
	for (int index = 0; ; index++) {
		_snprintf(section, 0x40, "Index-%04d", index);
		if (!ini.Section_Present(section)) {
			break;
		}
		int frame = ini.Get_Int(section, "Frame", -1);
		if (frame > 0 && frame < frames - 1) {
			if (ini.Is_Present(section, "Mood")) {
				mood = ini.Get_List_Index(section, "Mood", 0, moods);
			}
			int brow = ini.Get_List_Index(section, "Brow", -1, brows);
			if (brow >= 0) {
				brow += mood * 5;
				Unknown240 = brow;
				Unknown0BC->Insert_Morph_Key(1, frame, brow);
			}
			int eye = ini.Get_List_Index(section, "Eye", -1, eyes);
			if (eye >= 0) {
				Unknown0BC->Insert_Morph_Key(2, frame, eye);
			}
		}
	}
	Unknown240 = Desc.Mood_Type * 5;
	Unknown0BC->Insert_Morph_Key(1, frames - 1, Unknown240);
	Unknown0BC->Insert_Morph_Key(2, frames - 1, 0);
	return(true);
}

// 0x782CA0
// NOT EXACT: functional for POC (D64)
void CharacterViewAvatarClass::Unknown_782CA0()
{
	int delay = PlayerNLP->Personality->Get_Blink_Delay();
	int step = 1;
	if (delay > 0) {
		step = delay;
	}
	int frames;
	if (Unknown0C0[1] == NULL) {
		frames = Unknown0C0[0]->Get_Num_Frames() + Unknown248;
	} else {
		frames = Unknown0C0[1]->Get_Num_Frames() + Unknown248;
	}
	int count;
	if (frames < step) {
		count = Unknown_BDBB84(0, 1);
		step = frames;
	} else {
		int n = Unknown_BDBB84(1, frames / step);
		count = frames / 4;
		if (n <= count) {
			count = n;
		}
		step = frames / count;
	}
	int brow = Desc.Mood_Type * 5;
	int r;
	if (Unknown208 == 1) {
		r = Unknown_BDBB84(0, 1);
	} else {
		r = Unknown_BDBB84(0, 3);
	}
	if (r == 1) {
		brow += 2;
	}
	Unknown0BC->Insert_Morph_Key(1, 0, Unknown240);
	int i;
	int at;
	for (i = 0, at = 2; i < count; i++, at += step) {
		int key = Unknown_BDBB84(at, at - 7 + step);
		Unknown0BC->Insert_Morph_Key(1, key, brow);
		Unknown0BC->Insert_Morph_Key(1, key + 2, brow + 1);
		Unknown0BC->Insert_Morph_Key(1, key + 4, brow);
	}
	Unknown0BC->Insert_Morph_Key(1, frames - 1, brow);
	Unknown240 = brow;
	Unknown0BC->Insert_Morph_Key(2, 0, 0);
	for (i = 0, at = 0; i < count; i++, at += step) {
		int shut = Unknown_BDBB84(0, 1) * 2 + 1;
		int hold = Unknown_BDBB84(2, step / 4);
		int key = Unknown_BDBB84(at + 1, at - hold - 4 + step);
		Unknown0BC->Insert_Morph_Key(2, key, 0);
		Unknown0BC->Insert_Morph_Key(2, key + 2, shut);
		Unknown0BC->Insert_Morph_Key(2, key + 2 + hold, shut);
		Unknown0BC->Insert_Morph_Key(2, key + 4 + hold, 0);
	}
	Unknown0BC->Insert_Morph_Key(2, frames - 1, 0);
}

// 0x782FF0
// the frame count from the second animation if set, else the first
void CharacterViewAvatarClass::Unknown_782FF0(const char *text)
{
	int delay = PlayerNLP->Personality->Get_Speech_Delay();
	int base;
	if (Desc.Mood_Type == 3) {
		base = 0;
	} else {
		base = Desc.Mood_Type * 17;
	}
	Unknown24C = 0;
	int phonemes[256];
	int count = Unknown_BE856C->Phonemes.Unknown_791400(text, phonemes, 0xFF);
	Unknown0BC->Insert_Morph_Key(0, Unknown24C, base);
	Unknown24C += delay;
	for (int i = 0; i < count; i++) {
		Unknown0BC->Insert_Morph_Key(0, Unknown24C, phonemes[i] + 1 + base);
		Unknown24C += delay;
	}
	Unknown24C += delay;
	Unknown0BC->Insert_Morph_Key(0, Unknown24C, base);
	int frames;
	if (Unknown0C0[1] != NULL) {
		frames = Unknown0C0[1]->Get_Num_Frames();
	} else {
		frames = Unknown0C0[0]->Get_Num_Frames();
	}
	Unknown248 = (Unknown24C - frames < 0) ? 0 : Unknown24C - frames;

	if (strcmp(text, "") == 0) {
		Unknown24C = -1;
	}
}


















































































































































































































































































// 0x7831C0
// NOT EXACT: functional for POC (D64), inline-budget (RefCountPtr 0x57B6B0 / 0x78A440 as a plain pointer)
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

// 0x783470
// NOT EXACT: functional for POC (D64), inline-budget (RefCountPtr 0x474080 as a plain pointer)
void CharacterViewAvatarClass::Unknown_783470()
{
	char name[32];
	if (Desc.Avatar_Type == 0) {
		_snprintf(name, 0x20, "%s%sHEAD%02d", Unknown_B9CD30[Desc.Gender], Unknown_B9CD38[Desc.Race], (int)Desc.Head_Type);
	} else {
		_snprintf(name, 0x20, "ANPCHEAD%03d", (int)Desc.Head_Type);
	}
	RenderObjClass *head = BodyObj->Get_Sub_Object_By_Name(name);
	if (head) {
		if (Unknown0BC->Get_Num_Frames() < 1) {
			head->Set_Animation();
		} else {
			head->Set_Animation(Unknown0BC, 0.0f, RenderObjClass::ANIM_MODE_ONCE);
		}
		head->Release_Ref();
	}
}

























































// 0x783620
// NOT EXACT: functional for POC (D64), literal (__LINE__ of the FAIL_IF; the +0x24C queue test not built)
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
		BodyObj->Set_Animation(Unknown0C0[0], Unknown0CC[0], 0);
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
	BodyObj->Set_Animation(Unknown0C0[0], Unknown0CC[0], Unknown0C0[1], Unknown0CC[1], blend * 0.5f);
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
// NOT EXACT: functional for POC (D64), inline-budget (the NLP, blink and face callees not built; the movement's branches merged)
void CharacterViewAvatarClass::Unknown_1C4()
{
	if (Unknown204 == 0) {
		return;
	}
	if (Unknown0E4 == (int)WW3D::Get_Frame_Count()) {
		return;
	}
	Unknown0E4 = WW3D::Get_Frame_Count();
	if (BodyObj == NULL || HeadObj == NULL || PlayerNLP == NULL) {
		Unknown_4BC260("Avatar Animation", "", __FILE__, __LINE__, "Invalid avatar objects (%x %x %x)", BodyObj, HeadObj, PlayerNLP);
		return;
	}
	Unknown_788900();
	Unknown_7897D0();
	float frame = (float)WW3D::Get_Frame_Time() * 0.001f;
	float time = frame;
	if (time > 0.25f) {
		time = 0.25f;
	}
	if (Unknown2F8 != 0 || Unknown308 != 0.0f) {
		float accel = time * 30.0f;
		Unknown360.Set(0.0f, 0.0f, 0.0f);
		if (Unknown308 > 0.0f && Unknown2F4) {
			accel *= 2.5f;
		}
		float turn = 2.0f * time;
		if (frame > 0.25f) {
			turn = 0.0f;
		}
		if (Unknown318.size() != 0 && Unknown20C != 2) {
			turn *= 3.0f;
		}
		if (Unknown2FC < 0.25f) {
			turn = Unknown2FC * 4.0f * turn;
		}
		if (Unknown2F8 & 4) {
			Unknown330.Rotate_Z(turn);
		} else if (Unknown2F8 & 8) {
			Unknown330.Rotate_Z(-turn);
		}
		float pitch = Unknown300 * 1.0f * time;
		if (frame > 0.25f) {
			pitch = 0.0f;
		}
		int flags = Unknown2F8;
		if (flags & 0x10) {
			pitch = Unknown30C - pitch;
			if (pitch <= -0.8f) {
				pitch = -0.8f;
			}
			Unknown30C = pitch;
		} else if (flags & 0x20) {
			pitch = pitch + Unknown30C;
			if (pitch >= 0.4f) {
				pitch = 0.4f;
			}
			Unknown30C = pitch;
		}
		float yaw = Unknown304 * 2.0f * time;
		if (flags & 0x40) {
			Unknown310 = Unknown310 - yaw;
		} else if (flags & 0x80) {
			Unknown310 = yaw + Unknown310;
		}
		bool move = true;
		if (flags & 3) {
			if (Unknown20C == 2 && Unknown210 == 0) {
				Unknown20C = 0;
				Unknown214 = true;
				Unknown208 = 2;
			}
			if (Unknown20C != 1) {
				move = false;
			} else if (flags & 1) {
				float speed = accel + Unknown308;
				float top = Unknown2F4 ? 25.0f : 10.0f;
				if (speed > top) {
					speed = top;
				}
				Unknown308 = speed;
			} else {
				float speed = Unknown308 - accel;
				if (speed < -10.0f) {
					speed = -10.0f;
				}
				Unknown308 = speed;
			}
		} else if (Unknown308 == 0.0f) {
			move = false;
		} else if (Unknown308 > 0.0f) {
			float speed = Unknown308 - accel;
			if (speed < 0.0f) {
				speed = 0.0f;
			}
			Unknown308 = speed;
		} else if (Unknown308 < 0.0f) {
			float speed = accel + Unknown308;
			if (speed > 0.0f) {
				speed = 0.0f;
			}
			Unknown308 = speed;
		}
		if (move) {
			Unknown360.Y = Unknown330[1][0] * Unknown308 * time;
			Unknown360.X = Unknown330[0][0] * Unknown308 * time;
			Unknown360.Z = Unknown330[2][0] * Unknown308 * time;
			Unknown330[0][3] = Unknown360.X + Unknown330[0][3];
			Unknown330[1][3] = Unknown360.Y + Unknown330[1][3];
			Unknown330[2][3] = Unknown360.Z + Unknown330[2][3];
		}
	}
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
	if (Unknown233) {
		Unknown228 += time;
		int steps = (int)(Unknown228 * 20.0f);
		if (steps > 0) {
			Unknown228 -= steps * 0.05f;
			if (Unknown29C) {
				Unknown_781410(0, steps);
			}
			if (Unknown29D) {
				Unknown_781570(0, steps);
			}
		}
	}
	if (Unknown2EC && (BodyObj == NULL || !BodyObj->Is_Hidden())) {
		Unknown2F0 -= time;
		if (Unknown2F0 < 0.0f) {
			if (TutorialManagerClass::Instance) {
				TutorialManagerClass::Instance->Start_Step(Unknown2EC);
			}
			Unknown2F0 = 60.0f;
		}
	}
	Unknown_783620(time);
	Unknown_7854B0(time);
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
					if (Unknown_7860C0()) {
						Unknown_782200();
					} else {
						Unknown_781EA0();
					}
					break;
				case 1:
					Unknown_782200();
					break;
				case 2:
					Unknown_7826A0();
					break;
				case 3:
					Unknown_782850();
					break;
			}
		}
	}
	// The client then shows +0x2E4 (the mask) or the head by 0x8190D0's 0x819280: not built yet.
}

// 0x784700
// NOT EXACT: functional for POC (D64)
void CharacterViewAvatarClass::Unknown_784700()
{
	const Unknown_ChatBlockStruct *block = (const Unknown_ChatBlockStruct *)Unknown0E8.Unknown_00();
	if (block == NULL) {
		return;
	}
	unsigned int i;
	for (i = 0; i < Unknown2A0.size(); i++) {
		if (Unknown2A0[i]) {
			delete Unknown2A0[i];
		}
	}
	Unknown2A0.clear();
	int mood = -1;
	for (;;) {
		switch ((unsigned char)block->Type) {
			case 0:
				if (Unknown210 != 1 && Unknown210 != 2 && Unknown20C == 2) {
					mood = (char)block->Value;
				}
				break;
			case 1:
				if (block->Value && Unknown210 != 1 && Unknown210 != 2 && Unknown20C == 2) {
					Unknown_781880(block->Value, (const char *)(block + 1), mood);
					Unknown208 = 1;
					mood = -1;
				}
				break;
			case 2:
				if (block->Value && Unknown210 != 1 && Unknown210 != 2 && Unknown20C == 2) {
					Unknown_781880(block->Value, "", mood);
					Unknown208 = 1;
					mood = -1;
				}
				break;
			case 3:
				Desc.Mood_Type = (char)block->Value;
				break;
			case 4:
				if (Unknown210 == 0) {
					Unknown210 = 1;
					Unknown22C = 0.0f;
					Unknown244 = 0;
					Unknown208 = 3;
					Unknown214 = true;
					Unknown20C = 2;
				}
				break;
			case 5:
				if (Unknown210 == 3) {
					Unknown210 = 2;
					Unknown208 = 3;
					Unknown214 = true;
				}
				break;
			case 6:
				if (Unknown20C == 2 && Unknown210 == 0) {
					Unknown20C = 0;
					Unknown214 = true;
					Unknown208 = 2;
				}
				break;
			case 7:
				if (Unknown20C != 2) {
					Unknown208 = 0;
					Unknown20C = 2;
					Unknown214 = true;
				}
				break;
			case 8:
				if (Unknown210 == 1 || Unknown210 == 2 || Unknown20C != 2) {
					return;
				}
				if (Unknown204 == 1 && mood < 0) {
					return;
				}
				Unknown_781880((char)PlayerNLP->Unknown_78F640("breathe1"), "", mood);
				Unknown314 = 1;
				Unknown208 = 1;
				return;
		}
		block = (const Unknown_ChatBlockStruct *)((const char *)block + block->Length + sizeof(Unknown_ChatBlockStruct));
	}
}

// 0x784A50
// (the asserts' lines written as the client's numbers)
void CharacterViewAvatarClass::Unknown_784A50(const char *name, bool a, bool b)
{
	if (Unknown2F8 == 0 && Unknown308 == 0.0f) {
		if (PlayerNLP == NULL) {
			Unknown_Assert_Failed("PlayerNLP", __FILE__, 2675);
			Unknown_4BC260("Avatar Animation", "", __FILE__, 2676, "Invalid NLP pointer");
			return;
		}
		Unknown0E8.Unknown_4C44A0();
		PlayerNLP->Unknown_78F690(name, strlen(name), &Desc.Mood_Type, &Unknown0E8);
		Unknown0E8.Unknown_4C4410();
		Unknown_784700();
		Unknown214 = a;
		if (a) {
			Unknown24C = -1;
		}
		Unknown220 = false;
		Unknown215 = b;
	}
}

// 0x784B80
// (the asserts' lines written as the client's numbers)
void CharacterViewAvatarClass::Unknown_784B80(char *data, int size, bool flag)
{
	if (Unknown2F8 == 0 && Unknown308 == 0.0f) {
		if (PlayerNLP == NULL) {
			Unknown_Assert_Failed("PlayerNLP", __FILE__, 2704);
			Unknown_4BC260("Avatar Animation", "", __FILE__, 2705, "Invalid NLP pointer");
			return;
		}
		Unknown0E8.Unknown_4C44A0();
		Unknown0E8.Unknown_4C44E0(data, size);
		Unknown_784700();
		Unknown214 = flag;
		if (flag) {
			Unknown24C = -1;
		}
		Unknown220 = false;
		Unknown208 = 1;
	}
}




























// 0x784C90
// (the asserts on the client's lines 2728 / 2729)
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
		SpeechSound = NULL;
	}

	TutorialManagerClass::Instance->Stop_Step();

	Unknown208 = 0;
	Unknown214 = true;
	if (Unknown2EC) {
		delete[] Unknown2EC;
		Unknown2EC = NULL;
	}
}

bool CharacterViewAvatarClass::Unknown_784EA0(const char *name) { return(TutorialManagerClass::Instance->Was_Just_Finished(name)); }	// 0x784EA0 (on one line: avatar.cpp's later asserts keep their lines)

// 0x7854B0
void CharacterViewAvatarClass::Unknown_7854B0(float time)
{
	if (!Unknown230) {
		return;
	}
	Unknown260 += time;
	int steps = (int)(Unknown260 * 20.0f);
	if (steps == 0) {
		return;
	}
	Unknown260 -= steps * 0.05f;
	if (Unknown26C >= Unknown270) {
		Unknown270 = Unknown26C + Unknown_BDBB84(5, 30);
		Unknown264 = (float)Unknown_BDBB84(-10, 10);
		Unknown268 = (float)Unknown_BDBB84(-5, 5);
	}
	Unknown26C++;
	for (int i = steps; i > 0; i--) {
		Unknown254 = Unknown254 * 0.95f + (Unknown264 + Unknown250) * 0.05f;
		Unknown25C = Unknown25C * 0.95f + (Unknown268 + Unknown258) * 0.05f;
	}
	Matrix3D m(true);
	m.Rotate_Z(DEG_TO_RAD(Unknown254));
	m.Rotate_Y(DEG_TO_RAD(Unknown25C));
	int bone = BodyObj->Get_Bone_Index("K_Head");
	BodyObj->Capture_Bone(bone);
	BodyObj->Control_Bone(bone, m, false);
}


































































































































































































// 0x7857D0: Head_Texture_Num's face texture '<gender><race>FACETEX<num>.tga' onto the head
// object's head0..4 meshes (stage 0)
// NOT EXACT: functional for POC (D64), literal (__LINE__), inline-budget (the client's RefCountPtr temporaries)
void CharacterViewAvatarClass::Unknown_7857D0(int num)
{
	FAIL_IF(!HeadObj) {
		return;
	}
	char name[32];
	char mesh[64];
	_snprintf(name, 0x20, "%s%sFACETEX%02d.tga", Unknown_B9CD30[Desc.Gender], Unknown_B9CD38[Desc.Race], num);
	TextureClass *tex = WW3DAssetManager::Get_Instance()->Get_Texture(name, MIP_LEVELS_ALL, WW3D_FORMAT_UNKNOWN, true);
	if (tex) {
		Desc.Head_Texture_Num = (char)num;
		for (int i = 0; i < 5; i++) {
			_snprintf(mesh, 0x40, "head%d", i);
			RenderObjClass *robj = HeadObj->Get_Sub_Object_By_Name(mesh);
			Unknown_495FD0(robj, tex, 0);
			Unknown_496AE0(robj, true);
			if (robj) { robj->Release_Ref(); }
		}
	}
	Unknown_496AE0(HeadObj, true);
	if (tex) { tex->Release_Ref(); }
}


// 0x7859F0: Tattoo_Texture_Num's 'ATATTOO<num>.tga' (clamped) onto the head object's HEAD0..4
// meshes (stage 1)
// NOT EXACT: functional for POC (D64), literal (__LINE__), inline-budget (the client's RefCountPtr temporaries)
void CharacterViewAvatarClass::Unknown_7859F0(int num)
{
	FAIL_IF(!HeadObj) {
		return;
	}
	char name[32];
	char mesh[64];
	_snprintf(name, 0x20, "ATATTOO%02d.tga", num);
	TextureClass *tex = WW3DAssetManager::Get_Instance()->Get_Texture(name, MIP_LEVELS_ALL, WW3D_FORMAT_UNKNOWN, true);
	if (tex) {
		tex->Set_U_Addr_Mode(TextureClass::TEXTURE_ADDRESS_CLAMP);
		tex->Set_V_Addr_Mode(TextureClass::TEXTURE_ADDRESS_CLAMP);
		for (int i = 0; i < 5; i++) {
			_snprintf(mesh, 0x40, "HEAD%d", i);
			RenderObjClass *robj = HeadObj->Get_Sub_Object_By_Name(mesh);
			Unknown_495FD0(robj, tex, 1);
			Unknown_496AE0(robj, true);
			if (robj) { robj->Release_Ref(); }
		}
		Desc.Tattoo_Texture_Num = (char)num;
	}
	Unknown_496AE0(HeadObj, true);
	if (tex) { tex->Release_Ref(); }
}





// 0x785C00: Tattoo_Offset onto the head object's HEAD0..4 meshes: a linear offset mapper on stage 1
// of vertex material 0 (X, Y the tattoo's place, Z its scale, clamped to 0.5 .. 16)
// NOT EXACT: functional for POC (D64), literal (__LINE__), inline-budget (the client's RefCountPtr temporaries)
void CharacterViewAvatarClass::Unknown_785C00(const Vector3 &offset)
{
	FAIL_IF(!HeadObj) {
		return;
	}
	float scale = offset.Z;
	if (scale < 0.5f) {
		scale = 0.5f;
	} else if (scale > 16.0f) {
		scale = 16.0f;
	}
	float size = 1.0f / scale;
	float half = size * 0.5f;
	Vector2 start(-((offset.X - half) / size), -((offset.Y - half) / size));
	Vector2 scales(scale, scale);
	char mesh[64];
	for (int i = 0; i < 5; i++) {
		_snprintf(mesh, 0x40, "HEAD%d", i);
		RenderObjClass *robj = HeadObj->Get_Sub_Object_By_Name(mesh);
		if (robj) {
			MaterialInfoClass *info = robj->Get_Material_Info();
			if (info) {
				VertexMaterialClass *material = info->Peek_Vertex_Material(0);
				LinearOffsetTextureMapperClass *mapper = new LinearOffsetTextureMapperClass(Vector2(0.0f, 0.0f), start, true, scales, 1);
				material->Set_Mapper(mapper, 1);
				mapper->Release_Ref();
				info->Release_Ref();
			}
			robj->Release_Ref();
		}
	}
	Unknown_496AE0(HeadObj, true);
	Desc.Tattoo_Offset = offset;
}

// 0x7860C0
// NOT EXACT: functional for POC (D64)
int CharacterViewAvatarClass::Unknown_7860C0()
{
	if (Desc.Avatar_Type != 0) {
		TutorialLineStruct *line = TutorialManagerClass::Instance->Next_Line();
		if (line) {
			const char *text = line->Text;
			if (PlayerNLP) {
				Unknown0E8.Unknown_4C44A0();
				PlayerNLP->Unknown_78F690(text, strlen(text), &Desc.Mood_Type, &Unknown0E8);
				Unknown0E8.Unknown_4C4410();
			}
			Unknown_7861B0(line->Speech);
			Unknown215 = line->SkipBlend;
			Unknown_784700();
			Unknown24C = -1;
			return(1);
		}
	}
	return(0);
}

// 0x7861B0
// (SpeechSound a RefCountPtr: Create_Get, then its operator=)
void CharacterViewAvatarClass::Unknown_7861B0(const char *speech)
{
	if (SpeechSound) {
		SpeechSound->Stop(1);
		SpeechSound = NULL;

	}
	SpeechSound = Create_Get(SoundLibraryClass::Play_2D_Sound(speech, 1));
}

// 0x786370
void CharacterViewAvatarClass::Unknown_786370(bool on)
{
	if (BodyObj) {
		BodyObj->Set_Hidden(on);
	}
	if (HeadObj) {
		HeadObj->Set_Hidden(on);
	}
}

// 0x7863E0
bool CharacterViewAvatarClass::Unknown_7863E0()
{
	if (BodyObj) {
		if (BodyObj->Is_Hidden()) {
			return(true);
		}
	}
	return(false);
}

// 0x786B60
void CharacterViewAvatarClass::Unknown_786B60(bool keep)
{
	if (!keep) {
		Unknown_Inline_786C70();
	}
	Unknown2F8 |= 1;
}

// 0x786BF0
void CharacterViewAvatarClass::Unknown_786BF0(bool keep)
{
	if (!keep) {
		Unknown_Inline_786C70();
	}
	Unknown2F8 |= 2;
}

// 0x786C70
void CharacterViewAvatarClass::Unknown_786C70()
{
	if (Unknown20C != 2) {
		Unknown208 = 0;
		Unknown20C = 2;
		Unknown214 = true;
	}
	Unknown2F8 &= 0xFFFC;
}

// 0x786CC0
void CharacterViewAvatarClass::Unknown_786CC0(float rate)
{
	Unknown2F8 = (Unknown2F8 & 0xFFF3) | 8;
	Unknown2FC = rate;
}

// 0x786CF0
void CharacterViewAvatarClass::Unknown_786CF0(float rate)
{
	Unknown2F8 = (Unknown2F8 & 0xFFF3) | 4;
	Unknown2FC = rate;
}

// 0x786D20
void CharacterViewAvatarClass::Unknown_786D20()
{
	Unknown2F8 &= 0xFFF3;
}

// 0x786D40
void CharacterViewAvatarClass::Unknown_786D40(float rate)
{
	Unknown2F8 = (Unknown2F8 & 0xFFCF) | 0x10;
	Unknown300 = rate;
}

// 0x786D70
void CharacterViewAvatarClass::Unknown_786D70(float rate)
{
	Unknown2F8 = (Unknown2F8 & 0xFFCF) | 0x20;
	Unknown300 = rate;
}

// 0x786DA0
void CharacterViewAvatarClass::Unknown_786DA0()
{
	Unknown2F8 &= 0xFFCF;
}

// 0x786DC0
void CharacterViewAvatarClass::Unknown_786DC0(float rate)
{
	if (Unknown2F5) {
		Unknown2F8 = (Unknown2F8 & 0xFF3F) | 0x40;
		Unknown304 = rate;
	}
}

// 0x786E00
void CharacterViewAvatarClass::Unknown_786E00(float rate)
{
	if (Unknown2F5) {
		Unknown2F8 = (Unknown2F8 & 0xFF3F) | 0x80;
		Unknown304 = rate;
	}
}

// 0x786E40
void CharacterViewAvatarClass::Unknown_786E40()
{
	Unknown2F8 &= 0xFF3F;
}

// 0x786E60
void CharacterViewAvatarClass::Unknown_786E60()
{
	Unknown2F8 = 0;
	Unknown308 = 0.0f;
	if (Unknown20C != 2) {
		Unknown208 = 0;
		Unknown20C = 2;
		Unknown214 = true;
	}
}

// 0x7885D0
// NOT EXACT: functional for POC (D64), inline-budget (the client inlines +0x318's erase and calls +0x324's)
void CharacterViewAvatarClass::Unknown_7885D0(bool second)
{
	std::list<Matrix3D *>::iterator it;
	if (second) {
		for (it = Unknown324.begin(); it != Unknown324.end(); it++) {
			delete *it;
		}
		Unknown324.erase(Unknown324.begin(), Unknown324.end());
	} else {
		for (it = Unknown318.begin(); it != Unknown318.end(); it++) {
			delete *it;
		}
		Unknown318.erase(Unknown318.begin(), Unknown318.end());
	}
}

// 0x788700
void CharacterViewAvatarClass::Unknown_788700(const Matrix3D &goal)
{
	Unknown_7885D0(false);
	Unknown_788730(goal, true);
}

// 0x788730
void CharacterViewAvatarClass::Unknown_788730(const Matrix3D &goal, bool trim)
{
	Unknown318.push_back(new Matrix3D(goal));
	if (trim) {
		for (int i = Unknown318.size() - 4; i > 0; i--) {
			Unknown_788890();
		}
	}
}

// 0x788890
void CharacterViewAvatarClass::Unknown_788890()
{
	if (Unknown318.size() != 0) {
		delete Unknown318.front();
		Unknown318.pop_front();
	}
}

// 0x788900
// NOT EXACT: functional for POC (D64), inline-budget (the client calls 0x784F20 where this repeats its three stores)
void CharacterViewAvatarClass::Unknown_788900()
{
	if (Unknown318.size() == 0) {
		return;
	}
	Matrix3D *goal;
	Vector3 delta;
	float dist;
	for (;;) {
		goal = Unknown318.front();
		delta.X = (*goal)[0][3] - Unknown330[0][3];
		delta.Y = (*goal)[1][3] - Unknown330[1][3];
		delta.Z = (*goal)[2][3] - Unknown330[2][3];
		dist = sqrt(delta.X * delta.X + delta.Y * delta.Y + delta.Z * delta.Z);
		if (dist >= 3.0f) {
			break;
		}
		if (Unknown318.size() < 2) {
			if (Unknown20C != 2) {
				Unknown208 = 0;
				Unknown20C = 2;
				Unknown214 = true;
			}
			int flags = Unknown2F8;
			Unknown2F8 = flags & 0xFFFC;
			float facing = Unknown330[0][0] * (*goal)[0][1] + Unknown330[1][0] * (*goal)[1][1] + Unknown330[2][0] * (*goal)[2][1];
			float rate = (float)fabs(facing);
			if (rate < 0.0436193f) {
				Unknown2F8 = flags & 0xFFF0;
				if (Unknown318.size() != 0) {
					delete *Unknown318.begin();
					Unknown318.erase(Unknown318.begin());
				}
				return;
			}
			Unknown2FC = rate;
			if (facing < 0.0f) {
				Unknown2F8 = (flags & 0xFFF0) | 4;
			} else {
				Unknown2F8 = (flags & 0xFFF0) | 8;
			}
			return;
		}
		if (Unknown318.size() != 0) {
			delete *Unknown318.begin();
			Unknown318.erase(Unknown318.begin());
		}
		if (Unknown318.size() == 0) {
			return;
		}
	}
	float len2 = delta.X * delta.X + delta.Y * delta.Y + delta.Z * delta.Z;
	if (len2 != 0.0f) {
		float inv = WWMath::Inv_Sqrt(len2);
		delta.X *= inv;
		delta.Y *= inv;
		delta.Z *= inv;
	}
	float side = Unknown330[0][1] * delta.X + delta.Y * Unknown330[1][1] + delta.Z * Unknown330[2][1];
	float rate = (float)fabs(side);
	if (Unknown330[0][0] * delta.X + Unknown330[1][0] * delta.Y + Unknown330[2][0] * delta.Z <= 0.0f
		&& Unknown330[0][0] * (*goal)[0][0] + Unknown330[1][0] * (*goal)[1][0] + Unknown330[2][0] * (*goal)[2][0] >= 0.0f
		&& dist <= 10.0f) {
		// The goal is behind and faces our way: back up to it.
		bool full = false;
		if (rate < 0.5f) {
			int flags = Unknown2F8;
			if (flags & 1) {
				if (dist >= 10.0f) {
					full = true;
				} else {
					if (Unknown20C != 2) {
						Unknown208 = 0;
						Unknown20C = 2;
						Unknown214 = true;
					}
					flags &= 0xFFFC;
				}
			}
			if (!full) {
				Unknown2F8 = flags | 2;
			}
		}
		Unknown2FC = full ? 1.0f : rate;
		if (side < 0.0f) {
			Unknown2F8 = (Unknown2F8 & 0xFFF3) | 4;
		} else {
			Unknown2F8 = (Unknown2F8 & 0xFFF3) | 8;
		}
		return;
	}
	if (rate < 0.5f) {
		int flags = Unknown2F8;
		if (flags & 2) {
			if (Unknown20C != 2) {
				Unknown208 = 0;
				Unknown20C = 2;
				Unknown214 = true;
			}
			flags &= 0xFFFC;
		}
		Unknown2F8 = flags | 1;
	}
	Unknown2FC = rate;
	if (side < 0.0f) {
		Unknown2F8 = (Unknown2F8 & 0xFFF3) | 8;
	} else {
		Unknown2F8 = (Unknown2F8 & 0xFFF3) | 4;
	}
}

// 0x788EF0
// the teleport effect dropped at once (charview 0x67B390)
// NOT EXACT: functional for POC (D64), inline-budget (the client's +0x370 / +0x374 are RefCountPtrs)
void CharacterViewAvatarClass::Unknown_788EF0()
{
	if (Unknown370) {
		Unknown370->Set_Animation();
		Unknown370->Remove();
		REF_PTR_RELEASE(Unknown370);
	}
	if (Unknown374) {
		REF_PTR_RELEASE(Unknown374);
	}
	Unknown380 = 0;
	Unknown37C = 0.0f;
	Unknown378 = -100.0f;
}

// 0x788F90
// the teleport effect 'av_tele0N' and its animation into +0x370 / +0x374, put in the body's scene on the avatar,
// hidden until 0x7896B0 plays it; +0x37C the time it runs, +0x378 the time left at which 0x7897D0 shows / hides the body
// NOT EXACT: functional for POC (D64), inline-budget (the client's +0x370 / +0x374 are RefCountPtrs)
void CharacterViewAvatarClass::Unknown_788F90(int mode)
{
	if (BodyObj == NULL || Unknown380 == mode) {
		return;
	}
	if (Unknown370) {
		Unknown370->Set_Animation();
		Unknown370->Remove();
		REF_PTR_RELEASE(Unknown370);
	}
	if (Unknown374) {
		REF_PTR_RELEASE(Unknown374);
	}
	Unknown378 = -100.0f;
	Unknown37C = 0.0f;
	Unknown380 = mode;
	if (mode == 0) {
		return;
	}
	switch (mode) {
		case 1:
			Unknown370 = WW3DAssetManager::Get_Instance()->Create_Render_Obj("av_tele01");
			Unknown374 = WW3DAssetManager::Get_Instance()->Get_HAnim("av_tele01a.Av_tele01a-1");
			Unknown378 = 0.7f;
			Unknown37C = 3.2f;
			break;
		case 2:
			Unknown370 = WW3DAssetManager::Get_Instance()->Create_Render_Obj("av_tele03");
			Unknown374 = WW3DAssetManager::Get_Instance()->Get_HAnim("av_tele03a.Av_tele03a-1");
			if (BodyObj) {
				BodyObj->Set_Hidden(1);
			}
			if (HeadObj) {
				HeadObj->Set_Hidden(1);
			}
			Unknown378 = 0.5f;
			Unknown37C = 1.0f;
			break;
		case 3:
			Unknown370 = WW3DAssetManager::Get_Instance()->Create_Render_Obj("av_tele02");
			Unknown374 = WW3DAssetManager::Get_Instance()->Get_HAnim("av_tele02a.Av_tele02a-1");
			Unknown378 = 0.5f;
			Unknown37C = 1.0f;
			break;
		case 4:
			Unknown370 = WW3DAssetManager::Get_Instance()->Create_Render_Obj("av_tele04");
			Unknown374 = WW3DAssetManager::Get_Instance()->Get_HAnim("av_tele04a.Av_tele04a-1");
			Unknown_786370(true);
			Unknown378 = 0.0f;
			Unknown37C = 0.5f;
			break;
	}
	if (Unknown370 == NULL) {	// POC guard: the client goes on without testing
		return;
	}
	Unknown_496AE0(Unknown370, true);
	BodyObj->Peek_Scene()->Add_Render_Object(Unknown370);
	if (!Unknown36C) {
		Unknown370->Set_Transform(Get_Transform());
	} else {
		Matrix3D m = Unknown330;
		Unknown370->Set_Transform(m);
	}
	Unknown370->Set_Hidden(1);
	Unknown370->Set_Animation_Hidden(1);
}

// 0x7896B0
// the teleport effect 'mode' (0x788F90) shown and its animation played once (charview 0x67B390: 4, ui_avsel 0x55D9C0: 1)
// NOT EXACT: functional for POC (D64), inline-budget (the client's +0x370 / +0x374 are RefCountPtrs)
void CharacterViewAvatarClass::Unknown_7896B0(int a)
{
	Unknown_788F90(a);
	if (Unknown370) {
		if (Unknown374) {
			Reset_All_Texture_Mappers(Unknown370, false);
			Unknown370->Set_Animation(Unknown374, 0.0f, RenderObjClass::ANIM_MODE_ONCE);
			Unknown370->Set_Hidden(0);
			Unknown370->Set_Animation_Hidden(0);
			return;
		}
		if (Unknown370) {
			Unknown370->Set_Animation();
			Unknown370->Remove();
			REF_PTR_RELEASE(Unknown370);
		}
	}
	if (Unknown374) {
		REF_PTR_RELEASE(Unknown374);
	}
	Unknown380 = 0;
	Unknown37C = 0.0f;
	Unknown378 = -100.0f;
}

// 0x7897D0
// NOT EXACT: functional for POC (D64), inline-budget (RefCountPtr +0x370 / +0x374 as plain pointers)
void CharacterViewAvatarClass::Unknown_7897D0()
{
	if (Unknown380 == 0) {
		return;
	}
	if (Unknown370 != NULL && Unknown374 != NULL) {
		Unknown37C -= (float)WW3D::Get_Frame_Time() * 0.001f;
		if (Unknown37C <= Unknown378) {
			Unknown378 = -100.0f;
			bool hidden = !(Unknown380 == 2 || Unknown380 == 4);
			if (BodyObj) {
				BodyObj->Set_Hidden(hidden);
			}
			if (HeadObj) {
				HeadObj->Set_Hidden(hidden);
			}
		}
		if (Unknown37C > 0.0f) {
			return;
		}
	}
	if (Unknown370) {
		Unknown370->Set_Animation();
		Unknown370->Remove();
		Unknown370->Release_Ref();
		Unknown370 = NULL;
	}
	if (Unknown374) {
		Unknown374->Release_Ref();
		Unknown374 = NULL;
	}
	Unknown380 = 0;
	Unknown378 = -100.0f;
	Unknown37C = 0.0f;
}

// 0x789D80: the accessory's meshes '<part>0'..'<part>4' copied into the head prototype as LOD models,
// each into the LOD that holds it in the accessory
// NOT EXACT: functional for POC (D64), inline-budget (the client's RefCountPtr calls)
void CharacterViewAvatarClass::Unknown_789D80(RenderObjClass **slot, const char *part)
{
	if (HeadSource == NULL || *slot == NULL || part == NULL) {
		return;
	}
	char name[64];
	for (int i = 0; i < 5; i++) {
		_snprintf(name, 0x40, "%s%d", part, i);
		RenderObjClass *sub = (*slot)->Get_Sub_Object_By_Name(name);
		if (sub && sub->Class_ID() == RenderObjClass::CLASSID_MESH) {
			HLodClass *head = (HLodClass *)HeadSource;
			HLodClass *source = (HLodClass *)*slot;
			RenderObjClass *copy = sub->Clone();
			Unknown_496AE0(copy, true);
			copy->Set_Container(NULL);
			for (int lod = 0; lod < source->Get_Lod_Count(); lod++) {
				for (int j = 0; j < source->Get_Lod_Model_Count(lod); j++) {
					if (sub == source->Peek_Lod_Model(lod, j)) {
						head->Add_Lod_Model(lod, copy, 0);
						Unknown_496AE0(HeadSource, true);
						lod = 100;
						break;
					}
				}
			}
			if (copy) {
				copy->Release_Ref();
			}
		}
		if (sub) {
			sub->Release_Ref();
		}
	}
}

// Layout (CS12, 2026-11-05): the functions stand in the client's address order, and blank lines put each assert and
// __LINE__ user on the client's line (D25): 0x77E830 725, 0x77E900 748, 0x7812A0 1415, 0x784C90 2728 ...
