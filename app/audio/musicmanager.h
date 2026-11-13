#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __MUSICMANAGER_H  // NAME 2: musicmanager.cpp is the client's; header name assumed
#define __MUSICMANAGER_H

#include <vector>
#include "musictheme.h"
#include "musiclist.h"

class INIClass;
class AudibleSoundClass;

// The game's music, from music.ini: the themes of each kind of music ([MusicTheme-NNN]: Type, Race, Sector,
// LevelAvailable, components), one play list per kind, and the kind playing now. 0x1FC bytes (SClient's
// +0x132C, made at Init).
class MusicManagerClass  // NAME 3: class-from-file (D24) for audio\musicmanager.cpp
{
public:
	// the kinds of music: music.ini's Type (the values 0x43B5A0 returns for the names)
	enum {
		MUSIC_THEME_NONE = -1,		// NAME 2: CurrentMusicType's -1
		MUSIC_THEME_WANDER,			// NAME 2: the ini's WANDER
		MUSIC_THEME_COMBAT,			// NAME 2: the ini's COMBAT
		MUSIC_THEME_PLANET,			// NAME 2: the ini's PLANET
		MUSIC_THEME_STARBASE,		// NAME 2: the ini's STARBASE (3)
		MUSIC_THEME_STARGATE,		// NAME 2: the ini's STARGATE (4)
		NUM_MUSIC_THEME_TYPES		// the client's (FAIL_IF '(CurrentMusicType >= NUM_MUSIC_THEME_TYPES) || (CurrentMusicType < 0)', l.572)
	};

	MusicManagerClass();					// 0x439E40 (G_ASSERT 'MusicINI', l.83; 'music.ini' loaded)
	~MusicManagerClass();					// 0x43A040 (l.114)
	void Load_INI(int race);				// 0x43A1F0 (l.141-205): [Global]'s fades and silences, then the themes of race (and of race 8)  // NAME 2: what it does; NAME 1 [race]: what the themes' Race is compared with
	void Set_Music_Type(int type);			// 0x43ACE0: the old kind's list faded out, the new one's faded in  // NAME 2: what it does
	void Stop();							// 0x43AD70 (SClient's Shutdown, before the delete): every list reset, the sector music stopped  // NAME 2: what it does
	void Fade_Out();						// 0x43ADF0 (CAMERA_CONTROL 146's case 5, after the 'Docking_Init' sound): the current list faded out over 2000 ms  // NAME 2: what it does
	void Update(int type, int level, int race, int theme_race);	// 0x43AE30 ('-NOMUSIC'; 0x744690 passes 0, 0, 0, 8 while loading, 0x74BFB0 every frame)  // NAME 2: what it does; NAME 1 [level, race, theme_race]: what LevelAvailable, Load_INI and 0x43B1C0 get
	const char *Get_Current_Name();			// 0x43B190: the current list's playing component (MusicListClass::Get_Current_Name inline)  // NAME 2: what it returns
	void Queue_Themes(int type, int race);	// 0x43B1C0: the themes of type allowed at Level (no Sector; for STARBASE, of race), but for the last one played, added to its list in random order  // NAME 2: what it does
	int Race_From_Name(const char *name);	// 0x43B430 (TERRAN 0, JENQUAI 1, PROGEN 2, OTHER and anything else 8; ret 4: not static)  // NAME 2: what it does
	int Type_From_Name(const char *name);	// 0x43B5A0 (WANDER, COMBAT, PLANET, STARGATE, STARBASE; anything else WANDER; ret 4: not static)  // NAME 2: what it does
	void Set_Sector(unsigned sector);		// 0x43B760 (0x750A70: SERVER_PARAMETERS 66's SectorNum): Sector set for the next Update, the sector music stopped  // NAME 2: what it does
	void Queue_Sector_Themes();				// 0x43B7D0: Queue_Sector_Theme(Sector) for every type  // NAME 2: what it does
	bool Queue_Sector_Theme(int sector);	// 0x43B860 (l.572, G_ASSERT 'theme', l.586): the current type's theme of that sector put alone in its list  // NAME 2: what it does
	void Play_Music(int music_id);			// 0x43B970 (0x74BFB0: the 'MusicID' aux value): [MusicTheme-NNN]'s Component-00 played LoopCount times as SectorMusic  // NAME 2: what it does
	void Stop_Sector_Music();				// 0x43BBA0 (no caller: the out-of-line copy of what the destructor, Stop, Set_Sector and Play_Music do inline): SectorMusic stopped and released  // NAME 2: what it does

	std::vector<MusicThemeClass *> Themes[NUM_MUSIC_THEME_TYPES];	// +0x000: by Type (G_ASSERT 'theme')  // NAME 2: what it holds
	MusicListClass Lists[NUM_MUSIC_THEME_TYPES];	// +0x050  // NAME 2: what it holds
	int CurrentMusicType;				// +0x1A4: 0 (the constructor); -1 none
	int PreviousMusicType;				// +0x1A8: -1  // NAME 2: Set_Music_Type keeps the old type here
	int Level;							// +0x1AC: Update's; themes with a higher LevelAvailable are skipped  // NAME 2: what it is compared with
	int SectorThemeSector;				// +0x1B0: the sector whose own themes are queued, -1 none  // NAME 2: what it holds
	int FadeIn[NUM_MUSIC_THEME_TYPES];	// +0x1B4: milliseconds  // NAME 2: the ini's WanderFadeIn, CombatFadeIn, PlanetFadeIn
	int FadeOut[NUM_MUSIC_THEME_TYPES];	// +0x1C8  // NAME 2: the ini's WanderFadeOut, CombatFadeOut, PlanetFadeOut
	int SilenceMin;						// +0x1DC: 10000  // NAME 2: the ini's SilenceMin
	int SilenceMax;						// +0x1E0: 30000  // NAME 2: the ini's SilenceMax
	unsigned SilenceEnd;				// +0x1E4: timeGetTime() value until which nothing plays, 0 none  // NAME 2: what it holds
	bool IsLoaded;						// +0x1E8: Load_INI ran  // NAME 2: what it does
	int Sector;							// +0x1EC: Set_Sector's, -1 once Update has used it  // NAME 2: what it holds
	AudibleSoundClass *SectorMusic;		// +0x1F0: Play_Music's sound, a reference held  // NAME 2: what it holds
	INIClass *MusicINI;					// +0x1F4
	bool Enabled;						// +0x1F8: true (the constructor, every type change); Update does nothing when false  // NAME 2: what it does
};

#endif // __MUSICMANAGER_H
