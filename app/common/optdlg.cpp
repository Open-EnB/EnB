// Earth & Beyond client (reconstruction): app\Common\optdlg.cpp, the options dialog (WinMain's -MENU, 0x846C90:
// a tab control 0x3FB with one page per entry of the page table, each page a table of controls set from and saved
// to startup.arg). So far the folder browser's callback 0x846B80, 0x846CD0, 0x846D10, the dialog procedure 0x8472E0 and
// 0x847CD0 (exact), 0x846FC0 and the page procedures 0x847630, 0x8476C0, 0x847BB0, 0x847C40 (NOT EXACT). Notes at the end.
#include "wwlib/win.h"
#include <commctrl.h>
#include <shlobj.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include "argv.h"
#include "ffactory.h"
#include "wwfile.h"
#include "..\..\wwlib\ini.h"
#include "..\..\wwlib\nstrdup.h"
#include "..\shared\g_assert.h"

// One control of an options page (0x50 bytes; a table ends with Type 3)
struct OptionChoiceStruct	// NAME 1: neutral (D10)
{
	const char *Display;								// +0x00 (the combo box's text; NULL ends the list)  // NAME 1: neutral
	const char *Value;									// +0x04 (the value written to startup.arg)  // NAME 1: neutral
};

struct OptionControlStruct	// NAME 1: neutral (D10)
{
	int Id;												// +0x00 (the dialog item)  // NAME 1: neutral
	char Name[0x44];									// +0x04 (the command-line option, '-LOGIN')  // NAME 1: neutral
	int Type;											// +0x48 (0 check box, 1 edit, 2 combo box, 3 end of table)  // NAME 1: neutral
	OptionChoiceStruct *Choices;						// +0x4C (the combo box's entries)  // NAME 1: neutral
};

// One page of the tab control (0x10 bytes)
struct OptionPageStruct	// NAME 1: neutral (D10)
{
	const char *Name;									// +0x00 (the tab's text)  // NAME 1: neutral
	int Id;												// +0x04 (the page's dialog template)  // NAME 1: neutral
	DLGPROC Proc;										// +0x08  // NAME 1: neutral
	HWND Window;										// +0x0C  // NAME 1: neutral
};

static BOOL CALLBACK Unknown_847630(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);	// NAME 1 [Unknown_847630]: the Account page's procedure

static OptionPageStruct Unknown_BAF730[] = {	// NAME 1 [Unknown_BAF730]: neutral (D10), the page table
	{ "Account", 111, Unknown_847630, NULL },
};

static const char *Unknown_BAF740 = "startup.arg";	// NAME 1 [Unknown_BAF740]: neutral (D10)

static OptionControlStruct Unknown_BAF748[] = {	// NAME 1 [Unknown_BAF748]: neutral (D10), the Account page's controls
	{ 1000, "-LOGIN", 1, NULL },
	{ 1001, "-PASSWORD", 1, NULL },
	{ -1, "", 3, NULL },
};

static OptionChoiceStruct Unknown_BF35AC[256];	// NAME 1 [Unknown_BF35AC]: neutral (D10), the servers from servers.ini (0x8476C0)

static OptionControlStruct Unknown_BAF838[] = {	// NAME 1 [Unknown_BAF838]: neutral (D10), the server page's controls (0x8476C0)
	{ 1020, "-s", 0, NULL },
	{ 1021, "-c", 0, NULL },
	{ 1041, "-g", 0, NULL },
	{ 1027, "-TASK", 0, NULL },
	{ 1022, "-SERVER_ADDR", 2, Unknown_BF35AC },
	{ 1044, "-GLOBAL_ADDR", 1, NULL },
	{ 1029, "-TEST_PATH", 1, NULL },
	{ 1036, "-ENABLE_MESSAGES", 0, NULL },
	{ 1037, "-NO_AUTHAUTH", 0, NULL },
	{ -1, "", 3, NULL },
};

