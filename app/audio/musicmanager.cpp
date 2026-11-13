// Earth & Beyond client (reconstruction): app\Audio\musicmanager.cpp. Notes at the end of the file.
#include "musicmanager.h"
#include "..\shared\g_assert.h"
#include "..\..\wwlib\ini.h"
#include "..\..\wwaudio\AudibleSound.h"
#include "..\..\wwaudio\WWAudio.h"
#include "..\..\wwlib\random.h"
#include <stdio.h>
#include <string.h>
#include "..\..\wwlib\inisup.h"
extern RandomClass Unknown_BDBB84;	// NAME 1 [Unknown_BDBB84]: neutral (D10), as avatar.cpp's
unsigned int Unknown_Get_Ticks();	// 0x4A3B40, the aux-data clock (soundlib.cpp's)

















































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



















// @0x43A1F0
// NOT EXACT: inline budget (explain.py: 2287 bytes vs 2227, 0.61): the client calls push_back's _Ucopy / _Ufill / _Destroy (0x43BF20, 0x43BF60, 0x43BC60) and its last size() (0x43BC30) out of line, here they are inline; this and the section name swap edi / ebp
void MusicManagerClass::Load_INI(int race)
{
	G_ASSERT(MusicINI);
	G_ASSERT(MusicINI->Is_Loaded());
	for (int type = 0; type < NUM_MUSIC_THEME_TYPES; type++) {
		Themes[type].clear();
		FadeIn[type] = 6000;
		FadeOut[type] = 2000;
	}
	char section[32];
	_snprintf(section, sizeof(section), "Global", 6);
	if (MusicINI->Section_Present(section)) {
		FadeIn[MUSIC_THEME_COMBAT] = (int)(MusicINI->Get_Float(section, "CombatFadeIn", 6.0f) * 1000.0f);
		FadeOut[MUSIC_THEME_COMBAT] = (int)(MusicINI->Get_Float(section, "CombatFadeOut", 2.0f) * 1000.0f);
		FadeIn[MUSIC_THEME_WANDER] = (int)(MusicINI->Get_Float(section, "WanderFadeIn", 6.0f) * 1000.0f);
		FadeOut[MUSIC_THEME_WANDER] = (int)(MusicINI->Get_Float(section, "WanderFadeOut", 2.0f) * 1000.0f);
		FadeIn[MUSIC_THEME_PLANET] = (int)(MusicINI->Get_Float(section, "PlanetFadeIn", 6.0f) * 1000.0f);
		FadeOut[MUSIC_THEME_PLANET] = (int)(MusicINI->Get_Float(section, "PlanetFadeOut", 2.0f) * 1000.0f);
		SilenceMin = (int)(MusicINI->Get_Float(section, "SilenceMin", 0.0f) * 1000.0f);
		SilenceMax = (int)(MusicINI->Get_Float(section, "SilenceMax", 0.0f) * 1000.0f);
	}
	for (INISection *ini_section = MusicINI->Get_Section_List().First_Valid(); ini_section != NULL; ini_section = ini_section->Next_Valid()) {
		const char *name = ini_section->Section;
		if (strncmp(name, "MusicTheme", 10) == 0) {
			char temp[32];
			MusicINI->Get_String(name, "Race", "Other", temp, sizeof(temp));
			strupr(temp);
			int theme_race = Race_From_Name(temp);
			MusicINI->Get_String(name, "Type", "Wander", temp, sizeof(temp));
			strupr(temp);
			int theme_type = Type_From_Name(temp);
			if (theme_race == race || theme_race == 8 || theme_type == MUSIC_THEME_STARBASE) {
				MusicThemeClass *theme = new MusicThemeClass;


















				G_ASSERT(theme);
				theme->Race = theme_race;
				theme->Sector = MusicINI->Get_Int(name, "Sector", -1);
				theme->LevelAvailable = MusicINI->Get_Int(name, "LevelAvailable", 0);
				int count = MusicINI->Get_Int(name, "ComponentCount", 0);
				for (int component = 0; component < count; component++) {
					char entry[32];
					_snprintf(entry, sizeof(entry), "Component-%02d", component);
					MusicINI->Get_String(name, entry, "None", temp, sizeof(temp));






					G_ASSERT(strcmp("None", temp) != 0);
					_snprintf(entry, sizeof(entry), "FadeIn-%02d", component);
					float fade_in = MusicINI->Get_Float(name, entry, 0.0f);
					float fade_out = MusicINI->Get_Float(name, entry, 0.0f);
					theme->Add_Component(temp, fade_in, fade_out);
				}
				Themes[theme_type].push_back(theme);
			}
		}
	}
	IsLoaded = true;
	for (type = 0; type < NUM_MUSIC_THEME_TYPES; type++) {
		Queue_Themes(type, 8);
	}
	SilenceEnd = Unknown_Get_Ticks() + 5000;
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

// @0x43B190
const char *MusicManagerClass::Get_Current_Name()
{
	if (CurrentMusicType == MUSIC_THEME_NONE) {
		return NULL;
	}
	return Lists[CurrentMusicType].Get_Current_Name();
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

// @0x43BBA0
void MusicManagerClass::Stop_Sector_Music()
{
	if (SectorMusic != NULL) {
		SectorMusic->Stop(true);
		SectorMusic->Release_Ref();
		SectorMusic = NULL;
	}
}

/*
Reconstruction notes:
musicmanager.cpp holds MusicManagerClass: the themes of music.ini by kind of music, a MusicListClass per kind and
the kind playing now. The object is 0x439E40-0x43BFD0 (after musiclist.cpp's deque instances, before
musictheme.cpp's 0x43C010). Not rebuilt yet: Update 0x43AE30. Load_INI 0x43A1F0 is NOT EXACT (inline budget:
the client keeps push_back's helpers out of line; one dead size() after the push_back gives 0.74, 2219 bytes, with
size() out of line one call too early). Get_Current_Name 0x43B190 is a plain tail call of MusicListClass's 0x438800
(no assert of its own: the musiclist.cpp lines 364 / 365 are 0x438800's). Stop_Sector_Music 0x43BBA0 has no caller:
the out-of-line copy of a member whose code the destructor, Stop and Play_Music hold inline.
*/
