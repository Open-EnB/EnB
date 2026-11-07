// Earth & Beyond client (reconstruction): app\common\circbuff.cpp. Notes at the end of the file.
#include "circbuff.h"
#include "..\shared\g_assert.h"
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
#include <stdio.h>
#include <ctype.h>
#include <windows.h>




























































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



// 0x830F60
int CircBuffClass::Get_Contiguous_Used() const
{
	if (BytesUsed == 0) {
		return(0);
	}
	if (WritePtr <= ReadPtr) {
		return(BufferSize - (ReadPtr - Buffer));
	}
	return(BytesUsed);
}


// 0x830F90
int CircBuffClass::Get_Contiguous_Free() const
{
	int space = BufferSize - BytesUsed;
	if (space == 0) {
		return(0);
	}
	if (WritePtr >= ReadPtr) {
		return(BufferSize - (WritePtr - Buffer));
	}
	return(space);
}


// 0x830FD0
char *CircBuffClass::Peek(int offset, int *len)
{
	if (offset >= BytesUsed) {
		*len = 0;
		return(0);
	}
	int contiguous = Get_Contiguous_Used();
	if (offset < contiguous) {
		*len = contiguous - offset;
		return(ReadPtr + offset);
	}
	*len = BytesUsed - contiguous;
	return(Buffer + (offset - contiguous));
}


// 0x831060
bool CircBuffClass::Is_Valid() const
{
	if (WritePtr == ReadPtr) {
		return(BytesUsed == 0 || BytesUsed == BufferSize);
	}
	if (WritePtr > ReadPtr) {
		return(WritePtr - ReadPtr == BytesUsed);
	}
	return(ReadPtr - WritePtr == BufferSize - BytesUsed);
}


// 0x8310D0
void CircBuffClass::Dump(const char *title, int row)
{
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD pos;
	pos.X = 0;
	pos.Y = row;
	if (handle != NULL) {
		SetConsoleCursorPosition(handle, pos);
		if (title) {
			printf("%s ", title);
		}
		printf("Buffer(%p) Read(%4d) Write(%4d) Used(%4d) Free(%4d)\n", Buffer, ReadPtr - Buffer, WritePtr - Buffer, BytesUsed, BufferSize - BytesUsed);
		char *p = Buffer;
		char *end = Buffer + BufferSize;
		if (ReadPtr > WritePtr) {
			for (; p < WritePtr; p++) {
				if (isalnum(*p)) {
					putchar(*p);
				} else {
					putchar('.');
				}
			}
			for (; p < ReadPtr; p++) {
				putchar('_');
			}
			for (; p < end; p++) {
				if (isalnum(*p)) {
					putchar(*p);
				} else {
					putchar('.');
				}
			}
		} else if (ReadPtr < WritePtr) {
			for (; p < ReadPtr; p++) {
				putchar('_');
			}
			for (; p < WritePtr; p++) {
				if (isalnum(*p)) {
					putchar(*p);
				} else {
					putchar('.');
				}
			}
			for (; p < end; p++) {
				putchar('_');
			}
		} else if (BytesUsed) {
			for (; p < end; p++) {
				if (isalnum(*p)) {
					putchar(*p);
				} else {
					putchar('.');
				}
			}
		} else {
			for (; p < end; p++) {
				putchar('_');
			}
		}
	}
}


/*
**	Reconstruction notes (circbuff.cpp)
**
**	* All seven functions are the client's (0x830BF0-0x830F30), byte for byte; the asserts sit on
**	  the client's lines 112 and 166.
**	* The five after Unread (0x830F60-0x831277, TG3) have no caller in the client: two contiguous-run
**	  sizes (Read and Write compute the same inline), a peek at an offset, a consistency check and a
**	  console dump ('Buffer(%p) Read(%4d) ...', printf / putchar / isalnum); their names are chosen
**	  (NAME 2). A dump character is written as if / else around two putchar calls: the client pushes
**	  the byte or '.' and shares the call (a ?: makes one char first). The int3 block after the dump
**	  (0x831280-0x83133F) is padding.
**	* Write refuses a block larger than the free space and then returns the free space (the
**	  client's eax); Unread puts back the last len bytes read (the stream connection's peek of a
**	  header).
**	* The function before the constructor, 0x830B60 (a name lookup in a table at 0xBF3188), is not
**	  this file's.
*/