static OptionControlStruct Unknown_BAFB58[] = {	// NAME 1 [Unknown_BAFB58]: neutral (D10), the 0x847BB0 page's controls
	{ 1007, "-ORACLE", 0, NULL },
	{ 1039, "-SHUTDOWN_EMPTY", 0, NULL },
	{ 1027, "-STATS", 0, NULL },
	{ 1031, "-LOGS", 0, NULL },
	{ 1032, "-PROXY_ADDR", 1, NULL },
	{ 1033, "-PROXY_PORT", 1, NULL },
	{ -1, "", 3, NULL },
};

static OptionChoiceStruct Unknown_BAFD88[] = {	// NAME 1 [Unknown_BAFD88]: neutral (D10), -PROTOCOL's choices
	{ "", "" },
	{ "Memory", "MEMORY" },
	{ "TCP", "TCP" },
	{ "UDP", "UDP" },
	{ NULL, NULL },
};

static OptionControlStruct Unknown_BAFDB0[] = {	// NAME 1 [Unknown_BAFDB0]: neutral (D10), the 0x847C40 page's controls
	{ 1026, "-NOSTARTUPMOVIE", 0, NULL },
	{ 1012, "-NOPROJECTORS", 0, NULL },
	{ 1009, "-NOSHADOWS", 0, NULL },
	{ 1010, "-NOMUSIC", 0, NULL },
	{ 1008, "-NOAUDIO", 0, NULL },
	{ 1023, "-FASTSTART", 0, NULL },
	{ 1024, "-NOCLASSRESTRICTIONS", 0, NULL },
	{ 1027, "-SUPERCLIENT", 0, NULL },
	{ 1005, "-SECT", 1, NULL },
	{ 1013, "-PROTOCOL", 2, Unknown_BAFD88 },
	{ -1, "", 3, NULL },
};

static void Unknown_846D10(OptionControlStruct *control, HWND dialog);	// NAME 1 [Unknown_846D10]: neutral (D10), one control set from startup.arg's options  // NAME 1 [control, dialog]: neutral

// @0x846B80
int CALLBACK Unknown_846B80(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)	// NAME 1 [Unknown_846B80]: neutral (D10), the folder browser's callback (BFFM_INITIALIZED selects the folder lpData names)
{
	if (uMsg == BFFM_INITIALIZED) {
		char path[256];										// NAME 1 [path]: neutral
		strcpy(path, (const char *)lpData);
		while (strlen(path) != 0 && !isgraph(path[strlen(path) - 1])) {
			path[strlen(path) - 1] = '\0';
		}
		if (strlen(path) != 0) {
			SendMessage(hwnd, BFFM_SETSELECTION, TRUE, (LPARAM)path);
		}
	}
	return 0;
}

// @0x846CD0
void Unknown_846CD0(OptionControlStruct *controls, HWND dialog)	// NAME 1 [Unknown_846CD0]: neutral (D10), a page's controls set (the page procedures inline it)  // NAME 1 [controls, dialog]: neutral
{
	for (int i = 0; controls[i].Type != 3; i++) {
		Unknown_846D10(&controls[i], dialog);
	}
}

#pragma auto_inline(off)
// @0x846D10
static void Unknown_846D10(OptionControlStruct *control, HWND dialog)
{
	ArgvClass argv(false, true);
	if (control->Type == 3) {
		return;
	}
	if (control->Type == 0) {
		if (argv.Find(control->Name)) {
			SendDlgItemMessage(dialog, control->Id, BM_SETCHECK, 1, 0);
		}
	} else if (control->Type == 1) {
		const char *value = argv.Find_Value(control->Name);	// NAME 1 [value]: neutral
		if (value != NULL) {
			char text[260];								// NAME 1 [text]: neutral
			strcpy(text, value);
			SendDlgItemMessage(dialog, control->Id, WM_SETTEXT, 0, (LPARAM)text);
		}
	} else if (control->Type == 2) {
		const char *value = argv.Find_Value(control->Name);	// NAME 1 [value]: neutral
		SendDlgItemMessage(dialog, control->Id, CB_RESETCONTENT, 0, 0);
		if (control->Choices != NULL) {
			bool found = false;							// NAME 1 [found]: neutral
			int i;
			for (i = 0; control->Choices[i].Display != NULL; i++) {
				SendDlgItemMessage(dialog, control->Id, CB_INSERTSTRING, (WPARAM)-1, (LPARAM)control->Choices[i].Display);
				if (value != NULL && stricmp(value, control->Choices[i].Value) == 0) {
					SendDlgItemMessage(dialog, control->Id, CB_SETCURSEL, i, 0);
					found = true;
				}
			}
			if (SendDlgItemMessage(dialog, control->Id, CB_GETCURSEL, 0, 0) == CB_ERR) {
				SendDlgItemMessage(dialog, control->Id, CB_SETCURSEL, 0, 0);
			}
			if ((GetWindowLong(GetDlgItem(dialog, control->Id), GWL_STYLE) & 0xF) == CBS_DROPDOWNLIST) {
				if (!found) {
					SendDlgItemMessage(dialog, control->Id, CB_SETCURSEL, i, 0);
				}
			} else if (!found && value != NULL) {
				int index = SendDlgItemMessage(dialog, control->Id, CB_INSERTSTRING, (WPARAM)-1, (LPARAM)value);	// NAME 1 [index]: neutral
				SendDlgItemMessage(dialog, control->Id, CB_SETCURSEL, index, 0);
			}
		}
	}
}

