/*
**	Earth & Beyond client (reconstruction)
**	Original location: e:\build\beta\G\app\Client\tasks\tsk_login.cpp (client 0x767240-0x76BC40)
**	LoginTaskClass (tsk_login.h), partly rebuilt. The asserts sit on the client's lines (D25).
*/
#define ENABLE_WWPROFILE
#include "tsk_login.h"
#include "tsk_game.h"
#include "..\..\common\connect.h"
#include "..\..\packets\pk.h"
#include "..\..\packets\pkglobalticketrequest.h"
#include "..\..\packets\pkversionrequest.h"
#include "..\..\packets\pkunknown_866ac0.h"
#include "..\..\shared\unknown_debug.h"
#include "..\..\shared\auxdata\unknown_root.h"
#include "ww3d.h"
#include "wwprofile.h"
#include "nstrdup.h"
#include "ini.h"
#include "argv.h"
#include "wwstring.h"
#include <stdio.h>
#include <string.h>

void Unknown_Debug_Print(const char *text);			// NAME 1 [Unknown_Debug_Print]: 0x509E30 (dbgmgr.cpp), the log line
class GlobalCreateCharacterPacketClass : public PacketClass { public: GlobalCreateCharacterPacketClass(int galaxy, int slot, const char *name, int a, int b, int c); ~GlobalCreateCharacterPacketClass(); char Unknown0C[0x2BC]; };	// opcode 114: 0x864090, 0x864590 (pk_classes.h: the receiving side only)
class GlobalConnectPacketClass : public PacketClass { public: GlobalConnectPacketClass(const char *ticket, const char *password); ~GlobalConnectPacketClass(); char Unknown0C[0x08]; };	// opcode 109: 0x863400, 0x863570  // NAME 1 [password]: '123456'
void Unknown_4BC000(const char *sender, const char *category, const char *file, int line, const char *format, ...);	// NAME 1 [Unknown_4BC000]: 0x4BC000, E&B's message reporter (tcpconn.cpp)
static char *AUTH_INI = "auth.ini";				// 0xB9B9D0  // NAME 1: its text
static char *LOGIN_INI = "login.ini";				// 0xB9B9D4  // NAME 1: its text
static char *LOGIN_DATA = "LOGIN_DATA";			// 0xB9B9D8  // NAME 1: its text
static char *LOGIN_NAME = "Name";					// 0xB9B9DC  // NAME 1: its text
static char *LOGIN_NAME_DEFAULT = "Click to enter name";	// 0xB9B9E0  // NAME 1: its text
static char *LOGIN_PASSWORD_DEFAULT = "";			// 0xB9B9E4  // NAME 1: Load_Login's second default
struct Unknown_VersionStruct { int Major; int Minor; };	// NAME 1 [Unknown_VersionStruct]: the version 0x4456C0 returns (0xBDBE78)
Unknown_VersionStruct *Unknown_4456C0();				// NAME 1 [Unknown_4456C0]: neutral (D10)
class Unknown_8190D0Class { public: void Unknown_8191B0(); };	// NAME 1 [Unknown_8190D0Class]: what 0x8190D0 returns
Unknown_8190D0Class *Unknown_8190D0(int unknown);		// NAME 1 [Unknown_8190D0]: neutral (D10)
class Unknown_6A9B60Class { public: Unknown_6A9B60Class(int unknown); ~Unknown_6A9B60Class(); void Unknown_6A9E40(); char Unknown00[0x10]; };	// NAME 1 [Unknown_6A9B60Class]: 0x10 bytes, constructor 0x6A9B60, destructor 0x6A9DB0
void Unknown_723150();								// NAME 1 [Unknown_723150]: neutral (D10); the destructor calls it
extern bool Unknown_BB9654;							// NAME 1 [Unknown_BB9654]: neutral (D10); Accept_Focus clears it
class GlobalErrorPacketClass : public PacketClass { public: GlobalErrorPacketClass(RawPacketClass *raw); virtual bool Process_Packet(void *client); int Unknown0C; int Error; std::string Message; };	// opcode 117: 0x864C90, 0x24 bytes (pk_classes.h declares no fields)  // NAME 1 [Error, Message]: the log 'GlobalErrorPacketClass recv: %d : %d : %s'
class GlobalTicketPacketClass : public PacketClass { public: GlobalTicketPacketClass(RawPacketClass *raw); virtual bool Process_Packet(void *client); int Status; TicketClass Ticket; };	// opcode 111: 0x867650, 0x58 bytes  // NAME 1 [Status, Ticket]: 'Got the global ticket: %d', copied to the task's Ticket
class Packet_0076Class : public PacketClass { public: Packet_0076Class(RawPacketClass *raw); virtual bool Process_Packet(void *client); int Unknown0C; LoginTicketInfoClass Ticket_Info; int Unknown0AC; short Unknown0B0; int Unknown0B4; int Unknown0B8; int Unknown0BC; bool Is_Unknown0B0() const { return Unknown0B0 != 0; } int Get_Unknown0B4() const { return Unknown0B4 - Unknown0B8; } };	// opcode 118 (the client's log: GlobalLoginPacketClass): 0x866050, 0xC0 bytes  // NAME 1 [Ticket_Info, Is_Unknown0B0, Get_Unknown0B4]: copied whole to the task's; the two accessors (inline budget: 0x76B170 calls string::assign out of line only with them)
class Packet_0077Class : public PacketClass { public: Packet_0077Class(const char *name); virtual ~Packet_0077Class() {} virtual RawPacketClass *Build(); virtual bool Process_Packet(void *client); virtual int Get_Data_Size(); virtual int Print(char *buffer, int len); std::string Name; int Unknown1C; };	// opcode 119: 0x866AC0 (the sending constructor), 0x20 bytes; its destructor 0x76B400 is emitted here  // NAME 1 [name]: the referrer's name (0x55D060)






















































