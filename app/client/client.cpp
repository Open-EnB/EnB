/*
**	Earth & Beyond client (reconstruction)
**	Original location: e:\build\beta\G\app\client\client.cpp (client 0x726100-0x728450)
**	ClientClass (client.h). The asserts sit on the client's lines (D25). Notes at the end.
*/
#include <windows.h>
#include <mmsystem.h>
#include "client.h"
#include "..\common\connect.h"
#include "..\common\dbgconn.h"
#include "..\packets\pkdebug.h"
#include "..\shared\spln_db.h"
#include "..\shared\unknown_debug.h"
#include "wwaudio.h"
#include <stdlib.h>

void Unknown_Debug_Printf(const char *format, ...);	// 0x509DC0 (dbgmgr.cpp, D38)

// NOT EXACT: inline budget (the client calls the 'Null Item' string's assign out of line, ours inlines all three strings and swaps two temporaries' slots; the member stores and the prologue scheduled differently; five forms; attempts.py show 0x726100)
// 0x726100
ClientClass::ClientClass() :
	Px(NULL),
	PxProcess(NULL),
	DebugMode(false),
	Unknown11(false),
	Kernel(NULL),
	Unknown10BC(0),
	Unknown10C0(0),
	Unknown10C8(0),
	Unknown10CC(32),
	Unknown10D0(0),
	Unknown10D4(-2500.0f),
	Unknown10D8(2500.0f),
	Unknown10DC(-1000000.0f),
	Unknown10E0(-1000000.0f),
	Unknown10E4(1000000.0f),
	Unknown10E8(1000000.0f),
	Unknown10EC(0),
	Unknown10F0(0),
	Unknown10F4(0),
	Unknown10F8(false),
	Unknown10F9(false),
	Unknown10FA(false),
	Unknown10FC(10000.0f),
	Unknown1100(10000.0f),
	Unknown1104(1.2217305f),
	Unknown1108(true),
	Unknown110C(0),
	Unknown1110(1.0f),
	Unknown1114(1.0f),
	Unknown1128(0),
	DebugObjectID(0),
	ShipID(0),
	Unknown1134(0),
	Unknown1138(0),
	Unknown113C(0),
	ObjectsChanged(false),
	Unknown1144(0),
	Splines(NULL)
{
	srand(timeGetTime());
	Unknown1138 = rand();
	Connection = new PeekConnectionClass(new NullConnectionClass);
	Unknown18 = new Unknown763E00Class;
	Px = new PxClass;
	PxProcess = new PxProcessClass;
	Splines = new SplineDatabaseClass(NULL);
	Splines->Load("splines.wsd");
	Unknown831340List.Add(new Unknown831340Class(-1, -1, -1, RefCountPtr<RefCountClass>(), -1, -1, -1, 0, 0, 1, 0, 0, std::string("Null Item"), std::string(""), std::string("")), -1);
}

// 0x726C60
ClientClass::~ClientClass()
{
	HashNodeClass<ClientObjectClass *> *node;
	while ((node = ObjectList.First_Valid()) != NULL) {
		ObjectList.Remove(node);
		ClientObjectClass *object = node->Get_Record();
		object->Unknown_08();
		delete object;
	}
	HashNodeClass<Unknown86CRecordClass *> *record = Unknown86CList.First_Valid();
	while (record) {
		HashNodeClass<Unknown86CRecordClass *> *next = record->Next_Valid();
		Unknown86CList.Remove(record);
		delete record->Get_Record();
		record = next;
	}
	HashNodeClass<Unknown831340Class *> *item = Unknown831340List.First_Valid();
	while (item) {
		HashNodeClass<Unknown831340Class *> *next = item->Next_Valid();
		Unknown831340Class *value = item->Get_Record();
		Unknown831340List.Remove(item);
		delete value;
		item = next;
	}
	delete Connection;
	delete Unknown18;
	delete Splines;
	delete PxProcess;
	delete Px;
	WWAudioClass::Get_Instance()->Free_Completed_Sounds();
}

// NOT EXACT: shape (the client lays out the list check before the found return; three forms; attempts.py show 0x727070)
// 0x727070
ClientObjectClass *ClientClass::Find_Object(unsigned id)
{
	HashNodeClass<ClientObjectClass *> *node = ObjectList.Find(id);
	if (node) {
		return(node->Get_Record());
	}
	node = ObjectList.First_Valid();
	while (node) {
		if (node->Get_Record()->Get_Game_ID() == id) {

















































			G_ASSERT(0);
		}
		node = node->Next_Valid();
	}
	return(NULL);
}

