// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pkitemba.h (the .cpp named by its FAIL_IF)
**
**	The packet class of client 0x895300-0x895BD0 (opcode 37, ITEM_BASE).
*/



#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKITEMBA_H
#define PKITEMBA_H

#include "pk.h"
#include "refcount.h"
#include "ref_ptr.h"
#include <string>

// What ITEM_BASE's Info points at: a reference-counted 0x90-byte object (vtable 0xB08C34 over
// RefCountClass; client 0x83F490-0x840400, not rebuilt) that reads, writes and sizes its own part
// of the message. Declared for opcode 37 only.
class ItemBaseInfoClass : public RefCountClass  // NAME 2: what it is (ITEM_BASE's FAIL_IF '!Info')
{
public:
	ItemBaseInfoClass();	// 0x83F490
	void Read(RawPacketClass *raw);	// 0x840030  // NAME 2: what it does
	void Write(RawPacketClass *raw);	// 0x83FEF0  // NAME 2: what it does
	int Get_Size();	// 0x83FE30  // NAME 2: what it returns

	char Unknown08[0x90 - 8];	// +0x08
};

// opcode 37 (0x0025, ITEM_BASE): vtable 0xB0AE94, constructor 0x8955C0, 0x6C bytes
class ItemBasePacketClass : public PacketClass  // NAME 3: the client's Print text 'Item_Base' and Net-7's ITEM_BASE
{
public:
	ItemBasePacketClass(int item_template_id, int category, int subcategory, int item_type, const RefCountPtr<ItemBaseInfoClass> &info,
		short game_basset, short icon_base_asset, short tech_level, int cost, int max_stack, unsigned int effect_usage, int flags,
		const std::string &name, const std::string &unknown30, const std::string &unknown40, int channel);	// 0x895300
	ItemBasePacketClass(RawPacketClass *raw);	// 0x8955C0
	virtual ~ItemBasePacketClass();	// +0x00 0x895820 / 0x895590
	virtual RawPacketClass *Build();	// +0x04 0x895960
	virtual bool Process_Packet(void *client);	// +0x08 0x7371B0 (cpacket.cpp)
	virtual int Get_Data_Size();	// +0x0C 0x895AF0
	virtual int Print(char *buffer, int len);	// +0x10 0x895B40
	virtual int Get_Channel() { return(Channel); }	// +0x18 0x895570

	int Channel;	// +0x0C (the value constructor's last argument; not sent)  // NAME 2: what Get_Channel returns
	int ItemTemplateID;	// +0x10 (network order)  // NAME 3: the client's Print text 'ItemTemplateID='
	int Category;	// +0x14  // NAME 3: the client's Print text 'Category='
	int Subcategory;	// +0x18  // NAME 3: the client's Print text 'Subcategory='
	int ItemType;	// +0x1C  // NAME 3: the client's Print text 'ItemType='
	std::string Name;	// +0x20  // NAME 3: the client's Print text 'Name='
	std::string Unknown30;	// +0x30
	std::string Unknown40;	// +0x40
	int Cost;	// +0x50 (network order)  // NAME 3: the client's Print text 'Cost='
	int MaxStack;	// +0x54 (network order)  // NAME 3: the client's Print text 'MaxStack='
	short GameBasset;	// +0x58  // NAME 3: the client's Print text 'GameBasset='
	short IconBaseAsset;	// +0x5A  // NAME 3: the client's Print text 'IconBaseAsset='
	short TechLevel;	// +0x5C  // NAME 3: the client's Print text 'TechLevel='
	unsigned int EffectUsage;	// +0x60 (sent as an int, network order)  // NAME 3: the client's Print text 'EffectUsage='
	int Flags;	// +0x64  // NAME 3: the client's Print text 'Flags='
	RefCountPtr<ItemBaseInfoClass> Info;	// +0x68  // NAME 3: the client's FAIL_IF '!Info'
};

#endif
