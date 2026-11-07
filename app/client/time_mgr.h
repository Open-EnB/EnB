// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location assumed: e:\build\beta\G\app\Client\time_mgr.h, from time_mgr.cpp's __FILE__ 0xBA7EA4;
**	the client holds no string of this header.
**
**	The server's clock as of login (0x769E70 sets it from the login's time), and one seasonal test on it.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __TIME_MGR_H
#define __TIME_MGR_H

#include <time.h>

class TimeMgrClass	// NAME 3: D24, the client's file time_mgr.cpp
{
public:
	TimeMgrClass() : Time(0), Dirty(true), Result(false) {}
	virtual ~TimeMgrClass();

	static TimeMgrClass *Get_Instance();	// 0x8190D0  // NAME 2: a local static's address

	void Set_Time(time_t time);																// 0x8191B0  // NAME 2: describes what it does
	bool In_Date_Range(int month1, int day1, int hour1, int month2, int day2, int hour2);	// 0x8191E0  // NAME 2: describes what it does
	bool Is_Unknown_819280();																	// 0x819280, in 30 Oct 17:00 - 3 Nov 17:00 (Halloween?)  // NAME 1: neutral (D10)

private:
	time_t Time;	// +0x04  // NAME 2: 0x8191E0's gmtime
	bool Dirty;		// +0x08  // NAME 2: Result is out of date
	bool Result;	// +0x09  // NAME 2: 0x819280's cached answer
};

#endif
