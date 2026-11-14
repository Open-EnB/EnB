// Earth & Beyond client (reconstruction): app\shared\time_string_format.cpp (the client's
// __FILE__ 'e:\build\beta\G\app\shared\time_string_format.cpp', the FAIL_IF in 0x4FEDB0;
// 0x4FE780-0x4FF7B0, the duration.h instances to 0x4FFBD0). Notes at the end of the file.
#include <stdlib.h>
#include "g_assert.h"
#include "time_string_format.h"


// NOT EXACT: inline-budget (explain.py): the formatter's string constructor inlines _Tidy, the client calls it
// @0x4FE780
std::string Time_String(const MillisecondsClass &time, const MillisecondsClass &largest, const MillisecondsClass &smallest, const MillisecondsClass &minimum)
{
	TimeStringFormatClass format(largest, smallest, minimum);
	return format.Format(time);
}

// "1 day, 2 hours, 3 minutes, 4 seconds, 5ms": the units from the largest down
// NOT EXACT: inline-budget: two of the four unit conversions (0x4FF940, 0x4FF9E0) inline here, the client calls all four
// @0x4FE890
std::string TimeStringFormatClass::Format(const MillisecondsClass &time)
{
	if (time < Minimum) {
		return Format(Minimum);
	}
	Result = "";
	RemainingTime = time;
	Started = false;
	if (Add(DaysClass(RemainingTime), " day", true)
		&& Add(HoursClass(RemainingTime), " hour", true)
		&& Add(MinutesClass(RemainingTime), " minute", true)
		&& Add(SecondsClass(RemainingTime), " second", true)) {
		Add_Unit(MillisecondsClass(RemainingTime), RemainingTime.Get(), "ms", false);
	}
	return Result;
}

// false: stop (the last unit, or the FAIL_IF)
// NOT EXACT: inline-budget: Result's two appends inline here, the client calls append(ptr, n) and append(str, 0, npos)
// @0x4FEDB0
bool TimeStringFormatClass::Add_Unit(const MillisecondsClass &portion, int count, const std::string &name, bool plural)
{
	if (Largest.Get() != 0 && portion >= Largest) {
		return true;
	}
	RemainingTime -= portion;










	FAIL_IF(RemainingTime.Get() < 0) return false;
	if (portion.Get() != 0) {
		Started = true;
	}
	if (!Started) {
		return true;
	}
	std::string text = Unit_String(count, name, plural);
	if (Result.length() == 0) {
		Result = text;
	} else {
		Result += ", ";
		Result += text;
	}
	return portion >= Smallest;
}

// "1 day", "3 days" ("s" when plural)
// @0x4FF050
std::string TimeStringFormatClass::Unit_String(int count, const std::string &name, bool plural)
{
	if (count == 1) {
		return std::string("1") + name;
	}
	char buffer[64];
	itoa(count, buffer, 10);
	std::string text = std::string(buffer) + name;
	if (plural) {
		text += "s";
	}
	return text;
}

// days and hours, at least one day
// NOT EXACT: inline-budget (explain.py): the static formatter's string constructor inlines _Tidy, the client calls it
// @0x4FF390
std::string Time_String(const DaysClass &time)
{
	static TimeStringFormatClass format(0, 86400000, 86400000);
	return format.Format(time);
}

// days and hours, at least one hour
// NOT EXACT: inline-budget (explain.py): the static formatter's string constructor inlines _Tidy, the client calls it
// @0x4FF490
std::string Time_String(const HoursClass &time)
{
	static TimeStringFormatClass format(0, 86400000, 3600000);
	return format.Format(time);
}

// hours and minutes, at least one minute
// NOT EXACT: inline-budget (explain.py): the static formatter's string constructor inlines _Tidy, the client calls it
// @0x4FF590
std::string Time_String(const MinutesClass &time)
{
	static TimeStringFormatClass format(86400000, 3600000, 60000);
	return format.Format(time);
}

// minutes and seconds, at least one second
// NOT EXACT: inline-budget (explain.py): the static formatter's string constructor inlines _Tidy, the client calls it
// @0x4FF6A0
std::string Time_String(const SecondsClass &time)
{
	static TimeStringFormatClass format(3600000, 60000, 1000);
	return format.Format(time);
}

// "250ms"
// @0x4FF7B0
std::string Time_String(const MillisecondsClass &time)
{
	char buffer[64];
	itoa(time.Get(), buffer, 10);
	return std::string(buffer) + "ms";
}

/*
**	Reconstruction notes
**
**	The client's FAIL_IF text names RemainingTime and its accessor 'Get()': duration.h's Get (line 190),
**	renamed from As_Int for it. Nothing in client.exe calls these
**	functions (only their incremental-link thunks reach them), so the parameter types are by the
**	unit each formatter's limits use. The limits are milliseconds (86400000 a day).
*/
