// Earth & Beyond client (reconstruction): the message server task (msgserv.h). Notes at the end of the file.
#include "msgserv.h"
#include <winsock.h>
#include <string.h>
#include "..\g_assert.h"
#include "..\kv_encoder_base.h"
#include "..\..\..\wwdebug\wwprofile.h"
























// 0x8B20D0
MsgServer::MsgServer()
{
	WSADATA data;
	WSAStartup(0x101, &data);
}

// 0x8B21B0
MsgServer::~MsgServer()
{
	WSACleanup();
}

// 0x8B2230
void MsgServer::Unknown_8B2230()
{
	G_ASSERT(Get_Internal_Server_Protocol() == NULL);
	Server.Unknown_4BCCB0();
	Set_Internal_Server_Protocol(&Server);
}

// 0x8B2280
bool MsgServer::Shutdown()
{
	Set_Internal_Server_Protocol(NULL);
	return true;
}

// 0x8B22A0
int MsgServer::Get_Interval()
{
	return 200;
}

// 0x8B22C0
void MsgServer::Run()
{
	WWPROFILE("MsgServer::Run");
	Server.Unknown_4BCD30();
}

// 0x8B2350
void MsgServer::Describe(void *table)
{
	((KVEncoderBaseClass *)table)->Add(0x3FF, "TaskType", "MessageServer", strlen("MessageServer") + 1);
}

/*
**	Reconstruction notes
**
**	client.exe 0x8B20D0-0x8B2379 (the object ends at configfile.cpp's 0x8B23C0, wlib): the task's
**	constructor (TaskClass, the server member, WSAStartup(1.1) into a stack WSADATA), the inline
**	Delete_On_Remove and the deleting destructor the vtable 0xB0C418 emits after it, the destructor
**	(WSACleanup), 0x8B2230 (WinMain's call after new: the assert, Unknown_4BCCB0's listen on the first
**	free port of 9999-10009 (TCPMgr::addListener(0, ...): every interface, the client's choice), then
**	the server goes to 0xBE55D0), Shutdown, Get_Interval (200), Run (profiled; the server's pump
**	0x4BCD30) and Describe (KVEncoderBaseClass::Add 0x4B4A70 of 'TaskType'). 0x8B2390 after them (a
**	guarded atexit of an empty destructor, 0xC416FC) is a header static's: not rebuilt.
**
**	The protocol (message*.cpp): a listener connects and sends filter lines, each '\0'-ended:
**	'S' name '`' type '`' modules '`' files '`' prefix (Wstring::getToken skips empty fields, so each
**	needs a character: ' ' is "any"; type is a mask of the levels 1 debug, 2 info, 4 warn, 8 error,
**	0x10 stat, 0x20 0x4BC3A0's; 0xFFFFFF clears its filters). The server sends each message the filters
**	take as 'M' level '`' sender[,category] '`' file '`' line '`' x '`' y '`' sec.usec '`' text '\0'.
**	The reporters drop everything while no listener is connected (0x4BCD10 is the listeners' count).
*/
