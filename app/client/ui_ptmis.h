// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: app\client\ui_ptmis.h (PROVISIONAL, D137: the header of ui_ptmis.cpp)
**
**	The petition terminal's missions page ui_ptmis.cpp rebuilds (0x5DE8A0-0x5DF960); ui_pterm.cpp's 0x5DE030
**	makes it.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef UI_PTMIS_H  // NAME 1 [ui_ptmis.h]: provisional header name for ui_ptmis.cpp
#define UI_PTMIS_H

#include "intrface.h"
#include "layer.h"
#include <vector>

class UIViewClass;
class RenderObjClass;
class FontSys;
class Unknown675370GadgetClass;
class Unknown696320Class;

// One row of the missions list (0x0C bytes): its three text styles
struct UIPTMisRowStruct  // NAME 1: neutral (D10)
{
	FontSys *Mission;									// +0x00 ('Mission_%02d_PT_MIS')  // NAME 1: the style's name
	FontSys *Request;									// +0x04 ('Request_%02d_PT_MIS')  // NAME 1: the style's name
	FontSys *Payout;									// +0x08 ('Payoutval_%02d_PT_MIS')  // NAME 1: the style's name
};

// The missions page (0xD4 bytes, tables 0xAF6100 / 0xAF60F4 / 0xAF60E8)
class UIPTMisClass : public InterfaceClass	// NAME 3: class-from-file (D24) for app\Client\ui_ptmis.cpp
{
public:
	UIPTMisClass(UIViewClass *view, LayerClass *layer);	// 0x5DE8A0  // NAME 1 [view, layer]: as LoginInterface's
	virtual ~UIPTMisClass();							// 0x5DEAE0 (deleting 0x5DEAB0)
	virtual void Unknown_54();							// 0x5DECF0 (the slot +0x10, as the base's)
	virtual void Unknown_58();							// 0x5DED10 (opens: the rows' styles and the buttons)
	virtual void Unknown_5C();							// 0x5DF120 (closes)
	virtual void Unknown_60();							// 0x5DF200 (empty)
	virtual bool Unknown_68(int key);					// 0x5DF220 (the buttons: 1 the list, 5.. a row)
	void Unknown_5DF340(int index);						// 0x5DF340: a mission selected  // NAME 1 [index]: neutral
	void Unknown_5DF410();								// 0x5DF410: the rows filled from the list
	void Unknown_Make_Rows();							// (inline in 0x5DED10: a row per 'Mission_%02d_PT_MIS' style)  // NAME 1: neutral (D10)
	void Unknown_Make_Buttons();						// (inline in 0x5DED10: the rows' and the page's buttons, once)  // NAME 1: neutral (D10)

	LayerClass Layer;									// +0x84 (a copy of the constructor's layer)  // NAME 1: what it holds
	RenderObjClass *ListScreen;							// +0xA8 ('TLI03A')  // NAME 3: the G_ASSERT's
	RenderObjClass *DescScreen;							// +0xAC ('TLI06A')  // NAME 3: the G_ASSERT's
	Unknown675370GadgetClass *Unknown0B0;				// +0xB0 (the list's scroll bar)
	Unknown696320Class *Unknown0B4;						// +0xB4 (pt_data.cpp's missions list)
	std::vector<UIPTMisRowStruct *> Unknown0B8;			// +0xB8 (the rows)
	int Unknown0C8;										// +0xC8 (the selected mission, -1 none)
	int Unknown0CC;										// +0xCC (the selected row)
	bool Unknown0D0;									// +0xD0 (the buttons made)
};

#endif
