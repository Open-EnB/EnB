// Earth & Beyond client (reconstruction): app\packets\pkitemstate.cpp. Notes at the end of the file.
#include "pkitemstate.h"
#include <stdio.h>


ItemStatePacketClass::ItemStatePacketClass(RawPacketClass *raw)
{
	raw->Get(&ID, sizeof(ID));
	raw->Get(&State, sizeof(State));
	raw->Get(&BoolVal, sizeof(BoolVal));
	raw->Get(&Inventory, sizeof(Inventory));
	raw->Get(&Slot, sizeof(Slot));
}

RawPacketClass *ItemStatePacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(41, Get_Data_Size());
	raw->Add(&ID, sizeof(ID));
	raw->Add(&State, sizeof(State));
	raw->Add(&BoolVal, sizeof(BoolVal));
	raw->Add(&Inventory, sizeof(Inventory));
	raw->Add(&Slot, sizeof(Slot));
	return(raw);
}

int ItemStatePacketClass::Get_Data_Size()
{
	return(sizeof(ID) + sizeof(State) + sizeof(BoolVal) + sizeof(Inventory) + sizeof(Slot));
}

int ItemStatePacketClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s {ID=%d Inventory=%d Slot=%d State=%d BoolVal=%d}", "ItemStates", ID, Inventory, Slot, State, BoolVal));
}


/*
**	Reconstruction notes (pkitemstate.cpp)
**
**	* Client 0x895C00-0x895ED0 (the object ends with the guarded static every <string> includer has,
**	  0xC416FC, atexit 0x42B320). Drafted from the client by the packet decoder (tools/re/pkdecode.py,
**	  pkemit.py): members from the raw message's Get / Build's Add (sizes) and Print (types,
**	  names: 'Name=%d'), the constructors' stores in the initialiser list.
**	* The file name is provisional (provisional, pk + its first named class's name).
*/
