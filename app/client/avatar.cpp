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
#include "..\..\ww3d2\coltest.h"
#include "..\..\wwmath\castres.h"
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
// (every member in the init list, in declaration order: the client stores them all before the vtable)
CharacterViewAvatarClass::CharacterViewAvatarClass(AvatarDescClass *desc, bool a, bool b, EnvironmentMapManagerClass *env_maps)
	: CompositeRenderObjClass(),
	  Unknown0E4(0),
	  PlayerNLP(NULL),
	  Desc(*desc),
	  Unknown204(0),
	  Unknown208(0),
	  Unknown20C(2),
	  Unknown210(0),
	  Unknown214(false),
	  Unknown215(false),
	  Unknown218(0.0f),
	  Unknown21C(0.0f),
	  Unknown220(false),
	  Unknown221(false),
	  Unknown224(0),
	  Unknown228(0.0f),
	  Unknown22C(0.0f),
	  Unknown230(true),
	  Unknown231(false),
	  Unknown232(false),
	  Unknown233(true),
	  Unknown234(0.0f),
	  Unknown238(0),
	  Unknown23C(0),
	  Unknown240(0),
	  Unknown244(0),
	  Unknown248(0),
	  Unknown24C(-1),
	  Unknown250(0.0f),
	  Unknown254(0.0f),
	  Unknown258(0.0f),
	  Unknown25C(0.0f),
	  Unknown260(0.0f),
	  Unknown264(0.0f),
	  Unknown268(0.0f),
	  Unknown26C(0),
	  Unknown270(0),
	  Unknown29C(false),
	  Unknown29D(false),
	  Unknown2EC(NULL),
	  Unknown2F0(0.0f),
	  Unknown2F4(true),
	  Unknown2F5(true),
	  Unknown2F8(0),
	  Unknown2FC(1.0f),
	  Unknown300(1.0f),
	  Unknown304(1.0f),
	  Unknown308(0.0f),
	  Unknown30C(0.0f),
	  Unknown310(0.0f),
	  Unknown314(0),
	  Unknown330(true),
	  Unknown360(0.0f, 0.0f, 0.0f),
	  Unknown36C(false),
	  Unknown378(-100.0f),
	  Unknown37C(0.0f),
	  Unknown380(0)
{
	for (int i = 0; i < 2; i++) {
		Unknown0C0[i] = NULL;
		Unknown0C8[i] = false;
		Unknown0CC[i] = 0.0f;
		Unknown0D4[i] = 0.0f;
		Unknown0DC[i] = -1;
	}
	for (int w = 0; w < 5; w++) {
		Unknown274[w] = Desc.Body_Weight[w];
	}
	for (w = 0; w < 5; w++) {
		Unknown288[w] = Desc.Head_Weight[w];
	}
	Desc.Upgrade();
	if (a) {
		Unknown_77D7F0();
		Unknown_780810(env_maps);
		Unknown221 = true;
	} else {
		Unknown221 = false;
	}
	if (!b || Desc.Avatar_Type != 0) {
		Unknown_77D4A0();
	}
}

