/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Client\tasks\tsk_movie.h (name assumed from tsk_movie.cpp)
**
**	The movie task (docs/kernel-map.md 3): plays a list of Bink movies through Renegade's BINKMovie
**	(client 0xA161B0-0xA162D0), one after the other, while it has the focus; a key after the first
**	3 s skips; it removes itself after the last. WinMain makes one for the intro (eb_sizzle.bik).
**	Vtable 0xB04280, 0x38 bytes.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef TSK_MOVIE_H  // NAME 3 [tsk_movie.h]: header name assumed from the client's tsk_movie.cpp
#define TSK_MOVIE_H

#include <string>
#include <vector>
#include "..\..\kernel\kernel.h"

class MovieTaskClass : public TaskClass		// NAME 3: after tsk_movie.cpp and LoginTaskClass (D24)
{
public:
	MovieTaskClass();									// 0x76BF40
	virtual ~MovieTaskClass();							// 0x76C030 (scalar deleting 0x76C000)
	virtual bool Accept_Focus();						// +0x28 0x76C140: starts the current movie
	virtual bool Yield_Focus();							// +0x2C 0x76C1C0: only after the last movie
	virtual void Run();									// +0x30 0x76C430: the next movie, or removes itself
	virtual int Get_Interval();							// +0x34 0x76C200
	virtual void Pre_Run();								// +0x38 0x76C4D0: the input (skip)
	virtual void Render();								// +0x3C 0x76C230
	virtual bool Delete_On_Remove() { return true; }	// +0x44 0x76BFE0

	void Add_Movie(const char *movie_name, int volume);	// 0x76C2C0 (the assert '!movie_name')  // NAME 2: what it does
	void Handle_Input(KernelEventClass *event);			// 0x76C5A0 (inlined in Pre_Run)  // NAME 2: what it does

	std::vector<std::string> Movies;					// +0x0C NAME 2: the Bink files, in order
	std::vector<int> Volumes;							// +0x1C NAME 2: each movie's BinkSetVolume (WinMain: 0x8000)
	unsigned int Index;									// +0x2C NAME 2: the movie playing
	unsigned int Start_Time;							// +0x30 NAME 2: timeGetTime at its first frame
	bool Starting;										// +0x34 NAME 2: set when a movie starts, cleared at its first Render
};

#endif
