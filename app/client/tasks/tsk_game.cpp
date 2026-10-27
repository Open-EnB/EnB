/*
**	Earth & Beyond client (reconstruction)
**	Original location: e:\build\beta\G\app\Client\tasks\tsk_game.cpp (client 0x765F80-0x767230)
**	ClientTaskClass (tsk_game.h). The asserts sit on the client's lines (D25). Notes at the end.
*/
#define ENABLE_WWPROFILE
#include "tsk_game.h"
#include "tsk_login.h"
#include "..\sclient.h"
#include "..\..\common\connect.h"
#include "..\..\packets\pk.h"
#include "..\..\shared\unknown_debug.h"
#include "..\..\..\wwlib\argv.h"
#include "..\..\..\wwlib\ini.h"
#include "wwprofile.h"
#include <time.h>

class MasterJoinPacketClass : public PacketClass { public: MasterJoinPacketClass(TicketClass *ticket); ~MasterJoinPacketClass(); char Unknown0C[0x54]; };	// opcode 53: 0x8974C0, 0x897650 (pk_classes.h: the receiving side only)
class LoginPacketClass : public PacketClass { public: LoginPacketClass(TicketClass *ticket, const char *gm_name, int unknown); ~LoginPacketClass(); char Unknown0C[0xAC]; };	// opcode 2: 0x89A740, 0x89A940  // NAME 1 [gm_name]: '-GM_NAME'
class ServerRedirectPacketClass : public PacketClass { public: ServerRedirectPacketClass(RawPacketClass *raw); virtual bool Process_Packet(void *client); int Unknown0C; unsigned long Address; unsigned short Port; };	// opcode 54: 0x8A1590, 0x18 bytes (pk_classes.h declares no fields)  // NAME 1 [Address, Port]: handed to Redirect
// 0x765F80
ClientTaskClass::ClientTaskClass(const std::string &username, const std::string &password) :
	Ticket(0),
	Mode(MODE_JOIN),
	RedirectCount(0),
	Master_Connection(NULL),
	Username(username),
	Password(password),
	Timeout(0)
{
}

// 0x7661C0
ClientTaskClass::~ClientTaskClass()
{
	if (Master_Connection) {
		delete Master_Connection;
		Master_Connection = NULL;
	}
	if (Client) {
		Unknown_BE8B38->Unknown_7F8EF0();
		delete Client;
		Client = NULL;
	}
}









// 0x766350
bool ClientTaskClass::Init(KernelClass *kernel, TicketClass *ticket, const std::string &login_key, unsigned long address)
{
	G_ASSERT(kernel && ticket);
	if (kernel == NULL || ticket == NULL) {
		return false;
	}
	Login_Key = login_key;
	Client = new SClient(this);
	Ticket = Client->Ticket = *ticket;
	Unknown_BE8B38->Unknown_7F84F0(ticket->Character_ID, Client);
	Unknown_BE67B8->Unknown_6C47B0();
	Unknown_43FD70();
	Client->Kernel = kernel;
	Client->Init();
	Address = address;
	Set_Mode(MODE_JOIN); Login_Key.size(); login_key.size();	// two dead size() calls: inline budget (D40's lever), the client calls string::erase out of line
	return true;
}

// 0x7665A0
bool ClientTaskClass::Accept_Focus()
{
	Unknown_7CA2D0()->Set_String("lkey", Login_Key.c_str());
	Unknown_7CA2D0()->Set_Int64("characterid", Ticket.Character_ID);
	return true;
}

// 0x7665F0
bool ClientTaskClass::Yield_Focus()
{
	Unknown_7CA2D0()->Set_Int("lkey", 0);
	Unknown_7CA2D0()->Set_Int("characterid", 0);
	return true;
}















// 0x766630
void ClientTaskClass::Run()
{
	WWPROFILE("ClientTask::Run");
	switch (Mode) {
		case MODE_JOIN:
			Join();
			break;

		case MODE_WAIT_FOR_REDIRECT:
			Wait_For_Redirect();
			break;

		case MODE_SECTOR:
			Sector();
			break;

		case MODE_LEAVING:
		{
			int pending = Unknown_82E1D0();
			if (time(NULL) > Timeout || pending == 0) {
				KernelClass *kernel = Kernel;
				kernel->Remove_Task(this);
				if (kernel->Get_Focus() == NULL) {
					if (Unknown_5FADE0()) {
						LoginTaskClass *task = new LoginTaskClass(7);
						task->Set_Step(0, false);
						kernel->Add_Task(task);
						kernel->Set_Focus(task);
					} else {
						kernel->Quit = true;
					}
				}
			}
			break;
		}

		default:
			G_ASSERT(false);
			break;
	}
}



// 0x7667F0
void ClientTaskClass::Join()
{
	G_ASSERT(Mode == MODE_JOIN);
	if (Master_Connection) {
		if (Master_Connection->Is_Dead()) {
			delete Master_Connection;
			Master_Connection = NULL;
			Unknown_Log("HANDOFF:WRN", "Master connection died!  About to reconnect....");
			Connect_Master();
		}
	} else {
		Connect_Master();
	}
	Master_Connection->Send_Packet(&MasterJoinPacketClass(&Ticket));
	Set_Mode(MODE_WAIT_FOR_REDIRECT);
}








