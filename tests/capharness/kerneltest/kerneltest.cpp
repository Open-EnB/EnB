// The task kernel's test (docs/kernel-map.md 6, step 1): app\kernel\kernel.cpp with scripted tasks
// on a clock this program sets. Prints one line per event; tools/harness/kerneltest.py checks them.
//
//     kerneltest OUT.txt
//
// Cases: intervals and the sleep (the earliest due task), focus (Add_Task, Set_Focus, a task that
// refuses to yield, Next_Focus round the list), Pre_Run / Run / Render for the focused task only,
// Remove_Task of the running task (Pending_Delete: Shutdown and delete after its Run), Remove_Task
// of another, Handle_Key (Esc, 0x6A), Shutdown, the asserts (a second kernel, adding twice).
#include <stdio.h>
#include <string.h>
#include "kernel.h"

static FILE *Out;
unsigned int Clock = 1000;
static int Slept = -1;
static int Asserts = 0;

// ------------------------------------------------------------------------- what kernel.cpp calls

unsigned int Unknown_Get_Ticks() { return Clock; }
int Unknown_9179D0() { return 0; }
KernelEventClass Pending;
bool Has_Pending = false;
bool Unknown_42EA80(KernelEventClass *event)
{
	if (!Has_Pending) {
		return false;
	}
	*event = Pending;
	return true;
}
void Unknown_42E8A0(KernelEventClass *)
{
	Has_Pending = false;
	fprintf(Out, "event passed on\n");
}
class UnknownBDADC0Class
{
public:
	char Unknown0000[0x4074];
	bool Unknown4074;
};
static UnknownBDADC0Class BDADC0;
UnknownBDADC0Class *Unknown_BDADC0 = &BDADC0;
class UnknownBFAEE0Class
{
public:
	virtual void Unknown_00() {}
	virtual void Unknown_04() {}
	virtual void Unknown_08() {}
	virtual void Unknown_0C() {}
	virtual void Unknown_10() { fprintf(Out, "BFAEE0 slot 0x10\n"); }
};
static UnknownBFAEE0Class BFAEE0;
UnknownBFAEE0Class *Unknown_BFAEE0 = &BFAEE0;
void Unknown_Assert_Failed(const char *expr, const char *, int line)
{
	Asserts++;
	fprintf(Out, "assert %s (line %d)\n", expr, line);
}

// Sleep is the kernel's (Windows'); the test sees the time it asks for through the clock instead.

// ------------------------------------------------------------------------- the test tasks

class TestTask : public TaskClass
{
public:
	TestTask(const char *name, int interval, bool focus, bool yield, bool owned) :
		Name(name), Interval(interval), Focus(focus), Yield(yield), Owned(owned), Runs(0), Remove_At(-1), Remove_Other(0)
	{
	}
	virtual ~TestTask() { fprintf(Out, "%s deleted\n", Name); }
	virtual bool Accept_Focus() { return Focus; }
	virtual bool Yield_Focus() { fprintf(Out, "%s yield? %s\n", Name, Yield ? "yes" : "no"); return Yield; }
	virtual void Run()
	{
		Runs++;
		fprintf(Out, "%s run %d at %u%s\n", Name, Runs, Clock, Is_Focused() ? " (focused)" : "");
		if (Runs == Remove_At) {
			Kernel->Remove_Task(this);
		}
		if (Remove_Other != 0 && Runs == 1) {
			Kernel->Remove_Task(Remove_Other);
		}
	}
	virtual int Get_Interval() { return Interval; }
	virtual void Pre_Run() { fprintf(Out, "%s pre-run\n", Name); }
	virtual void Render() { fprintf(Out, "%s render\n", Name); }
	virtual bool Shutdown() { fprintf(Out, "%s shutdown\n", Name); return true; }
	virtual bool Delete_On_Remove() { return Owned; }

	const char *Name;
	int Interval;
	bool Focus;
	bool Yield;
	bool Owned;
	int Runs;
	int Remove_At;
	TaskClass *Remove_Other;
};

int Tasks_Test(FILE *out, const char *bik);	// taskstest.cpp: the start-up tasks (kernel-map.md 6, step 7)

static void Cycle(KernelClass &k, int advance)
{
	Clock += advance;
	k.Run_One_Cycle();
}

int main(int argc, char **argv)
{
	Out = fopen(argc > 1 ? argv[1] : "kerneltest.txt", "w");
	if (!Out) {
		return 2;
	}
	KernelClass *kernel = new KernelClass;
	fprintf(Out, "instance %s\n", KernelClass::Get_Instance() == kernel ? "ok" : "wrong");
	kernel->Render_Enabled = true;

	TestTask *a = new TestTask("A", 100, true, true, true);		// focusable, every 100 ms
	TestTask *b = new TestTask("B", 30, false, true, true);		// background, every 30 ms
	TestTask *c = new TestTask("C", 0, true, false, true);			// focusable, keeps the focus, every cycle
	kernel->Add_Task(a);
	kernel->Add_Task(b);
	fprintf(Out, "focus %s\n", kernel->Get_Focus() == a ? "A" : "other");
	fprintf(Out, "add twice: %d\n", (int)kernel->Add_Task(a));
	a->Last_Run = Clock;
	b->Last_Run = Clock;
	Cycle(*kernel, 0);		// nothing due: the kernel sleeps 30 ms (B), then runs nothing at the old time
	Cycle(*kernel, 30);		// B
	Cycle(*kernel, 70);		// A (focused: pre-run, run, render) and B
	kernel->Add_Task(c);
	fprintf(Out, "set focus C: %d\n", (int)kernel->Set_Focus(c));
	fprintf(Out, "focus %s\n", kernel->Get_Focus() == c ? "C" : "other");
	fprintf(Out, "set focus A (C keeps it): %d\n", (int)kernel->Set_Focus(a));
	c->Yield = true;
	kernel->Next_Focus();
	fprintf(Out, "next focus: %s\n", kernel->Get_Focus() == a ? "A" : kernel->Get_Focus() == c ? "C" : "none");
	b->Remove_At = b->Runs + 1;	// B removes itself during its next run: Pending_Delete
	Cycle(*kernel, 30);
	fprintf(Out, "tasks left B? %d\n", (int)kernel->Contains_Task(b));
	c->Remove_Other = a;			// C's next first run... (C ran already?) remove A from inside C's run
	c->Runs = 0;
	Cycle(*kernel, 100);
	fprintf(Out, "A left? %d, focus %s\n", (int)kernel->Contains_Task(a), kernel->Get_Focus() == c ? "C" : kernel->Get_Focus() ? "other" : "none");
	// keys: 0x6A cycles the focus (one focusable task: stays), Esc with a focused task does nothing
	Pending.Key = 0x6A;
	Has_Pending = true;
	kernel->Process_Input();
	Pending.Key = 0x1B;
	Has_Pending = true;
	kernel->Process_Input();
	fprintf(Out, "running %d\n", (int)kernel->Is_Running());
	fprintf(Out, "shutdown %d\n", (int)kernel->Shutdown());
	fprintf(Out, "running %d\n", (int)kernel->Is_Running());
	KernelClass *second = new KernelClass;	// the assert 'Instance == NULL'
	delete second;
	delete kernel;
	fprintf(Out, "asserts %d\n", Asserts);
	Tasks_Test(Out, argc > 2 ? argv[2] : NULL);
	fprintf(Out, "asserts %d\n", Asserts);
	fclose(Out);
	return 0;
}
