// dbgmgr.cpp (e:\build\beta\G\app\dbgmgr\dbgmgr.cpp): E&B's debug log. Reconstructed from client.exe; notes at the end.
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <deque>
#include "wwdebug.h"
#include "wwmemlog.h"
#include "..\shared\unknown_debug.h"

void Unknown_4BC000(const char *sender, const char *category, const char *file, int line, const char *format, ...);	// 0x4BC000 (unknown_message.h)
void Unknown_4BC130(const char *sender, const char *category, const char *file, int line, const char *format, ...);	// 0x4BC130 (unknown_message.h)
void Unknown_4BC260(const char *sender, const char *category, const char *file, int line, const char *format, ...);	// 0x4BC260 (unknown_message.h)
unsigned int Unknown_Get_Ticks();				// 0x4A3B40, the aux-data clock (unknown_root.h)
void Unknown_509C40(int type, const char *buf);	// NAME 1 [Unknown_509C40]: the log's output (WWDebug's handler 0x509C10 and the print forms)

struct DbgStepStruct {			// NAME 1 [DbgStepStruct]: a step of the step log, 0x5C bytes (44 to a deque block of 0xFD0)
	char Name[80];				// +0x00 Begin_Step's text
	unsigned int Start;			// +0x50 Unknown_Get_Ticks at Begin_Step (0 with Timing off)
	unsigned int Unknown54;		// +0x54 always 0
	int Depth;					// +0x58 Unknown_BE59C4 at Begin_Step
};

static std::deque<DbgStepStruct> Steps;		// 0xBE5990  // NAME 1 [Steps]: Begin_Step pushes, End_Step pops
static bool Timing = true;					// 0xB761D8  // NAME 1 [Timing]: End_Step prints the milliseconds
static int Unknown_BE59C4;					// 0xBE59C4, cleared by Unknown_509E70

static void Unknown_509C10(DebugType type, const char *message)	// NAME 1 [Unknown_509C10]: WWDebug's message handler
{
	Unknown_509C40(type, message);
}



void Unknown_509C40(int type, const char *buf)
{
	G_ASSERT(buf);

	switch (type) {
		case WWDEBUG_TYPE_INFORMATION:
			Unknown_4BC000("", "", __FILE__, __LINE__, "%s", buf);
			break;
		case WWDEBUG_TYPE_WARNING:
			Unknown_4BC130("", "", __FILE__, __LINE__, "%s", buf);
			break;
		case WWDEBUG_TYPE_ERROR:
			Unknown_4BC260("", "", __FILE__, __LINE__, "%s", buf);
			break;
	}
	char prefix[80] = "";
	OutputDebugString(prefix);
	OutputDebugString(buf);
	OutputDebugString("\n");
}

void Unknown_509D50(int type, const char *format, ...)
{
	char buf[256];
	va_list va;
	va_start(va, format);
	if (vsprintf(buf, format, va) >= 0) {
		buf[sizeof(buf) - 1] = 0;
		Unknown_509C40(type, buf);
	}
}

void Unknown_Debug_Printf(const char *format, ...)
{
	char buf[256];
	va_list va;
	va_start(va, format);
	if (vsprintf(buf, format, va) >= 0) {
		buf[sizeof(buf) - 1] = 0;
		Unknown_509C40(0, buf);
	}
}

void Unknown_Debug_Print(const char *text)
{
	Unknown_509C40(0, text);
}

void Unknown_509E50()
{
}

void Unknown_509E70()
{
	Unknown_BE59C4 = 0;
	WWDebug_Install_Message_Handler(Unknown_509C10);
	Unknown_509C40(0, "Earth and Beyond");
}



























// WWMemoryLogClass's counts and its protected Release_Log, reached from here (see the notes)
static inline int Get_Allocate_Count() { return WWMemoryLogClass::Get_Allocate_Count(); }
static inline int Get_Free_Count() { return WWMemoryLogClass::Get_Free_Count(); }
struct DbgMgrMemoryLogClass : public WWMemoryLogClass {	// NAME 2 [DbgMgrMemoryLogClass]: access only
	static void Release_Log() { WWMemoryLogClass::Release_Log(); }
};

void Unknown_509EB0()
{
	G_ASSERT(Get_Allocate_Count() == Get_Free_Count());
	DbgMgrMemoryLogClass::Release_Log();
}









void Unknown_Debug_Begin_Step(const char *buf)
{
	G_ASSERT(buf);
	Unknown_Debug_Printf("%s%s", buf, "... ");
	DbgStepStruct step;
	strncpy(step.Name, buf, sizeof(step.Name));
	step.Name[sizeof(step.Name) - 1] = 0;
	if (Timing) {
		step.Start = Unknown_Get_Ticks();
	} else {
		step.Start = 0;
	}
	step.Unknown54 = 0;
	step.Depth = Unknown_BE59C4;
	Steps.push_back(step);
}



// NOT EXACT: shape, 0.56 (explain.py): register allocation (the client keeps the deque's map in ebx and reloads buf; see the notes)
void Unknown_Debug_End_Step(const char *buf)
{
	G_ASSERT(buf);
	DbgStepStruct step = Steps.back();
	Steps.pop_back();
	unsigned int end;
	if (Timing) {
		end = Unknown_Get_Ticks();
	}
	if (Timing) {
		Unknown_Debug_Printf("%s [done %s] (%u ms)", buf, step.Name, end - step.Start);
	} else {
		Unknown_Debug_Printf("%s [done %s]", buf, step.Name);
	}
}

/*
** Notes (D25: the client's assert lines 36, 40/43/46, 128, 142 and 162 hold through the layout above).
**
** The file is e:\build\beta\G\app\dbgmgr\dbgmgr.cpp: every assert and reporter call of 0x509C40-0x50A1E0 names it.
** Its functions in client order: the static deque's initializer 0x509AD0 and atexit destructor 0x509B00 (compiler
** generated), std::deque<DbgStepStruct>::~deque 0x509B30, WWDebug's message handler 0x509C10, the output 0x509C40
** (reporter by WWDebug type, then OutputDebugString of an empty 80-byte prefix, the text and a newline), the print forms
** 0x509D50 / 0x509DC0 / 0x509E30, the empty 0x509E50, the start 0x509E70 (installs 0x509C10, prints
** "Earth and Beyond"), the end 0x509EB0 (asserts the memory log's counts, then WWMemoryLogClass::Release_Log),
** WinMain's step log 0x509EF0 / 0x50A1E0, then the deque's out-of-line members 0x50A560-0x50A810.
**
** 0x509EB0's assert text is 'Get_Allocate_Count() == Get_Free_Count()' with no class, and it calls the protected
** WWMemoryLogClass::Release_Log: E&B's code probably sat in a WWMemoryLogClass-derived scope. The forwarders above
** give the same text and code without changing wwmemlog.h (an interim form, D137).
*/
