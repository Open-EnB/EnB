// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\shared\attrib_dir_collective.h (name assumed from
**	attrib_dir_collective.cpp, whose __FILE__ is 'e:\build\beta\G\app\shared\attrib_dir_collective.cpp')
**
**	Attributes combined over several directories: a value function collects one value per member
**	(average, smallest, largest) and a mod hands its result to an attribute. The client names Values
**	and ValueFunction through its asserts; the classes are named by what they do (D24). Declared as
**	far as rebuilt: the collective itself (Members, DestinationAttributes) is not yet.
*/



#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __ATTRIB_DIR_COLLECTIVE_H  // NAME 3 [attrib_dir_collective.h]: header name assumed from the client's attrib_dir_collective.cpp
#define __ATTRIB_DIR_COLLECTIVE_H

#include <vector>
#include "attrib_dir.h"
#include "..\..\wwlib\refcount.h"
#include "..\..\wwlib\ref_ptr.h"

// Combines one value per member (slots: Delete_This, the destructor, Add, Remove, Replace, Calculate)
class AttribValueFunctionClass : public RefCountClass  // NAME 2: what it is
{
public:
	virtual void Add(float value) = 0;																// slot 2  // NAME 2: what it does
	virtual void Remove(float value) = 0;															// slot 3  // NAME 2: what it does
	virtual void Replace(float old_value, float new_value) = 0;								// slot 4  // NAME 2: what it does
	virtual float Calculate() = 0;																	// slot 5  // NAME 2: what it does
};

// The average (vtable 0xAEC2DC, 0x20 bytes)
class AttribAverageFunctionClass : public AttribValueFunctionClass  // NAME 2: what it computes
{
public:
	AttribAverageFunctionClass();																	// 0x4DA880
	virtual ~AttribAverageFunctionClass() {}													// 0x4DA8F0
	virtual void Add(float value);																// 0x4DA930
	virtual void Remove(float value);															// 0x4DAB40
	virtual void Replace(float old_value, float new_value);								// 0x4DABD0
	virtual float Calculate();																		// 0x4DAC40

	static RefCountPtr<AttribValueFunctionClass> Create();									// 0x4DA7F0  // NAME 2: what it does

	std::vector<float> Values;																		// +0x08
	bool Changed;																						// +0x18  // NAME 2: set by every change
	float Unknown1C;																					// +0x1C  // NAME 1: neutral placeholder (D10)
};

// The smallest (vtable 0xAEC2F8, 0x1C bytes)
class AttribMinFunctionClass : public AttribValueFunctionClass  // NAME 2: what it computes
{
public:
	AttribMinFunctionClass();																		// 0x4DAD50
	virtual ~AttribMinFunctionClass() {}														// 0x4DADC0
	virtual void Add(float value);																// 0x4DAE00
	virtual void Remove(float value);															// 0x4DB010
	virtual void Replace(float old_value, float new_value);								// 0x4DB0A0
	virtual float Calculate();																		// 0x4DB110

	static RefCountPtr<AttribValueFunctionClass> Create();									// 0x4DACD0  // NAME 2: what it does

	std::vector<float> Values;																		// +0x08
	bool Sorted;																						// +0x18  // NAME 2: what it means
};

// The largest (vtable 0xAEC314, 0x1C bytes)
class AttribMaxFunctionClass : public AttribValueFunctionClass  // NAME 2: what it computes
{
public:
	AttribMaxFunctionClass();																		// 0x4DB3B0
	virtual ~AttribMaxFunctionClass() {}														// 0x4DB420
	virtual void Add(float value);																// 0x4DB460
	virtual void Remove(float value);															// 0x4DB670
	virtual void Replace(float old_value, float new_value);								// 0x4DB700
	virtual float Calculate();																		// 0x4DB770

	static RefCountPtr<AttribValueFunctionClass> Create();									// 0x4DB330  // NAME 2: what it does

	std::vector<float> Values;																		// +0x08
	bool Sorted;																						// +0x18  // NAME 2: what it means
};

// A mod whose value is a value function's (vtable 0xAEC330, 0x10 bytes)
class AttribFunctionModClass : public AttribModClass  // NAME 2: what it is
{
public:
	AttribFunctionModClass(const RefCountPtr<AttribValueFunctionClass> &function, int type, void *owner);	// 0x4DB990
	virtual ~AttribFunctionModClass();															// 0x4DBAA0
	virtual int Get_Type() { return Type; }													// 0x4DBB40
	virtual float Get_Value();																		// 0x4DBB60
	virtual void *Get_Owner() { return Owner; }												// 0x4DBBA0

	RefCountPtr<AttribValueFunctionClass> ValueFunction;									// +0x04
	int Type;																							// +0x08  // NAME 2: as AttribModValueClass
	void *Owner;																						// +0x0C  // NAME 2: as AttribModValueClass
};


// Several directories' attributes combined into one directory's (0x48 bytes)
class AttribDirCollective  // NAME 3: by convention (D24) from the client's attrib_dir_collective.cpp
{
public:
	// A member attribute's destination: its name in Dir and the value function (0x14 bytes)
	struct DestinationStruct  // NAME 2: what it holds
	{
		DestinationStruct(const std::string &name, const RefCountPtr<AttribValueFunctionClass> &function) : Name(name), Function(function) {}
		std::string Name;																			// +0x00  // NAME 2: what it holds
		RefCountPtr<AttribValueFunctionClass> Function;									// +0x10  // NAME 2: what it holds
	};

	// Told when a member's attribute changes (vtable 0xAEC344, 0x0C bytes)
	class MemberCallbackClass : public AttribCallbackClass  // NAME 2: what it is
	{
	public:
		MemberCallbackClass(AttribDirCollective &parent, AttribDir *member);		// 0x4DCD10
		virtual void Value_Changed(const char *name, float value);					// 0x4DCD40
		AttribDirCollective &Parent;															// +0x04 (the FAIL_IFs' Parent)
		AttribDir *Member;																		// +0x08  // NAME 2: what it holds
	};

	// One member: its last values by attribute and its callback (0x1C bytes)
	struct MemberStruct  // NAME 2: what it holds
	{
		MemberStruct();																			// 0x4DD240
		std::hash_map<std::string, float> Values;											// +0x00  // NAME 2: what it holds
		MemberCallbackClass *Callback;														// +0x18  // NAME 2: what it holds
	};

	typedef std::hash_map<unsigned int, MemberStruct *> MemberMap;	// keyed by the member AttribDir's address
	typedef std::hash_map<std::string, DestinationStruct *> DestinationMap;

	AttribDirCollective();																		// 0x4DBBC0
	~AttribDirCollective();																		// 0x4DBFD0

	void Add_Destination(const std::string &source, const std::string &destination, const RefCountPtr<AttribValueFunctionClass> &function);	// 0x4DC310  // NAME 2: what it does
	void Add_Member(AttribDir *member);														// 0x4DC720  // NAME 2: what it does
	void Remove_Member(AttribDir *member);													// 0x4DC9E0  // NAME 2: what it does
	void Link(AttribDir *member, const std::string &destination);						// 0x4DCC80  // NAME 2: what it does
	void Detach_Owned_Mods(AttribDir *dir);													// 0x4DCCD0  // NAME 2: what it does

	MemberMap Members;																			// +0x00
	DestinationMap DestinationAttributes;													// +0x18
	AttribDir Dir;																					// +0x30  // NAME 2: what it holds
};

#endif //__ATTRIB_DIR_COLLECTIVE_H
