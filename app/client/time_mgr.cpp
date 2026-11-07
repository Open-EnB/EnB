// Earth & Beyond client (reconstruction): app\Client\time_mgr.cpp (__FILE__ 0xBA7EA4). Notes at the end.
#include "time_mgr.h"
#include "..\shared\g_assert.h"

// 0x8190D0
TimeMgrClass *TimeMgrClass::Get_Instance()
{
	static TimeMgrClass _instance;	// 0xBE8E68  // NAME 2: describes what it does
	return(&_instance);
}

// 0x819190
TimeMgrClass::~TimeMgrClass()
{
}

// 0x8191B0
void TimeMgrClass::Set_Time(time_t time)
{
	Time = time;
	if (time < 0) {
		Time = 0;
	}
	Dirty = true;
}

// 0x8191E0
bool TimeMgrClass::In_Date_Range(int month1, int day1, int hour1, int month2, int day2, int hour2)
{
	struct tm *now = gmtime(&Time);


	FAIL_IF(!now) return(false);
	int month = now->tm_mon + 1;
	int day = now->tm_mday;
	int hour = now->tm_hour;
	if (month < month1 || month > month2) {
		return(false);
	}
	if (month == month1) {
		if (day < day1) {
			return(false);
		}
		if (day == day1 && hour < hour1) {
			return(false);
		}
	}
	if (month == month2) {
		if (day > day2) {
			return(false);
		}
		if (day == day2 && hour > hour2) {
			return(false);
		}
	}
	return(true);
}

// 0x819280
bool TimeMgrClass::Is_Unknown_819280()
{
	if (Dirty) {
		Result = In_Date_Range(10, 30, 17, 11, 3, 17);
		Dirty = false;
	}
	return(Result);
}

/*
**	Reconstruction notes.
**
**	The object runs from 0x8190D0, right after skillnam.cpp's <string> initialiser (0x8190A0), to 0x8192A7. 0x8192C0 (a vsprintf
**	into a std::string) and 0x819420 (a gadget-like constructor) are not attributed here. 0x819140 is the deleting destructor,
**	0x819170 the static's atexit stub.
*/
