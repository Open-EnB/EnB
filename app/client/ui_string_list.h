// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: app\client\ui_string_list.h (PROVISIONAL, D137: the header of ui_string_list.cpp)
**
**	A scrolled list of strings shown in a mesh's 'DESC%02d' text styles (ui_string_list.cpp,
**	0x6483D0-0x649860). No constructor call to it survives in the client.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef UI_STRING_LIST_H  // NAME 1 [ui_string_list.h]: provisional header name for ui_string_list.cpp
#define UI_STRING_LIST_H

#include "intrface.h"
#include <vector>

class UIMeshClass;
class UIViewClass;
class LayerClass;
class FontSys;
class UISliderGadget;

// One entry of the list (8 bytes)
struct UIStringListItemStruct  // NAME 1: neutral (D10)
{
	UIStringListItemStruct(int id, char *string) : Id(id), String(string) {}
	int Id;												// +0x00 (what 0x649010 looks up)  // NAME 1: neutral
	char *String;										// +0x04 (copied when the list owns its strings)  // NAME 1: what Change_Text gets
};

// One shown line (4 bytes): its text style, referenced
struct UIStringListLineStruct  // NAME 1: neutral (D10)
{
	UIStringListLineStruct(FontSys *text) : Text(text) {}
	FontSys *Text;										// +0x00  // NAME 1: neutral
};

// The list (0xD0 bytes, tables 0xAF8770 / 0xAF8764 / 0xAF8758)
class UIStringListClass : public InterfaceClass	// NAME 3: class-from-file (D24) for app\Client\ui_string_list.cpp
{
public:
	UIStringListClass(UIMeshClass *mesh, UIViewClass *view, LayerClass *layer, const char *format, bool owns_strings, int unknown);	// 0x6483D0 (ret 0x18: the last argument unused)  // NAME 1 [mesh, view, layer, format, owns_strings, unknown]: what it does with them
	virtual ~UIStringListClass();						// 0x648770 (deleting 0x648740)
	virtual void Unknown_54();							// 0x648A60 (empty)
	virtual void Unknown_58();							// 0x648C50 (opens: the lines shown, the scroll bar set)
	virtual void Unknown_5C();							// 0x648D60 (closes when active)
	virtual void Unknown_60();							// 0x648D80 (the lines shown)
	virtual bool Unknown_68(int key);					// 0x648DA0 (1 the scroll bar, 2.. a line selected)
	virtual void Add(int id, const char *string);		// +0x88 0x648AA0  // NAME 1: what it does
	virtual bool Refresh();								// +0x8C 0x648E40 (the lines' texts from the items; false when unchanged)  // NAME 1: what it does
	void Clear();										// 0x648990  // NAME 1: what it does
	void Set_Dirty();									// 0x648A80  // NAME 1: what it does
	int Find(int id);									// 0x649010 (the item's index; G_ASSERT 'iterator != Items.end()')  // NAME 1: what it does
	void Update_Scroll_Bar();							// (inline: Add, Unknown_58)  // NAME 1: what it does
	void Unknown_649090();								// 0x649090: Update_Scroll_Bar out of line, uncalled

	std::vector<UIStringListItemStruct *> Items;		// +0x84  // NAME 3: the assert's
	std::vector<UIStringListItemStruct *> Shown;		// +0x94 (the items on the lines)  // NAME 1: neutral
	std::vector<UIStringListLineStruct *> Lines;		// +0xA4 (one per 'DESC%02d' bone)  // NAME 1: neutral
	int TopIndex;										// +0xB4 (the scroll bar's first row)  // NAME 1: neutral
	int Unknown0B8;										// +0xB8
	bool OwnsStrings;									// +0xBC  // NAME 1: neutral
	bool Dirty;											// +0xBD  // NAME 1: neutral
	UISliderGadget *ScrollBar;				// +0xC0 ('pull_cargo01', 'bar_cargo01')  // NAME 1: neutral
	int Unknown0C4;										// +0xC4
	int Unknown0C8;										// +0xC8
	UIStringListItemStruct *Selected;					// +0xCC  // NAME 1: neutral
};

#endif
