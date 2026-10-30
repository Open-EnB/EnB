// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\shared\auxdata\linearbu.h (name assumed from linearbu.cpp)
**
**	The buffer the aux-data fields read from and write to: Size bytes of data in a Capacity-byte
**	Buffer, read and written at Pos, growing by GrowBy. Members from linearbu.cpp's asserts (Buffer,
**	Capacity) and its parameters (size, pos). The client doesn't name the class (D24: "linear
**	buffer", after the file name).
*/



#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __LINEARBU_H  // NAME 3 [linearbu.h]: header name assumed from the client's linearbu.cpp
#define __LINEARBU_H

class LinearBufferClass  // NAME 3: by convention (D24) from the client's file linearbu.cpp ("linear buffer", D37)
{
public:
	LinearBufferClass(int capacity, int grow_by);								// 0x4AAB60
	LinearBufferClass(const void *data, int size, int grow_by, bool copy);	// 0x4AABE0 (copy: own a copy)
	~LinearBufferClass();																// 0x4AACB0

	int Read(void *buf, int len);						// 0x4AACE0, returns the bytes read  // NAME 2: describes what it does
	int Read(int pos, void *buf, int len);			// 0x4AADA0
	char *Read_String();									// 0x4AAE90 (new[]'d, 0-terminated)  // NAME 2: describes what it does
	int Write(const void *buf, int len);			// 0x4AAFC0, returns the bytes written  // NAME 2: describes what it does
	int Write(int pos, const void *buf, int len);	// 0x4AB0D0
	int Write_String(const char *string);			// 0x4AB140 (with its 0)  // NAME 2: describes what it does
	int Append(const void *buf, int len);			// 0x4AB180 (at Size)  // NAME 2: describes what it does
	int Seek_Relative(int offset);					// 0x4AB1F0 (-1 below 0)  // NAME 2: describes what it does
	int Truncate(int len);								// 0x4AB230 (drops len bytes from the end)  // NAME 2: describes what it does
	int Get_Pos() const;									// 0x4AB280  // NAME 2: describes what it does
	int Seek(int pos);									// 0x4AB2A0  // NAME 2: describes what it does
	int Skip(int offset);								// 0x4AB2E0 (Pos unchanged below 0)  // NAME 2: describes what it does
	int Grow(int len);									// 0x4AB330 (by GrowBy steps to at least len)  // NAME 2: describes what it does
	int Get_Size() const;								// 0x4AB3A0  // NAME 2: describes what it does
	int Set_Size(int size);								// 0x4AB3C0  // NAME 2: describes what it does
	int Get_Capacity() const;							// 0x4AB410  // NAME 2: describes what it does
	int Unknown_Set_Size(int size);					// 0x4AB430 (Capacity and Size; Pos below them)
	int Set_Capacity(int capacity);					// 0x4AB460 (reallocates)  // NAME 2: describes what it does

	char *Buffer;					// +0x00  // NAME 2: describes what it does
	int m_Unknown04;				// +0x04 not set by the constructors
	int Size;						// +0x08 the bytes of data  // NAME 3: after the assert's parameter size (linearbu.cpp)
	int Capacity;					// +0x0C the bytes allocated  // NAME 3: after the assert's parameter capacity (linearbu.cpp)
	int GrowBy;						// +0x10  // NAME 2: describes what it does
	int Pos;							// +0x14  // NAME 3: after the assert's parameter pos (linearbu.cpp)
	bool OwnsBuffer;				// +0x18 Buffer is deleted with the buffer  // NAME 2: describes what it does
};

#endif //__LINEARBU_H
