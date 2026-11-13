// Earth & Beyond client (reconstruction): app\Audio\musiccomponent.cpp. Notes at the end of the file.
#include <stddef.h>
#include "musiccomponent.h"
#include "..\shared\g_assert.h"
#include "..\..\wwlib\nstrdup.h"

































// @0x4370C0
MusicComponentClass::MusicComponentClass(const char *name, float fade_in, float fade_out) :
	ThemeName(NULL),
	FadeIn(fade_in * 1000.0f),
	FadeOut(fade_out * 1000.0f)
{
	G_ASSERT(name);
	ThemeName = nstrdup(name);
}
















// @0x437140
MusicComponentClass::~MusicComponentClass()
{
	G_ASSERT(ThemeName);
	delete [] ThemeName;
	ThemeName = NULL;
}

/*
Reconstruction notes:
musiccomponent.cpp holds MusicComponentClass, one sound file of a music theme with its fade times. The object is
0x4370C0-0x437180 (after bridgesounds.cpp, before musiclist.cpp's 0x437180).
*/
