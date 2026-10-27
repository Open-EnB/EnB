/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Client\client_lego_impl.h (name assumed from
**	client_lego_impl.cpp, whose __FILE__ is 'e:\build\beta\G\app\Client\client_lego_impl.cpp')
**
**	A "lego" is one part of an assembled model (a ship's hull, its engines, its weapons...): a render
**	object hung on a bone ("knob") of its parent's render object, found by a LegoIdentifierClass
**	path. ClientLegoImplClass (0xA0 bytes, vtables 0xB02F1C / 0xB02F10) is the client's lego: its
**	children by bone name, the children it could not attach yet (orphans), its asset's bone map
**	(cbasset.ini's sections, one map per asset id, shared through _Global_Bone_Maps) and its
**	radius, the furthest child's reach. Declared as far as the rebuilt functions need.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __CLIENT_LEGO_IMPL_H  // NAME 3 [client_lego_impl.h]: header name assumed from the client's client_lego_impl.cpp
#define __CLIENT_LEGO_IMPL_H

#include <string>
#include <vector>
#include <hash_map.h>
#include "..\shared\unknown_debug.h"
#include "..\..\wwlib\refcount.h"
#include "..\..\wwlib\ref_ptr.h"
#include "..\..\wwmath\matrix3d.h"
#include "..\..\ww3d2\rendobj.h"
#include "..\common\lego_identifier.h"

class INIClass;
class LegoParentClass;

// A bone of an asset that a child lego can hang on (a "knob"): reference counted.
class LegoKnobClass : public RefCountClass  // NAME 1 [LegoKnobClass]: neutral (D10), the client's 'knob' locals
{
public:
	LegoKnobClass(std::string bone_name, int bone_index);			// 0x72DE40

	std::string BoneName;											// +0x08: what Add_Sub_Object_To_Bone gets  // NAME 2: what it holds
	int BoneIndex;													// +0x18: what Capture_Bone / Release_Bone get  // NAME 2: what it holds
};

// One asset's knobs by bone name (cbasset.ini), 0x24 bytes, vtable 0xB02FF4.
class LegoBoneMapClass : public RefCountClass  // NAME 1 [LegoBoneMapClass]: neutral (D10), the client's 'BoneMap' member
{
public:
	LegoBoneMapClass(int asset_id);									// 0x72D5F0: cbasset.ini's BASE-n section and its Mount sections
	virtual ~LegoBoneMapClass(void);								// 0x72DC10
	RefCountPtr<LegoKnobClass> Get_Knob(const std::string &bone_name);	// 0x72DDA0  // NAME 2: what it returns

	std::hash_map<std::string, RefCountPtr<LegoKnobClass> > Knobs;	// +0x08  // NAME 2: what it holds
	float Radius;													// +0x20: the asset's own radius  // NAME 2: what it holds
};

// A lego (the abstract base: vtable 0xB02F88, every slot but Delete_This pure).
class LegoClass : public RefCountClass  // NAME 1 [LegoClass]: neutral (D10), the client's 'lego' locals
{
public:
	virtual ~LegoClass(void) = 0;									// +0x04
	virtual const LegoIdentifierClass &Get_Identifier(void) const = 0;	// +0x08  // NAME 2: what it returns
	virtual float Get_Scale(void) const = 0;						// +0x0C  // NAME 2: what it returns
	virtual void Set_Scale(float scale) = 0;						// +0x10  // NAME 2: what it does
	virtual float Get_Radius(void) const = 0;						// +0x14  // NAME 2: what it returns
	virtual void Unknown_18(bool unknown) = 0;						// +0x18: Attach passes the parent's +0x65  // NAME 1: neutral (D10)
	virtual void Unknown_1C(void) = 0;								// +0x1C: recursive over the children  // NAME 1: neutral (D10)
	virtual void Init(int asset_id, const RefCountPtr<RenderObjClass> &render_object) = 0;	// +0x20  // NAME 2: what it does
	virtual RefCountPtr<RenderObjClass> Get_Render_Object(void) const = 0;	// +0x24  // NAME 2: what it returns
	virtual void Unknown_28(void) = 0;								// +0x28  // NAME 1: neutral (D10)
	virtual void Unknown_2C(void) = 0;								// +0x2C  // NAME 1: neutral (D10)
	virtual void Unknown_30(void) = 0;								// +0x30  // NAME 1: neutral (D10)
	virtual void Unknown_34(void) = 0;								// +0x34  // NAME 1: neutral (D10)
	virtual void Detach_All(std::vector<RefCountPtr<LegoClass> > &legos) = 0;	// +0x38  // NAME 2: what it does
	virtual void Unknown_3C(int unknown) = 0;						// +0x3C  // NAME 1: neutral (D10)
	virtual void Set_Parent(LegoParentClass *parent) = 0;			// +0x40: the client's assert 'parent'  // NAME 2: what it does
	virtual void Clear_Parent(void) = 0;							// +0x44  // NAME 2: what it does
	virtual void Notify_Parent(void) = 0;							// +0x48  // NAME 2: what it does
	virtual const Matrix3D &Get_Transform(void) const = 0;			// +0x4C  // NAME 2: what it returns
};

inline LegoClass::~LegoClass(void)
{
}

// The children of a lego that lost their knob, 0x18 bytes, vtable 0xB02FE8.
class LegoOrphanDataClass : public RefCountClass  // NAME 1 [LegoOrphanDataClass]: neutral (D10), the client's 'child_orphan_data' local
{
public:
	std::vector<RefCountPtr<LegoClass> > Legos;						// +0x08  // NAME 2: what it holds
};

// Where one identifier sits relative to another (lego_identifier.cpp's 0x844ED0, 0x3C bytes): the
// first bone below the base (Name) and the rest of the path. Declared from its users here.
struct LegoSplitStruct  // NAME 1 [LegoSplitStruct]: neutral (D10), the client's 'split' locals
{
	LegoSplitStruct(const LegoIdentifierClass &base, const LegoIdentifierClass &id);	// 0x844ED0
	bool Is_Invalid(void) const;									// 0x8451A0: State is neither 0 nor 1  // NAME 3: the client's asserts 'split.Is_Invalid()'

	int State;														// +0x00  // NAME 1: neutral (D10)
	LegoIdentifierClass Base;										// +0x04  // NAME 1: neutral (D10)
	std::string Name;												// +0x18: the child's bone name  // NAME 2: what it holds
	LegoIdentifierClass Rest;										// +0x28  // NAME 2: what it holds
};

// What a lego's parent is told (vtable 0xB02F7C, both slots pure).
class LegoParentClass  // NAME 1 [LegoParentClass]: neutral (D10), the client's 'Parent' member
{
public:
	virtual void Child_Moved(const LegoIdentifierClass &id) = 0;	// +0x00: Init tells the parent; re-controls the child's bone  // NAME 2: what it does
	virtual void Child_Changed(const LegoIdentifierClass &id) = 0;	// +0x04: a child's radius changed  // NAME 2: what it does
};

struct LegoChildStruct  // NAME 1 [LegoChildStruct]: neutral (D10): a child lego and the knob it hangs on
{
	LegoChildStruct(void) : Child(0), Knob(0) {}					// 0x72C040

	RefCountPtr<LegoClass> Child;									// +0x00: the client's asserts '!child'  // NAME 2: what it holds
	RefCountPtr<LegoKnobClass> Knob;								// +0x04: the client's asserts '!knob'  // NAME 2: what it holds
};

class ClientLegoImplClass : public LegoClass, public LegoParentClass  // NAME 3 [ClientLegoImplClass]: by convention (D24) from the client's client_lego_impl.cpp
{
public:
	ClientLegoImplClass(const LegoIdentifierClass &id, int unknown);	// 0x729FC0  // NAME 1 [unknown]: neutral (D10)
	virtual ~ClientLegoImplClass(void);								// 0x72A340

	static void Init_Bone_Maps(const std::string &filename);		// 0x729BF0  // NAME 2: what it does
	static void Shutdown_Bone_Maps(void);							// 0x729D60  // NAME 2: what it does
	static RefCountPtr<LegoBoneMapClass> Get_Bone_Map(int asset_id);	// 0x72D3E0: one map per asset id, loaded once  // NAME 2: what it returns
	static RefCountPtr<LegoClass> Create(const LegoIdentifierClass &id, int unknown, int asset_id, const RefCountPtr<RenderObjClass> &render_object);	// 0x729E00  // NAME 2: what it does

	virtual const LegoIdentifierClass &Get_Identifier(void) const;	// 0x72A5A0
	virtual float Get_Scale(void) const;							// 0x72A5C0
	virtual void Set_Scale(float scale);							// 0x72A5E0
	virtual float Get_Radius(void) const;							// 0x72A6C0
	virtual void Unknown_18(bool unknown);							// 0x72A6E0
	virtual void Unknown_1C(void);									// 0x72A700
	virtual void Init(int asset_id, const RefCountPtr<RenderObjClass> &render_object);	// 0x72A7C0
	virtual RefCountPtr<RenderObjClass> Get_Render_Object(void) const;	// 0x72AB80
	virtual void Unknown_28(void);									// 0x72ABB0
	virtual void Unknown_2C(void);									// 0x72B000
	virtual void Unknown_30(void);									// 0x72B7F0
	virtual void Unknown_34(void);									// 0x72C1D0
	virtual void Detach_All(std::vector<RefCountPtr<LegoClass> > &legos);	// 0x72C8B0
	virtual void Unknown_3C(int unknown);							// 0x72CBF0
	virtual void Set_Parent(LegoParentClass *parent);				// 0x72CAE0
	virtual void Clear_Parent(void);								// 0x72CB50
	virtual void Notify_Parent(void);								// 0x72CB90
	virtual const Matrix3D &Get_Transform(void) const;				// 0x72CBD0

	virtual void Child_Moved(const LegoIdentifierClass &id);		// 0x72CE40
	virtual void Child_Changed(const LegoIdentifierClass &id);		// 0x72D0C0

	void Attach(const RefCountPtr<LegoClass> &child, const RefCountPtr<RenderObjClass> &render_object, const RefCountPtr<LegoKnobClass> &knob);	// 0x72D0E0  // NAME 2: what it does
	void Detach(const RefCountPtr<LegoClass> &child, const RefCountPtr<RenderObjClass> &render_object, const RefCountPtr<LegoKnobClass> &knob);	// 0x72D2A0  // NAME 2: what it does
	void Update_Radius(void);										// 0x72CC10: the client's assert '!BoneMap'  // NAME 2: what it does

	std::hash_map<std::string, LegoChildStruct> Children;			// +0x0C: the client's asserts
	std::hash_map<std::string, RefCountPtr<LegoOrphanDataClass> > Orphans;	// +0x24: the client's asserts
	LegoIdentifierClass Identifier;									// +0x3C  // NAME 2: what it holds
	int Unknown50;													// +0x50: the constructor's second argument  // NAME 1: neutral (D10)
	int AssetId;													// +0x54: -1 until Init  // NAME 2: Init's asset_id (the client's assert 'asset_id < 0')
	RefCountPtr<LegoBoneMapClass> BoneMap;							// +0x58: the client's assert '!BoneMap'
	float Scale;													// +0x5C  // NAME 2: what Set_Scale sets
	float Radius;													// +0x60  // NAME 2: what Update_Radius sets
	bool Unknown64;													// +0x64  // NAME 1: neutral (D10)
	bool Unknown65;													// +0x65  // NAME 1: neutral (D10)
	RefCountPtr<RenderObjClass> RenderObject;						// +0x68: the client's asserts '!RenderObject'
	Matrix3D Transform;												// +0x6C  // NAME 2: what Get_Transform returns
	LegoParentClass *Parent;										// +0x9C: the client's asserts 'Parent'

};

#endif
