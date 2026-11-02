// Earth & Beyond client (reconstruction): app\input\w32mouse.cpp. Notes at the end of the file.
#include "w32mouse.h"
#include "w32kbd.h"
#include "..\shared\unknown_debug.h"
#include "..\..\wwlib\wwstring.h"
#include "..\..\wwmath\wwmath.h"

#ifndef WM_MOUSEWHEEL
#define WM_MOUSEWHEEL	0x020A	// VC6's winuser.h has it only for _WIN32_WINNT >= 0x0400
#endif
#ifndef WHEEL_DELTA
#define WHEEL_DELTA		120
#endif

void Unknown_Debug_Print(const char *text);			// NAME 1 [Unknown_Debug_Print]: 0x509E30 (dbgmgr.cpp), the log line

static Win32MouseHandlerClass *Mouse;		// 0xBDAACC (the client's assert 'Mouse == NULL')
static bool Unknown_BDAC2C;				// the cursor is shown  // NAME 1 [Unknown_BDAC2C]: neutral (D10)
static int Unknown_BDAC30;					// the wheel's delta not yet sent  // NAME 1 [Unknown_BDAC30]: neutral (D10)

int Unknown_4312C0(int message, int wparam);	// NAME 1 [Unknown_4312C0, message, wparam]: neutral (D10)
int Unknown_431470(int message, int wparam);	// NAME 1 [Unknown_431470, message, wparam]: neutral (D10)
int Unknown_4314C0(unsigned short button, bool up);	// NAME 1 [Unknown_4314C0, button, up]: neutral (D10)
int Unknown_4316A0(unsigned int message, int wparam);	// NAME 1 [Unknown_4316A0, message, wparam]: neutral (D10)










Win32MouseHandlerClass::Win32MouseHandlerClass(KeyQueueClass *queue) : MouseClass(queue)
{
	G_ASSERT(Mouse == NULL);
	Mouse = this;
	Unknown_04();
	int count = ShowCursor(FALSE);
	StringClass text;
	text.Format("ShowCursor(FALSE) returned %d\n", count);
	Unknown_Debug_Print(text);
	Unknown_BDAC2C = false;
}

Win32MouseHandlerClass::~Win32MouseHandlerClass()
{
	Unknown_08();
	Mouse = NULL;
}

void Win32MouseHandlerClass::Unknown_04()
{
	Unknown_42F940(WM_LBUTTONDOWN, (int (*)(int, int))Unknown_4316A0);
	Unknown_42F940(WM_LBUTTONDBLCLK, (int (*)(int, int))Unknown_4316A0);
	Unknown_42F940(WM_LBUTTONUP, (int (*)(int, int))Unknown_4316A0);
	Unknown_42F940(WM_RBUTTONDOWN, (int (*)(int, int))Unknown_4316A0);
	Unknown_42F940(WM_RBUTTONDBLCLK, (int (*)(int, int))Unknown_4316A0);
	Unknown_42F940(WM_RBUTTONUP, (int (*)(int, int))Unknown_4316A0);
	Unknown_42F940(WM_MBUTTONDOWN, (int (*)(int, int))Unknown_4316A0);
	Unknown_42F940(WM_MBUTTONDBLCLK, (int (*)(int, int))Unknown_4316A0);
	Unknown_42F940(WM_MBUTTONUP, (int (*)(int, int))Unknown_4316A0);
	Unknown_42F940(WM_NCLBUTTONDOWN, (int (*)(int, int))Unknown_4316A0);
	Unknown_42F940(WM_NCLBUTTONDBLCLK, (int (*)(int, int))Unknown_4316A0);
	Unknown_42F940(WM_NCLBUTTONUP, (int (*)(int, int))Unknown_4316A0);
	Unknown_42F940(WM_NCRBUTTONDOWN, (int (*)(int, int))Unknown_4316A0);
	Unknown_42F940(WM_NCRBUTTONDBLCLK, (int (*)(int, int))Unknown_4316A0);
	Unknown_42F940(WM_NCRBUTTONUP, (int (*)(int, int))Unknown_4316A0);
	Unknown_42F940(WM_NCMBUTTONDOWN, (int (*)(int, int))Unknown_4316A0);
	Unknown_42F940(WM_NCMBUTTONDBLCLK, (int (*)(int, int))Unknown_4316A0);
	Unknown_42F940(WM_NCMBUTTONUP, (int (*)(int, int))Unknown_4316A0);
	Unknown_42F940(WM_MOUSEWHEEL, (int (*)(int, int))Unknown_4316A0);
	Unknown_42F940(WM_MOVE, Unknown_431470);
	Unknown_42F940(WM_ACTIVATEAPP, Unknown_4312C0);
}

