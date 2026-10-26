// Earth & Beyond client (reconstruction): app\packets\pkopeninterface.cpp. Notes at the end of the file.
#include "pkopeninterface.h"
#include <stdio.h>


OpenInterfacePacketClass::OpenInterfacePacketClass(int ui_change, int ui_type) : UIChange(ui_change), UIType(ui_type)
{
}

OpenInterfacePacketClass::OpenInterfacePacketClass(RawPacketClass *raw)
{
	raw->Get(&UIChange, sizeof(UIChange));
	raw->Get(&UIType, sizeof(UIType));
}

RawPacketClass *OpenInterfacePacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(102, Get_Data_Size());
	raw->Add(&UIChange, sizeof(UIChange));
	raw->Add(&UIType, sizeof(UIType));
	return(raw);
}

int OpenInterfacePacketClass::Get_Data_Size()
{
	return(sizeof(UIChange) + sizeof(UIType));
}

int OpenInterfacePacketClass::Print(char *buffer, int len)
{
	return(_snprintf(buffer, len, "%s {UIChange=%d UIType=%d}", "Open_Interface", UIChange, UIType));
}


/*
**	Reconstruction notes (pkopeninterface.cpp)
**
**	* Client 0x89F1F0-0x89F460 (the object ends with the guarded static every <string> includer has,
**	  0xC416FC, atexit 0x42B320). Drafted from the client by the packet decoder (builder B,
**	  2026-10-25): members from the raw message's Get / Build's Add (sizes) and Print (types,
**	  names: 'Name=%d'), the constructors' stores in the initialiser list.
**	* The file name is provisional (provisional, pk + its first named class's name).
*/
