// Earth & Beyond client (reconstruction): app\packets\pkgloballogin.cpp. Notes at the end of the file.
#include "pkgloballogin.h"
#include <stdio.h>

GlobalLoginPacketClass::GlobalLoginPacketClass(const LoginTicketInfoClass &info, bool referral_available, int total_referral_points, int spent_referral_points) :
	Ticket_Info(info),
	Referral_Available(referral_available),
	Total_Referral_Points(total_referral_points),
	Spent_Referral_Points(spent_referral_points)
{
}

GlobalLoginPacketClass::GlobalLoginPacketClass(RawPacketClass *raw)
{
	raw->Get(&Ticket_Info.EA_Customer);
	raw->Get(&Ticket_Info.EA_Ticket);
	raw->Get(*(__int64 *)&Ticket_Info.Unknown20);
	raw->Get(&Ticket_Info.Metrics_ID);
	raw->Get(&Ticket_Info.Login_Key);
	raw->Get(&Ticket_Info.EMail);
	raw->Get(&Ticket_Info.Alias);
	raw->Get(&Ticket_Info.Entitlement_Expiry);
	raw->Get(&Ticket_Info.Approval_Code);
	raw->Get(&Ticket_Info.Approval_Message);
	raw->Get(Ticket_Info.Create_Time);
	raw->Get(Referral_Available);
	raw->Get(Total_Referral_Points);
	raw->Get(Spent_Referral_Points);
}

GlobalLoginPacketClass::~GlobalLoginPacketClass()
{
}

RawPacketClass *GlobalLoginPacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(118, Get_Data_Size());
	raw->Add(Ticket_Info.EA_Customer);
	raw->Add(Ticket_Info.EA_Ticket);
	raw->Add(*(__int64 *)&Ticket_Info.Unknown20);
	raw->Add(Ticket_Info.Metrics_ID);
	raw->Add(Ticket_Info.Login_Key);
	raw->Add(Ticket_Info.EMail);
	raw->Add(Ticket_Info.Alias);
	raw->Add(Ticket_Info.Entitlement_Expiry);
	raw->Add(Ticket_Info.Approval_Code);
	raw->Add(Ticket_Info.Approval_Message);
	raw->Add(Ticket_Info.Create_Time);
	raw->Add(Referral_Available);
	raw->Add(Total_Referral_Points);
	raw->Add(Spent_Referral_Points);
	return(raw);
}

int GlobalLoginPacketClass::Get_Data_Size()
{
	return(RawPacketClass::Get_Size(Ticket_Info.EA_Customer) + RawPacketClass::Get_Size(Ticket_Info.EA_Ticket) + sizeof(__int64) +
		RawPacketClass::Get_Size(Ticket_Info.Metrics_ID) + RawPacketClass::Get_Size(Ticket_Info.Login_Key) +
		RawPacketClass::Get_Size(Ticket_Info.EMail) + RawPacketClass::Get_Size(Ticket_Info.Alias) +
		RawPacketClass::Get_Size(Ticket_Info.Entitlement_Expiry) + RawPacketClass::Get_Size(Ticket_Info.Approval_Code) +
		RawPacketClass::Get_Size(Ticket_Info.Approval_Message) + sizeof(Ticket_Info.Create_Time) + sizeof(Referral_Available) +
		sizeof(Total_Referral_Points) + sizeof(Spent_Referral_Points));
}

int GlobalLoginPacketClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s : EACustomer=%s  EATicket=%s  MetricsID=%s  LKey=%s  EMail=%s  Alias=%s  EntitlementExpiry=%s  "
		"CreateTime=%ld  ApprovalCode=%s  ApprovalMessage=%s  ReferralAvailable=%s  TotalReferralPoints=%d  SpentReferralPoints=%d",
		"Global Login", Ticket_Info.EA_Customer.c_str(), Ticket_Info.EA_Ticket.c_str(), Ticket_Info.Metrics_ID.c_str(),
		Ticket_Info.Login_Key.c_str(), Ticket_Info.EMail.c_str(), Ticket_Info.Alias.c_str(), Ticket_Info.Entitlement_Expiry.c_str(),
		Ticket_Info.Create_Time, Ticket_Info.Approval_Code.c_str(), Ticket_Info.Approval_Message.c_str(),
		Referral_Available ? "YES" : "NO", Total_Referral_Points, Spent_Referral_Points));
}


/*
**	Reconstruction notes (pkgloballogin.cpp)
**
**	* Client 0x865D60-0x866770 (each object ends with <string>'s guarded static 0xC416FC): opcode 118, written
**	  by hand (builder P9, 2026-10-27): the login ticket information (LoginTicketInfoClass, nine strings, an
**	  __int64 kept as two ints as tsk_login.cpp has it, the creation time) and the referral points.
**	  The destructor is declared (the client's 0x8662B0 stores the class's own vtable first). Process_Packet
**	  is tsk_login.cpp's (0x76B9C0).
**	* The file name is provisional (no assert names it).
*/
