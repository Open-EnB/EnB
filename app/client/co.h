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
**	same class. Its bases: client.cpp's UnknownB02C90Class (declared here, moved from client.h, so
**	that co.h stands alone) and ww3d2's MultiFixedPoolDecalSystemClass at +0x4C. Names: D10, D24, D56.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __CO_H  // NAME 3 [co.h]: header name assumed from the client's co.cpp
#define __CO_H

#include "hashlist.h"
#include "refcount.h"
#include <string>
#include <vector>
#include "vector3.h"
#include "matrix3d.h"
#include "decalsys.h"
#include "..\packets\pkcolorization.h"
#include "..\packets\pk_player_object_relationship.h"
#include "..\common\lego_identifier.h"
#include "ref_ptr.h"
#include "sphere.h"
#include "..\shared\unknown_debug.h"

class RenderObjClass;
class AudibleSoundClass;

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
	virtual void Unknown_20(int base_asset, const RefCountPtr<RenderObjClass> &model);	// +0x20 (Set_Model 0x6F33F0: the new BaseAsset and model)  // NAME 2 [base_asset, model]: what Set_Model passes
	virtual void Unknown_24();
	virtual void Unknown_28();
	virtual Matrix3D Unknown_2C(int unknown);		// +0x2C (0x6FED80 returns it when the object is set)  // NAME 1 [unknown]: neutral
	void Unknown_732690();							// 0x732690 (co.cpp's destructor calls it before the release)
};

// The +0xB4 object's source (cdecl; co.cpp's constructor assigns its result)
class ClientObjectClass;
RefCountPtr<UnknownB4Class> Unknown_731FA0(ClientObjectClass *object);	// 0x731FA0  // NAME 1 [object]: neutral

// co.cpp's +0x134 object (16 bytes: a flag and three floats; constructor 0x498C10, its setter 0x498F00 sets
// +0x0C, +0x04 and +0x08 and the flag when the first two differ)
class Unknown498C10Class							// NAME 1: neutral (D10)
{
public:
	Unknown498C10Class();							// 0x498C10
	void Unknown_498F00(float a, float b, float c);	// 0x498F00  // NAME 1 [a, b, c]: neutral
	void Unknown_498CC0(float value);				// 0x498CC0 (co.cpp's tilt 0x6F3FA0 passes the turn's acceleration)  // NAME 1 [value]: neutral
	float Unknown_498E00(float time);				// 0x498E00 (0x6F3FA0 passes WW3D's frame time when the flag is set)  // NAME 1 [time]: what it gets
	bool Unknown00;									// +0x00
	float Unknown04[3];								// +0x04
};

// co.cpp's +0x8C object (the destructor deletes it through its slot +0x00)
class Unknown8CClass								// NAME 1: neutral (D10)
{
public:
	virtual ~Unknown8CClass();
};

// co.cpp's +0x98 object, the object's positional state (postate.cpp's states derive from it): vtable
// 0xB00768, its slots +0x04-+0x14 pure (_purecall), +0x18-+0x2C inline (0x6F2D40-0x6F2E10, emitted with
// co.cpp's default state); ADVANCED_POSITIONAL_UPDATE 62 asks a state's type (+0x04), whether it holds
// a second state (+0x2C), that state (+0x28), and replaces it (+0x24)
class Unknown98Class							// NAME 1: neutral (D10)
{
public:
	virtual ~Unknown98Class() {}				// 0x6F2FB0 (scalar deleting), 0x6F2F90
	virtual int Unknown_04() = 0;				// +0x04 the state's type
	virtual Matrix3D Unknown_08(int unknown) = 0;	// NAME 1 [unknown]: neutral
	virtual Vector3 Unknown_0C(int unknown) = 0;	// NAME 1 [unknown]: neutral
	virtual Vector3 Unknown_10(int unknown) = 0;	// +0x10 0x6F5050 passes the client's +0x10BC  // NAME 1 [unknown]: neutral
	virtual Vector3 Unknown_14(int unknown) = 0;	// +0x14 0x6F3ED0 passes the client's +0x10BC  // NAME 1 [unknown]: neutral
	virtual Vector3 Unknown_18(int unknown) { return(Vector3(0.0f, 0.0f, 0.0f)); }	// +0x18 0x6F2D40  // NAME 1 [unknown]: neutral
	virtual Vector3 Unknown_1C(int unknown) { return(Unknown_0C(unknown)); }		// +0x1C 0x6F2D70  // NAME 1 [unknown]: neutral
	virtual Matrix3D Unknown_20(int unknown) { return(Unknown_08(unknown)); }		// +0x20 0x6F2DA0  // NAME 1 [unknown]: neutral
	virtual void Unknown_24(Unknown98Class *state) {}							// +0x24 0x6F2DD0  // NAME 1 [state]: neutral
	virtual Unknown98Class *Unknown_28() { return(0); }						// +0x28 0x6F2DF0
	virtual bool Unknown_2C() { return(false); }								// +0x2C 0x6F2E10
};

