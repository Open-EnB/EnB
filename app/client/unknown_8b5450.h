// Earth & Beyond client (reconstruction): app\client\unknown_8b5450.h
/*
**	PLACEMENT: provisional (D137): 0x8B5450-0x8B5500 (queue row 492; 0x8B5510 / 0x8B5540 are already
**	wlib/wdebug.cpp's DebugLibSemaphore init/dtor). Client evidence / EA matchbot wlib/threadfac.h
**	identifies this as Runnable (vtable 0xB0C4BC: slot 0 deleting dtor 0x8B5480, slot 1 purecall),
**	with ThreadCount_ and CritSec_ moved from static to per-instance members at +0x04 / +0x08.
**	ThreadFactory / launchers (0x8B52D0-0x8B5450) lie outside this claim; kept separate from
**	libs/wwonline/wlib/threadfac.h (shared header).
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __UNKNOWN_8B5450_H
#define __UNKNOWN_8B5450_H

#include "..\..\libs\wwonline\wlib\critsec.h"

class Runnable	// NAME 4: EA matchbot wlib/threadfac.h (E&B made ThreadCount_ and CritSec_ per-instance at +4/+8)
{
public:
	Runnable();
	virtual ~Runnable();

	virtual void run(void *data) = 0;

	bit8 isRunning(void);
	int getThreadCount(void);

private:
	int ThreadCount_;	// NAME 4: EA matchbot wlib/threadfac.h (per-instance at +0x04)
	CritSec CritSec_;	// NAME 4: EA matchbot wlib/threadfac.h (per-instance at +0x08)
};

#endif //__UNKNOWN_8B5450_H
