// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Client\tasks\ (client 0x76BC50-0x76BF2F). The object has
**	no __FILE__ string; E&B's objects are linked in alphabetical order and this one lies between
**	tsk_login.cpp's and tsk_movie.cpp's, so the name sorts between those two.
**
**	MenuTask (tsk_menu.h). Notes at the end.
*/



#include "tsk_menu.h"
#include "tsk_login.h"

// NAME 1 [tsk_menu.cpp]: neutral name from the class's log text, sorting between tsk_login.cpp and tsk_movie.cpp (the link order)
void Unknown_Debug_Print(const char *text);			// NAME 1 [Unknown_Debug_Print]: 0x509E30 (dbgmgr.cpp), the log line
void Unknown_Debug_Begin_Step(const char *text);	// NAME 1 [Unknown_Debug_Begin_Step]: 0x509EF0 (dbgmgr.cpp), WinMain's step log
void Unknown_Debug_End_Step(const char *text);		// NAME 1 [Unknown_Debug_End_Step]: 0x50A1E0 (dbgmgr.cpp)
bool Unknown_5FAE70();								// NAME 1 [Unknown_5FAE70]: ui_registration.cpp
bool Unknown_5FAD50();								// NAME 1 [Unknown_5FAD50]: ui_registration.cpp

// 0x76BC50
MenuTaskClass::MenuTaskClass() :
	Done(false)
{
}

// 0x76BD10
bool MenuTaskClass::Accept_Focus()
{
	Unknown_Debug_Print("MenuTask: accepting focus.");
	return true;
}

// 0x76BD30
bool MenuTaskClass::Yield_Focus()
{
	if (Done) {
		Unknown_Debug_Print("MenuTask: yielding focus.");
		return true;
	}
	Unknown_Debug_Print("MenuTask: F* That! not yielding focus.");
	return false;
}

// 0x76BD70
void MenuTaskClass::Pre_Run()
{
	KernelEventClass event;
	event.Key = 0x43;
	Handle_Input(&event);
}

// 0x76BDB0
void MenuTaskClass::Handle_Input(KernelEventClass *event)
{
	if ((event->Flags & 0x1000) != 0) {
		return;
	}
	switch (event->Key) {
		case 0x43:
		{
			KernelClass *kernel = Kernel;
			Done = true;
			int mode = 1;
			if (Unknown_5FAE70() && !Unknown_5FAD50()) {
				mode = 9;
			}
			LoginTaskClass *task = new LoginTaskClass(mode);
			kernel->Add_Task(task);
			Unknown_Debug_Print("MenuTask: Starting Login Task.");
			Unknown_Debug_Begin_Step("starting login task");
			kernel->Set_Focus(task);
			kernel->Remove_Task(this);
			Unknown_Debug_End_Step("ok");
			break;
		}
		case 0x51:
			Unknown_Debug_Print("MenuTask: killing kernel.");
			Kernel->Quit = true;
			break;
	}
}

/*
**	Reconstruction notes (client.exe 0x76BC50-0x76BF2F; vtable 0xB04228, slots in tsk_menu.h).
**	Pre_Run builds a zeroed event with key 0x43 each cycle, so the first cycle with the focus
**	starts the login task: MenuTask's own key handling (0x51 stops the kernel) is reachable only
**	through that call. 0x76BF10 (a guarded static, atexit 0x42B320: every tasks object has it)
**	comes from a header the client's file includes and isn't rebuilt.
*/
