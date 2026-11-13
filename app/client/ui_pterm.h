// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: app\client\ui_pterm.h (PROVISIONAL, D137: the header of ui_pterm.cpp)
**
**	The petition terminal ui_pterm.cpp rebuilds (0x5DDCA0-0x5DE88A): a mesh window ('TLI10A') with three pages,
**	ui_ptmis.cpp's missions, ui_ptnew.cpp's news and ui_ptppl.cpp's people, chosen by its buttons.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef UI_PTERM_H  // NAME 1 [ui_pterm.h]: provisional header name for ui_pterm.cpp
#define UI_PTERM_H

#include "unknown_5bb250.h"

class UIViewClass;
class ButtonGadgetClass;
class UIPTMisClass;
class UIPTNewClass;
class UIPTPplClass;
class Unknown5DE6E0Class;

// The petition terminal (0xD8 bytes, tables 0xAF5FB4 / 0xAF5FA8 / 0xAF5F9C; nothing in the client makes one)
class UIPTermClass : public Unknown5BB320Class	// NAME 2: class-from-file (D24) for the provisional ui_pterm.cpp ('fa_pterm.ini')
{
public:
	enum { MAX_BUTTONS = 5 };							// NAME 1: the constructor's loop (buttons 1..4)

	UIPTermClass(UIViewClass *view, LayerClass *layer);	// 0x5DDCA0  // NAME 1 [view, layer]: as its pages'
	virtual ~UIPTermClass();							// 0x5DDEF0 (deleting 0x5DDEC0)
	virtual void Unknown_54();							// 0x5DE030 (makes the three pages)
	virtual void Unknown_58();							// 0x5DE230 (opens: the 'icl01a' animation, the missions page)
	virtual void Unknown_5C();							// 0x5DE350 (closes)
	virtual void Unknown_60();							// 0x5DE410 (every frame)
	virtual bool Unknown_68(int key);					// 0x5DE480 (the buttons: 1..3 a page, 4 exit)
	virtual void Unknown_7C(UIViewClass *view, bool arg);	// 0x5DE1C0 (InterfaceClass's, then the pages')  // NAME 1 [view, arg]: InterfaceClass's
	void Unknown_5DE500(int page);						// 0x5DE500: a page shown ('MISSIONS_PTERM', 'NEWS_PTERM', 'PEOPLE_PTERM')  // NAME 1 [page]: neutral

	int Unknown0AC;										// +0xAC (the page shown: 0 none, 1 missions, 2 news, 3 people)
	UIPTMisClass *Unknown0B0;							// +0xB0 (the missions page)
	UIPTNewClass *Unknown0B4;							// +0xB4 (the news page)
	UIPTPplClass *Unknown0B8;							// +0xB8 (the people page)
	ButtonGadgetClass *Unknown0BC[MAX_BUTTONS];			// +0xBC (the constructor fills 1..4: 'TLI10A.BUTTON00'..'02', 'TLI10A.EXIT')
	Unknown5DE6E0Class *Unknown0D0;						// +0xD0 (the 'icl01a' animation while open)
	bool Unknown0D4;									// +0xD4 (exit pressed)
};

#endif
