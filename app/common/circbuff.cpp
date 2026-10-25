// Earth & Beyond client (reconstruction): app\common\circbuff.cpp. Notes at the end of the file.
#include "circbuff.h"
#include "..\shared\unknown_debug.h"
#include <string.h>


CircBuffClass::CircBuffClass(int size, void *buffer)
{
	Buffer = (char *)buffer;
	BufferSize = size;
	BytesUsed = 0;
	ReadPtr = 0;
	WritePtr = 0;
	IsExternal = true;
	if (buffer == 0) {
		IsExternal = false;
		Buffer = new char[size];
	}
	ReadPtr = Buffer;
	WritePtr = Buffer;
}


CircBuffClass::~CircBuffClass()
{
	if (!IsExternal) {
		delete Buffer;
	}
}

// (spacing kept: the next assert is on the client's line 112)































































int CircBuffClass::Read(void *buffer, int len)
{
	if (len > BytesUsed) {
		len = BytesUsed;
	}
	if (len == 0) {
		return(0);
	}
	int contiguous;
	if (BytesUsed == 0) {
		contiguous = 0;
	} else if (WritePtr <= ReadPtr) {
		contiguous = BufferSize - (ReadPtr - Buffer);
	} else {
		contiguous = BytesUsed;
	}
	BytesUsed -= len;
	G_ASSERT(BytesUsed >= 0);
	int rest = len - contiguous;
	if (rest <= 0) {
		memcpy(buffer, ReadPtr, len);
		ReadPtr += len;
		return(len);
	}
	memcpy(buffer, ReadPtr, contiguous);
	memcpy((char *)buffer + contiguous, Buffer, rest);
	ReadPtr = Buffer + rest;
	return(len);
}

// (spacing kept: the next assert is on the client's line 166)

























int CircBuffClass::Write(const void *buffer, int len)
{
	int space = BufferSize - BytesUsed;
	if (len > space) {
		return(space);
	}
	int contiguous;
	if (space == 0) {
		contiguous = 0;
	} else if (WritePtr >= ReadPtr) {
		contiguous = BufferSize - (WritePtr - Buffer);
	} else {
		contiguous = space;
	}
	BytesUsed += len;
	G_ASSERT(BytesUsed <= BufferSize);
	int rest = len - contiguous;
	if (rest <= 0) {
		memcpy(WritePtr, buffer, len);
		WritePtr += len;
		return(len);
	}
	memcpy(WritePtr, buffer, contiguous);
	memcpy(Buffer, (const char *)buffer + contiguous, rest);
	WritePtr = Buffer + rest;
	return(len);
}


int CircBuffClass::Get_Free() const
{
	return(BufferSize - BytesUsed);
}


int CircBuffClass::Get_Used() const
{
	return(BytesUsed);
}


bool CircBuffClass::Unread(int len)
{
	if (len > Get_Free()) {
		return(false);
	}
	if (len) {
		int back = (ReadPtr - Buffer) - len;
		if (back > 0) {
			ReadPtr = Buffer + back;
		} else {
			ReadPtr = Buffer + BufferSize + back;
		}
		BytesUsed += len;
	}
	return(true);
}


/*
**	Reconstruction notes (circbuff.cpp)
**
**	* All seven functions are the client's (0x830BF0-0x830F30), byte for byte; the asserts sit on
**	  the client's lines 112 and 166.
**	* Write refuses a block larger than the free space and then returns the free space (the
**	  client's eax); Unread puts back the last len bytes read (the stream connection's peek of a
**	  header).
**	* The function before the constructor, 0x830B60 (a name lookup in a table at 0xBF3188), is not
**	  this file's.
*/
