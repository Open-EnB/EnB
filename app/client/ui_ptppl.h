// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: app\client\ui_ptppl.h (PROVISIONAL, D137: the header of ui_ptppl.cpp)
**
**	The petition terminal's people page ui_ptppl.cpp rebuilds (0x5E09D0-0x5E1B88); ui_pterm.cpp's 0x5DE030
**	makes it.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef UI_PTPPL_H  // NAME 1 [ui_ptppl.h]: provisional header name for ui_ptppl.cpp
#define UI_PTPPL_H

#include "intrface.h"
#include "layer.h"
#include <vector>

class UIViewClass;
class RenderObjClass;
class RefCountClass;
class TextureClass;
class FontSys;
class UISliderGadget;
class Unknown696CD0Class;

// One row of the people list (0x10 bytes): its four text styles
struct UIPTPplRowStruct  // NAME 1: neutral (D10)
{
	FontSys *Name;										// +0x00 ('Name_%02d_PT_PEOPLE')  // NAME 1: the style's name
	FontSys *Prof;										// +0x04 ('Prof_%02d_PT_PEOPLE')  // NAME 1: the style's name
	FontSys *Loc;										// +0x08 ('Loc_%02d_PT_PEOPLE')  // NAME 1: the style's name
	FontSys *Level;										// +0x0C ('Level_%02d_PT_PEOPLE')  // NAME 1: the style's name
};

// The people page (0xE4 bytes, tables 0xAF6388 / 0xAF637C / 0xAF6370)
class UIPTPplClass : public InterfaceClass	// NAME 3: class-from-file (D24) for app\Client\ui_ptppl.cpp
{
public:
	UIPTPplClass(UIViewClass *view, LayerClass *layer);	// 0x5E09D0  // NAME 1 [view, layer]: as LoginInterface's
	virtual ~UIPTPplClass();							// 0x5E0C60 (deleting 0x5E0C30)
	virtual void Unknown_54();							// 0x5E0DE0 (the slot +0x10, as the base's)
	virtual void Unknown_58();							// 0x5E0E00 (opens: the rows' styles and the buttons)
	virtual void Unknown_5C();							// 0x5E11E0 (closes)
	virtual void Unknown_60();							// 0x5E12F0 (empty)
	virtual bool Unknown_68(int key);					// 0x5E1310 (the buttons: 1 the list, 6.. a row)
	void Unknown_5E1390(int index);						// 0x5E1390: a person selected  // NAME 1 [index]: neutral
	void Unknown_5E15F0();								// 0x5E15F0: the rows filled from the list
	void Unknown_Make_Rows();							// (inline in 0x5E0E00: a row per 'Name_%02d_PT_PEOPLE' style)  // NAME 1: neutral (D10)
	void Unknown_Make_Buttons();						// (inline in 0x5E0E00: the rows' and the page's buttons, once)  // NAME 1: neutral (D10)

	LayerClass Layer;									// +0x84 (a copy of the constructor's layer)  // NAME 1: what it holds
	RenderObjClass *ListScreen;							// +0xA8 ('TLI08A')  // NAME 3: the G_ASSERT's
	RenderObjClass *DescScreen;							// +0xAC ('TLI09A')  // NAME 3: the G_ASSERT's
	Unknown696CD0Class *Unknown0B0;						// +0xB0 (pt_data.cpp's people list)
	std::vector<UIPTPplRowStruct *> Unknown0B4;			// +0xB4 (the rows)
	UISliderGadget *Unknown0C4;				// +0xC4 (the list's scroll bar)
	RefCountClass *Unknown0C8;							// +0xC8 (released on close; nothing here sets it)
	int Unknown0CC;										// +0xCC
	TextureClass *Unknown0D0;							// +0xD0 ('IF_TERR.TGA')
	TextureClass *Unknown0D4;							// +0xD4 ('IF_CIR20.TGA')
	int Unknown0D8;										// +0xD8 (the selected person, -1 none)
	int Unknown0DC;										// +0xDC (the selected row)
	bool Unknown0E0;									// +0xE0 (the buttons made)
};

#endif
