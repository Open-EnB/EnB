// Earth & Beyond client (reconstruction): app\shared\timer_manager.cpp, TimerManager (client 0x4A0C40-0x4A0F9B). Notes at the end.
#include <windows.h>
#include <mmsystem.h>
#include "timer_manager.h"
#include "g_assert.h"
static unsigned int Unknown_BE39F4;	// 0xBE39F4 (.bss, just before uvcontrl.cpp's)  // NAME 1: neutral (D10)














// 0x4A0C40
TimerManager::TimerManager() : Callbacks(NULL)
{
	Callbacks = new CallbackManager;
	G_ASSERT(Callbacks);
}


// 0x4A0CE0
TimerManager::~TimerManager()
{
	G_ASSERT(Callbacks);
	delete Callbacks;
	Callbacks = NULL;
}

// 0x4A0D30
unsigned int TimerManager::Get_Time()
{
	return(timeGetTime());
}





// 0x4A0D50
bool TimerManager::Add(unsigned int delay, TimerClientClass *t)
{
	FAIL_IF(!t) {
		return(false);
	}
	unsigned int time = timeGetTime() + delay;
	CallbackClass *callback = New_Member_Callback(t, &TimerClientClass::Timer_Expired, time);
	Callbacks->Add(t, time, callback);
	return(true);
}




// 0x4A0DE0
bool TimerManager::Remove(TimerClientClass *t)
{
	FAIL_IF(!t) {
		return(false);
	}
	return(Callbacks->Remove_All(t));
}

// 0x4A0E20
void TimerManager::Process()
{
	while (Callbacks->Process(timeGetTime())) {
	}
}

// 0x4A0F90 (after the template and inline copies, before uvcontrl.cpp's object 0x4A0FB0)
unsigned int Unknown_4A0F90()	// NAME 1: neutral (D10)
{
	return(++Unknown_BE39F4);
}

/*
**	Reconstruction notes
**
**	client.exe 0x4A0C40-0x4A0F9B, after selector.cpp's object (its vector<unsigned long> copies end at 0x4A0C20) and
**	before uvcontrl.cpp's (0x4A0FB0): the file's functions in source order (0x4A0C40 to Process 0x4A0E20), then the
**	template and inline copies (New_Member_Callback 0x4A0E60, CallbackClass's Delete_When_Removed 0x4A0EB0 and
**	scalar deleting destructor 0x4A0ED0 with its table 0xAEAA08, MemberCallbackClass's Execute 0x4A0F00, scalar
**	deleting destructor 0x4A0F30, destructor 0x4A0F60 and table 0xAEA9F8, the vcall thunk 0x4A0F80 for
**	&TimerClientClass::Timer_Expired), then the counter 0x4A0F90 (its .bss 0xBE39F4 sits just before uvcontrl.cpp's
**	0xBE39F8). The asserts sit on the client's lines 25 and 32 ('Callbacks'), 50 and 65 ('!t'). Every function is
**	byte-exact (appfile.py match, 2026-11-13); Get_Time 0x4A0D30 (jmp [timeGetTime]), the counter and the vcall
**	thunk are too short to search for and match at their markers only. Nothing in the client calls any of them
**	(each has an incremental-link thunk, no thunk is referenced): TimerManager is linked but unused.
**
**	Names: Callbacks is the assert text, t the FAIL_IFs' '!t'; TimerManager follows the file's name; the rest are
**	neutral (NAME tags in the header).
**
**	The forms the bytes asked for: the constructor clears Callbacks before the new (the initializer list); Execute
**	returns the member function's bool and has no return on the other path (eax keeps this there: C4715, a warning,
**	where a void member function gives C4716, an error); the counter returns the new value (mov / inc / mov).
*/
