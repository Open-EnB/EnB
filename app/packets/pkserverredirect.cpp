// Earth & Beyond client (reconstruction): app\packets\pkserverredirect.cpp. Notes at the end of the file.
#include "pkserverredirect.h"
#include <winsock.h>
#include <stdio.h>

ServerRedirectPacketClass::ServerRedirectPacketClass(int sector, int ip, unsigned short port) :
	Packet_003BClass(sector, ip, port)
{
}

ServerRedirectPacketClass::ServerRedirectPacketClass(RawPacketClass *raw) :
	Packet_003BClass(raw)
{
}

int ServerRedirectPacketClass::Get_Type()
{
	return(54);
}

int ServerRedirectPacketClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s {IP=%x Port=%hd}", "Client_Redirect", ntohl(IP), ntohs(Port)));
}


/*
**	Reconstruction notes (pkserverredirect.cpp)
**
**	* Client 0x8A1500-0x8A1630 (0x8A14D0 is the previous object's guarded static, 0xC416FC; this one's is
**	  0x8A1630): SERVER_REDIRECT 54 written by hand (builder A-opus, 2026-10-27). The class is Server_Ready
**	  59's (pkserverready.cpp) with its own type and Print: the constructors call 59's out of line, the
**	  destructor (0x8A1570) is the implicit one.
**	* The file name is provisional (no assert names it).
*/
