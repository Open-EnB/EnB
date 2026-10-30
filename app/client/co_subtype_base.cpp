// Earth & Beyond client (reconstruction): app\client\co_subtype_base.cpp, a provisional name (D132, D10). Notes at the end of the file.
#include "co_subtypes.h"
#include "client.h"

extern int Unknown_BE350C;	// NAME 1: neutral (D10): SClient::Do_Frame's frame toggle (sclient.cpp)

// 0x70DBC0
Unknown70DBC0Class::Unknown70DBC0Class(ClientClass *client, int type, unsigned id) :
	ClientObjectClass(client, type, id)
{
}

// 0x70DC90
DynObj *Unknown70DBC0Class::Unknown_Dyn04()
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
	DynObj::Unknown04 = value;
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
void Unknown70DBC0Class::Unknown_Dyn74(int a, int b, int c, int d)
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

/*
** Notes
**
** The run 0x70DBC0-0x70E2D0 is type 24's base (Unknown70DBC0Class, co_subtypes.h): its constructor, the DynObj and
** ClientObjectClass slots it overrides, the destructor (scalar deleting 0x70E090), the per-frame update 0x70E150 and
** the this-adjusting thunks 0x70E2A0-0x70E2D0. It has no assert or string naming its file; D132 gives it this file
** of its own because the client never inlines the base into type 24 (co_subtypes.cpp's 0x70E2E0, 0x70E380,
** 0x70E4E0 call it out of line), which a separate translation unit gives without a pragma. The name follows
** D122's co_subtypes.cpp (NAME 1, provisional) until an assert or string names the file.
**
** Type 24's base is also dynamic_obj.cpp's DynObj (at +0x188, vtable 0xB020E4 in the base,
** 0xB021B0 in type 24): its slots return the object's position, velocity, radius, type and the client's time, and the
** transform of the other frame (0xBE350C toggles every frame; slot +0x20 0x70E150 writes this frame's copy). Two of its
** slots share ClientObjectClass's names because one function overrides both (0x70DD40 / 0x70DDE0 with the thunks
** 0x70E2C0 / 0x70E2D0).
*/
