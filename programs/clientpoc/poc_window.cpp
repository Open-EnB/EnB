// Earth & Beyond client (reconstruction): the proof-of-concept client's window and display (D179). NOT THE CLIENT'S CODE.
/*
**	Functional stand-ins, written from Ghidra's pseudocode of the client's functions, for what WinMain
**	(app\main\main_client.cpp) needs before anything can draw (none left: E&B's asset manager's
**	constructor 0x48B070 is app's now). Delete one here when its real function lands in app/ (the linker reports the duplicate).
**
**	The display is the client's own (app\display\display.cpp, 0x442AC0): the registry's render device
**	(read only, RegistryClass below), windowed when the registry says so; ENB_POC_WINDOWED has no effect.
*/



#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include "..\..\stubs\stub_log.h"
#include "..\..\ww3d2\ww3d.h"
#include "..\..\ww3d2\assetmgr.h"
#include "..\..\ww3d2\rendobj.h"
#include "..\..\wwlib\registry.h"

// ---------------------------------------------------------------- no registry changes (D69)

// RegistryClass opens every key read only and writes nothing from before WinMain on (IsLocked is constant-
// initialized, so this dynamic initializer runs after it whatever the objects' order).
static struct PocRegistryReadOnly
{
	PocRegistryReadOnly() { RegistryClass::Set_Read_Only(true); }
} Poc_Registry_Read_Only;

// ---------------------------------------------------------------- the crash line (stubs.log)

// A first-chance vectored handler (kernel32's, XP on: looked up, VC6's headers lack it) writes the faulting
// address and the code addresses on the stack as one 'CRASH:' line (tools/harness/symbolize.py names them
// with client.map) before the client's own filter (0x445330) ends the program. It only logs.
// For a register holding a render object (a crash in its member: edi in HLodClass's loops): its name and class
// ID, and the name of the bone ebx / 0xB0 (HLodClass::Update_Sub_Object_Transforms keeps the bone's transform
// offset, bone * 0xB0, in ebx). Guarded: the register may hold anything.
static int Poc_Crash_Robj(char *out, const char *reg, DWORD value, DWORD ebx)
{
	int n = 0;
	__try {
		RenderObjClass *robj = (RenderObjClass *)value;
		if (value < 0x10000 || IsBadReadPtr(robj, 4) || *(DWORD *)robj < 0x401000 || *(DWORD *)robj >= 0x1000000) {
			return 0;
		}
		const char *name = robj->Get_Name();
		int id = robj->Class_ID();
		n = sprintf(out, " %s: '%.40s' class %d", reg, (name && !IsBadStringPtr(name, 64)) ? name : "?", id);
		if (id == RenderObjClass::CLASSID_HLOD && ebx % 0xB0 == 0) {
			const char *bone = robj->Get_Bone_Name(ebx / 0xB0);
			n += sprintf(out + n, " bone %u '%.40s'", (unsigned)(ebx / 0xB0), (bone && !IsBadStringPtr(bone, 64)) ? bone : "?");
		}
	} __except (EXCEPTION_EXECUTE_HANDLER) {
		n += sprintf(out + n, " %s: (fault)", reg);
	}
	return n;
}

static LONG CALLBACK Poc_Crash_Log(EXCEPTION_POINTERS *info)
{
	static int count = 0;
	DWORD code = info->ExceptionRecord->ExceptionCode;
	if ((code != EXCEPTION_ACCESS_VIOLATION && code != EXCEPTION_ILLEGAL_INSTRUCTION && code != EXCEPTION_INT_DIVIDE_BY_ZERO
			&& code != EXCEPTION_STACK_OVERFLOW && code != EXCEPTION_PRIV_INSTRUCTION) || count >= 4) {
		return EXCEPTION_CONTINUE_SEARCH;
	}
	count++;
	char line[1024];
	int n = sprintf(line, "CRASH: code %08X at %08X (address %08X) stack:", code, (unsigned)info->ContextRecord->Eip,
		code == EXCEPTION_ACCESS_VIOLATION ? (unsigned)info->ExceptionRecord->ExceptionInformation[1] : 0);
	unsigned *sp = (unsigned *)info->ContextRecord->Esp;
	int found = 0;
	for (int i = 0; i < 2048 && found < 24 && n < (int)sizeof(line) - 12; i++) {
		if (IsBadReadPtr(sp + i, 4)) {
			break;
		}
		unsigned value = sp[i];
		if (value >= 0x401000 && value < 0x400000 + 0x300000) {		// the exe's code (client.map: .text)
			n += sprintf(line + n, " %08X", value);
			found++;
		}
	}
	Stub_Log_Line("%s", line);
	// The registers and, for each that points at readable memory, its first four dwords (an object's vtable
	// first: symbolize.py names it): which object a bad virtual call went through.
	CONTEXT *c = info->ContextRecord;
	const char *names[] = { "eax", "ebx", "ecx", "edx", "esi", "edi", "ebp" };
	DWORD regs[] = { c->Eax, c->Ebx, c->Ecx, c->Edx, c->Esi, c->Edi, c->Ebp };
	n = sprintf(line, "CRASH regs:");
	for (int r = 0; r < 7; r++) {
		n += sprintf(line + n, " %s=%08X", names[r], (unsigned)regs[r]);
		unsigned *p = (unsigned *)regs[r];
		if (regs[r] >= 0x10000 && !IsBadReadPtr(p, 16)) {
			n += sprintf(line + n, "[%08X %08X %08X %08X]", p[0], p[1], p[2], p[3]);
		}
	}
	n += Poc_Crash_Robj(line + n, "edi", c->Edi, c->Ebx);
	n += Poc_Crash_Robj(line + n, "esi", c->Esi, c->Ebx);
	Stub_Log_Line("%s", line);
	return EXCEPTION_CONTINUE_SEARCH;
}

typedef PVOID (WINAPI *PocAddVectoredHandler)(ULONG, LONG (CALLBACK *)(EXCEPTION_POINTERS *));

static struct PocCrashLog
{
	PocCrashLog()
	{
		PocAddVectoredHandler add = (PocAddVectoredHandler)GetProcAddress(GetModuleHandle("kernel32.dll"), "AddVectoredExceptionHandler");
		if (add != NULL) {
			add(1, Poc_Crash_Log);
		}
	}
} Poc_Crash_Log_Installed;

// ---------------------------------------------------------------- the window-message handlers

// The list is app\input\winmsg.cpp's (0x42F940 add, 0x42FAC0 remove, 0x42FBA0 dispatch).
void Unknown_42F940(int message, int (*handler)(int, int));
void Unknown_42FAC0(int message, int (*handler)(int, int));
int Unknown_42FBA0(int message, int wparam, int lparam);

// ---------------------------------------------------------------- the main window

// app\input\w32window.cpp has it (0x431CA0) and its window procedure (0x431DE0).

// E&B's asset manager's constructor 0x48B070 is app\shared\enbassetmgr.cpp's (its own table slots not yet written).
