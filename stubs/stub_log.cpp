// Earth & Beyond client (reconstruction): the stub log (D44). See stub_log.h.



#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "stub_log.h"

static void Write_Line(const char *line)
{
	OutputDebugStringA(line);
	// One WriteFile per line to stderr, so a line is not split into pieces another process's output can land between
	// (antprox.out carries the client's stderr beside its own lines; e2e.py reads the G_ASSERT / FAIL_IF tee there).
	HANDLE err = GetStdHandle(STD_ERROR_HANDLE);
	if (err && err != INVALID_HANDLE_VALUE) {
		DWORD written;
		WriteFile(err, line, (DWORD)strlen(line), &written, NULL);
	}
	// stubs.log is opened once and flushed per line: readable while the client runs, complete after a crash.
	static FILE *fp = NULL;
	static bool tried = false;
	if (!tried) {
		tried = true;
		const char *path = getenv("ENB_STUB_LOG");
		fp = fopen(path && *path ? path : "stubs.log", "a");
	}
	if (fp) {
		fputs(line, fp);
		fflush(fp);
	}
}

extern "C" void Stub_Log_Line(const char *format, ...)
{
	char line[1024];
	va_list va;
	va_start(va, format);
	_vsnprintf(line, sizeof(line) - 2, format, va);
	va_end(va);
	line[sizeof(line) - 2] = 0;
	size_t n = strlen(line);
	if (n == 0 || line[n - 1] != '\n') {
		line[n] = '\n';
		line[n + 1] = 0;
	}
	Write_Line(line);
}

// The first-call flags set so far, so that Stub_Log_Mark can clear them.
static int *Seen_Flags[4096];
static int Seen_Count = 0;

extern "C" void Stub_Log_First_Call(const char *name, int *seen)
{
	*seen = 1;
	if (Seen_Count < (int)(sizeof(Seen_Flags) / sizeof(Seen_Flags[0]))) {
		Seen_Flags[Seen_Count++] = seen;
	}
	Stub_Log_Line("STUB first call: %s", name);
}

// A marker line in the stub log; every stub's first-call flag starts over, so the stubs called after the marker
// are logged again. Exported by name: a test harness inside the process finds it with GetProcAddress
// (tools/harness/d3d8shot.c's 'mark NAME' trigger; tools/harness/e2e.py marks each scenario of a session).
extern "C" __declspec(dllexport) void Stub_Log_Mark(const char *name)
{
	for (int i = 0; i < Seen_Count; i++) {
		*Seen_Flags[i] = 0;
	}
	Seen_Count = 0;
	Stub_Log_Line("MARK %s", name);
}
