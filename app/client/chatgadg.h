// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Client\ChatGadg.h (name assumed from ChatGadg.cpp, whose
**	__FILE__ is 'e:\build\beta\G\app\Client\ChatGadg.cpp'; the client holds no string of this header)
**
**	The chat panel's text entry (0x80 bytes, constructor 0x65BFD0, table 0xAF99D8): a text gadget whose
**	enter sends the line on the chat panel's channel (ClientChatPacketClass, AvatarEmotePacketClass,
**	ClientChatRequestPacketClass), with a history of the last ten lines (up / down), the 'chatgadg' module's
**	control keys while it is idle (the channels, reply, positions, quick messages kept in chat.ini).
**	Declared as far as chatgadg.cpp and its callers use it (D10).
*/



#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __CHATGADG_H  // NAME 3 [chatgadg.h]: header name assumed from the client's ChatGadg.cpp
#define __CHATGADG_H

#include "textgadg.h"

class LineDisplayClientClass;
class ClientClass;
class INIClass;

class ChatGadgetClass : public TextBaseGadgetClass  // NAME 3: class-from-file (D24) for app\Client\ChatGadg.cpp
{
public:
	ChatGadgetClass(LineDisplayClientClass *client);					// 0x65BFD0 (the 'chatgadg' keys registered on the first one; chat.ini loaded)  // NAME 1 [client]: what ui_chat_panel.cpp passes
	virtual ~ChatGadgetClass();											// +0x00 0x65C970 (deleting 0x65C940)
	virtual bool Unknown_2C(KernelEventClass *event, float x, float y, unsigned int mask);	// +0x2C 0x65CA10 (active: tab the channel, enter 0x65CCD0, escape, up / down the history, the rest to the text; idle: 0x65DE90)

	void Unknown_65CCD0();												// 0x65CCD0 (FAIL_IF '!sclient': the text sent on the chat panel's channel, kept in the history)
	bool Unknown_65DE90(KernelEventClass *event);						// 0x65DE90 (FAIL_IF '!sclient': the module's control keys)  // NAME 1 [event]: what slot +0x2C passes
	bool Unknown_65E250(int index);										// 0x65E250 (FAIL_IF 'index < 1', 'index > NUM_QUICK_MESSAGES': the text saved as quick message index)  // NAME 5 [index]: the FAIL_IFs'
	bool Unknown_65E3E0(int index);										// 0x65E3E0 (the same FAIL_IFs: quick message index inserted)  // NAME 5 [index]: the FAIL_IFs'
	bool Unknown_65E520();												// 0x65E520 (the avatar's position inserted; 0x65DE90 inlines it)
	bool Unknown_65E5E0();												// 0x65E5E0 (the target's name and position inserted, or 'No target.')
	void Unknown_65E7C0(bool reply);									// 0x65E7C0 (the chat window's slot +0xAC: activated, emptied; with reply only when 0x65E840 finds the target's owner)  // NAME 1 [reply]: what 0x744E70's argument means
	bool Unknown_65E840();												// 0x65E840 (FAIL_IF '!sclient', '!chatpanel': a reply to the target's owner, its '[...]' cut)
	void Unknown_65EBF0(char *print, int s_size, int &cursorpos, bool &insert);	// 0x65EBF0 (G_ASSERT 's_size > 10', 'print': the text to draw, no prompt)  // NAME 5 [print, s_size]: the asserts'; NAME 1 [cursorpos, insert]: what 0x571470 does with them

	ClientClass *Client;												// +0x44 (the constructor's: slot +0x18 gives the sclient)  // NAME 1: neutral (D10), as InterfaceClass's +0x40
	char *History[10];													// +0x48 (the lines sent, Get_String's copies; the destructor deletes them)  // NAME 2: what it holds
	int HistoryNext;													// +0x70 (where the next line goes)  // NAME 2: what it holds
	int HistoryIndex;													// +0x74 (the line up / down shows)  // NAME 2: what it holds
	INIClass *QuickMessages;											// +0x78 (chat.ini: MSG_1..MSG_9 under the character's id)  // NAME 2: what it holds
	bool Unknown7C;														// +0x7C (up / down showed a line: up moves on from it)
};

#endif // __CHATGADG_H
