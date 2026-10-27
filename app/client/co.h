/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\client\co.h (name assumed from co.cpp)
**
**	co.cpp's client object, ClientObjectClass (the RTTI's .?AVClientObjectClass@@): co.cpp is rebuilt
**	only in part (its small members), so the class is declared as far as co.cpp, client.cpp, cpacket.cpp
**	and co_data.cpp use it (moved here from client.h). What the client shows of it: the constructor 0x6F2820 (vtable
**	0xB0070C; its first argument, the client, at +0x7C: CREATE 0x733700 passes its SClient), the
**	destructor 0x6F2FE0 (it deletes the object's data through co_data.cpp's Remove_All 0x79D390 and
**	resets a MultiFixedPoolDecalSystemClass vtable at +0x4C), ClientClass's object list node at
**	+0x60 and the game id at +0x90. aux_roots.h's UnknownCOClass (the factory 0x6F6000) is the
**	same class. Names: D10, D24, D56.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __CO_H  // NAME 3 [co.h]: header name assumed from the client's co.cpp
#define __CO_H

#include "hashlist.h"
#include "refcount.h"
#include <string>
#include "..\shared\unknown_debug.h"

class RenderObjClass;
class Vector3;
class Matrix3D;
class SphereClass;
class RelationshipClass;

// co.cpp's +0xB4 object: a RefCountClass the constructor gets from 0x731FA0 (0xB8 bytes, constructor
// 0x732030); 0x6FCB20 passes the visibility to its slot +0x18
class UnknownB4Class : public RefCountClass		// NAME 1: neutral (D10)
{
public:
	virtual void Unknown_08();
	virtual void Unknown_0C();
	virtual void Unknown_10();
	virtual void Unknown_14();
	virtual void Unknown_18(bool visible);			// NAME 2 [visible]: 0x6FCB20's argument
	virtual void Unknown_1C();
	virtual void Unknown_20();
	virtual void Unknown_24();
	virtual void Unknown_28();
	virtual Matrix3D Unknown_2C(int unknown);		// +0x2C (0x6FED80 returns it when the object is set)  // NAME 1 [unknown]: neutral
};

// co.cpp's +0x98 object: 4 bytes, vtable 0xB006B8 (its trivial slots 0x6F2D40-0x6F2F00), base vtable
// 0xB00768; the constructor news it
class Unknown98Class							// NAME 1: neutral (D10)
{
public:
	virtual ~Unknown98Class();
	virtual int Unknown_04();
	virtual Matrix3D Unknown_08(int unknown);
	virtual Vector3 Unknown_0C(int unknown);
	virtual Vector3 Unknown_10(int unknown);	// +0x10 (0x6F2ED0: a zero Vector3); 0x6F5050 passes the client's +0x10BC  // NAME 1 [unknown]: neutral
	virtual Vector3 Unknown_14(int unknown);	// +0x14 (0x6F2F00: a zero Vector3); 0x6F3ED0 passes the client's +0x10BC  // NAME 1 [unknown]: neutral
};

void Unknown_810540(RenderObjClass *model, int state, int unknown);	// shiputils.cpp (cdecl)  // NAME 1 [model, state, unknown]: co.cpp passes the model, 1 or 2, and 0
class SubPartsPacket;
class ClientClass;

class ClientObjectClass		// NAME 3: the RTTI's .?AVClientObjectClass@@ for co.cpp's class, not tied to this layout by a vtable
{
public:
	virtual ~ClientObjectClass();						// +0x00 (ClientClass's destructor deletes through it)
	virtual void Unknown_04();
	virtual void Unknown_08();							// +0x08: ClientClass's destructor calls it before the delete
	virtual void Unknown_0C(ClientObjectClass *child, int a, int b);	// +0x0C (cpacket.cpp: CREATE_ATTACHMENT 74 passes the child, its Unknown18 and 0)  // NAME 1 [child, a, b]: what the handler passes
	virtual void Unknown_10(int a, int b, int c);		// +0x10 0x7257F0 (the base's, client.cpp's region)  // NAME 1 [a, b, c]: neutral
	virtual void Unknown_14(int a, int b, int c) {}		// +0x14 0x6F2C50 (empty)  // NAME 1 [a, b, c]: neutral
	virtual void Unknown_18(int a, int b, int c) {}		// +0x18 0x6F2C70 (empty)  // NAME 1 [a, b, c]: neutral
	virtual void Unknown_1C();							// +0x1C 0x6F5120 (the aux data's slot +0x20)
	virtual void Unknown_20();							// +0x20 0x6F3C60 (FAIL_IF '!RenderObject' l.1779 inline: 0x6FD2C0)
	virtual Vector3 Unknown_24();						// +0x24 0x6F4C20: +0x98's slot +0x0C clamped to the client's bounds (G_ASSERT 'pos.Is_Valid()', l.762)
	virtual Matrix3D Unknown_28(int unknown);			// +0x28 0x6F4610 (0x6FEC40 transforms the sphere by it)  // NAME 1 [unknown]: neutral
	virtual Vector3 Unknown_2C();						// +0x2C 0x6F5050
	virtual float Unknown_30() { return(Radius); }		// +0x30 0x6F2CC0
	virtual void Unknown_34(float value);				// +0x34 0x6F3BA0  // NAME 1 [value]: neutral
	virtual void Unknown_38();							// +0x38 0x6F39F0: the radius from cbasset.ini's BASE-n section (FAIL_IF l.331 / 336)
	virtual void Unknown_3C() {}						// +0x3C 0x6F2CE0 (empty)
	virtual int Unknown_40() { return(0); }				// +0x40 0x6F2D00
	virtual int Unknown_44() { return(0); }				// +0x44 0x6F2D20

