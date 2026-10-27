// Earth & Beyond client (reconstruction): app\packets\pkverblist.cpp. Notes at the end of the file.
#include "pkverblist.h"
#include <stdio.h>

// NOT EXACT: 0x8AFD10, the client calls the first set's _Init and inlines the second's (inline-budget;
// attempts.py show 0x8AFD10)
VerbUpdatePacketClass::VerbUpdatePacketClass(RawPacketClass *raw) :
	GameID(0)
{
	raw->Get(GameID);
	int count;
	raw->Get(count);
	for (int i = 0; i < count; i++) {
		VerbEnum verb;
		raw->Get(&verb, sizeof(verb));
		Adds.insert(verb);
	}
	raw->Get(count);
	for (int j = 0; j < count; j++) {
		VerbEnum verb;
		raw->Get(&verb, sizeof(verb));
		Drops.insert(verb);
	}
}

// NOT EXACT: 0x8AFF40, the client inlines both loops' iterator steps (inline-budget; attempts.py show 0x8AFF40)
RawPacketClass *VerbUpdatePacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(Get_Type(), Get_Data_Size());
	raw->Add(GameID);
	int adds = Adds.size();
	raw->Add(adds);
	for (std::set<VerbEnum>::iterator it = Adds.begin(); it != Adds.end(); ++it) {
		VerbEnum verb = *it;
		raw->Add(&verb, sizeof(verb));
	}
	int drops = Drops.size();
	raw->Add(drops);
	for (std::set<VerbEnum>::iterator jt = Drops.begin(); jt != Drops.end(); ++jt) {
		VerbEnum verb = *jt;
		raw->Add(&verb, sizeof(verb));
	}
	return(raw);
}

int VerbUpdatePacketClass::Get_Data_Size()
{
	return((Drops.size() + Adds.size()) * sizeof(VerbEnum) + sizeof(GameID) + sizeof(int) + sizeof(int));
}

// NOT EXACT: 0x8B0150, the client inlines the first loop's iterator step only (inline-budget;
// attempts.py show 0x8B0150)
int VerbUpdatePacketClass::Print(char *buffer, int)
{
	std::string adds;
	std::string drops;
	char name[256];
	for (std::set<VerbEnum>::iterator it = Adds.begin(); it != Adds.end(); ++it) {
		Value_To_String(name, sizeof(name), *it);
		adds += name;
		adds += " ";
	}
	for (std::set<VerbEnum>::iterator jt = Drops.begin(); jt != Drops.end(); ++jt) {
		Value_To_String(name, sizeof(name), *jt);
		drops += name;
		drops += " ";
	}
	return(sprintf(buffer, "%s {GameID=%d Drops=%s Adds=%s}", "VerbUpdate", GameID, drops.c_str(), adds.c_str()));
}


/*
**	Reconstruction notes (pkverblist.cpp)
**
**	* Client 0x8AEAD0-0x8B15C0 (the object ends with the guarded static every <string> includer has,
**	  0xC416FC, atexit 0x42B320), with std::set<VerbEnum>'s members (0x8B0440-0x8B1360). VERB_UPDATE
**	  92 written by hand (builder P4, 2026-10-26); its destructor is the implicit one (0x8AFBB0).
**	* Not rebuilt yet: opcode 91 (declared in pk_classes.h).
**	* The file name is provisional (no assert names it).
*/
