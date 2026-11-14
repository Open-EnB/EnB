// Earth & Beyond client (reconstruction): app\Common\guild_defs.cpp (__FILE__ 0xBA9AB8). Notes at the end of the file.
#include <string>
#include <stdio.h>
#define __GUILD_DEFS_ALL
#include "guild_defs.h"
#include "..\shared\g_assert.h"











static inline bool Is_Valid_Guild_Rank(int v) { return(v >= GUILD_RANK_NONE && v < NUM_GUILD_RANKS); }	// inlined into String_To_Value (its compare order)  // NAME 1: what it does

// 0x83D7D0
bool String_To_Value(const char *buf, GuildRankType *value)
{
	FAIL_IF(!buf) return(false);
	int v = GUILD_RANK_NONE;
	if (sscanf(buf, "%d", &v) < 1) {
		return(false);
	}
	if (!Is_Valid_Guild_Rank(v)) {
		*value = (GuildRankType)GUILD_RANK_NONE;
	} else {
		*value = (GuildRankType)v;
	}
	return(true);
}







// 0x83D860
char *Value_To_String(char *buf, int size, GuildRankType value)
{
	FAIL_IF(!buf) return(NULL);
	_snprintf(buf, size, "%d", value);
	return(buf);
}

// 0x83D8B0
const char *Type_Name(GuildRankType value)
{
	return("GuildRankType");
}

/*
**	Reconstruction notes
**
**	The object runs from 0x83D7D0 (after unknown_83d740's 0x83D740) to its guarded <string> initialiser
**	0x83D8D0; its data starts with the __FILE__ string 0xBA9AB8 and 'GuildRankType' (0xBA9AEC), and
**	guild_strings.cpp's table (0xBA9B00) follows. The FAIL_IFs pin lines 23 and 45. String_To_Value keeps
**	a number in -1..9 and stores -1 for any other; the range test is an inline helper (written out, the
**	compiler places the in-range store first).
*/