void Win32MouseHandlerClass::Unknown_08()
{
	Unknown_42FAC0(WM_LBUTTONDOWN, (int (*)(int, int))Unknown_4316A0);
	Unknown_42FAC0(WM_LBUTTONDBLCLK, (int (*)(int, int))Unknown_4316A0);
	Unknown_42FAC0(WM_LBUTTONUP, (int (*)(int, int))Unknown_4316A0);
	Unknown_42FAC0(WM_RBUTTONDOWN, (int (*)(int, int))Unknown_4316A0);
	Unknown_42FAC0(WM_RBUTTONDBLCLK, (int (*)(int, int))Unknown_4316A0);
	Unknown_42FAC0(WM_RBUTTONUP, (int (*)(int, int))Unknown_4316A0);
	Unknown_42FAC0(WM_MBUTTONDOWN, (int (*)(int, int))Unknown_4316A0);
	Unknown_42FAC0(WM_MBUTTONDBLCLK, (int (*)(int, int))Unknown_4316A0);
	Unknown_42FAC0(WM_MBUTTONUP, (int (*)(int, int))Unknown_4316A0);
	Unknown_42FAC0(WM_NCLBUTTONDOWN, (int (*)(int, int))Unknown_4316A0);
	Unknown_42FAC0(WM_NCLBUTTONDBLCLK, (int (*)(int, int))Unknown_4316A0);
	Unknown_42FAC0(WM_NCLBUTTONUP, (int (*)(int, int))Unknown_4316A0);
	Unknown_42FAC0(WM_NCRBUTTONDOWN, (int (*)(int, int))Unknown_4316A0);
	Unknown_42FAC0(WM_NCRBUTTONDBLCLK, (int (*)(int, int))Unknown_4316A0);
	Unknown_42FAC0(WM_NCRBUTTONUP, (int (*)(int, int))Unknown_4316A0);
	Unknown_42FAC0(WM_NCMBUTTONDOWN, (int (*)(int, int))Unknown_4316A0);
	Unknown_42FAC0(WM_NCMBUTTONDBLCLK, (int (*)(int, int))Unknown_4316A0);
	Unknown_42FAC0(WM_NCMBUTTONUP, (int (*)(int, int))Unknown_4316A0);
	Unknown_42FAC0(WM_MOUSEWHEEL, (int (*)(int, int))Unknown_4316A0);
	Unknown_42FAC0(WM_MOVE, Unknown_431470);
	Unknown_42FAC0(WM_ACTIVATEAPP, Unknown_4312C0);
}

int Unknown_431270(int message, int wparam)	// NAME 1 [Unknown_431270, message, wparam]: neutral (D10)
{
	SetFocus(Unknown_BDAA7C);
	SetCapture(Unknown_BDAA7C);
	return -1;
}

int Unknown_4312A0(int message, int wparam)	// NAME 1 [Unknown_4312A0, message, wparam]: neutral (D10)
{
	SetFocus(Unknown_BDAA7C);
	return -1;
}

