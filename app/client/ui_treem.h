// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: app\client\ui_treem.h (PROVISIONAL, D137: the header of ui_treem.cpp)
**
**	A tree of strings with plus / minus buttons shown in a mesh's 'DESC%02d' / 'PM%02d' bones
**	(ui_treem.cpp, 0x654860-0x656FF0). No constructor call to it survives in the client.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef UI_TREEM_H  // NAME 1 [ui_treem.h]: provisional header name for ui_treem.cpp
#define UI_TREEM_H

#include "intrface.h"
#include "vector.h"

class UIMeshClass;
class UIViewClass;
class LayerClass;
class FontSys;
class TextureClass;
class ButtonGadgetClass;
class UISliderGadget;

// One item of the tree (16 bytes; the vector instances compare items by address)
struct UITreeMItemStruct  // NAME 1: neutral (D10)
{
	bool operator == (const UITreeMItemStruct &that) const { return(&that == this); }
	bool operator != (const UITreeMItemStruct &that) const { return(&that != this); }

	int Id;												// +0x00 (what Find looks up)  // NAME 1: neutral
	char *String;										// +0x04 (copied when the tree owns its strings)  // NAME 1: neutral
	int Level;											// +0x08  // NAME 5: the asserts' 'item->Level'
	unsigned int Flags;									// +0x0C (1 has children, 2 open, 0x10 shown)  // NAME 1: neutral
};

// One shown line (12 bytes)
struct UITreeMLineStruct  // NAME 1: neutral (D10)
{
	bool operator == (const UITreeMLineStruct &that) const { return(&that == this); }
	bool operator != (const UITreeMLineStruct &that) const { return(&that != this); }

	ButtonGadgetClass *Desc;							// +0x00 ('DESC%02d')  // NAME 1: the bone's
	ButtonGadgetClass *PlusMinus;						// +0x04 ('PM%02d')  // NAME 1: the bone's
	FontSys *String;									// +0x08  // NAME 5: the assert's 'line->String'
};

// The tree (0xF8 bytes, tables 0xAF9110 / 0xAF9104 / 0xAF90F8)
class UITreeMClass : public InterfaceClass	// NAME 3: class-from-file (D24) for app\Client\ui_treem.cpp
{
public:
	UITreeMClass(UIMeshClass &mesh, UIViewClass *view, LayerClass *layer, const char *format, bool owns_strings);	// 0x654860 (ret 0x14)  // NAME 1 [view, layer, format, owns_strings]: what it does with them; [mesh] the assert's
	virtual ~UITreeMClass();							// 0x654E50 (deleting 0x654D80)
	virtual void Unknown_54();							// 0x654E30 (empty)
	virtual void Unknown_58();							// 0x655330 (opens: the lines shown, the scroll bar set)
	virtual void Unknown_5C();							// 0x655420 (closes when active)
	virtual void Unknown_60();							// 0x655440 (the lines shown)
	virtual bool Unknown_68(int key);					// 0x655460 (1 the scroll bar, 2.. a line's text or plus / minus button)
	virtual void Add(int id, const char *string);		// +0x88 0x655070 (a top-level item)  // NAME 1: what it does
	virtual void Add_Child(int id, const char *string, int parent);	// +0x8C 0x6551B0 (an item under the item 'parent')  // NAME 1: what it does
	virtual bool Refresh();								// +0x90 0x655700 (the lines' texts and buttons from the items; false when unchanged)  // NAME 1: what it does
	UITreeMItemStruct *Prev(UITreeMItemStruct *item);			// 0x655AA0  // NAME 1: what it does
	UITreeMItemStruct *Next(UITreeMItemStruct *item);			// 0x655AD0  // NAME 1: what it does
	UITreeMItemStruct *Next_Shown(UITreeMItemStruct *item);		// 0x655B10  // NAME 1: what it does
	UITreeMItemStruct *Parent(UITreeMItemStruct *item);			// 0x655D40  // NAME 1: what it does
	UITreeMItemStruct *Next_Sibling(UITreeMItemStruct *item);	// 0x655DE0  // NAME 1: what it does
	void Open(UITreeMItemStruct *item);					// 0x655E50  // NAME 1: what it does
	void Close(UITreeMItemStruct *item);				// 0x655F70  // NAME 1: what it does
	void Toggle(UITreeMItemStruct *item);				// 0x6560A0  // NAME 1: what it does
	int Find(int id);									// 0x656300 (the item's index; G_ASSERT 'false')  // NAME 1: what it does
	void Update_Scroll_Bar();							// 0x656350  // NAME 1: what it does

	DynamicVectorClass<UITreeMItemStruct> Items;		// +0x84  // NAME 1: neutral
	DynamicVectorClass<UITreeMLineStruct> Lines;		// +0x9C (one per 'DESC%02d' bone)  // NAME 1: neutral
	VectorClass<UITreeMItemStruct *> Shown;				// +0xB4 (the items on the lines)  // NAME 1: neutral
	UITreeMItemStruct *Opened;							// +0xC4 (the item last opened: its children drawn in style 2)  // NAME 1: neutral
	int TopIndex;										// +0xC8 (the scroll bar's first top-level item)  // NAME 1: neutral
	int ShownCount;										// +0xCC (items shown)  // NAME 1: neutral
	int TopCount;										// +0xD0 (top-level items)  // NAME 1: neutral
	bool OwnsStrings;									// +0xD4  // NAME 1: neutral
	bool Unknown0D5;									// +0xD5 (set by the constructor, cleared by Refresh)
	bool Dirty;											// +0xD6  // NAME 1: neutral
	int Unknown0D8;										// +0xD8
	UISliderGadget *ScrollBar;				// +0xDC ('pull_cargo01', 'bar_cargo01')  // NAME 1: neutral
	unsigned int ClickTime;								// +0xE0 (a text's first click, for the double click)  // NAME 1: neutral
	int ClickIndex;										// +0xE4  // NAME 1: neutral
	UITreeMItemStruct *Selected;						// +0xE8  // NAME 1: neutral
	TextureClass *MinusTexture;							// +0xEC ('IF_MINUS.TGA')  // NAME 1: neutral
	TextureClass *PlusTexture;							// +0xF0 ('IF_PLUS.TGA')  // NAME 1: neutral
	TextureClass *NoneTexture;							// +0xF4 ('IF_NONE.TGA')  // NAME 1: neutral
};

#endif
