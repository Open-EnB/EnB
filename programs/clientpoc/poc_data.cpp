// Earth & Beyond client (reconstruction): the proof-of-concept client's data globals (D179). NOT THE CLIENT'S CODE.
/*
**	The globals app files declare and no tree file defines yet that gen_stubs.py --link cannot write
**	(objects, not pointers or scalars), each constructed as its type's default. Delete one here when a tree
**	file defines it. (E&B's BINKMovie::Play, once here, is binkmovie's own since run 2026-11-11b.)
*/



#include <string>
#include <vector>
#include "..\..\wwlib\random.h"
#include "..\..\wwmath\rect.h"

RectClass Unknown_C07550;						// 0xC07550 (sclient.cpp's, scriptedmovie.cpp's)


// textgadg.h's flag constants (.rdata 0xB08C90-0xB08CC4, the client's values; the file that defines them is not known)
extern const int Unknown_B08C90 = 0x1;
extern const int Unknown_B08C94 = 0x2;
extern const int Unknown_B08C98 = 0x4;
extern const int Unknown_B08CA8 = 0x40;	// 0xB08CA8 (ui_radio_join.cpp's, ui_log.cpp's)
extern const int Unknown_B08CA0 = 0x10;
extern const int Unknown_B08CB4 = 0x200;
extern const int Unknown_B08C9C = 0x8;
extern const int Unknown_B08CAC = 0x80;
extern const int Unknown_B08CB0 = 0x100;
extern const int Unknown_B08CB8 = 0x400;
extern const int Unknown_B08CBC = 0x800;
extern const int Unknown_B08CC0 = 0x1000;
extern const int Unknown_B08CC4 = 0x2C0;
