/*
**	Earth & Beyond client (reconstruction)
**	Original location: unknown. The run 0x70DBC0-0x70F030 (after co_player_relationship.cpp, before extrap.cpp)
**	has no assert and no __FILE__, so the file name is provisional (D122): app\client\co_subtypes.cpp.
**	CREATE 4's two object subclasses (type 24 0x70E2E0, type 28 0x70E5A0) and type 24's base 0x70DBC0, which adds
**	dynamic_obj.cpp's DynObj to ClientObjectClass. Notes at the end of co_subtypes.cpp.
*/
#if defined(_MSC_VER)
#pragma once
#endif
#ifndef CO_SUBTYPES_H  // NAME 1 [co_subtypes.h]: provisional file name (D122)
#define CO_SUBTYPES_H

#include "co.h"
#include "..\shared\dynamic_obj.h"

// type 24's base (constructor 0x70DBC0, vtables 0xB02084 / 0xB0206C / 0xB020E4; 0x20C bytes): a ClientObjectClass that
// is also a proximity object, with two transforms indexed by the frame toggle 0xBE350C
class Unknown70DBC0Class : public ClientObjectClass, public DynObj	// NAME 1: neutral (D10); its Get_Name text is 'Unnamed ClientProximityObject' (0x70DD20)
{
public:
	Unknown70DBC0Class(ClientClass *client, int type, unsigned id);	// 0x70DBC0  // NAME 1 [client, type, id]: as the base's
	virtual ~Unknown70DBC0Class();						// +0x00 0x70E0C0 (scalar deleting 0x70E090; thunks 0x70E2A0, 0x70E2B0)
	virtual void Unknown_20();							// +0x20 0x70E150 (this frame's transform from slot +0x4C, then the base's update)
	virtual Vector3 Unknown_24();						// +0x24 0x70DD40 (the base's; also the proximity object's +0x34, thunk 0x70E2C0)
	virtual Vector3 Unknown_2C();						// +0x2C 0x70DDE0 (the base's; also the proximity object's +0x40, thunk 0x70E2D0)
	virtual float Unknown_30();							// +0x30 0x70DCD0 (Radius)
	virtual void Unknown_34(float value);				// +0x34 0x70DCF0 (the base's, and the proximity object's +0x04 member)  // NAME 1 [value]: as the base's
	virtual int Unknown_40();							// +0x40 0x70DCB0 (this)
	virtual Vector3 Unknown_48();						// +0x48 0x70DD70 (slot +0x4C's x axis)
	virtual Matrix3D Unknown_4C();						// +0x4C 0x70DDB0 (the base's slot +0x28 with thrust)

	virtual DynObj *Unknown_Dyn04();		// +0x04 0x70DC90 (this)
	virtual unsigned Unknown_Dyn1C();					// +0x1C 0x70E1F0 (the client's +0x10BC / 100)
	virtual int Unknown_Dyn20();						// +0x20 0x70E220 (Type)
	virtual const char *Unknown_Dyn30();				// +0x30 0x70DD20 ('Unnamed ClientProximityObject')
	virtual float Unknown_Dyn44();						// +0x44 0x70DE10 (slot +0x2C's length)
	virtual Vector3 Unknown_Dyn48();					// +0x48 0x70DE80 (the other frame's translation)
	virtual Vector3 Unknown_Dyn4C();					// +0x4C 0x70DEC0 (the other frame's x axis)
	virtual Matrix3D Unknown_Dyn50();					// +0x50 0x70DF00 (the other frame's transform)
	virtual Vector3 Unknown_Dyn54();					// +0x54 0x70DF90 (the other frame's x axis, as +0x4C)
	virtual float Unknown_Dyn58();						// +0x58 0x70DFD0 (the length of the other frame's x axis)
	virtual void Unknown_Dyn64();						// +0x64 0x70E240 (slot +0x68 with slot +0x48)
	virtual Vector3 Unknown_Dyn70();					// +0x70 0x70E040 (slot +0x4C)
	virtual void Unknown_Dyn74(int a, int b, int c, int d);	// +0x74 0x70E070 (empty, ret 0x10)  // NAME 1 [a, b, c, d]: as the base's