	unsigned Get_Game_ID() { return(GameID); }			// (the assert's name)
	void Get_Name(std::string &name);					// 0x6FF4B0  // NAME 2: Find_Object compares it with a name
	bool Unknown_6FFE90();								// 0x6FFE90 (co.cpp)
	int Unknown_6FFF00();								// 0x6FFF00 (co.cpp)
	int Unknown_6FFF50();								// 0x6FFF50 (co.cpp)
	bool Unknown_700400();								// 0x700400 (co.cpp)
	RenderObjClass *Unknown_6F3BC0();					// 0x6F3BC0 (co.cpp; cpacket.cpp's DECAL 16 releases the model it returns)
	bool Unknown_6F5DC0();								// 0x6F5DC0 (co.cpp; cpacket.cpp's SET_TARGET 25 tests it)
	void Unknown_6FEF70(SubPartsPacket *packet);		// 0x6FEF70 (co.cpp; cpacket.cpp's SUBPARTS 180)  // NAME 1 [packet]: the handler's packet
	void Unknown_70E750(int id);						// 0x70E750 (cpacket.cpp's SET_TARGET 25 passes its TargetID)  // NAME 1 [id]: the packet's TargetID
	float Unknown_6F3ED0();								// 0x6F3ED0 an angle from +0x98's slot +0x14, clamped to 36 degrees
	float Unknown_6F4290();								// 0x6F4290 (not rebuilt)
	void Unknown_6F3BF0(bool hidden);					// 0x6F3BF0 (the model's Set_Hidden)  // NAME 2 [hidden]: Set_Hidden's argument
	float Unknown_6F45E0();								// 0x6F45E0
	float Unknown_6F4590();								// 0x6F4590
	bool Unknown_6F5850();								// 0x6F5850 the type tests on Type, 0x6F5850-0x6F5E20
	bool Unknown_6F5980();
	bool Unknown_6F5A10();
	bool Unknown_6F5A40();
	bool Unknown_6F5A60();
	bool Unknown_6F5A80();
	bool Unknown_6F5AB0();
	bool Unknown_6F5AD0();
	bool Unknown_6F5AF0();
	bool Unknown_6F5B10();
	bool Unknown_6F5B30();
	bool Unknown_6F5B50();
	bool Unknown_6F5B70();
	bool Unknown_6F5B90();
	bool Unknown_6F5BB0();
	bool Unknown_6F5BD0();
	bool Unknown_6F5BF0();
	bool Unknown_6F5C20();
	bool Unknown_6F5C50();
	bool Unknown_6F5C80();
	bool Unknown_6F5CA0();
	bool Unknown_6F5CD0();
	bool Unknown_6F5D00();
	bool Unknown_6F5D30();
	bool Unknown_6F5D50();
	bool Unknown_6F5D70();
	bool Unknown_6F5D90();
	int Unknown_6F5DE0();
	int Unknown_6F5E00();
	int Unknown_6F5E20();
	void Unknown_6FC950(bool value);					// 0x6FC950  // NAME 1 [value]: neutral
	bool Unknown_6FC970();								// 0x6FC970
	void Unknown_6FED30();								// 0x6FED30 the bounding sphere from the model
	int Unknown_6FFFA0();								// 0x6FFFA0 (client.cpp's 0x7283B0 jumps to it)
	void Unknown_700820(const Vector3 &position);		// 0x700820  // NAME 1 [position]: a Vector3
	bool Unknown_700860();								// 0x700860
	void Unknown_6FC9A0();								// 0x6FC9A0 the aux data's changed values into the model
	void Unknown_6FCB20(bool visible);					// 0x6FCB20  // NAME 2 [visible]: what it toggles
	void Unknown_6FD2C0();								// 0x6FD2C0 (FAIL_IF '!RenderObject', l.1779)
	void Unknown_6FD310();								// 0x6FD310 (co.cpp, not rebuilt: the damage decals)
	void Unknown_6FDBD0();								// 0x6FDBD0 (co.cpp, not rebuilt: the engine sounds)
	void Unknown_6FE1E0();								// 0x6FE1E0 (co.cpp, not rebuilt: the damage state)
	void Unknown_7009A0(bool on, bool force);			// 0x7009A0  // NAME 2 [on, force]: what it tests
	void Unknown_6FEC40(SphereClass *sphere);			// 0x6FEC40 (FAIL_IF '!sphere', l.2232): the bounding sphere in slot +0x28's frame
	void Unknown_6FEEE0(const RelationshipClass &relationship);	// 0x6FEEE0: sets +0x12C and emits PlayerObjectRelationshipChangeEvent
	Matrix3D Unknown_6FED80(int unknown);				// 0x6FED80 (G_ASSERT 'RenderObject', l.2259): +0xB4's or the model's transform  // NAME 1 [unknown]: neutral

