// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location assumed: e:\build\beta\G\app\Client\slot_data.h, after slot_data_render.cpp's asserts' 'sd' and the
**	client's RTTI (.?AVSlotDataClass@@, 0xB8A5C8). The client holds no string of this header.
**
**	What a slot's data derives from, as far as skill_data.cpp needs it: constructor 0x6A7420, destructor 0x6A7530 (deleting
**	0x6A7500), table 0xAFD758 and, for its RefCountClass at +0x18, 0xAFD74C. Those functions are not rebuilt yet (D137:
**	interim placement, ui_slot_scut.h and four other files still declare their own stand-ins).
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __SLOT_DATA_H
#define __SLOT_DATA_H

#include <vector>
#include "refcount.h"
#include "ui_cpit.h"

class InvDataClass;
class SkillDataClass;
class DeltaCompoundClass;

class SlotDataClass : public UnknownObjectWatchClass, public RefCountClass	// NAME 5: the client's RTTI, 0xB8A5C8
{
public:
	SlotDataClass(SClient *client);								// 0x6A7420  // NAME 3 [client]: the assert's Get_Client()
	virtual ~SlotDataClass();										// +0x00 0x6A7530
	virtual void Unknown_Watch_0C();								// +0x0C 0x6A76A0
	virtual InvDataClass *Unknown_10() { return(0); }			// +0x10 0x6A46A0
	virtual SkillDataClass *Unknown_14() { return(0); }		// +0x14 0x688660
	virtual bool Unknown_18() = 0;								// +0x18
	virtual DeltaCompoundClass *Unknown_1C() = 0;				// +0x1C
	virtual bool Unknown_20() = 0;								// +0x20
	virtual void Unknown_24();										// +0x24 0x6A7A40

	SClient *Get_Client() { return(Client); }					// NAME 3: skill_data.cpp's FAIL_IF
	DeltaCompoundClass *Peek_Aux_Data() { return(AuxData); }	// NAME 3: skill_data.cpp's FAIL_IF

	SClient *Client;													// +0x20  // NAME 3: Get_Client()
	DeltaCompoundClass *AuxData;									// +0x24  // NAME 3: Peek_Aux_Data()
	char Unknown28[8];												// +0x28
	std::vector<int> Unknown30;									// +0x30
	std::vector<int> Unknown40;									// +0x40
};

#endif //__SLOT_DATA_H
