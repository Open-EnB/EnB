// Earth & Beyond client (reconstruction): app\common\guild_strings.h
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Common\guild_strings.h (name assumed from guild_strings.cpp,
**	whose __FILE__ is 0xBAE080; no assert names the header)
**
**	A guild message's text (with $OtherName$ / $GuildName$ symbols) and its sound, by message type
**	(guild_strings.cpp, 0x83D900-0x83D9BA; the table 0xBA9B00). GuildMessageSectorPacket::Process_Packet
**	(cpacket.cpp 0x73E9E0) prints the text and plays the sound.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef GUILD_STRINGS_H  // NAME 3 [guild_strings.h]: header name assumed from the client's guild_strings.cpp
#define GUILD_STRINGS_H

// A guild message's type: the client's FAIL_IF texts 'message_type < GMSG_INVALID' and
// 'message_type >= GMSG_MAX' (0..131; entry 0 is the table's 'INVALID').
enum { GMSG_INVALID = 0, GMSG_MAX = 132 };

const char *Unknown_83D900(int message_type);	// 0x83D900: the type's text, NULL out of range  // NAME 1: what it returns (no client string names it)
const char *Unknown_83D970(int message_type);	// 0x83D970: the type's sound, NULL when it has none or out of range  // NAME 1: what it returns (no client string names it)

#endif
