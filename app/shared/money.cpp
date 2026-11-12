// Earth & Beyond client (reconstruction): app\shared\money.cpp (the client's __FILE__
// 'e:\build\beta\G\app\shared\money.cpp', asserts from 0x4F0220).
#include <limits.h>
#include <iostream>
#include <string>
#include "g_assert.h"
#define __MONEY_MEMBERS
#include "money.h"

#include "unknown_4a1590.h"
#include "unknown_4fac20.h"
#include "unknown_8b4560.h"


// Helper 0x4F0080
// @0x4F0080
MoneyClass Unknown_4F0080(const std::string &str)	// NAME 1: neutral
{
	return MoneyClass(Unknown_8B44C0(str.c_str()));
}

// 0x4F00C0: the amount in decimal; with commas, grouped in threes ("29,597,354": the PDA's "Credits:" line)
// @0x4F00C0
std::string MoneyClass::Amount_As_Text(bool commas) const
{
	char buffer[64];
	std::string text(Unknown_8B44E0(Amount, buffer));
	if (!commas) {
		return std::string(buffer);
	}
	String_Insert_Commas(text);
	return text;
}

// 0x4F0220
// @0x4F0220
unsigned int MoneyClass::Divide(float price_per_unit) const	// NAME 2: describes what it does
{
#line 29
	FAIL_IF(price_per_unit <= 0.0f) return 0;
	__int64 dividend = (__int64)((double)Amount / price_per_unit);
#line 32
	FAIL_IF(dividend > UINT_MAX) return UINT_MAX;
	return (unsigned int)dividend;
}

// 0x4F02A0
// @0x4F02A0
unsigned int MoneyClass::Divide(const MoneyClass &price_per_unit) const	// NAME 2: describes what it does
{
#line 38
	FAIL_IF(price_per_unit.Amount <= 0) return 0;
	__int64 dividend = Amount / price_per_unit.Amount;
#line 41
	FAIL_IF(dividend > UINT_MAX) return UINT_MAX;
	return (unsigned int)dividend;
}

// 0x4F0320
// @0x4F0320
MoneyClass MoneyClass::Div(unsigned int divisor, MoneyClass *remainder) const	// NAME 2: describes what it does
{
	MoneyClass q;
	q.Amount = Amount / divisor;
	remainder->Amount = Amount % divisor;
	return q;
}

// 0x4F03A0
// @0x4F03A0
float MoneyClass::operator/(int count) const	// NAME 2: describes what it does
{
	return (float)Amount / *(int *)&MoneyClass((unsigned int)count);
}

// 0x4F03D0
// @0x4F03D0
float MoneyClass::operator/(const MoneyClass &money) const	// NAME 2: describes what it does
{
#line 60
	FAIL_IF(!money.Amount) return 0.0f;
	return (float)Amount / (float)money.Amount;
}

// 0x4F0440
// @0x4F0440
int MoneyClass::As_Int() const	// NAME 2: describes what it does
{
	if (Amount > 0x7FFFFFFF) {
		return 0x7FFFFFFF;
	}
	if (Amount < (-0x7FFFFFFF - 1)) {
		return (-0x7FFFFFFF - 1);
	}
	return (int)Amount;
}

// 0x4F0490
// @0x4F0490
unsigned int MoneyClass::As_Unsigned_Int() const	// NAME 2: describes what it does
{
	if (Amount > 0xFFFFFFFF) {
		return 0xFFFFFFFF;
	}
	return (unsigned int)Amount;
}

// 0x4F04C0
// @0x4F04C0
MoneyClass operator-(const MoneyClass &money)	// NAME 1 [money]: neutral
{
	return(MoneyClass(-money.Amount));
}

// 0x4F0500
// @0x4F0500
MoneyClass operator+(const MoneyClass &a, const MoneyClass &b)	// NAME 1 [a, b]: neutral
{
	return(MoneyClass(a.Amount + b.Amount));
}

// 0x4F0540
// @0x4F0540
MoneyClass operator-(const MoneyClass &a, const MoneyClass &b)	// NAME 1 [a, b]: neutral
{
	return(MoneyClass(a.Amount - b.Amount));
}

