// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pkgloballogin.h (the name provisional: see the .cpp)
**
**	The packet class of client 0x865D60-0x866770 (opcode 118) and the login ticket information it carries
**	(LoginTicketInfoClass, formerly tsk_login.h's).
*/



#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKGLOBALLOGIN_H  // NAME 1 [pkgloballogin]: provisional, pk + its first named class's name
#define PKGLOBALLOGIN_H

#include "pk.h"
#include <string>

// What GlobalLoginPacketClass carries and its handler 0x76B170 copies to the login task (0xA0 bytes: the __int64 at +0x20 aligns it to 8, ui_login.cpp's 0x5AEA60 copies it so; step 16 copies it whole)
struct LoginTicketInfoClass	// NAME 1: what it holds
{
	LoginTicketInfoClass() : Unknown20(0) {}				// (LoginTaskClass's constructors)
	std::string EA_Customer;							// +0x00 NAME 3: the client's Print text 'EACustomer='
	std::string EA_Ticket;								// +0x10 NAME 3: the client's Print text 'EATicket='
	__int64 Unknown20;									// +0x20 (an __int64 on the wire; ui_login.cpp's 0x5AEA60 copies it as one)
														// (+0x9C: the tail padding to 0xA0)
	std::string Metrics_ID;								// +0x28 NAME 3: the client's Print text 'MetricsID='
	std::string Login_Key;								// +0x38 NAME 1: ClientTaskClass::Init's login key ('lkey')
	std::string EMail;									// +0x48 NAME 3: the client's Print text 'EMail='
	std::string Alias;									// +0x58 NAME 3: the client's Print text 'Alias='
	std::string Entitlement_Expiry;						// +0x68 NAME 3: the client's Print text 'EntitlementExpiry='
	std::string Approval_Code;							// +0x78 NAME 3: the client's Print text 'ApprovalCode='
	std::string Approval_Message;						// +0x88 NAME 3: the client's Print text 'ApprovalMessage='
	int Create_Time;									// +0x98: handed to 0x8190D0 (step 16)  // NAME 3: the client's Print text 'CreateTime='
};

// opcode 118 (0x0076, ?): vtable 0xB095C4, constructor 0x866050, 0xC0 bytes
class GlobalLoginPacketClass : public PacketClass  // NAME 2: tsk_login.cpp's log 'GlobalLoginPacketClass recv' (the client's Print text 'Global Login')
{
public:
	GlobalLoginPacketClass(const LoginTicketInfoClass &info, bool referral_available, int total_referral_points, int spent_referral_points);	// 0x865D60
	GlobalLoginPacketClass(RawPacketClass *raw);	// 0x866050
	virtual ~GlobalLoginPacketClass();	// +0x00 0x8662B0 / 0x866020
	virtual RawPacketClass *Build();	// +0x04 0x866420
	virtual bool Process_Packet(void *client);	// +0x08 0x76B9C0 (tsk_login.cpp)
	virtual int Get_Data_Size();	// +0x0C 0x8665A0
	virtual int Print(char *buffer, int len);	// +0x10 0x866640

	bool Is_Referral_Available() const { return Referral_Available != 0; }	// (inline in Handle_Global_Login)  // NAME 2: what it returns
	int Get_Referral_Points() const { return Total_Referral_Points - Spent_Referral_Points; }	// (inline in Handle_Global_Login)  // NAME 2: what it returns

	int Unknown0C;	// +0x0C (never set)  // NAME 1: neutral (D10)
	LoginTicketInfoClass Ticket_Info;	// +0x10  // NAME 1: copied to the login task's
	// +0xAC: Ticket_Info's tail padding (never set; Ticket_Info is 0xA0 bytes)
	short Referral_Available;	// +0xB0  // NAME 3: the client's Print text 'ReferralAvailable=' (YES / NO)
	int Total_Referral_Points;	// +0xB4  // NAME 3: the client's Print text 'TotalReferralPoints='
	int Spent_Referral_Points;	// +0xB8  // NAME 3: the client's Print text 'SpentReferralPoints='
	int Unknown0BC;	// +0xBC (never set: the class is 0xC0 bytes)  // NAME 1: neutral (D10)
};

#endif
