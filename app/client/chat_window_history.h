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
#include "..\..\wwmath\vector3.h"
#include <vector>
#include <string>

class ChatLineClass : public RefCountClass  // NAME 3: the asserts' chat_line
{
public:
	ChatLineClass(int id, const std::string &text, const Vector3 &color, bool first, bool last) :  // NAME 2 [id, text, color, first, last]: what they fill
		Unknown08(id),
		Text(text),
		Color(color),
		Unknown29(false)
	{
		Dirty = true;
		First = first;
		Last = last;
		Bit3 = false;
		Null = false;
	}

	bool Is_Null() const { return (Flags & 0x10) != 0; }  // the asserts' Is_Null (bit 4)
	bool Is_Last() const { return (Flags & 4) != 0; }  // the assert's (bit 2)
	bool Is_First() const { return (Flags & 2) != 0; }  // NAME 2: 0x682770's walk back stops at it (bit 1)
	bool Is_Bit_3() const { return (Flags & 8) != 0; }  // NAME 1: neutral (D10)
	void Set_Flag_1() { Flags |= 1; }  // NAME 1: neutral (D10)
	void Set_Bit_3() { if ((Flags & 8) != 8) { Flags |= 8; Flags |= 1; } }  // NAME 1: neutral (D10)
	int Unknown08;  // +0x08  // NAME 1: neutral (D10)
	std::string Text;  // +0x0C  // NAME 2: a string, the line's own
	Vector3 Color;  // +0x1C: 0x680320's FontSys::Set_Color  // NAME 2: what it holds
	union {
		unsigned char Flags;  // +0x28: bit 2 the asserts' Is_Last  // NAME 2: what it holds
		struct {
			unsigned char Dirty : 1;  // NAME 2: 0x680320 clears it when it redraws
			unsigned char First : 1;  // NAME 2: the first of a split
			unsigned char Last : 1;  // NAME 3: the assert's Is_Last
			unsigned char Bit3 : 1;  // NAME 1: neutral (D10)
			unsigned char Null : 1;  // NAME 3: the assert's Is_Null
		};
	};
	bool Unknown29;  // +0x29  // NAME 1: neutral (D10)
};

class ChatWindowHistoryClass  // NAME 3: the file's name, its asserts' History
{
public:
	ChatWindowHistoryClass(int unknown);  // 0x6812C0  // NAME 1 [unknown]: neutral (D10)
	virtual ~ChatWindowHistoryClass();  // 0x681640 (deleting 0x681610)

	virtual bool Unknown_04(unsigned int max_lines, unsigned int width);  // +0x04 0x6817E0  // NAME 1 [Unknown_04, max_lines, width]: neutral (D10)
	virtual int Unknown_08(const Vector3 &color, const std::string &text);  // +0x08 0x6821F0  // NAME 2 [color, text]: what the lines take
	virtual void Unknown_0C(const Vector3 &color, const std::string &text);  // +0x0C 0x6821C0  // NAME 2 [color, text]: Unknown_08's
	virtual int Unknown_10(int unknown);  // +0x10 0x682080  // NAME 1: neutral (D10)
	virtual bool Unknown_14(const char *command);  // +0x14 0x6828D0  // NAME 1: neutral (D10)

	void Set_Font(const RefCountPtr<Font32Class> &font);  // 0x6817A0  // NAME 2: calls the splitter's Set_Font

	const RefCountPtr<ChatLineClass> &Unknown_682730(unsigned int index);  // 0x682730  // NAME 1 [index]: neutral (D10)
	const RefCountPtr<ChatLineClass> &Unknown_682770(unsigned int index);  // 0x682770 (lines 412, 418)  // NAME 1 [index]: neutral (D10)
	int Unknown_682840();  // 0x682840  // NAME 1: neutral (D10)
	int Unknown_682880();  // 0x682880  // NAME 1: neutral (D10)
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
