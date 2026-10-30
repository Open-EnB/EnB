// Earth & Beyond client (reconstruction): app\shared\auxdata\lookup.cpp. Notes at the end of the file.
#include "lookup.h"
#include "delta.h"
#include "..\unknown_debug.h"

// (the client's file has no assert before line 67)

// Fills a LookupClass from a tree (vtable 0xAEB318; a local of the constructor): every field in
// visiting order, so a field's index is its position in the walk, and each by its name below the
// root (NameClass::Get_Rest of its FullName). The name is a guess (D10).
class LookupBuilderClass : public DeltaVisitorClass  // NAME 2: describes what it does
{
public:
	LookupBuilderClass(LookupClass *lookup, std::map<const char *, DeltaClass *> &map) :
		Lookup(lookup),
		Map(&map)
	{
	}
	virtual void Visit(DeltaCompoundClass *delta);		// 0x4AC560
	virtual void Visit(DeltaClass *delta);					// 0x4AC3B0

	LookupClass *Lookup;										// +0x04  // NAME 2: describes what it does
	std::map<const char *, DeltaClass *> *Map;			// +0x08  // NAME 2 [Map]: describes what it does
};

LookupClass::LookupClass(DeltaCompoundClass *owner)
{
	LookupBuilderClass builder(this, Unknown10);
	owner->Accept(builder);
}

void LookupBuilderClass::Visit(DeltaClass *delta)
{
	Lookup->Entries.push_back(delta);
	(*Map)[delta->FullName.Get_Rest().Get_String()] = delta;
}

void LookupBuilderClass::Visit(DeltaCompoundClass *delta)
{
	Lookup->Entries.push_back(delta);
	(*Map)[delta->FullName.Get_Rest().Get_String()] = delta;
	Visit_Children(delta);
}





















DeltaClass *LookupClass::Get_Delta(int index) const
{
	G_ASSERT(Is_Valid_Index(index));
	if (!Is_Valid_Index(index)) {
		return 0;
	}
	return Entries[index];
}



bool LookupClass::Unknown_Test_70(int index) const
{
	G_ASSERT(Is_Valid_Index(index));
	if (!Is_Valid_Index(index)) {
		return false;
	}
	return Entries[index]->Unknown70 != 0;
}




bool LookupClass::Unknown_Test_70_Above_1(int index) const
{
	G_ASSERT(Is_Valid_Index(index));
	if (!Is_Valid_Index(index)) {
		return false;
	}
	return Entries[index]->Unknown70 > 1;
}






DeltaClass *LookupClass::Find(const NameClass &name)
{
	G_ASSERT(!name.Is_Wildcard());
	std::map<const char *, DeltaClass *>::iterator i = Unknown10.find(name.Get_String());
	if (i == Unknown10.end()) return 0;
	return (*i).second;
}
bool LookupClass::Unknown_Test_70(const NameClass &name) const
{
	G_ASSERT(!name.Is_Wildcard());
	std::map<const char *, DeltaClass *>::const_iterator i = Unknown10.find(name.Get_String());
	if (i == Unknown10.end() || (*i).second == 0) {
		return false;
	}
	return (*i).second->Unknown70 != 0;	// NOT EXACT: the client calls the map's find out of line (0x4ACBD0); VC6 here inlines it
}
bool LookupClass::Unknown_Test_70_Above_1(const NameClass &name) const
{
	G_ASSERT(!name.Is_Wildcard());
	std::map<const char *, DeltaClass *>::const_iterator i = Unknown10.find(name.Get_String());
	if (i == Unknown10.end() || (*i).second == 0) {
		return false;
	}
	return (*i).second->Unknown70 > 1;	// NOT EXACT: as above (find 0x4ACBD0 out of line in the client)
}



/*
**	Reconstruction notes
**
**	client.exe: LookupClass's index accessors 0x4AC850, 0x4AC8D0 and 0x4AC960, asserts on the
**	client's lines 67, 78 and 90 (D25). The constructor 0x4AC250 walks the tree with a local
**	LookupBuilderClass (vtable 0xAEB318: Visit(leaf) 0x4AC3B0, Visit(compound) 0x4AC560, which
**	adds the compound itself before its children: DeltaVisitorClass::Visit_Children 0x4B0860);
**	`map[key] = delta` brings the map's _Insert 0x4ACF30 (exact) and vector insert 0x4ACC60,
**	begin 0x4ACBB0, the pair constructor 0x4ACF00 and _Dec 0x4AD280 (copies elsewhere: not
**	counted). Not rebuilt yet: 0x4AC720 (name.cpp/permstr.cpp callers). The name tests 0x4ACA90 and
**	0x4ACB20 (lines 111 and 120) are functional: they call the map's find 0x4ACBD0 out of line, ours
**	inline it (tried 2026-10-25: const or not, a local key, a wrapper, an inline helper, the find's
**	address taken earlier in the file, four more inlined finds before them: all inline it). Find
**	0x4AC9F0 (line 104): the map is keyed by the interned name pointer (_Lbound 0x4AD240).
*/
