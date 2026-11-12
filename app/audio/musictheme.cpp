// Earth & Beyond client (reconstruction): app\Audio\musictheme.cpp. Notes at the end of the file.
#include "musictheme.h"
#include "..\shared\g_assert.h"

// @0x43BFD0
MusicThemeClass::MusicThemeClass() :
	Sector(-1),
	LevelAvailable(0),
	Race(8)
{
	Components.clear();
}





















































// @0x43C010
MusicThemeClass::~MusicThemeClass()
{
	for (std::vector<MusicComponentClass *>::iterator itor = Components.begin(); itor != Components.end(); itor++) {
		G_ASSERT(*itor);
		delete *itor;
	}
	Components.clear();
}

// @0x43C100
void MusicThemeClass::Add_Component(const char *name, float fade_in, float fade_out)
{
	MusicComponentClass *component = new MusicComponentClass(name, fade_in, fade_out);
	Components.push_back(component);
}

// @0x43C3A0
int MusicThemeClass::Get_Num_Components()
{
	return(Components.size());
}











































// @0x43C3D0
MusicComponentClass *MusicThemeClass::Get_Component(int component)
{
	G_ASSERT(component < Get_Num_Components());
	G_ASSERT(component >= 0);
	return(Components[component]);
}

/*
Reconstruction notes:
musictheme.cpp holds MusicThemeClass, one [MusicTheme-NNN] section of music.ini: its components, sector, level
and race. The object is 0x43BFD0-0x43C4A0 (after musicmanager.cpp's instances, before sectorsound.cpp's
0x43C4A0); the vector's _Ufill 0x43C460 follows its functions.
*/
