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

// app/shared/g_assert.cpp's two functions, with a line in the stub log first: the client's reporter
// 0x4BC260 (called next, as the client does) drops every message while no listener is connected to the
// message server (-ENABLE_MESSAGES; tools/harness/msglisten.py), and the start-up ones (combo_ffactory.cpp,
// soundlib.cpp) come before the server can accept one. Defining both keeps g_assert.obj out.
void Message_Error(const char *sender, const char *category, const char *file, int line, const char *format, ...);

void G_Assert_Report(const char *expr, const char *file, int line)
{
	Stub_Log_Line("G_ASSERT: ASSERTION FAILED: %s, %s(%d)", expr, file, line);
	Message_Error("", "G_ASSERT", file, line, "ASSERTION FAILED: %s", expr);
}

void Fail_If_Report(const char *expr, const char *file, int line)
{
	Stub_Log_Line("FAIL_IF: ERROR: %s, %s(%d)", expr, file, line);
	Message_Error("", "FAIL_IF", file, line, "ERROR: %s", expr);
}


// ---------------------------------------------------------------- money.cpp's arithmetic and comparisons

// money.cpp (0x4F0500 operator+, 0x4F0700 operator>, 0x4F0740 operator>=; cdecl) is not rebuilt; ui_ptrade.cpp and
// ui_ship_cust.cpp / ui_avatar_cust.cpp call them. On MoneyClass's 8-byte value, without a log line (the trade and
// customization windows call them every frame).
#include "..\..\app\shared\money.h"

MoneyClass operator+(const MoneyClass &a, const MoneyClass &b)
{
	MoneyClass sum;
	sum.Unknown00 = a.Unknown00 + b.Unknown00;
	return sum;
}

bool operator>(const MoneyClass &a, const MoneyClass &b)
{
	return a.Unknown00 > b.Unknown00;
}

bool operator>=(const MoneyClass &a, const MoneyClass &b)
{
	return a.Unknown00 >= b.Unknown00;
}
