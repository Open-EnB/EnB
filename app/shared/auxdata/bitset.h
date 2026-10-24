/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\shared\auxdata\bitset.h (name assumed from bitset.cpp)
**
**	A fixed-size array of bits. The client names neither the class nor the header: BitSetClass is
**	a Westwood-style name (D24); Buffer and NumBits come from bitset.cpp's asserts.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __BITSET_H
#define __BITSET_H

class BitSetClass
{
public:
	BitSetClass(unsigned int size);										// 0x4A47B0, all bits clear
	BitSetClass(const void *buffer, unsigned int buff_size);		// 0x4A4870, a copy of buff_size bytes
	~BitSetClass();															// 0x4A4960

	void Swap(BitSetClass &other);										// 0x4A4980
	BitSetClass &operator=(const BitSetClass &other);				// 0x4A49C0
	bool operator==(const BitSetClass &other) const;				// 0x4A4A30

	bool Is_Set(unsigned int pos) const;								// 0x4A4AA0
	void Set(unsigned int pos);											// 0x4A4B10
	void Set_All();															// 0x4A4B80
	void Clear(unsigned int pos);											// 0x4A4BB0
	void Clear_All();															// 0x4A4C00
	bool Is_Any_Set() const;												// 0x4A4C30

	char *Buffer;																// +0x0 (assert name)
	unsigned int NumBytes;													// +0x4: the name is a guess (no assert names it)
	unsigned int NumBits;													// +0x8 (assert name)
};

#endif //__BITSET_H