#pragma auto_inline(on)
// @0x846FC0
// NOT EXACT: functional (D64), register 0.66: case 2 keeps the choices in edi where the client keeps the dialog and
// copies the name pointer (lea edi, [esi]); the client's 260-byte buffers and order are reproduced
static void Unknown_846FC0(OptionControlStruct *controls, FILE *file, HWND dialog)	// NAME 1 [Unknown_846FC0]: neutral (D10), a page's controls into the options and the file  // NAME 1 [controls, file, dialog]: neutral
{
	ArgvClass argv(false, true);
	for (OptionControlStruct *control = controls; ; control++) {	// NAME 1 [control]: neutral
		if (control->Name != NULL && strlen(control->Name) != 0) {
			argv.Remove_Value(control->Name);
		}
		if (control->Type == 3) {
			return;
		}
		if (control->Type == 0) {
			if (SendDlgItemMessage(dialog, control->Id, BM_GETCHECK, 0, 0)) {
				argv.Add_Value(control->Name);
				if (file != NULL) {
					fprintf(file, "%s\n", control->Name);
				}
			}
		} else if (control->Type == 1) {
			char text[_MAX_PATH];								// NAME 1 [text]: neutral
			text[0] = '\0';
			SendDlgItemMessage(dialog, control->Id, WM_GETTEXT, 256, (LPARAM)text);
			if (strlen(text) != 0) {
				argv.Add_Value(control->Name, text);
				if (file != NULL) {
					fprintf(file, "%s\n", control->Name);
					fprintf(file, "%s\n", text);
				}
			}
		} else if (control->Type == 2) {
			int selection = SendDlgItemMessage(dialog, control->Id, CB_GETCURSEL, 0, 0);	// NAME 1 [selection]: neutral
			int last = -1;								// NAME 1 [last]: neutral
			for (int i = 0; control->Choices[i].Display != NULL; i++) {
				last = i;
			}
			if (selection > last) {
				char text[_MAX_PATH];							// NAME 1 [text]: neutral
				SendDlgItemMessage(dialog, control->Id, CB_GETLBTEXT, selection, (LPARAM)text);
				argv.Add_Value(control->Name, text);
				fprintf(file, "%s\n", control->Name);
				fprintf(file, "%s\n", text);
			} else if (selection == CB_ERR) {
				char text[_MAX_PATH];							// NAME 1 [text]: neutral
				SendDlgItemMessage(dialog, control->Id, WM_GETTEXT, 256, (LPARAM)text);
				if (strlen(text) != 0) {
					argv.Add_Value(control->Name, text);
					fprintf(file, "%s\n", control->Name);
					fprintf(file, "%s\n", text);
				}
			} else {
				const char *value = control->Choices[selection].Value;	// NAME 1 [value]: neutral
				if (value != NULL && strlen(value) != 0) {
					argv.Add_Value(control->Name, value);
					if (file != NULL) {
						fprintf(file, "%s\n", control->Name);
						fprintf(file, "%s\n", control->Choices[selection].Value);
					}
				}
			}
		}
	}
}

