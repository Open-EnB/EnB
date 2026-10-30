// Earth & Beyond client (reconstruction): the assert reporters of unknown_debug.h. Notes at the end of the file.
#include "unknown_debug.h"

// E&B's message reporter (0x4BC260, cdecl, printf-style; the debug manager's: app\dbgmgr calls its
// sibling 0x4BC130 the same way). Not rebuilt: stubs/app_stubs.cpp has a stand-in (D75).
void Unknown_Report_Message(const char *sender, const char *category, const char *file, int line,
	const char *format, ...);	// NAME 1 [Unknown_Report_Message]: 0x4BC260 (its "Message_Error" string is a profiler label)

void Unknown_Assert_Failed(const char *expr, const char *file, int line)
{
	Unknown_Report_Message("", "G_ASSERT", file, line, "ASSERTION FAILED: %s", expr);
}

void Unknown_Fail_If_Report(const char *expr, const char *file, int line)
{
	Unknown_Report_Message("", "FAIL_IF", file, line, "ERROR: %s", expr);
}


/*
**	Reconstruction notes
**
**	client.exe 0x4E9410 and 0x4E9450, the first two functions of the object unknown_debug.h
**	describes. Both push the expression, the format, the line, the file, the category and ""
**	(0xBDA908: the client's pooled empty string, also the first two arguments of 0x4BC130's calls in
**	dbgmgr.cpp, pk.cpp's nstrdup fallback and dx8webbrowser's empty name; not an object) and call
**	0x4BC260. Byte-exact, but the two have the same bytes apart from their string relocations, so
**	objmatch finds each at both addresses (`multiple`) and verify cannot credit them.
**
**	The rest of the client's object: a file-scope std::map global (0xBE5648; constructed by
**	0x4E9490, destroyed through atexit by 0x4E94D0; nothing else refers to it, so its types are
**	unknown: not rebuilt) with its instances 0x4E94F0-0x4E9C40, then E&B's global operator new,
**	new[] and delete (0x4E9CB0, 0x4E9CE0, 0x4E9D10): unknown_new.cpp, kept apart so that a program
**	that defines its own assert reporters (tests/kerneltest, movietest) still links.
*/
