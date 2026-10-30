// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\kernel\kernel.cpp (client 0x443020-0x444150)
**
**	The task kernel (docs/kernel-map.md 1-3). The asserts sit on the client's lines (74, 157, 196,
**	225, 474: D25).
*/



#define ENABLE_WWPROFILE
#include "kernel.h"
#include "..\shared\unknown_debug.h"
#include "wwprofile.h"
#include <windows.h>
#include <malloc.h>

unsigned int Unknown_Get_Ticks();		// the aux-data clock 0x4A3B40 (unknown_root.h)
int Unknown_9179D0();					// NAME 1 [Unknown_9179D0]: WW3D's device check (0 when DX8Wrapper's restore succeeds)
bool Unknown_42EA80(KernelEventClass *event);	// NAME 1 [Unknown_42EA80]: the input queue's next event (input.cpp)
void Unknown_42E8A0(KernelEventClass *event);	// NAME 1 [Unknown_42E8A0]: hands the event on (input.cpp)

KernelClass *KernelClass::Instance = NULL;

// ------------------------------------------------------------------------ TaskClass

// 0x443020
bool TaskClass::Is_Focused()
{
	if (Kernel != NULL) {
		return Kernel->Is_Focus(this);
	}
	return false;
}

// 0x443050
TaskClass::TaskClass() :
	Last_Run(0)
{
}

// 0x4430A0
TaskClass::~TaskClass()
{
}

// 0x4430C0
void TaskClass::Set_Kernel(KernelClass *kernel)
{
	Kernel = kernel;
}

// 0x4430E0
void TaskClass::Describe(void *)
{
}











// ------------------------------------------------------------------------ KernelClass

// 0x443100: the one instance.
KernelClass::KernelClass()
{
	G_ASSERT(Instance == NULL);
	Instance = this;
	Current_Task = NULL;
	Pending_Delete = NULL;
	Quit = false;
	Render_Enabled = false;
	Device_Ok = false;
}

// 0x4431C0
KernelClass::~KernelClass()
{
	Instance = NULL;
}

class UnknownBDADC0Class		// NAME 1 [UnknownBDADC0Class]: the object at *0xBDADC0 (its flag +0x4074)
{
public:
	char Unknown0000[0x4074];
	bool Unknown4074;
};
extern UnknownBDADC0Class *Unknown_BDADC0;	// NAME 1 [Unknown_BDADC0]: 0xBDADC0

class UnknownBFAEE0Class		// NAME 1 [UnknownBFAEE0Class]: the object at *0xBFAEE0 (Shutdown calls its slot +0x10)
{
public:
	virtual void Unknown_00();
	virtual void Unknown_04();
	virtual void Unknown_08();
	virtual void Unknown_0C();
	virtual void Unknown_10();
};
extern UnknownBFAEE0Class *Unknown_BFAEE0;	// NAME 1 [Unknown_BFAEE0]: 0xBFAEE0

// 0x443250
KernelClass *KernelClass::Get_Instance()
{
	return Instance;
}

// 0x443270
void KernelClass::Cycle_Focus()
{
	Next_Focus();
	Unknown_BDADC0->Unknown4074 = true;
}

// 0x4432A0: unless the focused task keeps the focus, every task shuts down (and is deleted when
// it asks to be); then a heap check.
bool KernelClass::Shutdown()
{
	if (Current_Task != NULL && !Current_Task->Yield_Focus()) {
		return false;
	}
	Current_Task = NULL;
	for (std::list<TaskClass *>::iterator it = Tasks.begin(); it != Tasks.end(); ++it) {
		TaskClass *task = *it;
		task->Shutdown();
		if (task->Delete_On_Remove()) {
			delete task;
		}
	}
	Tasks.clear();
	Unknown_BFAEE0->Unknown_10();
	if (_heapchk() != _HEAPOK) {
		DebugBreak();
	}
	return true;
}









// 0x443380: the task joins the list; it gets the focus if nothing has it and it accepts it.
bool KernelClass::Add_Task(TaskClass *task)
{
	for (std::list<TaskClass *>::iterator it = Tasks.begin(); it != Tasks.end(); ++it) {
		if (*it == task) {
			G_ASSERT(false);
			return false;
		}
	}
	Tasks.push_back(task);
	task->Kernel = this;
	if (Current_Task == NULL && task->Accept_Focus()) {
		Current_Task = task;
	}
	return true;
}

