// Earth & Beyond client (reconstruction): app\client\ui_mfd_extended_context_strings.h (the client's
// ui_mfd_extended_context_strings.cpp per its asserts; this header's name is provisional, NAME 1)
#ifndef UI_MFD_EXTENDED_CONTEXT_STRINGS_H  // NAME 1 [ui_mfd_extended_context_strings.h]: provisional header name
#define UI_MFD_EXTENDED_CONTEXT_STRINGS_H

#include <string>
#include <vector>
#include "ui_mfd_extended_context.h"

class FontSys;

// A panel of text lines (vtable 0xAF39FC, slots +0x14..+0x1C pure): one FontSys per named string template
class UIMFDExtendedContextStringsClass : public UIMFDExtendedContextClass	// NAME 3: D24 class-from-file for ui_mfd_extended_context_strings.cpp
{
public:
	UIMFDExtendedContextStringsClass(ClientObjectClass *co, UIMFDClass *owner);	// 0x5C1090 (co unused)  // NAME 2 [co, owner]: what its subclass passes
	virtual ~UIMFDExtendedContextStringsClass();		// 0x5C1110 (deleting 0x5C10E0)
	virtual void Unknown_08();							// +0x08 0x5C17D0 (the fonts into the display's scene)
	virtual void Unknown_0C();							// +0x0C 0x5C18F0 (and out)
	virtual void Unknown_10();							// +0x10 0x5C1970 (the changed lines' text)
	virtual std::string Get_Font_Name(int index) = 0;	// +0x14 (empty: no more lines)  // NAME 2: what it returns
	virtual bool Line_Changed(unsigned int line_index) = 0;	// +0x18  // NAME 2: what it returns; NAME 5 [line_index]: the client's assert
	virtual std::string Get_Line_Text(unsigned int line_index) = 0;	// +0x1C  // NAME 2: what it returns; NAME 5 [line_index]: the client's assert

	void Create_Fonts();								// 0x5C1220  // NAME 2: what it does
	void Set_Texts();									// 0x5C1670  // NAME 2: what it does
	unsigned int Get_Line_Count();						// 0x5C1B30  // NAME 2: what it returns

	std::vector<FontSys *> Fonts;						// +0x0C  // NAME 5: the client's assert 'Fonts.size() == StringEmpty.size()'
	std::vector<bool> StringEmpty;						// +0x1C  // NAME 5: the client's assert
	int UnknownCount;									// +0x2C (the lines that aren't empty)  // NAME 1: neutral (D10)
};

#endif
