// Earth & Beyond client (reconstruction): the settings store and its browsers (client 0x7C97B0-0x7CEF30: one object).
// File PROVISIONAL (D137): no string ties it to a file. Notes at the end.
#include <windows.h>
#include <wininet.h>
#include <shellapi.h>
#include <ole2.h>
#include <comdef.h>
#include <atlbase.h>
#include <stdio.h>
#include <string.h>
#include "tsk_game.h"
#include "ww3d.h"
#include "rendobj.h"
#include "aabox.h"
#include "dx8webbrowser.h"
#include "wwaudio.h"
#include "wwstring.h"

CComModule _Module;										// 0xBE87E8 (ATL's: its m_nLockCnt 0xBE87FC counts the live COM objects)
static char Unknown_BE8900[260];						// 0xBE8900: the exit URL ShellExecuteEx gets  // NAME 1: neutral (D10)
static SHELLEXECUTEINFOA Unknown_BE8A08;				// 0xBE8A08  // NAME 1: neutral (D10)

// 0xB072A0 (and 0xBA01B8, the copy the interface map points at): the client's own dispatch interface, in its type library
static const GUID Unknown_B072A0 = { 0x12FE6B61, 0x2669, 0x4F9B, { 0xA8, 0x6C, 0xBD, 0xF0, 0x27, 0xB6, 0xC8, 0x22 } };	// NAME 1: neutral (D10)

// @0x7CAB30
inline Unknown_7C9F00Struct::Unknown_7C9F00Struct() :
	Id(BROWSER_COUNT),
	Name(""),
	UpdateTicks(0),
	Options(0),
	Url("")
{
}

// @0x7C9F00
inline Unknown_7C9F00Struct::Unknown_7C9F00Struct(int id, const char *name, const char *url, int options, int update_ticks) :
	Id(id),
	Name(name),
	UpdateTicks(update_ticks),
	Options(options),
	Url(url ? url : "")
{
}

inline Unknown_7C9CD0RootStruct::Unknown_7C9CD0RootStruct()
{
	RefCount = 0;
}

// @0x7C9CD0
inline Unknown_7C9CD0Class::Unknown_7C9CD0Class()
{
	TypeInfo = NULL;
	Dispatch = NULL;
	char path[256];
	GetModuleFileNameA(NULL, path, sizeof(path));
	_bstr_t name(path);
	ITypeLib *library;
	if (LoadTypeLib(name, &library) == S_OK) {
		HRESULT result = library->GetTypeInfoOfGuid(Unknown_B072A0, &TypeInfo);
		library->Release();
		if (result == S_OK) {
			CreateStdDispatch((IUnknown *)this, this, TypeInfo, &Dispatch);
			Dispatch->AddRef();
		}
	}
}

// @0x7CD730 (the deleting destructor)
inline Unknown_7C9CD0Class::~Unknown_7C9CD0Class()
{
	if (TypeInfo != NULL) {
		TypeInfo->Release();
	}
	if (Dispatch != NULL) {
		Dispatch->Release();
	}
}

inline Unknown_7CD500Class::Unknown_7CD500Class()
{
	_Module.Lock();
}

// @0x7CD500
inline Unknown_7CD500Class::~Unknown_7CD500Class()
{
	RefCount = 1;
	_Module.Unlock();
}

// @0x7CD550
inline unsigned long __stdcall Unknown_7CD500Class::AddRef()
{
	return(++RefCount);
}

// @0x7CD570
inline unsigned long __stdcall Unknown_7CD500Class::Release()
{
	unsigned long count = --RefCount;
	if (count == 0) {
		delete this;
	}
	return(count);
}

// @0x7CD6F0 (ATL's CComObjectRootBase::_Delegate)
static HRESULT WINAPI Unknown_7CD6F0(void *object, REFIID iid, void **out, DWORD offset)	// NAME 1: neutral (D10)
{
	long result = E_NOINTERFACE;
	IUnknown *unknown = *(IUnknown **)((char *)object + offset);
	if (unknown != NULL) {
		result = unknown->QueryInterface(iid, out);
	}
	return(result);
}

// 0xB07380: the interface map (ATL's BEGIN_COM_MAP: the interface itself, IDispatch from the +0x08 object)
static const _ATL_INTMAP_ENTRY Unknown_B07380[] = {		// NAME 1: neutral (D10)
	{ &Unknown_B072A0, 0, _ATL_SIMPLEMAPENTRY },
	{ &IID_IDispatch, 8, Unknown_7CD6F0 },
	{ NULL, 0, NULL }
};