int Unknown_4312C0(int message, int wparam)
{
	if (wparam) {
		Unknown_Debug_Print("Win32MouseHandlerClass: capturing mouse input.\n");
		SetCapture(Unknown_BDAA7C);
		if (Unknown_BDAC2C) {
			int count = ShowCursor(FALSE);
			StringClass text;
			text.Format("ShowCursor(FALSE)2 returned %d\n", count);
			Unknown_Debug_Print(text);
			Unknown_BDAC2C = false;
		}
	} else {
		Unknown_Debug_Print("Win32MouseHandlerClass: releasing mouse input.\n");
		ReleaseCapture();
		if (!Unknown_BDAC2C) {
			int count = ShowCursor(TRUE);
			StringClass text;
			text.Format("ShowCursor(TRUE) returned %d\n", count);
			Unknown_Debug_Print(text);
			Unknown_BDAC2C = true;
		}
	}
	return -1;
}

int Unknown_431470(int message, int wparam)
{
	return -1;
}

int Unknown_431490(int message, int wparam)	// NAME 1 [Unknown_431490, message, wparam]: neutral (D10)
{
	Unknown_Debug_Print("Win32MouseHandlerClass: hiding cursor.\n");
	SetCursor(NULL);
	return 1;
}

int Unknown_4314C0(unsigned short button, bool up)
{
	if (!Unknown_42EE90()) {
		return 0;
	}
	unsigned short flags = up ? 0x1000 : 0;
	LONG time = GetMessageTime();
	DWORD pos = GetMessagePos();
	POINT point;
	point.x = LOWORD(pos);
	point.y = HIWORD(pos);
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
	if (GetKeyState(VK_SHIFT) & 0x8000) {
		flags |= 0x100;
	}
	if (GetKeyState(VK_CONTROL) & 0x8000) {
		flags |= 0x200;
	}
	if (GetKeyState(VK_MENU) & 0x8000) {
		flags |= 0x400;
	}
	KernelEventClass event(button, flags);
	event.Unknown04 = x;
	event.Unknown08 = y;
	event.Unknown0C = time;
	MouseClass::Queue->Add(event);
	return 0;
}

int Unknown_4316A0(unsigned int message, int wparam)
{
	if (!Unknown_42EE90()) {
		return 0;
	}
	int result = -1;
	switch (message) {
		case WM_LBUTTONDOWN:
		case WM_NCLBUTTONDOWN:
			result = Unknown_4314C0(1, false);
			break;
		case WM_LBUTTONUP:
		case WM_NCLBUTTONUP:
			result = Unknown_4314C0(1, true);
			break;
		case WM_LBUTTONDBLCLK:
		case WM_NCLBUTTONDBLCLK:
			Unknown_4314C0(1, false);
			result = Unknown_4314C0(1, true);
			break;
		case WM_RBUTTONDOWN:
		case WM_NCRBUTTONDOWN:
			result = Unknown_4314C0(2, false);
			break;
		case WM_RBUTTONUP:
		case WM_NCRBUTTONUP:
			result = Unknown_4314C0(2, true);
			break;
		case WM_RBUTTONDBLCLK:
		case WM_NCRBUTTONDBLCLK:
			Unknown_4314C0(2, false);
			result = Unknown_4314C0(2, true);
			break;
		case WM_MBUTTONDOWN:
		case WM_NCMBUTTONDOWN:
			result = Unknown_4314C0(4, false);
			break;
		case WM_MBUTTONUP:
		case WM_NCMBUTTONUP:
			result = Unknown_4314C0(4, true);
			break;
		case WM_MBUTTONDBLCLK:
		case WM_NCMBUTTONDBLCLK:
			Unknown_4314C0(4, false);
			result = Unknown_4314C0(4, true);
			break;
		case WM_MOUSEWHEEL:
			Unknown_BDAC30 += wparam >> 16;
			while (Unknown_BDAC30 >= WHEEL_DELTA) {
				Unknown_BDAC30 -= WHEEL_DELTA;
				Unknown_4314C0(0x101, false);
			}
			while (Unknown_BDAC30 <= -WHEEL_DELTA) {
				Unknown_BDAC30 += WHEEL_DELTA;
				Unknown_4314C0(0x102, false);
			}
			return 0;
		default:
			break;
	}
	Unknown_BDAC30 = 0;
	return result;
}

