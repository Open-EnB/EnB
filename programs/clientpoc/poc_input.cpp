// Earth & Beyond client (reconstruction): the proof-of-concept client's virtual input (D179). NOT THE CLIENT'S CODE.
//
// With ENB_POC_INPUT=1 (tools/harness/clientpoc.py --input sets it) the exe's own user32 imports below are redirected
// so that the input clientpoc.py posts to the window (PostMessage) is what the client reads, and the real cursor and
// keyboard are never read or moved (an unattended run must not touch the desktop):
//   GetMessageA          passes each message through; a mouse message's lParam (client pixels) sets the virtual cursor
//                        and the message's pt, a key message sets the virtual key state
//   GetCursorPos         the virtual cursor (screen coordinates)          (w32mouse.cpp Get_Position / Get_Pixel_Position)
//   GetMessagePos        the virtual cursor at the last message           (w32mouse.cpp 0x4314C0)
//   SetCursorPos         moves the virtual cursor only                    (w32mouse.cpp Update / Set_Position)
//   GetForegroundWindow  the window the messages go to                    (input.cpp 0x42EE90: input only when foreground)
//   GetKeyState / GetAsyncKeyState  the virtual key state                 (w32kbd.cpp, w32mouse.cpp)
// Each mouse button and key-down message is logged to stubs.log as 'replayed' (it carries the harness's tag, POC_TAG
// below) or 'untagged'; with ENB_POC_INPUT_TAG=1 (tools/harness/e2e.py) untagged input is dropped ('NOT REPLAYED').
// Without ENB_POC_INPUT nothing is changed.
#include <windows.h>
#include <stdio.h>
#include <string.h>

static POINT Poc_Cursor = { 0, 0 };			// client pixels
static HWND Poc_Window = NULL;
static BYTE Poc_Keys[256];					// 0x80 down, 0x01 toggled

typedef BOOL (WINAPI *PocGetMessageType)(LPMSG, HWND, UINT, UINT);
static PocGetMessageType Poc_Real_GetMessageA = NULL;

static void Poc_Screen(POINT *p)
{
	*p = Poc_Cursor;
	if (Poc_Window != NULL) {
		ClientToScreen(Poc_Window, p);
	}
}

static void Poc_Input_Log(const char *text)
{
	FILE *f = fopen("stubs.log", "a");
	if (f != NULL) {
		fputs(text, f);
		fclose(f);
	}
}

// The harness's tag (tools/harness/clientpoc.py post_step(tag=True), e2e.py): POC_TAG in the high word of a mouse or
// key message's wParam (MK_ flags and virtual keys fit the low word). It is stripped here before the client sees the
// message. With ENB_POC_INPUT_TAG=1 an untagged mouse or key message (a human's click or keystroke in the test window,
// or the real cursor crossing it) is logged as such and dropped, so only replayed input reaches the client.
#define POC_TAG 0x5043
static int Poc_Tag_Only = -1;

static BOOL WINAPI Poc_GetMessageA(LPMSG msg, HWND window, UINT first, UINT last)
{
	BOOL result = Poc_Real_GetMessageA(msg, window, first, last);
	if (result > 0 && msg->hwnd != NULL) {
		if (Poc_Window == NULL) {
			Poc_Window = msg->hwnd;
		}
		if (Poc_Tag_Only < 0) {
			char value[8];
			Poc_Tag_Only = GetEnvironmentVariableA("ENB_POC_INPUT_TAG", value, sizeof(value)) && value[0] == '1';
		}
		bool input = (msg->message >= WM_MOUSEFIRST && msg->message <= WM_MBUTTONDBLCLK)
			|| msg->message == WM_KEYDOWN || msg->message == WM_KEYUP || msg->message == WM_SYSKEYDOWN || msg->message == WM_SYSKEYUP;
		const char *who = "untagged";
		if (input && HIWORD(msg->wParam) == POC_TAG) {
			msg->wParam &= 0xFFFF;
			who = "replayed";
		} else if (input && Poc_Tag_Only) {
			if (msg->message != WM_MOUSEMOVE) {
				char text[128];
				sprintf(text, "POC input: NOT REPLAYED (dropped) 0x%X wparam 0x%X lparam 0x%X\n", msg->message,
					(unsigned)msg->wParam, (unsigned)msg->lParam);
				Poc_Input_Log(text);
			}
			msg->message = WM_NULL;
			return result;
		}
		if (input && (msg->message == WM_KEYDOWN || msg->message == WM_SYSKEYDOWN)) {
			char text[96];
			sprintf(text, "POC input: %s key 0x%X\n", who, (unsigned)msg->wParam);
			Poc_Input_Log(text);
		}
		if (msg->message >= WM_MOUSEFIRST && msg->message <= WM_MBUTTONDBLCLK) {
			Poc_Cursor.x = (short)LOWORD(msg->lParam);
			Poc_Cursor.y = (short)HIWORD(msg->lParam);
			if (msg->message == WM_LBUTTONDOWN) Poc_Keys[VK_LBUTTON] |= 0x80;
			if (msg->message == WM_LBUTTONUP) Poc_Keys[VK_LBUTTON] &= ~0x80;
			if (msg->message == WM_RBUTTONDOWN) Poc_Keys[VK_RBUTTON] |= 0x80;
			if (msg->message == WM_RBUTTONUP) Poc_Keys[VK_RBUTTON] &= ~0x80;
			if (msg->message != WM_MOUSEMOVE) {
				char text[96];
				sprintf(text, "POC input: %s mouse 0x%X at %d, %d\n", who, msg->message, Poc_Cursor.x, Poc_Cursor.y);
				Poc_Input_Log(text);
			}
		} else if (msg->message == WM_KEYDOWN || msg->message == WM_SYSKEYDOWN) {
			if (!(Poc_Keys[msg->wParam & 0xFF] & 0x80)) {
				Poc_Keys[msg->wParam & 0xFF] ^= 0x01;
			}
			Poc_Keys[msg->wParam & 0xFF] |= 0x80;
		} else if (msg->message == WM_KEYUP || msg->message == WM_SYSKEYUP) {
			Poc_Keys[msg->wParam & 0xFF] &= ~0x80;
		}
		POINT p;
		Poc_Screen(&p);
		msg->pt = p;
	}
	return result;
}

