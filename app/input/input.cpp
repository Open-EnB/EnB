// Earth & Beyond client (reconstruction): app\input\input.cpp. Notes at the end of the file.
#include "input.h"
#include "w32kbd.h"
#include "w32mouse.h"
#include "..\shared\unknown_debug.h"
#include "..\..\wwlib\ini.h"

InputQueueClass *Unknown_BDA968;
unsigned char Unknown_BDA96C[INPUT_KEY_COUNT];
KeyboardClass *Unknown_BDAA70;
MouseClass *Unknown_BDAA74;
InputUnknownBDAA78Class *Unknown_BDAA78;
HWND Unknown_BDAA7C;
int TrackMouseCounter;
int KeyRepeatCount;
KeyQueueClass *KeyboardClass::Queue;
KeyQueueClass *MouseClass::Queue;

int Unknown_42E390(int unknown1, int unknown2)
{
	PostQuitMessage(0);
	return -1;
}

int Unknown_42E3B0(int unknown1, int unknown2)
{
	if (unknown2) {
		WINDOWPLACEMENT placement;
		placement.length = sizeof(placement);
		GetWindowPlacement(Unknown_BDAA7C, &placement);
		SetWindowPos(Unknown_BDAA7C, HWND_TOP, placement.rcNormalPosition.left, placement.rcNormalPosition.top, placement.rcNormalPosition.right, placement.rcNormalPosition.bottom, 0);
		SetCapture(Unknown_BDAA7C);
		return 0;
	} else {
		WINDOWPLACEMENT placement;
		placement.length = sizeof(placement);
		GetWindowPlacement(Unknown_BDAA7C, &placement);
		SetWindowPos(Unknown_BDAA7C, HWND_BOTTOM, placement.rcNormalPosition.left, placement.rcNormalPosition.top, placement.rcNormalPosition.right, placement.rcNormalPosition.bottom, 0);
		SetCapture(Unknown_BDAA7C);
		ReleaseCapture();
		return 0;
	}
}

// NOT EXACT: inline-budget: the client calls the queue element's constructor 0x42ED50 in new[]'s loop (EH state 2), ours inlines it (attempts.csv)
void Unknown_42E4A0(HWND window)
{
	INIClass ini;
	ini.Load("..\\data\\input.ini");
	char section[MAX_PATH];
	ini.Get_String("INPUT", "SETTINGS", "", section, sizeof(section));
	Unknown_BDAA7C = window;
	Unknown_BDA968 = new KeyQueueClass;
	if (ini.Get_Bool(section, "DIRECTX_KEYBOARD", false)) {
		Unknown_BDAA70 = new InputUnknownAE7160Class((KeyQueueClass *)Unknown_BDA968);
	} else {
		Unknown_BDAA70 = new Win32KeyboardHandlerClass((KeyQueueClass *)Unknown_BDA968);
	}
	if (ini.Get_Bool(section, "DIRECTX_MOUSE", false)) {
		Unknown_BDAA74 = new InputUnknownAE7134Class((KeyQueueClass *)Unknown_BDA968);
	} else {
		Unknown_BDAA74 = new Win32MouseHandlerClass((KeyQueueClass *)Unknown_BDA968);
	}
	if (ini.Get_Bool(section, "DIRECTX_JOYSTICK", false)) {
		Unknown_BDAA78 = new InputUnknownBDAA78Class;
	} else {
		Unknown_BDAA78 = new InputUnknownBDAA78Class;
	}
	memset(Unknown_BDA96C, 0, sizeof(Unknown_BDA96C));
	Unknown_42F940(WM_DESTROY, Unknown_42E390);
	KeyRepeatCount = 1;
	Unknown_BDAA70->Set_Repeat(true);
}

void Unknown_42E7B0()
{
	Unknown_42FAC0(WM_DESTROY, Unknown_42E390);
	if (Unknown_BDA968) {
		delete Unknown_BDA968;
	}
	if (Unknown_BDAA70) {
		delete Unknown_BDAA70;
	}
	if (Unknown_BDAA74) {
		delete Unknown_BDAA74;
	}
	if (Unknown_BDAA78) {
		delete Unknown_BDAA78;
	}
}

void Unknown_42E8A0(KernelEventClass *event)
{
	if (TrackMouseCounter > 0) {
		if (Unknown_BDA968->Peek(*event)) {
			Unknown_BDA968->Get(*event);
			return;
		}
		event->Key = 0;
		event->Flags = 0;
		Unknown_BDAA74->Get_Position(&event->Unknown04, &event->Unknown08);
		event->Unknown0C = GetTickCount();
		return;
	}
	while (!Unknown_42EA80(event)) {
	}
	Unknown_BDA968->Get(*event);
}