// 0x767A20
LoginTaskClass::~LoginTaskClass()
{
	Delete_Character_View();
	Unknown_723150();
	delete Unknown0018;
	delete Unknown000C;
	delete GlobalConnection;
	delete Username;
	delete Password;
}







































// 0x767CE0
bool LoginTaskClass::Accept_Focus()
{
	Unknown_Debug_Print("LoginTask: accepting focus.");
	if (CharacterView) {
		Unknown0018->Unknown_515340(CharacterView->Unknown_3C());
	}
	Unknown_BB9654 = false;
	Done = false;
	return true;
}

// 0x767D30
bool LoginTaskClass::Yield_Focus()
{
	if (Done) {
		Unknown_Debug_Print("LoginTask: yielding focus.");
		return true;
	}
	Unknown_Debug_Print("LoginTask: F* that! Not yielding focus.");
	return false;
}

// 0x767D70
int LoginTaskClass::Get_Interval()
{
	if (Kernel && Kernel->Render_Enabled && Kernel->Device_Ok) {
		return 0;
	}
	return 500;
}













































































// 0x767EE0
void LoginTaskClass::Render()
{
	WW3D::Sync(Unknown_Get_Ticks());
	if (Step < 16) {
		FAIL_IF(!CharacterView) return;
		CharacterView->Unknown_0C(true);
	}
}




// NOT EXACT: register-only (stricmp's argument in ecx, the client's eax; attempts.py show 0x767F50)



// 0x767F50
void LoginTaskClass::Run()
{
	WWPROFILE("LoginTask::Run");
	Unknown_64();
	if (Unknown0014) {
		FAIL_IF(!CharacterView) return;
		CharacterView->Unknown_67A300();
		if (Step < 16) {
			CharacterView->Unknown_67A370(Step);
			Unknown0018->Unknown_515340(CharacterView->Unknown_3C());
		}
		Unknown0014 = false;
	}

	if (Unknown0030 && (GlobalConnection == NULL || !GlobalConnection->Is_Connected()) &&
		Step != 16 && Step != 15 && Step != 17 && Step != 9 && Step != 10 && Step != 11) {
		Show_Message("Lost connection to the global server.");
	}

	if (CharacterView && CharacterView->Unknown034 && CharacterView->Unknown034->Unknown07C) {
		CharacterView->Unknown_14();
		if (stricmp(CharacterView->Unknown034->Unknown18C, "HTML") == 0 && !Unknown0018->Unknown24) {
			Unknown0018->Unknown_5153B0();
		}
		if (CharacterView->Unknown034->Unknown_5890B0("OK") || CharacterView->Unknown034->Unknown_5890B0("OK_SMALL")) {
			CharacterView->Unknown034->Unknown_5C();
			if (Step == 15) {
				Step = 17;
			}
		}
		if (CharacterView->Unknown034->Unknown_5890B0("CANCEL")) {
			CharacterView->Unknown034->Unknown_5C();
			if (Step == 1) {
				Step = 17;
			}
		}
		if (CharacterView->Unknown034->Unknown_5890B0("HTML")) {
			CharacterView->Unknown034->Unknown_5C();
			if (Unknown0018->Unknown24) {
				Unknown0018->Unknown_515400();
			}
			if (Step == 15) {
				Step = 17;
			}
		}
	}

	switch (Step) {
		case 0:
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
		case 9:
		case 10:
		case 11:
		case 13:






























			FAIL_IF(!CharacterView) return;
			CharacterView->Unknown_14();
			break;

		case 7:
			if (!Unknown0030) {
				if (CharacterView) {
					CharacterView->Unknown_14();
				}
			} else if (Unknown_4C()) {
				Set_Step(1, false);
			} else {
				Set_Step(8, false);
			}
			break;

		case 8:
			if (!Unknown001D) {








				FAIL_IF(!CharacterView) return;
				CharacterView->Unknown_14();
			} else {
				Unknown001D = false;
				Set_Step(2, false);
			}
			break;

		case 16:
			Start_Game();
			break;

		case 17:
			Kill_Kernel();
			break;

		case 18:
			Unknown_76A370();
			break;
	}

	if (Unknown0018) {
		Unknown0018->Unknown_515440();
	}
	Unknown_7CA2D0()->Unknown_7CB6F0(0);
	Process_Packets();
}


























