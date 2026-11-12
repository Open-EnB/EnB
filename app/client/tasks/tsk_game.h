// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Client\tasks\tsk_game.h (name assumed from tsk_game.cpp)
**
**	ClientTaskClass (its strings: 'ClientTask::Run'; tsk_game.cpp's asserts name its members): the
**	game's task. It joins the master server (MODE_JOIN), waits for the redirect to a sector server
**	(MODE_WAIT_FOR_REDIRECT), runs the sector through SClient (MODE_SECTOR) and, when that ends,
**	hands the kernel back to a new LoginTaskClass. Vtable 0xB03FF4, 0xA8 bytes (docs/kernel-map.md 3).
*/



#if defined(_MSC_VER)
#pragma once
#endif

#ifndef TSK_GAME_H  // NAME 3 [tsk_game.h]: header name assumed from the client's tsk_game.cpp
#define TSK_GAME_H

#include "..\..\kernel\kernel.h"
#include <string>
#include <map>
#include "..\..\shared\unknown_login_data.h"		// TicketClass

class ConnectionClass;
class SClient;

class ClientTaskClass : public TaskClass		// NAME 3: the client's 'ClientTask' (its profile label) in LoginTaskClass's form (D24)
{
public:
	enum {
		MODE_JOIN,										// the asserts' names
		MODE_WAIT_FOR_REDIRECT,
		MODE_SECTOR,
		MODE_LEAVING									// NAME 1: Run waits for the UDP manager or 5 s, then a LoginTaskClass
	};

	ClientTaskClass(const std::string &username, const std::string &password);	// 0x765F80  // NAME 2 [username, password]: LoginTaskClass's 0x7697A0 / 0x7697E0 get them back
	virtual ~ClientTaskClass();							// +0x04 0x766190 / 0x7661C0
	virtual int Unknown_0C() { return (int)this; }		// +0x0C 0x766150 (the base's returns 0)
	virtual bool Accept_Focus();						// +0x28 0x7665A0
	virtual bool Yield_Focus();							// +0x2C 0x7665F0
	virtual void Run();									// +0x30 0x766630 ('ClientTask::Run')
	virtual int Get_Interval();							// +0x34 0x7671E0
	virtual void Render();								// +0x3C 0x766F10
	virtual bool Shutdown();							// +0x40 0x766F80
	virtual bool Delete_On_Remove() { return true; }	// +0x44 0x766170

	bool Init(KernelClass *kernel, TicketClass *ticket, const std::string &login_key, unsigned long address);	// 0x766350 (the assert 'kernel && ticket')  // NAME 1 [Init, login_key, address]: what they are
	void Join();										// 0x7667F0 (MODE_JOIN)  // NAME 1: what it does
	void Connect_Master();								// 0x7668E0 ('Connecting to master server....')  // NAME 1: what it does
	void Wait_For_Redirect();							// 0x7669F0 (MODE_WAIT_FOR_REDIRECT)  // NAME 1: what it does
	void Redirect(unsigned long ip, unsigned short port);	// 0x766AB0 (the redirect packet's handler calls it)  // NAME 1: what it does
	void Sector();										// 0x766BD0 (MODE_SECTOR)  // NAME 1: what it does
	void Leave_Sector();								// 0x767030 (inlined in Sector)  // NAME 1: what it does
	void Set_Mode(int mode);							// 0x7670A0 (the assert 'RedirectCount <= 50')  // NAME 1: what it does

	int Unknown0C;										// +0x0C
	TicketClass Ticket;									// +0x10 NAME 1: Init's 'ticket'
	std::string Login_Key;								// +0x58 NAME 1: stored as 'lkey' (Accept_Focus)
	int Mode;											// +0x68 (the asserts 'Mode == MODE_JOIN' ...)
	int RedirectCount;									// +0x6C (the assert 'RedirectCount <= 50')
	ConnectionClass *Master_Connection;					// +0x70 (the asserts)
	unsigned long Address;								// +0x74 NAME 1: the master server's, or 0 for network.ini's
	SClient *Client;									// +0x78 (the asserts 'Client')
	int Unknown7C;										// +0x7C
	std::string Username;								// +0x80 NAME 1: handed back to the next LoginTaskClass
	std::string Password;								// +0x90 NAME 1: as Username
	long Timeout;										// +0xA0 NAME 1: MODE_LEAVING's deadline (time() + 5)
	int UnknownA4;										// +0xA4
};

// What tsk_game.cpp calls in files that aren't rebuilt (declared here only)
class INIClass;
class RenderObjClass;
INIClass &Get_Network_INI();							// 0x821B40 (connect.cpp)
int Unknown_Log(const char *tag, const char *format, ...);	// NAME 1 [Unknown_Log]: 0x7E5F90, the 'HANDOFF:INF' / 'LOGIN:INF' log
int Unknown_82E1D0();									// NAME 1 [Unknown_82E1D0]: udpconn.cpp
bool Unknown_5FADE0();									// NAME 1 [Unknown_5FADE0]: ui_registration.cpp

// The store's browsers (0x7C97B0 adds them in this order, under these DX8WebBrowser names; 8 is "none"). Trap: without a declaration here ui_radio_main.cpp loses 0x5E9410 (this header's declaration count; verify's path only)
enum BrowserType {										// NAME 1 [BrowserType and its values]: after the browsers' names in 0x7C97B0
	BROWSER_RNW = 0,
	BROWSER_SUPPORT,
	BROWSER_BUG,
	BROWSER_PIT,
	BROWSER_PRIVACY,									// ui_registration.cpp opens it (4)
	BROWSER_CREDITS,
	BROWSER_HELP,
	BROWSER_HTMLDIALOG,									// the message box's (7)
	BROWSER_COUNT
};

struct IUnknown;
struct ITypeInfo;
struct _GUID;

// One browser the store can open (0x2C bytes; the constructor 0x7C97B0 adds eight, ids 0-7; 8 is "none")
struct Unknown_7C9F00Struct								// NAME 1 [Unknown_7C9F00Struct]: neutral (D10)
{
	Unknown_7C9F00Struct();								// 0x7CAB30 (inline, in settings_store.cpp: no bodies in this widely included header)
	Unknown_7C9F00Struct(int id, const char *name, const char *url, int options, int update_ticks);	// 0x7C9F00 (inline, as 0x7CAB30)  // NAME 1 [id, name, url, options, update_ticks]: the fields they set

	int Id;												// +0x00 (the map's key)  // NAME 1: what it is
	std::string Name;									// +0x04 ('SUPPORT', 'PRIVACY', ...: DX8WebBrowser's browser name)  // NAME 1: what it is
	int UpdateTicks;									// +0x14 (CreateBrowser's updateticks)  // NAME 1: the argument it is passed as
	int Options;										// +0x18 (CreateBrowser's options)  // NAME 1: the argument it is passed as
	std::string Url;									// +0x1C  // NAME 1: what it holds
};

// The object the browsers get as their game dispatch (window.external): a COM object over CreateStdDispatch, with
// ATL's shapes (CComObjectRootBase's count, CComObject's lock count and destructor). Written without ATL's headers here.
struct Unknown_7C9CD0RootStruct							// NAME 1 [Unknown_7C9CD0RootStruct]: neutral (D10); ATL's CComObjectRootBase
{
	Unknown_7C9CD0RootStruct();							// inline in 0x7C9CD0 (the count stored before the table: a base's constructor body)
	long RefCount;										// +0x04 of the object  // NAME 1: what AddRef / Release count
};

class Unknown_7C9CD0Class : public Unknown_7C9CD0RootStruct	// NAME 1 [Unknown_7C9CD0Class]: neutral (D10); table 0xB07368
{
public:
	virtual long __stdcall QueryInterface(const _GUID &iid, void **object) = 0;	// +0x00
	virtual unsigned long __stdcall AddRef() = 0;		// +0x04
	virtual unsigned long __stdcall Release() = 0;		// +0x08
	virtual long __stdcall Unknown_0C(unsigned short **text) = 0;	// +0x0C  // NAME 1 [Unknown_0C, text]: neutral (D10)
	Unknown_7C9CD0Class();								// 0x7C9CD0
	virtual ~Unknown_7C9CD0Class();						// +0x10: 0x7CD730

	IUnknown *Dispatch;									// +0x08 (CreateStdDispatch's)  // NAME 1: what it holds
	ITypeInfo *TypeInfo;								// +0x0C (the client's own type library's, GUID 0xB072A0)  // NAME 1: what it holds
};

class Unknown_7CD500Class : public Unknown_7C9CD0Class	// NAME 1 [Unknown_7CD500Class]: neutral (D10); table 0xB07350
{
public:
	Unknown_7CD500Class();								// inline in 0x7C97B0 (counts itself in 0xBE87FC)
	virtual ~Unknown_7CD500Class();						// 0x7CD500 (0x7CD810)
	virtual long __stdcall QueryInterface(const _GUID &iid, void **object);	// 0x7CD5B0
	virtual unsigned long __stdcall AddRef();			// 0x7CD550
	virtual unsigned long __stdcall Release();			// 0x7CD570
	virtual long __stdcall Unknown_0C(unsigned short **text);	// 0x7C96F0
};

class Unknown_7CA2D0Class								// NAME 1 [Unknown_7CA2D0Class]: the settings store 0x7CA2D0 returns; table 0xB0734C, 0x70 bytes
{
public:
	Unknown_7CA2D0Class();									// 0x7C97B0 (the constructor; the accessor 0x7CA2D0 runs it once)
	virtual ~Unknown_7CA2D0Class();						// 0x7CA340 (0x7CA160)
	void Set_String(const char *key, const char *value);	// 0x7CCB30  // NAME 1: what it does
	void Set_Int(const char *key, int value);			// 0x7CCE80 ('%d')  // NAME 1: what it does
	void Set_Int64(const char *key, __int64 value);		// 0x7CCED0  // NAME 1: what it does
	void Unknown_7CB610(const char *url);				// 0x7CB610 (LoginTaskClass::Fetch_EA_Ticket: auth.ini's LKeyUrl)
	void Unknown_7CB6F0(int force);						// 0x7CB6F0 (the tasks' per-frame poll: the lkey check thread started when a deadline passed, its result collected)  // NAME 1 [force]: lets the +0x38 / +0x3C deadlines start it (the game task's 1)
	bool Unknown_7CB8A0(int which);						// 0x7CB8A0 (starts the check thread unless one runs)  // NAME 1 [Unknown_7CB8A0, which]: neutral (D10)
	bool Unknown_7CA5C0(int id, RenderObjClass *object, const char *url);	// 0x7CA5C0 (ui_registration.cpp 0x5FB030: 4, its 'LABEL_TEXT', NULL): opens browser 'id' over the object's box  // NAME 1 [Unknown_7CA5C0, id, object, url]: neutral (D10)
	bool Unknown_7CAD00(int id, const class AABoxClass &box, const char *url);	// 0x7CAD00: as 0x7CA5C0, over a box  // NAME 1 [Unknown_7CAD00, id, box, url]: neutral (D10)
	bool Unknown_7CA570(int id);						// 0x7CA570 (DX8WebBrowser::Is_Browser_Open)  // NAME 1 [Unknown_7CA570, id]: neutral (D10)
	void Unknown_7CB240(int id);						// 0x7CB240 (DX8WebBrowser::DestroyBrowser)  // NAME 1 [Unknown_7CB240, id]: neutral (D10)
	void Unknown_7CB290(int id, const char *url);		// 0x7CB290 (DX8WebBrowser::Navigate)  // NAME 1 [Unknown_7CB290, id, url]: neutral (D10)
	void Unknown_7CB2E0(const char *url, bool force);	// 0x7CB2E0 (sets the exit URL, when none is set or forced)  // NAME 1 [Unknown_7CB2E0, url, force]: neutral (D10)
	void Unknown_7CBC60(const Unknown_7C9F00Struct &browser);	// 0x7CBC60 (adds or replaces a browser)  // NAME 1 [Unknown_7CBC60, browser]: neutral (D10)
	Unknown_7C9F00Struct Unknown_7CC1B0(int id);		// 0x7CC1B0 (a copy of browser 'id')  // NAME 1 [Unknown_7CC1B0, id]: neutral (D10)
	void Unknown_7CC630(int id, const char *url);		// 0x7CC630 (sets browser 'id's URL)  // NAME 1 [Unknown_7CC630, id, url]: neutral (D10)
	std::string Unknown_7CC890();						// 0x7CC890 (the settings as 'key=value&key=value': CreateBrowser's query data)  // NAME 1 [Unknown_7CC890]: neutral (D10)
	std::string Get_String(const char *key);			// 0x7CCF20 (0x7CB8F0: 'lkey')  // NAME 1: what it returns
	static unsigned long __stdcall Unknown_7CB8F0(void *store);	// 0x7CB8F0: the check thread (WinInet, 'ENBCLIENT'; the reply starting 'Success': 1)  // NAME 1 [store]: CreateThread's parameter
	void Unknown_7CB3D0();								// 0x7CB3D0 (WinMain's shut-down: ShellExecuteEx, 'Earth & Beyond will now exit to allow you to check your E-Mail')  // NAME 1 [Unknown_7CB3D0]: neutral (D10)
	const char *Unknown_7CB4E0();						// 0x7CB4E0 (the text the browsers' pages read through the dispatch object)  // NAME 1 [Unknown_7CB4E0]: neutral (D10)

	std::map<int, Unknown_7C9F00Struct> Unknown04;		// +0x04: the browsers by id  // NAME 1: neutral (D10)
	Unknown_7CD500Class Unknown14;						// +0x14: the browsers' game dispatch  // NAME 1: neutral (D10)
	std::string Unknown24;								// +0x24: the URL 0x7CB3D0 opens at exit (its length at +0x2C: LoginTaskClass::Unknown_6C tests it)  // NAME 1: neutral (D10)
	bool Unknown34;										// +0x34 (the chat panel's slot +0x68 0x563280: set, no Online Manual): a browser is open
	unsigned int Unknown38;								// +0x38: a GetTickCount deadline (mode 1)
	unsigned int Unknown3C;								// +0x3C: a deadline (mode 3; zero: none)
	unsigned int Unknown40;								// +0x40: a deadline (mode 2)
	unsigned int Unknown44;								// +0x44: a deadline (mode 4; zero: none)
	void *Unknown48;									// +0x48: the check thread's handle
	int Unknown4C;										// +0x4C: which deadline started it (1-4)
	std::string Unknown50;								// +0x50: the lkey URL's format (0x7CB610's; empty: no checks)
	std::map<std::string, std::string> Unknown60;		// +0x60: the settings (Set_String / Get_String's canonicalised keys and values)  // NAME 1: neutral (D10)
};
Unknown_7CA2D0Class *Unknown_7CA2D0();					// NAME 1 [Unknown_7CA2D0]: a function-static instance
#include "..\option.h"								// OptionClass, option.cpp's object at 0xBE8B38 (D107)
class FontDatabaseClass;								// fontdatabase.h's (P-H12-3)
extern FontDatabaseClass *fontDatabase;				// NAME 5: the client's FAIL_IF '!::fontDatabase' (ui_option_font.cpp 0x5CD780; P-CP2-1); fontdatabase.h declares it too

#endif
