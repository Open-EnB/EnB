/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\client\client.h (name assumed from client.cpp)
**
**	ClientClass, the client base both clients derive from: SClient (sclient.cpp) and the movie
**	script's client (mclient.cpp). 0x1150 bytes, constructor 0x726100, destructor 0x726C60, vtable
**	0xB02D7C (23 slots). No RTTI or string names it (docs/kernel-map-step9.md 2.1). It holds the
**	sector's objects in four wwlib hash lists keyed by game id (the client's own name: the assert
**	'ObjectList.Find(object->Get_Game_ID())'), the connection to the sector server (+0x1124) and the
**	debug state the DEBUG packet (opcode 26) carries.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef CLIENT_H  // NAME 3 [client.h]: header name assumed from the client's client.cpp
#define CLIENT_H

#include "hashlist.h"
#include "refcount.h"
#include "ref_ptr.h"
#include <string>
#include "..\shared\unknown_debug.h"
#include "matrix3d.h"
#include "..\shared\duration.h"
class KernelClass;
class RenderObjClass;
class SubPartsPacket;
class PacketClass;
class ConnectionClass;
class SplineDatabaseClass;
class ClientClass;

#include "co.h"

#ifndef EFF_O_H	// eff_o.cpp's effect object as far as the lists need it (eff_o.h, included first, declares it all)
class EffectObjectClass		// NAME 3: class-from-file (D24) for eff_o.cpp
{
public:
	virtual ~EffectObjectClass();						// +0x00
	void Unknown_6E74B0(int arg);						// 0x6E74B0 (eff_o.cpp)
	void Unknown_6E6ED0(int arg);						// 0x6E6ED0 (eff_o.cpp)
	void Unknown_6E6E30();								// 0x6E6E30 (eff_o.cpp; cpacket.cpp's REMOVE_EFFECT 15)
	bool Unknown_6E6AD0();								// 0x6E6AD0 (eff_o.cpp; SClient::Process_AI removes the effect when it is false)

	HashNodeClass<EffectObjectClass *> Node;			// +0x04 ClientClass::EffectList's node  // NAME 2: what it is
	char Unknown20[0x30 - 0x20];						// +0x20
	unsigned ID;										// +0x30 the list's key  // NAME 2: what it is
};
#endif
// The objects of ClientClass's third list (class and file unknown)
class Unknown86CRecordClass	// NAME 1: neutral (D10)
{
public:
	virtual ~Unknown86CRecordClass();					// +0x00 (ClientClass's destructor deletes through it)
	virtual void Unknown_04();							// +0x04
	virtual bool Unknown_08(const Vector3 &position);	// +0x08 (SClient::Process_AI passes the main view camera's position; false: the record goes)  // NAME 1 [position]: what it gets

	HashNodeClass<Unknown86CRecordClass *> Node;		// +0x04 the list's node  // NAME 2: what it is
	char Unknown20[0x24 - 0x20];						// +0x20
	unsigned ID;										// +0x24 the list's key  // NAME 2: what it is
};

// The 0x60-byte objects of ClientClass's fourth list (constructor 0x831340, destructor 0x8316D0, after
// circbuff.cpp; the constructor makes one named 'Null Item')
class Unknown831340Class	// NAME 1: neutral (D10)
{
public:
	Unknown831340Class(int a, int b, int c, const RefCountPtr<RefCountClass> &d, int e, int f, int g, int h, int i, int j, int k, int l,
		const std::string &name, const std::string &m, const std::string &n);	// 0x831340  // NAME 1 [a..n]: the arguments are unknown  // NAME 2 [name]: 'Null Item'
	~Unknown831340Class();								// 0x8316D0 (not virtual: delete calls it directly)

	int Get_Item_Type() { return(ItemType); }			// (cpacket.cpp: ITEM_BASE 37's G_ASSERT 'ItemType == inode->Get()->Get_Item_Type()', l.1234)  // NAME 3: the assert's
	const std::string &Get_Name() { return(Name); }		// (ITEM_BASE 37's G_ASSERT 'Name == inode->Get()->Get_Name()', l.1235)  // NAME 3: the assert's

	char Unknown00[0x0C];
	int ItemType;										// +0x0C the constructor's g (ITEM_BASE 37 passes its ItemType)  // NAME 3: Get_Item_Type's
	char Unknown10[0x14 - 0x10];
	std::string Name;									// +0x14 the constructor's name  // NAME 3: Get_Name's
	char Unknown24[0x60 - 0x24];
};

// px.cpp's PxList (0x34 bytes, constructor 0x44BE90, destructor 0x44BF00; app\shared\px.h)
class PxList;

// px_process.cpp's list (0x0C bytes, constructor 0x455A90, destructor 0x455AB0; app\shared\px_process.h)
#include "..\shared\px_process.h"

