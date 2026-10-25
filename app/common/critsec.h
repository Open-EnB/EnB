/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	The lock udpconn.cpp takes around every CordMgr / CordCon call: Westwood's wlib CritSec
**	(libs\wwonline\wlib, client 0x8B42F0-0x8B4350: constructor, destructor, lock, unlock over a
**	Win32 CRITICAL_SECTION). Its interface is EA's (Generals Tools\matchbot\wlib\critsec.h), but
**	E&B's lock takes no argument (the callers push none; `ret`, not `ret 4`).
**
**	This header declares only what the app code calls (out of line, as the client calls it), so
**	that a program links either wlib's critsec.cpp (not imported yet: WWOnline is an open decision)
**	or a stand-in (tests/capharness/udpreplay/conn_replay.cpp).
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef CRITSEC_H  // NAME 4 [critsec.h]: wlib's critsec.h (EA's Generals Tools matchbot wlib); its place here assumed
#define CRITSEC_H

#include <windows.h>

class CritSec  // NAME 4: wlib's name (EA's Generals Tools matchbot wlib)
{
public:
	CritSec();				// 0x8B42F0
	~CritSec();				// 0x8B4310
	int lock() const;		// 0x8B4330 (returns 0)
	int unlock() const;		// 0x8B4350 (returns 0)

protected:
	mutable CRITICAL_SECTION CritSec_;	// NAME 4: wlib's name
};

#endif
