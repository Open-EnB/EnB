// Earth & Beyond client (reconstruction): app\client\co.cpp. Notes at the end of the file.
#include "co.h"
#include "client.h"
#include "..\shared\auxdata\auxdata.h"
#include "aux_roots.h"
#include "co_data.h"
#include "rendobj.h"
#include "sphere.h"
#include "vector3.h"
#include "ini.h"
#include <stdio.h>
#include <string.h>
#include "..\..\wwaudio\AudibleSound.h"
#include "control_keys_manager.h"
#include "..\packets\pk_player_object_relationship.h"
#include "..\shared\unknown_debug.h"

// The event 0x6FEEE0 emits (the client's RTTI .?AVPlayerObjectRelationshipChangeEvent@@, 0x14 bytes)
class PlayerObjectRelationshipChangeEvent	// NAME 5: the client's RTTI name
{
public:
	PlayerObjectRelationshipChangeEvent(ClientObjectClass *object, const RelationshipClass &old_relationship, const RelationshipClass &new_relationship) :
		Object(object), Old(old_relationship), New(new_relationship) {}

	ClientObjectClass *Object;		// +0x00  // NAME 2: what it holds
	RelationshipClass Old;			// +0x04  // NAME 2: what it holds
	RelationshipClass New;			// +0x0C  // NAME 2: what it holds
};

// The SignalMap the relationship event goes to: a function-static instance (0xBE57C0), not rebuilt
SignalMap &Unknown_4F7EF0();

// The data next to the object's code (.data 0xB93ACC-0xB93AE7): the pool sizes the constructor hands
// MultiFixedPoolDecalSystemClass, then four factors the size functions multiply
int Unknown_B93ACC[3] = { 15, 10, 5 };	// 0xB93ACC
float Unknown_B93AD8 = 0.1f;			// 0xB93AD8
float Unknown_B93ADC = 0.0125f;			// 0xB93ADC
float Unknown_B93AE0 = 0.02f;			// 0xB93AE0
float Unknown_B93AE4 = 0.025f;			// 0xB93AE4
float Unknown_B93AE8 = 1.0f;			// 0xB93AE8
float Unknown_B93AEC = 1.5f;			// 0xB93AEC

// 0x6F3BA0
void ClientObjectClass::Unknown_34(float value)
{
	Radius = value;
}

// 0x6F3BC0
RenderObjClass *ClientObjectClass::Unknown_6F3BC0()
{
	if (Model == 0) {
		return(Model);
	}
	Model->Add_Ref();
	return(Model);
}
static inline void Attach_To_Bone(RenderObjClass *model, RenderObjClass *object, const char *bone_name)
{
	int bone = model->Get_Bone_Index(bone_name);
	if (bone) {
		model->Add_Sub_Object_To_Bone(object, bone);
	}
}

// 0x6F3810
void ClientObjectClass::Unknown_6F3810(RenderObjClass *model)
{
	Attach_To_Bone(model, Unknown158[0].Peek(), "~FL");
	Attach_To_Bone(model, Unknown158[1].Peek(), "~FR");
	Attach_To_Bone(model, Unknown158[2].Peek(), "~BL");
	Attach_To_Bone(model, Unknown158[3].Peek(), "~BR");
	Attach_To_Bone(model, Unknown158[4].Peek(), "~FL");
	Attach_To_Bone(model, Unknown158[5].Peek(), "~FR");
	int count = model->Get_Num_Sub_Objects();
	for (int i = 0; i < count; i++) {
		RefCountPtr<RenderObjClass> sub = Create_Get(model->Get_Sub_Object(i));
		Unknown_6F3810(sub.Peek());
	}
}

// 0x6F2820
ClientObjectClass::ClientObjectClass(ClientClass *client, int type, unsigned id) :
	UnknownB02C90Class(this),
	MultiFixedPoolDecalSystemClass(3, (const uint32 *)Unknown_B93ACC),
	Client(client),
	Unknown8C(0),
	GameID(id),
	Type(type),
	Unknown98(0),
	Radius(1.0f),
	RadiusAdjust(0),
	Scale(1.0f),
	UnknownA8(-1),
	Model(0),
	UnknownB0(false),
	UnknownB8(0),
	Sphere(Vector3(0.0f, 0.0f, 0.0f), 0.0f),
	UnknownF8(0),
	UnknownFC(false),
	Unknown100(-1),
	Unknown118(0),
	Unknown11C(0),
	Unknown120(0),
	Unknown124(0),
	Unknown128(false),
	Unknown144(0),
	Unknown148(-1),
	Unknown14C(0),
	Unknown150(0),
	Unknown154(0),
	Unknown170(false),
	Unknown174(0),
	Unknown178(0.0f, 0.0f, 0.0f),
	Unknown184(true)
{
	AuxData = (char *)((UnknownCOClass *)this)->Unknown_Create_Aux_Root(type, id);
	Unknown98 = new UnknownB006B8Class;
	Node.Set(this);
	UnknownB4 = Unknown_731FA0(this);
	Unknown134.Unknown_498F00(0, 0, Unknown_6F4590());
	int i;
	for (i = 0; i < 5; i++) {
		Unknown104[i] = 1;
	}

	for (i = 0; i < 6; i++) {
		Unknown158[i] = NULL;
	}
}

