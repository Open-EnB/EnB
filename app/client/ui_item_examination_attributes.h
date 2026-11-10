// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: assumed e:\build\beta\G\app\Client\ui_item_examination_attributes.h (interim, D137:
**	the client holds no string of this file or its .cpp; named after its window section 'ITEM_EXAMINATION_ATTRIBUTES'
**	as ui_item_examination_options.cpp's panel is 'ITEM_EXAMINATION_OPTIONS')
**
**	The item examination window's attributes panel ('ITEM_EXAMINATION_ATTRIBUTES', 'ATTRIBUTES_IEI00'; 0x7C bytes,
**	table 0xAEE390, its container side 0xAEE380 at +0x4C; constructor 0x532BA0, destructor 0x533000, deleting
**	0x532FD0; made by the examination window 0x531660) and one attribute's row ('ATTRIBUTE00_IEI00'; 0x6C bytes,
**	constructor 0x533450).
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef UI_ITEM_EXAMINATION_ATTRIBUTES_H  // NAME 4 [ui_item_examination_attributes.h]: assumed from the window section 'ITEM_EXAMINATION_ATTRIBUTES' (interim, D137)
#define UI_ITEM_EXAMINATION_ATTRIBUTES_H

#include "dynamic_window_vertical.h"
#include "dynamic_window_horizontal.h"
#include "dynamic_window_background.h"
#include "item/client_item_property_attribute.h"
#include <vector>

class GadgetCollectionInterface;

class UIItemExaminationAttributeClass : public DynamicWindowHorizontalClass  // NAME 2 [UIItemExaminationAttributeClass]: from 'ATTRIBUTE00_IEI00', as UIItemExaminationOptionClass
{
public:
	UIItemExaminationAttributeClass(const RefCountPtr<ClientItemPropertyAttributeClass> &attribute, GadgetCollectionInterface *listener);	// 0x533450  // NAME 1 [attribute, listener]
	virtual ~UIItemExaminationAttributeClass();

protected:
	int Unknown64;																			// +0x64  // NAME 1 [Unknown64]: neutral (D10)
	int Unknown68;																			// +0x68  // NAME 1 [Unknown68]: neutral (D10)
};

class UIItemExaminationAttributesClass : public DynamicWindowBackgroundClass  // NAME 2 [UIItemExaminationAttributesClass]: from 'ITEM_EXAMINATION_ATTRIBUTES', as UIItemExaminationOptionsClass
{
public:
	UIItemExaminationAttributesClass(GadgetCollectionInterface *listener);		// 0x532BA0  // NAME 1 [listener]
	virtual ~UIItemExaminationAttributesClass();											// +0x04 0x533000 (deleting 0x532FD0)

	void Add_Attribute(ClientItemPropertyAttributeClass *attribute);						// 0x5331B0  // NAME 1 [Add_Attribute, attribute]: appends to Attributes
	void Rebuild();																			// 0x533250  // NAME 1 [Rebuild]: one row per attribute, as the options panel's
	void Clear();																			// 0x5333B0  // NAME 1 [Clear]: as the options panel's

protected:
	GadgetCollectionInterface *Listener;											// +0x64  // NAME 1 [Listener]: passed to each row
	std::vector< RefCountPtr<ClientItemPropertyAttributeClass> > Attributes;				// +0x68  // NAME 1 [Attributes]
	RefCountPtr<DynamicWindowVerticalClass> List;											// +0x78 (the rows)  // NAME 1 [List]
};

#endif
