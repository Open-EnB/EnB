// Earth & Beyond client (reconstruction): app\common\chat_defs.h
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Common\chat_defs.h (name assumed from chat_defs.cpp, whose
**	__FILE__ is 0xBA9030; no assert names the header)
**
**	The chat server's events (CHAT_EVENT_INVALID .. CHAT_EVENT_SET_TITLE_HIDDEN_OFF: the names of the table
**	0xBF3090) and errors ('ok' .. 'reached max limit', the table 0xBF3188), with their text converters
**	(chat_defs.cpp, 0x830460-0x830BC3). CLIENT_CHAT_ERROR's Process_Packet (cpacket.cpp 0x73D170) prints
**	an error's text.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef CHAT_DEFS_H  // NAME 3 [chat_defs.h]: header name assumed from the client's chat_defs.cpp
#define CHAT_DEFS_H

// The event names are the table's own strings (0xBA8FB8 ..), the enumerators' names stringised.
enum ChatEventType	// NAME 2: the client's FAIL_IF texts ('str.Event != event')
{
	CHAT_EVENT_INVALID = 0,
	CHAT_EVENT_LOGIN,
	CHAT_EVENT_LOGOUT,
	CHAT_EVENT_SAY_CHANNEL,
	CHAT_EVENT_SAY_PERSON,
	CHAT_EVENT_SAY_LOCAL,
	CHAT_EVENT_SAY_ALL,
	CHAT_EVENT_ENTER_CHANNEL,
	CHAT_EVENT_EXIT_CHANNEL,
	CHAT_EVENT_INSERT_CHANNEL,
	CHAT_EVENT_REMOVE_CHANNEL,
	CHAT_EVENT_REMOVE_CHANNEL_BY_PERSON,
	CHAT_EVENT_INVITE,
	CHAT_EVENT_UNINVITE,
	CHAT_EVENT_GAG,
	CHAT_EVENT_UNGAG,
	CHAT_EVENT_BAN,
	CHAT_EVENT_UNBAN,
	CHAT_EVENT_KICK,
	CHAT_EVENT_IGNORE,
	CHAT_EVENT_UNIGNORE,
	CHAT_EVENT_FRIEND,
	CHAT_EVENT_UNFRIEND,
	CHAT_EVENT_INSERT_OWNER,
	CHAT_EVENT_REMOVE_OWNER,
	CHAT_EVENT_SET_RESTRICTION_ON,
	CHAT_EVENT_SET_RESTRICTION_OFF,
	CHAT_EVENT_GM_GAG,
	CHAT_EVENT_GM_UNGAG,
	CHAT_EVENT_SET_TITLE_HIDDEN_ON,
	CHAT_EVENT_SET_TITLE_HIDDEN_OFF,
};

// A chat error code: 0 'ok' .. 18 'reached max limit' (the texts of the table 0xBF3188; no enumerator
// names in the client).
enum ChatErrorType { CHAT_ERROR_OK = 0 };	// NAME 2: the client's FAIL_IF texts ('str.Error != error'); CHAT_ERROR_OK: its text 'ok'

char *Value_To_String(char *buf, int size, ChatEventType event);	// 0x830770 (no caller in the client)
bool String_To_Value(const char *buf, ChatEventType *event);		// 0x830830 (no caller in the client)
char *Value_To_String(char *buf, int size, ChatErrorType error);	// 0x830AA0 (CLIENT_CHAT_ERROR 0x73D170)
bool String_To_Value(const char *buf, ChatErrorType *error);		// 0x830B60 (no caller in the client)

#endif