	Matrix3D Unknown1AC[2];								// +0x1AC by 0xBE350C (slot +0x20 writes this frame's, the proximity slots read the other)
};

// type 24 (constructor 0x70E2E0, vtables 0xB02294 / 0xB0227C / 0xB021B0, 0x224 bytes)
class Unknown70E2E0Class : public Unknown70DBC0Class	// NAME 1: neutral (D10)
{
public:
	Unknown70E2E0Class(ClientClass *client, int type, unsigned id);	// 0x70E2E0 (CREATE 4 passes its SClient, the packet's Type and GameID)  // NAME 1 [client, type, id]: as the base's
	virtual ~Unknown70E2E0Class();						// +0x00 0x70E380 (scalar deleting 0x70E350; thunks 0x70E580, 0x70E590)
	virtual void Unknown_20();							// +0x20 0x70E4E0 (the base's, then the height above the ground)
	virtual void Unknown_3C();							// +0x3C 0x70E530 (empty)
	virtual int Unknown_44();							// +0x44 0x70E330 (this)
	virtual void Unknown_Dyn68(Vector3 pos);			// +0x68 0x70E3F0 (the base's, then the height above the ground)  // NAME 1 [pos]: as the base's
	virtual void Unknown_Dyn6C(const Matrix3D &tm);		// +0x6C 0x70E460 (the base's, then the height above the ground)  // NAME 1 [tm]: as the base's
	void Unknown_70E3B0(const Vector3 &pos);			// 0x70E3B0 (no caller: the height above the ground at pos)  // NAME 1 [pos]: what it reads

	float Unknown20C;									// +0x20C the height above the land's height field
	char Unknown210[0x220 - 0x210];						// +0x210 (no function of the run touches it)
	bool Unknown220;									// +0x220 false
	char Unknown221[0x224 - 0x221];						// +0x221 (CREATE allocates 0x224 bytes)
};

// type 28 (constructor 0x70E5A0, vtables 0xB0230C / 0xB022F4, 0x1D4 bytes, over 0x6F2820): attached to a parent object,
// it turns the parent model's bone towards its target
class Unknown70E5A0Class : public ClientObjectClass	// NAME 1: neutral (D10)
{
public:
	Unknown70E5A0Class(ClientClass *client, unsigned id);	// 0x70E5A0 (CREATE 4 passes its SClient and the packet's GameID)  // NAME 1 [client, id]: as the base's
	virtual ~Unknown70E5A0Class();						// +0x00 0x70E6B0 (scalar deleting 0x70E680; thunk 0x70F030): releases the parent's bone
	virtual void Unknown_14(UnknownB02C90Class *parent, int slot, int child_slot);	// +0x14 0x70E780 (attached: the parent's id, its slot's bone, captured)  // NAME 1 [parent, slot, child_slot]: as the base's
	virtual void Unknown_18(UnknownB02C90Class *parent, int slot, int child_slot);	// +0x18 0x70E7D0 (detached: the bone released)  // NAME 1 [parent, slot, child_slot]: as the base's
	virtual void Unknown_20();							// +0x20 0x70E850 (turns the parent's bone towards the target)
	void Unknown_70E750(int id);						// 0x70E750 (cpacket.cpp's SET_TARGET 25 passes its TargetID)  // NAME 1 [id]: the packet's TargetID

	int TargetID;										// +0x188 -1 (0x70E750: SET_TARGET's TargetID)  // NAME 2: what it holds
	int ParentID;										// +0x18C -1 (0x70E780: the parent's GameID)  // NAME 2: what it holds
	Matrix3D Unknown190;								// +0x190 identity (0x70E850: the aim's last rotation)
	int Bone;											// +0x1C0 -1 (0x70E780: the parent's bone for the attachment slot)  // NAME 2: what it holds
	Vector3 Unknown1C4;									// +0x1C4 (1, 0, 0) (0x70E850: the target's position while +0x1D0 is 1)
	float Unknown1D0;									// +0x1D0 1.0f (0x70E750 clears it)
};

#endif //CO_SUBTYPES_H