// @0x7CD5B0
inline long __stdcall Unknown_7CD500Class::QueryInterface(const _GUID &iid, void **object)
{
	return(AtlInternalQueryInterface(this, Unknown_B07380, iid, object));
}

// @0x7C97B0
Unknown_7CA2D0Class::Unknown_7CA2D0Class() :
	Unknown24(""),
	Unknown34(false),
	Unknown38(0),
	Unknown3C(0),
	Unknown40(0),
	Unknown44(0),
	Unknown48(NULL),
	Unknown4C(0)
{
	Unknown_7CBC60(Unknown_7C9F00Struct(BROWSER_RNW, "RNW", "http://support.net-7.org/", 1, 0));
	Unknown_7CBC60(Unknown_7C9F00Struct(BROWSER_SUPPORT, "SUPPORT", "http://support.net-7.org", 1, 0));
	Unknown_7CBC60(Unknown_7C9F00Struct(BROWSER_BUG, "BUG", "http://support.net-7.org/", 5, 0));
	Unknown_7CBC60(Unknown_7C9F00Struct(BROWSER_PIT, "PIT", "http://www.net-7.org/browser", 5, 0));
	Unknown_7CBC60(Unknown_7C9F00Struct(BROWSER_PRIVACY, "PRIVACY", "http://www.enb-emulator.com", 5, 0));
	Unknown_7CBC60(Unknown_7C9F00Struct(BROWSER_CREDITS, "CREDITS", "http://www.enb-emulator.com", 5, 0));
	Unknown_7CBC60(Unknown_7C9F00Struct(BROWSER_HELP, "HELP", "http://gf1.enb-emulator.com/", 5, 0));
	Unknown_7CBC60(Unknown_7C9F00Struct(BROWSER_HTMLDIALOG, "HTMLDIALOG", NULL, 0, 0));
	unsigned int now = GetTickCount();
	Unknown38 = now + 5400000;	// 90 minutes
	Unknown40 = now + 6900000;	// 115 minutes
}

// @0x7CA2D0
Unknown_7CA2D0Class *Unknown_7CA2D0()
{
	static Unknown_7CA2D0Class store;
	return &store;
}

// @0x7CA340
Unknown_7CA2D0Class::~Unknown_7CA2D0Class()
{
}

// @0x7CA570
bool Unknown_7CA2D0Class::Unknown_7CA570(int id)
{
	if (id < BROWSER_COUNT) {
		return DX8WebBrowser::Is_Browser_Open(Unknown04[id].Name.c_str());
	}
	return false;
}

// @0x7CA5C0
bool Unknown_7CA2D0Class::Unknown_7CA5C0(int id, RenderObjClass *object, const char *url)
{
	if (id < BROWSER_COUNT) {
		int width;
		int height;
		int bits;
		bool windowed;
		WW3D::Get_Device_Resolution(width, height, bits, windowed);
		AABoxClass box = object->Get_Bounding_Box();
		float scale_x = width * (1.0f / 640.0f);
		float scale_y = height * (1.0f / 480.0f);
		int w = (int)((box.Extent.X + box.Extent.X) * scale_x);
		int h = (int)((box.Extent.Y + box.Extent.Y) * scale_y);
		int x = (int)((box.Center.X + 320.0f - box.Extent.X) * scale_x);
		int y = (int)((240.0f - box.Center.Y - box.Extent.Y) * scale_y);
		std::string query = Unknown_7CC890();
		IDispatch *dispatch = NULL;
		Unknown14.QueryInterface(IID_IDispatch, (void **)&dispatch);
		if (dispatch != NULL) {
			dispatch->AddRef();
		}
		const char *query_text = query.empty() ? NULL : query.c_str();
		const char *address = (url == NULL) ? Unknown04[id].Url.c_str() : url;
		DX8WebBrowser::CreateBrowser(Unknown04[id].Name.c_str(), address, x, y, w, h, Unknown04[id].UpdateTicks, Unknown04[id].Options, dispatch, query_text);
		if (dispatch != NULL) {
			dispatch->Release();
		}
		Unknown34 = true;
	}
	return false;
}

