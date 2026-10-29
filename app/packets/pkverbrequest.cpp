// Earth & Beyond client (reconstruction): app\packets\pkverbrequest.cpp. Notes at the end of the file.
#include "pkverbrequest.h"
#include <stdio.h>

VerbRequestPacketClass::VerbRequestPacketClass(int subject_id, int object_id, int action) : SubjectID(subject_id), ObjectID(object_id), Action(action)
{
}

VerbRequestPacketClass::VerbRequestPacketClass(RawPacketClass *raw)
{
	raw->Get(SubjectID);
	raw->Get(ObjectID);
	raw->Get(&Action, sizeof(Action));
}

RawPacketClass *VerbRequestPacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(Get_Type(), Get_Data_Size());
	raw->Add(SubjectID);
	raw->Add(ObjectID);
	raw->Add(&Action, sizeof(Action));
	return(raw);
}

int VerbRequestPacketClass::Get_Type()
{
	return(90);
}

int VerbRequestPacketClass::Get_Data_Size()
{
	return(sizeof(SubjectID) + sizeof(ObjectID) + sizeof(Action));
}

int VerbRequestPacketClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s { SubjectID=%d ObjectID=%d Action=%d}", "VerbRequest", SubjectID, ObjectID, Action));
}

VerbListPacketClass::VerbListPacketClass(int game_id, const std::set<VerbEnum> &verbs) :
	GameID(game_id),
	Verbs(verbs)
{
}

// NOT EXACT: 0x8AF150, insert inlined as the client's but its _Multi branch's pair constructor too (inline-budget; attempts.py show 0x8AF150)
VerbListPacketClass::VerbListPacketClass(RawPacketClass *raw) :
	GameID(0)
{
	raw->Get(GameID);
	int count;
	raw->Get(count);
	for (int i = 0; i < count; i++) {
		VerbEnum verb;
		raw->Get(&verb, sizeof(verb));
		Verbs.insert(verb);
	}
}

RawPacketClass *VerbListPacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(Get_Type(), Get_Data_Size());
	raw->Add(GameID);
	int count = Verbs.size();
	raw->Add(count);
	for (std::set<VerbEnum>::iterator it = Verbs.begin(); it != Verbs.end(); ++it) {
		VerbEnum verb = *it;
		raw->Add(&verb, sizeof(verb));
	}
	return(raw);
}

int VerbListPacketClass::Get_Data_Size()
{
	return(Verbs.size() * sizeof(VerbEnum) + sizeof(GameID) + sizeof(int));
}

// NOT EXACT: 0x8AF6D0, the client inlines the string's _Tidy and calls begin() (frame 0x11C; attempts.py show 0x8AF6D0)
int VerbListPacketClass::Print(char *buffer, int)
{
	std::string verbs;
	char name[256];
	for (std::set<VerbEnum>::iterator it = Verbs.begin(); it != Verbs.end(); ++it) {
		Value_To_String(name, sizeof(name), *it);
		verbs += name;
		verbs += " ";
	}
	return(sprintf(buffer, "%s { GameID=%d Verbs=%s }", "VerbList", GameID, verbs.c_str()));
}

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
**	Reconstruction notes (pkverbrequest.cpp)
**
**	* Client 0x8AEAE0-0x8B15C0, one object (P-PK2-2: pkverblist.cpp merged in, 2026-10-29): VERB_REQUEST 90
**	  (0x8AEAE0-0x8AED40), VERB_LIST 91 (0x8AED60-0x8AF840), VERB_UPDATE 92 (0x8AFD10-0x8B0430), then
**	  std::set<VerbEnum>'s members (0x8B0440-0x8B1360); the object ends with the guarded static every
**	  <string> includer has, 0xC416FC, atexit 0x42B320.
**	* VERB_REQUEST 90 drafted from the client by the packet decoder (tools/re/pkdecode.py, pkemit.py):
**	  members from the raw message's Get / Build's Add (sizes) and Print (types, names: 'Name=%d'), the
**	  constructors' stores in the initialiser list.
**	* VERB_UPDATE 92 written by hand (builder P4, 2026-10-26); its destructor is the implicit one (0x8AFBB0).
**	* VERB_LIST 91 (builder PK1, 2026-10-28) written after 92's form: one set; its destructor is the
**	  implicit one (0x8AEFE0).
**	* The file name is provisional (no assert names it: pk + its first named class's name).
*/