// The 0x10-byte object at ClientClass +0x18 (constructor 0x763E00, destructor 0x763E30: between
// proj_decal_manager.cpp and tsk_game.cpp; file unknown)
class Unknown763E00Class	// NAME 1: neutral (D10)
{
public:
	Unknown763E00Class();								// 0x763E00
	void Unknown_763E70(const char *ini);				// 0x763E70: SClient::Init passes 'rs.ini'
	void *Unknown_764030(int rslid);					// 0x764030 (co.cpp's model loader: the render state list of a base asset's RSLID)  // NAME 2 [rslid]: the INI key it is read from
	void Unknown_763FF0();								// 0x763FF0 (SClient::Shutdown, before the delete)
	~Unknown763E00Class();								// 0x763E30

	char Unknown00[0x10];
};

#pragma pack(push, 4)		// the client's layout: the vtable pointer isn't padded for Unknown10C0 (as tsk_game.h)
class ClientClass		// NAME 3: class-from-file (D24) for client.cpp; the client names no class
{
public:
	ClientClass();										// 0x726100
	~ClientClass();										// 0x726C60 (not virtual: ClientTaskClass deletes SClient through its own destructor)

	// The six slots every client fills (0x9CCD10 in this vtable); the names are SClient's (sclient.h)
	virtual void Init() = 0;							// +0x00: ClientTaskClass::Init calls it once Kernel is set  // NAME 2: what SClient's does
	virtual void Shutdown() = 0;						// +0x04: ClientTaskClass::Shutdown  // NAME 2: what SClient's does
	virtual void Unknown_08() = 0;						// +0x08
	virtual void Process_Packets() = 0;					// +0x0C (SClient: 'SClient::Process_Packets')  // NAME 3: SClient's string names its override
	virtual void Unknown_10() = 0;						// +0x10
	virtual int Do_Frame(bool focused) = 0;				// +0x14: ClientTaskClass::Sector, 0 .. 5  // NAME 2: what SClient's does  // NAME 1 [focused]: Is_Focused()

	virtual int Unknown_18() { return(0); }				// +0x18 0x7268E0
	virtual int Unknown_1C() { return(0); }				// +0x1C 0x726900
	virtual int Unknown_20() { return(0); }				// +0x20 0x726920
	virtual unsigned Set_Debug_Object(unsigned id);		// +0x24 0x728010: returns the previous id  // NAME 2: what it does (the DEBUG packet's game id)
	virtual ClientObjectClass *Get_Avatar() { return(Find_Object(AvatarID)); }	// +0x28 0x726940 (D123)  // NAME 2: what it does
	virtual unsigned Set_Ship_ID(unsigned id) { unsigned old = ShipID; ShipID = id; return(old); }	// +0x2C 0x726A00  // NAME 2: what it does (Get_Ship's id)
	virtual ClientObjectClass *Get_Ship() { return(Find_Object(ShipID)); }	// +0x30 0x726A30  // NAME 2: client.cpp's FAIL_IF '!ship' on its result
	virtual bool Unknown_34(int arg);					// +0x34 0x7281F0: false
	virtual bool Toggle_Debug_Mode(int unused);			// +0x38 0x728210 (ret 4: an argument it ignores)  // NAME 2: what it does  // NAME 1 [unused]: never read
	virtual void Set_Debug_Mode(bool on);				// +0x3C 0x728240 (SClient's override logs 'Debug mode turned %s.')  // NAME 2: what it does
	virtual void Add_Object(ClientObjectClass *object);	// +0x40 0x7274D0 (the assert names 'object')  // NAME 2: what it does
	virtual ClientObjectClass *Remove_Object(unsigned id);	// +0x44 0x727600  // NAME 2: what it does
	virtual void Add_Effect(EffectObjectClass *effect, int a, int b);	// +0x48 0x727780  // NAME 2: what it does  // NAME 1 [a, b]: handed to the effect
	virtual EffectObjectClass *Remove_Effects(unsigned id);	// +0x4C 0x7278F0: every one with the id; returns the last  // NAME 2: what it does
	virtual EffectObjectClass *Get_Effect(unsigned id);	// +0x50 0x727AD0  // NAME 2: what it does
	virtual void Add_Unknown86C(Unknown86CRecordClass *record);	// +0x54 0x727B40
	virtual Unknown86CRecordClass *Remove_Unknown86C(unsigned id);	// +0x58 0x727CA0