// @0x7CAD00
bool Unknown_7CA2D0Class::Unknown_7CAD00(int id, const AABoxClass &box, const char *url)
{
	if (id < BROWSER_COUNT) {
		int width;
		int height;
		int bits;
		bool windowed;
		WW3D::Get_Device_Resolution(width, height, bits, windowed);
		float scale_x = width * (1.0f / 640.0f);
		float scale_y = height * (1.0f / 480.0f);
		int w = (int)((box.Extent.X + box.Extent.X) * scale_x);
		int h = (int)((box.Extent.Y + box.Extent.Y) * scale_y);
		int x = (int)((box.Center.X + 320.0f - box.Extent.X) * scale_x);
		int y = (int)((240.0f - box.Center.Y - box.Extent.Y) * scale_y);
		std::string query = Unknown_7CC890();
		IDispatch *dispatch = NULL;
		Unknown14.QueryInterface(IID_IDispatch, (void **)&dispatch);
		if (dispatch != NULL) {
			dispatch->AddRef();
		}
		const char *query_text = query.empty() ? NULL : query.c_str();
		const char *address = (url == NULL) ? Unknown04[id].Url.c_str() : url;
		DX8WebBrowser::CreateBrowser(Unknown04[id].Name.c_str(), address, x, y, w, h, Unknown04[id].UpdateTicks, Unknown04[id].Options, dispatch, query_text);
		if (dispatch != NULL) {
			dispatch->Release();
		}
		Unknown34 = true;
	}
	return false;
}

// @0x7CB240
void Unknown_7CA2D0Class::Unknown_7CB240(int id)
{
	if (id < BROWSER_COUNT) {
		DX8WebBrowser::DestroyBrowser(Unknown04[id].Name.c_str());
		Unknown34 = false;
	}
}

// @0x7CB290
void Unknown_7CA2D0Class::Unknown_7CB290(int id, const char *url)
{
	if (id < BROWSER_COUNT && url != NULL) {
		DX8WebBrowser::Navigate(Unknown04[id].Name.c_str(), url);
	}
}

// @0x7CB2E0
void Unknown_7CA2D0Class::Unknown_7CB2E0(const char *url, bool force)
{
	if (Unknown24.length() != 0 && !force) {
		return;
	}
	Unknown24 = url;
}

// @0x7CB3D0
void Unknown_7CA2D0Class::Unknown_7CB3D0()
{
	if (Unknown24.length() != 0) {
		strcpy(Unknown_BE8900, Unknown24.c_str());
		memset(&Unknown_BE8A08, 0, sizeof(Unknown_BE8A08));
		Unknown_BE8A08.cbSize = sizeof(Unknown_BE8A08);
		Unknown_BE8A08.fMask = SEE_MASK_FLAG_NO_UI | SEE_MASK_FLAG_DDEWAIT;
		Unknown_BE8A08.hwnd = GetDesktopWindow();
		Unknown_BE8A08.lpFile = Unknown_BE8900;
		Unknown_BE8A08.nShow = SW_SHOWMAXIMIZED;
		if (!ShellExecuteExA(&Unknown_BE8A08)) {
			char text[1024];
			_snprintf(text, sizeof(text), "ShellExecute Error: %X", GetLastError());
			MessageBoxA(Unknown_BE8A08.hwnd, text, "Error", MB_ICONEXCLAMATION);
		}
	}
}

// @0x7CB4E0
const char *Unknown_7CA2D0Class::Unknown_7CB4E0()
{
	static StringClass name;
	name = WWAudioClass::Get_Instance()->Get_3D_Driver_Name();
	return(name);
}

// @0x7CB610
void Unknown_7CA2D0Class::Unknown_7CB610(const char *url)
{
	Unknown50 = url;
}

// @0x7CB6F0
void Unknown_7CA2D0Class::Unknown_7CB6F0(int force)
{
	if (Unknown50.length() == 0) {
		return;
	}

	unsigned int now = GetTickCount();
	if (Unknown48 != NULL) {
		DWORD code = 0;
		GetExitCodeThread(Unknown48, &code);
		if (code == STILL_ACTIVE) {
			return;
		}
		if (code == 0) {
			switch (Unknown4C) {
				case 1:
				case 3:
					Unknown3C = now + 900000;
					Unknown38 = now + 900000;
					break;
				case 2:
				case 4:
					Unknown3C = 0;
					Unknown44 = now + 900000;
					Unknown40 = now + 900000;
					Unknown38 = now + 900000;
					break;
			}
		} else {
			Unknown38 = now + 5400000;
			Unknown3C = 0;
			Unknown40 = now + 6900000;
			Unknown44 = 0;
		}
		CloseHandle(Unknown48);
		Unknown48 = NULL;
		Unknown4C = 0;
	} else {
		if (Unknown44 != 0 && Unknown44 - now > 0x80000000) {
			DWORD id;
			Unknown4C = 4;
			Unknown48 = CreateThread(NULL, 0, Unknown_7CB8F0, this, 0, &id);
		} else if (Unknown40 - now > 0x80000000) {
			DWORD id;
			Unknown4C = 2;
			Unknown48 = CreateThread(NULL, 0, Unknown_7CB8F0, this, 0, &id);
		} else if ((char)force) {
			if (Unknown3C != 0 && Unknown3C - now > 0x80000000) {
				DWORD id;
				Unknown4C = 3;
				Unknown48 = CreateThread(NULL, 0, Unknown_7CB8F0, this, 0, &id);
			} else if (Unknown38 - now > 0x80000000) {
				DWORD id;
				Unknown4C = 1;
				Unknown48 = CreateThread(NULL, 0, Unknown_7CB8F0, this, 0, &id);
			}
		}
	}
}

