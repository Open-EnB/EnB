// Earth & Beyond client (reconstruction): app\shared\unknown_4e0700.cpp. Notes at the end of the file.
#include "unknown_4e0700.h"

int Detach_Owner_Mods(AttribDir *dir, AttribModIterator mods, void *owner, int keep)
{
	int count = 0;
	while (!mods.Is_Done()) {
		AttribModClass *mod = mods.Get_Mod();
		if (mod->Get_Owner() == owner) {
			count++;
			mods = dir->Detach_Mod(mods);
			if (!keep) {
				delete mod;
			}
		} else {
			mods.Next();
		}
	}
	return count;
}


/*
**	Reconstruction notes
**
**	client.exe 0x4E0700-0x4E082F, one object between 0x4E06D0's and 0x4E0810's <string> guarded
**	initialisers (the second ends it; chat_filter.cpp's object follows at 0x4E0840). No string of its
**	own: the file name is provisional (D137). Called by AttribDirCollective's destructor and
**	Detach_Owned_Mods with keep 0; keep is tested as a whole dword (an int, not a bool). Exact
**	(appfile.py match: PREFIX of a record that runs into the initialiser).
*/