// 0x7668E0
void ClientTaskClass::Connect_Master()
{
	G_ASSERT(Master_Connection == NULL);
	Unknown_Log("HANDOFF:INF", "Connecting to master server....");

	INIClass &ini = Get_Network_INI();
	int port = ini.Get_Int("MasterServer", "Port", 3801);
	if (Address == 0) {
		Master_Connection = Create_Connection("MasterServer", true);
	} else {
		Master_Connection = Create_Connection(Address, port, true, true);
	}

	if (Master_Connection && Master_Connection->Is_Connected()) {
		Unknown_Log("HANDOFF:INF", "Master connection successful....");
	} else {
		Unknown_Log("HANDOFF:ERR", "Master connection FAILED.");
	}



	G_ASSERT(Master_Connection);
}





// 0x7669F0
void ClientTaskClass::Wait_For_Redirect()
{
	G_ASSERT(Mode == MODE_WAIT_FOR_REDIRECT);
	if (Client) {
		Client->Unknown_64();
	}

	if (Master_Connection && !Master_Connection->Is_Dead()) {
		PacketClass *packet = Master_Connection->Receive();
		if (packet) {
			packet->Process_Packet(this);
			delete packet;
		}
		if (Master_Connection->Is_Dead()) {
			delete Master_Connection;
			Master_Connection = NULL;
		}
		return;
	}

	Set_Mode(MODE_JOIN);
	Join();
}







// 0x766AB0
void ClientTaskClass::Redirect(unsigned long ip, unsigned short port)
{
	G_ASSERT(Mode == MODE_WAIT_FOR_REDIRECT);
	Client->Open_Connection(ip, port);
	LoginPacketClass packet(&Ticket, ArgvClass().Find("-GM_NAME"), 0);
	Client->Send_Packet(&packet);
	Set_Mode(MODE_SECTOR);
}
















// 0x766BD0
void ClientTaskClass::Sector()
{
	G_ASSERT(Mode == MODE_SECTOR);
	G_ASSERT(Client);
	switch (Client->Do_Frame(Is_Focused())) {
		case 0:
			Unknown_7CA2D0()->Unknown_7CB6F0(0);
			break;

		case 1:
		case 5:
		{
			KernelClass *kernel = Kernel;
			kernel->Remove_Task(this);
			LoginTaskClass *task = new LoginTaskClass(1);
			task->Set_Username(Username.c_str());
			task->Set_Password(Password.c_str());
			kernel->Add_Task(task);
			kernel->Set_Focus(task);
			break;
		}

		case 2:
		{
			KernelClass *kernel = Kernel;
			kernel->Remove_Task(this);
			LoginTaskClass *task = new LoginTaskClass(7);
			task->Set_Username(Username.c_str());
			task->Set_Password(Password.c_str());
			if (task->Unknown_64()) {
				task->Set_Step(7, false);
			} else {
				task->Set_Step(1, false);
			}
			kernel->Add_Task(task);
			kernel->Set_Focus(task);
			break;
		}

		case 3:
			Mode = MODE_LEAVING;
			Timeout = time(NULL) + 5;
			break;

		case 4:
			Leave_Sector();
			break;












		default:
			G_ASSERT(0);
	}
}

// 0x766F10
void ClientTaskClass::Render()
{
	G_ASSERT(Client);
	if (Mode == MODE_SECTOR || Client->LoadingView->Unknown68) {
		Client->Unknown_64();
	}
}

// 0x766F80
bool ClientTaskClass::Shutdown()
{
	G_ASSERT(Client);
	Client->Shutdown();
	return true;
}





// 0x766FC0
bool ServerRedirectPacketClass::Process_Packet(void *client)
{
	ClientTaskClass *task = (ClientTaskClass *)client;
	Unknown_Log("HANDOFF:INF", "Redirect packet received.");
	G_ASSERT(task);
	task->Redirect(Address, Port);
	return true;
}





// 0x767030
void ClientTaskClass::Leave_Sector()
{
	G_ASSERT(Mode == MODE_SECTOR);
	Ticket = Client->Ticket;
	Set_Mode(MODE_JOIN);
	Unknown_7CA2D0()->Unknown_7CB6F0(1);
}














// 0x7670A0
void ClientTaskClass::Set_Mode(int mode)
{
	Mode = mode;
	switch (mode) {
		case MODE_WAIT_FOR_REDIRECT:
			if (++RedirectCount > 50) {
				G_ASSERT(RedirectCount <= 50);
				KernelClass *kernel = Kernel;
				kernel->Remove_Task(this);
				LoginTaskClass *task = new LoginTaskClass(1);
				task->Set_Username(Username.c_str());
				task->Set_Password(Password.c_str());
				kernel->Add_Task(task);
				kernel->Set_Focus(task);
			}
			break;

		case MODE_SECTOR:
			RedirectCount = 0;
			break;
	}
}

// 0x7671E0
int ClientTaskClass::Get_Interval()
{
	if (Kernel && Kernel->Render_Enabled && Kernel->Device_Ok) {
		return 0;
	}
	return 500;
}

/*
**	Reconstruction notes (client.exe 0x765F80-0x767230; vtable 0xB03FF4, slots in tsk_game.h).
**	LoginTaskClass's step 5 (0x769E70) makes the task with the login's name and password and calls
**	Init with the kernel, the ticket, the 'lkey' string and the master server's address. Sector
**	inlines Leave_Sector (line 363 appears in both). After Get_Interval: a guarded static (atexit
**	0x42B320, <string>'s std::ctype id, every tasks object has it).
*/