static BOOL WINAPI Poc_GetCursorPos(LPPOINT point)
{
	Poc_Screen(point);
	return TRUE;
}

static DWORD WINAPI Poc_GetMessagePos()
{
	POINT p;
	Poc_Screen(&p);
	return MAKELONG((short)p.x, (short)p.y);
}

static BOOL WINAPI Poc_SetCursorPos(int x, int y)
{
	POINT p;
	p.x = x;
	p.y = y;
	if (Poc_Window != NULL) {
		ScreenToClient(Poc_Window, &p);
	}
	Poc_Cursor = p;
	return TRUE;
}

static HWND WINAPI Poc_GetForegroundWindow()
{
	return Poc_Window;
}

static SHORT WINAPI Poc_GetKeyState(int key)
{
	BYTE state = Poc_Keys[key & 0xFF];
	return (SHORT)(((state & 0x80) ? 0x8000 : 0) | (state & 0x01));
}

static SHORT WINAPI Poc_GetAsyncKeyState(int key)
{
	return (SHORT)((Poc_Keys[key & 0xFF] & 0x80) ? 0x8000 : 0);
}

static void Poc_Patch(void **slot, void *function)
{
	DWORD old;
	if (VirtualProtect(slot, sizeof(void *), PAGE_READWRITE, &old)) {
		*slot = function;
		VirtualProtect(slot, sizeof(void *), old, &old);
	}
}

static struct PocInput
{
	PocInput()
	{
		char value[8];
		if (!GetEnvironmentVariableA("ENB_POC_INPUT", value, sizeof(value)) || value[0] != '1') {
			return;
		}
		unsigned char *base = (unsigned char *)GetModuleHandleA(NULL);
		IMAGE_NT_HEADERS *nt = (IMAGE_NT_HEADERS *)(base + ((IMAGE_DOS_HEADER *)base)->e_lfanew);
		IMAGE_DATA_DIRECTORY *dir = &nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];
		if (!dir->VirtualAddress) {
			return;
		}
		int hooked = 0;
		for (IMAGE_IMPORT_DESCRIPTOR *imp = (IMAGE_IMPORT_DESCRIPTOR *)(base + dir->VirtualAddress); imp->Name; imp++) {
			if (stricmp((const char *)(base + imp->Name), "user32.dll") != 0 || !imp->OriginalFirstThunk) {
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
				if (strcmp(name, "GetMessageA") == 0) {
					Poc_Real_GetMessageA = (PocGetMessageType)funcs->u1.Function;
					hook = (void *)Poc_GetMessageA;
				} else if (strcmp(name, "GetCursorPos") == 0) {
					hook = (void *)Poc_GetCursorPos;
				} else if (strcmp(name, "GetMessagePos") == 0) {
					hook = (void *)Poc_GetMessagePos;
				} else if (strcmp(name, "SetCursorPos") == 0) {
					hook = (void *)Poc_SetCursorPos;
				} else if (strcmp(name, "GetForegroundWindow") == 0) {
					hook = (void *)Poc_GetForegroundWindow;
				} else if (strcmp(name, "GetKeyState") == 0) {
					hook = (void *)Poc_GetKeyState;
				} else if (strcmp(name, "GetAsyncKeyState") == 0) {
					hook = (void *)Poc_GetAsyncKeyState;
				}
				if (hook != NULL) {
					Poc_Patch((void **)&funcs->u1.Function, hook);
					hooked++;
				}
			}
		}
		char text[128];
		value[0] = 0;
		GetEnvironmentVariableA("ENB_POC_INPUT_TAG", value, sizeof(value));
		sprintf(text, "POC input: virtual input, %d user32 imports redirected; tagged input read%s\n", hooked,
			value[0] == '1' ? ", untagged dropped" : "");
		Poc_Input_Log(text);
	}
} Poc_Input_Installed;
