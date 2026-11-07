// Earth & Beyond client (reconstruction): app\shared\money.cpp (the client's __FILE__
// 'e:\build\beta\G\app\shared\money.cpp', asserts from 0x4F0220).
#include <limits.h>
#include <iostream>
#include <string>
#include "g_assert.h"
#define __MONEY_MEMBERS
#include "money.h"

char *Unknown_4A1590(char *buffer, int size, const char *string);	// 0x4A1590: a bounded strcpy  // NAME 1 [Unknown_4A1590]: neutral placeholder (D10)
__int64 Unknown_8B44C0(const char *text);	// 0x8B44C0: _atoi64(text)

// Helper 0x4F0080
// @0x4F0080
MoneyClass Unknown_4F0080(const std::string &str)	// NAME 1: neutral
{
	return MoneyClass(Unknown_8B44C0(str.c_str()));
}

// 0x4F0220
// @0x4F0220
unsigned int MoneyClass::Divide(float price_per_unit) const	// NAME 2: describes what it does
{
#line 29
	FAIL_IF(price_per_unit <= 0.0f) return 0;
	__int64 dividend = (__int64)((double)Unknown00 / price_per_unit);
#line 32
	FAIL_IF(dividend > UINT_MAX) return UINT_MAX;
	return (unsigned int)dividend;
}

// 0x4F02A0
// @0x4F02A0
unsigned int MoneyClass::Divide(const MoneyClass &price_per_unit) const	// NAME 2: describes what it does
{
#line 38
	FAIL_IF(price_per_unit.Unknown00 <= 0) return 0;
	__int64 dividend = Unknown00 / price_per_unit.Unknown00;
#line 41
	FAIL_IF(dividend > UINT_MAX) return UINT_MAX;
	return (unsigned int)dividend;
}

// 0x4F0320
// @0x4F0320
// NOT EXACT: shape: 1 masked rows differ; no simpler class fits
MoneyClass &MoneyClass::Div(MoneyClass *quotient, unsigned int divisor, MoneyClass *remainder) const	// NAME 2: describes what it does
{
	MoneyClass q;
	q.Unknown00 = 0;
	q.Unknown00 = Unknown00 / divisor;
	remainder->Unknown00 = Unknown00 % divisor;
	*quotient = q;
	return *quotient;
}

// 0x4F03A0
// @0x4F03A0
float MoneyClass::operator/(int count) const	// NAME 2: describes what it does
{
	return (float)Unknown00 / *(int *)&MoneyClass((unsigned int)count);
}

// 0x4F03D0
// @0x4F03D0
float MoneyClass::operator/(const MoneyClass &money) const	// NAME 2: describes what it does
{
#line 60
	FAIL_IF(!money.Unknown00) return 0.0f;
	return (float)Unknown00 / (float)money.Unknown00;
}

// 0x4F0440
// @0x4F0440
int MoneyClass::As_Int() const	// NAME 2: describes what it does
{
	if (Unknown00 > 0x7FFFFFFF) {
		return 0x7FFFFFFF;
	}
	if (Unknown00 < (-0x7FFFFFFF - 1)) {
		return (-0x7FFFFFFF - 1);
	}
	return (int)Unknown00;
}

// 0x4F0490
// @0x4F0490
unsigned int MoneyClass::As_Unsigned_Int() const	// NAME 2: describes what it does
{
	if (Unknown00 > 0xFFFFFFFF) {
		return 0xFFFFFFFF;
	}
	return (unsigned int)Unknown00;
}

// 0x4F04C0
// @0x4F04C0
MoneyClass operator-(const MoneyClass &money)	// NAME 1 [money]: neutral
{
	return(MoneyClass(-money.Unknown00));
}

// 0x4F0500
// @0x4F0500
MoneyClass operator+(const MoneyClass &a, const MoneyClass &b)	// NAME 1 [a, b]: neutral
{
	return(MoneyClass(a.Unknown00 + b.Unknown00));
}

// 0x4F0540
// @0x4F0540
MoneyClass operator-(const MoneyClass &a, const MoneyClass &b)	// NAME 1 [a, b]: neutral
{
	return(MoneyClass(a.Unknown00 - b.Unknown00));
}

// 0x4F0580
// @0x4F0580
MoneyClass operator*(const MoneyClass &money, float scale)	// NAME 1 [money, scale]: neutral
{
	return(MoneyClass((__int64)(money.Unknown00 * scale)));
}

// 0x4F05C0
// @0x4F05C0
MoneyClass operator*(float scale, const MoneyClass &money)	// NAME 1 [scale, money]: neutral
{
	return(MoneyClass((__int64)(money.Unknown00 * scale)));
}

// 0x4F0600
// @0x4F0600
bool operator==(const MoneyClass &a, const MoneyClass &b)
{
	return(a.Unknown00 == b.Unknown00);
}

// 0x4F0640
// @0x4F0640
bool operator!=(const MoneyClass &a, const MoneyClass &b)
{
	return(a.Unknown00 != b.Unknown00);
}

// 0x4F0680
// @0x4F0680
bool operator<(const MoneyClass &a, const MoneyClass &b)
{
	return(a.Unknown00 < b.Unknown00);
}

// 0x4F06C0
// @0x4F06C0
bool operator<=(const MoneyClass &a, const MoneyClass &b)
{
	return(a.Unknown00 <= b.Unknown00);
}

// 0x4F0700
// @0x4F0700
bool operator>(const MoneyClass &a, const MoneyClass &b)
{
	return(a.Unknown00 > b.Unknown00);
}

// 0x4F0740
// @0x4F0740
bool operator>=(const MoneyClass &a, const MoneyClass &b)
{
	return(a.Unknown00 >= b.Unknown00);
}

// 0x4F0780
// @0x4F0780
// NOT EXACT: inline-budget: 16 calls in ours, 17 in the client; 5 callees only in the client: ?_Eos@?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@AAEXI@Z
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
	} else {
		while (isdigit(ch)) {
			str += ch;
			ch = 0;
			is >> ch;
		}
		is.putback(ch);
		money = Unknown_4F0080(str);
	}
	return is;
}

// 0x4F09E0
// @0x4F09E0
std::ostream &operator<<(std::ostream &os, const MoneyClass &money)
{
	os << money.Unknown_4F00C0(false).c_str();
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
	return Unknown_4A1590(buf, size, value.Unknown_4F00C0(false).c_str());
}

/*
**	Reconstruction notes (money.cpp)
**
**	* Object boundaries: 0x4F0080 - 0x4F0CB0 (basic_istream<char>::putback). The next object file in link
**	  order is unique_design_id_lookup.cpp, beginning at 0x4F0E10 (basic_istream<char>::ipfx).
**	* 0x4F0320 (MoneyClass::Div) matches with 0.99 similarity (1 masked row difference: client has an
**	  uneliminated dead store to stack before __alldiv call).
**	* 0x4F0780 (operator>>) matches all structure and length (480 bytes); differs only in standard library
**	  string inlining budget for _Eos.
*/
