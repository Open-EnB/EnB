// PLACEMENT: provisional (D137): sits four functions after optdlg.cpp's servers.ini assert (0x8476C0); string "r" at 0xBB0430 precedes pal_mgr.cpp (0x848100/0x848250); may be optdlg.cpp's tail or an interim dialog helper

#include "wwlib/win.h"
#include <commctrl.h>
#include <stdio.h>
#include <string>
#include "unknown_847d20.h"

static HBRUSH Unknown_BF3DB4 = NULL;		// NAME 1 [Unknown_BF3DB4]: background brush for the EULA / ROC static text (0xBF3DB4)

static BOOL CALLBACK Unknown_847FE0(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);		// NAME 1 [Unknown_847FE0]: dialog procedure for the EULA / ROC dialog (0x847FE0)

// @0x847D20
bool Unknown_847D20(const char *filename)		// NAME 1 [Unknown_847D20]: the text file in a dialog (EULA, ROC); false when declined  // NAME 1 [filename]: what WinMain passes
{
	std::string text;
	InitCommonControls();
	FILE *fp = fopen(filename, "r");
	if (fp) {
		char buf[260];
		while (fgets(buf, 255, fp)) {
			int len = strlen(buf);
			for (int i = 1; i <= len; i++) {
				if (buf[len - i] == '\r' || buf[len - i] == '\n') {
					buf[len - i] = '\0';
				} else {
					break;
				}
			}
			strcat(buf, "\r\n");
			text += buf;
		}
		fclose(fp);
	}
	if (DialogBoxParamA(ProgramInstance, MAKEINTRESOURCE(0x70), NULL, Unknown_847FE0, (LPARAM)text.c_str()) == 2) {
		return false;
	}
	return true;
}

// @0x847FE0
// NOT EXACT: inline-budget
static BOOL CALLBACK Unknown_847FE0(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	case WM_DESTROY:
		if (Unknown_BF3DB4) {
			DeleteObject(Unknown_BF3DB4);
			Unknown_BF3DB4 = NULL;
		}
		return FALSE;

	case WM_ACTIVATE:
		break;

	case WM_INITDIALOG:
		SendDlgItemMessageA(hDlg, 1037, WM_SETTEXT, 0, lParam);
		return TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == 2) {
			EndDialog(hDlg, 2);
			return FALSE;
		}
		if (LOWORD(wParam) == 1) {
			EndDialog(hDlg, 1);
		}
		break;

	case WM_CTLCOLORSTATIC:
		if (!Unknown_BF3DB4) {
			Unknown_BF3DB4 = CreateSolidBrush(RGB(255, 255, 255));
		}
		SetTextColor((HDC)wParam, RGB(0, 0, 0));
		SetBkColor((HDC)wParam, RGB(255, 255, 255));
		return (BOOL)Unknown_BF3DB4;
	}

	return FALSE;
}