// The default state co.cpp's constructor news (4 bytes, vtable 0xB006B8): type 0, no motion
class UnknownB006B8Class : public Unknown98Class	// NAME 1: neutral (D10), named by its vtable
{
public:
	virtual int Unknown_04() { return(0); }										// +0x04 0x6F2E30
	virtual Matrix3D Unknown_08(int unknown) { return(Matrix3D(true)); }		// +0x08 0x6F2E50  // NAME 1 [unknown]: neutral
	virtual Vector3 Unknown_0C(int unknown) { return(Vector3(0.0f, 0.0f, 0.0f)); }	// +0x0C 0x6F2EA0  // NAME 1 [unknown]: neutral
	virtual Vector3 Unknown_10(int unknown) { return(Vector3(0.0f, 0.0f, 0.0f)); }	// +0x10 0x6F2ED0  // NAME 1 [unknown]: neutral
	virtual Vector3 Unknown_14(int unknown) { return(Vector3(0.0f, 0.0f, 0.0f)); }	// +0x14 0x6F2F00  // NAME 1 [unknown]: neutral
};

void Unknown_810540(RenderObjClass *model, int state, int unknown);	// shiputils.cpp (cdecl)  // NAME 1 [model, state, unknown]: co.cpp passes the model, 1 or 2, and 0
void Unknown_810750(RenderObjClass *model, bool on);	// (cdecl; co.cpp's 0x7008B0 passes the model and 0; the engine sounds 0x6FDBD0 push a bool's whole dword)  // NAME 1 [model, on]: what co.cpp passes
class SubPartsPacket;
class ClientClass;

// The base of co.cpp's object (0x4C bytes, vtable 0xB02C90; co.cpp's constructor 0x6F2820 calls its constructor
// with the object itself): the client's code for it sits before ClientClass's in client.cpp's region
// (0x725050-0x7260FF): the slots +0x04 .. +0x10 0x7251C0 (NOT EXACT), 0x725120, 0x725610, 0x7257F0.
class UnknownB02C90Class							// NAME 1: neutral (D10), named by its vtable
{
public:
	UnknownB02C90Class(void *owner);					// 0x725050  // NAME 1 [owner]: co.cpp passes the object itself
	virtual ~UnknownB02C90Class();						// 0x7250F0 (scalar deleting 0x7250C0)
	virtual void Unknown_04(int base_asset);			// +0x04 0x7251C0 the mount points of cbasset.ini's BASE-n section (CREATE 4 passes the packet's BaseAsset)  // NAME 2 [base_asset]: what CREATE passes
	virtual void Unknown_08();							// +0x08 0x725120 detaches every child: Unknown_10 and the client's Remove_Object, then deletes it
	virtual bool Unknown_0C(UnknownB02C90Class *child, int slot, int child_slot);	// +0x0C 0x725610 attaches child's element child_slot to this one's slot  // NAME 1 [child, slot, child_slot]: what it does with them
	virtual bool Unknown_10(UnknownB02C90Class *child, int slot, int child_slot);	// +0x10 0x7257F0 detaches child (a slot of -1: the one that holds it)  // NAME 1 [child, slot, child_slot]: what it does with them
	virtual void Unknown_14(UnknownB02C90Class *parent, int slot, int child_slot) {}	// +0x14 0x6F2C50 (empty; 0x725610 calls the child's)  // NAME 1 [parent, slot, child_slot]: what 0x725610 passes
	float Unknown_725C60(const Vector3 &center, float radius);	// 0x725C60 the radius around center that holds the attached objects of type 29, recursively  // NAME 1 [center, radius]: what it computes
	virtual void Unknown_18(UnknownB02C90Class *parent, int slot, int child_slot) {}	// +0x18 0x6F2C70 (empty; 0x7257F0 calls the child's)  // NAME 1 [parent, slot, child_slot]: what 0x7257F0 passes

