/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\shared\attrib_dir.h (name assumed from attrib_dir.cpp,
**	whose __FILE__ is 'e:\build\beta\G\app\shared\attrib_dir.cpp')
**
**	A directory of named attributes (the client's AttribDir, AttributeInfo): each attribute has a
**	list of mods (raise or lower the base's bounds, add, add a percentage, raise or lower the
**	result's bounds), a value computed from them, callbacks told when the value changes and other
**	attributes recalculated with it. Names are kept upper-case in an SGI hash_map (libs/sgistl,
**	D87). The client names AttribDir's methods ('AttribDir::Attach_Mod', ...) and
**	AttributeInfo::Apply_Mod through its strings; the rest is named by what it does.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __ATTRIB_DIR_H  // NAME 3 [attrib_dir.h]: header name assumed from the client's attrib_dir.cpp
#define __ATTRIB_DIR_H

#include <float.h>
#include <string>
#include <list>
#include <vector>
#include <hash_map.h>

class AttributeInfo;

// A mod of an attribute's value (vtable 0xAEC1D4)
class AttribModClass  // NAME 2: what it is
{
public:
	virtual ~AttribModClass() {}												// 0x4D7680
	virtual int Get_Type() = 0;													// slot 1: 0-5, AttributeInfo::Apply_Mod  // NAME 2: what it returns
	virtual float Get_Value() = 0;												// slot 2  // NAME 2: what it returns
	virtual void *Get_Owner() { return 0; }									// slot 3, 0x4D76A0  // NAME 2: what it returns
};

// A mod with its values (vtable 0xAEC1C0, 0x10 bytes)
class AttribModValueClass : public AttribModClass  // NAME 2: what it is
{
public:
	AttribModValueClass(int type, float value, void *owner) : Type(type), Value(value), Owner(owner) {}	// 0x4D7620
	virtual int Get_Type() { return Type; }									// 0x4D76F0
	virtual float Get_Value() { return Value; }								// 0x4D7710
	virtual void *Get_Owner() { return Owner; }								// 0x4D7730

	int Type;																			// +0x04  // NAME 2: what it holds
	float Value;																		// +0x08  // NAME 2: what it holds
	void *Owner;																		// +0x0C  // NAME 2: what it holds
};

// Told when an attribute's value changes
class AttribCallbackClass  // NAME 2: what it is
{
public:
	virtual void Value_Changed(const char *name, float value) = 0;		// slot 0  // NAME 2: what it is told
};

const float ATTRIB_MIN_VALUE = -FLT_MAX;										// 0xAEC1A0  // NAME 2: what it holds
const float ATTRIB_MAX_VALUE = FLT_MAX;										// 0xAEC1A4  // NAME 2: what it holds

// The sums of an attribute's mods (0x18 bytes)
struct AttribModValuesStruct  // NAME 2: what it holds
{
	AttribModValuesStruct() : BaseMin(ATTRIB_MIN_VALUE), BaseMax(ATTRIB_MAX_VALUE), Add(0), Percent(0), Min(ATTRIB_MIN_VALUE), Max(ATTRIB_MAX_VALUE) {}	// 0x4D74D0

	float BaseMin;																		// +0x00 type 0: the largest  // NAME 2: what it holds
	float BaseMax;																		// +0x04 type 1: the smallest  // NAME 2: what it holds
	float Add;																			// +0x08 type 2: the sum  // NAME 2: what it holds
	float Percent;																		// +0x0C type 3: the sum  // NAME 2: what it holds
	float Min;																			// +0x10 type 4: the largest  // NAME 2: what it holds
	float Max;																			// +0x14 type 5: the smallest  // NAME 2: what it holds
};

// The iterators walking one of an attribute's lists, moved on when their element is removed
template <class IT>
class AttribIteratorListClass : public std::vector<IT *>  // NAME 2: what it holds
{
public:
	void Advance(const IT &it)														// NAME 2: what it does
	{
		for (std::vector<IT *>::iterator i = begin(); i != end(); ++i) {
			if (*i != 0 && **i == it) {
				++(**i);
			}
		}
	}
	void Invalidate()																	// NAME 2: what it does
	{
		for (std::vector<IT *>::iterator i = begin(); i != end(); ++i) {
			if (*i != 0) {
				*i = 0;
			}
		}
	}
	bool Pop()																			// NAME 2: what it does
	{
		if (empty()) {
			return false;
		}
		IT *it = back();
		erase(end() - 1);
		return it != 0;
	}
};

struct AttributeInfoHash  // NAME 2: what it does
{
	size_t operator()(AttributeInfo *info) const { return (size_t)info; }
};

// One attribute (0x7C bytes)
class AttributeInfo
{
public:
	typedef std::list<AttribModClass *> ModList;
	typedef std::list<AttribCallbackClass *> CallbackList;

	AttributeInfo(const char *name);												// 0x4D6360
	~AttributeInfo();																	// 0x4D6620

	bool Attach_Mod(AttribModClass *mod);										// 0x4D6820  // NAME 2: as AttribDir's
	bool Detach_Mod(AttribModClass *mod);										// 0x4D6960  // NAME 2: as AttribDir's
	bool Attach_Callback(AttribCallbackClass *cb);							// 0x4D6B30  // NAME 2: as AttribDir's
	bool Detach_Callback(AttribCallbackClass *cb);							// 0x4D6BA0  // NAME 2: as AttribDir's
	bool Attach_Update(AttributeInfo *dst);									// 0x4D6C40  // NAME 2: as AttribDir's
	bool Detach_Update(AttributeInfo *dst);									// 0x4D6D10  // NAME 2: as AttribDir's
	void Apply_Mod(AttribModClass *mod);										// 0x4D6DF0
	float Get_Base();																	// 0x4D6F70  // NAME 2: as AttribDir's
	float Calculate();																// 0x4D6FC0  // NAME 2: what it does
	int Recalculate();																// 0x4D7060  // NAME 2: as AttribDir's
	void Notify();																		// 0x4D71D0  // NAME 2: what it does
	int Update_Value();																// inline: Calculate, Notify when it changed  // NAME 2: what it does

	std::string Name;																	// +0x00  // NAME 2: what it holds
	AttribModValuesStruct Values;													// +0x10  // NAME 2: what it holds
	ModList Mods;																		// +0x28  // NAME 2: what it holds
	AttribIteratorListClass<ModList::iterator> ModIterators;				// +0x34  // NAME 2: what it holds
	CallbackList Callbacks;															// +0x44  // NAME 2: what it holds
	AttribIteratorListClass<CallbackList::iterator> CallbackIterators;	// +0x50  // NAME 2: what it holds
	std::hash_map<AttributeInfo *, int, AttributeInfoHash> Updates;		// +0x60: recalculated after this one, with a count  // NAME 2: what it holds
	float Value;																		// +0x78  // NAME 2: what it holds
};

// What AttribDir::Get_Shadow returns: an attribute to read (4 bytes)
class AttribShadowClass  // NAME 2: from AttribDir::Get_Shadow
{
public:
	AttribShadowClass() : Info(0) {}												// 0x4D7510
	AttribShadowClass(AttributeInfo *info) : Info(info) {}					// 0x4D7530

	float Get_Value();																// 0x4D7550  // NAME 2: as AttribDir's
	float Get_Base();																	// 0x4D7590  // NAME 2: as AttribDir's

	AttributeInfo *Info;																// +0x00 (the FAIL_IF's '!Info')
};

// The directory (0x18 bytes)
class AttribDir
{
public:
	typedef std::hash_map<std::string, AttributeInfo *> AttributeMap;

	AttribDir();																		// 0x4D42C0
	~AttribDir();																		// 0x4D4490

	bool Attach_Mod(const char *name, AttribModClass *mod);				// 0x4D4810
	bool Detach_Mod(const char *name, AttribModClass *mod);				// 0x4D4A10
	bool Attach_Callback(const char *name, AttribCallbackClass *cb);		// 0x4D4EA0
	bool Detach_Callback(const char *name, AttribCallbackClass *cb);		// 0x4D5000
	bool Attach_Update(const char *src, const char *dst);					// 0x4D51D0
	bool Detach_Update(const char *src, const char *dst);					// 0x4D5380
	bool Recalculate(const char *name);											// 0x4D55D0
	float Get_Value(const char *name) const;									// 0x4D57E0
	float Get_Base(const char *name) const;									// 0x4D5950
	AttribShadowClass Get_Shadow(const char *name);							// 0x4D5B00

	AttributeInfo *Get_Info(const char *name);								// 0x4D5C50: made when missing  // NAME 2: what it does
	AttributeInfo *Find_Info(const char *name);								// 0x4D5F10  // NAME 2: what it does
	AttributeInfo *Find_Info(const char *name) const;						// 0x4D6070  // NAME 2: what it does
	std::string Make_Key(const char *name) const;							// 0x4D61D0: upper case  // NAME 2: what it does

	AttributeMap Attributes;														// +0x00  // NAME 2: what it holds
};

#endif //__ATTRIB_DIR_H
