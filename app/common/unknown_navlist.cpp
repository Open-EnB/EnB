// Earth & Beyond client (reconstruction): the music types' names and the navigation list, client
// 0x8462A0-0x846A3F. PROVISIONAL file name (D10, with unknown_navlist.h). Notes at the end of the file.
#include "unknown_navlist.h"
#include "..\packets\pk.h"
#include <string.h>

static const char *MusicTypeNames[] = { "", "MusicAmbient", "MusicCombat", "MusicPlanet", "MusicWander" };	// 0xBAF63C  // NAME 2: what it holds

bool Music_Type_From_Name(const char *name, int *type)
{
	for (int i = 0; i < 5; i++) {
		if (stricmp(name, MusicTypeNames[i]) == 0) {
			*type = i + 1;
			return(true);
		}
	}
	return(false);
}

char *Music_Type_Name(char *buffer, int len, int type)
{
	strncpy(buffer, MusicTypeNames[type], len);
	return(buffer);
}

NavigationListClass::NavigationListClass() :
	Index(0)
{
}

NavigationListClass::NavigationListClass(const NavigationListClass &src) :
	List(src.List),
	Index(src.Index)
{
}

NavigationListClass::NavigationListClass(RawPacketClass *raw) :
	Index(0)
{
	unsigned short count;
	raw->Get(&count, sizeof(count));
	for (int i = 0; i < count; i++) {
		unsigned long id;
		raw->Get(&id, sizeof(id));
		List.push_back(id);
	}
}

NavigationListClass::~NavigationListClass()
{
}

void NavigationListClass::Add(RawPacketClass *raw)
{
	int count = List.size();
	raw->Add(&count, sizeof(short));
	for (std::vector<unsigned long>::iterator it = List.begin(); it != List.end(); ++it) {
		unsigned long id = *it;
		raw->Add(&id, sizeof(id));
	}
}

int NavigationListClass::Get_Data_Size()
{
	return(List.size() * sizeof(unsigned long) + sizeof(short));
}

unsigned long NavigationListClass::Get_Current()
{
	if ((unsigned int)Index >= List.size()) {
		return((unsigned long)-1);
	}
	return(List[Index]);
}

bool NavigationListClass::Is_Last()
{
	return(Index == (int)List.size() - 1);
}

void NavigationListClass::Clear()
{
	List.clear();
	Index = 0;
}

bool NavigationListClass::Set_Index(unsigned int index)
{
	if (index >= List.size()) {
		Index = -1;
		return(false);
	}
	Index = index;
	return(true);
}

bool NavigationListClass::Next()
{
	if (Index >= 0) {
		unsigned int next = Index + 1;
		if (next >= List.size()) {
			Index = -1;
		} else {
			Index = next;
			return(true);
		}
	}
	return(false);
}

bool NavigationListClass::Contains(unsigned long id, bool from_current)
{
	std::vector<unsigned long>::iterator it = List.begin();
	if (from_current) {
		if ((unsigned int)Index >= List.size()) {
			return(false);
		}
		it = List.begin() + Index;
	}
	for (; it != List.end(); ++it) {
		if (*it == id) {
			return(true);
		}
	}
	return(false);
}

unsigned long NavigationListClass::Get(int index)
{
	if (index < 0) {
		index += List.size();
		if (index < 0) {
			return(0);
		}
	} else if (index >= (int)List.size()) {
		return(0);
	}
	return(List[index]);
}


/*
**	Reconstruction notes (unknown_navlist.cpp)
**
**	* Client 0x8462A0-0x846A3F: manufacturing_defs.cpp's object ends at 0x846270 (the guarded static
**	  every <string> includer has, 0xC416FC, atexit 0x42B320), this one at 0x846A20. No assert or
**	  __FILE__ names it: the file name is provisional (D10).
**	* The music types' table 0xBAF63C ("", MusicAmbient, MusicCombat, MusicPlanet, MusicWander) and its
**	  two lookups have no caller in the client.
**	* NavigationListClass: +0x04 a std::vector<unsigned long> (its allocator byte at +0x04 is copied
**	  from an uninitialised temporary, as VC6's vector constructor does), +0x14 the cursor. WARP 155
**	  (pkwarpindex.cpp) sends one: a 2-byte count, then 4 bytes per id.
*/