// @0x8472E0
BOOL CALLBACK Unknown_8472E0(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)	// NAME 1 [Unknown_8472E0]: neutral (D10), the options dialog's procedure (0x846C90's DialogBoxParam)
{
	ArgvClass argv(false, true);
	switch (uMsg) {
		case WM_INITDIALOG:
		{
			argv.Load_File(Unknown_BAF740);
			for (unsigned i = 0; i < sizeof(Unknown_BAF730) / sizeof(Unknown_BAF730[0]); i++) {
				TCITEM item;							// NAME 1 [item]: neutral
				item.pszText = (char *)Unknown_BAF730[i].Name;
				item.cchTextMax = strlen(Unknown_BAF730[i].Name);
				item.mask = TCIF_TEXT;
				SendDlgItemMessage(hDlg, 0x3FB, TCM_INSERTITEM, i, (LPARAM)&item);
				HWND page = CreateDialogParam(ProgramInstance, MAKEINTRESOURCE(Unknown_BAF730[i].Id), hDlg, Unknown_BAF730[i].Proc, 0);	// NAME 1 [page]: neutral
				Unknown_BAF730[i].Window = page;
				RECT dialog_rect;						// NAME 1 [dialog_rect]: neutral
				RECT tab_rect;							// NAME 1 [tab_rect]: neutral
				GetWindowRect(hDlg, &dialog_rect);
				GetWindowRect(GetDlgItem(hDlg, 0x3FB), &tab_rect);
				SetWindowPos(page, NULL, tab_rect.left - dialog_rect.left, tab_rect.top - dialog_rect.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
				if (i == 0) {
					ShowWindow(page, SW_SHOW);
				} else {
					ShowWindow(page, SW_HIDE);
				}
			}
			return TRUE;
		}

		case WM_ACTIVATE:
			return FALSE;

		case WM_NOTIFY:
		{
			NMHDR header = *(NMHDR *)lParam;			// NAME 1 [header]: neutral
			if (header.code == TCN_SELCHANGE) {
				int selection = TabCtrl_GetCurSel(GetDlgItem(hDlg, 0x3FB));	// NAME 1 [selection]: neutral
				for (unsigned i = 0; i < sizeof(Unknown_BAF730) / sizeof(Unknown_BAF730[0]); i++) {
					if (selection != (int)i) {
						ShowWindow(Unknown_BAF730[i].Window, SW_HIDE);
					} else {
						ShowWindow(Unknown_BAF730[i].Window, SW_SHOWNORMAL);
					}
				}
			}
			return TRUE;
		}

		case WM_COMMAND:
			if (LOWORD(wParam) == IDCANCEL) {
				EndDialog(hDlg, IDCANCEL);
			} else if (LOWORD(wParam) == IDOK) {
				file_auto_ptr file(_TheFileFactory, Unknown_BAF740);	// NAME 1 [file]: neutral
				FILE *fp = fopen(file->File_Name(), "w");	// NAME 1 [fp]: neutral
				if (fp != NULL) {
					fprintf(fp, "# This file is auto-generated by the options dialog\n");
					SendMessage(Unknown_BAF730[0].Window, 0x7E8, 0, (LPARAM)fp);
					fclose(fp);
				}
				EndDialog(hDlg, IDOK);
			}
			break;
	}
	return FALSE;
}

// @0x847630
// NOT EXACT: functional (D64), 0.77: the inlined 0x846CD0's first test; the client loads the first Type into ecx and
// the table into eax before comparing, this compares memory (0x847BB0 / 0x847C40 have the client's same shape)
static BOOL CALLBACK Unknown_847630(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
		case WM_INITDIALOG:
			{
			OptionControlStruct *controls = Unknown_BAF748;	// NAME 1 [controls]: neutral
			Unknown_846CD0(controls, hDlg);
			}
			return TRUE;

		case 0x7E8:
			Unknown_846FC0(Unknown_BAF748, (FILE *)lParam, hDlg);
			break;
	}
	return FALSE;
}
































































































































































