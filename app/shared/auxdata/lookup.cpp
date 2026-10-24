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


/*
**	Reconstruction notes
**
**	client.exe: LookupClass's index accessors 0x4AC850, 0x4AC8D0 and 0x4AC960, asserts on the
**	client's lines 67, 78 and 90 (D25). Not rebuilt yet: the constructor 0x4AC250, 0x4AC3B0,
**	0x4AC560, 0x4AC720 (name.cpp/permstr.cpp callers) and the name lookups 0x4AC9F0 and 0x4ACA90
**	(asserts "!name.Is_Wildcard()" at lines 104 and 111, the map at +0x10).
*/
