// Earth & Beyond client (reconstruction)
/*
**	The client's file (__FILE__ 0xB8DBA8); the header's name is assumed (D24).
**	ChatLineClass: 0x2C bytes, vtable 0xAFC174; ChatWindowHistoryClass: vtable 0xAFC180.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __CHAT_WINDOW_HISTORY_H
#define __CHAT_WINDOW_HISTORY_H

#include "..\shared\unknown_font_split_string.h"
#include "..\..\wwlib\ref_ptr.h"
#include "..\..\wwlib\refcount.h"
#include <vector>
#include <string>

class ChatLineClass : public RefCountClass  // NAME 3: the asserts' chat_line
{
public:
	virtual ~ChatLineClass();
public:
	void Set_Flag_1() { Flags |= 1; }  // NAME 1: neutral (D10)
	int Unknown08;  // +0x08  // NAME 1: neutral (D10)
	std::string Text;  // +0x0C  // NAME 2: a string, the line's own
	int Unknown1C;  // +0x1C  // NAME 1: neutral (D10)
	int Unknown20;  // +0x20  // NAME 1: neutral (D10)
	int Unknown24;  // +0x24  // NAME 1: neutral (D10)
	unsigned char Flags;  // +0x28: bit 2 the asserts' Is_Last  // NAME 2: what it holds
};

class ChatWindowHistoryClass  // NAME 3: the file's name, its asserts' History
{
public:
	ChatWindowHistoryClass(int unknown);  // 0x6812C0  // NAME 1 [unknown]: neutral (D10)
	virtual ~ChatWindowHistoryClass();  // 0x681640 (deleting 0x681610)

	virtual bool Unknown_04(unsigned int max_lines, unsigned int width);  // +0x04 0x6817E0  // NAME 1 [Unknown_04, max_lines, width]: neutral (D10)
	virtual void Unknown_08();  // +0x08 0x6821F0  // NAME 1: neutral (D10)
	virtual void Unknown_0C();  // +0x0C 0x6821C0  // NAME 1: neutral (D10)
	virtual void Unknown_10();  // +0x10 0x682080  // NAME 1: neutral (D10)
	virtual void Unknown_14();  // +0x14 0x6828D0  // NAME 1: neutral (D10)

	void Set_Font(const RefCountPtr<Font32Class> &font);  // 0x6817A0  // NAME 2: calls the splitter's Set_Font

	void Unknown_681DB0();  // 0x681DB0  // NAME 1: neutral (D10)
	void Unknown_6818C0();  // 0x6818C0  // NAME 1: neutral (D10)

	std::vector< RefCountPtr<ChatLineClass> > History;  // +0x04 (the asserts' History)
	FontSplitStringClass Splitter;  // +0x14  // NAME 2: its class
	RefCountPtr<ChatLineClass> NullLine;  // +0x40  // NAME 2: 0x682770's not-found return
	int Unknown44;  // +0x44  // NAME 1: neutral (D10)
	unsigned int Unknown48;  // +0x48  // NAME 1: neutral (D10)
	int Unknown4C;  // +0x4C  // NAME 1: neutral (D10)
	unsigned int Unknown50;  // +0x50  // NAME 1: neutral (D10)
	unsigned int Unknown54;  // +0x54  // NAME 1: neutral (D10)
	int Unknown58;  // +0x58  // NAME 1: neutral (D10)
	bool Unknown5C;  // +0x5C  // NAME 1: neutral (D10)
};

#endif