// 0x6F2FE0
ClientObjectClass::~ClientObjectClass()
{
	ClientObjectDataClass::Remove_All(this);
	if (Unknown118) {
		Unknown118->Stop(true);
		Unknown118->Remove_From_Scene();
		Unknown118->Release_Ref();
		Unknown118 = NULL;
	}
	if (Unknown11C) {
		Unknown11C->Stop(true);
		Unknown11C->Remove_From_Scene();
		Unknown11C->Release_Ref();
		Unknown11C = NULL;
	}
	if (Unknown120) {
		Unknown120->Stop(true);
		Unknown120->Remove_From_Scene();
		Unknown120->Release_Ref();
		Unknown120 = NULL;
	}
	if (Unknown124) {
		Unknown124->Stop(true);
		Unknown124->Remove_From_Scene();
		Unknown124->Release_Ref();
		Unknown124 = NULL;
	}
	if (Unknown8C) {
		delete Unknown8C;
		Unknown8C = NULL;
	}
	if (UnknownB4) {
		UnknownB4->Unknown_732690();
		UnknownB4 = NULL;
	}
	if (Model) {
		Model->Remove();
		Model->Release_Ref();
		Model = NULL;
	}
	if (Unknown98) {
		delete Unknown98;
		Unknown98 = NULL;
	}
	if (AuxData) {
		delete (UnknownRootClass *)AuxData;
		AuxData = NULL;
	}
	GameID = (unsigned)-1;
	for (int i = 0; i < 6; i++) {
		Unknown158[i] = NULL;
	}
}

























































#include <list>
#include "scene.h"

// The render hook Set_Model 0x6F33F0 gives every model (vtable 0xB007A4, 0x10 bytes, over RenderHookClass's
// 0xB007B4): a list of (material pass, flag) pairs its Pre_Render 0x6BB7E0 pushes around the model's render.
// Its two render functions lie after debris.cpp (file unknown, not rebuilt); the constructor is inline here
struct UnknownB007A4PassStruct	// NAME 1: neutral (D10)
{
	MaterialPassClass *Pass;	// +0x00 (0x6BB7E0 pushes it)  // NAME 2: what it holds
	bool Unknown04;				// +0x04 (0x6BB7E0: an override flag 4 when set)
};
class UnknownB007A4Class : public RenderHookClass	// NAME 1: neutral (D10), named by its vtable
{
public:
	virtual bool Pre_Render(RenderObjClass *robj, RenderInfoClass &rinfo);	// 0x6BB7E0
	virtual void Post_Render(RenderObjClass *robj, RenderInfoClass &rinfo);	// 0x6BB8B0

	std::list<UnknownB007A4PassStruct> Passes;	// +0x04  // NAME 2: what it holds
};

// 0x6F33F0
void ClientObjectClass::Unknown_6F33F0(RenderObjClass *model, int base_asset, float scale)
{
	SceneClass *scene = NULL;
	if (Model) {
		scene = Model->Get_Scene();
		Model->Remove();
		Model->Release_Ref();
		Model = NULL;
		if (Unknown118) {
			Unknown118->Attach_To_Object(NULL, -1);
		}
		if (Unknown11C) {
			Unknown11C->Attach_To_Object(NULL, -1);
		}
		if (Unknown120) {
			Unknown120->Attach_To_Object(NULL, -1);
		}
		if (Unknown124) {
			Unknown124->Attach_To_Object(NULL, -1);
		}
	}
	Model = model;
	UnknownA8 = base_asset;
	Scale = scale;
	if (UnknownB4) {
		UnknownB4->Unknown_20(base_asset, Create_Peek(Model));
	}
	if (Model) {
		Model->Add_Ref();
		if (scene) {
			Model->Add(scene);
		}
		Model->Set_Render_Hook(new UnknownB007A4Class);
		Unknown_38();
		if (Unknown118) {
			Unknown118->Attach_To_Object(Model, -1);
		}
		if (Unknown11C) {
			Unknown11C->Attach_To_Object(Model, -1);
		}
		if (Unknown120) {
			Unknown120->Attach_To_Object(Model, -1);
		}
		if (Unknown124) {
			Unknown124->Attach_To_Object(Model, -1);
		}
	} else {
		Unknown_34(0);
	}
	if (scene) {
		scene->Release_Ref();
	}
	Unknown_6FCB70();
}

// 0x6F39F0
void ClientObjectClass::Unknown_38()
{
	if (Model) {
		static INIClass ini;
		if (!ini.Is_Loaded()) {
			ini.Load("cbasset.ini");
		}
		char section[64];
		_snprintf(section, sizeof(section), "BASE-%d", UnknownA8);
		float radius = ini.Get_Float(section, "Radius");
		RadiusAdjust = ini.Get_Float(section, "RadiusAdjust");
		FAIL_IF(Scale <= 0.0f) {
			Scale = 1.0f;
		}
		radius = (radius + RadiusAdjust) * Scale;

		FAIL_IF(radius < 0.0f) {
			radius = 0.0f;
		}
		Unknown_34(radius);
	}
}
















// 0x6F3BF0
void ClientObjectClass::Unknown_6F3BF0(bool hidden)
{
	G_ASSERT(Model != 0);
	if (Model) {
		Model->Set_Hidden(hidden);
	}
}

// 0x6F3ED0
float ClientObjectClass::Unknown_6F3ED0()
{
	Vector3 v = Unknown98->Unknown_14(Client->Unknown10BC);
	float x = v.Z * -500.0f;
	float k;
	if (Unknown_6F5A40()) {
		k = Unknown_B93AE8;
	} else {
		if (Unknown_6F4290() < 0.174532935f) {
			return(0.0f);
		}
		k = Unknown_B93AEC;
	}
	return(WWMath::Clamp(k * x, -0.628318548f, 0.628318548f));
}

