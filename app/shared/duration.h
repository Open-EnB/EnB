// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\shared\duration.h (the client's __FILE__ string is
**	"..\shared/duration.h": E&B compiled with ".." on the include path, see progress.md)
**
**	A time span: a signed 64-bit Time (the name is the FAIL_IF's, "Time > INT_MAX") counted in
**	units of UNIT milliseconds. Get's FAIL_IF is on the client's line 190 (D25), so the lines
**	above it are kept free for the rest of the header (not rebuilt yet). The client doesn't name
**	the classes (D24). Notes at the end of the file.
*/



#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __DURATION_H
#define __DURATION_H

#include <limits.h>
#include "g_assert.h"

template<int UNIT> class DurationTemplateClass  // NAME 3: by convention (D24) from the client's file duration.h (D37)
{
public:
	DurationTemplateClass() {}
	DurationTemplateClass(__int64 time) : Time(time) {}
	template<int FROM> DurationTemplateClass(const DurationTemplateClass<FROM> &that) : Time(0)
	{
		Time = that.Time * FROM / UNIT;
	}

	bool operator<(const DurationTemplateClass &that) const { return Time < that.Time; }
	bool operator>=(const DurationTemplateClass &that) const { return Time >= that.Time; }
	DurationTemplateClass operator-(const DurationTemplateClass &that) const { return DurationTemplateClass(Time - that.Time); }
	DurationTemplateClass &operator-=(const DurationTemplateClass &that) { Time -= that.Time; return *this; }

	// (the rest of the class, not rebuilt yet: Get's FAIL_IF stays on line 190)


















































































































































	// Time as an int, clamped (0x4A8270 inlines it; the FAIL_IF reports the upper clamp only)
	int Get() const  // the client's name (time_string_format.cpp's FAIL_IF, 'RemainingTime.Get() < 0')
	{
		FAIL_IF(Time > INT_MAX) return INT_MAX;
		if (Time < INT_MIN) {
			return INT_MIN;
		}
		return (int)Time;
	}

	__int64 Time;		// +0x00
};

typedef DurationTemplateClass<1> MillisecondsClass;		// NAME 2: the unit of DurationClass
typedef DurationTemplateClass<1000> SecondsClass;		// NAME 2: the unit (time_string_format.cpp's " second")
typedef DurationTemplateClass<60000> MinutesClass;		// NAME 2: the unit (" minute")
typedef DurationTemplateClass<3600000> HoursClass;		// NAME 2: the unit (" hour")
typedef DurationTemplateClass<86400000> DaysClass;		// NAME 2: the unit (" day")

class DurationClass : public MillisecondsClass  // NAME 3: by convention (D24) from the client's file duration.h (D37)
{
public:
	DurationClass() {}
	DurationClass(__int64 time) : MillisecondsClass(time) {}

	// user-defined (the interpolator's Set 0x4A6590 stores both halves before the vtable load;
	// the implicit copy schedules the stores after it)
	DurationClass &operator=(const DurationClass &that) { Time = that.Time; return *this; }

	bool operator==(const DurationClass &that) const { return Time == that.Time; }
	bool operator!=(const DurationClass &that) const { return Time != that.Time; }
	bool operator<(const DurationClass &that) const { return Time < that.Time; }
	bool operator>=(const DurationClass &that) const { return Time >= that.Time; }
	DurationClass operator-(const DurationClass &that) const { return DurationClass(Time - that.Time); }
};

#endif //__DURATION_H

/*
**	Reconstruction notes
**
**	One template for every unit: time_string_format.obj holds four out-of-line copies of Get
**	(0x4FFA80, 0x4FFAF0, 0x4FFB60, 0x4FFBD0), each with the FAIL_IF on line 190, called on the day,
**	hour, minute and second counts, and four converting constructors to milliseconds (0x4FF940
**	.. 0x4FFA30: Time = 0, then Time = count * unit); one header line can only give one member
**	function, so Get is a member of a class template, instantiated per unit. DurationClass (the
**	delta files' type) keeps its own operators on the millisecond instance.
*/
