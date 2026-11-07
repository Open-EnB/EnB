// targeting.cpp: TargetFindClass's selection through the server (0x7C6120-0x7C636F); notes at the end
#include "targeting.h"
#include "sclient.h"
#include "client.h"
#include "co.h"
#include "..\packets\pkrequesttarget.h"

// @0x7C6120
TargetingClass::TargetingClass(SClient *client) :
	TargetFindClass(client),
	Unknown0C(0),
	Unknown10(false)
{
}

// @0x7C6190
TargetingClass::~TargetingClass()
{
}

// @0x7C61B0
void TargetingClass::Clear_Target()
{
	Select(NULL);
}

// @0x7C61D0
ClientObjectClass *TargetingClass::Get_Target()
{
	return(Client->Get_Target());
}

// @0x7C61F0
ClientObjectClass *TargetingClass::Select_Next_Target()
{
	return(Select_Next(Get_Target()));
}

// @0x7C6220
ClientObjectClass *TargetingClass::Select_Prev_Target()
{
	return(Select_Prev(Get_Target()));
}

// @0x7C6250
ClientObjectClass *TargetingClass::Select_Next_Target_Filtered()
{
	return(Select_Next_Filtered(Get_Target()));
}

// @0x7C6280
ClientObjectClass *TargetingClass::Select_Prev_Target_Filtered()
{
	return(Select_Prev_Filtered(Get_Target()));
}

// @0x7C62B0
ClientObjectClass *TargetingClass::Select(ClientObjectClass *co)
{
	if (co != Get_Target()) {
		Client->Send_Packet(&RequestTargetPacketClass(Client->Get_Avatar_ID(), co ? co->Get_Game_ID() : 0));
		Unknown10 = true;
	}
	return(co);
}

/*
**	Notes
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: unknown, no __FILE__ in the object; the name comes from the callers' asserts ('targeting').
**	PLACEMENT: provisional (D137): revisit if the client's name turns up.
**
**	The object: it starts after target_reticle.cpp's <string> initialiser 0x7C60F0 (guard 0xC416FC) and ends with its own,
**	0x7C6350. 0x7C6380 / 0x7C63B0 (the PermStringInitClass at 0xBE87E4 and its atexit) open the next object, ui_starmap.cpp:
**	an object's header statics come first, as in target_find.cpp (0x7C3EC0) and target_reticle.cpp (0x7C50B0).
*/