// 0x7683A0
void LoginTaskClass::Process_Packets()
{
	if (GlobalConnection) {
		PacketClass *packet;
		while ((packet = GlobalConnection->Receive()) != NULL) {
			packet->Process_Packet(this);
			delete packet;
		}
	}
}






















// 0x7689E0
void LoginTaskClass::Unknown_48(int a, int b, int galaxy, int c)
{
	if (GlobalConnection) {
		FAIL_IF(Galaxies.empty()) return;
		if (galaxy < 0) {
			galaxy = Galaxies.front().ID;
		}
		GlobalConnection->Send_Packet(&GlobalCreateCharacterPacketClass(galaxy, Selected, Username, a, b, c));
	}
}













// 0x768B00
bool LoginTaskClass::Unknown_4C()
{
	EATicket[0] = 0;
	Unknown_Log("LOGIN:INF", "Fetching EA.Com ticket.");
	if (!Fetch_EA_Ticket()) {
		Unknown_Log("LOGIN", "Failed to fetch EA.Com ticket.");
		return true;
	}
	Unknown_Log("LOGIN:INF", "EA.Com ticket fetched.");
	Unknown_4BC000("", "", __FILE__, __LINE__, "EA Ticket: %s", EATicket);
	if (GlobalConnection == NULL) {
		return true;
	}
	GlobalConnection->Send_Packet(&GlobalConnectPacketClass(EATicket, "123456"));
	Save_Login_Name();
	return false;
}



























































// 0x7692A0
void LoginTaskClass::Unknown_7692A0()
{
	if (GlobalConnection) {
		GlobalConnection->Send_Packet(&GlobalTicketRequestPacketClass(Selected));
		Unknown_Log("LOGIN:INF", "Avatar %d selected.  Ticket request sent.", Selected);
	}
}






// 0x769360
void LoginTaskClass::Unknown_6C()
{
	if (CharacterView && Unknown_7CA2D0()->Unknown2C == 0 && Unknown_5FADE0()) {
		CharacterView->Unknown1CD = true;
		CharacterView->Unknown1CE = true;
		CharacterView->Unknown1CF = true;
		CharacterView->Unknown1D0 = true;
		Step = 0;
		Unknown0014 = true;
		return;
	}
	Step = 17;
}

// 0x7693E0
void LoginTaskClass::Unknown_7693E0()
{
	Step = 17;
}



// 0x769400
void LoginTaskClass::Unknown_5C()
{
	for (std::list<LoginGalaxyClass>::iterator it = Galaxies.begin(); it != Galaxies.end(); it++) {
		if (it->ID == Characters[Selected].Galaxy_ID) {
			Galaxy_Address = it->Address;
		}
	}
	if (Galaxy_Address == 0) {
		Show_Message("The galaxy this character resides on is currently unavailable.  Please try again later.", "OK");
	} else {
		Unknown_7692A0();
		Step = 16;
	}
}





// 0x7694D0
void LoginTaskClass::Set_Step(int step, bool force)
{
	if (Step < 16 || force) {
		Unknown0014 = true;
		Step = step;
	}
}

