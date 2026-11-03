// Earth & Beyond client (reconstruction): app\shared\chat_edit_stream.cpp. Notes at the end of the file.
#include "unknown_debug.h"
#include "chat_edit_stream.h"
#include <stdlib.h>
#include <string.h>

// 0x4C4360
// NOT EXACT: call form (the client calls 0x4C44A0 through its vtable slot, call [0xAEBAB8]; this compiles to a direct call)
Unknown_ChatStreamClass::Unknown_ChatStreamClass(const void *data, int size) :
	Unknown04(NULL),
	Unknown08(0)
{
	Unknown_4C44E0(data, size);
}

// 0x4C43D0
char *Unknown_ChatStreamClass::Unknown_00()
{
	return((char *)Unknown04);
}

// 0x4C43F0
int Unknown_ChatStreamClass::Unknown_04()
{
	return(Unknown08);
}

// 0x4C4410
void *Unknown_ChatStreamClass::Unknown_4C4410()
{
	return(Unknown_4C45B0(8, 0, NULL, 0));
}

// 0x4C4430
Unknown_ChatStreamClass::Unknown_ChatStreamClass() :
	Unknown04(NULL),
	Unknown08(0)
{
}

// 0x4C4460
Unknown_ChatStreamClass::~Unknown_ChatStreamClass()
{
	if (Unknown04) {
		free(Unknown04);
		Unknown04 = NULL;
		Unknown08 = 0;
	}
}

// 0x4C44A0
void Unknown_ChatStreamClass::Unknown_4C44A0()
{
	if (Unknown04) {
		free(Unknown04);
		Unknown04 = NULL;
		Unknown08 = 0;
	}
}

// 0x4C44E0
void Unknown_ChatStreamClass::Unknown_4C44E0(const void *data, int size)
{
	Unknown_4C44A0();
	Unknown04 = malloc(size);
	memcpy(Unknown04, data, size);
	Unknown08 = size;
}

// 0x4C4530
void *Unknown_ChatStreamClass::Unknown_4C4530(int size)
{
	int old = Unknown_04();
	if (Unknown04 == NULL) {
		Unknown04 = malloc(size);
		Unknown08 = size;
	} else {
		Unknown04 = realloc(Unknown04, old + size);
		Unknown08 = old + size;
	}
	memset((char *)Unknown04 + old, 0, size);
	return(Unknown04);
}







// 0x4C45B0
void *Unknown_ChatStreamClass::Unknown_4C45B0(char type, unsigned char value, const char *text, short len)
{
	G_ASSERT(type <= HIGHWATER);
	int offset = Unknown_04();
	int length = len;
	int size = length;
	if (size) {
		size++;
	}
	Unknown_ChatBlockStruct head;
	head.Value = value;
	head.Type = type;
	head.Length = (short)size;
	Unknown_4C4530(size + sizeof(Unknown_ChatBlockStruct));
	Unknown_ChatBlockStruct *block = (Unknown_ChatBlockStruct *)((char *)Unknown04 + offset);
	*block = head;
	if (text && len > 0) {
		memcpy(block + 1, text, length);
	}
	return(Unknown04);
}

// 0x4C46E0
void *Unknown_ChatStreamClass::Unknown_4C46E0(const void *block)
{
	if (block == NULL) {
		return(Unknown04);
	}
	int size = ((const Unknown_ChatBlockStruct *)block)->Length + sizeof(Unknown_ChatBlockStruct);
	int offset = Unknown_04();
	Unknown_4C4530(size);
	memcpy((char *)Unknown04 + offset, block, size);
	return(Unknown04);
}

// 0x4C47B0
void *Unknown_ChatStreamClass::Unknown_4C47B0(Unknown_ChatStreamClass *stream, bool skip_end)
{
	if (stream == NULL) {
		return(Unknown04);
	}
	const Unknown_ChatBlockStruct *block = (const Unknown_ChatBlockStruct *)stream->Unknown_00();
	if (block == NULL) {
		return(Unknown04);
	}
	bool end = false;
	do {
		if (block->Type == 8) {
			end = true;
			if (skip_end) {
				break;
			}
		}
		Unknown_4C46E0(block);
		block = (const Unknown_ChatBlockStruct *)((const char *)block + block->Length + sizeof(Unknown_ChatBlockStruct));
	} while (!end);
	return(Unknown04);
}

// 0x4C4830
void Unknown_ChatStreamClass::Unknown_4C4830()
{
}

// 0x4C4850
void *Unknown_ChatStreamClass::Unknown_4C4850(const void *blocks, bool skip_end)
{
	if (blocks == NULL) {
		return(Unknown04);
	}
	Unknown_4C44A0();
	const Unknown_ChatBlockStruct *block = (const Unknown_ChatBlockStruct *)blocks;
	bool end = false;
	do {
		if (block->Type == 8) {
			end = true;
			if (skip_end) {
				break;
			}
		}
		Unknown_4C46E0(block);
		block = (const Unknown_ChatBlockStruct *)((const char *)block + block->Length + sizeof(Unknown_ChatBlockStruct));
	} while (!end);
	return(Unknown04);
}

// 0x4C48C0
void Unknown_ChatStreamClass::Unknown_4C48C0(char *text)
{
	const Unknown_ChatBlockStruct *block = (const Unknown_ChatBlockStruct *)Unknown_00();
	if (block == NULL) {
		return;
	}
	for (;;) {
		switch (block->Type) {
			case 1:
				strcat(text, (const char *)(block + 1));
				break;
			case 8:
				return;
		}
		block = (const Unknown_ChatBlockStruct *)((const char *)block + block->Length + sizeof(Unknown_ChatBlockStruct));
	}
}

// 0x4C4940
bool Unknown_ChatStreamClass::Unknown_4C4940()
{
	const Unknown_ChatBlockStruct *block = (const Unknown_ChatBlockStruct *)Unknown_00();
	if (block == NULL) {
		return(false);
	}
	for (;;) {
		switch (block->Type) {
			case 1:
				return(true);
			case 8:
				return(false);
		}
		block = (const Unknown_ChatBlockStruct *)((const char *)block + block->Length + sizeof(Unknown_ChatBlockStruct));
	}
}

/*
**	Reconstruction notes (chat_edit_stream.cpp)
**
**	* Client 0x4C4360-0x4C4962 (0x4C4980 starts the next object): the file from 0x4C45B0's assert
**	  ('type <= HIGHWATER', line 94; e:\build\beta\G\app\shared\chat_edit_stream.cpp), rebuilt by
**	  builder CHAT (2026-11-03) from the client's code. The class name is neutral (D10): the file's
**	  name suggests ChatEditStreamClass, unconfirmed. 0x4C4530 has no caller: 0x4C45B0 and 0x4C46E0
**	  inline it.
*/
