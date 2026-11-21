// Earth & Beyond client (reconstruction): the POC's stand-ins for the functions gen_stubs can't write (D179). NOT THE CLIENT'S CODE.
/*
**	gen_stubs cannot write a stand-in with a structure returned or passed by value or a vector of structures: the
**	calls go to the PE header (the crash at EIP 0x400003-5, with the return address popped). These are what the way
**	into the world reaches, written from the pseudocode where a real result matters, empty where it doesn't.
*/
#include <string.h>
#include <vector>
#include "..\..\stubs\stub_log.h"
#include "..\..\app\client\sclient.h"
#include "..\..\app\client\co.h"

// UnknownB02C90Class::BufferStruct::Insert (0x725E70) and Unknown_725AD0 (0x725AD0) are real app code in
// app/client/unknown_725050.cpp since run 2026-11-21 (WARP); the watchdog they started now starts with the process.

// ---------------------------------------------------------------- the client's view of keys no gadget took: nothing
void SClient::Unknown_744240(KernelEventClass event)
{
}

// Unknown8CClass (0x73FA60 / 0x73FB20) and UnknownAF0DD0Class::Unknown_73FDB0 (0x73FDB0) are real app code in app/client/unknown_73fa60.cpp.

// ---------------------------------------------------------------- a watchdog (POC only): every 5 s the main thread's EIP and the code addresses on its stack, in the stub log
#include <windows.h>
#include <stdio.h>
static HANDLE Poc_Main_Thread;
static DWORD WINAPI Poc_Watchdog(void *)
{
	for (int n = 0; n < 120; n++) {
		Sleep(5000);
		SuspendThread(Poc_Main_Thread);
		CONTEXT c;
		c.ContextFlags = CONTEXT_FULL;
		char text[512];
		int len = 0;
		if (GetThreadContext(Poc_Main_Thread, &c)) {
			len += sprintf(text + len, "POC watchdog %d: %08X stack:", n, c.Eip);
			DWORD *sp = (DWORD *)c.Esp;
			int found = 0;
			for (int i = 0; i < 400 && found < 12; i++) {
				if (IsBadReadPtr(sp + i, 4)) {
					break;
				}
				if (sp[i] >= 0x401000 && sp[i] < 0x6A0000) {
					len += sprintf(text + len, " %08X", sp[i]);
					found++;
				}
			}
		}
		ResumeThread(Poc_Main_Thread);
		// the same line as a first call, without one: Stub_Log_First_Call keeps the flag's address for Stub_Log_Mark,
		// and a flag on this thread's stack is gone once the thread ends after 300 s (the next mark wrote into it)
		Stub_Log_Line("STUB first call: %s", text);
	}
	return(0);
}
void Poc_Start_Watchdog()
{
	DuplicateHandle(GetCurrentProcess(), GetCurrentThread(), GetCurrentProcess(), &Poc_Main_Thread, 0, FALSE, DUPLICATE_SAME_ACCESS);
	DWORD id;
	CreateThread(NULL, 0, Poc_Watchdog, NULL, 0, &id);
}
// started with the process (a static initialiser: the main thread) since the mount buffer's insert, which started it
// at world entry, is real code: 120 x 5 s covers the login and the first minutes in the world
static struct Poc_Watchdog_Starter { Poc_Watchdog_Starter() { Poc_Start_Watchdog(); } } Poc_Watchdog_Starter_Instance;

// co.cpp's render hook 0x6BB7E0 / 0x6BB8B0 is real app code since WE5 (app/client/unknown_6bb7e0.cpp).
