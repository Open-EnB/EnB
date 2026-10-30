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

// The serialisation 91 and 92 share: three static helpers VC6 inlines into their callers (F9, 2026-10-29):
// written out in the callers, the compiler's inline budget never spends as the client's does (the two
// distinct count slots of 0x8AFD10, the per-copy verb slots of 0x8AFF40 and the set::begin() const copy
// 0x8B0440 the Prints call are the helpers' traces); the two loops that walk a set are while loops with
// the step last (a for loop leaves the client's _Inc out of line).
static void Read_Verbs(RawPacketClass *raw, std::set<VerbEnum> &verbs)
{
	int count;
	raw->Get(count);
	for (int i = 0; i < count; i++) {
		VerbEnum verb;
		raw->Get(&verb, sizeof(verb));
		verbs.insert(verb);
	}
}

static void Write_Verbs(RawPacketClass *raw, const std::set<VerbEnum> &verbs)
{
	int count = verbs.size();
	raw->Add(count);
	std::set<VerbEnum>::const_iterator it = verbs.begin();
	while (it != verbs.end()) {
		VerbEnum verb = *it;
		raw->Add(&verb, sizeof(verb));
		++it;
	}
}

static void Append_Verbs(std::string &out, const std::set<VerbEnum> &verbs)
{
	char name[256];
	std::set<VerbEnum>::const_iterator it = verbs.begin();
	while (it != verbs.end()) {
		Value_To_String(name, sizeof(name), *it);
		out += name;
		out += " ";
		++it;
	}
}

VerbListPacketClass::VerbListPacketClass(int game_id, const std::set<VerbEnum> &verbs) :
	GameID(game_id),
	Verbs(verbs)
{
}

VerbListPacketClass::VerbListPacketClass(RawPacketClass *raw) :
	GameID(0)
{
	raw->Get(GameID);
	Read_Verbs(raw, Verbs);
}

RawPacketClass *VerbListPacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(Get_Type(), Get_Data_Size());
	raw->Add(GameID);
	Write_Verbs(raw, Verbs);
	return(raw);
}

int VerbListPacketClass::Get_Data_Size()
{
	return(Verbs.size() * sizeof(VerbEnum) + sizeof(GameID) + sizeof(int));
}

int VerbListPacketClass::Print(char *buffer, int)
{
	std::string verbs;
	Append_Verbs(verbs, Verbs);
	return(sprintf(buffer, "%s { GameID=%d Verbs=%s }", "VerbList", GameID, verbs.c_str()));
}

VerbUpdatePacketClass::VerbUpdatePacketClass(RawPacketClass *raw) :
	GameID(0)
{
	raw->Get(GameID);
	Read_Verbs(raw, Drops);
	Read_Verbs(raw, Adds);
}

RawPacketClass *VerbUpdatePacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(Get_Type(), Get_Data_Size());
	raw->Add(GameID);
	Write_Verbs(raw, Drops);
	Write_Verbs(raw, Adds);
	return(raw);
}

int VerbUpdatePacketClass::Get_Data_Size()
{
	return((Adds.size() + Drops.size()) * sizeof(VerbEnum) + sizeof(GameID) + sizeof(int) + sizeof(int));
}

int VerbUpdatePacketClass::Print(char *buffer, int)
{
	std::string drops;
	std::string adds;
	Append_Verbs(drops, Drops);
	Append_Verbs(adds, Adds);
	return(sprintf(buffer, "%s { GameID=%d Drops=%s Adds=%s }", "VerbUpdate", GameID, drops.c_str(), adds.c_str()));
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
**	* The five functions that read, write or print a set (0x8AF150, 0x8AF6D0, 0x8AFD10, 0x8AFF40, 0x8B0150)
**	  are exact only through the static helpers at the top (F9, 2026-10-29; attempts.py show 0x8AFD10).
**	* 92's members are named from its Print: the set at +0x10 is printed under 'Drops=', +0x20 under
**	  'Adds=' (the arguments sprintf receives, 0x8B0150; the names were the other way round until F9).
**	* The file name is provisional (no assert names it: pk + its first named class's name).
*/
