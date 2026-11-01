// Earth & Beyond client (reconstruction): app\input\w32kbd.cpp. Notes at the end of the file.
#include "w32kbd.h"
#include "..\shared\unknown_debug.h"
#include <ctype.h>

void Unknown_4BC000(const char *sender, const char *category, const char *file, int line, const char *format, ...);	// NAME 1 [Unknown_4BC000]: 0x4BC000, E&B's message reporter (tcpconn.cpp)

static bool Unknown_B6EA58 = true;					// Set_Repeat's flag: auto-repeated presses are kept  // NAME 1 [Unknown_B6EA58]: neutral (D10)
static unsigned char Unknown_BDAAE8[INPUT_KEY_COUNT];	// 1: a letter, 2: a key of the numeric pad's second meaning  // NAME 1 [Unknown_BDAAE8]: neutral (D10)
static int Unknown_BDABF0;							// the table is filled  // NAME 1 [Unknown_BDABF0]: neutral (D10)

int Unknown_4307A0(int message, unsigned int wparam, unsigned int lparam);	// NAME 1 [Unknown_4307A0, message, wparam, lparam]: neutral (D10)

Win32KeyboardHandlerClass::Win32KeyboardHandlerClass(KeyQueueClass *queue) : KeyboardClass(queue)
{
	if (Unknown_BDABF0 == 0) {
		memset(Unknown_BDAAE8, 0, sizeof(Unknown_BDAAE8));
		Unknown_BDABF0 = 1;
		for (int key = 0; key < 0xFF; key++) {
			switch (key) {
				case VK_PRIOR:
				case VK_NEXT:
				case VK_END:
				case VK_HOME:
				case VK_LEFT:
				case VK_UP:
				case VK_RIGHT:
				case VK_DOWN:
				case VK_INSERT:
				case VK_DELETE:
					Unknown_BDAAE8[key] = 2;
					break;
				default:
					if (isalpha(key) && isupper(key)) {
						Unknown_BDAAE8[key] = 1;
					}
					break;
			}
		}
	}
	Unknown_42F940(WM_KEYDOWN, (int (*)(int, int))Unknown_4307A0);
	Unknown_42F940(WM_KEYUP, (int (*)(int, int))Unknown_4307A0);
	Unknown_42F940(WM_SYSKEYDOWN, (int (*)(int, int))Unknown_4307A0);
	Unknown_42F940(WM_SYSKEYUP, (int (*)(int, int))Unknown_4307A0);
}

Win32KeyboardHandlerClass::~Win32KeyboardHandlerClass()
{
	Unknown_42FAC0(WM_KEYDOWN, (int (*)(int, int))Unknown_4307A0);
	Unknown_42FAC0(WM_KEYUP, (int (*)(int, int))Unknown_4307A0);
	Unknown_42FAC0(WM_SYSKEYDOWN, (int (*)(int, int))Unknown_4307A0);
	Unknown_42FAC0(WM_SYSKEYUP, (int (*)(int, int))Unknown_4307A0);
}

bool Win32KeyboardHandlerClass::Set_Repeat(bool enable)
{
	bool old = Unknown_B6EA58;
	Unknown_B6EA58 = enable;
	return old;
}

void Win32KeyboardHandlerClass::Unknown_04()
{
	Unknown_42F940(WM_KEYDOWN, (int (*)(int, int))Unknown_4307A0);
	Unknown_42F940(WM_KEYUP, (int (*)(int, int))Unknown_4307A0);
	Unknown_42F940(WM_SYSKEYDOWN, (int (*)(int, int))Unknown_4307A0);
	Unknown_42F940(WM_SYSKEYUP, (int (*)(int, int))Unknown_4307A0);
}

void Win32KeyboardHandlerClass::Unknown_08()
{
	Unknown_42FAC0(WM_KEYDOWN, (int (*)(int, int))Unknown_4307A0);
	Unknown_42FAC0(WM_KEYUP, (int (*)(int, int))Unknown_4307A0);
	Unknown_42FAC0(WM_SYSKEYDOWN, (int (*)(int, int))Unknown_4307A0);
	Unknown_42FAC0(WM_SYSKEYUP, (int (*)(int, int))Unknown_4307A0);
}






































