// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: app\client\ui_ptnew.h (PROVISIONAL, D137: the header of ui_ptnew.cpp)
**
**	The petition terminal's news page ui_ptnew.cpp rebuilds (0x5DF970-0x5E09C0); ui_pterm.cpp's 0x5DE030
**	makes it.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef UI_PTNEW_H  // NAME 1 [ui_ptnew.h]: provisional header name for ui_ptnew.cpp
#define UI_PTNEW_H

#include "intrface.h"
#include "layer.h"
#include <vector>

class UIViewClass;
class RenderObjClass;
class FontSys;
class UISliderGadget;
class Unknown696810Class;

// One row of the news list (0x0C bytes): its three text styles
struct UIPTNewRowStruct  // NAME 1: neutral (D10)
{
	FontSys *Date;										// +0x00 ('Date_%02d_PT_NEWS')  // NAME 1: the style's name
	FontSys *Location;									// +0x04 ('Location_%02d_PT_NEWS')  // NAME 1: the style's name
	FontSys *Title;										// +0x08 ('Title_%02d_PT_NEWS')  // NAME 1: the style's name
};

// The news page (0xD4 bytes, tables 0xAF6244 / 0xAF6238 / 0xAF622C)
class UIPTNewClass : public InterfaceClass	// NAME 3: class-from-file (D24) for app\Client\ui_ptnew.cpp
{
public:
	UIPTNewClass(UIViewClass *view, LayerClass *layer);	// 0x5DF970  // NAME 1 [view, layer]: as LoginInterface's
	virtual ~UIPTNewClass();							// 0x5DFBB0 (deleting 0x5DFB80)
	virtual void Unknown_54();							// 0x5DFDC0 (the slot +0x10, as the base's)
	virtual void Unknown_58();							// 0x5DFDE0 (opens: the rows' styles and the buttons)
	virtual void Unknown_5C();							// 0x5E0180 (closes)
	virtual void Unknown_60();							// 0x5E0260 (empty)
	virtual bool Unknown_68(int key);					// 0x5E0280 (the buttons: 1 the list, 5.. a row)
	void Unknown_5E03A0(int index);						// 0x5E03A0: an item selected  // NAME 1 [index]: neutral
	void Unknown_5E0470();								// 0x5E0470: the rows filled from the list
	void Unknown_Make_Rows();							// (inline in 0x5DFDE0: a row per 'Date_%02d_PT_NEWS' style)  // NAME 1: neutral (D10)
	void Unknown_Make_Buttons();						// (inline in 0x5DFDE0: the rows' and the page's buttons, once)  // NAME 1: neutral (D10)

	LayerClass Layer;									// +0x84 (a copy of the constructor's layer)  // NAME 1: what it holds
	RenderObjClass *ListScreen;							// +0xA8 ('TLI11A')  // NAME 3: the G_ASSERT's
	RenderObjClass *DescScreen;							// +0xAC ('TLI07A')  // NAME 3: the G_ASSERT's
	UISliderGadget *Unknown0B0;				// +0xB0 (the list's scroll bar)
	Unknown696810Class *Unknown0B4;						// +0xB4 (pt_data.cpp's news list)
	std::vector<UIPTNewRowStruct *> Unknown0B8;			// +0xB8 (the rows)
	int Unknown0C8;										// +0xC8 (the selected item, -1 none)
	int Unknown0CC;										// +0xCC (the selected row)
	bool Unknown0D0;									// +0xD0 (the buttons made)
};

#endif
