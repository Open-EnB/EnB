// L18 (run 2026-11-11b): MusicManagerClass::Update 0x43AE30, round 2 (751 bytes for the client's 687, 0.55).
// Round 1 (718 bytes, 0.61) differed only in the silence test: 'if (SilenceEnd != 0) { if (timeGetTime() > SilenceEnd) SilenceEnd = 0; return; }'
// before the switch; the client has that block out of line at the end (this form's if / else), keeps 0 in ebp and -1 in ebx
// (ArgvClass::Find's CurrentPos, reused for the == -1 tests) and pushes ebx in the prologue. Not in the tree: it would
// replace the generated stand-in the POC calls every frame while Load_INI 0x43A1F0 is still a stand-in.
// @0x43AE30
void MusicManagerClass::Update(int type, int level, int race, int theme_race)
{
	if (SectorMusic != NULL) {
		if (SectorMusic->Is_Playing() == true) {
			return;
		}
		if (SectorMusic != NULL) {
			SectorMusic->Stop(true);
			SectorMusic->Release_Ref();
			SectorMusic = NULL;
		}
	}
	if (!IsLoaded) {
		Load_INI(race);
	}
	Level = level;
	Set_Music_Type(type);
	ArgvClass argv;
	if (argv.Find("-NOMUSIC") != NULL) {
		return;
	}
	if (CurrentMusicType == MUSIC_THEME_NONE) {
		return;
	}
	if (!Enabled) {
		return;
	}
	if (Sector != -1) {
		Queue_Sector_Themes();
	}
	if (SilenceEnd == 0) {
		switch (Lists[CurrentMusicType].Update(FadeIn[CurrentMusicType])) {
			case 3:
				if (SectorThemeSector != -1) {
					if (!Queue_Sector_Theme(SectorThemeSector)) {
						Queue_Themes(CurrentMusicType, theme_race);
					}
				} else {
					Queue_Themes(CurrentMusicType, theme_race);
				}
				SilenceEnd = Unknown_BDBB84(SilenceMin, SilenceMax) + Unknown_Get_Ticks();
				if (CurrentMusicType == MUSIC_THEME_STARGATE) {
					Enabled = false;
				}
				break;
			case 2:
				SilenceEnd = Unknown_BDBB84(SilenceMin, SilenceMax) + Unknown_Get_Ticks();
				if (CurrentMusicType == MUSIC_THEME_STARGATE) {
					Enabled = false;
				}
				break;
			case 1:
				if (SectorThemeSector != -1) {
					if (!Queue_Sector_Theme(SectorThemeSector)) {
						Queue_Themes(CurrentMusicType, theme_race);
					}
				} else {
					Queue_Themes(CurrentMusicType, theme_race);
				}
				break;
		}
	} else if (timeGetTime() > SilenceEnd) {
		SilenceEnd = 0;
	}
}

