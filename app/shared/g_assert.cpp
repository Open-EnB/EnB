// Earth & Beyond client (reconstruction): the assert reporters of g_assert.h. Notes at the end of the file.
#include "g_assert.h"

#include "message.h"
#include <map>

struct Unknown_BE5648Struct	// NAME 1: neutral (D10); 8 bytes: the map's 0x1C-byte nodes, nothing else uses it
{
	int Unknown00;	// NAME 1: neutral (D10)
	int Unknown04;	// NAME 1: neutral (D10)
};

struct Unknown_BE5628Struct {	// NAME 1: neutral (D10); 12 bytes, nothing refers to its two statics
	Unknown_BE5628Struct() : Unknown00(""), Unknown04(""), Unknown08(-1) {}
	const char *Unknown00;	// NAME 1: neutral (D10)
	const char *Unknown04;	// NAME 1: neutral (D10)
	int Unknown08;			// NAME 1: neutral (D10)
};
static Unknown_BE5628Struct Unknown_BE5638;	// 0xBE5638 (0x4E93B0)  // NAME 1: neutral (D10)
static Unknown_BE5628Struct Unknown_BE5628;	// 0xBE5628 (0x4E93E0)  // NAME 1: neutral (D10)

void G_Assert_Report(const char *expr, const char *file, int line)
{
	Message_Error("", "G_ASSERT", file, line, "ASSERTION FAILED: %s", expr);
}

void Fail_If_Report(const char *expr, const char *file, int line)
{
	Message_Error("", "FAIL_IF", file, line, "ERROR: %s", expr);
}

static std::map<int, Unknown_BE5648Struct> Unknown_BE5648;	// 0xBE5648 (0x4E9490 constructs, 0x4E94D0 destroys)  // NAME 1: neutral (D10); the key type is unknown


/*
**	Reconstruction notes
**
**	client.exe 0x4E9410 and 0x4E9450, the first two functions of the object g_assert.h
**	describes. Both push the expression, the format, the line, the file, the category and ""
**	(0xBDA908: the client's pooled empty string, also the first two arguments of 0x4BC130's calls in
**	dbgmgr.cpp, pk.cpp's nstrdup fallback and dx8webbrowser's empty name; not an object) and call
**	0x4BC260. Byte-exact, but the two have the same bytes apart from their string relocations, so
**	objmatch finds each at both addresses (`multiple`) and verify cannot credit them.
**
**	The rest of the client's object: a file-scope std::map global (0xBE5648; constructed by
**	0x4E9490, destroyed through atexit by 0x4E94D0) with its instances 0x4E94F0-0x4E9C40 (the
**	destructor, constructor, begin, erase, _Erase, _Inc: their only callers are 0x4E9490 / 0x4E94D0
**	and each other). Nothing else refers to it, so only its node size is known (0x1C bytes: a
**	12-byte pair, trivially destroyed); the key and value types are placeholders. Then E&B's global operator new,
**	new[] and delete (0x4E9CB0, 0x4E9CE0, 0x4E9D10): unknown_new.cpp, kept apart so that a program
**	that defines its own assert reporters (tests/kerneltest, movietest) still links.
**
**	IM8 (run 2026-11-26): the two 12-byte statics 0xBE5638 / 0xBE5628 ({"", "", -1}, initialisers 0x4E93B0 /
**	0x4E93E0, nothing refers to them) are this object's, out of event.cpp: the CRT table lists event's
**	ActiveIterators 0x4E8670, then 0x4E93B0, 0x4E93E0 and the map 0x4E9490, with one zero slot after 0x4E9490
**	only; an incremental link pads every object of two or more entries with a zero after it, so event's
**	object has one entry and this one three. Their "" is 0xBDA908, the literal the reporters push.
*/
