// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\shared\auxdata\bitset.h (name assumed from bitset.cpp)
**
**	A fixed-size array of bits. The client names neither the class nor the header: BitSet is
**	a Westwood-style name (D24); Buffer and NumBits come from bitset.cpp's asserts.
*/



#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __BITSET_H  // NAME 3 [bitset.h]: header name assumed from the client's bitset.cpp
#define __BITSET_H

class BitSet  // NAME 5: thunk order A, binary.md 5.11
{
public:
	BitSet(unsigned int size);										// 0x4A47B0, all bits clear
	BitSet(const void *buffer, unsigned int buff_size);		// 0x4A4870, a copy of buff_size bytes
	~BitSet();															// 0x4A4960

	void Swap(BitSet &other);										// 0x4A4980  // NAME 2: describes what it does
	BitSet &operator=(const BitSet &other);				// 0x4A49C0
	bool operator==(const BitSet &other);				// 0x4A4A30  // NAME 5: thunk order probable, binary.md 5.11

	bool Is_Set(unsigned int pos) const;								// 0x4A4AA0  // NAME 2: describes what it does
	bool operator[](unsigned int pos) const { return Is_Set(pos); }	// encode.cpp's assert on line 660 (b[1])
	void Set(unsigned int pos);											// 0x4A4B10  // NAME 2: describes what it does
	void Set_All() const;															// 0x4A4B80  // NAME 5: thunk order confirmed, binary.md 5.11
	void Clear(unsigned int pos);											// 0x4A4BB0  // NAME 2: describes what it does
	void Clear_All();															// 0x4A4C00  // NAME 2: describes what it does
	bool Is_Any_Set() const;												// 0x4A4C30  // NAME 2: describes what it does
	bool Is_None_Set() const;												// 0x4A4C60 (no caller in the client)  // NAME 1: Is_Any_Set's inverse
	char *Buffer;																// +0x0 (assert name)
	unsigned int NumBytes;													// +0x4: the name is a guess (no assert names it)  // NAME 2: after the asserts' NumBits
	unsigned int NumBits;													// +0x8 (assert name)
};

#endif //__BITSET_H
