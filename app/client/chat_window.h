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
#include "gd_slider.h"
#include "..\..\ww3d2\scene.h"
#include "..\..\wwmath\rect.h"
#include <vector>
#include <string>

class ChatFontSys : public FontSys  // NAME 2: a FontSys (0x4EC bytes, vtable 0xAFBF28) holding a chat line
{
public:
	ChatFontSys(const char *name) : FontSys(name, 0, "") { Set_Sort_Level(10); }

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

	void Set_Line_Always(const RefCountPtr<ChatLineClass> &line)  // NAME 2: Set_Line without its test: 0x67E250 compares first
	{
		if (line.Peek() != Line.Peek()) {
			Line = line;
			if (Line.Peek()) {
				Line->Set_Flag_1();
			}
			Remove();
		}
	}

	bool Unknown_680320(const char *indent, RefCountPtr<SceneClass> scene);  // 0x680320  // NAME 2 [indent, scene]: the text after it; +0x34 Add, +0x40 Peek_Scene

	RefCountPtr<ChatLineClass> Line;  // +0x4E8  // NAME 2: what it holds
};

class ChatWindowClass : public ChatWindowHistoryClass  // NAME 3: the file's name, the assert 'window->ChatWindow'
{
public:
	ChatWindowClass(GadgetSlider *owner, int unknown, int index);  // 0x67DC80 (ui_chat_window.cpp's 0x56FD30)  // NAME 1 [owner, unknown, index]: neutral (D10)
	virtual ~ChatWindowClass();  // 0x67DF50 (deleting 0x67DF20)

	virtual bool Unknown_04(unsigned int max_lines, unsigned int width);  // +0x04 0x67E620
	virtual int Unknown_08(const Vector3 &color, const std::string &text);  // +0x08 0x67E250
	virtual void Unknown_0C(const Vector3 &color, const std::string &text);  // +0x0C 0x67E220
	virtual int Unknown_10(int unknown);  // +0x10 0x67E480  // NAME 1 [unknown]: neutral (D10)
	virtual bool Unknown_14(const char *command);  // +0x14 0x67E4C0: '/chat render', '/chat extra', '/chat build'  // NAME 2 [command]: its strings

	void Unknown_67E670();  // 0x67E670  // NAME 1: neutral (D10)
	void Unknown_67E180(const RefCountPtr<SceneClass> &scene);  // 0x67E180 (FAIL_IF l.147)  // NAME 1 [Unknown_67E180]: neutral (D10)
	bool Unknown_67F7F0(unsigned int max_lines);  // 0x67F7F0  // NAME 1 [max_lines]: neutral (D10)
	void Unknown_67FC60();  // 0x67FC60 (FAIL_IF l.413, l.421)  // NAME 1: neutral (D10)
	void Unknown_67FFD0();  // 0x67FFD0 (FAIL_IF l.457, l.473)  // NAME 1: neutral (D10)
	void Unknown_6804E0(const RectClass &rect);  // 0x6804E0: called with Rect  // NAME 2 [rect]: copied to Rect
	int Get_Slider_Value() const { return Unknown74->UnknownD8; }  // NAME 2: reads the slider's floor of Value
	void Unknown_680670();  // 0x680670  // NAME 1: neutral (D10)
	void Unknown_6806C0();  // 0x6806C0  // NAME 1: neutral (D10)
	void Unknown_680700(bool unknown);  // 0x680700 (FAIL_IF l.547)  // NAME 1 [unknown]: neutral (D10)
	void Unknown_680790();  // 0x680790  // NAME 1: neutral (D10)
	void Unknown_6809B0();  // 0x6809B0  // NAME 1: neutral (D10)
	const RefCountPtr<ChatFontSys> &Get_Font(unsigned int index);  // 0x6808D0 (FAIL_IF '!size' l.598)  // NAME 2 [Get_Font, index]: what it does
	void Clear_Fonts();  // 0x67F680  // NAME 2: what it does (FAIL_IF '!font_ptr' l.293)

	RefCountPtr<SceneClass> Scene;  // +0x60: passed to the fonts' Add  // NAME 2: what it holds
	RectClass Rect;  // +0x64: 0x6804E0 copies it; left, top, right, bottom  // NAME 2: what it holds
	GadgetSlider *Unknown74;  // +0x74: the constructor's owner  // NAME 1: neutral (D10)
	float LineHeight;  // +0x78: Font->Get_Char_Height(480)  // NAME 2: what it holds
	std::vector< RefCountPtr<ChatFontSys> > Fonts;  // +0x7C  // NAME 3: the asserts' font_ptr
	RefCountPtr<Font32Class> Font;  // +0x8C  // NAME 2: its class
	unsigned int Unknown90;  // +0x90  // NAME 1: neutral (D10)
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