// 0x727130
ClientObjectClass *ClientClass::Find_Object(const char *name)
{
	std::string target("");
	if (name) {
		target = name;
	}
	HashNodeClass<ClientObjectClass *> *node = ObjectList.First_Valid();
	while (node) {
		std::string object_name("");
		node->Get_Record()->Get_Name(object_name);
		if (target == object_name) {
			return(node->Get_Record());
		}
		node = node->Next_Valid();
	}
	return(NULL);
}

// 0x7273F0
EffectObjectClass *ClientClass::Find_Effect(unsigned id)
{
	HashNodeClass<EffectObjectClass *> *node = EffectList.Find(id);
	if (node) {
		return(node->Get_Record());
	}
	return(NULL);
}

// 0x727460
Unknown86CRecordClass *ClientClass::Find_Unknown86C(unsigned id)
{
	HashNodeClass<Unknown86CRecordClass *> *node = Unknown86CList.Find(id);
	if (node) {
		return(node->Get_Record());
	}
	return(NULL);
}

// 0x7274D0
void ClientClass::Add_Object(ClientObjectClass *object)
{
	ObjectsChanged = true;
	object->Node.Set_Key(object->Get_Game_ID());
	ObjectList.Add(&object->Node);





	G_ASSERT(ObjectList.Find(object->Get_Game_ID()));
}

// NOT EXACT: shape (the client puts the not-found return last; three forms; attempts.py show 0x727600)
// 0x727600
ClientObjectClass *ClientClass::Remove_Object(unsigned id)
{
	HashNodeClass<ClientObjectClass *> *node = ObjectList.Find(id);
	if (node == NULL) {
		return(NULL);
	}
	ObjectsChanged = true;
	ClientObjectClass *object = node->Get_Record();
	ObjectList.Remove(node);
	return(object);
}

// 0x727780
void ClientClass::Add_Effect(EffectObjectClass *effect, int a, int b)
{
	effect->Node.Set_Key(effect->ID);
	EffectList.Add(&effect->Node);
	effect->Unknown_6E74B0(a);
	effect->Unknown_6E6ED0(b);
}

// 0x7278F0
EffectObjectClass *ClientClass::Remove_Effects(unsigned id)
{
	EffectObjectClass *effect = NULL;
	HashNodeClass<EffectObjectClass *> *node = EffectList.Find(id);
	while (node) {
		effect = node->Get_Record();
		EffectList.Remove(node);
		node = EffectList.Find(id);
	}
	return(effect);
}

// 0x727AD0
EffectObjectClass *ClientClass::Get_Effect(unsigned id)
{
	EffectObjectClass *effect = NULL;
	HashNodeClass<EffectObjectClass *> *node = EffectList.Find(id);
	if (node) {
		effect = node->Get_Record();
	}
	return(effect);
}

// 0x727B40
void ClientClass::Add_Unknown86C(Unknown86CRecordClass *record)
{
	record->Node.Set_Key(record->ID);
	Unknown86CList.Add(&record->Node);
}

// 0x727CA0
Unknown86CRecordClass *ClientClass::Remove_Unknown86C(unsigned id)
{
	Unknown86CRecordClass *record = NULL;
	HashNodeClass<Unknown86CRecordClass *> *node = Unknown86CList.Find(id);
	if (node) {
		record = node->Get_Record();
		Unknown86CList.Remove(node);
	}
	return(record);
}

// 0x727E10
void ClientClass::Open_Connection()
{
	ConnectionClass *connection = new PeekConnectionClass(new RateConnectionClass(Create_Connection("SectorServer", true)));
	delete Connection;
	Connection = connection;
}

// 0x727F00
void ClientClass::Open_Connection(unsigned long ip, unsigned short port)
{
	ConnectionClass *connection = new PeekConnectionClass(new RateConnectionClass(Create_Connection(ip, port, true, true)));
	delete Connection;
	Connection = connection;
}

// NOT EXACT: register-only (the flag in dl, the id in eax; two forms; attempts.py show 0x728010)
// 0x728010
unsigned ClientClass::Set_Debug_Object(unsigned id)
{
	unsigned old = DebugObjectID;
	DebugObjectID = id;

















	G_ASSERT(Connection);
	Connection->Send_Packet(&DebugPacketClass(DebugObjectID, 0x21, Unknown11 ? 1 : 0));
	return(old);
}

