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

#ifndef __MONEY_H
#define __MONEY_H

class MoneyClass
{
public:
	__int64 m_Unknown00;		// +0x00 read and written as 8 bytes
};

#endif //__MONEY_H
