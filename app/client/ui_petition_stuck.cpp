// Earth & Beyond client (reconstruction): app\client\ui_petition_stuck.cpp, the petition's 'stuck' page over the
// entry window (ui_petition_entry.cpp). The client's asserts name the file; they sit on the client's lines (D25).
#include <string>
#include "ui_petition_stuck.h"
#include "client.h"
#include "..\packets\pkpetitionstuck.h"
#include "..\shared\g_assert.h"

// 0x5D7370
UIPetitionStuckClass::UIPetitionStuckClass(SClient *client, int unknown0B4) :
	UIPetitionEntryClass(client),
	Unknown0B4(unknown0B4)
{
}

// 0x5D73F0
UIPetitionStuckClass::~UIPetitionStuckClass()
{
}























// 0x5D7420
bool UIPetitionStuckClass::Unknown_A0()
{
	FAIL_IF(!Get_Client()) return(false);
	std::string subject;
	std::string details;
	Get_Text(subject, details);
	int ship_id; ((ClientClass *)Get_Client())->Send_Packet(&PetitionPacketClass(ship_id = ((ClientClass *)Get_Client())->ShipID, Unknown0B4, subject.c_str(), details.c_str(), 0)); // NAME 1 [ship_id]: neutral (D10)
	return(true);
}

/*
**	Notes.
**	Object 0x5D7370-0x5D762F: the adjustor thunks 0x5D7610 / 0x5D7620 of the second and third tables close it;
**	PingInterface's constructor 0x5D7630 follows. ui_petition.cpp makes the page.
**	0x5D7420 is exact: assign the signed ship ID at the packet argument site, preserving read
**	order and temporary lifetimes while recovering the retail register choice.
*/
