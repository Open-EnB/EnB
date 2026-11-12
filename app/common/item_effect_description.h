// Earth & Beyond client (reconstruction): app\common\item_effect_description.h
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\common\item_effect_description.h (name assumed from
**	item_effect_description.cpp, the FAIL_IF of 0x83E5F0; no assert names the header)
**
**	An item's effects: what an item's info (item_info.h) holds twice, once for the effects the
**	item has when activated and once for those it has when equipped (item_effect_description.cpp,
**	client 0x83DB00-0x83E7C0). ItemEffectDescriptionClass moved here from client_item_property_builder.h,
**	where it was declared for its first caller; ItemEffectClass is effect_description.h's.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef ITEM_EFFECT_DESCRIPTION_H  // NAME 1 [item_effect_description.h]: header name assumed from the client's item_effect_description.cpp
#define ITEM_EFFECT_DESCRIPTION_H

#include "..\shared/duration.h"
#include "effect_description.h"
#include <string>
#include <vector>

class RawPacketClass;

class ItemEffectParameterClass;

// An item's effects (0x28 bytes, aligned to 8 by its duration: an item's info holds one at +0x40 and one at +0x68)
class ItemEffectDescriptionClass  // NAME 2: item_effect_description.cpp's 0x83E5F0
{
public:
	ItemEffectDescriptionClass();																		// 0x83DB00 (no effects, everything else 0)
	ItemEffectDescriptionClass(const std::vector<ItemEffectClass> &effects, DurationClass duration, int unused, ItemEffectParameterClass *parameters);	// 0x83DB40 (no caller in the client)  // NAME 1 [unused, parameters]: what they are
	const std::vector<ItemEffectClass> &Get_Effects() const { return(Effects); }	// NAME 1: neutral
	void Clear_Effects();																				// 0x83DD40 (erases Effects)  // NAME 2: what it does
	bool Has_Effects() const;																			// 0x83DE30 (no caller in the client)  // NAME 2: what it returns
	int Get_Size();																						// 0x83DE70 (its bytes in a message)  // NAME 2: what it returns
	void Write(RawPacketClass *raw);																	// 0x83DF10  // NAME 2: what it does
	void Read(RawPacketClass *raw);																		// 0x83E040  // NAME 2: what it does
	std::string Get_Summary() const;																	// 0x83E3A0 (no caller in the client: each effect's summary and '#')  // NAME 2: the client's 'summary_strings'
	void Set_Summary(const std::string &summary);														// 0x83E5F0 (an instance's Get_Instance_..._Effect_Info)  // NAME 2: the client's 'summary_strings'

	std::vector<ItemEffectClass> Effects;																// +0x00  // NAME 2: the client's 'Effects.size()'
	DurationClass Duration;																				// +0x10 (written as an int, 'Time > INT_MAX')  // NAME 2: its type
	float Unknown18;																					// +0x18 (Write 0x83DF10 sends it with RawPacketClass::Add(const float &))  // NAME 1: neutral
	int Range;																							// +0x1C (0x83DB40: the parameters' "Range", rounded)  // NAME 2: the client's string
	int Radius;																							// +0x20 (0x83DB40: the parameters' "Radius", rounded)  // NAME 2: the client's string
};

// What 0x83DB40 reads "Range" and "Radius" from (its second virtual function; no class of the client's known yet)
class ItemEffectParameterClass  // NAME 1: neutral, what it gives
{
public:
	virtual void Unknown_00() = 0;																		// NAME 1: neutral
	virtual void Get(const char *name, float *value) = 0;												// NAME 1: what it does
};

#endif