	void *Unknown04;									// +0x04 the constructor's argument
	void *Unknown08;									// +0x08 the constructor's argument
	// +0x0C .. +0x18: a buffer with its own inline destructor (clears the count, deletes the data): written as
	// fields in the base's destructor, the client's vtable store moves first and 0x7250C0 calls 0x7250F0
	struct BufferStruct {								// NAME 1: neutral (D10)
		struct ElementStruct {							// 0x44 bytes  // NAME 1: neutral (D10)
			int Bone;									// +0x00 (0x725610 hangs the child's model on this bone)  // NAME 2: what it holds
			Matrix3D Transform;							// +0x04 (0x725610: the child's +0x1C is it times this one's)  // NAME 2: what it holds
			bool Unknown34;							// +0x34 (0x7251C0: a Mount line's third field, true by default)
			int Unknown38;							// +0x38 (0x7251C0: a Mount line's fourth field)
			UnknownB02C90Class *Unknown3C;				// +0x3C (0x725120 calls its slot +0x10 with -1, -1 when there is no child)
			UnknownB02C90Class *Child;					// +0x40  // NAME 2: 0x725120 detaches and deletes it
			ElementStruct() : Bone(0), Transform(true), Unknown38(0), Unknown3C(0), Child(0) { Unknown34 = true; }
		};
		BufferStruct() : Count(0), Unknown04(0), Data(0), Radius(0) {}
		void Add(const ElementStruct &element) { Insert(element, Count); }	// NAME 2: what it does
		void Insert(const ElementStruct &element, int index);	// 0x725E70 (grows by twice the count, at least 10; not rebuilt)  // NAME 2 [element, index]: what it does
		~BufferStruct() { Count = 0; delete Data; }
		ElementStruct *Get(int index) { if (index < 0 || index >= Count) return(NULL); return(&Data[index]); }  // NAME 1: neutral
		int Count;										// +0x0C (+0x00) cleared by the destructor  // NAME 1: neutral
		int Unknown04;									// +0x10 (+0x04)
		ElementStruct *Data;							// +0x14 (+0x08) deleted by the destructor  // NAME 1: neutral
		float Radius;									// +0x18 (+0x0C) (0x725C60's starting radius)  // NAME 2: what it holds
	} Buffer;											// +0x0C
	Matrix3D Unknown1C;									// +0x1C identity
};


// The object's own vtable 0xB0070C (slots +0x04-+0x18 the base's: ClientClass's destructor calls +0x08 before
// the delete, CREATE 4 calls +0x04 with the BaseAsset, CREATE_ATTACHMENT 74 calls +0x0C), the decal system's
// 0xB006F4 at +0x4C (its slot +0x00 the thunk 0x709E90, +0x10 the inline Generate_Decal_Id 0x6F2C90)
class ClientObjectClass : public UnknownB02C90Class, public MultiFixedPoolDecalSystemClass		// NAME 3: the RTTI's .?AVClientObjectClass@@ for co.cpp's class, not tied to this layout by a vtable
{
public:
	ClientObjectClass(ClientClass *client, int type, unsigned id);	// 0x6F2820 (CREATE 4 passes its SClient, the packet's Type and GameID)  // NAME 2 [client, type, id]: what CREATE passes
	virtual ~ClientObjectClass();						// +0x00 0x6F2FE0 (scalar deleting 0x6F2F30; ClientClass's destructor deletes through it)
	virtual void Unknown_1C();							// +0x1C 0x6F5120 (the aux data's slot +0x20)
	virtual void Unknown_20();							// +0x20 0x6F3C60 (FAIL_IF '!RenderObject' l.1779 inline: 0x6FD2C0)
	virtual Vector3 Unknown_24();						// +0x24 0x6F4C20: +0x98's slot +0x0C clamped to the client's bounds (G_ASSERT 'pos.Is_Valid()', l.762)
	virtual Matrix3D Unknown_28(bool thrust);			// +0x28 0x6F4610 the state's transform at the client clock (0x6FEC40 transforms the sphere by it; the per-frame update passes true: the engine-thrust tilt 0x6F3FA0 and the thrust models)  // NAME 2 [thrust]: what true adds
	virtual Vector3 Unknown_2C();						// +0x2C 0x6F5050
	virtual float Unknown_30() { return(Radius); }		// +0x30 0x6F2CC0
	virtual void Unknown_34(float value);				// +0x34 0x6F3BA0  // NAME 1 [value]: neutral
	virtual void Unknown_38();							// +0x38 0x6F39F0: the radius from cbasset.ini's BASE-n section (FAIL_IF l.331 / 336)
	virtual void Unknown_3C() {}						// +0x3C 0x6F2CE0 (empty)
	virtual int Unknown_40() { return(0); }				// +0x40 0x6F2D00
	virtual int Unknown_44() { return(0); }				// +0x44 0x6F2D20

