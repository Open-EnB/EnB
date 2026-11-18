// Earth & Beyond client (reconstruction): app\kernel\unknown_4441e0.cpp. Notes at the end of the file.
#include "random.h"

RandomClass Unknown_BDBB84;				// 0xBDBB84 (initialiser 0x4441E0)  // NAME 1: neutral (D10)
Random2Class Unknown_BDADC8;			// 0xBDADC8 (initialiser 0x444200)  // NAME 1: neutral (D10)
Random3Class Unknown_BDB1B8;			// 0xBDB1B8 (initialiser 0x444220; nothing reads it)  // NAME 1: neutral (D10)
Random4Class Unknown_BDB1C0(0x1105);	// 0xBDB1C0 (initialiser 0x444240)  // NAME 1: neutral (D10)

#include <iostream>

/*
**	Notes
**
**	RECONSTRUCTED (XI, 2026-11-17): client.exe 0x4441E0-0x445560, an object of its own after kernel.cpp: the CRT
**	initialiser table lists, in this order, the four generators' initialisers (entries 17-20: 0x4441E0 RandomClass
**	0xBDBB84, 0x444200 Random2Class 0xBDADC8, 0x444220 Random3Class(0, 0) 0xBDB1B8, 0x444240 Random4Class(0x1105)
**	0xBDB1C0), then <iostream>'s ios_base::Init (0x4442C0, atexit 0x444380) and _Winit (0x445260, atexit 0x445300) and
**	<string>'s guard 0x445540 last; so the generators are defined before <iostream> is included. Its code is
**	library instances only (0x444290, 0x444F60, 0x4450B0); 0x444390-0x444F60 is int3 fill. No string names the file:
**	the name and directory are PROVISIONAL (D137).
*/