	ClientObjectClass *Find_Object(unsigned id);		// 0x727070: the hash, then a check of the whole list (assert l.165)  // NAME 2: what it does
	ClientObjectClass *Find_Object(const char *name);	// 0x727130  // NAME 2: what it does
	EffectObjectClass *Find_Effect(unsigned id);		// 0x7273F0  // NAME 2: what it does
	Unknown86CRecordClass *Find_Unknown86C(unsigned id);	// 0x727460
	void Open_Connection();								// 0x727E10: network.ini's SectorServer (no caller in the client)  // NAME 2: what it does
	void Open_Connection(unsigned long ip, unsigned short port);	// 0x727F00: ClientTaskClass's redirect  // NAME 2: what it does
	bool Get_Stats(float *a, float *b, float *c, float *d, float *e, float *f, int *g);	// 0x728100: the connection's  // NAME 2: what it does
	bool Send_Packet(PacketClass *packet);				// 0x728150  // NAME 2: what it does
	void Unknown_728180();								// 0x728180
	void Debug_Dump_Texture_Data();						// 0x7281D0 (S: 'Debug_Dump_Texture_Data - not yet implemented')
	bool Unknown_728300();								// 0x728300: Get_Ship()'s 0x6FFE90
	int Unknown_728330();								// 0x728330: Get_Ship()'s 0x6FFF00 (FAIL_IF l.419)
	int Unknown_728370();								// 0x728370: Get_Ship()'s 0x6FFF50 (FAIL_IF l.427)
	int Unknown_7283B0();								// 0x7283B0: Get_Ship()'s aux data +0x39C4 (FAIL_IF l.435)
	bool Unknown_7283F0();								// 0x7283F0: Get_Ship()'s 0x700400 (FAIL_IF l.443)
	bool Get_Objects_Changed();							// 0x728430: reads and clears ObjectsChanged  // NAME 2: what it does

	int Unknown04;										// +0x04 (the constructor leaves it)
	PxList *Px;											// +0x08  // NAME 3: px.cpp's
	PxProcessClass *PxProcess;							// +0x0C  // NAME 3: px_process.cpp's
	bool DebugMode;										// +0x10 (slots +0x38 / +0x3C, the DEBUG packet's user data)  // NAME 2: what it holds
	bool Unknown11;										// +0x11 (Set_Debug_Object's DEBUG packet's user data)
	KernelClass *Kernel;								// +0x14 ClientTaskClass::Init sets it  // NAME 2: what it holds
	Unknown763E00Class *Unknown18;						// +0x18
	HashListClass<ClientObjectClass *> ObjectList;		// +0x1C (the assert's name)
	HashListClass<EffectObjectClass *> EffectList;		// +0x444  // NAME 2: what it holds
	HashListClass<Unknown86CRecordClass *> Unknown86CList;	// +0x86C
	HashListClass<Unknown831340Class *> Unknown831340List;	// +0xC94 (the constructor adds the 'Null Item')
	int Unknown10BC;									// +0x10BC
	DurationClass Unknown10C0;							// +0x10C0 the server clock's offset (the packets' TimeStamp minus it; 0x728180 takes Unknown10C8 / Unknown10CC from it)
	float Unknown10C8;									// +0x10C8
	int Unknown10CC;									// +0x10CC (32)
	int Unknown10D0;									// +0x10D0
	float Unknown10D4;									// +0x10D4 (-2500)
	float Unknown10D8;									// +0x10D8 (2500)
	float Unknown10DC;									// +0x10DC (-1e6)
	float Unknown10E0;									// +0x10E0 (-1e6)
	float Unknown10E4;									// +0x10E4 (1e6)
	float Unknown10E8;									// +0x10E8 (1e6)
	float Unknown10EC;									// +0x10EC (SERVER_PARAMETERS 66: FogNear)
	float Unknown10F0;									// +0x10F0 (SERVER_PARAMETERS 66: FogFar)
	int Unknown10F4;									// +0x10F4 (SERVER_PARAMETERS 66: DebrisMode)
	bool Unknown10F8;									// +0x10F8
	bool Unknown10F9;									// +0x10F9
	bool Unknown10FA;									// +0x10FA
	float Unknown10FC;									// +0x10FC (10000)
	float Unknown1100;									// +0x1100 (10000)
	float Unknown1104;									// +0x1104 (1.2217: 70 degrees in radians)
	bool Unknown1108;									// +0x1108 (true)
	float Unknown110C;									// +0x110C (SERVER_PARAMETERS 66: ImpulseRate)
	float Unknown1110;									// +0x1110 (1.0)
	float Unknown1114;									// +0x1114 (1.0)
	int Unknown1118[3];									// +0x1118 (the constructor leaves them)
	ConnectionClass *Connection;						// +0x1124 (the asserts 'Connection')
	int Unknown1128;									// +0x1128
	unsigned AvatarID;									// +0x112C slots +0x24 / +0x28 (D123: CLIENT_AVATAR 55 sets it from the packet's AvatarID)  // NAME 2: what it holds
	unsigned ShipID;									// +0x1130 slots +0x2C / +0x30  // NAME 2: what it holds
	int Unknown1134;									// +0x1134
	int Unknown1138;									// +0x1138 rand() after srand(timeGetTime())
	int Unknown113C;									// +0x113C
	bool ObjectsChanged;								// +0x1140 set by Add_Object / Remove_Object  // NAME 2: what it holds
	int Unknown1144;									// +0x1144
	SplineDatabaseClass *Splines;						// +0x1148 splines.wsd  // NAME 2: what it holds
	int Unknown114C;									// +0x114C
};
#pragma pack(pop)

#endif
