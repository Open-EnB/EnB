// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\common\group_common.h (PROVISIONAL, D137: group_common.cpp's header, after its
**	__FILE__ 'e:\build\beta\G\app\Common\group_common.cpp' 0xBA9A84; the client holds no string of it)
**
**	A group's access as text: 'group', 'restricted', 'public' ('private' for any other value). No client code calls
**	the converters.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef GROUP_COMMON_H  // NAME 3 [group_common.h]: the header of group_common.cpp
#define GROUP_COMMON_H

enum GroupAccessType  // NAME 2 [GroupAccessType]: what its names describe
{
	GROUP_ACCESS_GROUP = 0,			// 'group'  // NAME 2: its string
	GROUP_ACCESS_RESTRICTED = 1,	// 'restricted'  // NAME 2: its string
	GROUP_ACCESS_PUBLIC = 2			// 'public'  // NAME 2: its string
};

char *Value_To_String(char *buf, int size, GroupAccessType value);	// 0x83D6A0 (G_ASSERT '0' l.18)  // NAME 2: as strconv_lib's
bool String_To_Value(const char *text, GroupAccessType *value);		// 0x83D740  // NAME 2: as strconv_lib's

#endif
