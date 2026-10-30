// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: unknown (no __FILE__ or assert in its object; D10 neutral name).
**	PLACEMENT: provisional (D137): a string, assert or link map naming the object 0x4FA8F0-0x4FAC0F;
**	it links between split_string.obj and symbol_substitution.obj (app\shared, in name order), so its
**	real name sorts between those two.
**
**	A SplitStringClass measured with a font (0x2C bytes, vtable 0xAECBAC): the widths come from the
**	font's slots +0x24 / +0x28 at a 640-wide screen, from the base's without a font. Built on the stack
**	by ManagedFontClass 0x6CF930 and UILineDisplayClass::Add_Line 0x5A7690, among nine others.
*/


#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __UNKNOWN_FONT_SPLIT_STRING_H  // NAME 1: neutral (D10), file name unknown
#define __UNKNOWN_FONT_SPLIT_STRING_H

#include "split_string.h"
#include "..\..\wwlib\ref_ptr.h"
#include "..\client\fontdatabase.h"

class FontSplitStringClass : public SplitStringClass  // NAME 2: a SplitStringClass measured with a font
{
public:
	FontSplitStringClass(const RefCountPtr<Font32Class> &font, const char *text = NULL, float width = 0.0f, int cursor = 0);	// 0x4FA8F0
	virtual ~FontSplitStringClass();												// 0x4FA9F0 (deleting 0x4FA9C0)

	void Set_Font(const RefCountPtr<Font32Class> &font);							// 0x4FAAC0  // NAME 2: what it does

	virtual float Get_Text_Width(const char *buff);									// slot +0x04, 0x4FAB30
	virtual float Get_Char_Width(char c);											// slot +0x08, 0x4FAB70

protected:
	RefCountPtr<Font32Class> Font;													// +0x28  // NAME 2: what it holds
};

#endif // __UNKNOWN_FONT_SPLIT_STRING_H
