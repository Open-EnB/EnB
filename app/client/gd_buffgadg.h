// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Client\gd_buffgadg.h (D24: the header of gd_buffgadg.cpp, whose __FILE__
**	0xB8CEF0 the client holds; the client holds no string of this header)
**
**	The buff gadget of ui_buff.cpp's display (table 0xAF9E70, 0xD8 bytes): one buff's icon with its count text
**	('BuffCount'), its values (BuffInfoStruct: the ship's BuffArray.N) and their elements (BuffElementStruct: its
**	Elements.N), over the 0x80-byte gadget of constructor 0x671300 (table 0xAFB278) over the button gadget.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef GD_BUFFGADG_H
#define GD_BUFFGADG_H

#include <string>
#include <vector>
#include "unknown_671300.h"
#include "..\shared\duration.h"
#include "..\..\wwlib\ref_ptr.h"

class ClientClass;
class FontSys;
class Vector2;
struct BuffDefStruct;

// One buff element's values (0x30 bytes; the ship's BuffArray.N.Elements.N)
struct BuffElementStruct	// NAME 1: neutral (D10)
{
	BuffElementStruct(const std::string &source_entity, const std::string &source_object, int magnitude, const DurationClass &expiration_time, bool is_active);	// 0x665940  // NAME 1 [source_entity, source_object, magnitude, expiration_time, is_active]: the aux fields ui_buff.cpp's 0x5615C0 passes

	std::string SourceEntity;							// +0x00  // NAME 2: the aux field's
	std::string SourceObject;							// +0x10  // NAME 2: the aux field's
	DurationClass ExpirationTime;						// +0x20  // NAME 2: the aux field's
	int Magnitude;										// +0x28  // NAME 2: the aux field's
	bool IsActive;										// +0x2C  // NAME 2: the aux field's
};

// One buff's values (0x30 bytes; the ship's BuffArray.N)
struct BuffInfoStruct	// NAME 1: neutral (D10)
{
	BuffInfoStruct();									// 0x665180
	void Set_Scrub_Type_Name(const std::string &name);	// 0x6651C0  // NAME 1 [Set_Scrub_Type_Name, name]: the member it sets
	std::string Get_Scrub_Type_Name() const { return ScrubTypeName; }	// 0x6669D0 (out of line in 0x666570)  // NAME 1: the member it answers
	void Set_Is_Permanent(bool permanent);				// 0x665340  // NAME 1 [Set_Is_Permanent, permanent]: the member it sets
	void Set_Buff_Removal_Time(const DurationClass &time);	// 0x665360  // NAME 1 [Set_Buff_Removal_Time, time]: the member it sets
	void Add_Element(const BuffElementStruct &element);	// 0x665390 (push_back)  // NAME 1 [Add_Element, element]: what it does
	unsigned int Get_Element_Count() const;				// 0x665630  // NAME 1: what it answers
	int Get_Active_Count() const;						// 0x665670 (the elements with IsActive)  // NAME 1: what it answers
	const BuffElementStruct &Get_Element(unsigned int index) const;	// 0x6656A0 (FAIL_IF 'index >= Elements.size()': a static empty element)  // NAME 1 [Get_Element]: what it does; NAME 3 [index]: the FAIL_IF's
	void Sort_Elements();								// 0x665A10 (std::sort: active first, the larger magnitude, the later expiry, the names)  // NAME 1: what it does

	std::string ScrubTypeName;							// +0x00  // NAME 2: the aux field's
	bool IsPermanent;									// +0x10  // NAME 2: the aux field's
	DurationClass BuffRemovalTime;						// +0x18  // NAME 2: the aux field's
	std::vector<BuffElementStruct> Elements;			// +0x20  // NAME 3: the FAIL_IF's 'index >= Elements.size()'
};

// The buff gadget (0xD8 bytes, table 0xAF9E70)
class BuffGadgetClass : public Unknown_671300Class	// NAME 3: class-from-file (D24) for app\Client\gd_buffgadg.cpp
{
public:
	BuffGadgetClass(ClientClass *client, BuffDefStruct *buffinfo, const BuffInfoStruct &info, RenderObjClass *robj, int unknown, const LayerClass &layer);	// 0x665A40 (G_ASSERT 'BuffCount' / 'robj')  // NAME 1 [client, info, unknown, layer]: what ui_buff.cpp's 0x560D20 passes; NAME 3 [buffinfo, robj]: the asserts'
	virtual ~BuffGadgetClass();							// +0x00 0x665D60 (deleting 0x665D30; G_ASSERT 'robj')
	virtual void Unknown_AC();							// +0xAC 0x665F20 (the pulse while the buff runs out, then the texture scrolled)
	bool Unknown_6661F0(const BuffInfoStruct &info, const DurationClass &now);	// 0x6661F0 (the values taken: true when the elements grew or the buff runs out sooner)  // NAME 1 [info, now]: what ui_buff.cpp's 0x560BC0 passes
	void Unknown_666440(bool unknown);					// 0x666440 (the pulse started: +0xB8 = 1, +0xBC = 0; the argument unused)  // NAME 1 [unknown]: neutral
	bool Unknown_666470();								// 0x666470 (true: the buff is over)
	void Unknown_6664E0(bool on);						// 0x6664E0 (on: the BuffDefStruct's AltToolTip as the tooltip, bit 6 of +0x2C set)  // NAME 1 [on]: neutral
	void Unknown_666570();								// 0x666570 (the tooltip text +0xC4: the tool tip, (type), a line per element)

	BuffDefStruct *BuffInfo;							// +0x80  // NAME 3: the assert's 'buffinfo' (ui_buff.cpp)
	bool Unknown84;										// +0x84 (0x6664E0: the alternative tooltip shown)
	BuffInfoStruct Info;								// +0x88  // NAME 1: its type
	float Unknown_B8;									// +0xB8 (the pulse's length, seconds; 0x666440 sets 1)
	float Unknown_BC;									// +0xBC (the pulse's time)
	RefCountPtr<FontSys> BuffCount;						// +0xC0 (the count of elements, 'BuffCount' on the model)  // NAME 3: the assert's
	std::string Unknown_C4;								// +0xC4 (the tooltip's text)
	ClientClass *Client;								// +0xD4 (its +0x10BC: the time)  // NAME 1: its type
};

#endif
