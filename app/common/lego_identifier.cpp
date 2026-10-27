// Earth & Beyond client (reconstruction): app\Common\lego_identifier.cpp, client 0x8436D0-0x8451A0.
// A lego identifier is a bone path: the bone names from the root down, written as one string with
// '/' between them. Notes at the end of the file.
//
// What uses it: the object-to-object effects (pkpointeffect.cpp, opcodes 11-14) carry a lego
// identifier: the received string sets it (operator=, Set_Name), Build and Get_Data_Size write its
// name (Get_Name). The string is split at Separator by 0x4FC2F0 (another object's helper).
//
// Layout (0x14 bytes, vtable 0xB08C5C: the scalar deleting destructor only):
//	+0x00	the vtable
//	+0x04	BoneNameChain, a std::vector<std::string> (allocator byte, first, last, end)
//
// The name of an empty identifier is EmptyName (""), which Get_Name, Get_Root and Pop_Root return
// and Set_Name ignores. Lego types: tractor (0) and emitter (1), with two lookups at the start of
// the object; their names are written with the bounded copy 0x4A1590.
//
// Function order is the client's (0x843810-0x844CD0); 0x844ED0, the last, is not rebuilt
// (see the notes).
//
#include "lego_identifier.h"
#include "..\shared\unknown_debug.h"
#include <string.h>
#include <algorithm>

char *Unknown_4A1590(char *buffer, int size, const char *string);	// 0x4A1590: a bounded strcpy  // NAME 1: neutral placeholder (D10)
void Unknown_4FC2F0(std::vector<std::string> &list, const std::string &string, const std::string &separator);	// 0x4FC2F0: splits string at separator  // NAME 1: neutral placeholder (D10)

// The name of an empty identifier (its initialiser 0x8436D0 opens the object, its destructor 0x843790)
static std::string EmptyName("");	// 0xBF3598  // NAME 2: what it holds

// Between the bone names
static char *Separator = "/";		// 0xBAECD4  // NAME 2: what it holds

struct LegoTypeStruct	// NAME 2: what it holds
{
	const char *Name;
	int Type;
};

// The lego types and their names (Lego_Type_Name, Lego_Type_From_Name)
static const LegoTypeStruct LegoTypes[] = {	// 0xB08C40  // NAME 2: what it holds
	{ "tractor", 0 },
	{ "emitter", 1 },
	{ NULL, 1 }
};


// The type's name into buffer, "root" for an unknown one. The client's loop never moves past the
// table's first entry (kept: it is the client's code).
void Lego_Type_Name(char *buffer, int len, int *type)
{
	for (const LegoTypeStruct *entry = LegoTypes; entry->Name != NULL; ) {
		if (entry->Type == *type) {
			Unknown_4A1590(buffer, len, entry->Name);
			return;
		}
	}
	G_ASSERT(0);
	Unknown_4A1590(buffer, len, "root");
}


// The type of a name (case ignored); false (and an assert) for an unknown one. The table ends
// with a null name. The comparison is stricmp(entry, name), the table's string first, as the client
// pushes it.
//
bool Lego_Type_From_Name(const char *name, int *type)
{
	for (const LegoTypeStruct *entry = LegoTypes; entry->Name != NULL; entry++) {
		if (stricmp(entry->Name, name) == 0) {
			*type = entry->Type;
			return(true);
		}
	}
	G_ASSERT(0);
	return(false);
}

LegoIdentifierClass::LegoIdentifierClass()
{
}

// NOT EXACT: 0x843970, the client inlines Set_Name here (inline-budget; attempts.py show 0x843970)
LegoIdentifierClass::LegoIdentifierClass(const std::string &name)
{
	Set_Name(name);
}

LegoIdentifierClass::LegoIdentifierClass(const LegoIdentifierClass &src) :
	BoneNameChain(src.BoneNameChain)
{
}

LegoIdentifierClass::~LegoIdentifierClass()
{
}

