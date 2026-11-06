// Earth & Beyond client (reconstruction)
/*
**	Original location: unknown (no __FILE__ or assert in its object; D10 neutral name).
**	PLACEMENT: provisional (D137): a string, assert or link map naming the object 0x67D430-0x67DC6F.
**
**	A ring of the last chat messages with a colour for each message type (0x70 bytes).
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __UNKNOWN_67D430_H  // NAME 1: neutral (D10), file name unknown
#define __UNKNOWN_67D430_H

#include <string>

struct Unknown67D430EntryStruct  // NAME 1: neutral (D10): 0x14 bytes, a message
{
	int Get_Type() const { return Type; }  // NAME 2: reads Type
	int Type;  // +0x00  // NAME 2: 0x67DB40 switches on it
	std::string Text;  // +0x04  // NAME 2: 0x67D780's second argument
};

class Unknown67D430Class  // NAME 1: neutral (D10): 0x70 bytes
{
public:
	Unknown67D430Class(unsigned int capacity);
	~Unknown67D430Class();

	void Unknown_67D660(int level);  // NAME 1 [Unknown_67D660, level]: neutral (D10): each type's colour from the level count
	Unknown67D430EntryStruct &Unknown_67D780(int type, const std::string &text);  // NAME 1 [Unknown_67D780, type, text]: neutral (D10): adds a message
	const Unknown67D430EntryStruct &Unknown_67D980(unsigned int index, int *color);  // NAME 1 [Unknown_67D980, index, color]: neutral (D10): oldest first
	const Unknown67D430EntryStruct &Unknown_67DA60(unsigned int index, int *color);  // NAME 1 [Unknown_67DA60, index, color]: neutral (D10): newest first
	int Unknown_67DB40(int type);  // NAME 1 [Unknown_67DB40, type]: neutral (D10): the type's colour, -1 when its option is off

	int Level;  // +0x00  // NAME 2: 0x67D660's argument
	unsigned int Capacity;  // +0x04  // NAME 2: the ring's size
	unsigned int Count;  // +0x08  // NAME 2: messages added
	Unknown67D430EntryStruct *Entries;  // +0x0C  // NAME 2: the ring
	int Colors[24];  // +0x10  // NAME 2: per type, what 0x67DB40 returns
};

#endif
