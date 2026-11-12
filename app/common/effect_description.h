// Earth & Beyond client (reconstruction): app\common\effect_description.h
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\common\effect_description.h (name assumed from
**	effect_description.cpp, its asserts' file; no assert names the header)
**
**	An effect's description: its name and description texts with '%value...%' tags the values
**	fill in (effect_description.cpp, client 0x8320D0-0x834D30), and an item's effect, an effect
**	description with its usage and flags (item_effect.cpp, client 0x834D30 on).
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef EFFECT_DESCRIPTION_H  // NAME 1 [effect_description.h]: header name assumed from the client's effect_description.cpp
#define EFFECT_DESCRIPTION_H

#include <string>
#include <vector>

class RawPacketClass;

// An effect's usage (ItemEffectClass::Usage) is an EffectGeneratorUsageBF (aux_roots.h; the client's
// Value_To_String 0x835590 names its values from item_effect.cpp's table: 0 'none', 1 'activated', 2 'permanent',
// 4 'toggled')

// An effect's texts and the values their '%value...%' tags show (0x50 bytes; the client has its implicit operator= out
// of line, 0x83EE00, called where ItemEffectClass's own operator= 0x83EBF0 is inlined: the vector's insert 0x83E810)
class EffectDescriptionClass  // NAME 2: the client's file effect_description.cpp
{
public:
	EffectDescriptionClass();																			// 0x8320D0
	EffectDescriptionClass(const std::string &unknown, const std::string &name, const std::string &description);	// 0x832200  // NAME 1 [unknown]: neutral
	void Add_Name_Value(float value);																	// 0x832310  // NAME 2: what it does
	void Add_Description_Value(float value);															// 0x832530  // NAME 2: what it does
	void Get_Name_Values(std::vector<float> &values) const;												// 0x832750  // NAME 2: what it does
	void Get_Description_Values(std::vector<float> &values) const;										// 0x832930  // NAME 2: what it does
	int Get_Size();																						// 0x832B10 (its bytes in a message)  // NAME 2: as the packets'
	void Write(RawPacketClass *raw);																	// 0x832B90  // NAME 2: as the packets'
	void Read(RawPacketClass *raw);																		// 0x832CA0  // NAME 2: as the packets'
	void Set_Name(const std::string &name);																// 0x832F20  // NAME 2: what it does
	std::string Get_Name_Text() const;																	// 0x833D00 (the name with its values filled in)  // NAME 2: what it returns
	std::string Get_Description_Text() const;															// 0x833D40 (the description with its values filled in)  // NAME 2: what it returns
	std::string Get_Summary() const;																	// 0x833FE0 (the values as text)  // NAME 2: the client's 'summary_strings'
	void Set_Summary(const std::string &summary);														// 0x8342A0 (the values from that text)  // NAME 2: the client's 'summary_strings'

	std::string Unknown00;																				// +0x00  // NAME 1: neutral
	std::string Name;																					// +0x10 (filled with NameValues)  // NAME 2: the client's 'NameValues', 'name_and_description'
	std::string Description;																			// +0x20 (filled with DescriptionValues)  // NAME 2: the client's 'DescriptionValues', 'name_and_description'
	std::vector<float> NameValues;																		// +0x30  // NAME 2: the client's G_ASSERT
	std::vector<float> DescriptionValues;																// +0x40  // NAME 2: the client's G_ASSERT
};

// An effect's flags (0x83DB40 tests them as bit fields; sent as their 4 bytes)
struct ItemEffectFlagsStruct  // NAME 1: what it is
{
	unsigned Unknown0 : 1;	// NAME 1: neutral
	unsigned Unknown1 : 1;	// (0x83DB40: the effect uses the radius)  // NAME 1: neutral
	unsigned Unknown2 : 1;	// (0x83DB40: the effect uses the range and the radius)  // NAME 1: neutral
	unsigned Unknown3 : 1;	// (0x83DB40: the effect uses the radius)  // NAME 1: neutral
	unsigned Unknown4 : 3;	// (0x83DB40: not 0, the effect uses the range)  // NAME 1: neutral
};

// One effect of an item (0x58 bytes; its copy 0x5293B0 and destructors 0x5295C0 / 0x5296D0 are the implicit ones)
class ItemEffectClass : public EffectDescriptionClass  // NAME 1: neutral (D10)
{
public:
	ItemEffectClass();																					// 0x834D30
	ItemEffectClass(const EffectDescriptionClass &description, int usage, ItemEffectFlagsStruct flags);	// 0x834D60 (no caller in the client)
	int Get_Size();																						// 0x834F20 (its bytes in a message)  // NAME 2: as the packets'
	void Write(RawPacketClass *raw);																	// 0x834F40  // NAME 2: as the packets'
	void Read(RawPacketClass *raw);																		// 0x834F80  // NAME 2: as the packets'
	std::string Get_Summary() const;																	// 0x834FC0 (one of the '#'-joined summary_strings)  // NAME 2: the client's 'summary_strings'
	void Set_Summary(const std::string &summary);														// 0x834FF0  // NAME 2: the client's 'summary_strings'
	bool Is_Unknown1() const { return(Flags.Unknown1); }			// NAME 1: neutral; an inline layer the client spends budget on (0x83DB40)
	bool Is_Unknown2() const { return(Flags.Unknown2); }			// NAME 1: neutral; an inline layer the client spends budget on (0x83DB40)
	bool Is_Unknown3() const { return(Flags.Unknown3); }			// NAME 1: neutral; an inline layer the client spends budget on (0x83DB40)
	bool Is_Unknown4() const { return(Flags.Unknown4 != 0); }		// NAME 1: neutral; an inline layer the client spends budget on (0x83DB40)

	int Usage;																							// +0x50 (an EffectGeneratorUsageBF: 0x83DB40 gives an activated effect the duration)  // NAME 2: the client's enum
	ItemEffectFlagsStruct Flags;																		// +0x54  // NAME 1: what it is
};

#endif