// 0x6F45E0
float ClientObjectClass::Unknown_6F45E0()
{
	if (Unknown_6F5A40()) {
		return(Unknown_B93AE0);
	}
	return(Unknown_B93AE4);
}

// 0x6F4590
float ClientObjectClass::Unknown_6F4590()
{
	if (Unknown_6F5A40()) {
		return(Unknown_6F45E0() * Unknown_B93AD8);
	}
	return(Unknown_6F45E0() * Unknown_B93ADC);
}
static inline void Clamp_In_Place(float &value, float min, float max)
{
	if (value < min) {
		value = min;
	} else if (value > max) {
		value = max;
	}
}









































































































































































































































































































































// 0x6F4C20
Vector3 ClientObjectClass::Unknown_24()
{
	Vector3 pos = Unknown98->Unknown_0C(Client->Unknown10BC);
	if (Type == 1) {
		if (Unknown98->Unknown_04() != 12 && Unknown98->Unknown_04() != 13 && Unknown98->Unknown_04() != 14) {
			Clamp_In_Place(pos.Z, Client->Unknown10D4, Client->Unknown10D8);
		}
		if (Unknown98->Unknown_04() != 12 && Unknown98->Unknown_04() != 13 && Unknown98->Unknown_04() != 14) {
			Clamp_In_Place(pos.X, Client->Unknown10DC, Client->Unknown10E4);
			Clamp_In_Place(pos.Y, Client->Unknown10E0, Client->Unknown10E8);
		}
	}











	G_ASSERT(pos.Is_Valid());
	return(pos);
}






































































#include "sclient.h"
#define RSTRACK_CLASSID	0x10001	// NAME 2: the class id the handlers test
#include "assetmgr.h"
#include "ww3d.h"
#include "nullrobj.h"
// The model loader's helpers, declared as far as 0x6F51D0 calls them (none rebuilt):
// E&B's asset manager (the object at *0xBDADC0, a WW3DAssetManager): its first own slot makes a model at a scale and HSV shift
class UnknownBDADC0Class : public WW3DAssetManager	// NAME 1: neutral (D10)
{
public:
	virtual RenderObjClass *Unknown_78(const char *name, float scale, float *hsv);	// +0x78  // NAME 1 [name, scale, hsv]: what 0x6F51D0 passes
};
extern UnknownBDADC0Class *Unknown_BDADC0;	// NAME 1 [Unknown_BDADC0]: 0xBDADC0
// rs_track.cpp's render object (0x100 bytes; ACTIVATE_RENDER_STATE 48 and INIT_RENDER_STATE 47 call the same two)
class RsTrackClass	// NAME 3: class-from-file (D24) for app\shared\rs_track.cpp
{
public:
	RsTrackClass(void *list, float scale, float *hsv);	// 0x45FFB0 (list: ClientClass +0x18's 0x764030)  // NAME 1 [list, scale, hsv]: what 0x6F51D0 passes
	void Unknown_4604D0(int render_state_id);			// 0x4604D0
	void Unknown_460880(int render_state_id, int link_id);	// 0x460880
	char Unknown00[0x100];
};
// a holder of a segmented line (0x4981E0 makes one when asked; 0x4983A0 reads it from an INI section)
class Unknown4981E0Class	// NAME 1: neutral (D10)
{
public:
	Unknown4981E0Class(bool create);			// 0x4981E0  // NAME 1 [create]: neutral
	~Unknown4981E0Class();						// 0x498320
	void Unknown_4983A0(INIClass *ini, const char *section, float scale, float *hsv);	// 0x4983A0  // NAME 1 [ini .. hsv]: what 0x6F51D0 passes
	bool operator!() const { return(Line == NULL); }
	RenderObjClass *Line;						// +0x00  // NAME 2: what it holds
};
void Unknown_4BC260(const char *sender, const char *category, const char *file, int line, const char *format, ...);	// 0x4BC260 (unknown_debug.h's reporter)
static RenderObjClass *Unknown_6F51D0(int base_asset, ClientClass *client, float scale, float *hsv);

