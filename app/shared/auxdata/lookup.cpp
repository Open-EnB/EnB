// Earth & Beyond client (reconstruction): app\shared\auxdata\lookup.cpp. Notes at the end of the file.
#include "lookup.h"
#include "delta.h"
#include "..\unknown_debug.h"

// (lines 6-63 hold the rest of the client's file, not rebuilt yet: the asserts below are on the
// client's lines 67, 78 and 90)

























































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
	return Entries[index]->m_Unknown70 != 0;
}




bool LookupClass::Unknown_Test_70_Above_1(int index) const
{
	G_ASSERT(Is_Valid_Index(index));
	if (!Is_Valid_Index(index)) {
		return false;
	}
	return Entries[index]->m_Unknown70 > 1;
}






DeltaClass *LookupClass::Find(const NameClass &name)
{
	G_ASSERT(!name.Is_Wildcard());
	std::map<const char *, DeltaClass *>::iterator i = m_Unknown10.find(name.Get_String());
	if (i == m_Unknown10.end()) {
		return 0;
	}
	return (*i).second;
}



/*
**	Reconstruction notes
**
**	client.exe: LookupClass's index accessors 0x4AC850, 0x4AC8D0 and 0x4AC960, asserts on the
**	client's lines 67, 78 and 90 (D25). Not rebuilt yet: the constructor 0x4AC250, 0x4AC3B0,
**	0x4AC560, 0x4AC720 (name.cpp/permstr.cpp callers) and the name tests 0x4ACA90 and 0x4ACB20
**	(lines 111 and 120: they call the map's find 0x4ACBD0 out of line, ours inline it). Find
**	0x4AC9F0 (line 104): the map is keyed by the interned name pointer (_Lbound 0x4AD240).
*/
