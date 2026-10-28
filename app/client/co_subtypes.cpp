// Earth & Beyond client (reconstruction): app\client\co_subtypes.cpp, a provisional name (D122). Notes at the end of the file.
#include "co_subtypes.h"
#include "client.h"
#include "rendobj.h"
#include "..\shared\heightfield.h"

extern int Unknown_BE350C;	// NAME 1: neutral (D10): SClient::Do_Frame's frame toggle (sclient.cpp)

#pragma auto_inline(off)	// type 24's base (to 0x70E240)
// 0x70DBC0
Unknown70DBC0Class::Unknown70DBC0Class(ClientClass *client, int type, unsigned id) :
	ClientObjectClass(client, type, id)
{
}

// 0x70DC90
Unknown44AC70Class *Unknown70DBC0Class::Unknown_Dyn04()
{
	return(this);
}

// 0x70DCB0
int Unknown70DBC0Class::Unknown_40()
{
	return((int)this);
}

// 0x70DCD0
float Unknown70DBC0Class::Unknown_30()
{
	return(Radius);
}

// 0x70DCF0
void Unknown70DBC0Class::Unknown_34(float value)
{
	Unknown44AC70Class::Unknown04 = value;
	ClientObjectClass::Unknown_34(value);
}

// 0x70DD20
const char *Unknown70DBC0Class::Unknown_Dyn30()
{
	return("Unnamed ClientProximityObject");
}

// 0x70DD40
Vector3 Unknown70DBC0Class::Unknown_24()
{
	return(ClientObjectClass::Unknown_24());
}

// 0x70DD70
Vector3 Unknown70DBC0Class::Unknown_48()
{
	return(Unknown_4C().Get_X_Vector());
}

// 0x70DDB0
Matrix3D Unknown70DBC0Class::Unknown_4C()
{
	return(ClientObjectClass::Unknown_28(true));
}

// 0x70DDE0
Vector3 Unknown70DBC0Class::Unknown_2C()
{
	return(ClientObjectClass::Unknown_2C());
}

// 0x70DE10
float Unknown70DBC0Class::Unknown_Dyn44()
{
	return(Unknown_2C().Length());
}

// 0x70DE80
Vector3 Unknown70DBC0Class::Unknown_Dyn48()
{
	return(Unknown1AC[Unknown_BE350C ^ 1].Get_Translation());
}

// 0x70DEC0
Vector3 Unknown70DBC0Class::Unknown_Dyn4C()
{
	return(Unknown1AC[Unknown_BE350C ^ 1].Get_X_Vector());
}

// 0x70DF00
Matrix3D Unknown70DBC0Class::Unknown_Dyn50()
{
	return(Unknown1AC[Unknown_BE350C ^ 1]);
}

// 0x70DF90
Vector3 Unknown70DBC0Class::Unknown_Dyn54()
{
	return(Unknown1AC[Unknown_BE350C ^ 1].Get_X_Vector());
}

// 0x70DFD0
float Unknown70DBC0Class::Unknown_Dyn58()
{
	return(Unknown1AC[Unknown_BE350C ^ 1].Get_X_Vector().Length());
}

// 0x70E040
Vector3 Unknown70DBC0Class::Unknown_Dyn70()
{
	return(Unknown_Dyn4C());
}

// 0x70E070
void Unknown70DBC0Class::Unknown_Dyn74()
{
}

// 0x70E0C0 (scalar deleting 0x70E090)
Unknown70DBC0Class::~Unknown70DBC0Class()
{
}

// 0x70E150
void Unknown70DBC0Class::Unknown_20()
{
	Matrix3D &tm = Unknown1AC[Unknown_BE350C];
	tm = Unknown_4C();
	ClientObjectClass::Unknown_20();
}

// 0x70E1F0
unsigned Unknown70DBC0Class::Unknown_Dyn1C()
{
	return((unsigned)Client->Unknown10BC / 100);
}

