// Earth & Beyond client (reconstruction): app\input\input.cpp. Notes at the end of the file.
#include "input.h"
#include "w32kbd.h"
#include "w32mouse.h"
#include "..\shared\g_assert.h"
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

static inline KeyQueueClass *New_Key_Queue_Raw()	// NAME 1 [New_Key_Queue_Raw]: no code of its own (D185 variant: two inline layers keep the element constructor 0x42ED50 out of line)
{
	return new KeyQueueClass;
}

static inline KeyQueueClass *New_Key_Queue()		// NAME 1 [New_Key_Queue]: as New_Key_Queue_Raw
{
	return New_Key_Queue_Raw();
}

void Unknown_42E4A0(HWND window)
{
	INIClass ini;
	ini.Load("..\\data\\input.ini");
	char section[MAX_PATH];
	ini.Get_String("INPUT", "SETTINGS", "", section, sizeof(section));
	Unknown_BDAA7C = window;
	Unknown_BDA968 = New_Key_Queue();
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

bool Unknown_42EC60(int unknown1, int unknown2)	// NAME 1 [unknown1, unknown2]: neutral (D10), as KeyboardClass::Unknown_0C
{
	return Unknown_BDAA70->Unknown_0C(unknown1, unknown2);
}

float Unknown_42EC90()
{
	float x;
	float y;
	Unknown_BDAA74->Get_Position(&x, &y);
	return(x);
}

float Unknown_42ECC0()
{
	float x;
	float y;
	Unknown_BDAA74->Get_Position(&x, &y);
	return(y);
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
**	zeroing): the client calls it from Init's new[] loop. Init (0x42E4A0) is exact with the queue's
**	new behind two no-code inline layers (D185's variant, 2026-11-02): the extra depth spends the
**	inline budget so the element constructor stays out of line and 0x42ED50 is emitted. One layer,
**	#pragma inline_depth, a template queue and extra inline calls before it did not move it.
**	0x42EC60 (the button-down query: the keyboard's slot +0x0C) is FontSys 0x6CD6F0's and
**	the gadgets' click test. It and the variant's helpers took their lines from the blank padding before
**	0x42EE00, whose FAIL_IF stays on line 298.
**	The key table's bound is sizeof (0x40C bytes) where the table has 0x103 dwords: the client's.
**	Forms that matched: 0x42E3B0 with a WINDOWPLACEMENT per branch (one shared local hoists the
**	length store above the test); 0x42E7B0 with `if (p) delete p;` (the joystick's plain delete
**	has the client's test); 0x42E8A0 with the peek and the take both written in the
**	Unknown_BDAA80 > 0 branch; 0x42EB80 with a 12-byte char buffer and `Key & 0xFF`; 0x42EE00
**	with one Set_Repeat call after the if / else; 0x42EEC0 with the state read into a local.
*/
