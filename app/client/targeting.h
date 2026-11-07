// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: unknown. The object 0x7C6120-0x7C636F has no __FILE__ of its own and sits between
**	target_reticle.cpp and ui_starmap.cpp; targeting.h / targeting.cpp are assumed from the asserts' 'targeting'.
**
**	The object at SClient +0x12A4: TargetFindClass's selection, which asks the server for the target it picks.
**	Table 0xB07134, 0x14 bytes.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __TARGETING_H  // NAME 2 [targeting.h]: assumed from the asserts' 'targeting' (mainview.cpp, ui_mfd.cpp, ui_group.cpp); the link order agrees
#define __TARGETING_H

#include "target_find.h"

class TargetingClass : public TargetFindClass	// NAME 2: the asserts' 'targeting' (mainview.cpp, ui_mfd.cpp, ui_group.cpp)
{
public:
	TargetingClass(SClient *client);								// 0x7C6120
	virtual ~TargetingClass();										// +0x00 0x7C6160 (deleting), 0x7C6190
	virtual ClientObjectClass *Select(ClientObjectClass *co);		// +0x04 0x7C62B0: sends REQUEST_TARGET when co isn't the target already

	void Clear_Target();											// 0x7C61B0 (ui_mfd.cpp's 0x5BC5E0)  // NAME 2: what it does
	ClientObjectClass *Get_Target();								// 0x7C61D0  // NAME 2: what it does
	ClientObjectClass *Select_Next_Target();						// 0x7C61F0  // NAME 2: what it does
	ClientObjectClass *Select_Prev_Target();						// 0x7C6220  // NAME 2: what it does
	ClientObjectClass *Select_Next_Target_Filtered();				// 0x7C6250  // NAME 2: what it does
	ClientObjectClass *Select_Prev_Target_Filtered();				// 0x7C6280  // NAME 2: what it does

	int Unknown0C;													// +0x0C: the constructor's 0; nothing else in this object reads it  // NAME 1: neutral (D10)
	bool Unknown10;													// +0x10: set when a request goes out; mainview.cpp's 0x512400 clears it  // NAME 1: neutral (D10)
};

#endif //__TARGETING_H
