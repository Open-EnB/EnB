// Earth & Beyond client (reconstruction): app\shared\timer_manager.h, TimerManager: calls an object back after a
// delay through CallbackManager (timer_manager.cpp, client 0x4A0C40-0x4A0F9B). The header's name follows the .cpp's
// __FILE__ (no string names it; interim placement, D137). Nothing in the client calls TimerManager.
#ifndef TIMER_MANAGER_H	// NAME 1 [timer_manager.h]: the .cpp's name (D137)
#define TIMER_MANAGER_H

#include "callback_manager.h"

// What TimerManager calls back: its first virtual gets the time the call was due (the member pointer's vcall thunk
// 0x4A0F80 calls slot +0x00; no class in the client is seen passing one)
class TimerClientClass	// NAME 1: neutral (D10)
{
public:
	virtual bool Timer_Expired(unsigned int time) = 0;	// +0x00  // NAME 1: what it does
};

// A callback that calls a member function of an object with one argument (0x10 bytes; table 0xAEA9F8: scalar
// deleting destructor 0x4A0F30, destructor 0x4A0F60, Execute 0x4A0F00)
template<class T, class F, class A>
class MemberCallbackClass : public CallbackClass	// NAME 1: neutral (D10)
{
public:
	MemberCallbackClass(const T &object, const F &function, const A &argument) : Object(object), Function(function), Argument(argument) {}

	// No return when there is nothing to call (eax keeps this: the client's 0x4A0F00; VC6's C4715 warning)
	virtual bool Execute()
	{
		if (Object && Function) {
			return((Object->*Function)(Argument));
		}
	}

	T Object;		// +0x04  // NAME 1: what it holds
	F Function;		// +0x08  // NAME 1: what it holds
	A Argument;		// +0x0C  // NAME 1: what it holds
};

// Makes a MemberCallbackClass (0x4A0E60)
template<class T, class F, class A>
CallbackClass *New_Member_Callback(const T &object, const F &function, const A &argument)	// NAME 1: what it does
{
	return(new MemberCallbackClass<T, F, A>(object, function, argument));
}

class TimerManager	// NAME 2: the file's name (timer_manager.cpp)
{
public:
	TimerManager();							// 0x4A0C40
	~TimerManager();						// 0x4A0CE0

	static unsigned int Get_Time();					// 0x4A0D30: timeGetTime()  // NAME 1: what it does
	bool Add(unsigned int delay, TimerClientClass *t);		// 0x4A0D50  // NAME 1: what it does; [t]: the FAIL_IF's '!t'
	bool Remove(TimerClientClass *t);				// 0x4A0DE0  // NAME 1: what it does; [t]: the FAIL_IF's '!t'
	void Process();							// 0x4A0E20: runs every callback that is due  // NAME 1: what it does

protected:
	CallbackManager *Callbacks;	// +0x00  // NAME 5: the client's assert text 'Callbacks'
};

#endif
