// Earth & Beyond client (reconstruction)
/*
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
class AttribDir;

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
	AttribModValueClass(int type, float value, void *owner);										// 0x4D7620
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
	AttribModValuesStruct();																		// 0x4D74D0

	float BaseMin;																		// +0x00 type 0: the largest  // NAME 2: what it holds
	float BaseMax;																		// +0x04 type 1: the smallest  // NAME 2: what it holds
	float Add;																			// +0x08 type 2: the sum  // NAME 2: what it holds
	float Percent;																		// +0x0C type 3: the sum  // NAME 2: what it holds
	float Min;																			// +0x10 type 4: the largest  // NAME 2: what it holds
	float Max;																			// +0x14 type 5: the smallest  // NAME 2: what it holds
};

// The iterators walking one of an attribute's lists, moved on when their element is removed. Advance and Invalidate
// walk with while loops: as for loops VC6 inlines Advance less in AttribDir::Detach_Callback 0x4D5000 and Invalidate
// less in ~AttributeInfo 0x4D6620 than the client (HL1, 2026-10-29)
template <class IT>
class AttribIteratorListClass : public std::vector<IT *>  // NAME 2: what it holds
{
public:
	void Advance(const IT &it)														// NAME 2: what it does
	{
		std::vector<IT *>::iterator i = begin();
		while (i != end()) {
			if (*i != 0 && **i == it) {
				++(**i);
			}
			++i;
		}
	}
	void Invalidate()																	// NAME 2: what it does
	{
		std::vector<IT *>::iterator i = begin();
		while (i != end()) {
			if (*i != 0) {
				*i = 0;
			}
			++i;
		}
	}
	bool Pop_Back()																	// NAME 2: what it does
	{
		if (empty()) {
			return false;
		}
		IT *it = back();
		pop_back();
		return it != 0;
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
	AttribShadowClass();																				// 0x4D7510
	AttribShadowClass(AttributeInfo *info);																// 0x4D7530

	float Get_Value();																// 0x4D7550  // NAME 2: as AttribDir's
	float Get_Base();																	// 0x4D7590  // NAME 2: as AttribDir's

	AttributeInfo *Info;																// +0x00 (the FAIL_IF's '!Info')
};

// Walks mods (vtable 0xAEC20C: every slot pure, no destructor)
class AttribModIteratorClass  // NAME 2: what it does
{
public:
	virtual AttribModClass *Get_Mod() = 0;											// slot 0  // NAME 2: what it returns
	virtual const char *Get_Name() = 0;												// slot 1: the mod's attribute  // NAME 2: what it returns
	virtual bool Is_Done() = 0;														// slot 2  // NAME 2: what it returns
	virtual AttribModIteratorClass *Clone() = 0;									// slot 3  // NAME 2: what it does
	virtual void Next() = 0;															// slot 4  // NAME 2: what it does
	virtual void Lock_Next() = 0;														// slot 5: steps on and registers with the attribute (Detach_Mod)  // NAME 2: what it does
	virtual void Unlock() = 0;															// slot 6  // NAME 2: what it does
};

// The mods of one attribute (vtable 0xAEC230, 0x10 bytes)
class AttribInfoModIteratorClass : public AttribModIteratorClass  // NAME 2: what it does
{
public:
	AttribInfoModIteratorClass(AttributeInfo *info) : Cur(info->Mods.begin()), End(info->Mods.end()), Info(info) {}	// 0x4D7E20
	virtual AttribModClass *Get_Mod() { return *Cur; }							// 0x4D7BB0
	virtual const char *Get_Name() { return Info->Name.c_str(); }				// 0x4D7BD0
	virtual bool Is_Done() { return Cur == End; }									// 0x4D7BF0
	virtual AttribModIteratorClass *Clone() { return new AttribInfoModIteratorClass(*this); }	// 0x4D7C20
	virtual void Next() { ++Cur; }													// 0x4D7C70
	virtual void Lock_Next();															// 0x4D7E60
	virtual void Unlock();																// 0x4D8090

	AttributeInfo::ModList::iterator Cur;										// +0x04  // NAME 2: what it holds
	AttributeInfo::ModList::iterator End;										// +0x08  // NAME 2: what it holds
	AttributeInfo *Info;																// +0x0C  // NAME 2: what it holds
};

// The mods of every attribute of a directory (vtable 0xAEC1E8, 0x1C bytes)
class AttribDirModIteratorClass : public AttribModIteratorClass  // NAME 2: what it does
{
public:
	AttribDirModIteratorClass(AttribDir *dir);										// 0x4D8120
	virtual AttribModClass *Get_Mod() { return *ModCur; }						// 0x4D7930
	virtual const char *Get_Name() { return (*MapCur).first.c_str(); }		// 0x4D7950
	virtual bool Is_Done() { return ModCur == ModEnd && MapCur == MapEnd; }	// 0x4D7980
	virtual AttribModIteratorClass *Clone() { return new AttribDirModIteratorClass(*this); }	// 0x4D79B0
	virtual void Next();																	// 0x4D8240
	virtual void Lock_Next();															// 0x4D8430
	virtual void Unlock();																// 0x4D8690

	void Skip_Empty();																	// 0x4D8370  // NAME 2: what it does

	std::hash_map<std::string, AttributeInfo *>::iterator MapCur;			// +0x04 (the FAIL_IFs' MapCur)
	std::hash_map<std::string, AttributeInfo *>::iterator MapEnd;			// +0x0C (the FAIL_IFs' MapEnd)
	AttributeInfo::ModList::iterator ModCur;									// +0x14  // NAME 2: as MapCur
	AttributeInfo::ModList::iterator ModEnd;									// +0x18  // NAME 2: as MapEnd
};

// What AttribDir's mod walks return: owns an iterator (4 bytes)
class AttribModIterator  // NAME 2: what it is
{
public:
	AttribModIterator();																	// 0x4D7C90
	AttribModIterator(AttribModIteratorClass *iterator);						// 0x4D7CB0
	AttribModIterator(const AttribModIterator &src);								// 0x4D7CD0
	~AttribModIterator();																// 0x4D7D20
	AttribModIterator &operator=(const AttribModIterator &src);				// 0x4D7D50

	AttribModClass *Get_Mod();															// 0x4D7DA0  // NAME 2: as the iterator's
	const char *Get_Name();																// 0x4D7DC0  // NAME 2: as the iterator's
	bool Is_Done();																		// 0x4D7DE0 (the FAIL_IF's 'iter.Is_Done()')
	void Next();																			// 0x4D7E00  // NAME 2: as the iterator's

	AttribModIteratorClass *Iterator;												// +0x00  // NAME 2: what it holds
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

	AttribModIterator Detach_Mod(AttribModIterator &iter);				// 0x4D4BD0: returns the next  // NAME 2: as the other Detach_Mod
	AttribModIterator Get_All_Mods();												// 0x4D77D0  // NAME 2: what it returns
	AttribModIterator Get_Mods(const char *name);								// 0x4D7A20  // NAME 2: what it returns

	AttributeInfo *Get_Info(const char *name);								// 0x4D5C50: made when missing  // NAME 2: what it does
	AttributeInfo *Find_Info(const char *name);								// 0x4D5F10  // NAME 2: what it does
	AttributeInfo *Find_Info(const char *name) const;						// 0x4D6070  // NAME 2: what it does
	std::string Make_Key(const char *name) const;							// 0x4D61D0: upper case  // NAME 2: what it does

	AttributeMap Attributes;														// +0x00  // NAME 2: what it holds
};

#endif //__ATTRIB_DIR_H