// 0x6F5140
RenderObjClass *ClientObjectClass::Unknown_6F5140(int base_asset, ClientClass *client, float scale, float *hsv)
{
	SClient *sclient = client ? (SClient *)client->Unknown_18() : NULL;
	WW3DAssetManager::Get_Instance()->Set_Activate_Fog_On_Load(true);
	if (sclient && sclient->Unknown1320) {
		WW3D::Enable_Overbright_Modify_On_Load(true);
	}
	RenderObjClass *robj = Unknown_6F51D0(base_asset, client, scale, hsv);
	WW3D::Enable_Overbright_Modify_On_Load(false);
	WW3DAssetManager::Get_Instance()->Set_Activate_Fog_On_Load(true);
	return(robj);
}
static inline INIClass &Unknown_Basset_INI() { static INIClass ini; if (!ini.Is_Loaded()) { ini.Load("basset.ini"); } return(ini); }	// (the function's guarded static, as 0x6F51D0 reaches it)  // NAME 1 [Unknown_Basset_INI]: what it holds
// 0x6F51D0
static RenderObjClass *Unknown_6F51D0(int base_asset, ClientClass *client, float scale, float *hsv)
{
	INIClass &ini = Unknown_Basset_INI();
	
	
	
	char section[256];
	sprintf(section, "BASE-%d", base_asset);
	if (base_asset == -1) {
		return(new Null3DObjClass("NULL"));
	}
	if (!ini.Find_Section(section)) {
		return(Unknown_BDADC0->Create_Render_Obj("missing"));
	}
	if (ini.Get_Bool(section, "OverbrightOff", false)) {
		WW3D::Enable_Overbright_Modify_On_Load(false);
	}
	if (ini.Is_Present(section, "NULLOBJECT")) {
		return(new Null3DObjClass("NULL"));
	}
	if (ini.Is_Present(section, "RSLID")) {
		int rslid = ini.Get_Int(section, "RSLID", -1);
		if (rslid == -1 || client == NULL) {
			return(Unknown_BDADC0->Create_Render_Obj("missing"));
		}
		void *list = client->Unknown18->Unknown_764030(rslid);	RsTrackClass *track = new RsTrackClass(list, scale, hsv);
		int link_id = ini.Get_Int(section, "LINKID", -1);
		int rs_id = ini.Get_Int(section, "RSID", -1);
		if (link_id == -1) {
			track->Unknown_4604D0(rs_id);
			return((RenderObjClass *)track);
		}
		track->Unknown_460880(link_id, rs_id);
		track->Unknown_4604D0(link_id);
		return((RenderObjClass *)track);
	}
	if (ini.Is_Present(section, "FILE")) {
		char name[256];
		char file[256];
		ini.Get_String(section, "FILE", "", file, 255);
		ini.Get_String(section, "NAME", "", name, 255);
		RenderObjClass *robj = Unknown_BDADC0->Unknown_78(name, scale, hsv);
		if (robj) {
			return(robj);
		}
		Unknown_BDADC0->Load_3D_Assets(file);
		robj = Unknown_BDADC0->Unknown_78(name, scale, hsv);
		if (robj) {
			return(robj);
		}
		Unknown_4BC260("Base Asset", "", __FILE__, __LINE__, "Could not create render object %s from file %s", name, file);
		return(Unknown_BDADC0->Create_Render_Obj("missing"));





	}
	if (ini.Is_Present(section, "Width")) {
		Unknown4981E0Class segline(true);
		segline.Unknown_4983A0(&ini, section, scale, hsv);
		RenderObjClass *line = segline.Line;
		FAIL_IF(!segline) return(NULL);
		line->Add_Ref();
		return(line);
	}
	return(NULL);
}

// 0x6F5050
Vector3 ClientObjectClass::Unknown_2C()
{
	return(Unknown98->Unknown_10(Client->Unknown10BC));
}

// 0x6F5120
void ClientObjectClass::Unknown_1C()
{
	if (AuxData) {
		((DeltaClass *)AuxData)->Unknown_20();
	}
}

































// 0x6F5850
bool ClientObjectClass::Unknown_6F5850()
{
	if (Type >= 0 && (Type <= 1 || Type == 25)) {
		return(true);
	}
	return(false);
}

// 0x6F5980
bool ClientObjectClass::Unknown_6F5980()
{
	switch (Type) {
		case 0:
		case 1:
		case 2:
		case 3:
		case 4:
		case 11:
		case 12:
		case 24:
		case 25:
		case 34:
		case 35:
		case 38:
			return(true);
		case 37:
			if (AuxData) {
				return(*(bool *)(AuxData + 0x1AC));
			}
			break;
	}
	return(false);
}

// 0x6F5A10
bool ClientObjectClass::Unknown_6F5A10()
{
	if (Type >= 0 && Type <= 1) {
		return(true);
	}
	return(false);
}

// 0x6F5A40
bool ClientObjectClass::Unknown_6F5A40()
{
	switch (Type) {
		case 1:
			return(true);
	}
	return(false);
}

// 0x6F5A60
bool ClientObjectClass::Unknown_6F5A60() { if (Type == 3) return(true); return(false); }

// 0x6F5A80
bool ClientObjectClass::Is_Component()
{
	if (Type == 4 || Type == 35) {
		return(true);
	}
	return(false);
}

// 0x6F5AB0
bool ClientObjectClass::Unknown_6F5AB0() { if (Type == 5) return(true); return(false); }
// 0x6F5AD0
bool ClientObjectClass::Unknown_6F5AD0() { if (Type == 6) return(true); return(false); }
// 0x6F5AF0
bool ClientObjectClass::Unknown_6F5AF0() { if (Type == 7) return(true); return(false); }
// 0x6F5B10
bool ClientObjectClass::Unknown_6F5B10() { if (Type == 8) return(true); return(false); }
// 0x6F5B30
bool ClientObjectClass::Unknown_6F5B30() { if (Type == 9) return(true); return(false); }
// 0x6F5B50
bool ClientObjectClass::Unknown_6F5B50() { if (Type == 10) return(true); return(false); }
// 0x6F5B70
bool ClientObjectClass::Unknown_6F5B70() { if (Type == 11) return(true); return(false); }
// 0x6F5B90
bool ClientObjectClass::Unknown_6F5B90() { if (Type == 12) return(true); return(false); }
// 0x6F5BB0
bool ClientObjectClass::Unknown_6F5BB0() { if (Type == 13) return(true); return(false); }
// 0x6F5BD0
bool ClientObjectClass::Unknown_6F5BD0() { if (Type == 14) return(true); return(false); }

// 0x6F5BF0
bool ClientObjectClass::Unknown_6F5BF0()
{
	if (Type >= 15 && Type <= 24) {
		return(true);
	}
	return(false);
}