	unsigned Get_Game_ID() { return(GameID); }			// (the assert's name)
	void Get_Name(std::string &name);					// 0x6FF4B0  // NAME 2: Find_Object compares it with a name
	void Unknown_6FFFF0(std::string &text);				// 0x6FFFF0 (co.cpp; '[guild]', or '[guild rank]' when the client's own guild is the same)  // NAME 2 [text]: what it receives
	void Unknown_6FF1D0();								// 0x6FF1D0 (co.cpp; the rs_track model's render state from the aux data's PercentFull)
	void Unknown_6FF330(std::string &text);				// 0x6FF330 (co.cpp; ' (%d)' with the aux data's CombatLevel)  // NAME 2 [text]: what it receives
	bool Unknown_6FFE90();								// 0x6FFE90 (co.cpp)
	int Unknown_6FFF00();								// 0x6FFF00 (co.cpp)
	int Unknown_6FFF50();								// 0x6FFF50 (co.cpp)
	bool Unknown_700400();								// 0x700400 (co.cpp)
	bool Unknown_6FFC80(std::string &text);				// 0x6FFC80 (co.cpp; 0x6FF7A0 when Owner, Name, Rank or Title changed)  // NAME 1 [text]: passed on to 0x6FF7A0
	void Unknown_6FF7A0(std::string &text);				// 0x6FF7A0 (co.cpp, not rebuilt)  // NAME 1 [text]: neutral
	RenderObjClass *Unknown_6F3BC0();					// 0x6F3BC0 (co.cpp; cpacket.cpp's DECAL 16 releases the model it returns)
	bool Unknown_6F5DC0();								// 0x6F5DC0 (co.cpp; cpacket.cpp's SET_TARGET 25 tests it)
	void Set_Decal_Name(std::string name) { DecalName = name; }	// 0x736140 (out of line in cpacket.cpp's object, after NAME_DECAL 178, its only caller)  // NAME 2: what it does
	void Unknown_6FEF70(SubPartsPacket *packet);		// 0x6FEF70 (co.cpp; cpacket.cpp's SUBPARTS 180)  // NAME 1 [packet]: the handler's packet
	void Unknown_70E750(int id);						// 0x70E750 (cpacket.cpp's SET_TARGET 25 passes its TargetID)  // NAME 1 [id]: the packet's TargetID
	void Unknown_6F3810(RenderObjClass *model);			// 0x6F3810 the +0x158 models on the model's bones, then its sub-objects'  // NAME 2 [model]: what it is
	float Unknown_6F3ED0();								// 0x6F3ED0 an angle from +0x98's slot +0x14, clamped to 36 degrees
	float Unknown_6F4290();								// 0x6F4290 (not rebuilt)
	void Unknown_6F3FA0(Matrix3D &tm);					// 0x6F3FA0 slot +0x28's roll: the turn +0x144 steered toward 0x6F3ED0's angle (types 0 and 1 that aren't IsOrganic)  // NAME 1 [tm]: a Matrix3D
	void Unknown_6FBEF0();								// 0x6FBEF0 (not rebuilt; the per-frame update calls it for types 0 and 1)
	void Unknown_6F3BF0(bool hidden);					// 0x6F3BF0 (the model's Set_Hidden)  // NAME 2 [hidden]: Set_Hidden's argument
	float Unknown_6F45E0();								// 0x6F45E0
	float Unknown_6F4590();								// 0x6F4590
	bool Unknown_6F58B0();								// 0x6F58B0 an organic type (0, 1, 25) that isn't IsOrganic
	bool Unknown_6F5E40();								// 0x6F5E40 the aux data's IsOrganic
	bool Unknown_6F5ED0();
	bool Unknown_6F5F60();								// 0x6F5F60 the aux data's IsInPVP field with +0x70 above 1
	bool Unknown_7008B0();								// 0x7008B0 0x810750 on the model of an organic object that isn't IsOrganic								// 0x6F5ED0 the aux data's IsInPVP
	bool Unknown_6F5850();								// 0x6F5850 the type tests on Type, 0x6F5850-0x6F5E20
	bool Unknown_6F5980();
	bool Unknown_6F5A10();
	bool Unknown_6F5A40();
	bool Unknown_6F5A60();
	bool Is_Component();								// 0x6F5A80 type 4 or 35  // NAME 4: cpacket.cpp's G_ASSERT text 'obj->Is_Component()' (0xB956F4)
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
	bool Unknown_700670(Vector3 &color);				// 0x700670 (FAIL_IF '!aux_data' l.2519): the aux data's colour for the same guild as the player's ship or another  // NAME 2 [color]: what it receives
	void Unknown_700820(const Vector3 &position);		// 0x700820  // NAME 1 [position]: a Vector3
	bool Unknown_700860();								// 0x700860
	void Unknown_6FC880();								// 0x6FC880 stops the two sounds, removes the effect, sets the model's state 2 for a type-1 object
	void Unknown_6FC9A0();								// 0x6FC9A0 the aux data's changed values into the model
	void Unknown_6FCB20(bool visible);					// 0x6FCB20  // NAME 2 [visible]: what it toggles
	void Unknown_6FD2C0();								// 0x6FD2C0 (FAIL_IF '!RenderObject', l.1779)
	void Unknown_6FD310();								// 0x6FD310 the damage decals: the aux data's DamageSpot / DamageLine / DamageBlotch items that changed into the three decal pools (NOT EXACT)
	void Unknown_6FDBD0();								// 0x6FDBD0 the engine sounds: the engine and thrust loops from EngineTrailType, the thrust state from EngineThrustState
	void Unknown_6FD7A0(RenderObjClass *model);			// 0x6FD7A0 (co.cpp, not rebuilt: the engine sounds call it for a type-1 object in view mode 1 when EngineTrailType changed)  // NAME 1 [model]: what 0x6FDBD0 passes
	void Unknown_6FE1E0();								// 0x6FE1E0 (co.cpp, not rebuilt: the damage state)
	void Unknown_7009A0(bool on, bool force);			// 0x7009A0  // NAME 2 [on, force]: what it tests
	void Unknown_6FEC40(SphereClass *sphere);			// 0x6FEC40 (FAIL_IF '!sphere', l.2232): the bounding sphere in slot +0x28's frame
	void Unknown_6FEEE0(const RelationshipClass &relationship);	// 0x6FEEE0: sets +0x12C and emits PlayerObjectRelationshipChangeEvent
	void Unknown_6F33F0(RenderObjClass *model, int base_asset, float scale);	// 0x6F33F0 (CREATE 4: the new model, the packet's BaseAsset and Scale; a NULL Null3DObjClass, -1 and 1.0f for type 10)  // NAME 2 [model, base_asset, scale]: what CREATE passes
	void Unknown_6FCB70();								// 0x6FCB70 (Set_Model calls it last)
	static RenderObjClass *Unknown_6F5140(int base_asset, ClientClass *client, float scale, float *hsv);	// 0x6F5140 (cdecl; CREATE 4 and 19 other callers: the model for a BaseAsset)  // NAME 2 [base_asset, client, scale, hsv]: what CREATE passes
	Matrix3D Unknown_6FED80(int unknown);				// 0x6FED80 (G_ASSERT 'RenderObject', l.2259): +0xB4's or the model's transform  // NAME 1 [unknown]: neutral

