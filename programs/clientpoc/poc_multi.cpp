// Earth & Beyond client (reconstruction): the proof-of-concept client's support for several instances at once. NOT THE CLIENT'S CODE.
//
// A static object whose constructor runs before WinMain redirects the exe's own imports (as poc_input.cpp does), always
// (the POC is a test build), so that many POC clients can run side by side and be addressed by PID:
//   kernel32 CreateMutexA  "enb_mutex_lock" becomes "enb_mutex_lock_<pid>": WinMain's ERROR_ALREADY_EXISTS check
//                          never fires for a second instance (the retail client gets the same from antprox, which
//                          renames that mutex in the suspended client)
//   user32 SetCursorPos    a no-op (TRUE) unless ENB_POC_INPUT=1, where poc_input.cpp's virtual cursor owns the slot:
//                          the client warps the real cursor back each frame (0x431C30), which fights the other
//                          instances and the user (antprox's /PATCHES:dml skips the same call)
//   user32 ClipCursor      a no-op (TRUE), same condition, if the exe imports it
#include <windows.h>
#include <stdio.h>
#include <string.h>

typedef HANDLE (WINAPI *PocCreateMutexType)(LPSECURITY_ATTRIBUTES, BOOL, LPCSTR);
static PocCreateMutexType Poc_Real_CreateMutexA = NULL;

static HANDLE WINAPI Poc_CreateMutexA(LPSECURITY_ATTRIBUTES attributes, BOOL owner, LPCSTR name)
{
	char renamed[64];
	if (name != NULL && strcmp(name, "enb_mutex_lock") == 0) {
		sprintf(renamed, "enb_mutex_lock_%lu", GetCurrentProcessId());
		name = renamed;
	}
	return Poc_Real_CreateMutexA(attributes, owner, name);
}

static BOOL WINAPI Poc_Multi_SetCursorPos(int, int)
{
	return TRUE;
}

static BOOL WINAPI Poc_Multi_ClipCursor(const RECT *)
{
	return TRUE;
}

static void Poc_Multi_Patch(void **slot, void *function)
{
	DWORD old;
	if (VirtualProtect(slot, sizeof(void *), PAGE_READWRITE, &old)) {
		*slot = function;
		VirtualProtect(slot, sizeof(void *), old, &old);
	}
}

static struct PocMulti
{
	PocMulti()
	{
		char value[8];
		bool virtualInput = GetEnvironmentVariableA("ENB_POC_INPUT", value, sizeof(value)) && value[0] == '1';
		unsigned char *base = (unsigned char *)GetModuleHandleA(NULL);
		IMAGE_NT_HEADERS *nt = (IMAGE_NT_HEADERS *)(base + ((IMAGE_DOS_HEADER *)base)->e_lfanew);
		IMAGE_DATA_DIRECTORY *dir = &nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];
		if (!dir->VirtualAddress) {
			return;
		}
		int hooked = 0;
		for (IMAGE_IMPORT_DESCRIPTOR *imp = (IMAGE_IMPORT_DESCRIPTOR *)(base + dir->VirtualAddress); imp->Name; imp++) {
			const char *dll = (const char *)(base + imp->Name);
			bool kernel = stricmp(dll, "kernel32.dll") == 0;
			bool user = stricmp(dll, "user32.dll") == 0;
			if ((!kernel && !user) || !imp->OriginalFirstThunk) {
				continue;
			}
			IMAGE_THUNK_DATA *names = (IMAGE_THUNK_DATA *)(base + imp->OriginalFirstThunk);
			IMAGE_THUNK_DATA *funcs = (IMAGE_THUNK_DATA *)(base + imp->FirstThunk);
			for (; names->u1.AddressOfData; names++, funcs++) {
				if (names->u1.Ordinal & IMAGE_ORDINAL_FLAG) {
					continue;
				}
				const char *name = (const char *)((IMAGE_IMPORT_BY_NAME *)(base + (DWORD)names->u1.AddressOfData))->Name;
				void *hook = NULL;
				if (kernel && strcmp(name, "CreateMutexA") == 0) {
					Poc_Real_CreateMutexA = (PocCreateMutexType)funcs->u1.Function;
					hook = (void *)Poc_CreateMutexA;
				} else if (user && !virtualInput && strcmp(name, "SetCursorPos") == 0) {
					hook = (void *)Poc_Multi_SetCursorPos;
				} else if (user && !virtualInput && strcmp(name, "ClipCursor") == 0) {
					hook = (void *)Poc_Multi_ClipCursor;
				}
				if (hook != NULL) {
					Poc_Multi_Patch((void **)&funcs->u1.Function, hook);
					hooked++;
				}
			}
		}
		FILE *f = fopen("stubs.log", "a");
		if (f != NULL) {
			fprintf(f, "POC multi: mutex per process, mouse lock off (%d imports redirected)\n", hooked);
			fclose(f);
		}
	}
} Poc_Multi_Installed;
