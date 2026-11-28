// Earth & Beyond client (reconstruction): app\main\unknown_4441e0.cpp (no __FILE__ or text of its own: this name
// is provisional, NAME 1; interim placement, D137). Notes at the end of the file.
#include "random.h"

RandomClass Unknown_BDBB84;				// 0xBDBB84 (initialiser 0x4441E0)  // NAME 1: neutral (D10)
Random2Class Unknown_BDADC8;			// 0xBDADC8 (initialiser 0x444200)  // NAME 1: neutral (D10)
Random3Class Unknown_BDB1B8;			// 0xBDB1B8 (initialiser 0x444220; nothing reads it)  // NAME 1: neutral (D10)
Random4Class Unknown_BDB1C0(0x1105);	// 0xBDB1C0 (initialiser 0x444240)  // NAME 1: neutral (D10)

/*
**	Notes
**
**	RECONSTRUCTED (IM8, run 2026-11-26): client.exe 0x4441E0-0x444260, an object of its own between kernel.cpp's
**	(its <string> guard 0x4441B0, the last of its initialisers) and main.cpp's: the CRT table lists the four
**	generators' initialisers (0x4441E0 RandomClass 0xBDBB84, 0x444200 Random2Class 0xBDADC8, 0x444220
**	Random3Class 0xBDB1B8, 0x444240 Random4Class(0x1105) 0xBDB1C0), then a zero slot, then main.cpp's
**	ios_base::Init 0x4442C0. The table's zero slots are object boundaries: 137 of the 138 between two app
**	initialisers whose objects our ledger knows separate two objects, and the 138th was this one while the
**	generators stood in main.cpp; an incremental link of ours puts its 169 app zeros after objects of two
**	or more entries, none inside one. No <string> guard: this object never saw <string>. Before: main.cpp's first
**	lines (XI, 2026-11-17).
*/
