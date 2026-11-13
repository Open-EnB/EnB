// Earth & Beyond client (reconstruction): app\common\manufacturing_defs.h
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Common\manufacturing_defs.h (name assumed from
**	manufacturing_defs.cpp; no assert names it)
**
**	The manufacturing lab's value types and their text converters (manufacturing_defs.cpp,
**	0x845200-0x846253): the item types, categories and subcategories (the client's MfgTypeStruct,
**	MfgCategoryStruct, MfgSubCategoryStruct tables), the lab's mode and its bit flags, an attempt's
**	validity and a formula's difficulty. aux_roots.h includes this header for the ManufacturingLab
**	root's fields; the four value types keep the tags aux_roots.h gave them from their leaf vtables
**	(the client names none of them).
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef MANUFACTURING_DEFS_H  // NAME 3 [manufacturing_defs.h]: header name assumed from the client's manufacturing_defs.cpp
#define MANUFACTURING_DEFS_H

// NAME 1: the value type of the leaf vtable 0xB0122C (its KVPair's Get_Type_Name 0x708690 says 'int'; no name in Net-7 or C#): a subcategory (_SubCategoryDef 0xBAEDD0)
enum UnknownB0122CEnum { UNKNOWN_B0122C_0 };	// vtable 0xB0122C, 4 bytes
// NAME 1: the value type of the leaf vtable 0xB0133C (its KVPair's Get_Type_Name 0x708AE0 says 'int'; no name in Net-7 or C#): a formula's difficulty (names 0xBAEF8C)
enum UnknownB0133CEnum { UNKNOWN_B0133C_0 };	// vtable 0xB0133C, 4 bytes
// NAME 1: the value type of the leaf vtable 0xB01394 (its KVPair's Get_Type_Name 0x708970 says 'int'; no name in Net-7 or C#): an attempt's validity (names 0xBAEF44)
enum UnknownB01394Enum { UNKNOWN_B01394_0 };	// vtable 0xB01394, 4 bytes
// NAME 1: the value type of the leaf vtable 0xB013EC (its KVPair's Get_Type_Name 0x708800 says 'int'; no name in Net-7 or C#): the lab's mode (names 0xBAEF30)
enum UnknownB013ECEnum { UNKNOWN_B013EC_0 };	// vtable 0xB013EC, 4 bytes

bool String_To_Value(const char *text, UnknownB0122CEnum *value);				// 0x8456C0
char *Value_To_String(char *buf, int size, UnknownB0122CEnum value);			// 0x845780
bool String_To_Value(const char *text, UnknownB0133CEnum *value);				// 0x846110
char *Value_To_String(char *buf, int size, UnknownB0133CEnum value);			// 0x846160
bool String_To_Value(const char *text, UnknownB01394Enum *value);				// 0x846040
char *Value_To_String(char *buf, int size, UnknownB01394Enum value);			// 0x846090
bool String_To_Value(const char *text, UnknownB013ECEnum *value);				// 0x8458C0
char *Value_To_String(char *buf, int size, UnknownB013ECEnum value);			// 0x845910

#ifdef __MANUFACTURING_DEFS_ALL	// manufacturing_defs.cpp only: aux_roots.cpp includes this header, and its declaration count moves the SYMBOL_PAD windows (workflow.md 3.4), so other files do not see these

// The lab's modes (the client's FAIL_IF 'mode < MODE_NONE || mode > MODE_DISMANTLE'; names 'None',
// 'Manufacture', 'Analyze', 'Dismantle', 'Refining').
enum
{
	MODE_NONE = 0,
	MODE_MANUFACTURE,		// NAME 2: the name table
	MODE_ANALYZE,			// NAME 2: the name table
	MODE_DISMANTLE,
	MODE_REFINE,			// NAME 2: the name table
	MODE_COUNT				// String_To_Value's bound (it reads one past the table)  // NAME 1: by analogy with MANUFACTURING_VALIDITY_COUNT
};

// An attempt's validity (the client's FAIL_IF; 'NoTarget' ... 'AttemptCriticalSuccess'; it lets 18 through).
enum { MANUFACTURING_VALIDITY_START = 0, MANUFACTURING_VALIDITY_COUNT = 18 };

// A formula's difficulty (the client's FAIL_IF; 'Invalid', 'Automatic' ... 'Impossible').
enum { MANUFACTURING_DIFFICULTY_INVALID = 0, NUM_MANUFACTURING_DIFFICULTY = 7 };

// An item's manufacturing type (the client's _TypeDef 0xBAED48: None -1, Items 0, Components 1).
enum MfgTypeEnum { MFG_TYPE_NONE = -1 };	// NAME 3: the client's MfgTypeStruct / _TypeDef
// An item's manufacturing category (_CategoryDef 0xBAED68: None -1, Weapons 10 ... Trade Goods 90).
enum MfgCategoryEnum { MFG_CATEGORY_NONE = -1 };	// NAME 3: the client's MfgCategoryStruct / _CategoryDef
// The lab's modes as bit flags (Mode_To_Bit_Flag 0x845990, Bit_Flag_To_Mode 0x845A10).
enum ManufacturingModeBitFlag { MODE_FLAG_NONE = 0 };	// NAME 2: by analogy with the client's GameActionValidityBitFlag

bool String_To_Value(const char *buf, MfgTypeEnum *value);						// 0x8452C0
char *Value_To_String(char *buf, int size, MfgTypeEnum value);					// 0x845380
bool String_To_Value(const char *buf, MfgCategoryEnum *value);					// 0x8454C0
char *Value_To_String(char *buf, int size, MfgCategoryEnum value);				// 0x845580
ManufacturingModeBitFlag Mode_To_Bit_Flag(UnknownB013ECEnum mode);				// 0x845990  // NAME 2: what it does
UnknownB013ECEnum Bit_Flag_To_Mode(ManufacturingModeBitFlag flag);				// 0x845A10  // NAME 2: what it does
bool String_To_Value(const char *text, ManufacturingModeBitFlag *value);		// 0x845AA0
char *Value_To_String(char *buf, int size, ManufacturingModeBitFlag value);		// 0x845D40
bool Is_Validity_Failure(UnknownB01394Enum validity);							// 0x8461E0 (0..12)  // NAME 2: what it does
bool Is_Validity_Attempt(UnknownB01394Enum validity);							// 0x846210 (14..17: an attempt's result; ui_manu.cpp and ui_refinery.cpp declare it locally)  // NAME 2: what it does
bool Is_Validity_Success(UnknownB01394Enum validity);							// 0x846240 (16..17)  // NAME 2: what it does

#endif

#endif
