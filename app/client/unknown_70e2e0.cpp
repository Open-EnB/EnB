// Earth & Beyond client (reconstruction): app\client\unknown_70e2e0.cpp, PROVISIONAL file name (D137). Notes at the end of the file.
#include "co_subtypes.h"
#include "client.h"
#include "rendobj.h"
#include "..\shared\heightfield.h"

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
	HeightFieldClass *field = Client->Unknown1144;
	float height = field->Get_Height(pos);
	Unknown20C = pos.Z - height;
}

// 0x70E3F0
void Unknown70E2E0Class::Unknown_Dyn68(Vector3 pos)
{
	DynObj::Unknown_Dyn68(pos);
	Unknown_70E3B0(pos);
}

// 0x70E460
void Unknown70E2E0Class::Unknown_Dyn6C(const Matrix3D &tm)
{
	DynObj::Unknown_Dyn6C(tm);
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

/*
** Notes
**
** Client 0x70E2E0-0x70E590, an object of its own by the CRT initialiser table (SG, run 2026-11-18): CREATE 4's
** type 24 (Unknown70E2E0Class, cpacket.cpp's switch), then <string>'s guarded initialiser 0x70E550 (entry 555)
** and the class's compiler-generated this-adjusting thunks 0x70E580 / 0x70E590; type 28 (from 0x70E5A0, its own
** guard 0x70F000, entry 556) stays in co_subtypes.cpp. No assert or string names the file: PROVISIONAL (D137).
*/