// 0x70E220
int Unknown70DBC0Class::Unknown_Dyn20()
{
	return(Type);
}

// 0x70E240
void Unknown70DBC0Class::Unknown_Dyn64()
{
	Unknown_Dyn68(Unknown_Dyn48());
}
#pragma auto_inline(on)	// type 24 calls its base out of line (0x70E2E0, 0x70E380, 0x70E4E0)

// 0x70E2E0
Unknown70E2E0Class::Unknown70E2E0Class(ClientClass *client, int type, unsigned id) :
	Unknown70DBC0Class(client, type, id),
	Unknown220(false)
{
}

// 0x70E330
int Unknown70E2E0Class::Unknown_44()
{
	return((int)this);
}

// 0x70E380 (scalar deleting 0x70E350)
Unknown70E2E0Class::~Unknown70E2E0Class()
{
}

// 0x70E3B0 (inlined into the next three)
void Unknown70E2E0Class::Unknown_70E3B0(const Vector3 &pos)
{
	HeightFieldClass *field = (HeightFieldClass *)Client->Unknown1144;
	float height = field->Get_Height(pos);
	Unknown20C = pos.Z - height;
}

// 0x70E3F0
void Unknown70E2E0Class::Unknown_Dyn68(Vector3 pos)
{
	Unknown44AC70Class::Unknown_Dyn68(pos);
	Unknown_70E3B0(pos);
}

// 0x70E460
void Unknown70E2E0Class::Unknown_Dyn6C(const Matrix3D &tm)
{
	Unknown44AC70Class::Unknown_Dyn6C(tm);
	Unknown_70E3B0(tm.Get_Translation());
}

// 0x70E4E0
void Unknown70E2E0Class::Unknown_20()
{
	Unknown70DBC0Class::Unknown_20();
	Unknown_70E3B0(Unknown_24());
}

// 0x70E530
void Unknown70E2E0Class::Unknown_3C()
{
}

// 0x70E5A0
Unknown70E5A0Class::Unknown70E5A0Class(ClientClass *client, unsigned id) :
	ClientObjectClass(client, 28, id),
	TargetID(-1),
	ParentID(-1),
	Unknown190(true),
	Bone(-1),
	Unknown1C4(1.0f, 0.0f, 0.0f),
	Unknown1D0(1.0f)
{
}

// 0x70E6B0 (scalar deleting 0x70E680)
Unknown70E5A0Class::~Unknown70E5A0Class()
{
	if (ParentID != -1) {
		ClientObjectClass *parent = Client->Find_Object(ParentID);
		if (parent) {
			parent->Model->Release_Bone(Bone);
		}
	}
}

// 0x70E750
void Unknown70E5A0Class::Unknown_70E750(int id)
{
	Unknown1D0 = 0.0f;
	TargetID = id;
}

// 0x70E780
void Unknown70E5A0Class::Unknown_14(UnknownB02C90Class *parent, int slot, int child_slot)
{
	ClientObjectClass *owner = (ClientObjectClass *)parent->Unknown04;
	ParentID = owner->GameID;
	Bone = owner->Unknown_725A90(slot);
	owner->Model->Capture_Bone(Bone);
}

// 0x70E7D0
void Unknown70E5A0Class::Unknown_18(UnknownB02C90Class *parent, int slot, int child_slot)
{
	if (ParentID != -1) {
		ClientObjectClass *parent_object = Client->Find_Object(ParentID);
		if (parent_object) {
			parent_object->Model->Release_Bone(Bone);
		}
	}
	ParentID = 0;
	TargetID = -1;
	Bone = -1;
}

