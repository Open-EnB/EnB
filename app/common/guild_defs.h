// Earth & Beyond client (reconstruction): app\common\guild_defs.h
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Common\guild_defs.h (name assumed from guild_defs.cpp,
**	whose __FILE__ is 0xBA9AB8; no assert names the header)
**
**	A guild member's rank and its text converters (guild_defs.cpp, 0x83D7D0-0x83D8B5). aux_roots.h
**	includes this header for the Player root's GuildRank field; it sees only what it declared before
**	(its declaration count moves the SYMBOL_PAD windows, workflow.md 3.4), the rest is behind
**	__GUILD_DEFS_ALL.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef GUILD_DEFS_H  // NAME 3 [guild_defs.h]: header name assumed from the client's guild_defs.cpp
#define GUILD_DEFS_H

// NAME 5: the client's Get_Type_Name string 'GuildRankType' (KVPair vtable 0xB01AF4, 0x708240, Type_Name 0x83D8B0), leaf vtable 0xB00A58
enum GuildRankType { UNKNOWN_B00A58_0 };	// vtable 0xB00A58, 4 bytes
const char *Type_Name(GuildRankType value);	// 0x83D8B0 ('GuildRankType')
bool String_To_Value(const char *text, GuildRankType *value);	// 0x83D7D0
char *Value_To_String(char *buf, int size, GuildRankType value);	// 0x83D860

#ifdef __GUILD_DEFS_ALL	// guild_defs.cpp only (aux_roots.cpp's declaration count, above)

// The ranks String_To_Value lets through: -1 (none, also what it stores for a number out of range) to 9.
enum { GUILD_RANK_NONE = -1, NUM_GUILD_RANKS = 10 };	// NAME 1: what they bound (no client string names them)

#endif

#endif