// NOT EXACT: register-only (the client keeps 1 in ebx and the slot's address; attempts.py show 0x769500)
// 0x769500
void LoginTaskClass::Go_Next_Step()
{
	if (Step < 16) {
		Previous_Step = Step;
		switch (Step) {
			case 0:
			case 7:
			case 8:
			case 9:
			case 10:
			case 11:
				LoginTaskClass::Set_Step(1, false);
				break;

			case 1:
				LoginTaskClass::Set_Step(2, false);
				break;

			case 2:
				if (Characters[Selected].ID == 0) {
					LoginTaskClass::Set_Step(3, false);
					break;
				}
				G_ASSERT(!Is_Registered_Character());
				LoginTaskClass::Set_Step(5, false);
				break;

			case 3:
				LoginTaskClass::Set_Step(4, false);
				break;

			case 4:
				LoginTaskClass::Set_Step(6, false);
				break;

			case 5:
				Unknown_5C();
				Unknown0014 = true;
				break;

			case 6:
				LoginTaskClass::Set_Step(5, false);
				break;

			default:













				G_ASSERT(!"Unknown state in LoginTaskClass::Go_Next_Step");
				Unknown0014 = true;
				break;
		}
	}
}

// NOT EXACT: register-only (the client keeps 1 in ebx for the flag and step 1; attempts.py show 0x769680)













// 0x769680
void LoginTaskClass::Go_Back_Step()
{
	if (Step < 16 && Step != 10 && Step != 11 && Step != 9) {
		Previous_Step = Step;
		switch (Step) {
			case 0:
				LoginTaskClass::Set_Step(17, false);
				break;

			case 1:
				LoginTaskClass::Set_Step(0, false);
				break;

			case 2:
			case 7:
			case 8:
				LoginTaskClass::Set_Step(1, false);
				break;

			case 3:
				LoginTaskClass::Set_Step(2, false);
				break;

			case 4:
				LoginTaskClass::Set_Step(3, false);
				break;

			case 5:
				LoginTaskClass::Set_Step(6, false);
				break;

			case 6:
				LoginTaskClass::Set_Step(4, false);
				break;

			default:
				G_ASSERT(!"Unknown state in LoginTaskClass::Go_Back_Step");
				Unknown0014 = true;
				break;
		}
	}
}


























// 0x7697A0
void LoginTaskClass::Set_Username(const char *name)
{
	delete Username;
	Username = nstrdup(name);
}

// 0x7697E0
void LoginTaskClass::Set_Password(const char *password)
{
	delete Password;
	Password = nstrdup(password);
}

// 0x769820
void LoginTaskClass::Show_Message(const char *text)
{
	if (CharacterView) {
		CharacterView->Unknown034->Unknown_5C();
		CharacterView->Unknown034->Unknown_588A70("OK", 0, text, 320, 0, 0);
	}
	Step = 15;
}

// 0x769870
void LoginTaskClass::Show_Message(const char *text, const char *button)
{
	if (CharacterView) {
		CharacterView->Unknown034->Unknown_5C();
		if (text && strlen(text) > 0) {
			CharacterView->Unknown034->Unknown_588A70(button, 0, text, 320, 0, 0);
		}
	}
}

// NOT EXACT: shape (the client shares its 'return true' tails; attempts.py show 0x7698E0)
// 0x7698E0
bool LoginTaskClass::Unknown_64()
{
	if (Unknown0030 || Step == 16 || Step == 15 || Step == 17 || Step == 0) {
		return false;
	}

	const char *protocol = ArgvClass().Find_Value("-PROTOCOL");
	if (protocol && (stricmp(protocol, "UDP") == 0 || stricmp(protocol, "TCP") == 0)) {
		if (GlobalConnection == NULL) {
			GlobalConnection = Create_Connection("GlobalServer_Client", false);
			G_ASSERT(GlobalConnection);
			char text[128];
			sprintf(text, "Connecting to server...");
			Show_Message(text, "CANCEL");
		}
		if (!GlobalConnection->Is_Connected()) {
			if (GlobalConnection->Connect(false)) {
				if (!GlobalConnection->Is_Connected()) {
					Show_Message("Connection to the global server failed.");
					return false;
				}
				if (CharacterView) {
					CharacterView->Unknown034->Unknown_5C();
				}
				Unknown0030 = true;
				Unknown_VersionStruct *version = Unknown_4456C0();
				GlobalConnection->Send_Packet(&VersionRequestPacketClass(version->Major, version->Minor));
			}
			return true;
		}
		if (Unknown0030) {
			return false;
		}









		G_ASSERT(0);
		if (CharacterView) {
			CharacterView->Unknown034->Unknown_5C();
		}
		Unknown0030 = true;
		return false;
	}

	if (GlobalConnection && GlobalConnection->Is_Dead()) {
		delete GlobalConnection;
		GlobalConnection = NULL;
	}
	if (GlobalConnection) {
		return true;
	}
	GlobalConnection = Create_Connection("GlobalServer_Client", true);





	G_ASSERT(GlobalConnection);
	if (GlobalConnection && GlobalConnection->Is_Connected()) {
		Unknown0030 = true;
		Unknown_VersionStruct *version = Unknown_4456C0();
		GlobalConnection->Send_Packet(&VersionRequestPacketClass(version->Major, version->Minor));
		return true;
	}
	Show_Message("Couldn't connect to the server, try again later.");
	return false;
}
















