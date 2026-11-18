// Earth & Beyond client (reconstruction): app\client\unknown_7196c0.cpp, PROVISIONAL file name (D137). Notes at the end of the file.
#include "unknown_7196c0.h"

PickInfoStruct Info[64];		// 0xBE71E0 (its initialiser 0x7196A0: Vector3's empty constructor, a bare ret)
int InfoCount;					// 0xBE79E0

// @0x7196C0: qsort's comparator of Info's indices by Dist2, the larger first
int __cdecl Unknown_7196C0(const void *a, const void *b)
{
	float diff = Info[*(const int *)b].Dist2 - Info[*(const int *)a].Dist2;	// NAME 1 [diff]: neutral
	if (diff == 0.0f) {
		return(0);
	}
	return((diff > 0.0f) ? 1 : -1);
}

// @0x719730: qsort's comparator of Info's indices by Cos, the larger first
int __cdecl Unknown_719730(const void *a, const void *b)
{
	float diff = Info[*(const int *)b].Cos - Info[*(const int *)a].Cos;	// NAME 1 [diff]: neutral
	if (diff == 0.0f) {
		return(0);
	}
	return((diff > 0.0f) ? 1 : -1);
}

/*
**	Reconstruction notes
**
**	client.exe 0x7196A0-0x719777: an object of its own between postate.cpp (its closing <string> initialiser
**	0x719640) and control_keys_manager.cpp (from its static string's initialiser 0x7197A0). Evidence: the
**	CRT initialiser table (0xB6CB84 postate's 0x719640, 0xB6CB88 0x719670, a zero, 0xB6CB90 0x7196A0, 0xB6CB94
**	control_keys_manager's 0x7197A0); 0x7196A0 is Info's initialiser (a bare ret: the array's Vector3 has an
**	empty constructor); and the .bss order: Info 0xBE71E0 and InfoCount 0xBE79E0 lie after postate.cpp's
**	0xBE71D4 and before control_keys_manager.cpp's 0xBE7B80, so they are this object's, not mainview.cpp's
**	(whose own statics lie at 0xBE59E4). MainViewClass::Unknown_60 0x512BB0 fills them and sorts with the two
**	comparators. The file and header names are PROVISIONAL (D137): no string names them. 0x719670 (two ids
**	numbered from 0xBE3B44) is a header's initialiser, not rebuilt (the zero entry before 0x7196A0 suggests it
**	is postate.cpp's).
*/
