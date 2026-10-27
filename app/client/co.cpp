// Earth & Beyond client (reconstruction): app\client\co.cpp. Notes at the end of the file.
#include "co.h"
#include "client.h"
#include "..\shared\auxdata\delta.h"
#include "rendobj.h"
#include "sphere.h"
#include "vector3.h"
#include "ini.h"
#include <stdio.h>
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
	Attach_To_Bone(model, Unknown158[0], "~FL");
	Attach_To_Bone(model, Unknown158[1], "~FR");
	Attach_To_Bone(model, Unknown158[2], "~BL");
	Attach_To_Bone(model, Unknown158[3], "~BR");
	Attach_To_Bone(model, Unknown158[4], "~FL");
	Attach_To_Bone(model, Unknown158[5], "~FR");
	int count = model->Get_Num_Sub_Objects();
	for (int i = 0; i < count; i++) {
		RefCountPtr<RenderObjClass> sub = Create_Get(model->Get_Sub_Object(i));
		Unknown_6F3810(sub.Peek());
	}
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
bool ClientObjectClass::Unknown_6F5A80()
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
	sphere->Init(((SphereClass *)Sphere)->Center, ((SphereClass *)Sphere)->Radius);
	sphere->Transform(Unknown_28(0));
}
// 0x6FED30
void ClientObjectClass::Unknown_6FED30()
{
	if (Model == 0) {
		((SphereClass *)Sphere)->Init(Vector3(0.0f, 0.0f, 0.0f), Radius);
	} else {
		Model->Get_Obj_Space_Bounding_Sphere(*(SphereClass *)Sphere);
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
	PlayerObjectRelationshipChangeEvent event(this, *(RelationshipClass *)Relationship, relationship);
	*(RelationshipClass *)Relationship = relationship;
	Unknown_4F7EF0().emit(this, event);
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










































































































// 0x700820
void ClientObjectClass::Unknown_700820(const Vector3 &position)
{
	Unknown174 = 2.0f;
	*(Vector3 *)Unknown178 = position;
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

/*
**	Reconstruction notes (T11, 2026-10-27)
**
**	co.cpp in the client: 0x6F2820-0x700A50 (__FILE__ e:\build\beta\G\app\Client\co.cpp). The
**	constructor 0x6F2820 (vtable 0xB0070C, base constructor 0x725050 in client.cpp's region, a second
**	vtable 0xB006F4 at +0x4C), the destructor 0x6F2FE0, the slots and the larger members are not
**	rebuilt yet; the aux-data roots' constructors and the CREATE factory 0x6F6000-0x6FBEF0 are in
**	aux_roots.cpp (generated). Rebuilt here: the model accessors, the type tests 0x6F5850-0x6F5E20,
**	the size factors and the player aux-data accessors (FAIL_IF '!aux_data', lines 2431-2590). The
**	asserts sit on the client's lines (D25): the blank stretches hold the members not yet written.
**
**	Forms: the aux-data accessors leave through 'if (!Unknown_6F5A10()) return(x);' (the inline type
**	test): a plain '(Type >= 0 && Type <= 1)' merges the early return with the FAIL_IF's tail. The
**	size factors test through the inline Unknown_6F5A40 (a one-case switch: movsx, dec, jne).
*/