bool LegoIdentifierClass::Is_Empty() const
{
	return(BoneNameChain.empty());
}
// NOT EXACT: 0x843E80, registers in the inlined vector assignment (shape; attempts.py show 0x843E80)
LegoIdentifierClass &LegoIdentifierClass::operator=(const LegoIdentifierClass &src)
{
	BoneNameChain = src.BoneNameChain;
	return(*this);
}
// NOT EXACT: 0x844120, npos read from memory in the client's clear (shape; attempts.py show 0x844120)
LegoIdentifierClass &LegoIdentifierClass::operator=(const std::string &name)
{
	BoneNameChain.clear();
	Set_Name(name);
	return(*this);
}
// NOT EXACT: 0x844210 (shape; attempts.py show 0x844210)
bool LegoIdentifierClass::operator==(const LegoIdentifierClass &src) const
{
	if (BoneNameChain.size() != src.BoneNameChain.size()) {
		return(false);
	}
	return(std::equal(BoneNameChain.begin(), BoneNameChain.end(), src.BoneNameChain.begin()));
}
int LegoIdentifierClass::Get_Count() const
{
	return(BoneNameChain.size());
}

// NOT EXACT: 0x844330, the client tests for the empty name first (shape; attempts.py show 0x844330)
std::string LegoIdentifierClass::Get_Root() const
{
	return(BoneNameChain.empty() ? EmptyName : BoneNameChain[0]);
}
// NOT EXACT: 0x8444C0, npos read from memory in the client's erase (shape; attempts.py show 0x8444C0)
std::string LegoIdentifierClass::Pop_Root()
{
	FAIL_IF(!BoneNameChain.size()) {
		return(EmptyName);
	}
	std::string root = BoneNameChain[0];
	BoneNameChain.erase(BoneNameChain.begin());
	return(root);
}

void LegoIdentifierClass::Add(const std::string &bone)
{
	BoneNameChain.push_back(bone);
}


// NOT EXACT: 0x8448A0, npos read from memory in the client's copy (shape; attempts.py show 0x8448A0).
// The client copies over the chain's end without growing it (kept: it is the client's code).
void LegoIdentifierClass::Add(const LegoIdentifierClass &src)
{
	std::copy(src.BoneNameChain.begin(), src.BoneNameChain.end(), BoneNameChain.end());
}

std::string LegoIdentifierClass::Get_Name() const
{
	if (BoneNameChain.empty()) {
		return(EmptyName);
	}
	std::vector<std::string>::const_iterator it = BoneNameChain.begin();
	std::string name = *it;
	for (++it; it != BoneNameChain.end(); ++it) {
		name += Separator;
		name += *it;
	}
	return(name);
}

// NOT EXACT: 0x844CD0, registers (attempts.py show 0x844CD0)
void LegoIdentifierClass::Set_Name(const std::string &name)
{

	FAIL_IF(!BoneNameChain.empty()) {
		BoneNameChain.clear();
	}
	if (name != EmptyName) {
		Unknown_4FC2F0(BoneNameChain, name, std::string(Separator));
	}
}


/*
**	Reconstruction notes (lego_identifier.cpp)
**
**	* Client 0x8436D0-0x8451A0: itemtype.cpp's object ends at 0x8436A0 (the guarded static every
**	  <string> includer has, 0xC416FC, atexit 0x42B320); this one opens with EmptyName's initialiser
**	  and ends before manufacturing_defs.cpp's 0x8451A0. The asserts sit on the client's lines 58, 75,
**	  136 and 175 (the constructor 0x843970 inlines Set_Name: its FAIL_IF is line 175 too).
**	* Not rebuilt: 0x844ED0 (a comparison of two identifiers into a 3-state result over other classes'
**	  helpers 0x4E4AA0, 0x73F950, 0x47D640, 0x441480, 0x47D440, 0x4D22B0).
**	* Written by builder P4 (2026-10-26) for the object-to-object effects (pkpointeffect.cpp, opcodes
**	  11-14), which send an identifier's name and set one from the received string.
*/
