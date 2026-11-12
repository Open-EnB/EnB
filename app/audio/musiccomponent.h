#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __MUSICCOMPONENT_H  // NAME 2: musiccomponent.cpp is the client's; header name assumed
#define __MUSICCOMPONENT_H

// One piece of music of a theme: music.ini's Component-NN of a [MusicTheme-NNN] section. 0xC bytes (made at
// MusicThemeClass::Add_Component 0x43C100).
class MusicComponentClass  // NAME 3: class-from-file (D24) for audio\musiccomponent.cpp
{
public:
	MusicComponentClass(const char *name, float fade_in, float fade_out);	// 0x4370C0 (G_ASSERT 'name', l.45; the times are seconds)  // NAME 2 [fade_in, fade_out]: the ini's FadeIn-NN, read twice
	~MusicComponentClass();					// 0x437140 (G_ASSERT 'ThemeName', l.67)

	char *ThemeName;	// +0x00: nstrdup(name), the sound file's name (WWAudioClass::Create_Sound_Effect gets it)
	float FadeIn;		// +0x04: milliseconds  // NAME 2: the ini's FadeIn-NN
	float FadeOut;		// +0x08: milliseconds; the client reads FadeIn-NN for it as well  // NAME 2: the second time
};

#endif // __MUSICCOMPONENT_H