	char Unknown04[0x60 - 0x04];						// +0x04
	HashNodeClass<ClientObjectClass *> Node;			// +0x60 ClientClass::ObjectList's node  // NAME 2: what it is
	ClientClass *Client;								// +0x7C the constructor's first argument (CREATE passes its SClient; co_data.cpp calls its slot +0x18)  // NAME 2: what it holds
	char Unknown80[0x88 - 0x80];						// +0x80
	char *AuxData;										// +0x88 (co.cpp's FAIL_IF '!aux_data', l.2461)  // NAME 3: the FAIL_IF's name
	int Unknown8C;										// +0x8C
	unsigned GameID;									// +0x90  // NAME 3: Get_Game_ID's
	char Type;											// +0x94 the constructor's second argument, CREATE's object type (the factory 0x6F6000 switches on it)  // NAME 2: what it holds
	char Unknown95[0x98 - 0x95];						// +0x95
	Unknown98Class *Unknown98;							// +0x98 (the constructor's new)
	float Radius;										// +0x9C 1.0f (slot +0x34 0x6F3BA0 sets it from 0x6F39F0's radius; 0x6FED30's sphere radius without a model)  // NAME 2: what it holds
	float RadiusAdjust;									// +0xA0 (0x6F39F0: cbasset.ini's RadiusAdjust)  // NAME 3: the INI key
	float Scale;										// +0xA4 1.0f (co.cpp's FAIL_IF 'Scale <= 0.0f', l.331)
	int UnknownA8;										// +0xA8 -1 (0x6F39F0's 'BASE-%d' number)
	RenderObjClass *Model;								// +0xAC (cpacket.cpp's render-state handlers: Class_ID, then rs_track.cpp's methods; co.cpp's G_ASSERT 'RenderObject != 0', l.361, names it RenderObject)  // NAME 2: what it holds
	bool UnknownB0;										// +0xB0 (0x6FCB20 toggles it)
	char UnknownB1[0xB4 - 0xB1];						// +0xB1
	UnknownB4Class *UnknownB4;							// +0xB4 a RefCountPtr in the client (0x731FA0's result)
	char UnknownB8[0xE8 - 0xB8];						// +0xB8
	float Sphere[4];									// +0xE8 a SphereClass (center, radius): 0x6FED30 fills it from the model  // NAME 2: what it holds
	int UnknownF8;										// +0xF8
	char UnknownFC[0x128 - 0xFC];						// +0xFC
	bool Unknown128;									// +0x128 (0x6FC950 sets it)
	char Unknown129[0x12C - 0x129];						// +0x129
	int Relationship[2];								// +0x12C a RelationshipClass (pk_player_object_relationship.h; the constructor's 0x848C80): 0x6FEEE0 sets it  // NAME 2: what it holds
	char Unknown134[0x174 - 0x134];						// +0x134
	float Unknown174;									// +0x174 (0x700820 sets 2.0f)
	float Unknown178[3];								// +0x178 a Vector3 (0x700820)
	bool Unknown184;									// +0x184 (the constructor's 1; 0x7009A0's state)
	char Unknown185[0x188 - 0x185];						// +0x185 (CREATE allocates 0x188 bytes)
};

#endif //__CO_H
