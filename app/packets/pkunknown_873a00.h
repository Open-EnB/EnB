/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\packets\pkunknown_873a00.h (the name provisional: see the .cpp)
**
**	The packet-data wrapper of a MoneyClass, client 0x873A00-0x873AE0 (vtable 0xB0ADC4).
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PKUNKNOWN_873A00_H  // NAME 1 [pkunknown_873a00]: provisional, neutral (D10): the object's first address
#define PKUNKNOWN_873A00_H

#include "pk.h"
#include "..\shared\money.h"

// A MoneyClass sent as its 8 bytes through RawPacketClass::Add / Get (vtable 0xB0ADC4; made on the
// stack around the call: GUILD_DEDUCT_FEE 220's raw constructor, Build and Get_Data_Size)
class MoneyPacketDataClass : public Unknown_PacketDataClass  // NAME 2: what it wraps
{
public:
	MoneyPacketDataClass(MoneyClass *money) : Money(money) {}
	virtual bool Write(LinearBufferClass *buffer);	// +0x00 0x873A00
	virtual bool Read(LinearBufferClass *buffer);	// +0x04 0x873A40
	virtual int Get_Size();	// +0x08 0x873A90

	MoneyClass *Money;	// +0x04  // NAME 2: what it points at
};

#endif
