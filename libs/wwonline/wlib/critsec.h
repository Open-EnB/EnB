/*
**	Command & Conquer Generals Zero Hour(tm)
**	Copyright 2025 Electronic Arts Inc.
**	Modified for the Earth & Beyond client reconstruction: each change is recorded at the end of this file.
**	This program is free software: you can redistribute it and/or modify
**	it under the terms of the GNU General Public License as published by
**	the Free Software Foundation, either version 3 of the License, or
**	(at your option) any later version.
**
**	This program is distributed in the hope that it will be useful,
**	but WITHOUT ANY WARRANTY; without even the implied warranty of
**	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**	GNU General Public License for more details.
**
**	You should have received a copy of the GNU General Public License
**	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef CRITSEC_HEADER
#define CRITSEC_HEADER

#include "wstypes.h"
#ifdef _WIN32
 #include <windows.h>
 #include <winbase.h>
#elif defined(_UNIX)
  #include <pthread.h>
  #include <errno.h>
#endif

// Windows headers have a tendency to redefine IN
#ifdef IN
#undef IN
#endif
#define IN const

//
// Critical Section built either on a POSIX Mutex, or a Win32 Critical Section
//
// POSIX version is done by keeping a thread_id and a reference count.  Win32 version
//  just calls the built in functions.
//
class CritSec
{
 public:
                CritSec();
               ~CritSec();
    
  sint32		lock(void) RO;					// E&B: no argument (0x8B4330 `ret`; matchbot's lock(int *refcount=NULL))
  sint32		unlock(void) RO;

 protected:
  #ifdef _WIN32
    mutable CRITICAL_SECTION    CritSec_;
  #else
    mutable pthread_mutex_t	Mutex_;         // Mutex lock
    mutable pthread_t		ThreadId_;	// Owner of mutex
    mutable int                 RefCount_;      // Reference count
  #endif
};

#endif

// Earth & Beyond client (reconstruction): changes to Zero Hour's critsec.h (frozen 2026-11-07 from importer.py)
// EnB: class CritSec (line 49 at the freeze): E&B's CritSec::lock 0x8B4330 takes no argument: `push ecx; call
//       [EnterCriticalSection]; xor eax, eax; ret` (matchbot's lock(int *) is `ret 4`); its callers (udpconn.cpp
//       0x82D420...) push none
