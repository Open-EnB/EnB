// Earth & Beyond client (reconstruction): the verbs' names (VerbEnum), rebuilt in unknown_verb.cpp
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	PROVISIONAL file name (D10): the object 0x84C450-0x84C5A2 lies between vendor_defs.cpp and
**	weapontype.cpp; no assert names it.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef UNKNOWN_VERB_H  // NAME 1 [unknown_verb.h]: provisional (D10)
#define UNKNOWN_VERB_H

// A verb (the client's Value_To_String 0x84C470 names it from its table 0xBB1218 as 'VerbEnum':
// the high 16 bits pick the verb, the low 16 bits are an argument)
enum VerbEnum  // NAME 4: the client's own label 'VerbEnum' (0x84C450's Type_Name)
{
	VERB_NONE = 0,			// NAME 3: the table's 'NA'
	VERB_SCAN = 0x10000,	// NAME 3: the table's 'Scan'
	VERB_LAND = 0x20000,	// NAME 3: the table's 'Land'
	VERB_LOOT = 0x30000,	// NAME 3: the table's 'Loot'
	VERB_GROUP = 0x40000,	// NAME 3: the table's 'Group'
	VERB_MESSAGE = 0x50000,	// NAME 3: the table's 'Message'
	VERB_TRADE = 0x60000,	// NAME 3: the table's 'Trade'
	VERB_TRACTOR = 0x70000,	// NAME 3: the table's 'Tractor'
	VERB_DOCK = 0x80000,	// NAME 3: the table's 'Dock'
	VERB_PROSPECT = 0x90000,	// NAME 3: the table's 'Prospect'
	VERB_GATE = 0xA0000,	// NAME 3: the table's 'Gate'
	VERB_REGISTER = 0xB0000,	// NAME 3: the table's 'Register'
	VERB_JUMPSTART = 0xC0000,	// NAME 3: the table's 'Jumpstart'
	VERB_FOLLOW = 0xD0000	// NAME 3: the table's 'Follow'
};

const char *Type_Name(VerbEnum *);								// 0x84C450
char *Value_To_String(char *buf, int size, VerbEnum verb);		// 0x84C470
bool String_To_Value(const char *text, VerbEnum *value);		// 0x84C560

#endif
