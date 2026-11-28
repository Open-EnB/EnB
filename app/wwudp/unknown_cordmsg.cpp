// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	PROVISIONAL file name (no assert or __FILE__ names it): the object 0x85AEC0-0x85C600 after
**	cordmgr.cpp (SG, run 2026-11-18: two objects by the CRT table: this one ends with <string>'s guard 0x85BF00,
**	entry 793; CordStatsClass is unknown_85bf30.cpp, guard 0x85C5C0, entry 794), which holds CordMessage's out-of-line members (0x85BE00-0x85BEE7) and the
**	connection's send window and statistics CordCon keeps at +0x68 (CordStatsClass, 0x4C bytes:
**	0x85BF30-0x85C5E0). Xtime is wlib's (libs\wwonline\wlib, D73).
*/



// NAME 1 [unknown_cordmsg.cpp]: PROVISIONAL file name (no assert or __FILE__ names the object)
#include "cordcon.h"
#include <time.h>
#include <string>	// the client object ends with <string>'s guard 0x85BF00 (CRT entry 793)
// 0x85BE00: never sent, received now.
CordMessage::CordMessage() :
	SentTime(0)
{
	SentTime.setDay(0);
	SentTime.setMsec(0);
	SendCount = 0;
	Length = 0;
	TotalLength = 0;
	Priority = 0;
	Seq = 0;
	Data = 0;
	Fragments = 0;
	Time = (unsigned int)time(0);
	Flags = 0;
	FragIndex = 0;
}

// 0x85BEA0: the client's only destroys its Xtime. The data stays: Free releases it.
CordMessage::~CordMessage()
{
}

// 0x85BEC0
void CordMessage::Free()
{
	delete [] Data;
	Data = 0;
	delete [] Fragments;
	Fragments = 0;
}
