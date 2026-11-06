// Earth & Beyond client (reconstruction): app\shared\chat_edit_stream.h (the name from its .cpp's __FILE__)
/*
**	The chat stream (vtable 0xAEBAA8; client 0x4C4360-0x4C4962, chat_edit_stream.cpp): a malloc'd
**	run of blocks, each a 4-byte head (Unknown_ChatBlockStruct) and its Length bytes. Type 8 ends a
**	stream (0x4C4410 adds it); type 1 is text, 0 a mood, 2 a gesture (nlp.cpp's 0x78F050). The
**	avatar's +0xE8 is one (0x77B870 constructs it with 0x4C4430); opcodes 94 / 95 carry one.
*/

#ifndef CHAT_EDIT_STREAM_H
#define CHAT_EDIT_STREAM_H

#define HIGHWATER	9	// the last block type: 0x4C45B0's 'type <= HIGHWATER' assert

// a block's head (0x4C45B0 builds it as one dword)
struct Unknown_ChatBlockStruct	// NAME 1: neutral (D10)
{
	char Type;	// +0x00 (<= HIGHWATER; 8 ends the stream)  // NAME 2: 0x4C45B0's 'type' assert
	unsigned char Value;	// +0x01  // NAME 1: neutral (D10)
	short Length;	// +0x02 (the bytes after the head; a text's with its 0)  // NAME 1: neutral (D10)
};

// the stream's interface: a vtable of 11 _purecall slots (0xB046F4, no RTTI), stored only by the
// implicit copy constructor's base part (the avatar's 0x77C8F0 copies +0xE8 through it)
class Unknown_ChatStreamBaseClass  // NAME 1: neutral (D10)
{
public:
	virtual char *Unknown_00() = 0;
	virtual int Unknown_04() = 0;
	virtual void Unknown_4C48C0(char *text) = 0;
	virtual bool Unknown_4C4940() = 0;
	virtual void Unknown_4C44A0() = 0;
	virtual void *Unknown_4C4850(const void *blocks, bool skip_end) = 0;
	virtual void *Unknown_4C47B0(class Unknown_ChatStreamClass *stream, bool skip_end) = 0;
	virtual void *Unknown_4C46E0(const void *block) = 0;
	virtual void *Unknown_4C45B0(char type, unsigned char value, const char *text, short len) = 0;
	virtual void *Unknown_4C4410() = 0;
	virtual void Unknown_4C4830() = 0;
};

class Unknown_ChatStreamClass : public Unknown_ChatStreamBaseClass  // NAME 1: neutral (D10): what it reads
{
public:
	Unknown_ChatStreamClass();	// 0x4C4430
	Unknown_ChatStreamClass(const void *data, int size);	// 0x4C4360 (the bytes copied)
	~Unknown_ChatStreamClass();	// 0x4C4460 (not virtual: the vtable has no destructor)

	virtual char *Unknown_00();	// +0x00 0x4C43D0: Unknown04  // NAME 1: neutral (D10)
	virtual int Unknown_04();	// +0x04 0x4C43F0: Unknown08 (AVATAR_EMOTE 94 takes it as its ChatSize)  // NAME 1: neutral (D10)
	virtual void Unknown_4C48C0(char *text);	// +0x08 0x4C48C0: the type 1 blocks' text appended (strcat) to text  // NAME 1: neutral (D10)
	virtual bool Unknown_4C4940();	// +0x0C 0x4C4940: a type 1 block before the end  // NAME 1: neutral (D10)
	virtual void Unknown_4C44A0();	// +0x10 0x4C44A0: emptied (the avatar clears it before parsing)  // NAME 1: neutral (D10)
	virtual void *Unknown_4C4850(const void *blocks, bool skip_end);	// +0x14 0x4C4850: emptied, then blocks' copied up to the end  // NAME 1 [blocks, skip_end]: neutral (D10)
	virtual void *Unknown_4C47B0(Unknown_ChatStreamClass *stream, bool skip_end);	// +0x18 0x4C47B0: stream's blocks appended up to the end  // NAME 1 [stream, skip_end]: neutral (D10)
	virtual void *Unknown_4C46E0(const void *block);	// +0x1C 0x4C46E0: one block appended  // NAME 1 [block]: neutral (D10)
	virtual void *Unknown_4C45B0(char type, unsigned char value, const char *text, short len);	// +0x20 0x4C45B0: adds a block (nlp.cpp: 0 mood, 1 text with its talk animation, 2 gesture)  // NAME 1 [value, text, len]: neutral (D10)
	virtual void *Unknown_4C4410();	// +0x24 0x4C4410: the end block (type 8)  // NAME 1: neutral (D10)
	virtual void Unknown_4C4830();	// +0x28 0x4C4830: empty  // NAME 1: neutral (D10)

	void Unknown_4C44E0(const void *data, int size);	// 0x4C44E0: emptied, then the bytes copied (0x781800, 0x781840, 0x784B80)  // NAME 1 [data, size]: neutral (D10)
	void *Unknown_4C4530(int size);	// 0x4C4530: size zero bytes added (no caller: 0x4C45B0 / 0x4C46E0 inline it)  // NAME 1 [size]: neutral (D10)

	void *Unknown04;	// +0x04 (malloc'd)
	int Unknown08;	// +0x08 (its bytes)
};

#endif