	HashNodeClass<ClientObjectClass *> Node;			// +0x60 ClientClass::ObjectList's node  // NAME 2: what it is
	ClientClass *Client;								// +0x7C the constructor's first argument (CREATE passes its SClient; co_data.cpp calls its slot +0x18)  // NAME 2: what it holds
	char Unknown80[0x88 - 0x80];						// +0x80
	char *AuxData;										// +0x88 (co.cpp's FAIL_IF '!aux_data', l.2461)  // NAME 3: the FAIL_IF's name
	Unknown8CClass *Unknown8C;							// +0x8C (the destructor deletes it)
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
	RefCountPtr<UnknownB4Class> UnknownB4;				// +0xB4 (the constructor assigns 0x731FA0's result)
	std::vector<LegoIdentifierClass> UnknownB8;			// +0xB8 (constructed with a count of 0)
	std::vector<ColorizationRegionStruct> Colorization;	// +0xC8 the model's colorization regions (cpacket.cpp's COLORIZATION 17 assigns them, 0x73F770 their operator=)  // NAME 2: what it holds
	std::string DecalName;								// +0xD8 (cpacket.cpp's NAME_DECAL 178 sets it through 0x736140)  // NAME 2: what NAME_DECAL stores in it
	SphereClass Sphere;									// +0xE8 (center, radius): 0x6FED30 fills it from the model  // NAME 2: what it holds
	int UnknownF8;										// +0xF8
	bool UnknownFC;										// +0xFC (0x7008B0 clears it after 0x810750 on the model)
	int Unknown100;										// +0x100 -1
	int Unknown104[5];									// +0x104 1 each (the constructor's body)
	AudibleSoundClass *Unknown118;						// +0x118 a sound 0x6FC880 stops (the destructor releases it)
	AudibleSoundClass *Unknown11C;						// +0x11C a sound 0x6FC880 stops (the destructor releases it)
	AudibleSoundClass *Unknown120;						// +0x120 a sound (the destructor releases it)
	AudibleSoundClass *Unknown124;						// +0x124 a sound (the destructor releases it)
	bool Unknown128;									// +0x128 (0x6FC950 sets it)
	char Unknown129[0x12C - 0x129];						// +0x129
	RelationshipClass Relationship;						// +0x12C (pk_player_object_relationship.h; constructor 0x848C80): 0x6FEEE0 sets it  // NAME 2: what it holds
	Unknown498C10Class Unknown134;						// +0x134 (the constructor's 0x498F00 with the size factors)
	float Unknown144;										// +0x144 (slot +0x28 compares it with +-0.157: which thrust models show)
	int Unknown148;										// +0x148 the render state 0x6FF1D0 last set (0 to 3 from PercentFull)
	unsigned Unknown14C;								// +0x14C an effect's id (0x6FC880 removes the effect through the client's Get_Effect)
	int Unknown150;										// +0x150
	int Unknown154;										// +0x154
	RefCountPtr<RenderObjClass> Unknown158[6];			// +0x158 the models 0x6F3810 hangs on the ~FL ~FR ~BL ~BR ~FL ~FR bones (the constructor's vector constructor 0x5464F0 / 0x4740B0)
	bool Unknown170;									// +0x170
	float Unknown174;									// +0x174 (0x700820 sets 2.0f)
	Vector3 Unknown178;									// +0x178 (0x700820)
	bool Unknown184;									// +0x184 (the constructor's 1; 0x7009A0's state)
	char Unknown185[0x188 - 0x185];						// +0x185 (CREATE allocates 0x188 bytes)
};

