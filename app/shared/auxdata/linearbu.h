/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
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

#ifndef __LINEARBU_H
#define __LINEARBU_H

class LinearBufferClass
{
public:
	LinearBufferClass(int capacity, int grow_by);								// 0x4AAB60
	LinearBufferClass(const void *data, int size, int grow_by, bool copy);	// 0x4AABE0 (copy: own a copy)
	~LinearBufferClass();																// 0x4AACB0

	int Read(void *buf, int len);						// 0x4AACE0, returns the bytes read
	int Read(int pos, void *buf, int len);			// 0x4AADA0
	char *Read_String();									// 0x4AAE90 (new[]'d, 0-terminated)
	int Write(const void *buf, int len);			// 0x4AAFC0, returns the bytes written
	int Write(int pos, const void *buf, int len);	// 0x4AB0D0
	int Write_String(const char *string);			// 0x4AB140 (with its 0)
	int Append(const void *buf, int len);			// 0x4AB180 (at Size)
	int Seek_Relative(int offset);					// 0x4AB1F0 (-1 below 0)
	int Truncate(int len);								// 0x4AB230 (drops len bytes from the end)
	int Get_Pos() const;									// 0x4AB280
	int Seek(int pos);									// 0x4AB2A0
	int Skip(int offset);								// 0x4AB2E0 (Pos unchanged below 0)
	int Grow(int len);									// 0x4AB330 (by GrowBy steps to at least len)
	int Get_Size() const;								// 0x4AB3A0
	int Set_Size(int size);								// 0x4AB3C0
	int Get_Capacity() const;							// 0x4AB410
	int Unknown_Set_Size(int size);					// 0x4AB430 (Capacity and Size; Pos below them)
	int Set_Capacity(int capacity);					// 0x4AB460 (reallocates)

	char *Buffer;					// +0x00
	int m_Unknown04;				// +0x04 not set by the constructors
	int Size;						// +0x08 the bytes of data
	int Capacity;					// +0x0C the bytes allocated
	int GrowBy;						// +0x10
	int Pos;							// +0x14
	bool OwnsBuffer;				// +0x18 Buffer is deleted with the buffer
};

#endif //__LINEARBU_H
