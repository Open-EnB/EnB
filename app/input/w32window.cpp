// Earth & Beyond client (reconstruction): app\input\w32window.cpp (PROVISIONAL name). Notes at the end of the file.
#include <windows.h>
#include "winmsg.h"

LRESULT CALLBACK Unknown_431DE0(HWND window, UINT message, WPARAM wparam, LPARAM lparam);	// NAME 1 [Unknown_431DE0, window, message, wparam, lparam]: neutral (D10)

// NOT EXACT: shape: the client branches on topmost for the ex-style argument (push 8 / push 0), ours computes it with neg / sbb / and (attempts.csv)
HWND Unknown_431CA0(HINSTANCE instance, int width, int height, bool topmost)	// NAME 1 [Unknown_431CA0, instance, width, height, topmost]: as main_client.cpp
{
	WNDCLASS wc;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = Unknown_431DE0;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = instance;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = NULL;
	wc.lpszMenuName = NULL;
	wc.lpszClassName = "G";
	RegisterClass(&wc);
	HWND window = CreateWindowEx(topmost ? WS_EX_TOPMOST : 0, "G", "Earth & Beyond", WS_CAPTION | WS_SYSMENU, 0, 0, width, height, NULL, NULL, instance, NULL);
	RECT client;
	RECT frame;
	GetClientRect(window, &client);
	GetWindowRect(window, &frame);
	if (client.right < frame.right - frame.left) {
		MoveWindow(window, frame.left, frame.top, frame.right - client.right - frame.left + width, frame.bottom - client.bottom - frame.top + height, TRUE);
	}
	SetFocus(window);
	return window;
}

LRESULT CALLBACK Unknown_431DE0(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
	int result = Unknown_42FBA0(message, wparam, lparam);
	if (result == -1) {
		return DefWindowProc(window, message, wparam, lparam);
	}
	return result;
}
/*
**	Reconstruction notes
**
**	client.exe: 0x431CA0-0x431E10, after w32mouse.cpp's functions (no __FILE__, no assert: the
**	file's name and place are provisional, D137): the main window (class 'G', title 'Earth &
**	Beyond', WS_CAPTION | WS_SYSMENU, WS_EX_TOPMOST when asked; the client area made width x height)
**	0x431CA0 NOT EXACT (shape) and its window procedure 0x431DE0 exact: the handler list's result,
**	else DefWindowProc. The object after it starts at 0x431E30 (the music, '-NOMUSIC').
**	Tried for the ex-style: == false, !topmost, WS_EX_LEFT, DWORD casts, an inline helper, a
**	static const: all compile to the branch-free form.
*/