// 0x6F5C20
bool ClientObjectClass::Unknown_6F5C20()
{
	if (Type >= 16 && Type <= 18) {
		return(true);
	}
	return(false);
}

// 0x6F5C50
bool ClientObjectClass::Unknown_6F5C50()
{
	if (Type >= 17 && Type <= 18) {
		return(true);
	}
	return(false);
}

// 0x6F5C80
bool ClientObjectClass::Unknown_6F5C80() { if (Type == 18) return(true); return(false); }

// 0x6F5CA0
bool ClientObjectClass::Unknown_6F5CA0()
{
	if (Type >= 19 && Type <= 23) {
		return(true);
	}
	return(false);
}

// 0x6F5CD0
bool ClientObjectClass::Unknown_6F5CD0()
{
	if (Type >= 20 && Type <= 23) {
		return(true);
	}
	return(false);
}

// 0x6F5D00
bool ClientObjectClass::Unknown_6F5D00()
{
	if (Type >= 21 && Type <= 23) {
		return(true);
	}
	return(false);
}

// 0x6F5D30
bool ClientObjectClass::Unknown_6F5D30() { if (Type == 22) return(true); return(false); }
// 0x6F5D50
bool ClientObjectClass::Unknown_6F5D50() { if (Type == 23) return(true); return(false); }
// 0x6F5D70
bool ClientObjectClass::Unknown_6F5D70() { if (Type == 24) return(true); return(false); }

// 0x6F5D90
bool ClientObjectClass::Unknown_6F5D90()
{
	switch (Type) {
		case 28:
			return(true);
		case 29:
			return(true);
	}
	return(false);
}

// 0x6F5DC0
bool ClientObjectClass::Unknown_6F5DC0() { if (Type == 28) return(true); return(false); }
// 0x6F5DE0
int ClientObjectClass::Unknown_6F5DE0() { return(Type == 37); }
// 0x6F5E00
int ClientObjectClass::Unknown_6F5E00() { return(Type == 39); }
// 0x6F5E20
int ClientObjectClass::Unknown_6F5E20() { return(Type == 38); }


// 0x6F5E40
bool ClientObjectClass::Unknown_6F5E40()
{
	NameClass name("IsOrganic");
	DeltaTemplateClass<bool> *t = Unknown_Find_Field<DeltaTemplateClass<bool> >((DeltaCompoundClass *)AuxData, name);
	if (t && t->m_Unknown70) return(t->Value);
	return(false);
}

// 0x6F5ED0
bool ClientObjectClass::Unknown_6F5ED0()
{
	NameClass name("IsInPVP");
	DeltaTemplateClass<bool> *t = Unknown_Find_Field<DeltaTemplateClass<bool> >((DeltaCompoundClass *)AuxData, name);
	if (t && t->m_Unknown70) return(t->Value);
	return(false);
}

// 0x6F58B0
bool ClientObjectClass::Unknown_6F58B0()
{
	if (Unknown_6F5850()) {
		return(!Unknown_6F5E40());
	}
	return(false);
}

// 0x6F5F60
bool ClientObjectClass::Unknown_6F5F60()
{
	NameClass name("IsInPVP");
	DeltaTemplateClass<bool> *t = Unknown_Find_Field<DeltaTemplateClass<bool> >((DeltaCompoundClass *)AuxData, name);
	if (t && t->m_Unknown70 <= 1) t = NULL;
	return(t != NULL);
}
















































































































































































































































































































































































































































// 0x6FC880
void ClientObjectClass::Unknown_6FC880()
{
	if (Unknown118) {
		Unknown118->Stop(true);
		Unknown118->Remove_From_Scene();
	}
	if (Unknown11C) {
		Unknown11C->Stop(true);
		Unknown11C->Remove_From_Scene();
	}
	if (Unknown14C) {
		EffectObjectClass *effect = Client->Get_Effect(Unknown14C);
		if (effect) {
			effect->Unknown_6E6E30();
		}
		Unknown14C = 0;
	}
	if (Unknown_6F5A40() && Unknown184 != true) {
		Unknown_810540(Model, 2, 0);
		Unknown184 = true;
	}
}
// 0x6FC950
void ClientObjectClass::Unknown_6FC950(bool value)
{
	Unknown128 = value;
}

// 0x6FC970
bool ClientObjectClass::Unknown_6FC970()
{
	if (Type == 0 || Type == 1) {
		return(UnknownF8 == 2);
	}
	return(false);
}
static inline float Get_Float(char *p) { return(*(float *)p); }
static inline void Set_Floats(char *p, float a, float b) { ((float *)p)[0] = a; ((float *)p)[1] = b; }
// NOT EXACT: shape (case 2's 'pop esi' scheduled between its two stores in ours, after them in the client; 3 forms)
// 0x6FC9A0
void ClientObjectClass::Unknown_6FC9A0()
{
	char *aux = AuxData;
	char *model = (char *)Model;
	if (*(unsigned *)(aux + 0x198) > 1) {
		*(float *)(model + 0x98) = Get_Float(aux + 0x1AC);
	}
	if (*(unsigned *)(aux + 0x220) > 1) {
		*(Vector3 *)(model + 0xA8) = *(Vector3 *)(aux + 0x234);
	}
	if (*(unsigned *)(aux + 0x2B0) > 1) {
		*(Vector3 *)(model + 0xB4) = *(Vector3 *)(aux + 0x2C4);
	}
	if (*(unsigned *)(aux + 0x340) > 1) {
		*(Vector3 *)(model + 0x9C) = *(Vector3 *)(aux + 0x354);
	}
	if (*(unsigned *)(aux + 0x3D0) > 1 || *(unsigned *)(aux + 0x458) > 1) {
		switch (*(int *)(aux + 0x3E4)) {
			case 0:
				Set_Floats(model + 0xC8, 1000000.0f, 1000000.0f);
				return;
			case 1:
				Set_Floats(model + 0xC8, Get_Float(aux + 0x46C), Get_Float(aux + 0x470));
				break;
			case 2:
				Set_Floats(model + 0xC8, Get_Float(aux + 0x46C), Get_Float(aux + 0x46C));
				break;
			case 3:
				Set_Floats(model + 0xC8, Get_Float(aux + 0x46C), Get_Float(aux + 0x470));
				break;
		}
	}
}

