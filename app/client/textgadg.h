// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Client\textgadg.h (name assumed from textgadg.cpp, whose
**	__FILE__ is 'e:\build\beta\G\app\Client\textgadg.cpp'; the client holds no string of this header)
**
**	The UI kit's text gadgets (the edit boxes): TextBaseGadgetClass (table 0xAFD980, constructor 0x6AA000,
**	0x44 bytes: the text in a StrEditClass, the active box '_ActiveUI' 0xBE66C0) and TextGadgetClass over
**	it (table 0xAFDA10, constructor 0x6AA810, 0x80 bytes: the text drawn by a FontStyleClass on a model,
**	with a cursor). The login screen's name and password boxes are TextGadgetClass. Declared as far as
**	textgadg.cpp and its callers use them (D10).
*/



#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __TEXTGADG_H  // NAME 3 [textgadg.h]: header name assumed from the client's textgadg.cpp
#define __TEXTGADG_H

#include "gadget.h"
#include "..\..\wwlib\ref_ptr.h"
#include "..\..\wwmath\aabox.h"

class StrEditClass;
class FontStyleClass;
class TextCursorClass;
class Font32Class;

// The text gadgets' flags (the constructors' flags argument, TextGadgetClass +0x7C): constants in .rdata
// 0xB08C90-0xB08CC4 (1, 2, 4 ... 0x1000, then 0x2C0), defined in a file not rebuilt yet (textgadg.cpp loads them)
extern const int Unknown_B08CAC;			// 0xB08CAC (0x80: the cursor's last argument)  // NAME 1: neutral (D10)
extern const int Unknown_B08CB0;			// 0xB08CB0 (0x100: the text's x offset from the box)  // NAME 1: neutral (D10)
extern const int Unknown_B08CB8;			// 0xB08CB8 (0x400: the text scrolled to fit the box)  // NAME 1: neutral (D10)
extern const int Unknown_B08CBC;			// 0xB08CBC (0x800: the model as given, not its top container)  // NAME 1: neutral (D10)
extern const int Unknown_B08CC0;			// 0xB08CC0 (0x1000: the cursor blinks)  // NAME 1: neutral (D10)
extern const int Unknown_B08CC4;			// 0xB08CC4 (0x2C0: the login boxes' flags)  // NAME 1: neutral (D10)

class TextBaseGadgetClass : public GadgetClass  // NAME 2: the text gadgets' base (no RTTI; the asserts are textgadg.cpp's)
{
public:
	TextBaseGadgetClass(int length, unsigned int flags, const char *chars);	// 0x6AA000 (two StrEditClass of the length)  // NAME 1 [length, flags, chars]: what TextGadgetClass passes (StrEditClass's)
	virtual ~TextBaseGadgetClass();										// +0x00 0x6AA140 (deleting 0x6AA110)
	virtual bool Unknown_2C(KernelEventClass *event, float x, float y, unsigned int mask);	// +0x2C 0x6AA200 (the keys: tab, enter, escape, the rest to the edit buffer)
	virtual bool Unknown_30(float x, float y) { return(false); }		// +0x30 0x65C920 (folded)
	virtual void Unknown_74(bool on);									// +0x74 0x6AA720 (G_ASSERT '_ActiveUI == this': the input focus)  // NAME 1 [on]: neutral

	void Unknown_6AA340(char *print, int s_size, int *cursorpos, bool *insert, const char *prompt, bool password);	// 0x6AA340 (the text to draw: the prompt, then the text or its stars)  // NAME 3 [print, s_size, cursorpos, prompt]: the asserts'; NAME 1 [insert, password]: what 0x6AAD10 passes
	void Set_Text(const char *text, bool append);						// 0x6AA570  // NAME 2: what it does; NAME 1 [text, append]: neutral
	void Unknown_6AA5F0();												// 0x6AA5F0 (Hidden gets a star per character; no caller: inlined in 0x6AA340)
	int Get_Text_Length();												// 0x6AA690  // NAME 2: what it does
	char *Get_Text();													// 0x6AA6C0 (a copy, deleted by the caller)  // NAME 2: what it does
	char *Get_Hidden_Text();											// 0x6AA6F0 (a copy of the stars)  // NAME 2: what it does

