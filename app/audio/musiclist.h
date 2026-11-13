#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __MUSICLIST_H  // NAME 2: musiclist.cpp is the client's; header name assumed
#define __MUSICLIST_H

#include <deque>
#include <vector>
#include "musictheme.h"

class AudibleSoundClass;

// The play list of one kind of music (wander, combat, ...): a queue of themes, each a list of components
// played in order, and the sound playing now. 0x44 bytes (MusicManagerClass holds five at +0x50).
// Its functions (0x437180-0x438800, the deque's instances to 0x439C40) lie right before musicmanager.cpp's in the
// client, and MusicManagerClass's 0x43B190 holds Get_Current_Name inline with musiclist.cpp's own G_ASSERT lines
// (l.364 / l.365): musiclist.cpp is compiled in musicmanager.cpp's object (included by it, or its functions are
// inline). Open: how the tree's musiclist.cpp is built (see musicmanager.cpp's notes).
class MusicListClass  // NAME 3: class-from-file (D24) for audio\musiclist.cpp
{
public:
	typedef std::vector<MusicComponentClass *> ThemeType;	// the client's local is 'theme' (asserts 'CurrentComponent < theme.size()', l.161; 'component < theme.size()', l.365)  // NAME 2: what it holds

	MusicListClass();						// 0x437180
	~MusicListClass();						// 0x437540: the sound stopped and released, the queue cleared
	void Add_Theme(MusicThemeClass *musictheme);	// 0x437920 (FAIL_IF 'musictheme == NULL', l.97): one of its components, picked at random, queued as a theme  // NAME 2: what it does
	int Update(int fade_in);				// 0x437C30 (G_ASSERT 'Music == NULL', l.160): the next component started when the sound has ended; 1, 2 or 3 tells MusicManagerClass::Update to queue themes or to start a silence  // NAME 1 [fade_in]: what Fade_In gets; NAME 2: what it does
	void Update_Fade_Out();					// 0x438070: the sound faded out as it nears its end  // NAME 2: what it does
	void Stop();							// 0x4381E0: the sound stopped and released  // NAME 2: what it does
	void Fade_Out(int time);				// 0x438230: the sound faded out and released, the front theme dropped  // NAME 2: what it does
	void Fade_In(int time);					// 0x438430  // NAME 2: what it does
	void Reset();							// 0x438460: the sound stopped, the queue emptied  // NAME 2: what it does
	const char *Get_Current_Name();			// 0x438800 (G_ASSERT 'component >= 0', l.364): the playing component's ThemeName, NULL when silent  // NAME 2: what it returns

	std::deque<ThemeType> Themes;	// +0x00 (0x30 bytes; 0x10-byte elements)  // NAME 2: what it holds
	int CurrentComponent;			// +0x30: index into the front theme of the component to play next
	AudibleSoundClass *Music;		// +0x34: the playing sound, a reference held
	bool FadingOut;					// +0x38: 0x438070 set it  // NAME 2: what it does
	bool FadingIn;					// +0x39: Fade_In set it  // NAME 2: what it does
	MusicThemeClass *LastTheme;		// +0x3C: the last theme MusicManagerClass 0x43B1C0 looked at for this list  // NAME 2: what it holds
	bool Looping;					// +0x40: the sound's loop count set to 0 (0x437C30); 0x43B1C0 sets it for the combat list  // NAME 2: what it does
};

#endif // __MUSICLIST_H