bool Unknown_42EA80(KernelEventClass *event)
{
	if (Unknown_BDA968->Peek(*event)) {
		return true;
	}
	*event = KernelEventClass();
	return TrackMouseCounter > 0;
}

char Unknown_42EB80(KernelEventClass *event)
{
	if (event->Flags & 0x1000) {
		return 0;
	}
	if (event->Flags & 0x100) {
		Unknown_BDA96C[VK_SHIFT] = 0x80;
	}
	if (event->Flags & 0x200) {
		Unknown_BDA96C[VK_CONTROL] = 0x80;
	}
	if (event->Flags & 0x400) {
		Unknown_BDA96C[VK_MENU] = 0x80;
	}
	char buffer[12];
	UINT scan = MapVirtualKey(event->Key & 0xFF, 0);
	int result = ToAscii(event->Key & 0xFF, scan, Unknown_BDA96C, (LPWORD)buffer, 0);
	if (event->Flags & 0x100) {
		Unknown_BDA96C[VK_SHIFT] = 0;
	}
	if (event->Flags & 0x200) {
		Unknown_BDA96C[VK_CONTROL] = 0;
	}
	if (event->Flags & 0x400) {
		Unknown_BDA96C[VK_MENU] = 0;
	}
	if (result != 1) {
		return 0;
	}
	return buffer[0];
}

void Unknown_42ECF0(float *x, float *y)
{
	Unknown_BDAA74->Get_Position(x, y);
}

void Unknown_42ED20(float x, float y)
{
	Unknown_BDAA74->Set_Position(x, y);
}

void Unknown_42ED80()
{
	Unknown_BDAA74->Set_Position(0.5f, 0.5f);
}

void Unknown_42EDB0()
{
	if (GetForegroundWindow() == Unknown_BDAA7C) {
		KernelClass *kernel = KernelClass::Get_Instance();
		if (kernel == NULL || (kernel->Render_Enabled && kernel->Device_Ok)) {
			Unknown_BDAA74->Update();
		}
	}
}


















































































































bool Unknown_42EE00(bool enable)
{
	if (enable) {
		KeyRepeatCount++;
	} else {
		KeyRepeatCount--;
		if (KeyRepeatCount > 0) {
			return false;
		}
		FAIL_IF(KeyRepeatCount) KeyRepeatCount = 0;
	}
	return Unknown_BDAA70->Set_Repeat(enable);
}

bool Unknown_42EE90()
{
	return GetForegroundWindow() == Unknown_BDAA7C;
}

bool KeyQueueClass::Add(KernelEventClass &event)
{
	if (event.Key < sizeof(KeyStates)) {
		unsigned int state = KeyStates[event.Key];
		if ((state & 0x10000) == 0x10000) {
			if (event.Flags & 0x1000) {
				event.Flags = state | 0x1000;
				KeyStates[event.Key] = 0;
			} else {
				event.Flags = state;
			}
		} else if (!(event.Flags & 0x1000)) {
			KeyStates[event.Key] = event.Flags | 0x10000;
		}
	}
	return InputQueueClass::Add(event);
}
/*
**	Reconstruction notes
**
**	client.exe: the window 0x42E390-0x42EF60 (FAIL_IF 'KeyRepeatCount' on line 298, D25), then the
**	inline functions of input.h out of line to 0x42F340: the null keyboard's and mouse's
**	virtuals 0x42EF90-0x42F170, the destructors 0x42F010-0x42F230 and 0x42F290-0x42F2C0, the
**	queue's 0x42F1C0 / 0x42F2F0 and the compiler's vector constructor iterator 0x42F250.
**	0x42E3B0 (the window to the top or the bottom) has no caller and no reference in the client.
**	0x42ED50, between 0x42ED20 and 0x42ED80, is the queue element's constructor (KernelEventClass's
**	zeroing): the client calls it from Init's new[] loop; ours inlines it there, so Init is NOT
**	EXACT and 0x42ED50 is not emitted. A non-inline constructor gives Init exactly (a probe with a
**	stand-in element type); defined in this file it is inlined again (VC6 /Ob2 inlines functions
**	defined after their callers). #pragma inline_depth, a template queue, the allocation in an
**	inline helper and extra inline calls before it did not move it.
**	The key table's bound is sizeof (0x40C bytes) where the table has 0x103 dwords: the client's.
**	Forms that matched: 0x42E3B0 with a WINDOWPLACEMENT per branch (one shared local hoists the
**	length store above the test); 0x42E7B0 with `if (p) delete p;` (the joystick's plain delete
**	has the client's test); 0x42E8A0 with the peek and the take both written in the
**	Unknown_BDAA80 > 0 branch; 0x42EB80 with a 12-byte char buffer and `Key & 0xFF`; 0x42EE00
**	with one Set_Repeat call after the if / else; 0x42EEC0 with the state read into a local.
*/
