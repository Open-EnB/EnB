#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __MUSICTHEME_H  // NAME 2: musictheme.cpp is the client's; header name assumed
#define __MUSICTHEME_H

#include <vector>
#include "musiccomponent.h"

// One [MusicTheme-NNN] section of music.ini: its components and where it may play. 0x1C bytes (made at
// MusicManagerClass::Load_INI 0x43A1F0).
class MusicThemeClass  // NAME 3: class-from-file (D24) for audio\musictheme.cpp
{
public:
	MusicThemeClass();						// 0x43BFD0: Sector -1, LevelAvailable 0, Race 8
	~MusicThemeClass();						// 0x43C010 (G_ASSERT '*itor', l.70): the components deleted
	void Add_Component(const char *name, float fade_in, float fade_out);	// 0x43C100  // NAME 2: what it does
	int Get_Num_Components();				// 0x43C3A0 (the client's name: musiclist.cpp's assert l.98, musictheme.cpp's l.134)
	MusicComponentClass *Get_Component(int component);	// 0x43C3D0 (G_ASSERT 'component < Get_Num_Components()', l.134)  // NAME 2: what it returns

	std::vector<MusicComponentClass *> Components;	// +0x00  // NAME 2: Get_Num_Components counts it
	int Sector;				// +0x10: -1 any  // NAME 2: the ini's Sector
	int LevelAvailable;		// +0x14  // NAME 2: the ini's LevelAvailable
	int Race;				// +0x18: 8 until read  // NAME 2: the ini's Race (TERRAN, JENQUAI, PROGEN, OTHER)
};

#endif // __MUSICTHEME_H
