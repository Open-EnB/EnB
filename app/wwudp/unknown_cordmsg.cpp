/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	PROVISIONAL file name (no assert or __FILE__ names it): the object 0x85AEC0-0x85C600 after
**	cordmgr.cpp, which holds CordMessageClass's out-of-line members (0x85BE00-0x85BEE7) and the
**	connection statistics CordCon keeps at +0x68 (a 0x4C-byte object: constructor 0x85BF30,
**	0x85C090-0x85C5F0: round trip, bandwidth estimate, the send limit pumpInternal asks 0x85C430;
**	not rebuilt).
*/

// NAME 1 [unknown_cordmsg.cpp]: PROVISIONAL file name (no assert or __FILE__ names the object)
#include "cordcon.h"
#include <time.h>

// 0x85BE00: the send time (a Wtime) zero, the receive time now.
CordMessageClass::CordMessageClass()
{
	SentTime[0] = 0;
	SentTime[1] = 0;
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

// 0x85BEA0: the client's only destroys its Wtime. The data stays: Free releases it.
CordMessageClass::~CordMessageClass()
{
}

// 0x85BEC0
void CordMessageClass::Free()
{
	delete [] Data;
	Data = 0;
	delete [] Fragments;
	Fragments = 0;
}
