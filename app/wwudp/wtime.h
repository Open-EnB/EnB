/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	The time value CordCon and its statistics use: Westwood's wlib Wtime (libs\wwonline, the
**	library beside wnet's UDP at 0x8BA300-0x8BB7A0). Its interface is EA's (Generals Tools\matchbot
**	\wlib\wtime.h), but E&B's copy keeps a different pair of fields: the day (days since year 0:
**	Update 0x8BA6F0 stores ftime's seconds / 86400 + 0xAFAA8) and the millisecond of the day
**	(< 86,400,000), where matchbot's keeps seconds and microseconds. CordCon's code reads them as
**	such: Set_Msec(rtt * 2 + 50) for the resend timeout, "elapsed Get_Msec() > 999" for the
**	per-second bandwidth count, "elapsed Get_Day() == 0" before a round-trip sample.
**
**	This header declares only what the wwudp code calls (out of line, as the client calls it), so
**	that a program links either wlib's wtime.cpp (not imported yet: WWOnline is an open decision)
**	or a stand-in (tests/capharness/udpreplay/wtime_replay.cpp: a clock the harness sets).
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef WTIME_H  // NAME 4 [wtime.h]: wlib's wtime.h (EA's Generals Tools matchbot wlib); its place here assumed
#define WTIME_H

class Wtime  // NAME 4: wlib's name (EA's Generals Tools matchbot wlib)
{
public:
	Wtime();								// 0x8BA300: now (Update)
	Wtime(const Wtime &other);				// 0x8BA420
	Wtime(unsigned int seconds);			// 0x8BA450: from a time_t
	~Wtime();								// 0x8BA4F0

	void Update();							// 0x8BA6F0: now (ftime)
	unsigned int Get_Time_T() const;		// 0x8BA510: as a time_t  // NAME 1: describes what it returns
	unsigned int Get_Day() const;			// 0x8BA810: the first field (matchbot's GetSec)  // NAME 2: what E&B's field holds
	unsigned int Get_Msec() const;			// 0x8BA830: the second field (matchbot's GetUsec)  // NAME 2: what E&B's field holds
	void Set_Day(unsigned int day);			// 0x8BA850 (matchbot's SetSec)  // NAME 2: what E&B's field holds
	void Set_Msec(unsigned int msec);		// 0x8BA870 (matchbot's SetUsec)  // NAME 2: what E&B's field holds

	bool operator < (const Wtime &other) const;		// 0x8BB3E0
	bool operator > (const Wtime &other) const;		// 0x8BB430
	Wtime &operator -= (const Wtime &other);		// 0x8BB5A0
	Wtime operator - (const Wtime &other) const;	// 0x8BB620
	Wtime operator + (const Wtime &other) const;	// 0x8BB6A0
	Wtime &operator = (const Wtime &other);			// 0x8BB720

private:
	unsigned int Day;	// NAME 2: +0x00 days since year 0 (matchbot's sec)
	unsigned int Msec;	// NAME 2: +0x04 the millisecond of the day (matchbot's usec)
};

#endif