// @0x8476C0
// NOT EXACT: functional (D64), as 0x847630: the inlined 0x846CD0's first test (the client loads the first Type into
// ecx and the table into eax before comparing); the rest, servers.ini's list and the folder browser, is the client's
BOOL CALLBACK Unknown_8476C0(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)	// NAME 1 [Unknown_8476C0]: neutral (D10), the server page's procedure (servers.ini's list for -SERVER_ADDR)
{
	switch (uMsg) {
		case WM_INITDIALOG:
		{
			INIClass ini;
			ini.Load("servers.ini");
			G_ASSERT(ini.Is_Loaded());
			char entry[260];									// NAME 1 [entry]: neutral
			char value[260];									// NAME 1 [value]: neutral
			signed char count = 0;								// NAME 1 [count]: neutral
			int i = 0;
			sprintf(entry, "Server%d", i);
			while (ini.Get_String("Servers", entry, "", value, 256)) {
				count++;
				i++;
				sprintf(entry, "Server%d", i);
			}
			int server_count = count;




			G_ASSERT(server_count < 256);
			memset(Unknown_BF35AC, 0, (server_count + 1) * sizeof(OptionChoiceStruct));
			for (i = 0; i < server_count; i++) {
				sprintf(entry, "Server%d", i);
				if (ini.Get_String("Servers", entry, "", value, 256)) {
					char *name = strtok(value, ",");			// NAME 1 [name]: neutral
					char *address = strtok(NULL, ",");			// NAME 1 [address]: neutral
					if (name != NULL && address != NULL) {
						sprintf(entry, "%s   %s", name, address);
						Unknown_BF35AC[i].Display = nstrdup(entry);
						Unknown_BF35AC[i].Value = nstrdup(address);
					}
				}
			}
			}
			{
			OptionControlStruct *controls = Unknown_BAF838;	// NAME 1 [controls]: neutral
			Unknown_846CD0(controls, hDlg);
			}
			return TRUE;

		case WM_DESTROY:
		{
			char **list = (char **)Unknown_BF35AC;			// NAME 1 [list]: neutral
			for (int i = 0; list[i] != NULL; i++) {
				delete list[i];
				list[i] = NULL;
			}
			break;
		}

		case WM_COMMAND:
			if (LOWORD(wParam) == 0x406) {
				char path[256];									// NAME 1 [path]: neutral
				memset(path, 0, sizeof(path));
				SendDlgItemMessage(hDlg, 0x405, WM_GETTEXT, 255, (LPARAM)path);
				BROWSEINFO info;								// NAME 1 [info]: neutral
				memset(&info, 0, sizeof(info));
				info.hwndOwner = hDlg;
				info.pszDisplayName = path;
				info.lpszTitle = "Select the directory with your test files:";
				info.ulFlags = BIF_EDITBOX;
				info.lpfn = Unknown_846B80;
				info.lParam = (LPARAM)path;
				LPITEMIDLIST pidl = SHBrowseForFolder(&info);	// NAME 1 [pidl]: neutral
				if (pidl != NULL) {
					SHGetPathFromIDList(pidl, path);
					LPMALLOC shell_malloc = NULL;				// NAME 1 [shell_malloc]: neutral
					if (SUCCEEDED(SHGetMalloc(&shell_malloc))) {
						shell_malloc->Free(pidl);
						shell_malloc->Release();
					}
					if (strlen(path) != 0) {
						if (path[strlen(path) - 1] == '\\') {
							path[strlen(path) - 1] = '\0';
						}
						SendDlgItemMessage(hDlg, 0x405, WM_SETTEXT, 0, (LPARAM)path);
					}
				}
			}
			break;

		case 0x7E8:
			Unknown_846FC0(Unknown_BAF838, (FILE *)lParam, hDlg);
			break;
	}
	return FALSE;
}

