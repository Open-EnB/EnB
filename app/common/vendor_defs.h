// Earth & Beyond client (reconstruction): app\common\vendor_defs.h
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Common\vendor_defs.h (name assumed from vendor_defs.cpp;
**	no assert names it)
**
**	A vendor's and an ambient character's type and their text converters (the key/value pairs'
**	Type_Name / String_To_Value / Value_To_String, vendor_defs.cpp 0x84C270-0x84C42C; nothing in
**	the client calls them).
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef VENDOR_DEFS_H  // NAME 3 [vendor_defs.h]: header name assumed from the client's vendor_defs.cpp
#define VENDOR_DEFS_H

// The client's Type_Name 'VendorTypeEnum'; its names (0xBB10D8) are Weapon, System, Core, Consumable,
// Junk, Component, Resource, BlackMarket.
enum VendorTypeEnum
{
	VENDOR_TYPE_INVALID = -1,	// the client's FAIL_IF 'r <= VENDOR_TYPE_INVALID'
	VENDOR_TYPE_NUM = 8			// the client's FAIL_IF 'r >= VENDOR_TYPE_NUM'
};

// The client's Type_Name 'AmbientTypeEnum'; its names (0xBB10F8) are None, Npc, Vendor, Bartender.
enum AmbientTypeEnum
{
	AMBIENT_TYPE_INVALID = -1,	// the client's FAIL_IF 'r <= AMBIENT_TYPE_INVALID'
	AMBIENT_TYPE_NUM = 4		// the client's FAIL_IF 'r >= AMBIENT_TYPE_NUM'
};

const char *Type_Name(VendorTypeEnum *);								// 0x84C270
bool String_To_Value(const char *text, VendorTypeEnum *value);		// 0x84C290
char *Value_To_String(char *buf, int size, VendorTypeEnum r);		// 0x84C2E0
const char *Type_Name(AmbientTypeEnum *);								// 0x84C360
bool String_To_Value(const char *text, AmbientTypeEnum *value);		// 0x84C380
char *Value_To_String(char *buf, int size, AmbientTypeEnum r);		// 0x84C3D0

#endif