// 0x443450
bool KernelClass::Contains_Task(TaskClass *task)
{
	for (std::list<TaskClass *>::iterator it = Tasks.begin(); it != Tasks.end(); ++it) {
		if (*it == task) {
			return true;
		}
	}
	return false;
}












// 0x443490: unless it keeps the focus, the task leaves the list (its Shutdown and delete wait for
// the end of its Run when it is the one running).
bool KernelClass::Remove_Task(TaskClass *task)
{
	if (task == NULL || !Contains_Task(task)) {
		G_ASSERT(false);
		return true;
	}
	if (Current_Task == task) {
		if (!task->Yield_Focus()) {
			return false;
		}
		Current_Task = NULL;
	}
	if (task == *Iterator) {
		Iterator = Tasks.erase(Iterator);
	} else {
		Tasks.remove(task);	// NOT EXACT: the client calls the iterator's operator++(int) 0x444190 out of line
	}
	task->Kernel = NULL;
	if (task->Delete_On_Remove()) {
		if (task == Running_Task) {












			G_ASSERT(!Pending_Delete);
			Pending_Delete = task;
		} else {
			task->Shutdown();
			delete task;
		}
	}
	if (task == Running_Task) {
		Running_Task = NULL;
	}
	if (Current_Task == NULL) {
		Next_Focus();
	}
	return true;
}

// 0x443640
TaskClass *KernelClass::Get_Task(int index)
{
	int i = 0;
	for (std::list<TaskClass *>::iterator it = Tasks.begin(); it != Tasks.end(); ++it) {
		if (i == index) {
			return *it;
		}
		i++;
	}
	return NULL;
}

// 0x443680
void KernelClass::Remove_Running(TaskClass *)
{
	Iterator = Tasks.erase(Iterator);
}

// 0x4436C0: WinMain's loop.
int KernelClass::Run()
{
	while (Process_Messages() && Run_One_Cycle() && Is_Running()) {
	}
	return 0;
}

// 0x443710: the window messages (WM_QUIT sets Quit), then one input event: the kernel's keys
// first; the event goes on unless a task has the focus and Handle_Key didn't take it.
bool KernelClass::Process_Messages()
{
	Pump_Messages();
	Process_Input();
	return true;
}

// 0x443820: sleeps until the first task is due, then runs every task that is: the focused one's
// Pre_Run, Run and (when rendering and the device is fine) Render, the others' Run; a task removed
// while running is shut down and deleted after its Run.
bool KernelClass::Run_One_Cycle()
{
	WWPROFILE("KernelClass::Run_One_Cycle");
	WWProfileManager::Increment_Frame_Counter();
	if (Tasks.size() == 0) {
		return true;
	}
	Iterator = Tasks.begin();
	unsigned int now = Unknown_Get_Ticks();
	int sleep = 0x7FFFFFFF;
	for (; Iterator != Tasks.end(); ++Iterator) {
		Running_Task = *Iterator;
		unsigned int last = Running_Task->Last_Run;
		int wait = Running_Task->Get_Interval() + (int)(last - now);
		if (sleep >= wait) {
			sleep = wait;
		}
	}
	if (sleep > 0) {
		WWPROFILE("Kernel::Sleep");
		Sleep(sleep);
		now = Unknown_Get_Ticks();
	}
	for (Iterator = Tasks.begin(); Iterator != Tasks.end();) {
		Running_Task = *Iterator;
		unsigned int last = Running_Task->Last_Run;
		if ((int)(Running_Task->Get_Interval() + last) <= (int)now) {
			TaskClass *task = Running_Task;
			task->Last_Run = now;
			if (Running_Task == Current_Task) {
				Running_Task->Pre_Run();
				if (Running_Task != NULL) {
					{
						WWPROFILE("Current_Task->Run()");
						Running_Task->Run();
					}
					if (Running_Task != NULL && Render_Enabled) {
						Device_Ok = Unknown_9179D0() == 0;
						if (Device_Ok) {
							WWPROFILE("Current_Task->Render()");
							Running_Task->Render();
						}
					}
				}
			} else {
				WWPROFILE("Background_Task->Run()");
				Running_Task->Run();
			}
			if (Pending_Delete != NULL) {
				WWPROFILE("Pending_Delete->Shutdown()");
				Pending_Delete->Shutdown();
				delete Pending_Delete;
				Pending_Delete = NULL;
			}
			now = Unknown_Get_Ticks();
		}
		if (Running_Task != NULL) {
			Iterator++;
		}
	}
	Running_Task = NULL;
	return true;
}