// 0x77C080
CharacterViewAvatarClass::~CharacterViewAvatarClass()
{
	Unknown_781C50();
	if (PlayerNLP) {
		delete PlayerNLP;
		PlayerNLP = NULL;
	}
	Unknown_7885D0(false);
	Unknown_7885D0(true);
	Unknown_788EF0();
	Unknown_77D4A0();
	if (Unknown0BC) {
		Unknown0BC->Free_Morph();
		Unknown0BC = NULL;
	}
	for (int a = 0; a < 2; a++) {
		Unknown_789CD0(a);
	}
	if (WeaponUnique) {
		BodyObj->Remove_Sub_Object(WeaponUnique.Peek());
		WeaponUnique = NULL;
	}
	if (WeaponHip) {
		BodyObj->Remove_Sub_Object(WeaponHip.Peek());
		WeaponHip = NULL;
	}
	if (WeaponBack) {
		BodyObj->Remove_Sub_Object(WeaponBack.Peek());
		WeaponBack = NULL;
	}
	if (CollisionObj) {
		BodyObj->Remove_Sub_Object(CollisionObj.Peek());
		CollisionObj = NULL;
	}
	Unknown_785F20(false);
	if (HeadObj) {
		BodyObj->Remove_Sub_Object(HeadObj.Peek());
		HeadObj = NULL;
	}
	if (MaskObj) {
		BodyObj->Remove_Sub_Object(MaskObj.Peek());
		MaskObj = NULL;
	}
	if (BodyObj) {
		BodyObj->Release_Bone(BodyObj->Get_Bone_Index("K_Head"));
		BodyObj->Remove();
		BodyObj = NULL;
	}
	if (Unknown2EC) {
		delete Unknown2EC;
		Unknown2EC = NULL;
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

// 0x77D2F0 (the body's and the head's LOD, each clamped to its count)
void CharacterViewAvatarClass::Set_LOD_Level(int lod)
{
	if (lod >= 0) {
		if (BodyObj) {
			BodyObj->Set_LOD_Level(min(BodyObj->Get_LOD_Count() - 1, lod));
		}
		if (HeadObj) {
			HeadObj->Set_LOD_Level(min(HeadObj->Get_LOD_Count() - 1, lod));
		}
	}
}

// 0x77D380 (the body's)
int CharacterViewAvatarClass::Get_LOD_Level() const
{
	if (BodyObj) {
		return(BodyObj->Get_LOD_Level());
	}
	return(0);
}

// 0x77D3B0 (the body's)
int CharacterViewAvatarClass::Get_LOD_Count() const
{
	if (BodyObj) {
		return(BodyObj->Get_LOD_Count());
	}
	return(1);
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
	return(BodyObj.Peek());
}

// 0x77D4A0: the accessories' meshes taken out of the head prototype and every creation prototype released
// (0x789FE0 inlined for each; the constructor, unless a player avatar is kept for the creation screens)
void CharacterViewAvatarClass::Unknown_77D4A0()
{
	if (HairSource) {
		Unknown_789FE0("HAIR");
		HairSource = NULL;
	}
	if (Unknown2CC) {
		Unknown_789FE0("EAR");
		Unknown2CC = NULL;
	}
	if (GoggleSource) {
		Unknown_789FE0("GOGGLE");
		GoggleSource = NULL;
	}
	if (BeardSource) {
		Unknown_789FE0("BEARD");
		BeardSource = NULL;
	}
	if (HeadSource) {
		HeadSource = NULL;
	}
	if (BodySource) {
		BodySource = NULL;
	}
	Unknown233 = false;
}

// 0x77D7F0: the NLP made again from Desc, the models loaded again (0x77DA20) and, for a player avatar, the morph
// weights normalised and shown at once (unreferenced; the constructor inlines it)
// NOT EXACT: register (this in edi, the client's esi: 0.62)
void CharacterViewAvatarClass::Unknown_77D7F0()
{
	Unknown_781680();
	Unknown224 = Unknown_BDBB84(0, PlayerNLP->Personality->Get_Fidget_Delay() / 2);
	Unknown_77DA20();
	if (Desc.Avatar_Type == 0) {
		Unknown_77E830(Desc.Body_Weight);
		Unknown_77E900(Desc.Head_Weight);
		Unknown_781410(1, 0);
		Unknown_781570(1, 0);
	}
	Unknown_781EA0();
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







































































































































































































// 0x77DA20: the models from Desc: body, collision, head, the mask, the face animation and (player avatars) the
// weapons and accessories, then the head object cloned again
// (the mask's index a local: the client's idiv before the asset manager's load)
void CharacterViewAvatarClass::Unknown_77DA20()
{
	Unknown_77E9D0(Desc.Body_Type, Desc.Pants_Type, false);
	CollisionObj = Create_Get(WW3DAssetManager::Get_Instance()->Create_Render_Obj("avcollision"));
	if (CollisionObj) {
		Unknown_496AE0(CollisionObj.Peek(), true);
		BodyObj->Add_Sub_Object_To_Bone(CollisionObj.Peek(), "skel_reference");
	}
	Unknown_77F000(Desc.Head_Type);
	if (!Unknown233 && !Desc.Unknown0C) {
		const char *masks[3] = { "hmask_01", "hmask_02", "hmask_03" };
		int mask = Desc.Head_Type % 3;
		MaskObj = Create_Get(WW3DAssetManager::Get_Instance()->Create_Render_Obj(masks[mask]));
		if (MaskObj) {
			Unknown_496AE0(MaskObj.Peek(), true);
			BodyObj->Add_Sub_Object_To_Bone(MaskObj.Peek(), "K_HEAD");
			MaskObj->Set_Hidden(true);
		}
	}
	HAnimClass *anims[3];
	char anim_name[32];
	for (int i = 0; i < 3; i++) {
		_snprintf(anim_name, 0x20, "%sHSKEL.%sHANIM%02d", Unknown_B9CD30[Desc.Gender], Unknown_B9CD30[Desc.Gender], i);
		anims[i] = WW3DAssetManager::Get_Instance()->Get_HAnim(anim_name);
	}
	if (anims[0] == NULL || anims[1] == NULL || anims[2] == NULL) {
		Unknown_4BC260("Avatar Creation", "", __FILE__, __LINE__, "Missing required Anim files for Avatar head");
	} else {
		Unknown0BC = Create_NEW(new HMorphAnimClass());
		Unknown0BC->Create_New_Morph(3, anims);
	}
	if (Desc.Avatar_Type == 0) {
		Unknown_77FDA0(Desc.Weapon_Unique_Num);
		Unknown_77FE20(Desc.Weapon_Hip_Num);
		Unknown_77FE70(Desc.Weapon_Back_Num);
		Unknown_77FC20(Desc.Hair_Num, false);
		Unknown_77FC90(Desc.Ear_Num, false);
		Unknown_77FCE0(Desc.Goggle_Num, false);
		Unknown_77FD30(Desc.Beard_Num, false);
		Unknown_77F750();
	}
}














// 0x77E0E0: the weapon model made into *slot and put on the body's bone; kind 0 '<gender><race><part><num>',
// 1 'a<race><part><num>', 2 'a<part><num>'
// (the slot a RefCountPtr: operator =(0), then Create_Get 0x47D2E0 through its operator =)
void CharacterViewAvatarClass::Unknown_77E0E0(int kind, const char *part, int num, const char *bone, RefCountPtr<RenderObjClass> *slot)
{
	if (*slot) {
		BodyObj->Remove_Sub_Object(slot->Peek());
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
	*slot = Create_Get(WW3DAssetManager::Get_Instance()->Create_Render_Obj(name));
	if (!making) {
		WW3D::Enable_Overbright_Modify_On_Load(false);
	}
	if (*slot) {
		Unknown_496AE0(slot->Peek(), true);
		BodyObj->Add_Sub_Object_To_Bone(slot->Peek(), bone);
		return;
	}
	Unknown_4BC000("Avatar Creation", "", __FILE__, __LINE__, "Avatar asset file does not exists %s", name);
}

// 0x77E350: empty (unreferenced; ret 4)
void CharacterViewAvatarClass::Unknown_77E350(int a)
{
}

// 0x77E370
void CharacterViewAvatarClass::Unknown_77E370(const char *name)
{
	Desc.Unknown_4C1460(name, true);
}

// 0x77E3A0
void CharacterViewAvatarClass::Unknown_77E3A0(const char *name)
{
	Desc.Unknown_4C15F0(name, true);
}

// 0x77E3D0
void CharacterViewAvatarClass::Unknown_77E3D0(int a)
{
	Unknown204 = a;
}

// 0x77E3F0: unless made (+0x221), the NLP and models made (0x77D7F0) and the creation prototypes released (0x77D4A0);
// one more reference added (SClient's 0x74C7B0 and 0x749590 / 0x749A70)
CharacterViewAvatarClass *CharacterViewAvatarClass::Unknown_77E3F0()
{
	if (!Unknown221) {
		Unknown_77D7F0();
		Unknown_77D4A0();
		Unknown221 = true;
	}
	Add_Ref();
	return(this);
}

// 0x77E610: 0x77E3F0 without the reference (SClient's 0x74C840)
CharacterViewAvatarClass *CharacterViewAvatarClass::Unknown_77E610()
{
	if (!Unknown221) {
		Unknown_77D7F0();
		Unknown_77D4A0();
		Unknown221 = true;
	}
	return(this);
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
// (the overbright switch on and off in each Avatar_Type branch; K_HEAD's transform copied: frame 0x50)
void CharacterViewAvatarClass::Unknown_77E9D0(int body, int pants, bool morph)
{
	char body_name[32];
	Desc.Body_Type = (char)body;
	Desc.Pants_Type = (char)pants;
	if (BodyObj) {
		if (HeadObj) BodyObj->Remove_Sub_Object(HeadObj.Peek());
		if (WeaponUnique) BodyObj->Remove_Sub_Object(WeaponUnique.Peek());
		if (WeaponHip) BodyObj->Remove_Sub_Object(WeaponHip.Peek());
		if (WeaponBack) BodyObj->Remove_Sub_Object(WeaponBack.Peek());
		if (CollisionObj) BodyObj->Remove_Sub_Object(CollisionObj.Peek());
		if (Unknown2C4) BodyObj->Remove_Sub_Object(Unknown2C4.Peek());
		BodyObj->Release_Bone(BodyObj->Get_Bone_Index("K_Head"));
		BodyObj->Remove();
		BodyObj = NULL;

	}
	if (BodySource) {
		BodySource = NULL;

	}
	bool making = WW3D::Is_Overbright_Modify_On_Load_Enabled();
	if (Desc.Avatar_Type == 0) {
		if (!making) WW3D::Enable_Overbright_Modify_On_Load(true);
		_snprintf(body_name, 0x20, "%s%sBODY%02d%02d", Unknown_B9CD30[Desc.Gender], Unknown_B9CD38[Desc.Race], (int)Desc.Body_Type, (int)Desc.Pants_Type);
		BodySource = Create_Get(WW3DAssetManager::Get_Instance()->Create_Render_Obj(body_name));
		if (!making) WW3D::Enable_Overbright_Modify_On_Load(false);
	} else {
		if (!making) WW3D::Enable_Overbright_Modify_On_Load(true);
		_snprintf(body_name, 0x20, "ANPCBODY%03d", (int)Desc.Body_Type);
		BodySource = Create_Get(WW3DAssetManager::Get_Instance()->Create_Render_Obj(body_name));
		if (!making) WW3D::Enable_Overbright_Modify_On_Load(false);
	}
	if (BodySource) {
	Unknown_496AE0(BodySource.Peek(), true);
	BodyObj = Create_Get(BodySource->Clone());
	Unknown_496AE0(BodyObj.Peek(), true);
	Unknown_496DB0(BodyObj.Peek());
	Set_Name(BodyObj->Get_Name());
	Set_Base_Model_Name(BodyObj->Get_Base_Model_Name());
	Update_Obj_Space_Bounding_Volumes();
	if (HeadObj) BodyObj->Add_Sub_Object_To_Bone(HeadObj.Peek(), "K_HEAD");
	if (WeaponUnique) BodyObj->Add_Sub_Object_To_Bone(WeaponUnique.Peek(), "WEAPON02");
	if (WeaponBack) BodyObj->Add_Sub_Object_To_Bone(WeaponBack.Peek(), "WEAPON03");
	if (CollisionObj) BodyObj->Add_Sub_Object_To_Bone(CollisionObj.Peek(), "skel_reference");
	if (Unknown2C4) BodyObj->Add_Sub_Object_To_Bone(Unknown2C4.Peek(), "skel_reference");
	if (Desc.Gender == 0 && WeaponHip) {
		BodyObj->Add_Sub_Object_To_Bone(WeaponHip.Peek(), "WEAPON01");
	} else if (WeaponHip) {
		BodyObj->Add_Sub_Object_To_Bone(WeaponHip.Peek(), "WEAPON04");
	}
	Matrix3D head = BodyObj->Get_Bone_Transform("K_HEAD");
	Unknown234 = head[2][3] - BodyObj->Get_Bone_Transform("skel_reference")[2][3];
	if (morph) {
		Unknown_781410(1, 0);
	}
	} else {
		Unknown_4BC260("Avatar Creation", "", __FILE__, __LINE__, "Error loading up the body asset %s", body_name);
	}
}


































// 0x77F000: Head_Type's head loaded again as the head prototype ('<gender><race>HEAD%02d', 'ANPCHEAD%03d'
// for an NPC); the accessories' meshes come off the old one and onto the new one (0x789D80), the hair
// colour onto its hair and beard, then the head object is cloned again (0x77F750)
// (the four removals through 0x789FE0, inlined; the head loaded in each Avatar_Type branch with overbright switched on and off around it, as 0x77E9D0)
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
		if (HairSource) {
			Unknown_789FE0("HAIR");
		}
		if (Unknown2CC) {
			Unknown_789FE0("EAR");
		}
		if (GoggleSource) {
			Unknown_789FE0("GOGGLE");
		}
		if (BeardSource) {
			Unknown_789FE0("BEARD");
		}
		HeadSource = NULL;
	}
	bool making = WW3D::Is_Overbright_Modify_On_Load_Enabled();
	if (Desc.Avatar_Type == 0) {
		if (!making) WW3D::Enable_Overbright_Modify_On_Load(true);
		_snprintf(head_name, 0x20, "%s%sHEAD%02d", Unknown_B9CD30[Desc.Gender], Unknown_B9CD38[Desc.Race], (int)Desc.Head_Type);
		HeadSource = Create_Get(WW3DAssetManager::Get_Instance()->Create_Render_Obj(head_name));
		if (!making) WW3D::Enable_Overbright_Modify_On_Load(false);
	} else {
		if (!making) WW3D::Enable_Overbright_Modify_On_Load(true);
		_snprintf(head_name, 0x20, "ANPCHEAD%03d", (int)Desc.Head_Type);
		HeadSource = Create_Get(WW3DAssetManager::Get_Instance()->Create_Render_Obj(head_name));
		if (!making) WW3D::Enable_Overbright_Modify_On_Load(false);
	}

	if (HeadSource) {
		Unknown_496AE0(HeadSource.Peek(), true);
		if (HeadSource) {
			if (HairSource) {
				Unknown_789D80(&HairSource, "HAIR");
				Unknown_77FFA0(Desc.Hair_Color, false);
			}
			if (Unknown2CC) {
				Unknown_789D80(&Unknown2CC, "EAR");
			}
			if (GoggleSource) {
				Unknown_789D80(&GoggleSource, "GOGGLE");
			}
			if (BeardSource) {
				Unknown_789D80(&BeardSource, "BEARD");
				Unknown_7802F0(Desc.Hair_Color, false);
			}
		}
		Unknown_77F750();
	} else {
		Unknown_4BC260("Avatar Creation", "", __FILE__, __LINE__, "Error loading up the head asset %s", head_name);
	}
}

























// 0x77F750: the head object cloned again from the head prototype (with the accessories it now
// holds) and put on the body's K_HEAD bone
// (exact once HeadObj is a RefCountPtr)
void CharacterViewAvatarClass::Unknown_77F750()
{
	FAIL_IF(!HeadSource || !BodyObj) {
		return;
	}
	if (HeadObj) {
		BodyObj->Remove_Sub_Object(HeadObj.Peek());
		HeadObj = NULL;

	}
	HeadObj = Create_Get(HeadSource->Clone());
	Unknown_496AE0(HeadObj.Peek(), true);
	Unknown_496DB0(HeadObj.Peek());
	if (Desc.Avatar_Type == 0) {
		Unknown_780F30();
		Unknown_7804A0(Desc.Eye_Color);
		Unknown_7857D0(Desc.Head_Texture_Num);
		Unknown_7859F0(Desc.Tattoo_Texture_Num);
		Unknown_785C00(Desc.Tattoo_Offset);
	}
	BodyObj->Add_Sub_Object_To_Bone(HeadObj.Peek(), "K_HEAD");
}







// 0x77F960: the accessory '<gender><race><part><num>' (AFTHAIR03) made into *slot; the meshes
// '<part>0'..'<part>4' of the head prototype are removed first, the new ones added by 0x789D80
// (the slot a RefCountPtr: Create_Get, then its operator =; the prototype a RefCountPtr local)
void CharacterViewAvatarClass::Unknown_77F960(const char *part, int num, RefCountPtr<RenderObjClass> *slot)
{
	FAIL_IF(!HeadSource) {
		return;
	}
	Unknown_789FE0(part);
	if (*slot) {
		*slot = NULL;

	}
	bool making = WW3D::Is_Overbright_Modify_On_Load_Enabled();
	if (!making) {
		WW3D::Enable_Overbright_Modify_On_Load(true);
	}
	char model[32];
	_snprintf(model, 0x20, "%s%s%s%02d", Unknown_B9CD30[Desc.Gender], Unknown_B9CD38[Desc.Race], part, num);
	RefCountPtr<RenderObjClass> proto = Create_Get(WW3DAssetManager::Get_Instance()->Create_Render_Obj(model));
	if (!making) {
		WW3D::Enable_Overbright_Modify_On_Load(false);
	}
	if (proto) {
		*slot = Create_Get(proto->Clone());
		Unknown_496AE0(slot->Peek(), true);
		Unknown_789D80(slot, part);

	}
}

// 0x77FC20: Hair_Num's hair ('HAIR'), Hair_Color onto it, then (rebuild) the head object cloned again
// (exact once 0x77FFA0 / 0x77FEC0 hold RefCountPtrs)
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
// (exact once 0x7802F0 / 0x77FEC0 hold RefCountPtrs)
void CharacterViewAvatarClass::Unknown_77FD30(int num, bool rebuild)
{
	Desc.Beard_Num = (char)num;
	Unknown_77F960("BEARD", num, &BeardSource);
	Unknown_7802F0(Desc.Beard_Color, false);
	if (rebuild) {
		Unknown_77F750();
	}
}

// 0x77FDA0
void CharacterViewAvatarClass::Unknown_77FDA0(int num)
{
	if (BodyObj && num) {
		Desc.Weapon_Unique_Num = num;
		if (Desc.Gender == 0) {
			Unknown_77E0E0(0, "WEAPLEG", Desc.Weapon_Unique_Num, "WEAPON01", &WeaponHip);
		} else {
			Unknown_77E0E0(0, "WEAPARM", Desc.Weapon_Unique_Num, "WEAPON04", &WeaponHip);
		}
	}
}

// 0x77FE20
void CharacterViewAvatarClass::Unknown_77FE20(int num)
{
	if (BodyObj && num) {
		Desc.Weapon_Hip_Num = num;
		Unknown_77E0E0(0, "WEAPHIP", Desc.Weapon_Hip_Num, "WEAPON02", &WeaponUnique);
	}
}

// 0x77FE70
void CharacterViewAvatarClass::Unknown_77FE70(int num)
{
	if (BodyObj && num) {
		Desc.Weapon_Back_Num = num;
		Unknown_77E0E0(2, "WEAPBAK", Desc.Weapon_Back_Num, "WEAPON03", &WeaponBack);
	}
}

// 0x77FEC0: the colour onto the mesh's vertex material (ambient and diffuse)
// (robj a RefCountPtr slot; the material info a RefCountPtr local, Create_Get 0x47D370)
void CharacterViewAvatarClass::Unknown_77FEC0(RefCountPtr<RenderObjClass> *robj, const Vector3 &color, int index)
{
	if (*robj) {
		RefCountPtr<MaterialInfoClass> info = Create_Get((*robj)->Get_Material_Info());
		if (info) {
			VertexMaterialClass *material = info->Peek_Vertex_Material(index);
			if (material) {
				material->Set_Ambient(color);
				material->Set_Diffuse(color);
			}
		}
		Unknown_496AE0(robj->Peek(), true);



	}
}

// 0x77FFA0: the hair colour onto the head prototype's HAIR0..4 and BEARD0..4 meshes; rebuild: 0x77F750
// (each mesh a RefCountPtr local, Create_Get 0x474080; 0x77FEC0 and 0x7802F0 inline)
void CharacterViewAvatarClass::Unknown_77FFA0(const Vector3 &color, bool rebuild)
{
	char name[64];
	int i;
	Desc.Hair_Color = color;
	for (i = 0; i < 5; i++) {
		_snprintf(name, 0x40, "HAIR%d", i);
		RefCountPtr<RenderObjClass> robj = Create_Get(HeadSource->Get_Sub_Object_By_Name(name));
		if (robj) {
			Unknown_77FEC0(&robj, color, 0);

		}
	}
	Unknown_7802F0(color, false);
	if (rebuild) {
		Unknown_77F750();
	}
}

// 0x7802F0: the beard colour onto the head prototype's BEARD0..4 meshes; rebuild: 0x77F750
// (each mesh a RefCountPtr local, Create_Get 0x474080; 0x77FEC0 inline)
void CharacterViewAvatarClass::Unknown_7802F0(const Vector3 &color, bool rebuild)
{
	char name[64];
	Desc.Beard_Color = color;
	for (int i = 0; i < 5; i++) {
		_snprintf(name, 0x40, "BEARD%d", i);
		RefCountPtr<RenderObjClass> robj = Create_Get(HeadSource->Get_Sub_Object_By_Name(name));
		if (robj) {
			Unknown_77FEC0(&robj, color, 0);

		}
	}
	if (rebuild) {
		Unknown_77F750();
	}
}

// 0x7804A0: the eye colour onto the head object's LEYE010..4 and REYE010..4 meshes (vertex material 1)
// (each mesh a RefCountPtr local, Create_Get 0x474080; 0x77FEC0 inline)
void CharacterViewAvatarClass::Unknown_7804A0(const Vector3 &color)
{
	Desc.Eye_Color = color;
	if (HeadObj) {
		for (int side = 0; side < 2; side++) {
			for (int i = 0; i < 5; i++) {
				char name[32];
				_snprintf(name, 0x20, "%sEYE01%d", Unknown_B9CD44[side], i);
				RefCountPtr<RenderObjClass> robj = Create_Get(HeadObj->Get_Sub_Object_By_Name(name));
				if (robj) {
					Unknown_77FEC0(&robj, color, 1);

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
		Unknown_80FCE0(BodyObj.Peek(), regions, env_maps, &Desc.Skin_Color);
		Unknown_80FCE0(HeadObj.Peek(), regions, env_maps, &Desc.Skin_Color);
	}
}





























































































// 0x780AF0
// NOT EXACT: functional for POC (D64), literal (__LINE__)
void CharacterViewAvatarClass::Unknown_780AF0()
{
	FAIL_IF(!BodyObj || !BodySource) { return; }
	Vector3 scale(0.0f, 0.0f, 0.0f); int i;
	for (i = 0; i < 5; i++) {
		float w = Unknown274[i];
		scale += Desc.Get_Body_Morph(i) * w;


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
	((HLodClass *)BodyObj.Peek())->Set_HTree(tree);
	if (tree) {
		delete tree;
	}
}




















// 0x780F30
// (exact once Get_Head_Morph returns by value and HeadObj is a RefCountPtr)
void CharacterViewAvatarClass::Unknown_780F30()
{
	FAIL_IF(HeadObj == 0 || HeadSource == 0) { return; }
	Vector3 scale(0.0f, 0.0f, 0.0f);
	int i;
	for (i = 0; i < 5; i++) {
		float w = Unknown288[i];
		scale += Desc.Get_Head_Morph(i) * w;




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
	((HLodClass *)HeadObj.Peek())->Set_HTree(tree);
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
// (1 - keep computed once, before the loop; each product parenthesised: the client's x87 order)
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
			Unknown274[i] = (keep * Unknown274[i]) + (rest * Desc.Body_Weight[i]);
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
// (each product parenthesised: the client's x87 order)
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
			Unknown288[i] = (keep * Unknown288[i]) + (rest * Desc.Head_Weight[i]);
		}
	}
	if (!moving) {
		Unknown29D = false;
	}
	Unknown_780F30();
}








// 0x781680: a new NLP from Desc's personality and lexicon (FAIL_IF on the client's line 1524 when one is there)
void CharacterViewAvatarClass::Unknown_781680()
{
	FAIL_IF(PlayerNLP != 0) {
		return;
	}
	PlayerNLP = new PlayerNLPClass();
	PlayerNLP->Unknown_78EC00(Desc.Personality, Desc.NLP);
}

// 0x781760
void CharacterViewAvatarClass::Unknown_781760(const char *text)
{
	if (PlayerNLP) {
		Unknown0E8.Unknown_4C44A0();
		PlayerNLP->Unknown_78F690(text, strlen(text), &Desc.Mood_Type, &Unknown0E8);
		Unknown0E8.Unknown_4C4410();
	}
}

// 0x781800
void CharacterViewAvatarClass::Unknown_781800(Unknown_ChatStreamClass *stream)
{
	Unknown0E8.Unknown_4C44A0();
	void *data = stream->Unknown04;
	Unknown0E8.Unknown_4C44E0(data, stream->Unknown_04());
}

// 0x781840
void CharacterViewAvatarClass::Unknown_781840(const void *data, int size)
{
	Unknown0E8.Unknown_4C44A0();
	Unknown0E8.Unknown_4C44E0(data, size);
}

// 0x781880
// (push_back: VC6 vector's insert inlined)
void CharacterViewAvatarClass::Unknown_781880(char anim, const char *text, int mood)
{
	Unknown2A0.push_back(new Unknown_AvatarQueueStruct(anim, text, mood));
}

// 0x781B20
Unknown_AvatarQueueStruct *CharacterViewAvatarClass::Unknown_781B20()
{
	if (Unknown2A0.size() > 0) {
		return Unknown2A0[0];
	}
	return NULL;
}

// 0x781B50
void CharacterViewAvatarClass::Unknown_781B50(Unknown_AvatarQueueStruct *entry)
{
	if (entry) {
		unsigned int i;
		for (i = 0; i < Unknown2A0.size(); i++) {
			if (Unknown2A0[i] == entry) {
				Unknown2A0.erase(Unknown2A0.begin() + i);
				delete entry;
				return;
			}
		}
	}
}

// 0x781C50
// (its callers inline it: the one more inline layer is what leaves their ??_G 0x781C10 and clear 0x78A510 out of line)
void CharacterViewAvatarClass::Unknown_781C50()
{
	unsigned int i;
	for (i = 0; i < Unknown2A0.size(); i++) {
		delete Unknown2A0[i];
	}
	Unknown2A0.clear();
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
		Unknown_785480();
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
		if (Unknown0C0[1]) {
			frames = Unknown0C0[1]->Get_Num_Frames();
		} else {
			frames = Unknown0C0[0]->Get_Num_Frames();
		}
		int left = Unknown248 - frames;
		Unknown248 = (left < 0) ? 0 : left;

	} else {
	Unknown_AvatarQueueStruct *entry = Unknown_781B20();
	if (entry != NULL) {
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
		Unknown_781B50(entry);
	}
	if (Unknown_781B20() == NULL && Unknown248 == 0) {
		Unknown208 = 0;
	}
	}
}

// 0x7826A0
// (one _snprintf per branch; the back flag read as a bool of the unsigned shift)
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
	if ((bool)(((unsigned int)Unknown2F8 >> 1) & 1)) {
		_snprintf(name, 0x20, "%s.ABANIM%02d-%02d", Unknown_B9CD28[Desc.Gender], (int)Desc.Personality, NLP_Of(this)->Unknown_78F640("walk_backward"));
	} else if (Unknown2F4) {
		_snprintf(name, 0x20, "%s.ABANIM%02d-%02d", Unknown_B9CD28[Desc.Gender], (int)Desc.Personality, NLP_Of(this)->Unknown_78F640("run_forward"));
	} else {
		_snprintf(name, 0x20, "%s.ABANIM%02d-%02d", Unknown_B9CD28[Desc.Gender], (int)Desc.Personality, NLP_Of(this)->Unknown_78F640("walk_forward"));
	}


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
// (frames 0 when neither animation is set, the section loop a while)
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
		frames = 0;
	}
	if (frames < 3) {
		return(false);
	}
	Unknown0BC->Insert_Morph_Key(1, 0, Unknown240);
	Unknown0BC->Insert_Morph_Key(2, 0, 0);
	int mood = 0;
	char section[64];
	int index = 0;
	while (true) {
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
				int key = brow + mood * 5;
				Unknown240 = key;
				Unknown0BC->Insert_Morph_Key(1, frame, key);
			}
			int eye = ini.Get_List_Index(section, "Eye", -1, eyes);
			if (eye >= 0) {
				Unknown0BC->Insert_Morph_Key(2, frame, eye);
			}
		}
		index++;
	}
	Unknown240 = Desc.Mood_Type * 5;
	Unknown0BC->Insert_Morph_Key(1, frames - 1, Unknown240);
	Unknown0BC->Insert_Morph_Key(2, frames - 1, 0);
	return(true);
}

// 0x782CA0
// (each pass's start written as i * step: the client's induction variables start after the count test)
void CharacterViewAvatarClass::Unknown_782CA0()
{
	int delay = PlayerNLP->Personality->Get_Blink_Delay();
	int step = (delay < 1) ? 1 : delay;
	int frames;
	if (Unknown0C0[1] != NULL) {
		frames = Unknown0C0[1]->Get_Num_Frames() + Unknown248;
	} else {
		frames = Unknown0C0[0]->Get_Num_Frames() + Unknown248;
	}
	int count;
	if (frames < step) {
		count = Unknown_BDBB84(0, 1);
		step = frames;
	} else {
		int n = Unknown_BDBB84(1, frames / step);
		count = (frames / 4 < n) ? frames / 4 : n;
		step = frames / count;
	}
	int brow = Desc.Mood_Type * 5;
	if (Unknown208 == 1) {
		if (Unknown_BDBB84(0, 1) == 1) {
			brow += 2;
		}
	} else {
		if (Unknown_BDBB84(0, 3) == 1) {
			brow += 2;
		}
	}
	Unknown0BC->Insert_Morph_Key(1, 0, Unknown240);
	int i;
	for (i = 0; i < count; i++) {
		int at = i * step + 2;
		int key = Unknown_BDBB84(at, at - 7 + step);
		Unknown0BC->Insert_Morph_Key(1, key, brow);
		Unknown0BC->Insert_Morph_Key(1, key + 2, brow + 1);
		Unknown0BC->Insert_Morph_Key(1, key + 4, brow);
	}
	int last = frames - 1;
	Unknown0BC->Insert_Morph_Key(1, last, brow);
	Unknown240 = brow;
	Unknown0BC->Insert_Morph_Key(2, 0, 0);
	for (i = 0; i < count; i++) {
		int at2 = i * step;
		int shut = Unknown_BDBB84(0, 1) * 2 + 1;
		int hold = Unknown_BDBB84(2, step / 4);
		int key = Unknown_BDBB84(at2 + 1, at2 - hold - 4 + step);
		Unknown0BC->Insert_Morph_Key(2, key, 0);
		Unknown0BC->Insert_Morph_Key(2, key + 2, shut);
		Unknown0BC->Insert_Morph_Key(2, key + 2 + hold, shut);
		Unknown0BC->Insert_Morph_Key(2, key + 4 + hold, 0);
	}
	Unknown0BC->Insert_Morph_Key(2, last, 0);
}

// 0x782FF0
// the frame count from the second animation if set, else the first (read as raw pointers: through RefCountPtr's inlines the loop's lea operands swap)
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
	HAnimClass **raw = (HAnimClass **)Unknown0C0;
	int frames;
	if (raw[1] != NULL) {
		frames = raw[1]->Get_Num_Frames();
	} else {
		frames = raw[0]->Get_Num_Frames();
	}
	Unknown248 = (Unknown24C - frames < 0) ? 0 : Unknown24C - frames;

	if (strcmp(text, "") == 0) {
		Unknown24C = -1;
	}
}

































































































































































