// @0x7CB8A0
bool Unknown_7CA2D0Class::Unknown_7CB8A0(int which)
{
	if (Unknown48 != NULL) {
		return false;
	}
	Unknown4C = which;
	DWORD id;
	Unknown48 = CreateThread(NULL, 0, Unknown_7CB8F0, this, 0, &id);
	return true;
}

// @0x7CB8F0
// NOT EXACT: order-only (the same 231 instructions: the client zeroes the count before the read's arguments are pushed
// and reloads a handle one instruction earlier at the end)
unsigned long __stdcall Unknown_7CA2D0Class::Unknown_7CB8F0(void *store)
{
	Unknown_7CA2D0Class *self = (Unknown_7CA2D0Class *)store;
	Unknown_7CA2D0();
	unsigned long result = 0;
	std::string lkey = self->Get_String("lkey");
	if (lkey.length() != 0) {
		char url[1024];
		_snprintf(url, sizeof(url), self->Unknown50.c_str(), lkey.c_str());
		HINTERNET internet = InternetOpenA("ENBCLIENT", 0, NULL, NULL, 0);
		if (internet != NULL) {
			std::string reply;
			HINTERNET request = InternetOpenUrlA(internet, url, NULL, 0, 0x84003100, 0);
			if (request != NULL) {
				char buffer[100];
				bool more = true;
				while (more) {
					DWORD read = 0;
					if (InternetReadFile(request, buffer, sizeof(buffer), &read) && read != 0) {
						reply.append(buffer);
						buffer[0] = '\0';
					} else {
						more = false;
					}
				}
				if (reply.substr(0, 7).compare("Success") == 0) {
					result = 1;
				}
				InternetCloseHandle(request);
			}
			InternetCloseHandle(internet);
		}
	}
	return(result);
}

// @0x7CBC60
void Unknown_7CA2D0Class::Unknown_7CBC60(const Unknown_7C9F00Struct &browser)
{
	if (browser.Id < BROWSER_COUNT) {
		Unknown04[browser.Id] = browser;
	}
}

// @0x7CC1B0
Unknown_7C9F00Struct Unknown_7CA2D0Class::Unknown_7CC1B0(int id)
{
	Unknown_7C9F00Struct none(BROWSER_COUNT, "", "", 1, 0);
	if (id < BROWSER_COUNT) {
		return Unknown04[id];
	}
	return none;
	none.Name.length();	// a dead read (D130; not the client's): spends VC6's inline budget (the last return's copy of Name is a call)
}

// @0x7CC630
void Unknown_7CA2D0Class::Unknown_7CC630(int id, const char *url)
{
	if (id < BROWSER_COUNT) {
		Unknown_7C9F00Struct browser = Unknown_7CC1B0(id);
		browser.Url = url;
		Unknown_7CBC60(browser);
	}
}

// @0x7CC890
std::string Unknown_7CA2D0Class::Unknown_7CC890()
{
	std::string result = "";
	for (std::map<std::string, std::string>::iterator it = Unknown60.begin(); it != Unknown60.end(); ++it) {
		if (it != Unknown60.begin()) {
			result += "&";
		}
		result += it->first;
		result += "=";
		result += it->second;
	}
	return result;
}

// @0x7CCB30
void Unknown_7CA2D0Class::Set_String(const char *key, const char *value)
{
	char canon_key[2084];
	char canon_value[2084];
	DWORD canon_key_size = sizeof(canon_key);
	DWORD canon_value_size = sizeof(canon_value);
	canon_key[0] = '\0';
	canon_value[0] = '\0';
	if (!InternetCanonicalizeUrlA(key, canon_key, &canon_key_size, 0)) {
		return;
	}
	if (value != NULL) {
		if (!InternetCanonicalizeUrlA(value, canon_value, &canon_value_size, 0)) {
			return;
		}
	}
	if (value == NULL || strlen(value) == 0) {
		std::map<std::string, std::string>::iterator it = Unknown60.find(std::string(canon_key));
		if (it != Unknown60.end()) {
			Unknown60.erase(it);
		}
	} else {
		Unknown60[std::string(canon_key)] = canon_value;
	}
}