// @0x847BB0
// NOT EXACT: functional (D64), as 0x847630: the inlined 0x846CD0's first test
BOOL CALLBACK Unknown_847BB0(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)	// NAME 1 [Unknown_847BB0]: neutral (D10), the -ORACLE / -PROXY_ADDR page's procedure (in no page table)
{
	switch (uMsg) {
		case WM_INITDIALOG:
			{
			OptionControlStruct *controls = Unknown_BAFB58;	// NAME 1 [controls]: neutral
			Unknown_846CD0(controls, hDlg);
			}
			return TRUE;

		case 0x7E8:
			Unknown_846FC0(Unknown_BAFB58, (FILE *)lParam, hDlg);
			break;
	}
	return FALSE;
}

// @0x847C40
// NOT EXACT: functional (D64), as 0x847630: the inlined 0x846CD0's first test
BOOL CALLBACK Unknown_847C40(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)	// NAME 1 [Unknown_847C40]: neutral (D10), the -NOSTARTUPMOVIE / -PROTOCOL page's procedure (in no page table)
{
	switch (uMsg) {
		case WM_INITDIALOG:
			{
			OptionControlStruct *controls = Unknown_BAFDB0;	// NAME 1 [controls]: neutral
			Unknown_846CD0(controls, hDlg);
			}
			return TRUE;

		case 0x7E8:
			Unknown_846FC0(Unknown_BAFDB0, (FILE *)lParam, hDlg);
			break;
	}
	return FALSE;
}

// @0x847CD0
OptionControlStruct *Unknown_847CD0(int id, OptionControlStruct *controls)	// NAME 1 [Unknown_847CD0]: neutral (D10), a page's control by its dialog item (NULL past the table's end)  // NAME 1 [id, controls]: neutral
{
	int i = 0;
	while (true) {
		if (controls[i].Id == -1 || controls[i].Type == 3) {
			return NULL;
		}
		if (controls[i].Id == id) {
			return &controls[i];
		}
		i++;
	}
}

/*
**	Reconstruction notes (optdlg.cpp)
**
**	* The file: the client's assert strings name e:\build\beta\G\app\Common\optdlg.cpp (0x8476C0's 'ini.Is_Loaded()'
**	  line 498 and 'server_count < 256' line 514); its functions run from the folder browser's callback 0x846B80
**	  and WinMain's -MENU dialog 0x846C90 to the pages 0x847BB0 / 0x847C40 and 0x847CD0. 0x846C90 stays a POC
**	  stand-in (D69: it opens a modal dialog); 0x846CD0 (a page's controls set, 0x846D10 over the table) is inlined
**	  by the page procedures (0x847630). 0x8476C0's asserts keep lines 498 / 514 (the blank lines above it).
**	* 0x846D10 sits inside '#pragma auto_inline(off)': without it the large 0x8476C0 inlines it into the inlined
**	  0x846CD0 (len 1510 against the client's 1002); the small page procedures call it out of line either way.
**	* 0x8476C0, 0x847BB0 and 0x847C40 are in no page table (only 0x847630's 'Account' page is): they are kept
**	  extern so the compiler keeps them. 0x8476C0's WM_DESTROY frees servers.ini's list as a flat char *[]
**	  (plain operator delete); its server count is a signed char widened once (movsx) for the assert.
**	* Data: the page table 0xBAF730 ({ "Account", 111, 0x847630, NULL }), the file name 0xBAF740, the Account page's
**	  controls 0xBAF748 (-LOGIN 1000, -PASSWORD 1001); 0xBAF838 (0x8476C0's, -SERVER_ADDR's choices the
**	  servers.ini list 0xBF35AC), 0xBAFB58 (0x847BB0's), -PROTOCOL's choices 0xBAFD88 and 0xBAFDB0 (0x847C40's).
**	* 0x8472E0: the pages are loops over the one-entry page table; WM_INITDIALOG's loop survives (i in ebx) only with
**	  ShowWindow's two calls written as if / else (the ternary folds the loop away), and WM_NOTIFY's folded loop
**	  compares the selection with the index (cmp eax, ebx) only as 'selection != i'. The TCITEM's text before its mask.
**	* 0x847CD0's loop is top-tested (jmp back, the NULL return last) only as 'while (true)'; 'for (;;)', a goto
**	  or a condition in the loop head rotate it.
**	* 0x7E8 is the private message that makes a page write its controls (0x8472E0's IDOK, lParam the open file).
*/
