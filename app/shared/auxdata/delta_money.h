// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: unknown; delta_money.cpp's (e:\build\beta\G\app\shared\auxdata\) is assumed.
**	// NAME 1 [delta_money.h]: provisional file name (the specialisation's members are delta_money.cpp's)
**
**	The money field: delta_template.h's DeltaTemplateClass, specialised for MoneyClass. Every member
**	is delta_money.cpp's, out of line: the roots call the constructor (0x4A7080) and the destructor
**	(0x4A7100) and their EH states count the destructor as one that can throw (Player's destructor
**	0x7551D0 stores state 0 before its call; the Item / EquipItem destructors 0x6FA4C0 / 0x702750 call
**	it where an implicit one is inlined). Read and Write are the money field's own (8 bytes at +0x88).
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __DELTA_MONEY_H
#define __DELTA_MONEY_H

#include "delta_template.h"
#include "..\money.h"

template<> class DeltaTemplateClass<MoneyClass> : public DeltaClass
{
public:
	DeltaTemplateClass(const char *name, const MoneyClass &value = MoneyClass());	// 0x4A7080
	virtual ~DeltaTemplateClass();											// 0x4A7100

	virtual void Unknown_00(KVEncoderClass *visitor);					// +0x00
	virtual bool Read(LinearBufferClass &buf, const DurationClass &base);	// +0x2C 0x4A7470
	virtual bool Write(LinearBufferClass &buf);				// +0x30 0x4A74F0

	void Set_Value(const MoneyClass &value);	// 0x4A7150  // NAME 2: describes what it does

	MoneyClass Value;	// +0x88 (8-aligned)  // NAME 2: describes what it does

	static int UnknownGlobal;	// 0xBE3A94
};

#endif //__DELTA_MONEY_H
