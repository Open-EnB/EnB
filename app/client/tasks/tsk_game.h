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
void Unknown_43FD70();									// NAME 1 [Unknown_43FD70]: soundlib.cpp
class Unknown_7CA2D0Class								// NAME 1 [Unknown_7CA2D0Class]: the settings store 0x7CA2D0 returns
{
public:
	Unknown_7CA2D0Class();									// 0x7C97B0 (the constructor; the accessor 0x7CA2D0 runs it once)
	void Set_String(const char *key, const char *value);	// 0x7CCB30  // NAME 1: what it does
	void Set_Int(const char *key, int value);			// 0x7CCE80 ('%d')  // NAME 1: what it does
	void Set_Int64(const char *key, __int64 value);		// 0x7CCED0  // NAME 1: what it does
	void Unknown_7CB610(const char *url);				// 0x7CB610 (LoginTaskClass::Fetch_EA_Ticket: auth.ini's LKeyUrl)
	void Unknown_7CB6F0(int force);						// 0x7CB6F0 (the tasks' per-frame poll: the lkey check thread started when a deadline passed, its result collected)  // NAME 1 [force]: lets the +0x38 / +0x3C deadlines start it (the game task's 1)
	unsigned int Unknown_7CA5C0(int unknown, RenderObjClass *object, void *unknown2);	// 0x7CA5C0 (ui_registration.cpp 0x5FB030: 4, its 'LABEL_TEXT', NULL)  // NAME 1 [Unknown_7CA5C0, unknown, object, unknown2]: neutral (D10)
	std::string Get_String(const char *key);			// 0x7CCF20 (0x7CB8F0: 'lkey')  // NAME 1: what it returns
	static unsigned long __stdcall Unknown_7CB8F0(void *store);	// 0x7CB8F0: the check thread (WinInet, 'ENBCLIENT'; the reply starting 'Success': 1)  // NAME 1 [store]: CreateThread's parameter
	void Unknown_7CB3D0();								// 0x7CB3D0 (WinMain's shut-down: ShellExecuteEx, 'Earth & Beyond will now exit to allow you to check your E-Mail')  // NAME 1 [Unknown_7CB3D0]: neutral (D10)

	char Unknown00[0x2C];
	int Unknown2C;										// +0x2C: LoginTaskClass::Unknown_6C tests it
	char Unknown30[0x34 - 0x30];						// +0x30
	bool Unknown34;										// +0x34 (the chat panel's slot +0x68 0x563280: set, no Online Manual)
	char Unknown35[0x38 - 0x35];						// +0x35
	unsigned int Unknown38;								// +0x38: a GetTickCount deadline (mode 1)
	unsigned int Unknown3C;								// +0x3C: a deadline (mode 3; zero: none)
	unsigned int Unknown40;								// +0x40: a deadline (mode 2)
	unsigned int Unknown44;								// +0x44: a deadline (mode 4; zero: none)
	void *Unknown48;									// +0x48: the check thread's handle
	int Unknown4C;										// +0x4C: which deadline started it (1-4)
	std::string Unknown50;								// +0x50: the lkey URL's format (0x7CB610's; empty: no checks)
	std::map<std::string, std::string> Unknown60;		// +0x60: the settings (Set_String / Get_String's canonicalised keys and values)  // NAME 1: neutral (D10)
	char Unknown70[0x1B4 - 0x60 - sizeof(std::map<std::string, std::string>)];	// +0x70: the browser descriptors (0x7C97B0 fills them: not rebuilt)
};
Unknown_7CA2D0Class *Unknown_7CA2D0();					// NAME 1 [Unknown_7CA2D0]: a function-static instance
#include "..\option.h"								// OptionClass, option.cpp's object at 0xBE8B38 (D107)
class FontDatabaseClass;								// fontdatabase.h's (P-H12-3)
extern FontDatabaseClass *Unknown_BE67B8;				// NAME 1 [Unknown_BE67B8]: neutral (D10); fontdatabase.h declares it too

#endif