// NOT EXACT: shape (stack layout: the client's frame is 0x128 bytes, ours 0xFC; the inverse transform's terms in another order; attempts.py show 0x70E850)
// 0x70E850
void Unknown70E5A0Class::Unknown_20()
{
	if (TargetID == -1 || ParentID == -1) {
		return;
	}
	ClientObjectClass *target = Client->Find_Object(TargetID);
	if (!target) {
		return;
	}
	RenderObjClass *model = Client->Find_Object(ParentID)->Model;
	Vector3 target_pos = target->Unknown_24();
	Matrix3D bone(true);
	bone = model->Get_Bone_Transform(Bone);
	Matrix3D inverse(true);
	Unknown190.Get_Orthogonal_Inverse(inverse);
	Matrix3D base(true);
	Matrix3D::Multiply(bone, inverse, &base);
	bone = base;
	Vector3 dir;
	Matrix3D::Inverse_Transform_Vector(base, target_pos, &dir);
	dir.Normalize();
	Vector3 y_axis(-dir.Y, dir.X, 0.0f);
	Vector3 z_axis(-(dir.X * dir.Z), dir.Z * y_axis.X, dir.X * y_axis.Y - y_axis.X * dir.Y);
	Matrix3D aim(dir, y_axis, z_axis, Vector3(0.0f, 0.0f, 0.0f));
	float x_rotation = WWMath::Fabs(aim.Get_X_Rotation());
	float y_rotation = WWMath::Fabs(aim.Get_Y_Rotation());
	float z_rotation = WWMath::Fabs(aim.Get_Z_Rotation());
	if (x_rotation < WWMATH_PI / 2 && y_rotation < WWMATH_PI / 2 && z_rotation < WWMATH_PI / 2) {
		model->Control_Bone(Bone, aim, false);
		Unknown190 = aim;
		if (Unknown1D0 == 1.0) {
			Unknown1C4 = target_pos;
		}
	}
}

/*
** Notes
**
** The run 0x70DBC0-0x70F030 lies between co_player_relationship.cpp (to 0x70DA40) and extrap.cpp (from 0x70F040)
** and has no assert or string naming its file: D122 names it co_subtypes.cpp until one does. It holds CREATE 4's
** two subclasses of ClientObjectClass (cpacket.cpp's switch: type 24 -> 0x70E2E0, type 28 -> 0x70E5A0; every other
** type is a plain ClientObjectClass) and type 24's base 0x70DBC0. The compiler-generated pieces (the scalar deleting
** destructors 0x70E090 / 0x70E350 / 0x70E680 and the this-adjusting thunks 0x70E2A0-0x70E2D0, 0x70E580, 0x70E590,
** 0x70F030) come from the declarations.
**
** Type 24's base is also dynamic_obj.cpp's proximity object (Unknown44AC70Class at +0x188, vtable 0xB020E4 in the base,
** 0xB021B0 in type 24): its slots return the object's position, velocity, radius, type and the client's time, and the
** transform of the other frame (0xBE350C toggles every frame; slot +0x20 0x70E150 writes this frame's copy). Two of its
** slots share ClientObjectClass's names because one function overrides both (0x70DD40 / 0x70DDE0 with the thunks
** 0x70E2C0 / 0x70E2D0). Type 24 adds the height above the land's height field (+0x20C), kept by its position and
** transform setters and its per-frame update through 0x70E3B0, which VC6 inlines into all three (0x70E3B0 itself has
** no caller). The height field is the client's +0x1144 (an int in client.h, cast here).
**
** Type 28 hangs on a parent object's attachment slot: attaching (+0x14 0x70E780) records the parent's id and captures
** the slot's bone (UnknownB02C90Class 0x725A90); SET_TARGET 25 (0x70E750) sets the target; the per-frame update 0x70E850
** turns the bone towards the target (rotation limits of a quarter turn about each axis) and remembers the target's
** position while +0x1D0 is still 1; detaching (+0x18 0x70E7D0) and the destructor release the bone.
**
** 0x70E850 is NOT EXACT (shape): Inverse_Transform_Vector gives the client's subtractions from memory, but its frame
** holds more temporaries (0x128 bytes against our 0xFC) and the inverse rotation's terms come in another order
** (attempts.py show 0x70E850 lists the forms tried).
*/