// 0x7831C0
// (the 0.5 a constant in .rdata, 0xB04494, shared with 0x783620; the not-rested test as a bool)
void CharacterViewAvatarClass::Unknown_7831C0(const char *name)
{
	static const float blend = 0.5f;	// NAME 1: neutral (D10)
	int i = 1;
	bool fresh = !Unknown215;
	if (Unknown0C0[0] == NULL || !fresh) {
		i = 0;


		Unknown0C0[1] = NULL;

		Unknown0C8[1] = false;
		Unknown0CC[1] = 0.0f;
		Unknown0D4[1] = 0.0f;
		Unknown0DC[1] = -1;
	}


	Unknown0C0[i] = NULL;

	Unknown0C8[i] = false;
	Unknown0CC[i] = 0.0f;
	Unknown0D4[i] = 0.0f;
	Unknown0DC[i] = -1;
	Unknown0C0[i] = Create_Get(WW3DAssetManager::Get_Instance()->Get_HAnim(name));
	if (Unknown0C0[i] != NULL) {
		if (i == 1) {
			Unknown218 = blend;
			float rate = Unknown0C0[1]->Get_Frame_Rate();
			if (rate > 0.0001f) {
				float t = (float)Unknown0C0[1]->Get_Num_Frames() / rate;
				if (t < Unknown218) {
					Unknown218 = t;
				}
			}
		} else {
			Unknown218 = 0.0f;
		}
		Unknown21C = Unknown218;
	} else {
		Unknown_4BC260("Avatar Animation", "", __FILE__, __LINE__, "Cannot Find Gesture Animation file %s for gender %d, persoanlity %d",
			name, Desc.Gender, (int)Desc.Personality);
	}
	Unknown215 = false;
}

