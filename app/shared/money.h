/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\shared\money.h (name assumed from money.cpp)
**
**	An amount of money: 8 bytes, 8-aligned (the money field keeps it at +0x88, after DeltaClass's
**	0x84 bytes). Only what delta_money.cpp uses so far: money.cpp (0x4F0220-...) is not rebuilt
**	yet. The client doesn't name the class (D24).
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __MONEY_H  // NAME 3 [money.h]: header name assumed from the client's money.cpp
#define __MONEY_H

#include <string>

class MoneyClass  // NAME 3: by convention (D24) from the client's file money.cpp (D37)
{
public:
	// The constructors and the destructor (inline, M1.3): the roots' money fields get a temporary
	// MoneyClass for their default value (zeroed, or Price's -1: two stores, 0x6FA150; a zeroed
	// local in 0x4A77A0), and the constructors' EH states count it as an object with a destructor
	// (Player 0x752510: states 1, 3; Read's local is an __int64: 0x4A7470 has no EH frame)
	MoneyClass() : m_Unknown00(0) {}
	MoneyClass(__int64 value) : m_Unknown00(value) {}
	~MoneyClass() {}

	// DeltaTemplateClass<MoneyClass>::Set_Value compares (VC6 instantiates it with the class's virtuals)
	bool operator!=(const MoneyClass &that) const { return m_Unknown00 != that.m_Unknown00; }
	// money.cpp 0x4F00C0 (not rebuilt): the amount in decimal; the flag's meaning unknown (opcode 220's Print passes false)
	std::string Unknown_4F00C0(bool unknown) const;  // NAME 1: neutral placeholder (D10)

	__int64 m_Unknown00;		// +0x00 read and written as 8 bytes
};

#endif //__MONEY_H
