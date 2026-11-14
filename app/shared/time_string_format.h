// Earth & Beyond client (reconstruction): app\shared\time_string_format.h (NAME 3: the header
// name is assumed from the client's time_string_format.cpp). A duration as text: "2 days, 3 hours",
// the units from the largest down, a ", " between them (time_string_format.cpp, 0x4FE780-0x4FF7B0).

#ifndef __TIME_STRING_FORMAT_H  // NAME 3 [time_string_format.h]: see above
#define __TIME_STRING_FORMAT_H

#include <string>
#include "duration.h"

class TimeStringFormatClass  // NAME 3: by convention (D24) from the client's file time_string_format.cpp (D37)
{
public:
	// the static formatters (0x4FF390 ..) store the three limits and zero RemainingTime; Started is
	// left alone (set by Format)
	TimeStringFormatClass(const MillisecondsClass &largest, const MillisecondsClass &smallest, const MillisecondsClass &minimum)
		: Largest(largest), Smallest(smallest), Minimum(minimum), RemainingTime(0) {}

	std::string Format(const MillisecondsClass &time);  // NAME 2: what it does

private:
	bool Add_Unit(const MillisecondsClass &portion, int count, const std::string &name, bool plural);  // NAME 2: what it does
	std::string Unit_String(int count, const std::string &name, bool plural);  // NAME 2: what it does

	// one unit's part: its count, and the same span in milliseconds (the converting constructors
	// 0x4FF940 .. 0x4FFA30 and Get 0x4FFA80 .. 0x4FFBD0 are called from Format: this layer's)
	template<class UNIT> bool Add(const UNIT &unit, const std::string &name, bool plural)  // NAME 2: what it does
	{
		return Add_Unit(MillisecondsClass(unit), unit.Get(), name, plural);
	}

	MillisecondsClass Largest;			// +0x00 NAME 2: a unit this long or longer is left out (0: none)
	MillisecondsClass Smallest;			// +0x08 NAME 2: the text stops after a unit shorter than this
	MillisecondsClass Minimum;			// +0x10 NAME 2: a shorter time is shown as this
	std::string Result;					// +0x18 NAME 2: the text so far
	MillisecondsClass RemainingTime;	// +0x28 (the FAIL_IF's name, 'RemainingTime.Get() < 0')
	bool Started;						// +0x30 NAME 2: a unit with a nonzero count was added
};

std::string Time_String(const MillisecondsClass &time, const MillisecondsClass &largest, const MillisecondsClass &smallest, const MillisecondsClass &minimum);  // NAME 2: what it does
std::string Time_String(const DaysClass &time);
std::string Time_String(const HoursClass &time);
std::string Time_String(const MinutesClass &time);
std::string Time_String(const SecondsClass &time);
std::string Time_String(const MillisecondsClass &time);

#endif //__TIME_STRING_FORMAT_H
