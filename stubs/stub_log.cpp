/*
**	Earth & Beyond client (reconstruction): the stub log (D44). See stub_log.h.
*/

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "stub_log.h"

static void Write_Line(const char *line)
{
	OutputDebugStringA(line);
	fputs(line, stderr);
	const char *path = getenv("ENB_STUB_LOG");
	FILE *fp = fopen(path && *path ? path : "stubs.log", "a");
	if (fp) {
		fputs(line, fp);
		fclose(fp);
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

extern "C" void Stub_Log_First_Call(const char *name, int *seen)
{
	*seen = 1;
	Stub_Log_Line("STUB first call: %s", name);
}
