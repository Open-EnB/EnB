// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: assumed e:\build\beta\G\app\Client\ui_item_examination_specials.h (interim, D137:
**	the client holds no string of this file or its .cpp; named after its window section 'ITEM_EXAMINATION_SPECIALS'
**	as ui_item_examination_options.cpp's panel is 'ITEM_EXAMINATION_OPTIONS')
**
**	The item examination window's specials panel ('ITEM_EXAMINATION_SPECIALS', 'SPECIALS_IEI00'; 0x7C bytes,
**	table 0xAEE718, its container side 0xAEE708 at +0x4C; constructor 0x537AD0, destructor 0x537F00, deleting
**	0x537ED0; made by the examination window 0x531660) and one special's row (0x6C bytes, table 0xAEE79C,
**	constructor 0x538350).
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef UI_ITEM_EXAMINATION_SPECIALS_H  // NAME 4 [ui_item_examination_specials.h]: assumed from the window section 'ITEM_EXAMINATION_SPECIALS' (interim, D137)
#define UI_ITEM_EXAMINATION_SPECIALS_H

#include "dynamic_window_vertical.h"
#include "dynamic_window_horizontal.h"
#include "dynamic_window_background.h"
#include "item/client_item_property_special.h"
#include <vector>

class GadgetCollectionInterface;

class UIItemExaminationSpecialClass : public DynamicWindowHorizontalClass  // NAME 2 [UIItemExaminationSpecialClass]: as the attributes panel's row
{
public:
	UIItemExaminationSpecialClass(const RefCountPtr<ClientItemPropertySpecialClass> &special, GadgetCollectionInterface *listener);	// 0x538350  // NAME 1 [special, listener]
	virtual ~UIItemExaminationSpecialClass();

protected:
	int Unknown64;																			// +0x64  // NAME 1 [Unknown64]: neutral (D10)
	int Unknown68;																			// +0x68  // NAME 1 [Unknown68]: neutral (D10)
};

class UIItemExaminationSpecialsClass : public DynamicWindowBackgroundClass  // NAME 2 [UIItemExaminationSpecialsClass]: from 'ITEM_EXAMINATION_SPECIALS', as UIItemExaminationOptionsClass
{
public:
	UIItemExaminationSpecialsClass(GadgetCollectionInterface *listener);		// 0x537AD0  // NAME 1 [listener]
	virtual ~UIItemExaminationSpecialsClass();											// +0x04 0x537F00 (deleting 0x537ED0)

	void Add_Special(ClientItemPropertySpecialClass *special);						// 0x5380B0  // NAME 1 [Add_Special, special]: appends to Specials
	void Rebuild();																			// 0x538150  // NAME 1 [Rebuild]: one row per special, as the options panel's
	void Clear();																			// 0x5382B0  // NAME 1 [Clear]: as the options panel's

protected:
	GadgetCollectionInterface *Listener;											// +0x64  // NAME 1 [Listener]: passed to each row
	std::vector< RefCountPtr<ClientItemPropertySpecialClass> > Specials;				// +0x68  // NAME 1 [Specials]
	RefCountPtr<DynamicWindowVerticalClass> List;											// +0x78 (the rows)  // NAME 1 [List]
};

#endif
