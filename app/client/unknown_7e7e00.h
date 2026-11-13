// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: unknown (PROVISIONAL, D137: a neutral name; the object 0x7E7E70-0x7E81AF has no assert, its
**	strings are 'if_check_on.tga' / 'if_check_off.tga')
**
**	The check box: a button gadget (unknown_675120.h's ButtonGadgetClass, mask 0x11) that shows one of two textures.
**	Table 0xB075E4 (41 slots: RObjGadgetClass's, +0x2C its own), constructors 0x7E7E70 (with a layer) / 0x7E7F20,
**	destructor 0x7E7FA0, 0x68 bytes; code in unknown_7e7e00.cpp. The windows that tick boxes (ui_friends.cpp,
**	ui_option_channel.cpp, ui_option_check.cpp) take the class from here. ui_manu.cpp still has its own view of
**	it (its own ButtonGadgetClass too, so it cannot include this header yet), declared to link to these definitions.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef UNKNOWN_7E7E00_H  // NAME 1 [unknown_7e7e00.h]: neutral (D137)
#define UNKNOWN_7E7E00_H

#include "unknown_675120.h"

class CheckBoxGadgetClass : public ButtonGadgetClass  // NAME 2: what it is, a check box ('if_check_on.tga' / 'if_check_off.tga'; D239 renamed it from UIManuCheckButtonClass)
{
public:
	CheckBoxGadgetClass(RenderObjClass *robj, const LayerClass &layer, unsigned int flags);	// 0x7E7E70 (ButtonGadgetClass 0x675240 with mask 0x11; ui_manu.cpp's 0x5B6410 passes 0, or 0x55)  // NAME 1 [robj, layer, flags]: the base's
	CheckBoxGadgetClass(RenderObjClass *robj, unsigned int flags);	// 0x7E7F20 (ButtonGadgetClass 0x675310; ui_chat_panel_base.cpp's 0x56D6F0)  // NAME 1 [robj, flags]: the base's
	virtual ~CheckBoxGadgetClass();						// +0x00 0x7E7FA0 (deleting 0x7E7EF0)
	virtual bool Unknown_2C(KernelEventClass *event, float x, float y, unsigned int mask);	// +0x2C 0x7E7FC0  // NAME 1 [event, x, y, mask]: the base's
	void Unknown_7E8030(bool on);						// 0x7E8030 (Checked set, then 0x7E8050)  // NAME 1 [on]: neutral
	void Unknown_7E8050();								// 0x7E8050 (the model's texture: 'if_check_on.tga' or 'if_check_off.tga')

	bool Checked;										// +0x64  // NAME 2: what 0x7E8030 sets
};

#endif
