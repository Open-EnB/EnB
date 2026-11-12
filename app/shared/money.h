// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\shared\money.h (name assumed from money.cpp)
**
**	An amount of money: 8 bytes, 8-aligned (the money field keeps it at +0x88, after DeltaClass's
**	0x84 bytes). money.cpp (0x4F0080-0x4F0CB0) holds the members and the operators. The client
**	names the member Amount (its asserts); it doesn't name the class (D24).
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
	MoneyClass() : Amount(0) {}
	MoneyClass(__int64 value) : Amount(value) {}
	~MoneyClass() {}

	// DeltaTemplateClass<MoneyClass>::Set_Value compares (VC6 instantiates it with the class's virtuals)
	bool operator!=(const MoneyClass &that) const { return Amount != that.Amount; }
	// money.cpp 0x4F00C0: the amount in decimal; with commas, a ',' between every three digits ("29,597,354")
	std::string Amount_As_Text(bool commas) const;  // NAME 2 [Amount_As_Text]: what it does (the length of the placeholder it replaces: the symbol pads)

#ifdef __MONEY_MEMBERS	// money.cpp only: other files' declaration counts move aux_roots.cpp's SYMBOL_PAD windows (workflow.md 3.3), so they do not see these
	unsigned int Divide(float price_per_unit) const;	// NAME 2: describes what it does
	unsigned int Divide(const MoneyClass &price_per_unit) const;	// NAME 2: describes what it does
	MoneyClass Div(unsigned int divisor, MoneyClass *remainder) const;	// 0x4F0320: the quotient; the rest goes to remainder  // NAME 2 [Div]: describes what it does
	float operator/(int count) const;	// NAME 2: describes what it does
	float operator/(const MoneyClass &money) const;	// NAME 2: describes what it does
	int As_Int() const;	// NAME 2: describes what it does
	unsigned int As_Unsigned_Int() const;	// NAME 2: describes what it does
#endif

	__int64 Amount;			// +0x00 read and written as 8 bytes; the client's name (money.cpp's asserts, lines 38 and 60)
};

#endif //__MONEY_H
