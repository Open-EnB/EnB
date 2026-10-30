// Earth & Beyond client (reconstruction): app\packets\pkglobalavatarlist.cpp. Notes at the end of the file.
#include "pkglobalavatarlist.h"
#include <stdio.h>

GlobalAvatarListPacketClass::GlobalAvatarListPacketClass(LoginCharacterClass *characters, AvatarDescClass *avatars, const std::list<LoginGalaxyClass> *galaxies)
{
	if (characters == NULL) {
		characters = new LoginCharacterClass[5];
	}
	Characters = characters;
	if (avatars == NULL) {
		avatars = new AvatarDescClass[5];
	}
	Avatars = avatars;
	if (galaxies != NULL) {
		Galaxies = *galaxies;
	}
}

GlobalAvatarListPacketClass::GlobalAvatarListPacketClass(RawPacketClass *raw)
{
	Characters = new LoginCharacterClass[5];
	Avatars = new AvatarDescClass[5];
	for (int i = 0; i < 5; i++) {
		raw->Get(&Characters[i]);
		raw->Get(&Avatars[i]);
	}
	int count = 0;
	raw->Get(count);
	LoginGalaxyClass galaxy;
	for (int j = 0; j < count; j++) {
		raw->Get(&galaxy);
		Galaxies.push_back(galaxy);
	}
}

GlobalAvatarListPacketClass::~GlobalAvatarListPacketClass()
{
	delete [] Characters;
	delete [] Avatars;
}

RawPacketClass *GlobalAvatarListPacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(112, Get_Data_Size());
	for (int i = 0; i < 5; i++) {
		raw->Add(Characters[i]);
		raw->Add(Avatars[i]);
	}
	int count = Galaxies.size();
	raw->Add(count);
	for (std::list<LoginGalaxyClass>::iterator it = Galaxies.begin(); it != Galaxies.end(); it++) {
		raw->Add(*it);
	}
	return(raw);
}

int GlobalAvatarListPacketClass::Get_Data_Size()
{
	int size = 0;
	for (int i = 0; i < 5; i++) {
		size += ((Unknown_PacketDataClass &)Characters[i]).Get_Size();
		size += ((Unknown_PacketDataClass &)Avatars[i]).Get_Size();
	}
	size += sizeof(int);
	for (std::list<LoginGalaxyClass>::iterator it = Galaxies.begin(); it != Galaxies.end(); it++) {
		size += ((Unknown_PacketDataClass &)*it).Get_Size();
	}
	return(size);
}

int GlobalAvatarListPacketClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s", "Avatar List"));
}


/*
**	Reconstruction notes (pkglobalavatarlist.cpp)
**
**	* Client 0x8629A0-0x8633D0 (each object ends with <string>'s guarded static 0xC416FC): GLOBAL_AVATAR_LIST
**	  112, written by hand (builder P9, 2026-10-27): the five character slots and five avatar descriptions
**	  (new[]'d arrays, deleted by the declared destructor) and the galaxies, a count then each one raw.
**	  The object also holds the two arrays' vector deleting destructors (0x862C90, 0x862D10) and
**	  LoginGalaxyClass's implicit copy (0x863360). Process_Packet is tsk_login.cpp's (0x76B520).
**	* The file name is provisional (no assert names it).
*/