// NOT EXACT: shape (the client keeps the argument in eax and masks a copy for the compare; 3 forms)
// 0x6FCB20
void ClientObjectClass::Unknown_6FCB20(bool visible)
{
	bool hidden = UnknownB0;
	if (visible != !hidden) {
		UnknownB0 = !hidden;
		if (UnknownB4) {
			UnknownB4->Unknown_18(visible);
		}
	}
}

















































// 0x6FD2C0
void ClientObjectClass::Unknown_6FD2C0()
{
	FAIL_IF(!Model) {return;}
	Unknown_6FD310();
	Unknown_6FDBD0();
	Unknown_6FE1E0();
}



























































































































































































































































































































































































































































// NOT EXACT: order (the client sums the transform's products in another order; explain.py says field-offset; 2 forms)
// 0x6FEC40
void ClientObjectClass::Unknown_6FEC40(SphereClass *sphere)
{

	FAIL_IF(!sphere) {return;}
	sphere->Init(Sphere.Center, Sphere.Radius);
	sphere->Transform(Unknown_28(0));
}
// 0x6FED30
void ClientObjectClass::Unknown_6FED30()
{
	if (Model == 0) {
		Sphere.Init(Vector3(0.0f, 0.0f, 0.0f), Radius);
	} else {
		Model->Get_Obj_Space_Bounding_Sphere(Sphere);
	}
}





// 0x6FED80
Matrix3D ClientObjectClass::Unknown_6FED80(int unknown)
{
	if (UnknownB4) {
		return(UnknownB4->Unknown_2C(unknown));
	}
	if (Model) {
		return(Model->Get_Transform());
	}
	G_ASSERT(Model);
	return(Matrix3D::Identity);
}
// 0x6FEEE0
void ClientObjectClass::Unknown_6FEEE0(const RelationshipClass &relationship)
{
	PlayerObjectRelationshipChangeEvent event(this, Relationship, relationship);
	Relationship = relationship;
	Unknown_4F7EF0().emit(this, event);
}
























































inline unsigned Unknown_Changed(DeltaClass *d) { return(d->m_Unknown70); }	// +0x70: 2 when the field changed since the last update, 1 when set, 0 when not  // NAME 1: neutral (D10)
// 0x6FF4B0  // NOT EXACT: inline-budget (the client calls rtti.h's cast 0x5469C0 at all four lookups and the string assign's helpers; ours inlines them)
void ClientObjectClass::Get_Name(std::string &name)
{
	DeltaStringClass *t;
	if (Unknown_6F5A40()) {
		NameClass owner("Owner");
		t = Unknown_Find_Field<DeltaStringClass>((DeltaCompoundClass *)AuxData, owner);
		if (!t || !t->m_Unknown70) {
			NameClass field("Name");
			t = Unknown_Find_Field<DeltaStringClass>((DeltaCompoundClass *)AuxData, field);
		}
	} else {
		NameClass field("Name");
		t = Unknown_Find_Field<DeltaStringClass>((DeltaCompoundClass *)AuxData, field);
		if (!t || !t->m_Unknown70) {
			NameClass owner("Owner");
			t = Unknown_Find_Field<DeltaStringClass>((DeltaCompoundClass *)AuxData, owner);
		}
	}
	name = (t && t->m_Unknown70) ? t->String : "Unknown";
}

// A field of the aux data changed since the last update (+0x70 above 1)
template<class C> inline bool Unknown_Field_Changed(C *, ClientObjectClass *object, const char *field)  // NAME 1: neutral (D10)
{
	NameClass name(field);
	C *t = Unknown_Find_Field<C>((DeltaCompoundClass *)object->AuxData, name);
	return(t && Unknown_Changed(t) > 1);
}

// 0x6FFC80 (the two dead type tests spend the inline budget: rtti.h's cast then stays out of line for Owner only, as the client's)
bool ClientObjectClass::Unknown_6FFC80(std::string &text)
{
	Unknown_6F5A40(); Unknown_6F5A40(); if (Unknown_Field_Changed((DeltaStringClass *)NULL, this, "Owner")
		|| Unknown_Field_Changed((DeltaStringClass *)NULL, this, "Name")
		|| Unknown_Field_Changed((DeltaStringClass *)NULL, this, "Rank")
		|| Unknown_Field_Changed((DeltaStringClass *)NULL, this, "Title")) {
		Unknown_6FF7A0(text);
		return(true);
	}
	return(false);
}

