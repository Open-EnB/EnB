// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\common\circbuff.h (name assumed from circbuff.cpp)
**
**	A ring buffer of bytes (circbuff.cpp 0x830BF0-0x831277): the stream connection's 100,000-byte
**	receive buffer (connect.cpp). Members from the asserts (BytesUsed, BufferSize) and the
**	constructor 0x830BF0's order; 0x18 bytes.
*/



#if defined(_MSC_VER)
#pragma once
#endif

#ifndef CIRCBUFF_H  // NAME 3 [circbuff.h]: header name assumed from the client's circbuff.cpp
#define CIRCBUFF_H

class CircBuffClass  // NAME 3: by convention (D24) from the client's file circbuff.cpp
{
public:
	CircBuffClass(int size, void *buffer = 0);	// 0x830BF0 (no buffer: its own, new'd)
	~CircBuffClass();							// 0x830C60

	int Read(void *buffer, int len);			// 0x830C80: at most BytesUsed; returns the bytes read  // NAME 2: what it does
	int Write(const void *buffer, int len);		// 0x830DA0: all or nothing  // NAME 2: what it does
	int Get_Free() const;						// 0x830EB0: BufferSize - BytesUsed  // NAME 2: what it returns
	int Get_Used() const;						// 0x830ED0: BytesUsed  // NAME 2: what it returns
	bool Unread(int len);						// 0x830EF0: the last len bytes read are back in front  // NAME 2: what it does
	int Get_Contiguous_Used() const;			// 0x830F60: the used bytes from ReadPtr before the wrap (no caller)  // NAME 2: what it returns
	int Get_Contiguous_Free() const;			// 0x830F90: the free bytes from WritePtr before the wrap (no caller)  // NAME 2: what it returns
	char *Peek(int offset, int *len);			// 0x830FD0: the byte offset bytes past ReadPtr and the contiguous run there (no caller)  // NAME 2: what it does
	bool Is_Valid() const;						// 0x831060: the pointers agree with BytesUsed (no caller)  // NAME 2: what it checks
	void Dump(const char *title, int row);		// 0x8310D0: the state and contents to the console at row (no caller)  // NAME 2: what it does						// 0x830EF0: the last len bytes read are back in front  // NAME 2: what it does

	char *Buffer;					// +0x00  // NAME 2: what it holds
	int BufferSize;					// +0x04
	int BytesUsed;					// +0x08
	char *ReadPtr;					// +0x0C the next byte to read  // NAME 2: what it holds
	char *WritePtr;					// +0x10 the next byte to write  // NAME 2: what it holds
	unsigned IsExternal : 1;		// +0x14 bit 0: the buffer was given, not deleted  // NAME 2: what it means
};

#endif