// 0x769CE0
void LoginTaskClass::Close_Global_Connection()
{
	if (GlobalConnection) {
		GlobalConnection->Disconnect();
		delete GlobalConnection;
		GlobalConnection = NULL;
		Unknown0030 = false;
	}
}

// 0x769D20
void LoginTaskClass::Delete_Character_View()
{
	if (CharacterView) {
		CharacterView->Unknown_18(0);
		CharacterView->Unknown_10();
		delete CharacterView;
		CharacterView = NULL;
	}
}

// 0x769D60
void LoginTaskClass::Create_Character_View()
{
	if (CharacterView == NULL) {
		CharacterView = new CharacterViewClass(this, Step);
		FAIL_IF(!CharacterView) return;
		CharacterView->Unknown_67A370(Step);
		Unknown0018->Unknown_515340(CharacterView->Unknown_3C());
	}
}

// 0x769E40
void LoginTaskClass::Kill_Kernel()
{
	Unknown_Debug_Print("LoginTask: killing kernel and exiting the game");
	Done = true;
	Kernel->Quit = true;
}






































































































































































































































// 0x76AD30
void LoginTaskClass::Save_Login_Name()
{
	INIClass ini(LOGIN_INI);
	ini.Put_String(LOGIN_DATA, LOGIN_NAME, Username);
	ini.Save(LOGIN_INI);
}

// 0x76ADD0
void LoginTaskClass::Load_Login()
{
	INIClass ini(LOGIN_INI);
	if (ini.Is_Loaded()) {
		StringClass name;
		ini.Get_String(name, LOGIN_DATA, LOGIN_NAME, LOGIN_NAME_DEFAULT);
		Set_Username(name);
		Set_Password(LOGIN_PASSWORD_DEFAULT);
	} else {
		Set_Username(LOGIN_NAME_DEFAULT);
		Set_Password(LOGIN_PASSWORD_DEFAULT);
	}
	const char *login = ArgvClass().Find_Value("-LOGIN");
	if (login) {
		Set_Username(login);
	}
	const char *password = ArgvClass().Find_Value("-PASSWORD");
	if (password) {
		Set_Password(password);
	}
}

// NOT EXACT: shape (the CharacterView test's registers: the client loads +0x1B4 into ecx; attempts.py show 0x76B000)
// 0x76B000
void LoginTaskClass::Handle_Global_Error(GlobalErrorPacketClass *packet)
{
	if (CharacterView && CharacterView->Unknown1B4 && CharacterView->Unknown1B4->Unknown_78(packet)) {
		return;
	}
	Unknown0020 = 2;
	Error = packet->Error;
	Unknown_Debug_Print(packet->Message.c_str());
	if ((packet->Error < 8 || packet->Error > 13) && packet->Error != 17 && packet->Error != 3 && packet->Error != 5) {
		Show_Message(packet->Message.c_str());
		return;
	}
	Show_Message(packet->Message.c_str(), "OK");
	if (Step >= 7 && Step <= 8) {
		Set_Step(1, false);
	}
}

// 0x76B170
void LoginTaskClass::Handle_Global_Login(Packet_0076Class *packet)
{
	Ticket_Info = packet->Ticket_Info;
	Unknown1138 = packet->Is_Unknown0B0();
	Unknown113C = packet->Get_Unknown0B4();
	if (Unknown_5FADE0()) {
		Unknown1138 = false;
	}
}

