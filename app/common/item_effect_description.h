// Earth & Beyond client (reconstruction): app\common\item_effect_description.h
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\common\item_effect_description.h (name assumed from
**	item_effect_description.cpp, the FAIL_IF of 0x83E5F0; no assert names the header)
**
**	An item's effects: what an item's info (item_info.h) holds twice, once for the effects the
**	item has when activated and once for those it has when equipped. item_effect_description.cpp
**	(client 0x83DB00-0x83E7C0) is not rebuilt yet; the classes moved here from
**	client_item_property_builder.h, where they were declared for their first caller.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef ITEM_EFFECT_DESCRIPTION_H  // NAME 1 [item_effect_description.h]: header name assumed from the client's item_effect_description.cpp
#define ITEM_EFFECT_DESCRIPTION_H

#include "..\shared/duration.h"
#include <string>
#include <vector>

class RawPacketClass;

// One effect of an item (0x58 bytes; its copy 0x5293B0 and destructors 0x5295C0 / 0x5296D0 are the implicit ones;
// item_effect_description.cpp's, not rebuilt)
class ItemEffectClass  // NAME 1: neutral (D10)
{
public:
	std::string Unknown_833D00() const;																	// 0x833D00  // NAME 1: neutral (D10)
	std::string Unknown_833D40() const;																	// 0x833D40  // NAME 1: neutral (D10)

	std::string Unknown00;																				// +0x00  // NAME 1: neutral
	std::string Unknown10;																				// +0x10  // NAME 1: neutral
	std::string Unknown20;																				// +0x20  // NAME 1: neutral
	std::vector<float> Unknown30;																		// +0x30  // NAME 1: neutral
	std::vector<float> Unknown40;																		// +0x40  // NAME 1: neutral
	int Unknown50;																						// +0x50  // NAME 1: neutral
	int Unknown54;																						// +0x54  // NAME 1: neutral
};

// An item's effects (0x28 bytes, aligned to 8 by its duration: an item's info holds one at +0x40 and one at +0x68;
// item_effect_description.cpp's, not rebuilt)
class ItemEffectDescriptionClass  // NAME 2: item_effect_description.cpp's 0x83E5F0
{
public:
	ItemEffectDescriptionClass();																		// 0x83DB00 (no effects, everything else 0)
	const std::vector<ItemEffectClass> &Get_Effects() const { return(Effects); }	// NAME 1: neutral
	void Clear_Effects();																				// 0x83DD40 (erases Effects)  // NAME 2: what it does
	int Get_Size();																						// 0x83DE70 (its bytes in a message)  // NAME 2: what it returns
	void Write(RawPacketClass *raw);																	// 0x83DF10  // NAME 2: what it does
	void Read(RawPacketClass *raw);																		// 0x83E040  // NAME 2: what it does
	void Unknown_83E5F0(const std::string &delta);														// 0x83E5F0  // NAME 1: neutral (D10); NAME 1 [delta]: what it gets

	std::vector<ItemEffectClass> Effects;																// +0x00  // NAME 2: what it holds
	DurationClass Duration;																				// +0x10 (written as an int, 'Time > INT_MAX')  // NAME 2: its type
	int Unknown18;																						// +0x18  // NAME 1: neutral
	int Unknown1C;																						// +0x1C  // NAME 1: neutral
	int Unknown20;																						// +0x20  // NAME 1: neutral
};

#endif