int Unknown_4307A0(int message, unsigned int wparam, unsigned int lparam)
{
	if (!Unknown_42EE90()) {
		return 0;
	}
	if (!Unknown_B6EA58 && (lparam & 0x40000000) && !(lparam & 0x80000000)) {
		return 0;
	}
	if (wparam >= VK_LWIN && wparam <= VK_RWIN) {
		Unknown_4BC000("input", "", __FILE__, __LINE__, "Ignore (%4x, %4x)", wparam, lparam);
		return 0;
	}
	unsigned short flags = 0;
	unsigned short key = (unsigned short)wparam;
	LONG time = GetMessageTime();
	DWORD pos = GetMessagePos();
	POINT point;
	point.x = (short)LOWORD(pos);
	point.y = (short)HIWORD(pos);
	ScreenToClient(Unknown_BDAA7C, &point);
	RECT rect;
	GetClientRect(Unknown_BDAA7C, &rect);
	float x = (float)point.x / (float)(rect.right - rect.left - 1);
	float y = (float)point.y / (float)(rect.bottom - rect.top - 1);
	if (x < 0.0f) {
		x = 0.0f;
	} else if (x > 1.0f) {
		x = 1.0f;
	}
	if (y < 0.0f) {
		y = 0.0f;
	} else if (y > 1.0f) {
		y = 1.0f;
	}
	bool translate = !Unknown_B6EA58;
	int shift = ((unsigned)GetKeyState(VK_SHIFT) >> 15) & 1;
	int control = ((unsigned)GetKeyState(VK_CONTROL) >> 15) & 1;
	int alt = ((unsigned)GetKeyState(VK_MENU) >> 15) & 1;
	int caps = (GetKeyState(VK_CAPITAL) & 1) && Unknown_BDAAE8[wparam & 0xFFFF] == 1;
	int pad = (GetKeyState(VK_NUMLOCK) & 1) && Unknown_BDAAE8[wparam & 0xFFFF] == 2;
	int extended = (lparam >> 24) & 1;
	if (shift || (caps && !translate)) {
		flags = 0x100;
	}
	if (control) {
		flags |= 0x200;
	}
	if (alt) {
		flags |= 0x400;
	}
	if (lparam & 0x80000000) {
		flags |= 0x1000;
	}
	if (translate) {
		if (extended) {
			if (key == VK_RETURN) {
				key = 0x100;
			}
		} else {
			switch (wparam & 0xFFFF) {
				case VK_INSERT:
					key = VK_NUMPAD0;
					break;
				case VK_DELETE:
					key = VK_DECIMAL;
					break;
				case VK_END:
					key = VK_NUMPAD1;
					break;
				case VK_DOWN:
					key = VK_NUMPAD2;
					break;
				case VK_NEXT:
					key = VK_NUMPAD3;
					break;
				case VK_LEFT:
					key = VK_NUMPAD4;
					break;
				case VK_CLEAR:
					key = VK_NUMPAD5;
					break;
				case VK_RIGHT:
					key = VK_NUMPAD6;
					break;
				case VK_HOME:
					key = VK_NUMPAD7;
					break;
				case VK_UP:
					key = VK_NUMPAD8;
					break;
				case VK_PRIOR:
					key = VK_NUMPAD9;
					break;
			}
			if (pad) {
				flags |= 0x100;
				shift = 1;
			}
		}
	}
	KernelEventClass event(key, flags);
	event.Unknown04 = x;
	event.Unknown08 = y;
	event.Unknown0C = time;
	KeyboardClass::Queue->Add(event);


















































	Unknown_4BC000("input", "", __FILE__, __LINE__, "(%4x, %4x, %4x) => (%4x %4x) :  %c%c%c%c%c%c \n", message, wparam, lparam, key, flags,
		control ? 'C' : '-', alt ? 'A' : '-', shift ? 'S' : '-', caps ? 'U' : '-', pad ? 'N' : '-', extended ? 'E' : '-');
	return 0;
}

// NOT EXACT: register: the client keeps control in the second argument's slot and alt in ebp, ours the reverse (attempts.csv)
bool Win32KeyboardHandlerClass::Unknown_0C(int unknown1, int unknown2)
{
	if (!Unknown_42EE90()) {
		return false;
	}
	unsigned short key = (unsigned short)unknown1;
	if (key == VK_LBUTTON || key == VK_RBUTTON) {
		if (GetSystemMetrics(SM_SWAPBUTTON) == 1) {
			key = (key != VK_LBUTTON) ? VK_LBUTTON : VK_RBUTTON;
		}
	}
	int shift, control, alt;
	if ((unsigned short)unknown2 != 0) {
		shift = GetAsyncKeyState(VK_SHIFT);
		control = GetAsyncKeyState(VK_CONTROL);
		alt = GetAsyncKeyState(VK_MENU);
	} else {
		shift = 1;
		control = 1;
		alt = 1;
	}
	int state = GetAsyncKeyState(key) & ~1;
	return shift && control && alt && state;
}
/*
**	Reconstruction notes
**
**	client.exe: the keyboard handler 0x430490-0x430D40 (the reporter's __LINE__ 124 and 270, D25):
**	the constructor 0x430490, the destructor 0x430630 (0x430600 its scalar deleting one), Set_Repeat
**	0x4306D0, the handler's registration 0x430700 / 0x430750, the WM_KEYDOWN / WM_KEYUP /
**	WM_SYSKEYDOWN / WM_SYSKEYUP handler 0x4307A0 and the key test 0x430CA0 (NOT EXACT, register).
**	0x430D70 is KeyboardClass's destructor out of line (input.h). Not rebuilt: the part of the
**	file before 0x430490, if it is this file's: the window-message handler list (0x42F860-0x430450,
**	0x42F940 adds, 0x42FAC0 removes, 0x42FBA0 dispatches; programs/clientpoc/poc_window.cpp has
**	stand-ins) and input.h's drag tracker (0x42F360 / 0x42F3A0, the TrackMouseCounter asserts).
**	The handlers take (message, wParam, lParam); the list's declared type is two-argument (cdecl:
**	the cast is the client's call through it).
**	Forms that matched: GetKeyState's down bit as `((unsigned)GetKeyState(k) >> 15) & 1` (the
**	client's shr / and 1; a short or unsigned short shift gives sar or and 0xFFFF).
**	Unknown_B6EA58 starts true (initialized data 0xB6EA58 = 1): presses repeated by the keyboard
**	are kept; when it is false they are dropped and the numeric pad's keys are translated.
*/
