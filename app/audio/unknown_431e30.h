#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __UNKNOWN_431E30_H  // NAME 1: PROVISIONAL, the object's first address (no __FILE__ in it)
#define __UNKNOWN_431E30_H

#include <string>
#include <vector>
#include <hash_map.h>

class AudibleSoundClass;

// An early music player nobody constructs in the retail client (its constructor 0x431E30 has no caller): the
// tracks of ambient.ini's [General] SOUND_<n> and, per [Music-NNN], a sector's DefaultMusic; one track plays at a
// time through WWAudio, faded out before the next one starts. 0x40 bytes (the constructor's stores).
class Unknown431E30Class  // NAME 1: neutral (D10), the constructor's address
{
public:
	enum {
		MUSIC_SECTOR = 4,	// NAME 2: State 4 plays the sector's DefaultMusic (Update 0x4328B0)
		MUSIC_PAUSED = 5,	// NAME 2: PlayingState once the fade-out of a pause ends
	};

	Unknown431E30Class();					// 0x431E30 (no caller): Load_INI, the first one made is _Instance
	~Unknown431E30Class();					// 0x432080 (no caller)
	void Load_INI();						// 0x4322B0 ('ambient.ini')  // NAME 2: what it does
	void Set_Paused(bool paused);			// 0x4325F0  // NAME 2: what it does
	void Play(int track, bool loop);		// 0x432610 ('-NOMUSIC'): Tracks[track] played as music  // NAME 2: what it does
	void Stop();							// 0x4327D0  // NAME 2: what it does
	void Resume();							// 0x432840: Play again when the sound is not playing  // NAME 2: what it does
	int Is_Playing();						// 0x432880 (mov eax, 1: not a bool)  // NAME 2: what it returns
	void Update(bool flag);					// 0x4328B0: the fades; State played once the old track has faded out  // NAME 2: what it does; NAME 1 [flag]: what Get_Sector_Music gets
	int Get_Sector_Music(bool flag);		// 0x432B00: Sector's DefaultMusic, else 2 (flag) or 1  // NAME 2: what it returns; NAME 1 [flag]

	bool Is_Active()	{ if (_Instance == NULL) _Instance = this; return _Instance == this; }	// inline in 0x432080, 0x432610, 0x4327D0, 0x4328B0  // NAME 2: what it tests

	AudibleSoundClass *Sound;				// +0x00: the track playing, a reference held  // NAME 2: what it holds
	int Track;								// +0x04: Play's track  // NAME 2: what it holds
	int State;								// +0x08: 1 (the constructor): the track (or MUSIC_SECTOR) to play  // NAME 2: what Update compares
	int PlayingState;						// +0x0C: 1 (the constructor); 0 once stopped  // NAME 2: what Update compares
	bool Paused;							// +0x10: Update fades out while it is set  // NAME 2: what it does
	int Sector;								// +0x14: Get_Sector_Music's key  // NAME 2: what it holds
	std::vector<std::string> Tracks;		// +0x18: [General] SOUND_<n> (default 'Destination.MP3')  // NAME 2: what it holds
	std::hash_map<int, int> SectorMusic;	// +0x28: [Music-NNN] Sector -> DefaultMusic  // NAME 2: what it holds

	static Unknown431E30Class *_Instance;	// 0xBDAC38  // NAME 2: what it holds
};

#endif // __UNKNOWN_431E30_H