// 0x728100
bool ClientClass::Get_Stats(float *a, float *b, float *c, float *d, float *e, float *f, int *g)
{
	return(Connection->Get_Stats(a, b, c, d, e, f, g));
}

// 0x728150
bool ClientClass::Send_Packet(PacketClass *packet)
{
	return(Connection->Send_Packet(packet));
}

// 0x728180
void ClientClass::Unknown_728180()
{
	Unknown10C0 -= (__int64)(Unknown10C8 / Unknown10CC);
	Unknown10D0 = 0;
	Unknown10C8 = 0;
}

// 0x7281D0
void ClientClass::Debug_Dump_Texture_Data()
{
	Unknown_Debug_Printf("Debug_Dump_Texture_Data - not yet implemented");
}

// 0x7281F0
bool ClientClass::Unknown_34(int arg)
{
	return(false);
}

// 0x728210
bool ClientClass::Toggle_Debug_Mode(int unused)
{
	Set_Debug_Mode(!DebugMode);
	return(true);
}

// NOT EXACT: register-only (the id in eax, the flag in ecx; two forms; attempts.py show 0x728240)
// 0x728240
void ClientClass::Set_Debug_Mode(bool on)
{
	if (DebugMode != on) {
		DebugMode = on;






















		G_ASSERT(Connection);
		Connection->Send_Packet(&DebugPacketClass(DebugObjectID, 0x1E, DebugMode));
	}
}
// 0x728300
bool ClientClass::Unknown_728300()
{
	ClientObjectClass *ship = Get_Ship();
	if (!ship) return(false);
	return(ship->Unknown_6FFE90());
}
// 0x728330
int ClientClass::Unknown_728330()
{
	ClientObjectClass *ship = Get_Ship();


	FAIL_IF(!ship) {return(0);}
	return(ship->Unknown_6FFF00());
}

// 0x728370
int ClientClass::Unknown_728370()
{
	ClientObjectClass *ship = Get_Ship();
	FAIL_IF(!ship) {return(-1);}
	return(ship->Unknown_6FFF50());
}
// (the client jumps to co.cpp's 0x6FFFA0)
// 0x7283B0
int ClientClass::Unknown_7283B0()
{
	ClientObjectClass *ship = Get_Ship();
	FAIL_IF(!ship) {return(0);}
	return(ship->Unknown_6FFFA0());
}

// 0x7283F0
bool ClientClass::Unknown_7283F0()
{
	ClientObjectClass *ship = Get_Ship();
	FAIL_IF(!ship) {return(false);}
	return(ship->Unknown_700400());
}

// 0x728430
bool ClientClass::Get_Objects_Changed()
{
	bool changed = ObjectsChanged;
	ObjectsChanged = false;
	return(changed);
}

/*
**	Reconstruction notes (client.cpp, 0x726100-0x728450; kernel-map-step9.md 2.1)
**
**	* The lists are wwlib's HashListClass (hashlist.h, 257 values) as Renegade has it: the client's
**	  code is its inlined Find / Add / Remove, the node flags (FirstInTable 1, LastInTable 2, KeySet
**	  0x40, InList and NewInList 0x18, ListCreated 0x20). The objects carry their own node (co.cpp's at
**	  +0x60, eff_o.cpp's at +0x04); the fourth list makes its nodes (the constructor's 'Null Item').
**	  The out-of-line helpers after 0x728450 (Next_Valid, the lists' destructors, the node's
**	  constructor 0x729260, Add 0x729320, Remove 0x7290F0) are the template's instances.
**	* The class is packed to 4 (#pragma pack in client.h): the client doesn't pad the vtable
**	  pointer for the __int64 at +0x10C0.
**	* Inline in client.h and emitted after the constructor: slots +0x18 .. +0x30 (0x7268E0 ..
**	  0x726A30; +0x28 / +0x30 inline Find_Object), then connect.h's ConnectionClass slots and the
**	  do-nothing connection's implicit destructor (0x726AF0 .. 0x726C40; too short to count).
**	  0x727040 is PxClass's scalar deleting destructor, 0x7280E0 DebugPacketClass's inline one.
**	* Forms: Find_Object(const char *) takes the next node before the name's destructor (a while
**	  loop); Get_Effect and Remove_Unknown86C return through a local set to NULL; Remove_Effects
**	  looks the id up again after each Remove; the DEBUG packets are temporaries.
**	* 0x727E10 (network.ini's SectorServer) has no caller in the client: the base's connection is the
**	  do-nothing one until ClientTaskClass's redirect calls 0x727F00.
*/
