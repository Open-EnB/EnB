// Earth & Beyond client (reconstruction): app\Audio\musicmanager.cpp. Notes at the end of the file.
#include "musicmanager.h"
#include "..\shared\g_assert.h"
#include "..\..\wwlib\ini.h"
#include "..\..\wwaudio\AudibleSound.h"
#include "..\..\wwaudio\WWAudio.h"
#include "..\..\wwlib\random.h"
#include <stdio.h>
#include <string.h>

extern RandomClass Unknown_BDBB84;	// NAME 1 [Unknown_BDBB84]: neutral (D10), as avatar.cpp's


















































// @0x439E40
MusicManagerClass::MusicManagerClass()
{
	PreviousMusicType = MUSIC_THEME_NONE;
	SectorThemeSector = -1;
	Sector = -1;
	CurrentMusicType = MUSIC_THEME_WANDER;
	Level = 0;
	SilenceMin = 10000;
	SilenceMax = 30000;
	SilenceEnd = 0;
	IsLoaded = false;
	SectorMusic = NULL;
	MusicINI = NULL;
	Enabled = true;
	for (int type = 0; type < NUM_MUSIC_THEME_TYPES; type++) {
		Themes[type].clear();
		FadeIn[type] = 0;
		FadeOut[type] = 0;
	}
	MusicINI = new INIClass;
	G_ASSERT(MusicINI);
	MusicINI->Load("music.ini");
}













// @0x43A040
MusicManagerClass::~MusicManagerClass()
{
	for (int type = 0; type < NUM_MUSIC_THEME_TYPES; type++) {
		for (std::vector<MusicThemeClass *>::iterator itor = Themes[type].begin(); itor != Themes[type].end(); itor++) {
			delete *itor;
		}
		Themes[type].clear();
		Lists[type].Reset();
	}
	if (SectorMusic != NULL) {
		SectorMusic->Stop(true);
		SectorMusic->Release_Ref();
		SectorMusic = NULL;
	}
	G_ASSERT(MusicINI);
	delete MusicINI;
	MusicINI = NULL;
}

// @0x43ACE0
void MusicManagerClass::Set_Music_Type(int type)
{
	if (type != CurrentMusicType) {
		Enabled = true;
		PreviousMusicType = CurrentMusicType;
		CurrentMusicType = type;
		if (PreviousMusicType != MUSIC_THEME_NONE) {
			Lists[PreviousMusicType].Fade_Out(FadeOut[PreviousMusicType]);
		}
		if (CurrentMusicType != MUSIC_THEME_NONE) {
			Lists[CurrentMusicType].Fade_In(FadeIn[CurrentMusicType]);
		}
	}
}

// @0x43AD70
void MusicManagerClass::Stop()
{
	for (int type = 0; type < NUM_MUSIC_THEME_TYPES; type++) {
		Lists[type].Reset();
	}
	if (SectorMusic != NULL) {
		SectorMusic->Stop(true);
		SectorMusic->Release_Ref();
		SectorMusic = NULL;
	}
}

// @0x43ADF0
void MusicManagerClass::Fade_Out()
{
	if (CurrentMusicType != MUSIC_THEME_NONE) {
		Lists[CurrentMusicType].Fade_Out(2000);
	}
}

// @0x43B1C0
// NOT EXACT: register allocation and inline budget (explain.py: 488 bytes vs 494, 0.46): the client keeps type in ebx and calls the vector's _Destroy 0x43BC60 after the first erase, here type is in ebp and both erases are inline
void MusicManagerClass::Queue_Themes(int type, int race)
{
	int count = Themes[type].size();
	if (count != 0) {
		std::vector<MusicThemeClass *> themes;
		MusicThemeClass *theme = NULL;
		for (int index = 0; index < count; index++) {
			theme = Themes[type][index];
			if (theme->LevelAvailable <= Level && theme->Sector == -1 && (type != MUSIC_THEME_STARBASE || theme->Race == race)) {
				themes.push_back(theme);
			}
		}
		if (themes.size() > 1) {
			for (std::vector<MusicThemeClass *>::iterator itor = themes.begin(); itor != themes.end(); itor++) {
				if (Lists[type].LastTheme == *itor) {
					themes.erase(itor);
					break;
				}
			}
		}
		while (themes.size() != 0) {
			int pick = Unknown_BDBB84(0, themes.size() - 1);
			theme = themes[pick];
			Lists[type].Add_Theme(theme);
			themes.erase(&themes[pick]);
		}
		Lists[type].LastTheme = theme;
		if (type == MUSIC_THEME_COMBAT) {
			Lists[type].Looping = true;
		}
	}
}

