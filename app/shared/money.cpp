// Earth & Beyond client (reconstruction): app\shared\money.cpp (the client's __FILE__
// 'e:\build\beta\G\app\shared\money.cpp', asserts from 0x4F0220). So far only the free operators
// 0x4F04C0-0x4F0740 (TG3); the file's other functions (0x4F0080-0x4F0490, the asserts' lines 29-60,
// and 0x4F0780 on) are not rebuilt yet. Notes at the end of the file.
#include "money.h"

// 0x4F04C0
MoneyClass operator-(const MoneyClass &money)	// NAME 1 [money]: neutral
{
	return(MoneyClass(-money.Unknown00));
}

// 0x4F0500
MoneyClass operator+(const MoneyClass &a, const MoneyClass &b)	// NAME 1 [a, b]: neutral
{
	return(MoneyClass(a.Unknown00 + b.Unknown00));
}

// 0x4F0540
MoneyClass operator-(const MoneyClass &a, const MoneyClass &b)	// NAME 1 [a, b]: neutral
{
	return(MoneyClass(a.Unknown00 - b.Unknown00));
}

// 0x4F0580
MoneyClass operator*(const MoneyClass &money, float scale)	// NAME 1 [money, scale]: neutral
{
	return(MoneyClass((__int64)(money.Unknown00 * scale)));
}

// 0x4F05C0
MoneyClass operator*(float scale, const MoneyClass &money)	// NAME 1 [scale, money]: neutral
{
	return(MoneyClass((__int64)(money.Unknown00 * scale)));
}

// 0x4F0600
bool operator==(const MoneyClass &a, const MoneyClass &b)
{
	return(a.Unknown00 == b.Unknown00);
}

// 0x4F0640
bool operator!=(const MoneyClass &a, const MoneyClass &b)
{
	return(a.Unknown00 != b.Unknown00);
}

// 0x4F0680
bool operator<(const MoneyClass &a, const MoneyClass &b)
{
	return(a.Unknown00 < b.Unknown00);
}

// 0x4F06C0
bool operator<=(const MoneyClass &a, const MoneyClass &b)
{
	return(a.Unknown00 <= b.Unknown00);
}

// 0x4F0700
bool operator>(const MoneyClass &a, const MoneyClass &b)
{
	return(a.Unknown00 > b.Unknown00);
}

// 0x4F0740
bool operator>=(const MoneyClass &a, const MoneyClass &b)
{
	return(a.Unknown00 >= b.Unknown00);
}

/*
**	Reconstruction notes (money.cpp)
**
**	* The free operators 0x4F04C0-0x4F0740, all exact (TG3): their declarations stay in the files
**	  that call them (delta_money.cpp, ui_avatar_cust.cpp, ui_ptrade.cpp, ui_ship_cust.cpp,
**	  inv_render.cpp's Unknown_4F0640), money.h is left as delta_money.cpp needs it.
**	* The by-value results keep the client's construction flag (`mov [esp], 0`): MoneyClass's
**	  destructor is user-declared.
*/
