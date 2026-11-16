// Earth & Beyond client (reconstruction): app\client\unknown_8b5450.h
/*
**	PLACEMENT: provisional (D137): 0x8B5450-0x8B5500 (queue row 492; 0x8B5510 / 0x8B5540 are already
**	wlib/wdebug.cpp's DebugLibSemaphore init/dtor). Client evidence / EA matchbot wlib/threadfac.h
**	identifies this as Runnable (vtable 0xB0C4BC: slot 0 deleting dtor 0x8B5480, slot 1 purecall),
**	with ThreadCount_ and CritSec_ moved from static to per-instance members at +0x04 / +0x08.
**	ThreadFactory and the two launchers (0x8B52D0-0x8B5424, E&B's revision: _beginthread, void
**	launchers, the count per instance) are the object's first half: the whole object is wlib's
**	threadfac.cpp (0x8B52D0-0x8B5500, matchbot's function order). Kept separate from
**	libs/wwonline/wlib/threadfac.h (shared header; nothing includes it).
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __UNKNOWN_8B5450_H
#define __UNKNOWN_8B5450_H

#include "..\..\libs\wwonline\wlib\critsec.h"

class Runnable;

// Starts a thread in a function or in a Runnable (EA matchbot wlib/threadfac.h)
class ThreadFactory	// NAME 4: EA matchbot wlib/threadfac.h
{
public:
	static bit8 startThread(void (*start_func)(void *), void *data);	// 0x8B5360
	static bit8 startThread(Runnable &runable, void *data, bit8 destroy = FALSE);	// 0x8B52D0
};

class Runnable	// NAME 4: EA matchbot wlib/threadfac.h (E&B made ThreadCount_ and CritSec_ per-instance at +4/+8)
{
public:
	Runnable();
	virtual ~Runnable();

	virtual void run(void *data) = 0;

	bit8 isRunning(void);
	int getThreadCount(void);

private:
	friend class ThreadFactory;
	friend void __cdecl threadClassLauncher(void *temp);
	int ThreadCount_;	// NAME 4: EA matchbot wlib/threadfac.h (per-instance at +0x04)
	CritSec CritSec_;	// NAME 4: EA matchbot wlib/threadfac.h (per-instance at +0x08)
};

#endif //__UNKNOWN_8B5450_H