// 0x700400
bool ClientObjectClass::Unknown_700400()
{
	if (Unknown_Field_Changed((DeltaStringClass *)NULL, this, "GuildName")
		|| Unknown_Field_Changed((DeltaTemplateClass<UnknownB00A58Enum> *)NULL, this, "GuildRank")
		|| Unknown_Field_Changed((DeltaStringClass *)NULL, this, "GuildRankName")
		|| Unknown_Field_Changed((DeltaTemplateClass<UnknownB00A00Type> *)NULL, this, "SameGuildTagColor")
		|| Unknown_Field_Changed((DeltaTemplateClass<UnknownB00A00Type> *)NULL, this, "OtherGuildTagColor")) return(true);
	return(false);
}















// 0x6FF1D0
void ClientObjectClass::Unknown_6FF1D0()
{
	if (Type == 38 && Model && Model->Class_ID() == RSTRACK_CLASSID && AuxData) {
		NameClass name("PercentFull");
		DeltaTemplateClass<float> *t = Unknown_Find_Field<DeltaTemplateClass<float> >((DeltaCompoundClass *)AuxData, name);
		if (t && t->m_Unknown70 > 1) {
			float percent = t->Value;
			int state;
			if (percent < 0.0001f) state = 3;
			else if (percent < 0.3f) state = 2;
			else if (percent < 0.75f) state = 1;
			else state = 0;
			if (Unknown148 != state) {
				Unknown148 = state;
				((RsTrackClass *)Model)->Unknown_4604D0(state);
			}
		}
	}
}

// 0x6FF330
void ClientObjectClass::Unknown_6FF330(std::string &text)
{
	int level = 0;
	NameClass name("CombatLevel");
	DeltaTemplateClass<int> *t = Unknown_Find_Field<DeltaTemplateClass<int> >((DeltaCompoundClass *)AuxData, name);
	if (t && t->m_Unknown70) level = t->Value;
	char buf[64];
	_snprintf(buf, sizeof(buf), " (%d)", level);
	text = buf;
}

// 0x6FFE90
bool ClientObjectClass::Unknown_6FFE90()
{
	if (!Unknown_6F5A10()) return(false);
	char *aux_data = AuxData; FAIL_IF(!aux_data) return(false);
	return(*(int *)(aux_data + 0x393C) != -1);
}



// 0x6FFF00
int ClientObjectClass::Unknown_6FFF00()
{
	if (!Unknown_6F5A10()) return(0);
	char *aux_data = AuxData; FAIL_IF(!aux_data) return(0);
	return(*(int *)(aux_data + 0x38B4));
}



// 0x6FFF50
int ClientObjectClass::Unknown_6FFF50()
{
	if (!Unknown_6F5A10()) return(-1);
	char *aux_data = AuxData; FAIL_IF(!aux_data) return(-1);
	return(*(int *)(aux_data + 0x393C));
}



// 0x6FFFA0
int ClientObjectClass::Unknown_6FFFA0()
{
	if (!Unknown_6F5A10()) return(0);
	char *aux_data = AuxData; FAIL_IF(!aux_data) return(0);
	return(*(int *)(aux_data + 0x39C4));
}

// 0x6FFFF0
void ClientObjectClass::Unknown_6FFFF0(std::string &text)
{
	if (Unknown_6FFE90()) {
		const char *guild = (const char *)Unknown_6FFF00();
		const char *own = (const char *)Client->Unknown_728330();
		if (guild && own && strcmp(own, guild) == 0) {
			text = "[";
			text += guild;
			text += " ";
			text += (const char *)Unknown_6FFFA0();
			text += "]";
		} else {
			text = "[";
			text += guild;
			text += "]";
		}
	}
}






// 0x7008B0
bool ClientObjectClass::Unknown_7008B0()
{
	if (!Unknown_6F58B0()) return(false);
	if (!Model) return(false);
	Unknown_810750(Model, 0);
	UnknownFC = false;
	return(true);
}








// 0x700670
bool ClientObjectClass::Unknown_700670(Vector3 &color)
{
	if (!Unknown_6FFE90()) {
		return(false);
	}
	const char *guild = (const char *)Unknown_6FFF00();
	const char *own_guild = (const char *)Client->Unknown_728330();
	bool same = guild && own_guild && strcmp(guild, own_guild) == 0;



	char *aux_data = AuxData; FAIL_IF(!aux_data) return(false);
	if (same) {
		color = *(Vector3 *)(aux_data + 0x3A4C);
	} else {
		color = *(Vector3 *)(aux_data + 0x3ADC);
	}
	return(true);
}











































// 0x700820
void ClientObjectClass::Unknown_700820(const Vector3 &position)
{
	Unknown174 = 2.0f;
	Unknown178 = position;
}










// 0x700860
bool ClientObjectClass::Unknown_700860()
{
	if (!Unknown_6F5A10()) return(false);
	char *aux_data = AuxData; FAIL_IF(!aux_data) return(false);
	return(*(bool *)(aux_data + 0x11FC));
}

// 0x7009A0
void ClientObjectClass::Unknown_7009A0(bool on, bool force)
{
	if (force || on != Unknown184) {
		if (on) {
			Unknown_810540(Model, 2, 0);
			Unknown184 = true;
			return;
		}
		Unknown_810540(Model, 1, 0);
		Unknown184 = false;
	}
}