// 0x783470
// the head sub-object playing the face animation (+0xBC, a RefCountPtr)
void CharacterViewAvatarClass::Unknown_783470()
{
	char name[32];
	if (Desc.Avatar_Type == 0) {
		_snprintf(name, 0x20, "%s%sHEAD%02d", Unknown_B9CD30[Desc.Gender], Unknown_B9CD38[Desc.Race], (int)Desc.Head_Type);
	} else {
		_snprintf(name, 0x20, "ANPCHEAD%03d", (int)Desc.Head_Type);
	}
	RefCountPtr<RenderObjClass> head = Create_Get(BodyObj->Get_Sub_Object_By_Name(name));
	if (head) {
		if (Unknown0BC->Get_Num_Frames() > 0) {
			head->Set_Animation(Unknown0BC.Peek(), 0.0f, RenderObjClass::ANIM_MODE_ONCE);
		} else {
			head->Set_Animation();
		}

	}
}

// 0x7835D0
bool CharacterViewAvatarClass::Unknown_7835D0()
{
	if (Unknown0C0[1] != NULL) {
		return false;
	}
	if (Unknown214) {
		Unknown214 = false;
		Unknown248 = 0;
		return true;
	}
	if (Unknown0C0[0] == NULL) {
		return true;
	}
	return Unknown0C8[0];
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
		BodyObj->Set_Animation(Unknown0C0[0].Peek(), Unknown0CC[0], 0);
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
	BodyObj->Set_Animation(Unknown0C0[0].Peek(), Unknown0CC[0], Unknown0C0[1].Peek(), Unknown0CC[1], blend * 0.5f);
	if (Unknown21C >= 0.0f) {
		return;
	}
	Unknown21C = 0.0f;
	Unknown218 = 0.0f;



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
		Unknown_4BC260("Avatar Animation", "", __FILE__, __LINE__, "Invalid avatar objects (%x %x %x)", BodyObj.Peek(), HeadObj.Peek(), PlayerNLP);
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
	{
		if (Unknown_7835D0()) {
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
// (the queue emptied by 0x781C50, cases 4-7 by 0x784FF0 / 0x785040 / 0x784ED0 / 0x784F20 and the Value reads through an inline helper: the budget they spend leaves 0x781C50's ??_G 0x781C10 and clear 0x78A510 out of line, as the client calls them)
void CharacterViewAvatarClass::Unknown_784700()
{
	const Unknown_ChatBlockStruct *block = (const Unknown_ChatBlockStruct *)Unknown0E8.Unknown_00();
	if (block == NULL) {
		return;
	}
	Unknown_781C50();





	int mood = -1;
	for (;;) {
		switch (block->Type) {
			case 0:
				if (Unknown210 != 1 && Unknown210 != 2 && Unknown20C == 2) {
					mood = (char)Unknown_Block_Value(block);
				}
				break;
			case 1:
				if (Unknown_Block_Value(block) && Unknown210 != 1 && Unknown210 != 2 && Unknown20C == 2) {
					Unknown_781880(Unknown_Block_Value(block), (const char *)(block + 1), mood);
					Unknown208 = 1;
					mood = -1;
				}
				break;
			case 2:
				if (Unknown_Block_Value(block) && Unknown210 != 1 && Unknown210 != 2 && Unknown20C == 2) {
					Unknown_781880(Unknown_Block_Value(block), "", mood);
					Unknown208 = 1;
					mood = -1;
				}
				break;
			case 3:
				Desc.Mood_Type = (char)Unknown_Block_Value(block);
				break;
			case 4:
				Unknown_784FF0();
				break;
			case 5:
				Unknown_785040();
				break;
			case 6:
				Unknown_784ED0();
				break;
			case 7:
				Unknown_784F20();
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
		Unknown_781760(name);


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
		Unknown_781840(data, size);

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

// 0x784ED0
void CharacterViewAvatarClass::Unknown_784ED0()
{
	if (Unknown20C == 2 && Unknown210 == 0) {
		Unknown20C = 0;
		Unknown214 = true;
		Unknown208 = 2;
	}
}

// 0x784F20
void CharacterViewAvatarClass::Unknown_784F20()
{
	if (Unknown20C != 2) {
		Unknown208 = 0;
		Unknown20C = 2;
		Unknown214 = true;
	}
}

// 0x784F60
void CharacterViewAvatarClass::Unknown_784F60(bool on)
{
	if (Unknown2F4 != on) {
		Unknown2F4 = on;
		if (Unknown2F8 & 1) {
			Unknown_784F20();
			Unknown_784ED0();
		}
	}
}

// 0x784FF0
void CharacterViewAvatarClass::Unknown_784FF0()
{
	if (Unknown210 == 0) {
		Unknown210 = 1;
		Unknown22C = 0.0f;
		Unknown244 = 0;
		Unknown208 = 3;
		Unknown214 = true;
		Unknown20C = 2;
	}
}

// 0x785040
void CharacterViewAvatarClass::Unknown_785040()
{
	if (Unknown210 == 3) {
		Unknown210 = 2;
		Unknown208 = 3;
		Unknown214 = true;
	}
}

// 0x785080: +0xE8 emptied, then one block by kind (0 type 6, 1 type 7, 2 type 4, 3 type 5, 4..7 type 3 with
// value 0..3) and the end block (unreferenced)
void CharacterViewAvatarClass::Unknown_785080(int kind)
{
	Unknown0E8.Unknown_4C44A0();
	switch (kind) {
		case 0:
			Unknown0E8.Unknown_4C45B0(6, 0, NULL, 0);
			break;
		case 1:
			Unknown0E8.Unknown_4C45B0(7, 0, NULL, 0);
			break;
		case 2:
			Unknown0E8.Unknown_4C45B0(4, 0, NULL, 0);
			break;
		case 3:
			Unknown0E8.Unknown_4C45B0(5, 0, NULL, 0);
			break;
		case 4:
			Unknown0E8.Unknown_4C45B0(3, 0, NULL, 0);
			break;
		case 5:
			Unknown0E8.Unknown_4C45B0(3, 1, NULL, 0);
			break;
		case 6:
			Unknown0E8.Unknown_4C45B0(3, 2, NULL, 0);
			break;
		case 7:
			Unknown0E8.Unknown_4C45B0(3, 3, NULL, 0);
			break;
	}
	Unknown0E8.Unknown_4C4410();
}

// 0x7851F0: the head's look-at angles +0x250 / +0x258 toward target: the direction from K_Head, less the body's X
// axis, against its Y and Z axes, in degrees (unreferenced)
void CharacterViewAvatarClass::Unknown_7851F0(Vector3 target)
{
	Matrix3D tm = BodyObj->Get_Transform();
	int bone = BodyObj->Get_Bone_Index("K_Head");
	Vector3 head_pos = BodyObj->Get_Bone_Transform(bone).Get_Translation();
	Vector3 x = tm.Get_X_Vector();
	Vector3 dir = target - head_pos;
	dir.Normalize();
	target = dir - x;
	Vector3 z = tm.Get_Z_Vector();
	Vector3 y = tm.Get_Y_Vector();
	Unknown250 = RAD_TO_DEG(asin(Vector3::Dot_Product(y, target)));
	Unknown258 = RAD_TO_DEG(asin(Vector3::Dot_Product(z, target)));
}

// 0x785450
void CharacterViewAvatarClass::Unknown_785450(float yaw, float pitch)
{
	Unknown250 = yaw;
	Unknown258 = pitch;
}

// 0x785480
void CharacterViewAvatarClass::Unknown_785480()
{
	Unknown250 = 0.0f;
	Unknown258 = 0.0f;
	Unknown264 = 0.0f;
	Unknown268 = 0.0f;
}

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
// (the texture and each mesh RefCountPtr locals: Create_Get 0x47D010 / 0x474080)
void CharacterViewAvatarClass::Unknown_7857D0(int num)
{
	FAIL_IF(!HeadObj) {
		return;
	}
	char name[32];
	char mesh[64];
	_snprintf(name, 0x20, "%s%sFACETEX%02d.tga", Unknown_B9CD30[Desc.Gender], Unknown_B9CD38[Desc.Race], num);
	RefCountPtr<TextureClass> tex = Create_Get(WW3DAssetManager::Get_Instance()->Get_Texture(name, MIP_LEVELS_ALL, WW3D_FORMAT_UNKNOWN, true));
	if (tex) {
		Desc.Head_Texture_Num = (char)num;
		for (int i = 0; i < 5; i++) {
			_snprintf(mesh, 0x40, "head%d", i);
			RefCountPtr<RenderObjClass> robj = Create_Get(HeadObj->Get_Sub_Object_By_Name(mesh));
			Unknown_495FD0(robj.Peek(), tex.Peek(), 0);
			Unknown_496AE0(robj.Peek(), true);

		}
	}
	Unknown_496AE0(HeadObj.Peek(), true);

}


// 0x7859F0: Tattoo_Texture_Num's 'ATATTOO<num>.tga' (clamped) onto the head object's HEAD0..4
// meshes (stage 1)
// (the texture and each mesh RefCountPtr locals: Create_Get 0x47D010 / 0x474080)
void CharacterViewAvatarClass::Unknown_7859F0(int num)
{
	FAIL_IF(!HeadObj) {
		return;
	}
	char name[32];
	char mesh[64];
	_snprintf(name, 0x20, "ATATTOO%02d.tga", num);
	RefCountPtr<TextureClass> tex = Create_Get(WW3DAssetManager::Get_Instance()->Get_Texture(name, MIP_LEVELS_ALL, WW3D_FORMAT_UNKNOWN, true));
	if (tex) {
		tex->Set_U_Addr_Mode(TextureClass::TEXTURE_ADDRESS_CLAMP);
		tex->Set_V_Addr_Mode(TextureClass::TEXTURE_ADDRESS_CLAMP);
		for (int i = 0; i < 5; i++) {
			_snprintf(mesh, 0x40, "HEAD%d", i);
			RefCountPtr<RenderObjClass> robj = Create_Get(HeadObj->Get_Sub_Object_By_Name(mesh));
			Unknown_495FD0(robj.Peek(), tex.Peek(), 1);
			Unknown_496AE0(robj.Peek(), true);

		}
		Desc.Tattoo_Texture_Num = (char)num;
	}
	Unknown_496AE0(HeadObj.Peek(), true);

}





// 0x785C00: Tattoo_Offset onto the head object's HEAD0..4 meshes: a linear offset mapper on stage 1
// of vertex material 0 (X, Y the tattoo's place, Z its scale, clamped to 0.5 .. 16)
// (the differences as a Vector2, then each start component divided: the client's x87 order)
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
	Vector2 scales(scale, scale);
	Vector2 o(offset.X - half, offset.Y - half);
	Vector2 start;
	start.X = -(o.X / size);
	start.Y = -(o.Y / size);
	char mesh[64];
	for (int i = 0; i < 5; i++) {
		_snprintf(mesh, 0x40, "HEAD%d", i);
		RefCountPtr<RenderObjClass> robj = Create_Get(HeadObj->Get_Sub_Object_By_Name(mesh));
		if (robj) {
			RefCountPtr<MaterialInfoClass> info = Create_Get(robj->Get_Material_Info());
			if (info) {
				VertexMaterialClass *material = info->Peek_Vertex_Material(0);
				LinearOffsetTextureMapperClass *mapper = new LinearOffsetTextureMapperClass(Vector2(0.0f, 0.0f), start, true, scales, 1);
				material->Set_Mapper(mapper, 1);
				mapper->Release_Ref();

			}

		}
	}
	Unknown_496AE0(HeadObj.Peek(), true);
	Desc.Tattoo_Offset = offset;
}

// 0x785F20
void CharacterViewAvatarClass::Unknown_785F20(bool on)
{
	if (Unknown2C4) {
		BodyObj->Remove_Sub_Object(Unknown2C4.Peek());
		Unknown2C4->Remove();
		Unknown2C4 = NULL;
	}
	if (on) {
		Unknown2C4 = Create_Get(WW3DAssetManager::Get_Instance()->Create_Render_Obj("avatar_shadow"));
		if (Unknown2C4) {
			Unknown_496AE0(Unknown2C4.Peek(), true);
			BodyObj->Add_Sub_Object_To_Bone(Unknown2C4.Peek(), "skel_reference");
		}
	}
}

// 0x7860A0: empty (unreferenced)
void CharacterViewAvatarClass::Unknown_7860A0()
{
}

// 0x7860C0
// (bool: mov al, 1 / xor al, al)
bool CharacterViewAvatarClass::Unknown_7860C0()
{
	if (Desc.Avatar_Type != 0) {
		TutorialLineStruct *line = TutorialManagerClass::Instance->Next_Line();
		if (line) {
			const char *text = line->Text;
			Unknown_781760(text);





			Unknown_7861B0(line->Speech);
			Unknown215 = line->SkipBlend;
			Unknown_784700();
			Unknown24C = -1;
			return(true);
		}
	}
	return(false);
}

// 0x7861B0
// (SpeechSound a RefCountPtr: Create_Get, then its operator=)
void CharacterViewAvatarClass::Unknown_7861B0(const char *speech)
{
	Unknown_7862D0();
	SpeechSound = Create_Get(SoundLibraryClass::Play_2D_Sound(speech, 1));
}

// 0x7862D0
void CharacterViewAvatarClass::Unknown_7862D0()
{
	if (SpeechSound) {
		SpeechSound->Stop(1);
		SpeechSound = NULL;
	}
}

// 0x786320
void CharacterViewAvatarClass::Unknown_786320()
{
	if (Unknown2F4) {
		Unknown_4428E0(Unknown23C, Unknown330);
	} else {
		Unknown_442930(Unknown23C, Unknown330);
	}
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













































// 0x786410: *camera_m from the avatar's transform turned a quarter about X and Y, then moved to K_HEAD (its offset
// from skel_reference, 0.1 above or below) and 2.5 (close up) or 3.0 along its Z (FAIL_IF on the client's line 3242)
// NOT EXACT: register (the Set_Translation stores' y / z go through ecx / edx in the client, edx / eax here: 0.94)
void CharacterViewAvatarClass::Unknown_786410(Matrix3D *camera_m, bool close_up)
{
	FAIL_IF(!camera_m) return;
	*camera_m = Get_Transform();
	camera_m->Rotate_X(DEG_TO_RADF(90.0f));
	camera_m->Rotate_Y(DEG_TO_RADF(90.0f));
	Matrix3D head = BodyObj->Get_Bone_Transform("K_HEAD");
	Matrix3D ref = BodyObj->Get_Bone_Transform("skel_reference");
	float height = head.Get_Z_Translation() - ref.Get_Z_Translation();
	Vector3 offset = head.Get_Translation() - ref.Get_Translation();
	float x = Vector3::Dot_Product(camera_m->Get_X_Vector(), offset);
	if (close_up) {
		float y = height - -0.1f;
		float z = Vector3::Dot_Product(camera_m->Get_Z_Vector(), offset);
		camera_m->Set_Translation(camera_m->Get_Translation() + camera_m->Get_Y_Vector() * y + camera_m->Get_X_Vector() * x + camera_m->Get_Z_Vector() * z);
		camera_m->Set_Translation(camera_m->Get_Translation() + camera_m->Get_Z_Vector() * 2.5f);
	} else {
		float y = height - 0.1f;
		camera_m->Set_Translation(camera_m->Get_Translation() + camera_m->Get_Y_Vector() * y + camera_m->Get_X_Vector() * x);
		camera_m->Set_Translation(camera_m->Get_Translation() + camera_m->Get_Z_Vector() * 3.0f);
	}
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

// 0x786EB0: the transform +0x330 set, then skel_reference's raised to K_HEAD's height + 0.2, moved half a unit along
// its X and pitched by +0x30C (0x787A90 calls it)
Matrix3D CharacterViewAvatarClass::Unknown_786EB0()
{
	Set_Transform(Unknown330);
	Matrix3D head = BodyObj->Get_Bone_Transform("K_HEAD");
	Matrix3D m = BodyObj->Get_Bone_Transform("skel_reference");
	static const float rise = 0.2f;
	m.Adjust_Z_Translation(head.Get_Z_Translation() - m.Get_Z_Translation() + rise);
	m.Adjust_Translation(m.Get_X_Vector() * 0.5f);
	m.Rotate_Y(Unknown30C);
	return(m);
}

// 0x787130: the transform +0x330 set, then skel_reference's raised by +0x234 + 2, turned by +0x310 about Z, moved
// 11 back and pitched by +0x30C + 0.3 (unreferenced)
// (height a local: VC6 moves a literal summand after m[2][3], as the client's second 1.0f)
Matrix3D CharacterViewAvatarClass::Unknown_787130()
{
	Set_Transform(Unknown330);
	Matrix3D m = BodyObj->Get_Bone_Transform("skel_reference");
	float height = Unknown234 + 1.0f;
	m.Adjust_Z_Translation(height + 1.0f);
	m.Rotate_Z(Unknown310);
	m.Adjust_Translation(m.Get_X_Vector() * -11.0f);
	m.Rotate_Y(Unknown30C + 0.3f);
	return(m);
}

// 0x787450: the transform +0x330 set, then a camera 3.5 behind skel_reference (away from other), 3.5 aside, 8.5 up, looking at
// the point 7 above the middle between it and other's (the UI's 0x63D2F0)
// NOT EXACT: order-only (the second Normalize sums z*z + y*y where the client sums y*y first, or the reverse: 0.92)
Matrix3D CharacterViewAvatarClass::Unknown_787450(RenderObjClass *other)
{
	Set_Transform(Unknown330);
	Matrix3D a = BodyObj->Get_Bone_Transform("skel_reference");
	Matrix3D b = other->Get_Bone_Transform("skel_reference");
	Vector3 center = (a.Get_Translation() + b.Get_Translation()) * 0.5f;
	Vector3 dir = center - a.Get_Translation();
	dir.Normalize();
	Vector3 side = Vector3::Cross_Product(Vector3(0.0f, 0.0f, 1.0f), dir);
	center.Z += 7.0f;
	Vector3 pos = a.Get_Translation() - side * 3.5f;
	pos += -dir * 3.5f;
	pos.Z += 8.5f;
	Vector3 x = center - pos;
	x.Normalize();
	Vector3 y = Vector3::Cross_Product(Vector3(0.0f, 0.0f, 1.0f), x);
	Vector3 z = Vector3::Cross_Product(x, y);
	Matrix3D m(x, y, z, pos);
	m.Re_Orthogonalize();
	return(m);
}

// 0x787A90: the camera matrix, behind the avatar (0x787130, inlined) while +0x2F5 is set, else at its head (0x786EB0);
// the UI's 0x63BDB0 / 0x63C080 call it
Matrix3D CharacterViewAvatarClass::Unknown_787A90()
{
	if (Unknown2F5) {
		return(Unknown_787130());
	}
	return(Unknown_786EB0());
}

// 0x787DE0
Matrix3D CharacterViewAvatarClass::Unknown_787DE0()
{
	Matrix3D m = Unknown330;
	m.Adjust_Translation(-Unknown360);
	return m;
}

// 0x787F40: the frame's step +0x360 taken back off +0x330, swept as the collision object's box (0.1 up) against obj;
// on a hit the step slides along the wall's normal flattened to XY; the step is put back on +0x330 (the UI's 0x63BC30)
bool CharacterViewAvatarClass::Unknown_787F40(RenderObjClass *obj)
{
	if (obj) {
		if (Unknown360.Length() != 0.0f) {
			CastResultStruct result;
			bool hit = false;
			Unknown330.Adjust_Translation(-Unknown360);
			Matrix3D tm(true);
			tm.Set_Translation(Get_Transform().Get_Translation());
			AABoxClass box;
			CollisionObj->Get_Obj_Space_Bounding_Box(box);
			box.Center.Z += 0.1f;
			OBBoxClass obox(box.Center, box.Extent);
			OBBoxClass world;
			OBBoxClass::Transform(tm, obox, &world);
			OBBoxCollisionTestClass test(world, Unknown360, &result, COLLISION_TYPE_0);
			Matrix3D objtm = obj->Get_Transform();
			Matrix3D inv;
			objtm.Get_Orthogonal_Inverse(inv);
			OBBoxCollisionTestClass local_test(test, inv);
			if (obj->Cast_OBBox(local_test)) {
				hit = true;
				Vector3 normal;
				Matrix3D::Rotate_Vector(objtm, result.Normal, &normal);
				normal.Z = 0.0f;
				normal.Normalize();
				float d = Vector3::Dot_Product((Unknown330.Get_Translation() + Unknown360 * result.Fraction) - (Unknown330.Get_Translation() + Unknown360), normal) + 0.001f;
				Unknown360 += normal * d;
			}
			Unknown330.Adjust_Translation(Unknown360);
			return(hit);
		}
	}
	return(false);
}

// 0x7885D0
// (list clear(): the second clear() inlines the whole erase; the first one, budget spent, calls its iterator ++ 0x78A7F0 and erase 0x78A730)
void CharacterViewAvatarClass::Unknown_7885D0(bool second)
{
	std::list<Matrix3D *>::iterator it;
	if (second) {
		for (it = Unknown324.begin(); it != Unknown324.end(); it++) {
			delete *it;
		}
		Unknown324.clear();
	} else {
		for (it = Unknown318.begin(); it != Unknown318.end(); it++) {
			delete *it;
		}
		Unknown318.clear();
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
			Unknown_784F20();
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
// (exact once +0x370 / +0x374 are RefCountPtrs)
void CharacterViewAvatarClass::Unknown_788EF0()
{
	if (Unknown370) {
		Unknown370->Set_Animation();
		Unknown370->Remove();
		Unknown370 = NULL;
	}
	if (Unknown374) {
		Unknown374 = NULL;
	}
	Unknown380 = 0;
	Unknown378 = -100.0f;
	Unknown37C = 0.0f;
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
		Unknown370 = NULL;
	}
	if (Unknown374) {
		Unknown374 = NULL;
	}
	Unknown378 = -100.0f;
	Unknown37C = 0.0f;
	Unknown380 = mode;
	if (mode == 0) {
		return;
	}
	switch (mode) {
		case 1:
			Unknown370 = Create_Get(WW3DAssetManager::Get_Instance()->Create_Render_Obj("av_tele01"));
			Unknown374 = Create_Get(WW3DAssetManager::Get_Instance()->Get_HAnim("av_tele01a.Av_tele01a-1"));
			Unknown378 = 0.7f;
			Unknown37C = 3.2f;
			break;
		case 2:
			Unknown370 = Create_Get(WW3DAssetManager::Get_Instance()->Create_Render_Obj("av_tele03"));
			Unknown374 = Create_Get(WW3DAssetManager::Get_Instance()->Get_HAnim("av_tele03a.Av_tele03a-1"));
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
			Unknown370 = Create_Get(WW3DAssetManager::Get_Instance()->Create_Render_Obj("av_tele02"));
			Unknown374 = Create_Get(WW3DAssetManager::Get_Instance()->Get_HAnim("av_tele02a.Av_tele02a-1"));
			Unknown378 = 0.5f;
			Unknown37C = 1.0f;
			break;
		case 4:
			Unknown370 = Create_Get(WW3DAssetManager::Get_Instance()->Create_Render_Obj("av_tele04"));
			Unknown374 = Create_Get(WW3DAssetManager::Get_Instance()->Get_HAnim("av_tele04a.Av_tele04a-1"));
			Unknown_786370(true);
			Unknown378 = 0.0f;
			Unknown37C = 0.5f;
			break;
	}
	if (Unknown370 == NULL) {	// POC guard: the client goes on without testing
		return;
	}
	Unknown_496AE0(Unknown370.Peek(), true);
	BodyObj->Peek_Scene()->Add_Render_Object(Unknown370.Peek());
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
// (its tail is 0x788EF0 inlined)
void CharacterViewAvatarClass::Unknown_7896B0(int a)
{
	Unknown_788F90(a);
	if (Unknown370) {
		if (Unknown374) {
			Reset_All_Texture_Mappers(Unknown370.Peek(), false);
			Unknown370->Set_Animation(Unknown374.Peek(), 0.0f, RenderObjClass::ANIM_MODE_ONCE);
			Unknown370->Set_Hidden(0);
			Unknown370->Set_Animation_Hidden(0);
			return;
		}
	}
	Unknown_788EF0();










}

// 0x7897D0
// (the teardown written in both branches)
void CharacterViewAvatarClass::Unknown_7897D0()
{
	if (Unknown380 == 0) {
		return;
	}
	if (Unknown370 != NULL && Unknown374 != NULL) {
		Unknown37C -= (float)WW3D::Get_Frame_Time() * 0.001f;
		if (Unknown37C <= Unknown378) {
			Unknown378 = -100.0f;
			if (Unknown380 == 2 || Unknown380 == 4) {
				if (BodyObj) BodyObj->Set_Hidden(0);
				if (HeadObj) HeadObj->Set_Hidden(0);
			} else {
				if (BodyObj) BodyObj->Set_Hidden(1);
				if (HeadObj) HeadObj->Set_Hidden(1);
			}
		}
		if (Unknown37C <= 0.0f) {
			if (Unknown370) {
				Unknown370->Set_Animation();
				Unknown370->Remove();
				Unknown370 = NULL;
			}
			if (Unknown374) {
				Unknown374 = NULL;
			}
			Unknown380 = 0;
			Unknown378 = -100.0f;
			Unknown37C = 0.0f;
		}
	} else {
		if (Unknown370) {
			Unknown370->Set_Animation();
			Unknown370->Remove();
			Unknown370 = NULL;
		}
		if (Unknown374) {
			Unknown374 = NULL;
		}
		Unknown380 = 0;
		Unknown378 = -100.0f;
		Unknown37C = 0.0f;
	}
}
































































































// 0x7899F0: the personality's body animation by tag played as the gesture, from frame with flags (0x789C10 inlined;
// the UI's 0x53CD60 calls it; FAIL_IFs on the client's lines 3895 / 3898 / 3901: '!anim' tests the tag, not the result)
void CharacterViewAvatarClass::Unknown_7899F0(const char *anim, float frame, int flags)
{
	FAIL_IF(!Get_NLP()) return;
	Unknown_78F700Class *personality = Get_NLP()->Personality;

	FAIL_IF(!personality) return;
	Unknown_78F700Class::BAnimStruct *body_anim = personality->Find_BAnim(anim);

	FAIL_IF(!anim) return;
	std::string name = Unknown_781D10(body_anim->AnimNumber);
	Unknown_7831C0(name.c_str());
	Unknown_789C10(frame, flags);
}







// 0x789C10: the gesture's frame to stop at (below 0: its frame now, flags | 1)
// (one store of flags after the if: the client's write-back of flags | 1 into the parameter)
void CharacterViewAvatarClass::Unknown_789C10(float frame, int flags)
{
	int i;
	if (Unknown0C0[1]) i = 1;
	else if (Unknown0C0[0]) i = 0;
	else { G_ASSERT(false);
		return;
	}
	if (frame < 0.0f) {
		Unknown0D4[i] = Unknown0CC[i];
		flags |= 1;
	} else {
		Unknown0D4[i] = frame;
	}
	Unknown0DC[i] = flags;
}

// 0x789CD0
void CharacterViewAvatarClass::Unknown_789CD0(int a)
{
	Unknown0C0[a] = NULL;
	Unknown0C8[a] = false;
	Unknown0CC[a] = 0.0f;
	Unknown0D4[a] = 0.0f;
	Unknown0DC[a] = -1;
}

// 0x789D40
bool CharacterViewAvatarClass::Unknown_789D40()
{
	if (Unknown0C0[1]) {
		return Unknown0DC[1] != -1;
	}
	return Unknown0DC[0] != -1;
}

// 0x789D80: the accessory's meshes '<part>0'..'<part>4' copied into the head prototype as LOD models,
// each into the LOD that holds it in the accessory
// (the sub-object and its clone RefCountPtr locals, Create_Get 0x474080)
void CharacterViewAvatarClass::Unknown_789D80(RefCountPtr<RenderObjClass> *slot, const char *part)
{
	if (HeadSource == NULL || *slot == NULL || part == NULL) {
		return;
	}
	char name[64];
	for (int i = 0; i < 5; i++) {
		_snprintf(name, 0x40, "%s%d", part, i);
		RefCountPtr<RenderObjClass> sub = Create_Get((*slot)->Get_Sub_Object_By_Name(name));
		if (sub && sub->Class_ID() == RenderObjClass::CLASSID_MESH) {
			HLodClass *head = (HLodClass *)HeadSource.Peek();
			HLodClass *source = (HLodClass *)slot->Peek();
			RefCountPtr<RenderObjClass> copy = Create_Get(sub->Clone());
			Unknown_496AE0(copy.Peek(), true);
			copy->Set_Container(NULL);
			for (int lod = 0; lod < source->Get_Lod_Count(); lod++) {
				for (int j = 0; j < source->Get_Lod_Model_Count(lod); j++) {
					if (sub.Peek() == source->Peek_Lod_Model(lod, j)) {
						head->Add_Lod_Model(lod, copy.Peek(), 0);
						Unknown_496AE0(HeadSource.Peek(), true);
						lod = 100;
						break;
					}
				}
			}
		}
	}
}

// 0x789FE0
void CharacterViewAvatarClass::Unknown_789FE0(const char *part)
{
	if (HeadSource && part) {
		char name[64];
		for (int i = 0; i < 5; i++) {
			_snprintf(name, 0x40, "%s%d", part, i);
			RenderObjClass *robj = HeadSource->Get_Sub_Object_By_Name(name);
			if (robj) {
				HeadSource->Remove_Sub_Object(robj);
				robj->Release_Ref();
			}
		}
	}
}

// Layout (CS12, 2026-11-05): the functions stand in the client's address order, and blank lines put each assert and
// __LINE__ user on the client's line (D25): 0x77E830 725, 0x77E900 748, 0x7812A0 1415, 0x784C90 2728 ...