	StrEditClass *Edit;													// +0x30 (the text)  // NAME 2: what it holds
	StrEditClass *Hidden;												// +0x34 (the password's stars, 0x6AA5F0)  // NAME 2: what it holds
	bool Changed;														// +0x38 (0x6AAD10 redraws and clears it)  // NAME 2: what 0x6AAD10 does with it
	bool Unknown39;														// +0x39 (tab with no next box: ui_login.cpp clears it)
	bool Unknown3A;														// +0x3A (escape)
	bool Unknown3B;														// +0x3B (a key changed the text: the focus keeps it)
	bool Active;														// +0x3C (slot +0x74's)  // NAME 2: what slot +0x74 sets
	TextBaseGadgetClass *Next;											// +0x40 (tab moves the focus to it)  // NAME 2: what slot +0x2C does with it

	static TextBaseGadgetClass *_ActiveUI;								// 0xBE66C0  // NAME 5: the assert's
};

class TextGadgetClass : public TextBaseGadgetClass  // NAME 3: class-from-file (D24) for app\client\textgadg.cpp
{
public:
	TextGadgetClass(InterfaceClass *iface, int length, const char *string_name, RenderObjClass *model, unsigned int flags, const char *chars);	// 0x6AA810  // NAME 5 [string_name]: the assert's; NAME 1 [iface, length, model, flags, chars]: what ui_login.cpp passes (chars: StrEditClass's)
	virtual ~TextGadgetClass();											// +0x00 0x6AAB60 (deleting 0x6AAB30)
	virtual void Unknown_28(LayerClass *layer);							// +0x28 0x6AB0F0 (the cursor into the layer's scene)  // NAME 1 [layer]: what the windows pass
	virtual void Unknown_64(AABoxClass &box);							// +0x64 0x6AB080 (the text's bounding box)
	virtual void Unknown_74(bool on);									// +0x74 0x6AACD0 (off: the cursor hidden)  // NAME 1 [on]: neutral

	void Unknown_6AAD10();												// 0x6AAD10 (every frame: G_ASSERT 'font', FAIL_IF 'max >= 255': the text redrawn, the cursor blinked)
	static char *Unknown_6AB190(char *string, float pwidth, Font32Class *&font, int &cp);	// 0x6AB190 (FAIL_IF 'pwidth <= 0.0f', G_ASSERT 'cp >= 0': the string clipped to the width around the cursor)  // NAME 3 [pwidth, cp]: the asserts'; NAME 1 [string, font]: neutral

	RefCountPtr<FontStyleClass> Text;									// +0x44 (0x4E8 bytes, 0x6D47C0: string_name's style on the model)  // NAME 2: what it draws
	RefCountPtr<RenderObjClass> Model;									// +0x48  // NAME 2: the constructor's model
	RefCountPtr<TextCursorClass> Cursor;								// +0x4C (fontsys_cursor.cpp's, 0x685680)  // NAME 2: what it is
	char Unknown50;														// +0x50 (' ')
	char Unknown51;														// +0x51 (' ')
	unsigned int Unknown54;												// +0x54 (GetTickCount at the cursor's last blink)
	float Unknown58;													// +0x58 (1.0: the clip width's scale)
	bool Unknown5C;														// +0x5C (the password's: stars)
	AABoxClass Box;														// +0x60 (the model's named sub-object's bounding box)  // NAME 2: what it holds
	float Unknown78;													// +0x78 (the text's x inside the box)
	unsigned int Flags;													// +0x7C (the constructor's)  // NAME 1: neutral (D10)
};

#endif // __TEXTGADG_H
