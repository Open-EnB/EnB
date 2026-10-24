/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\shared\duration.h (the client's __FILE__ string is
**	"..\shared/duration.h": E&B compiled with ".." on the include path, see progress.md)
**
**	A time span: a signed 64-bit Time (the name is the FAIL_IF's, "Time > INT_MAX"). Only what the
**	delta files use so far; As_Int's FAIL_IF is on the client's line 190 (D25), so the lines above
**	it are kept free for the rest of the header (not rebuilt yet). The client doesn't name the
**	class (D24).
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __DURATION_H
#define __DURATION_H

#include <limits.h>
#include "unknown_debug.h"

class DurationClass
{
public:
	// (the rest of the class, not rebuilt yet, goes here: As_Int's FAIL_IF stays on line 190)






























































































































































	// Time as an int, clamped (0x4A8270 inlines it; the FAIL_IF reports the upper clamp only)
	int As_Int() const
	{
		FAIL_IF(Time > INT_MAX) return INT_MAX;
		if (Time < INT_MIN) {
			return INT_MIN;
		}
		return (int)Time;
	}

	__int64 Time;		// +0x00
};

#endif //__DURATION_H
