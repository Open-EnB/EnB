// Earth & Beyond client (reconstruction): app\packets\pkglobalconnect.cpp. Notes at the end of the file.
#include "pkglobalconnect.h"
#include "nstrdup.h"
#include <stdio.h>
#include <string.h>

GlobalConnectPacketClass::GlobalConnectPacketClass(const char *ticket, const char *password)
{
	Ticket = nstrdup(ticket);
	Password = nstrdup(password);
}

GlobalConnectPacketClass::GlobalConnectPacketClass(RawPacketClass *raw)
{
	raw->Get(&Ticket);
	raw->Get(&Password);
}

GlobalConnectPacketClass::~GlobalConnectPacketClass()
{
	delete [] Ticket;
	delete [] Password;
}

RawPacketClass *GlobalConnectPacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(109, Get_Data_Size());
	raw->Add(Ticket);
	raw->Add(Password);
	return(raw);
}

int GlobalConnectPacketClass::Get_Data_Size()
{
	return(strlen(Password) + strlen(Ticket) + 2 * (sizeof(int) + 1));
}

int GlobalConnectPacketClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "Global Connect"));
}


/*
**	Reconstruction notes (pkglobalconnect.cpp)
**
**	* Client 0x863400-0x8636F0 (0x8633D0 is the previous object's guarded static, 0xC416FC; this one's is 0x8636F0):
**	  opcode 109, written by hand (builder P4, 2026-10-27). The value constructor comes first.
**	* The file name is provisional (no assert names it).
*/
