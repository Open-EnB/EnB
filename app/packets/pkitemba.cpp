// Earth & Beyond client (reconstruction): app\packets\pkitemba.cpp. Notes at the end of the file.
#include "pkitemba.h"
#include "..\shared\unknown_debug.h"
#include <stdio.h>

ItemBasePacketClass::ItemBasePacketClass(int item_template_id, int category, int subcategory, int item_type, const RefCountPtr<ItemBaseInfoClass> &info,
	short game_basset, short icon_base_asset, short tech_level, int cost, int max_stack, unsigned int effect_usage, int flags,
	const std::string &name, const std::string &unknown30, const std::string &unknown40, int channel) :
	Channel(channel),
	ItemTemplateID(item_template_id),
	Category(category),
	Subcategory(subcategory),
	ItemType(item_type),
	Name(name),
	Unknown30(unknown30),
	Unknown40(unknown40),
	Cost(cost),
	MaxStack(max_stack),
	GameBasset(game_basset),
	IconBaseAsset(icon_base_asset),
	TechLevel(tech_level),
	EffectUsage(effect_usage),
	Flags(flags),
	Info(info)
{
















	FAIL_IF(!Info) {
		Info = Create_NEW(new ItemBaseInfoClass);
	}
}

ItemBasePacketClass::ItemBasePacketClass(RawPacketClass *raw)
{
	Info = Create_NEW(new ItemBaseInfoClass);
	raw->Get(ItemTemplateID);
	raw->Get(&Category, sizeof(Category));
	raw->Get(&Subcategory, sizeof(Subcategory));
	raw->Get(&ItemType, sizeof(ItemType));
	Info->Read(raw);
	raw->Get(GameBasset);
	raw->Get(IconBaseAsset);
	raw->Get(TechLevel);
	raw->Get(Cost);
	raw->Get(MaxStack);
	int effect_usage;
	raw->Get(effect_usage);
	EffectUsage = effect_usage;
	raw->Get(&Flags, sizeof(Flags));
	raw->Get(&Name);
	raw->Get(&Unknown30);
	raw->Get(&Unknown40);
}

ItemBasePacketClass::~ItemBasePacketClass()
{
	Info = 0;
}

RawPacketClass *ItemBasePacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(37, Get_Data_Size());
	raw->Add(ItemTemplateID);
	raw->Add(&Category, sizeof(Category));
	raw->Add(&Subcategory, sizeof(Subcategory));
	raw->Add(&ItemType, sizeof(ItemType));
	Info->Write(raw);
	raw->Add(GameBasset);
	raw->Add(IconBaseAsset);
	raw->Add(TechLevel);
	raw->Add(Cost);
	raw->Add(MaxStack);
	int effect_usage = EffectUsage;
	raw->Add(effect_usage);
	raw->Add(&Flags, sizeof(Flags));
	raw->Add(Name);
	raw->Add(Unknown30);
	raw->Add(Unknown40);
	return(raw);
}

int ItemBasePacketClass::Get_Data_Size()
{
	return(RawPacketClass::Get_Size(Unknown40) + RawPacketClass::Get_Size(Unknown30) + RawPacketClass::Get_Size(Name)
		+ sizeof(ItemTemplateID) + sizeof(Category) + sizeof(Subcategory) + sizeof(ItemType) + sizeof(GameBasset) + sizeof(IconBaseAsset)
		+ sizeof(TechLevel) + sizeof(Cost) + sizeof(MaxStack) + sizeof(EffectUsage) + sizeof(Flags) + Info->Get_Size());
}

int ItemBasePacketClass::Print(char *buffer, int len)
{
	int n = _snprintf(buffer, len, "%s {ItemTemplateID=%d Category=%d Subcategory=%d ItemType=%d Name=%0.64s Cost=%d MaxStack=%d TechLevel=%d EffectUsage=%d  GameBasset=%d IconBaseAsset=%hd Flags=%x}",
		"Item_Base", ItemTemplateID, Category, Subcategory, ItemType, Name.c_str(), Cost, MaxStack, TechLevel, EffectUsage, GameBasset, IconBaseAsset, Flags);
	if (len > 0) {
		buffer[len - 1] = 0;
	}
	if (n <= 0) {
		n = len - 1;
	}
	return(n);
}


/*
**	Reconstruction notes (pkitemba.cpp)
**
**	* Client 0x895300-0x895BD0 (0x8952D0 is the previous object's guarded static, 0xC416FC; this one's is
**	  0x895BD0); its FAIL_IF '!Info' (line 42) names the file. Opcode 37 (ITEM_BASE), written by hand
**	  (builder P5, 2026-10-27): the value constructor first. Info, a RefCountPtr to the item's
**	  0x90-byte description (0x83F490, not rebuilt), reads, writes and sizes its own part of the
**	  message between ItemType and GameBasset; both constructors make one through Create_NEW
**	  (0x528EF0, the shared out-of-line copy) when there is none.
*/