// 0x76B2F0
void LoginTaskClass::Handle_Referrer_Response(Packet_0078Class *packet)
{
	Referrer_Result = packet->Result;
	if (Referrer_Result == 1 || Referrer_Result == 2) {
		Unknown1138 = false;
	}
}

// 0x76B330
void LoginTaskClass::Send_Referrer(const char *name)
{
	Referrer_Result = 0;
	GlobalConnection->Send_Packet(&Packet_0077Class(name));
}























// 0x76B4A0
bool GlobalErrorPacketClass::Process_Packet(void *client)
{
	LoginTaskClass *task = (LoginTaskClass *)client;
	G_ASSERT(task);
	Unknown_Log("LOGIN:WRN", "GlobalErrorPacketClass recv: %d : %d : %s", Unknown0C, Error, Message.c_str());
	task->Handle_Global_Error(this);
	return true;
}




























// 0x76B730
bool GlobalTicketPacketClass::Process_Packet(void *client)
{
	LoginTaskClass *task = (LoginTaskClass *)client;
	G_ASSERT(task);
	Unknown_Log("LOGIN:INF", "Got the global ticket: %d", Status);
	if (Status >= 1000) {
		switch (Status) {
			case 1000:
				Unknown_Debug_Print("Error: AvatarSelectionTask-User not authorized");
				task->Show_Message("Error: You have not been authorized to play this character.");
				break;

			case 1002:
				Unknown_Debug_Print("Error: AvatarSelectionTask-Shard is full");
				task->Show_Message("Error: This galaxy is full, please select another character.", "OK");
				task->Set_Step(2, true);
				break;
		}
	} else {
		task->Ticket = Ticket;
	}
	return true;
}






// 0x76B8A0
bool VersionResponsePacketClass::Process_Packet(void *client)
{
	LoginTaskClass *task = (LoginTaskClass *)client;
	FAIL_IF(!task) return true;
	switch (Status) {
		case 0:
			return true;

		case 1:
			Unknown_Debug_Print("Error: AvatarSelectionTask-Version too old");
			task->Show_Message("Error: Your client is out of date. Please run the patch utility.");
			break;

		case 2:
			Unknown_Debug_Print("Error: AvatarSelectionTask-Version too new");
			task->Show_Message("Error: Your client requires a more recent version of the server.");
			break;

		default:


			G_ASSERT(!"Unhandled status in version response packet");
			return true;
	}
	return true;
}



// 0x76B9C0
bool Packet_0076Class::Process_Packet(void *client)
{
	LoginTaskClass *task = (LoginTaskClass *)client;
	G_ASSERT(task);
	Unknown_Log("LOGIN:INF", "GlobalLoginPacketClass recv");
	task->Handle_Global_Login(this);
	return true;
}





// 0x76BB70
bool Packet_0078Class::Process_Packet(void *client)
{
	LoginTaskClass *task = (LoginTaskClass *)client;
	G_ASSERT(task);
	Unknown_Log("LOGIN:INF", "GlobalSetReferrerResponsePacketClass recv");
	task->Handle_Referrer_Response(this);
	return true;
}

/*
**	Reconstruction notes (client.exe 0x767240-0x76BC40; vtable 0xB040D4, slots in tsk_login.h).
**	Not rebuilt yet: the two constructors 0x767240 / 0x7675D0 (they inline Create_Character_View,
**	line 947) and so the vtable's inline slots, Pre_Run 0x767DA0, 0x7683F0, 0x768CB0, 0x769110 (the
**	galaxy list's operator=), step 16 0x769E70 (Start_Game: a register-only near miss, stashed in
**	docs/attempts/769E70), 0x76A370, the EA.Com ticket 0x76A7B0 (kcom's authFactory / iauth), 0x76AC60,
**	and the avatar list's handler 0x76B520. Near misses in the tree: attempts.py list --lib app.
**	Go_Next_Step / Go_Back_Step inline Set_Step (called through the class: it is virtual), Unknown_5C
**	inlines Show_Message(text, button), Packet_0078Class::Process_Packet Handle_Referrer_Response,
**	Packet_0076Class::Process_Packet Handle_Global_Login. The packet classes declared at the top
**	(opcodes 111, 117, 118, 119) are views with the fields these handlers read; pk_classes.h declares
**	them without fields (they belong there once app/packets decodes them).
*/