// 0x6FF7A0 the name label: the object's name, behind its Title or Rank for a type other than 0  // NOT EXACT: inline-budget (the client calls the string lookup 0x514BD0 at all six sites and inlines more of std::string; ours the reverse)
void ClientObjectClass::Unknown_6FF7A0(std::string &text)
{
	DeltaStringClass *t;
	if (!Unknown_6F5A40()) {
		NameClass field("Name");
		t = Unknown_Find_Field<DeltaStringClass>((DeltaCompoundClass *)AuxData, field);
		if (!t || !t->m_Unknown70) {
			NameClass owner("Owner");
			t = Unknown_Find_Field<DeltaStringClass>((DeltaCompoundClass *)AuxData, owner);
		}
	} else {
		NameClass owner("Owner");
		t = Unknown_Find_Field<DeltaStringClass>((DeltaCompoundClass *)AuxData, owner);
		if (!t || !t->m_Unknown70) {
			NameClass field("Name");
			t = Unknown_Find_Field<DeltaStringClass>((DeltaCompoundClass *)AuxData, field);
		}
	}
	const char *name = (t && t->m_Unknown70) ? t->String : "Unknown";
	if (Type == 0) {
		text = std::string(name);
	} else {
		NameClass title_field("Title");
		t = Unknown_Find_Field<DeltaStringClass>((DeltaCompoundClass *)AuxData, title_field);
		const char *title = (t && t->m_Unknown70) ? t->String : "";
		if (*title) {
			text = std::string(title) + " " + name;
		} else {
			NameClass rank_field("Rank");
			t = Unknown_Find_Field<DeltaStringClass>((DeltaCompoundClass *)AuxData, rank_field);
			const char *rank = (t && t->m_Unknown70) ? t->String : "";
			if (*rank) {
				text = std::string(rank) + " " + name;
			} else {
				text = name;
			}
		}
	}
}

/*
**	Reconstruction notes (T11, 2026-10-27)
**
**	co.cpp in the client: 0x6F2820-0x700A50 (__FILE__ e:\build\beta\G\app\Client\co.cpp). Not rebuilt
**	yet: the constructor 0x6F2820, the destructor 0x6F2FE0, the slots +0x20 0x6F3C60 and +0x28 0x6F4610,
**	the damage / engine / warp members (0x6FBEF0, 0x6FCB70, 0x6FD310-0x6FE1E0), 0x6FEF70 (l.2284) and
**	0x6F5F60. The aux-data roots' constructors and the CREATE factory 0x6F6000-0x6FBEF0 are in
**	aux_roots.cpp (generated). The asserts sit on the client's lines (D25): the blank stretches hold the
**	members not yet written; members without asserts (0x6FF4B0-0x700400, 0x6FF7A0) sit where room was.
**
**	The constructor (T17's reading of 0x6F2820, for whoever rebuilds it): ClientObjectClass is
**	'UnknownB02C90Class (client.h, +0x00, 0x4C bytes, constructed with the object itself), then
**	MultiFixedPoolDecalSystemClass (+0x4C, 0x14 bytes, ww3d2's decalsys.h; constructed (3, the pool sizes at
**	0xB93ACC); its vtable in the object 0xB006F4: 0x709E90, ww3d2's 0x981A40 / 0x981BB0 / 0x981CC0 and
**	Generate_Decal_Id 0x6F2C90)'. Then in order: the node at +0x60 (vtable 0xB00764, GenericNode; +0x6C the
**	node's address, +0x74 -1, +0x78 4), +0x7C the client, +0x8C 0, +0x90 the id (third argument), +0x94 the type
**	(second argument, a byte), +0x98 0, +0x9C 1.0f, +0xA0 0, +0xA4 1.0f, +0xA8 -1, +0xAC 0, +0xB0 false,
**	+0xB4 0, an object at +0xB8 (0x843910; byte +0xB8 = type), +0xBC/+0xC0/+0xC4 one 0x4E9CB0(0) result,
**	+0xC8 (0x843DB0; +0xCC 0, byte +0xC8 = type), +0xD0/+0xD4 0, byte +0xD8 = type, a std::string at +0xDC,
**	+0xE8-+0xF4 0 (the sphere), +0xF8 0, +0xFC false, +0x100 -1, +0x118-+0x124 0, +0x128 false, the
**	RelationshipClass at +0x12C (0x848C80), 0x498C10 at +0x134, +0x144 0, +0x148 -1, +0x14C-+0x154 0, six
**	RefCountPtr at +0x158 (vector constructor 0x5464F0 / 0x4740B0), +0x170 false, +0x174-+0x180 0,
**	+0x184 true; then the two vtables, AuxData = UnknownCOClass's Unknown_Create_Aux_Root(type, id),
**	+0x98 = new Unknown98Class (4 bytes, vtable 0xB006B8), +0x70 = this, +0xB4 = 0x731FA0(...) (a
**	RefCountPtr assignment), 0x498F00 at +0x134 with a radius from the floats 0xB93AD8-0xB93AE4 by type,
**	+0x104-+0x114 = 1 and the six RefCountPtrs cleared. The destructor 0x6F2FE0 has 7 EH states and
**	releases +0x118-+0x124 (slots +0xA0 / +0x70, a ref count at +0x14), +0x8C, +0xB4 (0x732690), +0xAC.
**
**	Forms: the aux-data accessors leave through 'if (!Unknown_6F5A10()) return(x);' (the inline type
**	test): a plain '(Type >= 0 && Type <= 1)' merges the early return with the FAIL_IF's tail. Tests of
**	type 1 go through the inline one-case switch 0x6F5A40 (movsx, dec, jne) where the client has that
**	form, and a plain 'Type == 1' where it compares the byte. Float copies the client makes through the
**	FPU need an inline getter or setter (Get_Float / Set_Floats, SphereClass::Init).
*/