// 0x4F0580
// @0x4F0580
MoneyClass operator*(const MoneyClass &money, float scale)	// NAME 1 [money, scale]: neutral
{
	return(MoneyClass((__int64)(money.Amount * scale)));
}

// 0x4F05C0
// @0x4F05C0
MoneyClass operator*(float scale, const MoneyClass &money)	// NAME 1 [scale, money]: neutral
{
	return(MoneyClass((__int64)(money.Amount * scale)));
}

// 0x4F0600
// @0x4F0600
bool operator==(const MoneyClass &a, const MoneyClass &b)
{
	return(a.Amount == b.Amount);
}

// 0x4F0640
// @0x4F0640
bool operator!=(const MoneyClass &a, const MoneyClass &b)
{
	return(a.Amount != b.Amount);
}

// 0x4F0680
// @0x4F0680
bool operator<(const MoneyClass &a, const MoneyClass &b)
{
	return(a.Amount < b.Amount);
}

// 0x4F06C0
// @0x4F06C0
bool operator<=(const MoneyClass &a, const MoneyClass &b)
{
	return(a.Amount <= b.Amount);
}

// 0x4F0700
// @0x4F0700
bool operator>(const MoneyClass &a, const MoneyClass &b)
{
	return(a.Amount > b.Amount);
}

// 0x4F0740
// @0x4F0740
bool operator>=(const MoneyClass &a, const MoneyClass &b)
{
	return(a.Amount >= b.Amount);
}

// 0x4F0780 (the early return gives the client's out-of-line _Eos in the '-' append: one more inlined string destructor spends the budget)
// @0x4F0780
std::istream &operator>>(std::istream &is, MoneyClass &money)
{
	std::string str;
	char ch = 0;
	is >> ch;
	if (ch == '-') {
		str += '-';
		is >> ch;
	}
	if (!isdigit(ch)) {
		is.putback(ch);
		if (str.size()) {
			is.putback('-');
		}
		return is;
	}
	while (isdigit(ch)) {
		str += ch;
		ch = 0;
		is >> ch;
	}
	is.putback(ch);
	money = Unknown_4F0080(str);
	return is;
}

// 0x4F09E0
// @0x4F09E0
std::ostream &operator<<(std::ostream &os, const MoneyClass &money)
{
	os << money.Amount_As_Text(false).c_str();
	return os;
}

// 0x4F0AB0
// @0x4F0AB0
const char *Type_Name(MoneyClass *)	// NAME 2: describes what it does
{
	return "MoneyType";
}

// 0x4F0AD0
// @0x4F0AD0
bool String_To_Value(const char *text, MoneyClass *value)	// NAME 2: describes what it does
{
	*value = Unknown_4F0080(std::string(text));
	return true;
}

// 0x4F0BE0
// @0x4F0BE0
char *Value_To_String(char *buf, int size, MoneyClass value)	// NAME 2: describes what it does
{
	return Bounded_Strcpy(buf, size, value.Amount_As_Text(false).c_str());
}

/*
**	Reconstruction notes (money.cpp)
**
**	* Object boundaries: 0x4F0080 - 0x4F0CB0 (basic_istream<char>::putback). The next object file in link
**	  order is unique_design_id_lookup.cpp, beginning at 0x4F0E10 (basic_istream<char>::ipfx).
**	* 0x4F00C0 (Amount_As_Text): the plain branch comes first and returns a string built from the buffer
**	  again (a second strlen), not the local one; the commas branch calls 0x4FAEE0 and returns the local.
**	* 0x4F0320 (MoneyClass::Div) returns the quotient by value: the local's constructor (a store of 0 to its
**	  low half) stays before __alldiv, and the copy to the caller's object comes last. A reference return
**	  through a quotient parameter never keeps that store (17 forms, docs/data/attempts.csv).
**	* 0x4F0780 (operator>>): with the non-digit case returning early (its own string destructor) the
**	  '-' append calls _Eos out of line, as the client's; with one return at the end both appends inline it.
*/
