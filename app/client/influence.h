// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\client\influence.h (name assumed from influence.cpp)
**
**	influence.cpp's records of ClientClass's third list (+0x86C, keyed by +0x24): the base
**	(client.h's Unknown86CRecordClass, vtable 0xB07730), the influence that follows an object's
**	model or one of its sub-objects (vtable 0xB07748, 0x50 bytes) and the nebula CREATE 4 makes
**	for a type 10 object (vtable 0xB07778, 0xB8 bytes): its rooms in an AABTree, the fog of the
**	room the camera is in or of the space between them (nebula.ini).
*/



#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __INFLUENCE_H  // NAME 3 [influence.h]: header name assumed from the client's influence.cpp
#define __INFLUENCE_H

#include "client.h"
#include "sphere.h"
#include "aabtreecull.h"
#include "rendobj.h"
#include "mesh.h"


// The influence that follows an object (vtable 0xB07748, 0x50 bytes; constructor 0x7E82E0)
class UnknownObjectInfluenceClass : public Unknown86CRecordClass	// NAME 1: neutral (D10)
{
public:
	UnknownObjectInfluenceClass(ClientClass *client, unsigned id);	// 0x7E82E0  // NAME 1 [client, id]: the base's
	virtual ~UnknownObjectInfluenceClass();				// +0x00 0x7E83C0
	virtual void Unknown_04(int arg);					// +0x04 0x7E8520 (the object's node goes: the object is forgotten)  // NAME 1 [arg]: the base's
	virtual bool Unknown_08(const Vector3 &position);	// +0x08 0x7E8480 (the sphere again; true)  // NAME 1 [position]: the base's
	virtual UnknownObjectInfluenceClass *Unknown_0C();	// +0x0C 0x7E8370 (itself)
	virtual void Unknown_10(int game_id);				// +0x10 0x7E8550 (CREATE passes the packet's GameID: the object to follow)  // NAME 1 [game_id]: what CREATE passes
	virtual void Set_Sphere(const SphereClass &sphere);	// +0x14 0x7E8620  // NAME 1: what it does
	virtual void Set_Sub_Object_Name(const char *name);	// +0x18 0x7E8650 (nstrdup; then the sphere of that sub-object)  // NAME 1: what it does
	void Update_Sphere();								// 0x7E8720 (the object's model's bounding sphere, or its named sub-object's)  // NAME 1: what it does

	ClientObjectClass *Object;							// +0x28  // NAME 1: what it holds
	DataNode<UnknownAF0DD0Class *> ObjectNode;			// +0x2C (vtable 0xAF0DDC; constructor 0x7E9910)  // NAME 1: eff_o.h's watch nodes'
	SphereClass Sphere;									// +0x3C  // NAME 1: what it holds
	char *SubObjectName;								// +0x4C  // NAME 1: what it holds
};

// One room of a nebula (vtable 0xB0776C, 0x34 bytes; destructor 0x7E9260)
class RoomClass : public CullableClass	// NAME 2: influence.cpp's assert 'TempRoomPtrs[bidx]->BoundaryMesh == NULL' and 'Room%02d'
{
public:
	RoomClass() : Model(NULL), BoundaryMesh(NULL), Inverted(false) {}
	~RoomClass() { REF_PTR_RELEASE(Model); REF_PTR_RELEASE(BoundaryMesh); }
	bool Contains(const Vector3 &point) { return(BoundaryMesh->Contains(point) ^ Inverted); }

	RenderObjClass *Model;								// +0x28 (a collection named 'Room%02d', an HLod once it has additional models)  // NAME 1: what it holds
	MeshClass *BoundaryMesh;							// +0x2C  // NAME 3: the assert's
	bool Inverted;										// +0x30 (the first room's: the camera is in it while outside its mesh)  // NAME 1: what it does
};

// The nebula (vtable 0xB07778, 0xB8 bytes; constructor 0x7E87C0, destructor 0x7E9380)
class NebulaClass : public UnknownObjectInfluenceClass	// NAME 2: its strings 'NEBULA_ROOT' and 'nebula.ini'
{
public:
	NebulaClass(ClientClass *client, unsigned id, RenderObjClass *model);	// 0x7E87C0 (CREATE 4: its client, the client's +0x113C decremented, the new model)  // NAME 1 [client, id, model]: what CREATE passes
	virtual ~NebulaClass();								// +0x00 0x7E9380
	virtual bool Unknown_08(const Vector3 &position);	// +0x08 0x7E9560 (the room the camera is in: its fog and the sky box)  // NAME 1 [position]: what it gets

	TypedAABTreeCullSystemClass<RoomClass> RoomTree;	// +0x50 (vtable 0xB0779C)  // NAME 1: what it holds
	RoomClass *CurrentRoom;								// +0x78  // NAME 1: what it holds
	RoomClass *DefaultRoom;								// +0x7C (room 0, Inverted)  // NAME 1: what it holds
	bool Initialized;									// +0x80 (the rooms placed on the object's model once)  // NAME 1: what it does
	RenderObjClass *SkyBox;								// +0x84 (base asset 0x137; the main view's 0x513600)  // NAME 2: nebula.ini's 'SkyBoxColor'
	Vector3 RoomFogColor;								// +0x88 (nebula.ini [Room] FogColorR / G / B)  // NAME 2: its keys'
	float RoomFogNear;									// +0x94 ([Room] FogNearRange)  // NAME 2: its key's
	float RoomFogFar;									// +0x98 ([Room] FogFarRange)  // NAME 2: its key's
	Vector3 BetweenFogColor;							// +0x9C ([Between] FogColorR / G / B)  // NAME 2: its keys'
	float BetweenFogNear;								// +0xA8 ([Between] FogNearRange)  // NAME 2: its key's
	float BetweenFogFar;								// +0xAC ([Between] FogFarRange)  // NAME 2: its key's
	RoomClass **TempRoomPtrs;							// +0xB0  // NAME 3: the asserts'
	int RoomCount;										// +0xB4 (the model's bones)  // NAME 1: what it holds
};

#endif //__INFLUENCE_H
