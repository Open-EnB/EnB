// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Common\recustomization_cost.h (name assumed from
**	recustomization_cost.cpp; no assert names it)
**
**	The recustomization costs (recustomization_cost.cpp, client 0x8492F0-0x84A256): one cost per
**	slot of the avatar (14) or the ship (12), sent with RECUSTOMIZE_AVATAR_START / _SHIP_START
**	(pkrecustomize.h), summed over the slots an edit changed.
*/



#if defined(_MSC_VER)
#pragma once
#endif

#ifndef RECUSTOMIZATION_COST_H  // NAME 3 [recustomization_cost.h]: header name assumed from the client's recustomization_cost.cpp
#define RECUSTOMIZATION_COST_H

#include "..\packets\pk.h"

class AvatarDescClass;
class ShipCustDescClass;
class ShipCustSettingsClass;
class LinearBufferClass;

// The avatar's 14 costs (0x38 bytes; the constructor 0x849320 copies one)
struct RecustomizationCostDataStruct  // NAME 1: neutral (D10)
{
	int Per_Cost[14];	// +0x00  // NAME 5: the FAIL_IF 'Costs.Per_Cost[slot] == 0' (line 52)
};

// The avatar's costs on the wire: vtable 0xB08C6C, 0x3C bytes; Write 0x849920 / Read 0x849960 move
// its 14 ints, Get_Size 0x8499A0 returns 0x38
class RecustomizationCostClass : public Unknown_PacketDataClass  // NAME 1: neutral, after the client's file recustomization_cost.cpp (D10)
{
public:
	RecustomizationCostClass();	// 0x8492F0 (the 14 ints 0)
	RecustomizationCostClass(const RecustomizationCostDataStruct &data);	// 0x849320  // NAME 1 [data]: neutral
	int Unknown_849360(int slot, AvatarDescClass *oldavatar, AvatarDescClass *newavatar);	// 0x849360 (lines 50-52): the slot's cost (inline in 0x8493F0)  // NAME 5 [slot, oldavatar, newavatar]: the FAIL_IFs'
	int Unknown_8493F0(AvatarDescClass *oldavatar, AvatarDescClass *newavatar);	// 0x8493F0 (lines 60 / 61): the costs of the slots that differ, summed  // NAME 5 [oldavatar, newavatar]: the FAIL_IFs'
	bool Unknown_8494C0(int slot, AvatarDescClass *oldavatar, AvatarDescClass *newavatar);	// 0x8494C0: the slot differs  // NAME 1 [slot, oldavatar, newavatar]: as 0x849360's
	virtual bool Write(LinearBufferClass *buffer);	// +0x00 0x849920
	virtual bool Read(LinearBufferClass *buffer);	// +0x04 0x849960
	virtual int Get_Size();	// +0x08 0x8499A0

	RecustomizationCostDataStruct Costs;	// +0x04  // NAME 5: the FAIL_IF 'Costs.Per_Cost[slot] == 0'
};

// The ship's 12 costs (0x30 bytes)
struct ShipRecustomizationCostDataStruct  // NAME 1: neutral (D10)
{
	int Per_Cost[12];	// +0x00  // NAME 5: the FAIL_IF 'Costs.Per_Cost[slot] == 0' (line 252)
};

// The ship's costs on the wire: vtable 0xB08C7C, 0x34 bytes; Write 0x84A1D0 / Read 0x84A210 move its
// 12 ints, Get_Size 0x84A250 returns 0x30
class ShipRecustomizationCostClass : public Unknown_PacketDataClass  // NAME 1: neutral, the ship recustomization's (D10)
{
public:
	ShipRecustomizationCostClass();	// 0x8499C0 (the 12 ints 0)
	ShipRecustomizationCostClass(const ShipRecustomizationCostDataStruct &data);	// 0x8499F0  // NAME 1 [data]: neutral
	int Unknown_849A30(int slot, ShipCustDescClass *oldship, ShipCustDescClass *newship);	// 0x849A30 (lines 250-252): the slot's cost (inline in 0x849AC0)  // NAME 5 [slot, oldship, newship]: the FAIL_IFs'
	int Unknown_849AC0(ShipCustDescClass *oldship, ShipCustDescClass *newship);	// 0x849AC0 (lines 259 / 260): the costs of the slots that differ, summed  // NAME 5 [oldship, newship]: the FAIL_IFs'
	bool Unknown_849B90(int slot, ShipCustDescClass *oldship, ShipCustDescClass *newship);	// 0x849B90: the slot differs  // NAME 1 [slot, oldship, newship]: as 0x849A30's
	bool Unknown_84A150(ShipCustSettingsClass oldsettings, ShipCustSettingsClass newsettings);	// 0x84A150: a part's colour setting differs  // NAME 1 [oldsettings, newsettings]: neutral
	virtual bool Write(LinearBufferClass *buffer);	// +0x00 0x84A1D0
	virtual bool Read(LinearBufferClass *buffer);	// +0x04 0x84A210
	virtual int Get_Size();	// +0x08 0x84A250

	ShipRecustomizationCostDataStruct Costs;	// +0x04  // NAME 5: the FAIL_IF 'Costs.Per_Cost[slot] == 0'
};

#endif
