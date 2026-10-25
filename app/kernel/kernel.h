/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\kernel\kernel.h (name assumed from kernel.cpp)
**
**	The task kernel (docs/kernel-map.md 2, 3): KernelClass (one instance, 0xBDADBC) runs a list
**	of tasks; the focused one (Current_Task) gets Pre_Run, Run and Render, the others Run, each
**	every Get_Interval() ms. DECLARED ONLY: kernel.cpp (0x443020-0x444150) is not rebuilt yet, so
**	a program that links a task links its own kernel. The task base's members are in the client's
**	layout (+0x04 the kernel, +0x08 the last run), its virtuals in the client's slot order (vtable
**	0xAE7614, 18 slots).
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef KERNEL_H  // NAME 3 [kernel.h]: header name assumed from the client's kernel.cpp
#define KERNEL_H

class KernelClass;

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
	virtual int Get_Interval();							// +0x34 (0): ms between runs  // NAME 2: Run_One_Cycle's use
	virtual void Pre_Run();								// +0x38 0x444100: the focused task, before Run  // NAME 1: Run_One_Cycle's order
	virtual void Render();								// +0x3C 0x444120 ('Current_Task->Render()')
	virtual bool Shutdown();							// +0x40 0x444140 ('Pending_Delete->Shutdown()')
	virtual bool Delete_On_Remove();					// +0x44 (false): the kernel deletes the task  // NAME 2: Remove_Task's use

	KernelClass *Kernel;								// +0x04 NAME 2: Add_Task sets it
	unsigned int Last_Run;								// +0x08 NAME 2: Run_One_Cycle's ms clock at the last run
};

class KernelClass
{
public:
	static KernelClass *Get_Instance();					// 0x443250  // NAME 2: returns the instance (assert 'Instance == NULL')
	bool Add_Task(TaskClass *task);						// 0x443380  // NAME 2: what it does
	bool Remove_Task(TaskClass *task);					// 0x443490  // NAME 2: what it does
};

#endif