// 0x443B90
bool KernelClass::Is_Running()
{
	if (Quit) {
		return false;
	}
	return Tasks.size() != 0;	// NOT EXACT: the client's has no xor before the setne
}

// 0x443BC0
int KernelClass::Is_Focus(TaskClass *task)
{
	return task == Current_Task;
}

// 0x443BF0
TaskClass *KernelClass::Get_Focus()
{
	return Current_Task;
}

// 0x443C10: unless the focused task keeps it, the focus goes to the next task after it that
// accepts it, round the list; none: no task has it.
void KernelClass::Next_Focus()
{
	if (Current_Task != NULL && !Current_Task->Yield_Focus()) {
		return;
	}
	std::list<TaskClass *>::iterator it = Tasks.begin();
	while (it != Tasks.end() && *it != Current_Task) {
		++it;
	}
	for (++it; it != Tasks.end(); ++it) {
		TaskClass *task = *it;
		if (task->Accept_Focus()) {
			Current_Task = task;
			return;
		}
	}
	for (it = Tasks.begin(); it != Tasks.end(); ++it) {
		TaskClass *task = *it;
		if (task->Accept_Focus()) {
			Current_Task = task;
			return;
		}
		if (task == Current_Task) {
			Current_Task = NULL;
			return;
		}
	}
}












































































// 0x443CD0
bool KernelClass::Set_Focus(TaskClass *task)
{
	G_ASSERT(Contains_Task(task));
	if (Current_Task != NULL && !Current_Task->Yield_Focus()) {
		return false;
	}
	Current_Task = NULL;
	if (task->Accept_Focus()) {
		Current_Task = task;
		return true;
	}
	Next_Focus();
	return Current_Task == task;
}

// 0x443D90
bool KernelClass::Process_Input()
{
	KernelEventClass event;
	if (Unknown_42EA80(&event) && (Handle_Key(&event) || Current_Task == NULL)) {
		Unknown_42E8A0(&event);
	}
	return true;
}

// 0x443E20: Esc with no task focused shuts the kernel down; 0x6A moves the focus on.
bool KernelClass::Handle_Key(KernelEventClass *event)
{
	if ((event->Flags & 0x1000) == 0) {
		switch (event->Key) {
			case 0x1B:
				if (Current_Task == NULL) {
					Shutdown();
					return true;
				}
				break;
			case 0x6A: {
				TaskClass *task = Current_Task;
				Next_Focus();
				return task != Current_Task;
			}
		}
	}
	return false;
}

// 0x443E90
void KernelClass::Pump_Messages()
{
	MSG msg;
	while (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE)) {
		if (GetMessage(&msg, NULL, 0, 0)) {
			TranslateMessage(&msg);
			if (msg.message == WM_QUIT) {
				Quit = true;
			}
			DispatchMessage(&msg);
		}
	}
}

// ------------------------------------------------------------------------ TaskClass's defaults

int TaskClass::Unknown_08() { return 0; }		// 0x443FA0
int TaskClass::Unknown_0C() { return 0; }		// 0x443FC0
int TaskClass::Unknown_10() { return 0; }		// 0x443FE0
int TaskClass::Unknown_14() { return 0; }		// 0x444000
int TaskClass::Unknown_18() { return 0; }		// 0x444020
int TaskClass::Unknown_1C() { return 0; }		// 0x444040
int TaskClass::Unknown_20() { return 0; }		// 0x444060
int TaskClass::Unknown_24() { return 0; }		// 0x444080
bool TaskClass::Accept_Focus() { return false; }	// 0x4440A0
bool TaskClass::Yield_Focus() { return true; }		// 0x4440C0
void TaskClass::Run() {}						// 0x4440E0
void TaskClass::Pre_Run() {}					// 0x444100
void TaskClass::Render() {}						// 0x444120
bool TaskClass::Shutdown() { return true; }		// 0x444140
