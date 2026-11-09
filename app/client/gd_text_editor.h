// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Client\gd_text_editor.h (D24: the header of gd_text_editor.cpp, whose __FILE__
**	0xB8D6FC the client holds; the client holds no string of this header)
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef GD_TEXT_EDITOR_H
#define GD_TEXT_EDITOR_H

#include <string>
#include <vector>
#include "gd_robj.h"
#include "..\..\wwlib\ref_ptr.h"

class FontSys;
class FontSplitStringClass;
class GadgetSlider;
class InterfaceClass;
class SClient;
class StrEditClass;
class TextCursorClass;

// The multi-line text editor (0xBC bytes, table 0xAFB3D4): ui_log's 'UI_LOG' and the petition window's 'UI_PETITION_ENTRY'
class TextEditorGadgetClass : public RObjGadgetClass	// NAME 3: class-from-file (D24) for app\Client\gd_text_editor.cpp
{
public:
	TextEditorGadgetClass(InterfaceClass *iface, const RefCountPtr<RenderObjClass> &model, const char *font, GadgetSlider *slider, int maxlen, const char *text);	// 0x671790  // NAME 1 [iface, model, font, maxlen, text]: what the windows pass; NAME 3 [slider]: the FAIL_IF's
	virtual ~TextEditorGadgetClass();					// +0x00 0x671B30 (deleting 0x671B00)
	virtual bool Unknown_2C(KernelEventClass *event, float x, float y, unsigned int mask);	// +0x2C 0x672120 (the clicks and keys)  // NAME 1 [event, x, y, mask]: the base's
	virtual bool Unknown_30(float x, float y);			// +0x30 0x6720A0  // NAME 1 [x, y]: the base's
	virtual bool Unknown_68();							// +0x68 0x6725B0 (held: the lines' 0x6CE910 on)
	virtual void Unknown_6C();							// +0x6C 0x672680 (released: off, the cursor hidden)
	virtual void Unknown_74();							// +0x74 0x672740 (the cursor into the layer's scene)
	virtual void Unknown_78();							// +0x78 0x6727E0 (the cursor out of it)
	virtual void Set_Render_Obj(RenderObjClass *robj);	// +0x94 0x671CB0 (the lines made to fill the model)  // NAME 2: the base's

	std::string Get_Text();								// 0x672870  // NAME 2: what it answers
	void Set_Text(const std::string &text);				// 0x6729B0 (the slider back to the top)  // NAME 2: what it does; NAME 1 [text]: neutral
	void Insert_Text(const std::string &text);			// 0x6729F0  // NAME 2: what it does; NAME 1 [text]: neutral
	void Unknown_672A60();								// 0x672A60 (FAIL_IF '!Slider': the top line from the slider)
	void Unknown_672AD0();								// 0x672AD0 (the slider's 0x66FD80)
	void Update_Lines(bool split);						// 0x672B00 (split: the text cut into lines again, the slider set; the lines and the cursor drawn)  // NAME 2: what it does; NAME 1 [split]: neutral
	bool Cursor_Up(bool update);						// 0x672DC0  // NAME 2: what it does; NAME 1 [update]: neutral
	bool Cursor_Down(bool update);						// 0x672E40  // NAME 2: what it does; NAME 1 [update]: neutral
	bool Page_Up();										// 0x672F00  // NAME 2: what it does
	void Page_Down();									// 0x672FF0  // NAME 2: what it does
	bool Cursor_Home();									// 0x673040  // NAME 2: what it does
	bool Cursor_End();									// 0x6730B0  // NAME 2: what it does
	void Take_Focus();									// 0x673150 (held, the key repeat on, a first text cleared)  // NAME 2: what it does
	void Drop_Focus();									// 0x673240  // NAME 2: what it does
	bool Has_Focus();									// 0x673280  // NAME 2: what it answers
	bool Unknown_6732A0();								// 0x6732A0 (the other held gadgets released, then held)
	void Unknown_6732F0(float x, float y);				// 0x6732F0 (the cursor to a point)  // NAME 1 [x, y]: the base's
	bool Unknown_6735A0(float x, float y);				// 0x6735A0 (the point on the current view's interface)  // NAME 1 [x, y]: the base's
	void Set_Unknown60(bool on) { Unknown60 = on; }		// (ui_log.cpp's 0x5ABCC0: the inline's cost keeps its first string's assign out of line)  // NAME 1: neutral (D10)

	bool Unknown60;										// +0x60 (true: a first text, cleared at the first focus)
	StrEditClass *Edit;									// +0x64  // NAME 2: what it holds
	std::string Font;									// +0x68 (the lines' string template)  // NAME 2: what it holds
	std::vector<RefCountPtr<FontSys> > Lines;			// +0x78  // NAME 2: what it holds
	FontSplitStringClass *Split;						// +0x88  // NAME 2: what it holds
	RefCountPtr<TextCursorClass> Cursor;				// +0x8C (fontsys_cursor.cpp's, 0x685680)  // NAME 2: what it is
	int TopLine;										// +0x90  // NAME 2: what it holds
	int LineCount;										// +0x94  // NAME 2: what it holds
	int CursorLine;										// +0x98  // NAME 2: what it holds
	int CursorColumn;									// +0x9C  // NAME 2: what it holds
	int Width;											// +0xA0 (the model's width in screen units)  // NAME 2: what it holds
	GadgetSlider *Slider;								// +0xA4  // NAME 3: the FAIL_IF's
	bool Unknown_A8;									// +0xA8 (the key repeat before the focus)
	bool Unknown_A9;									// +0xA9 (the slider is held: the next event only clears it)
	float LineHeight;									// +0xAC (in screen units)  // NAME 2: what it holds
	float Unknown_B0;									// +0xB0 (the first line's width)
	int Unknown_B4;										// +0xB4 (the constructor leaves it)
	SClient *Client;									// +0xB8 (0x6735A0: the current view; set by nothing in this file)  // NAME 2: its type
};

#endif
