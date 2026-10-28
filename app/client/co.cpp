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
	virtual ~UnknownB007A4Class() {}										// 0x6F3770 (scalar deleting 0x6F3740)
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
static inline bool Clamp_In_Place(float &value, float min, float max)
{
	if (value < min) {
		value = min; return(true);
	} else if (value > max) {
		value = max; return(true);
	} return(false);
}

#include "_mono.h"
#include "ww3d.h"
#include "sclient.h"
#include "emote_data.h"

// camera_manager.cpp's class (sclient.cpp declares its other members): the per-frame update's calls
class CameraManagerClass
{
public:
	bool Unknown_79B670();			// 0x79B670 (the per-frame update tests it for the player's object)
	bool Unknown_79B690();			// 0x79B690 (the engine sounds 0x6FDBD0 test it for the player's object: thrust on)
	void Unknown_79B2B0(int mode);	// 0x79B2B0  // NAME 1 [mode]: CAMERA_CONTROL 146 passes 0x0C, 0x0D, 0, 0x0F, 6, 0x0A, 0x17, 0x10 or 0x19
};

// 0x6F3C60 (out of address order: here the includes above it move no assert line)
void ClientObjectClass::Unknown_20()
{
	EmoteDataClass *emote = Get_Emote_Data(this, false);
	if (emote) {
		if (GameID == Client->DebugObjectID && emote->Unknown_7E7A40()) {
			if (((SClient *)Client->Unknown_18())->CameraManager->Unknown_79B670()) {
				((SClient *)Client->Unknown_18())->CameraManager->Unknown_79B2B0(0x14);
				emote->Unknown_7E76E0();
			}
		}
		emote->Unknown_7E6860();
	}
	if (UnknownB4) {
		UnknownB4->Unknown_1C();
	}
	Matrix3D tm = Unknown_28(true);
	if (Client->DebugObjectID == GameID) {
		Mono.Set_Cursor(0, 1);
		Vector3 pos = tm.Get_Translation(); Mono.Printf("Player Pos(%6.1f,%6.1f,%3.1f) ", pos.X, pos.Y, pos.Z);
	}
	switch (Type) {
		case 8: Unknown_6FC9A0(); break;
		case 41: return;
	}

	if (Model) {
		if (!tm.Is_Orthogonal()) {
			tm.Re_Orthogonalize();
		}
		Model->Set_Transform(tm);
		Unknown_6FD2C0();
	}
	if (Type == 0 || Type == 1) {
		Unknown_6FBEF0();
	}
	if (Type == 38) {
		Unknown_6FF1D0();
	}
	if (Unknown174 > 0.0f) {
		Unknown174 -= (float)WW3D::Get_Frame_Time() * 0.001f;
		if (Unknown174 < 0.0f) {
			Unknown174 = 0.0f;
		}
	}
}
template<class C> inline C *Unknown_Find_Aux(C *, ClientObjectClass *object, const char *field) { NameClass name(field); return(Unknown_Find_Field<C>((DeltaCompoundClass *)object->AuxData, name)); }  // NAME 1: neutral (D10); the field under the aux data, the name built first and the aux data read after it (0x6F4610, 0x6FDBD0: one NameClass slot, auxdata.h's find out of line)
static inline bool Clamp_Changed(float &value, float min, float max)
{
	if (value < min) {
		value = min;
		return(true);
	}
	if (value > max) {
		value = max;
		return(true);
	}
	return(false);
}
// NOT EXACT: register (eax / edx swapped for the vtable loads from the thrust sound on), the one-case switch 0x6F5A40 at done (movsx / dec where the client compares the byte) and 'x || y' (the client ORs the bools); the find is out of line as the client's since Unknown_Find_Aux (IB1)
// 0x6F4610
Matrix3D ClientObjectClass::Unknown_28(bool thrust)
{
	Matrix3D tm = Unknown98->Unknown_08(Client->Unknown10BC);
	if (thrust) {
		Unknown_6F3FA0(tm);
		float turn = Unknown144;
		if (Unknown170) {
			bool on = false;

			DeltaTemplateClass<int> *t = Unknown_Find_Aux((DeltaTemplateClass<int> *)NULL, this, "EngineThrustState");
			if (t && t->m_Unknown70) {
				bool full = (t->Value == 2);
				on = (t->Value == 1 || Unknown_6FC970());
				if (full) {
					if (Unknown124 && !Unknown124->Is_In_Scene()) {
						Unknown124->Add_To_Scene(true);
					}
					Unknown158[0]->Set_Hidden(true);
					Unknown158[1]->Set_Hidden(true);
					Unknown158[2]->Set_Hidden(true);
					Unknown158[3]->Set_Hidden(true);
					Unknown158[4]->Set_Hidden(false);
					Unknown158[5]->Set_Hidden(false);
					goto done;
				}
			}
			if (turn < -0.15707964f) {
				if (Unknown124 && !Unknown124->Is_In_Scene() && !on) {
					Unknown124->Add_To_Scene(true);
				}
				Unknown158[0]->Set_Hidden(true);
				Unknown158[1]->Set_Hidden(false);
				Unknown158[2]->Set_Hidden(false);
				Unknown158[3]->Set_Hidden(true);
				Unknown158[4]->Set_Hidden(true);
				Unknown158[5]->Set_Hidden(true);
			} else if (turn > 0.15707964f) {
				if (Unknown124 && !Unknown124->Is_In_Scene() && !on) {
					Unknown124->Add_To_Scene(true);
				}
				Unknown158[0]->Set_Hidden(false);
				Unknown158[1]->Set_Hidden(true);
				Unknown158[2]->Set_Hidden(true);
				Unknown158[3]->Set_Hidden(false);
				Unknown158[4]->Set_Hidden(true);
				Unknown158[5]->Set_Hidden(true);
			} else {
				if (Unknown124 && Unknown124->Is_In_Scene()) {
					Unknown124->Stop(true);
					Unknown124->Remove_From_Scene();
				}
				Unknown158[0]->Set_Hidden(true);
				Unknown158[1]->Set_Hidden(true);
				Unknown158[2]->Set_Hidden(true);
				Unknown158[3]->Set_Hidden(true);
				Unknown158[4]->Set_Hidden(true);
				Unknown158[5]->Set_Hidden(true);
			}
		}
	}
done:
	if (Unknown_6F5A40()) {
		Vector3 pos = tm.Get_Translation();
		if (Unknown98->Unknown_04() != 12 && Unknown98->Unknown_04() != 13 && Unknown98->Unknown_04() != 14) {
			if (Clamp_Changed(pos.Z, Client->Unknown10D4, Client->Unknown10D8)) {
				tm.Set_Translation(pos);
			}
		}
		if (Unknown98->Unknown_04() != 12 && Unknown98->Unknown_04() != 13 && Unknown98->Unknown_04() != 14) {
			bool x = Clamp_Changed(pos.X, Client->Unknown10DC, Client->Unknown10E4);
			bool y = Clamp_Changed(pos.Y, Client->Unknown10E0, Client->Unknown10E8);
			if (x || y) {
				tm.Set_Translation(pos);
			}
		}
	}
	return(tm);
}
// NOT EXACT: inline budget (the client calls 0x6F3ED0, 0x6F5A40, 0x6F45E0 and auxdata.h's find 0x7079B0 out of line, ours inlines them; 9 forms)
// 0x6F3FA0 (slot +0x28's tilt: the turn +0x144 steered toward 0x6F3ED0's angle through +0x134, then the roll)
void ClientObjectClass::Unknown_6F3FA0(Matrix3D &tm)
{
	if (!Unknown_6F5A10()) {
		return;
	}
	if (Unknown_6F5E40()) {
		return;
	}
	double target = Unknown_6F3ED0();
	double current = Unknown144;
	double rate = Unknown134.Unknown04[2];
	double stop = (rate * rate) / (Unknown_6F4590() * 2.0f);
	double diff = fabs(target - current);
	double limit = Unknown_6F45E0();
	if (stop > limit) {
		limit = stop;
	}
	float accel = 0.0f;
	if (diff > limit) {
		if (target > current) {
			accel = Unknown_6F45E0();
		} else if (target < current) {
			accel = -Unknown_6F45E0();
		}
	}
	Unknown134.Unknown_498CC0(accel);
	if (Unknown134.Unknown00) {
		Unknown134.Unknown_498E00((float)WW3D::Get_Frame_Time());
	}
	Unknown144 = Unknown134.Unknown04[2] + Unknown144;
	tm.Rotate_X(Unknown144);
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

// 0x6F4E70 (unreferenced in the client: Unknown_24 clamps on its own) the height into the client's bounds unless the state is 12, 13 or 14
bool ClientObjectClass::Unknown_6F4E70(Vector3 &pos)
{
	if (Unknown98->Unknown_04() != 12 && Unknown98->Unknown_04() != 13 && Unknown98->Unknown_04() != 14) {
		return(Clamp_In_Place(pos.Z, Client->Unknown10D4, Client->Unknown10D8));
	}
	return(false);
}

// 0x6F4F30 (unreferenced in the client) X and Y into the client's bounds unless the state is 12, 13 or 14
bool ClientObjectClass::Unknown_6F4F30(Vector3 &pos)
{
	if (Unknown98->Unknown_04() != 12 && Unknown98->Unknown_04() != 13 && Unknown98->Unknown_04() != 14) {
		bool clamped = Clamp_In_Place(pos.X, Client->Unknown10DC, Client->Unknown10E4);
		clamped |= Clamp_In_Place(pos.Y, Client->Unknown10E0, Client->Unknown10E8);
		return(clamped);
	}
	return(false);
}


// 0x6F5090 the +0x8C object, made on first use (0x73FDB0 asks for it)
Unknown8CClass *ClientObjectClass::Unknown_6F5090()
{
	if (!Unknown8C) {
		Unknown8C = new Unknown8CClass;
	}
	return(Unknown8C);
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


#include "..\audio\soundlib.h"		// the warp state 0x6FBEF0, the engine sounds 0x6FDBD0
#include "..\..\wwaudio\Sound3D.h"
// eff_o.cpp's effects as far as the warp state calls them (as cpacket.cpp's OBJECT_EFFECT 9): the factory (cdecl),
// the effect's slot +0x10 (its target end) and the target's slot +0x28 (the object's id)
class Unknown6E9490TargetClass	// NAME 1: neutral (D10)
{
public:
	virtual void Unknown_00(); virtual void Unknown_04(); virtual void Unknown_08(); virtual void Unknown_0C();
	virtual void Unknown_10(); virtual void Unknown_14(); virtual void Unknown_18(); virtual void Unknown_1C();
	virtual void Unknown_20(); virtual void Unknown_24();
	virtual void Unknown_28(int game_id);	// NAME 1 [game_id]: what OBJECT_EFFECT 9 passes
};
class Unknown6E9490EffectClass : public EffectObjectClass	// NAME 1: neutral (D10)
{
public:
	virtual void Unknown_04(); virtual void Unknown_08(); virtual void Unknown_0C();
	virtual Unknown6E9490TargetClass *Unknown_10();
};
Unknown6E9490EffectClass *Unknown_6E9490(int desc_id, int effect_id, int duration, int time, ClientClass *client, float scale, const Vector3 &hsv_shift, float a, int b, int c);	// 0x6E9490  // NAME 1 [desc_id .. c]: what OBJECT_EFFECT 9 passes



















































































































































































































































// NOT EXACT: inline budget (the client calls the four finds' Casts 0x531590 / 0x7099F0 out of line, ours inlines them; 74 calls vs 70; 1 form, attempts.py show 0x6FBEF0)
// 0x6FBEF0 (the warp state: PrivateWarpState's messages, then GlobalWarpState's sounds, effects and camera modes)
void ClientObjectClass::Unknown_6FBEF0()
{
	bool player = (Unknown_6F5A40() && GameID == Client->DebugObjectID);
	{
		NameClass name("PrivateWarpState");
		DeltaTemplateClass<int> *t = Unknown_Find_Field<DeltaTemplateClass<int> >((DeltaCompoundClass *)AuxData, name);
		if (t && t->m_Unknown70 > 1) {
			UnknownF8 = t->Value;
			switch (UnknownF8) {
				case 5:
					((SClient *)Client->Unknown_18())->Unknown_74D990(0x11, "Engine unavailable. Please wait.", 0);
					SoundLibraryClass::Play_2D_Sound("On_Fail", 2);
					break;
				case 6:
					if (Type == 1) {
						SoundLibraryClass::Play_2D_Sound("On_Fail", 2);
						((SClient *)Client->Unknown_18())->Unknown_74D990(0x11, "Warp failed - local gravity interference.", 0);
					}
					break;
				case 7:
					((SClient *)Client->Unknown_18())->Unknown_74D990(0x11, "Warp target lost.", 0);
					break;
				case 8:
					((SClient *)Client->Unknown_18())->Unknown_74D990(0, "Warp target reached.", 0);
					break;
				case 9:
					((SClient *)Client->Unknown_18())->Unknown_74D990(0x11, "Warp followers disrupted.", 0);
					break;
				case 11:
					((SClient *)Client->Unknown_18())->Unknown_74D990(0x11, "Warp interrupted - out of energy.", 0);
					break;
			}
		}
	}
	NameClass name("GlobalWarpState");
	DeltaTemplateClass<int> *t = Unknown_Find_Field<DeltaTemplateClass<int> >((DeltaCompoundClass *)AuxData, name);
	if (!t || t->m_Unknown70 <= 1) {
		return;
	}
	if (Unknown128) {
		return;
	}
	UnknownF8 = t->Value;
	NameClass stats_name("CurrentStats");
	DeltaStatsClass *stats = Unknown_Find_Field<DeltaStatsClass>((DeltaCompoundClass *)AuxData, stats_name);
	if (stats && !stats->m_Unknown70) stats = NULL;
	int level = 1;
	if (stats) {
		level = stats->WarpPowerLevel.Value;
		if (level < 1) {
			level = 1;
		} else if (level > 6) {
			level = 6;
		}
	}
	char loop[128];
	char sound[128];
	Unknown6E9490EffectClass *effect;
	switch (UnknownF8) {
		case 1: {
			NameClass trail_name("EngineTrailType");
			DeltaTemplateClass<int> *trail = Unknown_Find_Field<DeltaTemplateClass<int> >((DeltaCompoundClass *)AuxData, trail_name);
			if (trail && !trail->m_Unknown70) trail = NULL;
			_snprintf(loop, sizeof(loop), "engine_loop_%d", trail ? trail->Value + 22 : 22);
			_snprintf(sound, sizeof(sound), "warp_start_%d", level);
			if (player) {
				SoundLibraryClass::Play_2D_Sound(sound, 2);
			} else {
				SoundLibraryClass::Play_3D_Sound(sound, Unknown_28(true), 2, 1.0f);
			}
			if (Unknown118) {
				Unknown118->Stop(true);
				Unknown118->Remove_From_Scene();
				Unknown118->Release_Ref();
			}
			if (Unknown11C) {
				Unknown11C->Stop(true);
				Unknown11C->Remove_From_Scene();
				Unknown11C->Release_Ref();
			}
			Unknown118 = SoundLibraryClass::Play_3D_Sound(loop, Unknown_28(true), 0, player ? 2.5f : 1.0f);
			Unknown11C = SoundLibraryClass::Play_3D_Sound("Warp_Secondary", Unknown_28(true), 0, 1.0f);
			if (Unknown118) {
				Unknown118->Attach_To_Object(Model, -1);
			}
			if (Unknown11C) {
				Unknown11C->Attach_To_Object(Model, -1);
			}
			if (player) {
				((SClient *)Client)->CameraManager->Unknown_79B2B0(2);
			}
			if (Unknown_6F5A40() && Unknown184) {
				Unknown_810540(Model, 1, 0);
				Unknown184 = false;
			}
			Unknown14C = --Client->Unknown113C;
			effect = Unknown_6E9490(0x1BB, Unknown14C, -1, Client->Unknown10BC, Client, 1.0f, Vector3(0.0f, 0.0f, 0.0f), 1.0f, -1, 0);
			if (!effect) {
				return;
			}
			Unknown6E9490TargetClass *target = effect->Unknown_10();
			if (target) {
				target->Unknown_28(GameID);
			} else {
				G_ASSERT(0);
			}
			MainViewClass *view = ((SClient *)Client->Unknown_18())->MainView;
			Client->Add_Effect(effect, (int)view->Unknown_64(), view->Unknown_7C());
			break;
		}






		case 2: {
			_snprintf(sound, sizeof(sound), "warp_enter_%d", level);
			if (player) {
				((SClient *)Client)->CameraManager->Unknown_79B2B0(3);
				SoundLibraryClass::Play_2D_Sound(sound, 2);
			} else {
				SoundLibraryClass::Play_3D_Sound(sound, Unknown_28(true), 2, 1.0f);
			}
			if (Unknown118) {
				Unknown118->Add_To_Scene(true);
			}
			if (Unknown11C) {
				Unknown11C->Add_To_Scene(true);
			}
			if (!Unknown14C) {
				return;
			}
			EffectObjectClass *old = Client->Get_Effect(Unknown14C);
			if (old) {
				old->Unknown_6E6E30();
			}
			Unknown14C = --Client->Unknown113C;
			effect = Unknown_6E9490(0x2C6, Unknown14C, -1, Client->Unknown10BC, Client, 1.0f, Vector3(0.0f, 0.0f, 0.0f), 1.0f, -1, 0);
			if (!effect) {
				return;
			}
			Unknown6E9490TargetClass *target = effect->Unknown_10();
			if (target) {
				target->Unknown_28(GameID);
			} else {
				G_ASSERT(0);
			}
			MainViewClass *view = ((SClient *)Client->Unknown_18())->MainView;
			Client->Add_Effect(effect, (int)view->Unknown_64(), view->Unknown_7C());
			break;
		}
		case 3: case 7: case 8: case 9: case 11:	// (the lines after l.1620 are short: 0x6FC880 follows)
			_snprintf(sound, sizeof(sound), "warp_exit_%d", level);
			if (player) {
				((SClient *)Client)->CameraManager->Unknown_79B2B0(4); SoundLibraryClass::Play_2D_Sound(sound, 2);
			} else {
				SoundLibraryClass::Play_3D_Sound(sound, Unknown_28(true), 2, 1.0f);
			}
			if (Unknown_6F5A40() && Unknown184 != true) {
				Unknown_810540(Model, 2, 0); Unknown184 = true;
			}
			Unknown_6FC880(); break;
	}
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






































// delta_array.h's item accessor (its G_ASSERT on that header's line 93, inline in the damage decals 0x6FD310), written
// here: declaring it in the header moves aux_roots.cpp's pairs (the 0x6FB1E0 canary, workflow.md 3.4; P-DF1-2)
template<class T> static inline DeltaTemplateClass<T> *Unknown_Get_Item(DeltaArrayClass<T> *array, unsigned index) { std::vector<DeltaClass *> &Items = array->Children; if (!(index < Items.size())) Unknown_Assert_Failed("index < Items.size()", "..\\shared/auxdata/delta_array.h", 93); return((DeltaTemplateClass<T> *)Items[index]); }	// NAME 1: neutral
// the decal pools of co.cpp's decal system (the constructor's MultiFixedPoolDecalSystemClass(3, ...))
#define NUM_DAMAGE_POOLS 3	// NAME 3: the FAIL_IF's name
class ProjDecalManagerClass	// NAME 3: the FAIL_IF's 'proj_decal_manager' (sclient.cpp declares its other members)
{
public:
	void Unknown_7633E0(DecalGeneratorClass *generator, const UnknownB01A40Type &decal, RenderObjClass *model);	// 0x7633E0 (the damage decals 0x6FD310: an aux item's value onto the model)  // NAME 1 [generator, decal, model]: what 0x6FD310 passes
};

// 0x6FD2C0
void ClientObjectClass::Unknown_6FD2C0()
{
	FAIL_IF(!Model) {return;}
	Unknown_6FD310();
	Unknown_6FDBD0();
	Unknown_6FE1E0();
}
// NOT EXACT: inline budget (the client calls the array Cast 0x709A40 out of line for DamageSpot and DamageLine and inlines it for DamageBlotch; ours inlines all three; 4 forms, attempts.py show 0x6FD310)
// 0x6FD310 (the damage decals: the DamageSpot / DamageLine / DamageBlotch items that changed, one decal pool each)
void ClientObjectClass::Unknown_6FD310()
{
	if (!Unknown_6F5850()) {
		return;
	}
	ProjDecalManagerClass *proj_decal_manager = ((SClient *)Client)->ProjDecalManager;




	FAIL_IF(!proj_decal_manager) {return;}
	bool spot_changed[15];
	bool line_changed[10];
	bool blotch_changed[5];
	DeltaArrayClass<UnknownB01A40Type> *arrays[NUM_DAMAGE_POOLS];
	{
		NameClass name("DamageSpot");
		DeltaArrayClass<UnknownB01A40Type> *t = Unknown_Find_Field<DeltaArrayClass<UnknownB01A40Type> >((DeltaCompoundClass *)AuxData, name);
		if (t && t->m_Unknown70 <= 1) t = NULL;
		arrays[0] = t;
		if (t) {
			for (int i = 0; i < 15; i++) spot_changed[i] = t->Children[i]->m_Unknown70 > 1;
		} else {
			memset(spot_changed, 0, sizeof(spot_changed));
		}
	}
	{
		NameClass name("DamageLine");
		DeltaArrayClass<UnknownB01A40Type> *t = Unknown_Find_Field<DeltaArrayClass<UnknownB01A40Type> >((DeltaCompoundClass *)AuxData, name);
		if (t && t->m_Unknown70 <= 1) t = NULL;
		arrays[1] = t;
		if (t) {
			for (int i = 0; i < 10; i++) line_changed[i] = t->Children[i]->m_Unknown70 > 1;
		} else {
			memset(line_changed, 0, sizeof(line_changed));
		}
	}
	{
		NameClass name("DamageBlotch");
		DeltaArrayClass<UnknownB01A40Type> *t = Unknown_Find_Field<DeltaArrayClass<UnknownB01A40Type> >((DeltaCompoundClass *)AuxData, name);
		if (t && t->m_Unknown70 <= 1) t = NULL;
		arrays[2] = t;
		if (t) {
			for (int i = 0; i < 5; i++) blotch_changed[i] = t->Children[i]->m_Unknown70 > 1;
		} else {
			memset(blotch_changed, 0, sizeof(blotch_changed));
		}
	}
	if (arrays[0] || arrays[1] || arrays[2]) {
		bool *changed[NUM_DAMAGE_POOLS] = { spot_changed, line_changed, blotch_changed };
		int num_pools = PoolCount;







		FAIL_IF(num_pools != NUM_DAMAGE_POOLS) {
			if (num_pools >= NUM_DAMAGE_POOLS) num_pools = NUM_DAMAGE_POOLS;
		}
		for (int pool = 0; pool < num_pools; pool++) {
			DeltaArrayClass<UnknownB01A40Type> *array = arrays[pool];
			bool *pool_changed = changed[pool];
			if (array) {
				int size = Pools[pool].Size;
				for (int slot = 0; slot < size; slot++) {
					if (pool_changed[slot]) {
						const UnknownB01A40Type &decal = Unknown_Get_Item(array, slot)->Value;
						if (decal.I0C < 0) {
							Clear_Decal_Slot(pool, slot);
						} else {
							Generator_PoolID = pool;
							Generator_SlotID = slot;
							DecalGeneratorClass *generator = Lock_Decal_Generator();
							proj_decal_manager->Unknown_7633E0(generator, decal, Model);
							Model->Create_Decal(generator);
							Unlock_Decal_Generator(generator);
						}
						pool_changed[slot] = false;
					}
				}
			} else {
				Clear_Pool(pool);
			}
		}
	}
}

// NOT EXACT: shape after the two Unknown_Find_Aux asserts (ours tests t again where the client jumps / returns); the calls out of line now match the client's (0x7079B0, 0x531590; IB1)
// 0x6FDBD0 (the engine sounds: the engine and thrust loops when EngineTrailType changed, then EngineThrustState)
void ClientObjectClass::Unknown_6FDBD0()
{
	Unknown_Find_Field<DeltaTemplateClass<int> >(NULL, *(NameClass *)this); Unknown_Find_Field<DeltaTemplateClass<int> >(NULL, *(NameClass *)this); Unknown_Find_Field<DeltaTemplateClass<int> >(NULL, *(NameClass *)this); Unknown_Find_Field<DeltaTemplateClass<int> >(NULL, *(NameClass *)this);	// dead finds (root NULL: no code) spend the inline budget: rtti.h's cast then stays out of line for EngineTrailType only, as the client's
	if (!Unknown_6F58B0()) {
		return;
	}
	{
		DeltaTemplateClass<int> *t = Unknown_Find_Aux((DeltaTemplateClass<int> *)NULL, this, "EngineTrailType");
		if (t && t->m_Unknown70 > 1) {
			if (Type == 1 && ((SClient *)Client->Unknown_18())->ViewMode == 1) {
				Unknown_6FD7A0(Model);
			}
			if (Unknown120) {
				Unknown120->Stop(true);
				Unknown120->Remove_From_Scene();
				Unknown120->Release_Ref();
				Unknown120 = NULL;
			}
			int trail = t->Value;
			char sound[128];
			if (Unknown_6F5A40()) {
				_snprintf(sound, sizeof(sound), "engine_loop_%d", trail + 1);
			} else {
				_snprintf(sound, sizeof(sound), "npc_engine_loop_%d", UnknownA8 % 6);
			}
			Unknown120 = SoundLibraryClass::Play_3D_Sound(sound, Unknown_28(false), 0, 1.0f);
			if (!Unknown120) {
				Unknown120 = SoundLibraryClass::Play_3D_Sound("engine_loop_1", Unknown_28(false), 0, 1.0f);
			}
			if (Unknown120) {
				Unknown120->Attach_To_Object(Model, -1);
			}
			if (Unknown124) {
				Unknown124->Stop(true);
				Unknown124->Remove_From_Scene();
				Unknown124->Release_Ref();
				Unknown124 = NULL;
			}
			if (Unknown_6F5A40()) {
				_snprintf(sound, sizeof(sound), "thrust_loop_%d", trail + 1);
			} else {
				_snprintf(sound, sizeof(sound), "npc_thrust_loop_%d", UnknownA8 % 6);
			}
			Unknown124 = SoundLibraryClass::Play_3D_Sound(sound, Unknown_28(false), 0, 1.0f);
			if (!Unknown124) {
				Unknown124 = SoundLibraryClass::Play_3D_Sound("imploop1.wav", Unknown_28(false), 0, 1.0f);
			}
			if (Unknown124) {
				Unknown124->Attach_To_Object(Model, -1);
			}
		}
	}

	DeltaTemplateClass<int> *t = Unknown_Find_Aux((DeltaTemplateClass<int> *)NULL, this, "EngineThrustState");
	if (!t || !t->m_Unknown70) {
		return;
	}
	int state = t->Value;
	bool emote = false;
	EmoteDataClass *data = Get_Emote_Data(this, false);
	if (data) {
		emote = data->Unknown_7E7A70();
	}
	bool camera = (GameID == Client->DebugObjectID && ((SClient *)Client->Unknown_18())->CameraManager->Unknown_79B690());
	bool forced = (emote || camera);
	bool on = (state == 1 || Unknown_6FC970() || forced);
	if (UnknownFC != on) {
		if (Unknown120) {
			if (on && UnknownF8 != 1) {
				if (!Unknown_6FC970()) {
					Unknown120->Add_To_Scene(true);
				}
				if (Unknown124) {
					Unknown124->Stop(true);
					Unknown124->Remove_From_Scene();
				}
			} else {
				Unknown120->Stop(true);
				Unknown120->Remove_From_Scene();
			}
		}
		Unknown_810750(Model, on);
		UnknownFC = on;
	}
	if (UnknownFC) {
		if (Unknown100 > 0) {
			Unknown100--;
			Unknown_810750(Model, false);
			return;
		}
		if (Unknown100 == 0) {
			Unknown100 = -1;
			Unknown_810750(Model, true);
		}
	}
}








































































































































// shiputils.cpp's damage-state calls, as far as the damage state 0x6FE1E0 makes them (cdecl, not rebuilt): the ship's
// four quadrant levels and its hull level (type 1), or the hull level with the view (the other types); both return +0x150
int Unknown_810CF0(RenderObjClass *model, int *levels, int hull_level, ClientClass *client, unsigned id, int unknown);	// 0x810CF0  // NAME 1 [model, levels, hull_level, client, id, unknown]: what 0x6FE1E0 passes
int Unknown_8117D0(RenderObjClass *model, int hull_level, int number, unsigned id, bool organic, bool view, ClientClass *client, int unknown);	// 0x8117D0  // NAME 1 [model, hull_level, number, id, organic, view, client, unknown]: what 0x6FE1E0 passes
// a damage fraction's level: 4 from one half, 3 from a quarter, 2 above 0, else 1 (inline in 0x6FE1E0)
static inline int Unknown_Damage_Level(float damage) { if (damage >= 0.5f) return(4); if (damage >= 0.25f) return(3); if (damage > 0.0f) return(2); return(1); }	// NAME 1 [damage]: what it is

// NOT EXACT: inline budget (the client calls IsOrganic's find 0x7079B0 out of line inside the inlined 0x6F5E40, ours inlines it: six dead finds do it, 2-5 don't; registers follow; attempts.py show 0x6FE1E0)
// 0x6FE1E0 (the damage state: the damage sounds of a type-1 object, then the shield or hull and the four quadrants' levels into the model)
void ClientObjectClass::Unknown_6FE1E0()
{	Unknown_Find_Field<DeltaTemplateClass<int> >(NULL, *(NameClass *)this); Unknown_Find_Field<DeltaTemplateClass<int> >(NULL, *(NameClass *)this); 	// dead finds (root NULL: no code) spend the inline budget (P-IB1-1): the float field's Cast 0x546970 then goes out of line as the client's
	if (!Unknown_6F5850()) {
		return;
	}
	if (Client && Unknown_6F5A40()) {
		unsigned now = Client->Unknown10BC;
		if ((unsigned)Unknown154 < now) {
			if (Unknown104[0] == 4 || Unknown104[1] == 4 || Unknown104[2] == 4 || Unknown104[3] == 4 || Unknown104[4] == 4) {
				SoundLibraryClass::Play_3D_Sound("Level_3_Ship_Damage", Unknown_28(true), 2, 1.0f);
				Unknown154 = now + 4000;
			} else if (Unknown104[0] == 3 || Unknown104[1] == 3 || Unknown104[2] == 3 || Unknown104[3] == 3 || Unknown104[4] == 3) {
				SoundLibraryClass::Play_3D_Sound("Level_2_Ship_Damage", Unknown_28(true), 2, 1.0f);
				Unknown154 = now + 5000;
			} else if (Unknown104[0] == 2 || Unknown104[1] == 2 || Unknown104[2] == 2 || Unknown104[3] == 2 || Unknown104[4] == 2) {
				SoundLibraryClass::Play_3D_Sound("Level_1_Ship_Damage", Unknown_28(true), 2, 1.0f);
				Unknown154 = now + 6000;
			}
		}
	}
	float hull = 0.0f;
	if (Unknown_6F5E40()) {

		DeltaInterpolatorClass *t = Unknown_Find_Aux((DeltaInterpolatorClass *)NULL, this, "ShieldPercent");
		float shield;
		if (t && t->m_Unknown70) shield = t->Get_Value(DurationClass((unsigned)Client->Unknown10BC));
		else shield = 1.0f;
		hull = WWMath::Clamp(1.0f - shield, 0.0f, 1.0f);
	} else {

		DeltaTemplateClass<float> *points = Unknown_Find_Aux((DeltaTemplateClass<float> *)NULL, this, "HullPoints");
		if (points && !points->m_Unknown70) points = NULL;

		DeltaTemplateClass<float> *max = Unknown_Find_Aux((DeltaTemplateClass<float> *)NULL, this, "MaxHullPoints");
		if (max && !max->m_Unknown70) max = NULL;
		if (points && max && max->Value != 0.0f) {
			hull = 1.0f - WWMath::Clamp(points->Value / max->Value, 0.0f, 1.0f);
		}
	}
	float quadrant[4];
	{
		NameClass name("VisibleQuadrantDamagePercent");
		DeltaArrayClass<float> *t = Unknown_Find_Field<DeltaArrayClass<float> >((DeltaCompoundClass *)AuxData, name);
		if (t && !t->m_Unknown70) t = NULL;
		if (t) {
			for (int i = 0; i < 4; i++) {
				quadrant[i] = 1.0f - WWMath::Clamp(Unknown_Get_Item(t, i)->Value, 0.0f, 1.0f);
			}
		} else {
			quadrant[0] = 0.0f;
			quadrant[1] = 0.0f;
			quadrant[2] = 0.0f;
			quadrant[3] = 0.0f;
		}
	}
	int level[4];
	for (int i = 0; i < 4; i++) {
		level[i] = Unknown_Damage_Level(quadrant[i]);
		if (level[i] == Unknown104[i]) level[i] = 0;
	}
	int hull_level = Unknown_Damage_Level(hull);
	if (hull_level == Unknown104[4]) hull_level = 0;
	if (level[0] == 0 && level[1] == 0 && level[2] == 0 && level[3] == 0 && hull_level == 0) {
		return;
	}
	if (Unknown_6F5A40()) {
		Unknown150 = Unknown_810CF0(Model, level, hull_level, Client, GameID, Unknown150);
	} else {
		bool view = false;
		switch (((SClient *)Client->Unknown_18())->ViewMode) {
			case 0:
				view = false;
				break;
			case 1:
			case 5:
				view = true;
				break;
			default:
				G_ASSERT(0);
				break;
		}
		bool is_organic = Unknown_6F5E40();
		Unknown150 = Unknown_8117D0(Model, hull_level, UnknownA8, GameID, is_organic, view, Client, Unknown150);
	}
	for (int j = 0; j < 4; j++) {
		if (level[j]) Unknown104[j] = level[j];
	}
	if (hull_level) Unknown104[4] = hull_level;
}

// NOT EXACT: inline budget (the client calls the bool field's Cast 0x584FF0 out of line in the inlined 0x6F58B0, ours inlines it; ebx holds the 1 in the client; 16 forms, attempts.py show 0x6FEAC0)
// 0x6FEAC0 (unreferenced in the client) the damage state back to level 1: the model's levels and +0x104
void ClientObjectClass::Unknown_6FEAC0()
{
	if ((Unknown_6F58B0() || Unknown_6F5850()) && Model) {
		int levels[4];
		for (int i = 0; i < 4; i++) levels[i] = 1;
		Unknown_810750(Model, 0);
		Unknown150 = Unknown_810CF0(Model, levels, 1, Client, GameID, Unknown150);
		UnknownFC = false;
		for (int j = 0; j < 4; j++) Unknown104[j] = 1; Unknown104[4] = 1;
	}
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
Matrix3D ClientObjectClass::Unknown_6FED80(const LegoIdentifierClass &lego)
{
	if (UnknownB4) {
		return(UnknownB4->Unknown_2C(lego));
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
#include "..\packets\pk_sub_parts.h"
#include "client_lego_impl.h"	// the sub parts' legos (SUBPARTS 0x6FEF70)
// 0x6FEF70 (cpacket.cpp's SUBPARTS 180): the +0xB4 object drops the old sub parts' legos (last first), the
// list takes the packet's names, and each gets its lego from 0x7294D0; then 0x6FCB70
RefCountPtr<LegoClass> Unknown_7294D0(const LegoIdentifierClass &id, int kind, int value, ClientClass *client, float scale);	// 0x7294D0 (cdecl, client.cpp's run; not rebuilt)  // NAME 1 [id, kind, value, client, scale]: what 0x6FEF70 passes
void ClientObjectClass::Unknown_6FEF70(SubPartsPacket *packet)
{	UnknownB8.size();	// a dead read (no code) spends the inline budget: the vector's _Destroy 0x700E60 then stays out of line as the client's (P-IB1-1)
	if (UnknownB4) {
		for (std::vector<LegoIdentifierClass>::reverse_iterator it = UnknownB8.rbegin(); it != UnknownB8.rend(); it++) {

			UnknownB4->Unknown_34(*it);
		}
	}
	UnknownB8.clear();
	ClientClass *client = Client;
	G_ASSERT(client);
	unsigned count = packet->Get_Count();
	for (unsigned i = 0; i < count; i++) {
		const SubPartStruct &part = packet->Get(i);
		LegoIdentifierClass id(part.Name);
		UnknownB8.push_back(id);
		if (UnknownB4 && client) {
			RefCountPtr<LegoClass> lego = Unknown_7294D0(id, 1, part.Value, client, 1.0f); UnknownB4->Unknown_30(lego);
		}
	}
	Unknown_6FCB70();
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
**	the damage / engine / warp members (0x6FCB70, 0x6FE1E0; the warp state 0x6FBEF0, the damage decals 0x6FD310 and the engine sounds 0x6FDBD0 written by DF1, NOT EXACT), 0x6FEF70 (l.2284) and
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
