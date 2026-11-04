// Earth & Beyond client (reconstruction): the proof-of-concept client's stand-ins (D179). NOT THE CLIENT'S CODE.
/*
**	The side systems WinMain (app\main\main_client.cpp) calls that the tree does not have yet and the POC
**	(start-up videos -> login -> character select) can do without. Each stand-in logs its first call
**	(stubs/stub_log.h: OutputDebugString, stderr and the stub log) and returns a neutral value. Delete one
**	here when its real function lands in app/ (the linker reports the duplicate otherwise).
**
**	This file lives with the POC program, not in stubs/ (every program links stubs.lib, and one object
**	defining several of these would clash with a test program's own definitions), and not in app/ (it
**	is not the client's code, so it must never be matched or counted). The class declarations are
**	main_client.cpp's views, copied: keep them the same.
*/



#include <windows.h>
#include "..\..\stubs\stub_log.h"
#include "..\..\app\kernel\kernel.h"

// ---------------------------------------------------------------- ui_registration.cpp, the dialogs

// 0x847D20: the EULA / ROC text in a dialog; true, as if accepted
bool Unknown_847D20(const char *filename)
{
	STUB_CALLED("Unknown_847D20 (0x847D20, the EULA / ROC dialog): accepted");
	return true;
}

// 0x846C90: the -MENU dialog; true, go on
bool Unknown_846C90()
{
	STUB_CALLED("Unknown_846C90 (0x846C90, the -MENU dialog): go on");
	return true;
}

// ---------------------------------------------------------------- the assert reporters (0x4E9410 / 0x4E9450), teed

// app/shared/unknown_debug.cpp's two functions, with a line in the stub log first: the client's reporter
// 0x4BC260 (called next, as the client does) drops every message while no listener is connected to the
// message server (-ENABLE_MESSAGES; tools/harness/msglisten.py), and the start-up ones (combo_ffactory.cpp,
// soundlib.cpp) come before the server can accept one. Defining both keeps unknown_debug.obj out.
void Unknown_4BC260(const char *sender, const char *category, const char *file, int line, const char *format, ...);

void Unknown_Assert_Failed(const char *expr, const char *file, int line)
{
	Stub_Log_Line("G_ASSERT: ASSERTION FAILED: %s, %s(%d)", expr, file, line);
	Unknown_4BC260("", "G_ASSERT", file, line, "ASSERTION FAILED: %s", expr);
}

void Unknown_Fail_If_Report(const char *expr, const char *file, int line)
{
	Stub_Log_Line("FAIL_IF: ERROR: %s, %s(%d)", expr, file, line);
	Unknown_4BC260("", "FAIL_IF", file, line, "ERROR: %s", expr);
}

