/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\kernel\kernel.h (name assumed from kernel.cpp)
**
**	The task kernel (docs/kernel-map.md 2, 3): KernelClass (one instance, 0xBDADBC) runs a list
**	of tasks; the focused one (Current_Task) gets Pre_Run, Run and Render, the others Run, each
**	every Get_Interval() ms (kernel.cpp). The members are in the client's layout, the virtuals in
**	its slot order (the task base's vtable 0xAE7614, 18 slots; the kernel's 0xAE766C, 6).
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef KERNEL_H  // NAME 3 [kernel.h]: header name assumed from the client's kernel.cpp
#define KERNEL_H

#include <list>

class KernelClass;

// An input event as the kernel sees it (16 bytes: the input queue 0x42EA80 fills it).
struct KernelEventClass		// NAME 1: describes what it holds; the input code's name not found
{
	KernelEventClass() : Key(0), Flags(0), Unknown04(0.0f), Unknown08(0.0f), Unknown0C(0) {}
	unsigned short Key;		// NAME 2: +0x00 Handle_Key: 0x1B Esc, 0x6A
	unsigned short Flags;	// NAME 1: +0x02 0x1000: Handle_Key ignores the event
	float Unknown04;		// +0x04 (floats: the constructor stores 0.0f as immediates)
	float Unknown08;		// +0x08
	int Unknown0C;			// +0x0C
};

// The task base class (constructor 0x443050, destructor 0x4430A0). No string names it; the tasks
// call themselves ClientTask, LoginTask, MenuTask and LoginTaskClass.
class TaskClass		// NAME 1: after the tasks' own names (LoginTaskClass); the client's not found
{
public:
	TaskClass();										// 0x443050
	virtual void Describe(void *table);					// +0x00 0x4430E0 (the message server writes 'TaskType')  // NAME 1: what the message server's does
	virtual ~TaskClass();								// +0x04 0x443070 / 0x4430A0
	virtual int Unknown_08();							// +0x08 .. +0x24: input handlers, all 0 (0x443FA0 ...)
	virtual int Unknown_0C();
	virtual int Unknown_10();
	virtual int Unknown_14();
	virtual int Unknown_18();
	virtual int Unknown_1C();
	virtual int Unknown_20();
	virtual int Unknown_24();
	virtual bool Accept_Focus();						// +0x28 0x4440A0  // NAME 2: 'LoginTask: accepting focus.'
	virtual bool Yield_Focus();							// +0x2C 0x4440C0  // NAME 2: 'LoginTask: yielding focus.'
	virtual void Run();									// +0x30 0x4440E0 ('Current_Task->Run()', 'ClientTask::Run')
	virtual int Get_Interval() = 0;						// +0x34: ms between runs  // NAME 2: Run_One_Cycle's use
	virtual void Pre_Run();								// +0x38 0x444100: the focused task, before Run  // NAME 1: Run_One_Cycle's order
	virtual void Render();								// +0x3C 0x444120 ('Current_Task->Render()')
	virtual bool Shutdown();							// +0x40 0x444140 ('Pending_Delete->Shutdown()')
	virtual bool Delete_On_Remove() = 0;				// +0x44: the kernel deletes the task  // NAME 2: Remove_Task's use

	bool Is_Focused();									// 0x443020  // NAME 2: what it does
	void Set_Kernel(KernelClass *kernel);				// 0x4430C0  // NAME 2: what it does

	KernelClass *Kernel;								// +0x04 NAME 2: Add_Task sets it
	unsigned int Last_Run;								// +0x08 NAME 2: Run_One_Cycle's ms clock at the last run
};

class KernelClass
{
public:
	KernelClass();										// 0x443100
	~KernelClass();										// 0x4431C0

	virtual void Cycle_Focus();							// +0x00 0x443270  // NAME 1: what it does
	virtual bool Shutdown();							// +0x04 0x4432A0  // NAME 2: the tasks' Shutdown, all
	virtual bool Process_Messages();					// +0x08 0x443710  // NAME 2: what it does
	virtual bool Run_One_Cycle();						// +0x0C 0x443820 ('KernelClass::Run_One_Cycle')
	virtual bool Is_Running();							// +0x10 0x443B90  // NAME 2: what it does
	virtual bool Handle_Key(KernelEventClass *event);	// +0x14 0x443E20  // NAME 2: what it does

	static KernelClass *Get_Instance();					// 0x443250  // NAME 2: returns the instance (assert 'Instance == NULL')
	bool Add_Task(TaskClass *task);						// 0x443380  // NAME 2: what it does
	bool Contains_Task(TaskClass *task);				// 0x443450 (the assert 'Contains_Task(task)')
	bool Remove_Task(TaskClass *task);					// 0x443490  // NAME 2: what it does
	TaskClass *Get_Task(int index);						// 0x443640  // NAME 2: what it does
	void Remove_Running(TaskClass *task);				// 0x443680: erases Iterator's task (task unused)  // NAME 1: what it does
	int Run();											// 0x4436C0: the loop  // NAME 2: what it does
	int Is_Focus(TaskClass *task);						// 0x443BC0 (an int: Is_Focused converts it)  // NAME 2: what it does
	TaskClass *Get_Focus();								// 0x443BF0  // NAME 2: what it does
	void Next_Focus();									// 0x443C10  // NAME 2: what it does
	bool Set_Focus(TaskClass *task);					// 0x443CD0  // NAME 2: what it does
	bool Process_Input();								// 0x443D90: Process_Messages' input step alone  // NAME 2: what it does
	void Pump_Messages();								// 0x443E90: Process_Messages' message loop alone  // NAME 2: what it does

	static KernelClass *Instance;						// 0xBDADBC (the assert 'Instance == NULL')

	TaskClass *Current_Task;							// +0x04 (the profile label 'Current_Task->Run()')
	TaskClass *Pending_Delete;							// +0x08 (the assert '!Pending_Delete')
	bool Quit;											// +0x0C NAME 2: WM_QUIT seen
	bool Render_Enabled;								// +0x0D NAME 2: Run_One_Cycle renders the focused task
	bool Device_Ok;										// +0x0E NAME 2: the last WW3D device check passed
	std::list<TaskClass *> Tasks;						// +0x10 NAME 2: (allocator +0x10, head +0x14, size +0x18)
	TaskClass *Running_Task;							// +0x1C NAME 2: the task Run_One_Cycle is at
	std::list<TaskClass *>::iterator Iterator;			// +0x20 NAME 2: Run_One_Cycle's place in Tasks
};

#endif