// CREATE 4's two subclasses, declared as far as CREATE calls them. Their constructors lie in the unattributed run
// 0x70CC90-0x70F1D0 (after co_player_relationship.cpp, before extrap.cpp; no file known), so they are declared here
// type 24's base (constructor 0x70DBC0, vtable 0xB02084, over 0x6F2820)
class Unknown70DBC0Class : public ClientObjectClass	// NAME 1: neutral (D10)
{
public:
	Unknown70DBC0Class(ClientClass *client, int type, unsigned id);	// 0x70DBC0  // NAME 1 [client, type, id]: as the base's
};
// type 24 (constructor 0x70E2E0, vtable 0xB02294, 0x224 bytes)
class Unknown70E2E0Class : public Unknown70DBC0Class	// NAME 1: neutral (D10)
{
public:
	Unknown70E2E0Class(ClientClass *client, int type, unsigned id);	// 0x70E2E0 (CREATE 4 passes its SClient, the packet's Type and GameID)  // NAME 1 [client, type, id]: as the base's

	char Unknown188[0x224 - 0x188];						// +0x188 (CREATE allocates 0x224 bytes; the split with 0x70DBC0's part is unknown)
};
// type 28 (constructor 0x70E5A0, vtable 0xB0230C, 0x1D4 bytes, over 0x6F2820)
class Unknown70E5A0Class : public ClientObjectClass	// NAME 1: neutral (D10)
{
public:
	Unknown70E5A0Class(ClientClass *client, unsigned id);	// 0x70E5A0 (CREATE 4 passes its SClient and the packet's GameID)  // NAME 1 [client, id]: as the base's

	char Unknown188[0x1D4 - 0x188];						// +0x188 (CREATE allocates 0x1D4 bytes)
};

#endif //__CO_H
