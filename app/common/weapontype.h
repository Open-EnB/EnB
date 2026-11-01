// Earth & Beyond client (reconstruction): app\common\weapontype.h
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Common\weapontype.h (name assumed from weapontype.cpp;
**	no assert names it)
**
**	A weapon's type and its text converters (weapontype.cpp 0x84C5D0-0x84C69D; nothing in the
**	client calls them).
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef WEAPONTYPE_H  // NAME 3 [weapontype.h]: header name assumed from the client's weapontype.cpp
#define WEAPONTYPE_H

// Its names (0xBB1358): Beam, Projectile, Missile, Area Effect.
enum WeaponType  // NAME 2: the client's file name weapontype.cpp
{
	WEAPON_TYPE_FIRST = 0,	// the client's FAIL_IF 'a < WEAPON_TYPE_FIRST'
	MAX_TYPE_WEAPON = 4		// the client's FAIL_IF 'a >= MAX_TYPE_WEAPON'
};

bool String_To_Value(const char *text, WeaponType *value);		// 0x84C5D0 (asserts on an unknown name)
char *Value_To_String(char *buf, int size, WeaponType a);		// 0x84C640

#endif