void Win32MouseHandlerClass::Get_Pixel_Position(int *x, int *y)
{
	POINT point;
	GetCursorPos(&point);
	ScreenToClient(Unknown_BDAA7C, &point);
	*x = point.x;
	*y = point.y;
}

void Win32MouseHandlerClass::Get_Position(float *x, float *y)
{
	POINT point;
	GetCursorPos(&point);
	ScreenToClient(Unknown_BDAA7C, &point);
	*x = (float)point.x;
	*y = (float)point.y;
	RECT rect;
	GetClientRect(Unknown_BDAA7C, &rect);
	*x = (float)point.x / (float)(rect.right - rect.left - 1);
	*y = (float)point.y / (float)(rect.bottom - rect.top - 1);
	*x = WWMath::Clamp(*x, 0.0f, 1.0f);
	*y = WWMath::Clamp(*y, 0.0f, 1.0f);
}

// NOT EXACT: register: x and the clamp flag in ecx / al in the client, eax / cl in ours (attempts.csv)
void Win32MouseHandlerClass::Update()
{
	RECT rect;
	GetClientRect(Unknown_BDAA7C, &rect);
	int x, y;
	Get_Pixel_Position(&x, &y);
	bool moved = x < rect.left;
	if (moved) {
		x = rect.left;
	} else {
		moved = x > rect.right;
		if (moved) {
			x = rect.right;
		}
	}
	if (y < rect.top) {
		y = rect.top;
		Set_Pixel_Position(x, y);
	} else if (y > rect.bottom) {
		y = rect.bottom;
		Set_Pixel_Position(x, y);
	} else if (moved) {
		Set_Pixel_Position(x, y);
	}
}

void Win32MouseHandlerClass::Set_Position(float x, float y)
{
	RECT rect;
	GetClientRect(Unknown_BDAA7C, &rect);
	POINT point;
	point.x = (int)((rect.right - rect.left) * x + rect.left);
	point.y = (int)((rect.bottom - rect.top) * y + rect.top);
	ClientToScreen(Unknown_BDAA7C, &point);
	SetCursorPos(point.x, point.y);
}

void Win32MouseHandlerClass::Set_Pixel_Position(int x, int y)
{
	POINT point;
	point.x = x;
	point.y = y;
	ClientToScreen(Unknown_BDAA7C, &point);
	SetCursorPos(point.x, point.y);
}
/*
**	Reconstruction notes
**
**	client.exe: the mouse handler 0x430D90-0x431C80 (G_ASSERT 'Mouse == NULL' line 37, D25): the
**	constructor 0x430D90, the destructor 0x430EE0 (0x430EB0 its scalar deleting one), the
**	handlers' registration 0x430F50 / 0x4310E0, the message handlers 0x431270 / 0x4312A0 /
**	0x431490 (no reference in the client), WM_ACTIVATEAPP 0x4312C0, WM_MOVE 0x431470, the button
**	event 0x4314C0 and the button / wheel messages 0x4316A0, the pixel and 0..1 positions 0x431900 /
**	0x431950 / 0x431B70 / 0x431C30, and Update 0x431AA0 (NOT EXACT, register). 0x431A80 is
**	MouseClass's destructor out of line (input.h). The next object starts at 0x431CA0 (the main
**	window).
**	Forms that matched: Get_Position clamps through WWMath::Clamp (the client's fld / fcom / fld
**	constant), 0x4314C0 inline; 0x4316A0 with `result = -1` before the switch and the wheel's
**	`wparam >> 16` against WHEEL_DELTA (>= 120, <= -120).
*/
