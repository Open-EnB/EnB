// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Client\ui_item_examination_options.h (name assumed from
**	ui_item_examination_options.cpp, whose __FILE__ the client holds; no string of this header)
**
**	The item examination window's options panel ('ITEM_EXAMINATION_OPTIONS', 'OPTIONS_IEI00'; 0x80 bytes,
**	constructor 0x5352D0, made by the examination window 0x531660 at its +0xD4) and one option's row
**	('OPTION00_IEI00', 'ItemOptionObject'; 0x74 bytes, constructor 0x535CB0, destructor 0x5362F0).
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef UI_ITEM_EXAMINATION_OPTIONS_H  // NAME 3 [ui_item_examination_options.h]: header name assumed from the client's ui_item_examination_options.cpp
#define UI_ITEM_EXAMINATION_OPTIONS_H

#include "dynamic_window_vertical.h"
#include "dynamic_window_horizontal.h"
#include "dynamic_window_background.h"
#include "item/client_item_property_option.h"
#include <vector>

class GadgetCollectionInterface;  // ui_dynamicwindow.h's gadget side (the examination window's +0x84, table 0xAEE298)
class DynamicElementGadgetClass;
class ToggleButtonDynamicWindow;
class ToggleButtonStateEvent;

class UIItemExaminationOptionClass : public DynamicWindowHorizontalClass  // NAME 2 [UIItemExaminationOptionClass]: from the file name and 'ItemOptionObject'
{
public:
	UIItemExaminationOptionClass(const RefCountPtr<ClientItemPropertyOptionClass> &option, GadgetCollectionInterface *listener);	// 0x535CB0  // NAME 1 [option, listener]
	virtual ~UIItemExaminationOptionClass();												// +0x04 0x5362F0 (deleting 0x5362C0)

protected:
	void On_Toggle_State(ToggleButtonStateEvent *event, ToggleButtonDynamicWindow *toggle);	// 0x536450 (FAIL_IFs '!event', '!Option': the option's slot +0x18 with the state)  // NAME 1 [On_Toggle_State, toggle]; NAME 3 [event]: the FAIL_IF's

	RefCountPtr<ClientItemPropertyOptionClass> Option;										// +0x64  // NAME 3 [Option]: the FAIL_IF's '!Option'
	GadgetCollectionInterface *Listener;													// +0x68 (takes the tooltip's gadget)  // NAME 1 [Listener]: as the panel's
	DynamicElementGadgetClass *Gadget;														// +0x6C (the tooltip's, 'ItemOptionObject')  // NAME 1 [Gadget]: what it holds
	RefCountPtr<ToggleButtonDynamicWindow> Toggle;											// +0x70 (if_check_off.tga / if_check_on.tga)  // NAME 1 [Toggle]: what it holds
};

class UIItemExaminationOptionsClass : public DynamicWindowVerticalClass  // NAME 2 [UIItemExaminationOptionsClass]: class from file (D24), ui_item_examination_options.cpp
{
public:
	UIItemExaminationOptionsClass(GadgetCollectionInterface *listener);			// 0x5352D0  // NAME 1 [listener]
	virtual ~UIItemExaminationOptionsClass();											// +0x04 0x5357E0

	void Add_Option(ClientItemPropertyOptionClass *option);								// 0x535970  // NAME 1 [Add_Option, option]: appends to Options
	void Rebuild();																			// 0x535A10  // NAME 1 [Rebuild]: empties the panel, adds the frame and one row per option
	void Clear();																			// 0x535BF0  // NAME 1 [Clear]: removes the rows and drops the options

protected:
	GadgetCollectionInterface *Listener;											// +0x64  // NAME 1 [Listener]: passed to each row
	std::vector< RefCountPtr<ClientItemPropertyOptionClass> > Options;						// +0x68  // NAME 1 [Options]
	RefCountPtr<DynamicWindowBackgroundClass> Frame;										// +0x78 ('ITEM_EXAMINATION_OPTIONS')  // NAME 1 [Frame]
	RefCountPtr<DynamicWindowVerticalClass> List;											// +0x7C (the rows)  // NAME 1 [List]
};

#endif