// @0x7CCE80
void Unknown_7CA2D0Class::Set_Int(const char *key, int value)
{
	char buffer[100];
	_snprintf(buffer, 100, "%d", value);
	Set_String(key, buffer);
}

// @0x7CCED0
void Unknown_7CA2D0Class::Set_Int64(const char *key, __int64 value)
{
	char buffer[100];
	_snprintf(buffer, 100, "%I64d", value);
	Set_String(key, buffer);
}

// @0x7CCF20
std::string Unknown_7CA2D0Class::Get_String(const char *key)
{
	char canon_value[2084];
	char canon_key[2084];
	DWORD canon_key_size = sizeof(canon_key);
	if (!InternetCanonicalizeUrlA(key, canon_key, &canon_key_size, 0)) {
		return "";
	}
	std::string entry = Unknown60[canon_key];
	DWORD canon_value_size = sizeof(canon_value);
	canon_value[0] = '\0';
	if (!InternetCanonicalizeUrlA(entry.c_str(), canon_value, &canon_value_size, ICU_NO_ENCODE | ICU_DECODE)) {
		return "";
	}
	return canon_value;
}

/*
**	Reconstruction notes
**
**	client.exe: one object, 0x7C97B0-0x7CEF30 (0x7C96F0 is an object of its own: unknown_7c96f0.cpp). Unknown_7CA2D0Class (tsk_game.h) is a function-static object (0x70
**	bytes at 0xBE8890, constructor 0x7C97B0, accessor 0x7CA2D0, table 0xB0734C) with three jobs:
**	* the browsers: a std::map<int, Unknown_7C9F00Struct> at +0x04 of eight descriptors (id, DX8WebBrowser name,
**	  update ticks, options, URL: 'RNW', 'SUPPORT', 'BUG', 'PIT', 'PRIVACY', 'CREDITS', 'HELP', 'HTMLDIALOG'), opened
**	  over a render object's box (0x7CA5C0) or a box (0x7CAD00) scaled from 640x480, closed (0x7CB240), navigated
**	  (0x7CB290), asked (0x7CA570). Each gets the COM object at +0x14 as its game dispatch (window.external): ATL
**	  shapes (CComObjectRootBase's count at +4, CComObject's destructor and module lock count 0xBE87FC, the interface
**	  map 0xB07380 with a COM_INTERFACE_ENTRY_AGGREGATE for IDispatch, CComBSTR in 0x7C96F0) over CreateStdDispatch
**	  and the client's own type library; its one method returns WWAudio's 3D driver name (0x7CB4E0). The classes are
**	  written without ATL's templates (tsk_game.h is included widely); this file uses ATL's _Module, interface map
**	  and AtlInternalQueryInterface.
**	* the settings: Set_String / Get_String keep canonicalised (InternetCanonicalizeUrlA) keys and values in a
**	  std::map at +0x60 (an empty value erases the entry); 0x7CC890 joins them as the browsers' query data.
**	* the lkey check: 0x7CB6F0 is the login and game tasks' per-frame poll: with an lkey URL set (0x7CB610,
**	  auth.ini's LKeyUrl) and a deadline passed it starts 0x7CB8F0 on a thread, which fetches the URL formatted
**	  with the stored 'lkey' and returns 1 for a reply starting "Success"; the next poll collects the exit code and
**	  sets the next deadlines (15 min after a success, 90 / 115 min after a failure; the constructor's first ones:
**	  90 and 115 minutes after start, so no fetch before then).
**	And the exit URL at +0x24 (0x7CB2E0 sets it, 0x7CB3D0 hands it to ShellExecuteEx at WinMain's shut-down).
**	The URLs are the installed client's (Net-7's edits of EA's).
**	Forms that mattered: Set_String's erase arm comes first in the source (`if (value == NULL || strlen(value) == 0)
**	{ erase } else { insert }`: VC6 lays the else arm out first, and the inline budget follows the source); the check
**	thread's read loop runs on a flag (unrotated); 0x7CC1B0 needs one dead read after its last return (D130).
**	NOT EXACT (D64): 0x7CB8F0 (order-only: two instructions out of place).
**	Other files still reach 0x7CB2E0 / 0x7CB240 / 0x7CA570 / 0x7CAD00 through local stand-in classes (ui_login.cpp,
**	ui_popupmsg.cpp, ui_popup_browser.cpp, ui_registration.cpp, unknown_msgbox.cpp).
*/
