/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
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

class ConnectionClass;
class SClient;

// The login ticket (vtable 0xB0C608, 0x48 bytes; constructor 0x8C15F0, destructor 0x8C16D0, assignment
// 0x8C16F0): the master server's answer to the login, copied into SClient (+0x12D8) and the task.
#pragma pack(push, 4)		// the client's layout: the vtable pointer isn't padded for Character_ID
class TicketClass	// NAME 1: Init's assert 'kernel && ticket'
{
public:
	TicketClass(int unknown);							// 0x8C15F0 (ClientTask passes 0)
	virtual ~TicketClass();								// 0x8C16D0
	TicketClass &operator=(const TicketClass &that);	// 0x8C16F0

	int Unknown04;										// +0x04 (the constructor's argument)
	int Unknown08;										// +0x08
	int Unknown0C;										// +0x0C
	__int64 Character_ID;								// +0x10 NAME 2: stored as 'characterid' (Accept_Focus)
	int Unknown18[6];									// +0x18
	int Unknown30[5];									// +0x30
	bool Unknown44;										// +0x44
};
#pragma pack(pop)

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
INIClass &Get_Network_INI();							// 0x821B40 (connect.cpp)
void Unknown_Log(const char *tag, const char *format, ...);	// NAME 1 [Unknown_Log]: 0x7E5F90, the 'HANDOFF:INF' / 'LOGIN:INF' log
int Unknown_82E1D0();									// NAME 1 [Unknown_82E1D0]: udpconn.cpp
bool Unknown_5FADE0();									// NAME 1 [Unknown_5FADE0]: ui_registration.cpp
void Unknown_43FD70();									// NAME 1 [Unknown_43FD70]: soundlib.cpp
class Unknown_7CA2D0Class								// NAME 1 [Unknown_7CA2D0Class]: the settings store 0x7CA2D0 returns
{
public:
	void Set_String(const char *key, const char *value);	// 0x7CCB30  // NAME 1: what it does
	void Set_Int(const char *key, int value);			// 0x7CCE80 ('%d')  // NAME 1: what it does
	void Set_Int64(const char *key, __int64 value);		// 0x7CCED0  // NAME 1: what it does
	void Unknown_7CB6F0(int unknown);					// 0x7CB6F0

	char Unknown00[0x2C];
	int Unknown2C;										// +0x2C: LoginTaskClass::Unknown_6C tests it
};
Unknown_7CA2D0Class *Unknown_7CA2D0();					// NAME 1 [Unknown_7CA2D0]: a function-static instance
class Unknown_BE8B38Class								// NAME 1 [Unknown_BE8B38Class]: option.cpp's object at 0xBE8B38
{
public:
	void Unknown_7F84F0(__int64 character_id, SClient *client);	// 0x7F84F0 (option.cpp)
	void Unknown_7F8EF0();								// 0x7F8EF0
};
extern Unknown_BE8B38Class *Unknown_BE8B38;				// NAME 1 [Unknown_BE8B38]: neutral (D10)
class Unknown_BE67B8Class								// NAME 1 [Unknown_BE67B8Class]: fontdatabase.cpp's object at 0xBE67B8
{
public:
	void Unknown_6C47B0();								// 0x6C47B0 (fontdatabase.cpp)
};
extern Unknown_BE67B8Class *Unknown_BE67B8;				// NAME 1 [Unknown_BE67B8]: neutral (D10)

#endif
