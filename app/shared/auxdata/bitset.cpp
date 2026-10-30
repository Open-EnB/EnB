// Earth & Beyond client (reconstruction): app\shared\auxdata\bitset.cpp. Notes at the end of the file.
#include "bitset.h"
#include "..\unknown_debug.h"
#include <string.h>

BitSetClass::BitSetClass(unsigned int size) : Buffer(NULL), NumBytes(0), NumBits(size)
{
	G_ASSERT(size > 0);
	NumBits = size;
	NumBytes = size / 8;
	if (size % 8) {
		NumBytes++;
	}

	Buffer = new char[NumBytes];

	// (spacing kept: the next assert is on the client's line 19)

	G_ASSERT(Buffer);
	memset(Buffer, 0, NumBytes);
}


BitSetClass::BitSetClass(const void *buffer, unsigned int buff_size) :
	Buffer(NULL), NumBytes(0), NumBits(0)
{

	FAIL_IF(buff_size < 1) buff_size = 1;
	G_ASSERT(buffer);

	NumBytes = buff_size;
	NumBits = buff_size * 8;

	Buffer = new char[NumBytes];

	G_ASSERT(Buffer);
	if (buffer) {
		memcpy(Buffer, buffer, NumBytes);
	} else {
		memset(Buffer, 0, NumBytes);
	}
}


BitSetClass::~BitSetClass()
{
	delete [] Buffer;
}


void BitSetClass::Swap(BitSetClass &other)
{
	char *b = Buffer;
	Buffer = other.Buffer;
	other.Buffer = b;
	unsigned int n = NumBytes;
	NumBytes = other.NumBytes;
	other.NumBytes = n;
	unsigned int m = NumBits;
	NumBits = other.NumBits;
	other.NumBits = m;
}


BitSetClass &BitSetClass::operator=(const BitSetClass &other)
{
	NumBytes = other.NumBytes;
	NumBits = other.NumBits;
	delete [] Buffer;
	Buffer = new char[NumBytes];
	memcpy(Buffer, other.Buffer, NumBytes);
	return *this;
}


bool BitSetClass::operator==(const BitSetClass &other) const
{
	if (NumBits != other.NumBits || NumBytes != other.NumBytes) {
		return false;
	}
	if (Buffer == other.Buffer) {
		return true;
	}
	if (Buffer == NULL || other.Buffer == NULL) {
		return false;
	}
	return memcmp(Buffer, other.Buffer, NumBytes) == 0;
}

bool BitSetClass::Is_Set(unsigned int pos) const
{
	G_ASSERT(Buffer && pos >= 0 && pos < NumBits);

	return (Buffer[pos >> 3] & (1 << (pos & 7))) != 0;
}


void BitSetClass::Set(unsigned int pos)
{
	G_ASSERT(Buffer && pos >= 0 && pos < NumBits);
	Buffer[pos >> 3] |= (1 << (pos & 7));
}


void BitSetClass::Set_All()
{
	memset(Buffer, 0xFF, NumBytes);
}


void BitSetClass::Clear(unsigned int pos)
{
	G_ASSERT(pos >= 0 && pos < NumBits);
	Buffer[pos >> 3] &= ~(1 << (pos & 7));
}


void BitSetClass::Clear_All()
{
	memset(Buffer, 0, NumBytes);
}


bool BitSetClass::Is_Any_Set() const
{
	for (unsigned int i = 0; i < NumBytes; i++) {
		if (Buffer[i] != 0) {
			return true;
		}
	}
	return false;
}

/*
**	Notes (here, below the code, because the asserts pin the lines above: D25)
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\shared\auxdata\bitset.cpp (its __FILE__ string).
**
**	Client functions 0x4A47B0-0x4A4C4B, all byte-exact. The asserts are on the client's __LINE__
**	values (8, 19, 28, 29, 36, 92, 100, 113); the blank lines and comments between functions are
**	placed to keep them there. Class and member names: bitset.h.
*/
