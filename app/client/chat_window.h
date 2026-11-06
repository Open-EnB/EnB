// Earth & Beyond client (reconstruction)
/*
**	The client's file (__FILE__ 0xB8DAC8); the header's name is assumed (D24). ChatWindowClass: vtable 0xAFBEF0.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __CHAT_WINDOW_H
#define __CHAT_WINDOW_H

#include "chat_window_history.h"
#include "fontmanager.h"
#include "font32bit.h"
#include <vector>
#include <string>

class ChatFontSys : public FontSys  // NAME 2: a FontSys (0x4EC bytes, vtable 0xAFBF28) holding a chat line
{
public:
	ChatFontSys(const char *name) : FontSys(name, 0, "") {}

	void Set_Line(const RefCountPtr<ChatLineClass> &line)  // NAME 2: what it does
	{
		if (Line.Peek()) {
			Line = line;
			if (Line.Peek()) {
				Line->Set_Flag_1();
			}
			Remove();
		}
	}

	RefCountPtr<ChatLineClass> Line;  // +0x4E8  // NAME 2: what it holds
};

class ChatWindowClass : public ChatWindowHistoryClass  // NAME 3: the file's name, the assert 'window->ChatWindow'
{
public:
	ChatWindowClass(void *owner, int unknown, int index);  // 0x67DC80 (ui_chat_window.cpp's 0x56FD30)  // NAME 1 [owner, unknown, index]: neutral (D10)
	virtual ~ChatWindowClass();  // 0x67DF50 (deleting 0x67DF20)

	virtual bool Unknown_04(unsigned int max_lines, unsigned int width);  // +0x04 0x67E620
	virtual void Unknown_08();  // +0x08 0x67E250
	virtual void Unknown_0C();  // +0x0C 0x67E220
	virtual void Unknown_10();  // +0x10 0x67E480
	virtual void Unknown_14();  // +0x14 0x67E4C0

	void Unknown_67E670();  // 0x67E670  // NAME 1: neutral (D10)
	void Clear_Fonts();  // 0x67F680  // NAME 2: what it does (FAIL_IF '!font_ptr' l.293)

	RefCountPtr<ChatLineClass> Unknown60;  // +0x60  // NAME 1: neutral (D10)
	int Unknown64;  // +0x64  // NAME 1: neutral (D10)
	int Unknown68;  // +0x68  // NAME 1: neutral (D10)
	int Unknown6C;  // +0x6C  // NAME 1: neutral (D10)
	int Unknown70;  // +0x70  // NAME 1: neutral (D10)
	void *Unknown74;  // +0x74: the constructor's owner  // NAME 1: neutral (D10)
	int Unknown78;  // +0x78  // NAME 1: neutral (D10)
	std::vector< RefCountPtr<ChatFontSys> > Fonts;  // +0x7C  // NAME 3: the asserts' font_ptr
	RefCountPtr<Font32Class> Font;  // +0x8C  // NAME 2: its class
	int Unknown90;  // +0x90  // NAME 1: neutral (D10)
	int Unknown94;  // +0x94  // NAME 1: neutral (D10)
	int Unknown98;  // +0x98  // NAME 1: neutral (D10)
	bool Unknown9C;  // +0x9C  // NAME 1: neutral (D10)
	bool Unknown9D;  // +0x9D  // NAME 1: neutral (D10)
	bool Unknown9E;  // +0x9E  // NAME 1: neutral (D10)
	int UnknownA0;  // +0xA0  // NAME 1: neutral (D10)
	int UnknownA4;  // +0xA4  // NAME 1: neutral (D10)
	int UnknownA8;  // +0xA8  // NAME 1: neutral (D10)
	std::string Name;  // +0xAC: 'Chat_Line_NN'  // NAME 2: what it holds
	char UnknownBC[4];  // +0xBC: 0x67E670 fills it with spaces (size unknown)  // NAME 1: neutral (D10)
};

#endif