// @0x43B430
int MusicManagerClass::Race_From_Name(const char *name)
{
	if (strcmp(name, "TERRAN") == 0) {
		return(0);
	}
	if (strcmp(name, "JENQUAI") == 0) {
		return(1);
	}
	if (strcmp(name, "OTHER") == 0) {
		return(8);
	}
	return((strcmp(name, "PROGEN") == 0) ? 2 : 8);
}

// @0x43B5A0
int MusicManagerClass::Type_From_Name(const char *name)
{
	if (strcmp(name, "WANDER") == 0) {
		return(MUSIC_THEME_WANDER);
	}
	if (strcmp(name, "COMBAT") == 0) {
		return(MUSIC_THEME_COMBAT);
	}
	if (strcmp(name, "PLANET") == 0) {
		return(MUSIC_THEME_PLANET);
	}
	if (strcmp(name, "STARGATE") == 0) {
		return(MUSIC_THEME_STARGATE);
	}
	return((strcmp(name, "STARBASE") == 0) ? MUSIC_THEME_STARBASE : MUSIC_THEME_WANDER);
}

// @0x43B760
void MusicManagerClass::Set_Sector(unsigned sector)
{
	Sector = sector;
	if (SectorMusic != NULL) {
		SectorMusic->Stop(true);
		SectorMusic->Release_Ref();
		SectorMusic = NULL;
	}
}

// @0x43B7D0
// NOT EXACT: order-only (explain.py: same length, 0.84): the client stores Sector = -1 before it saves CurrentMusicType to the stack, here after
void MusicManagerClass::Queue_Sector_Themes()
{
	bool found = false;
	int type = CurrentMusicType;
	int sector = Sector;
	Sector = -1;
	for (int index = 0; index < NUM_MUSIC_THEME_TYPES; index++) {
		CurrentMusicType = index;
		if (Queue_Sector_Theme(sector)) {
			SectorThemeSector = sector;
			found = true;
		}
	}
	CurrentMusicType = type;
	if (!found) {
		SectorThemeSector = -1;
	}
}


























































































































































































































































































































// @0x43B860
bool MusicManagerClass::Queue_Sector_Theme(int sector)
{
	FAIL_IF((CurrentMusicType >= NUM_MUSIC_THEME_TYPES) || (CurrentMusicType < 0)) {
		return(false);
	}
	int count = Themes[CurrentMusicType].size();
	if (count == 0) {
		return(false);
	}
	for (int index = 0; index < count; index++) {
		MusicThemeClass *theme = Themes[CurrentMusicType][index];





		G_ASSERT(theme);
		if (theme->Sector == sector) {
			Lists[CurrentMusicType].Reset();
			Lists[CurrentMusicType].Add_Theme(theme);
			return(true);
		}
	}
	return(false);
}

// @0x43B970
void MusicManagerClass::Play_Music(int music_id)
{
	if (SectorMusic != NULL) {
		SectorMusic->Stop(true);
		SectorMusic->Release_Ref();
		SectorMusic = NULL;
	}
	if (music_id >= 0 && MusicINI != NULL && MusicINI->Is_Loaded()) {
		char section[32];
		char name[32];
		_snprintf(section, sizeof(section), "MusicTheme-%03d", music_id);
		if (MusicINI->Section_Present(section)) {
			MusicINI->Get_String(section, "Component-00", "None", name, sizeof(name));
			if (strcmp("None", name) != 0) {
				int loop_count = MusicINI->Get_Int(section, "LoopCount", 0);
				SectorMusic = WWAudioClass::Get_Instance()->Create_Sound_Effect(name);
				if (SectorMusic != NULL) {
					SectorMusic->Set_Type(AudibleSoundClass::TYPE_MUSIC);
					SectorMusic->Set_Loop_Count(loop_count);
					SectorMusic->Play(true);
					Set_Music_Type(MUSIC_THEME_NONE);
				}
			}
		}
	}
}

/*
Reconstruction notes:
musicmanager.cpp holds MusicManagerClass: the themes of music.ini by kind of music, a MusicListClass per kind and
the kind playing now. The object is 0x439E40-0x43BFD0 (after musiclist.cpp's deque instances, before
musictheme.cpp's 0x43C010). Not rebuilt yet: Load_INI 0x43A1F0, Update 0x43AE30 and Get_Current_Name
0x43B190, which carries musiclist.cpp's G_ASSERTs (l.364 / l.365, as MusicListClass's 0x438800): musiclist.cpp's
code is compiled into this object (the client's musicmanager.cpp includes it, or MusicListClass's functions are
inline there); until that is settled 0x43B190 cannot be written here.
*/
