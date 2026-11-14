// Earth & Beyond client (reconstruction): app\Audio\musiclist.cpp. Notes at the end of the file.
// An object of its own, linked right before musicmanager.cpp's (see the notes).
#include "musiclist.h"
#include "..\shared\g_assert.h"
#include "..\..\wwaudio\AudibleSound.h"
#include "..\..\wwaudio\WWAudio.h"
#include "..\..\wwlib\random.h"
extern RandomClass Unknown_BDBB84;	// NAME 1 [Unknown_BDBB84]: neutral (D10), as avatar.cpp's

// @0x437180
MusicListClass::MusicListClass() :
	CurrentComponent(0),
	Music(NULL),
	FadingOut(false),
	FadingIn(false),
	LastTheme(NULL),
	Looping(false)
{
	Themes.clear();
}

// @0x437540
MusicListClass::~MusicListClass()
{
	Reset();
}



































































// @0x437920
void MusicListClass::Add_Theme(MusicThemeClass *musictheme)
{
	FAIL_IF(musictheme == NULL) return;
	G_ASSERT(musictheme->Get_Num_Components());

	ThemeType theme;
	theme.push_back(musictheme->Get_Component(Unknown_BDBB84(0, musictheme->Get_Num_Components() - 1)));
	Themes.push_back(theme);
}
































// @0x437C30
// NOT EXACT: shape (explain.py: a merged or split tail, 0.74): the client shares one `return 3` tail between the two pop_front exits and keeps the stop branch's size in ecx; a `?:` return in the stop branch gives 0.80 with setcc code the client does not have
int MusicListClass::Update(int fade_in)
{
	if (Music != NULL && Music->Is_Playing()) {
		Update_Fade_Out();
		return 0;
	}
	if (Music == NULL && Themes.size() == 0) {
		return 1;
	}

	ThemeType theme = Themes.front();
	if (Music != NULL && !Music->Is_Playing()) {
		Stop();
		if (CurrentComponent >= (int)theme.size()) {
			CurrentComponent = 0;
			Themes.pop_front();
			if (Themes.size() != 0) {
				return 2;
			}
			return 3;
		}
	}
	G_ASSERT(Music == NULL);
	G_ASSERT(CurrentComponent < theme.size());
	Music = WWAudioClass::Get_Instance()->Create_Sound_Effect(theme[CurrentComponent]->ThemeName);
	if (Music != NULL) {
		Music->Set_Type(AudibleSoundClass::TYPE_MUSIC);
		Music->Play(true);
		if (Looping) {
			Music->Set_Loop_Count(0);
		}
		Music->Set_Volume(1.0f);
		if (CurrentComponent == 0) {
			Music->Fade_In((int)theme[0]->FadeIn);
		} else if (FadingIn == true) {
			Music->Fade_In(fade_in);
		}
		CurrentComponent++;
	} else {
		CurrentComponent++;
		if (CurrentComponent >= (int)theme.size()) {
			CurrentComponent = 0;
			Themes.pop_front();
			if (Themes.size() == 0) {
				return 3;
			}
			return 2;
		}
	}
	return 0;
}

// @0x438070
void MusicListClass::Update_Fade_Out()
{
	if (Music != NULL) {
		ThemeType theme = Themes.front();
		if (!FadingOut && CurrentComponent == theme.size()) {
			int time = (int)theme[CurrentComponent - 1]->FadeOut;
			int start = Music->Get_Duration() - time;
			int position = Music->Get_Play_Position();
			if (position >= start) {
				FadingOut = true;
				Music->Fade_Out(time);
			}
		}
	}
}

// @0x4381E0
void MusicListClass::Stop()
{
	if (Music != NULL) {
		Music->Stop(true);
		Music->Release_Ref();
		Music = NULL;
		FadingOut = false;
		FadingIn = false;
	}
}

// @0x438230
void MusicListClass::Fade_Out(int time)
{
	if (Music != NULL) {
		Music->Fade_Out(time);
		Music->Release_Ref();
		Music = NULL;

		ThemeType theme = Themes.front();
		if (CurrentComponent >= (int)theme.size()) {
			CurrentComponent = 0;
			Themes.pop_front();
		}
	}
}

// @0x438430
void MusicListClass::Fade_In(int time)
{
	if (Music != NULL) {
		Music->Fade_In(time);
		FadingIn = true;
	}
}

// @0x438460
void MusicListClass::Reset()
{
	Stop();
	Themes.clear();
	CurrentComponent = 0;
}








































































































// @0x438800
const char *MusicListClass::Get_Current_Name()
{
	if (Music == NULL) {
		return NULL;
	}

	ThemeType theme = Themes.front();
	int component = CurrentComponent - 1;
	G_ASSERT(component >= 0);
	G_ASSERT(component < theme.size());
	return theme[component]->ThemeName;
}

/*
Reconstruction notes:
musiclist.cpp holds MusicListClass, the play list of one kind of music (MusicManagerClass holds five at +0x50). It is
an object of its own, 0x437180-0x439C40 (MusicListClass's functions to 0x438800, then the deque's and vector's
instances), linked right before musicmanager.cpp's 0x439E40 by name. Evidence: musicmanager.cpp calls Reset 0x438460,
Fade_Out 0x438230, Fade_In 0x438430 (31 bytes), Add_Theme 0x437920 and Get_Current_Name 0x438800 out of line
(0x43A040, 0x43ACE0, 0x43AD70, 0x43B860, 0x43B190), where /Ob2 inlines them in one translation unit: built through an
`#include "musiclist.cpp"` in musicmanager.cpp they went inline and those five musicmanager.cpp functions stopped
matching. musiclist.cpp's literals lie before musicmanager.cpp's in .data (0xB6EFB8-0xB6F080, then 'music.ini'
0xB6F094), the link order. The missing guarded <string> initialiser (none between bridgesounds.cpp's 0x437090 and
musicmanager.cpp's 0x43BFA0) is no evidence against it: the file includes no <string> (a standalone compile of this
file emits none either). 0x43B190 has no assert of its own (it tail-calls 0x438800, whose lines 364 / 365 these are).
NOT EXACT: Update 0x437C30 (0.74, shape: the client shares the `return 3` tail of both pop_front branches and
allocates the stop branch's registers another way; attempts.csv).
*/
