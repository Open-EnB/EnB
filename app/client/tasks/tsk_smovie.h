/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Client\tasks\tsk_smovie.h (name assumed from tsk_smovie.cpp)
**
**	The scripted movie task (docs/kernel-map.md 3): plays movie scripts (WinMain: moviescript1.txt,
**	moviescript2.txt ... as long as the files exist) through mclient.cpp's movie clients, one client
**	per script, the first one only; it removes itself after the last. Vtable 0xB04360, 0x24 bytes.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef TSK_SMOVIE_H  // NAME 3 [tsk_smovie.h]: header name assumed from the client's tsk_smovie.cpp
#define TSK_SMOVIE_H

#include <vector>
#include "..\..\kernel\kernel.h"

class MovieClientClass;

class ScriptedMovieTaskClass : public TaskClass		// NAME 2: tsk_smovie.cpp's task: movie scripts (D24)
{
public:
	ScriptedMovieTaskClass(const char *moviename, int script_number);	// 0x76C670 (the assert 'moviename')  // NAME 2 [script_number]: the first script's number (WinMain: 1)
	virtual ~ScriptedMovieTaskClass();					// 0x76C7D0 (scalar deleting 0x76C7A0)
	virtual int Unknown_18() { return (int)this; }		// +0x18 0x76C760 (the base's returns 0)
	virtual bool Accept_Focus();						// +0x28 0x76CB80: the next script into the first client
	virtual bool Yield_Focus();							// +0x2C 0x76CC20: once one client is left
	virtual void Run();									// +0x30 0x76CD50
	virtual int Get_Interval();							// +0x34 0x76CC50
	virtual void Pre_Run();								// +0x38 0x76CE70: the input to the first client
	virtual void Render();								// +0x3C 0x76CC80
	virtual bool Shutdown();							// +0x40 0x76CB60
	virtual bool Delete_On_Remove() { return true; }	// +0x44 0x76C780

	bool Init(KernelClass *kernel);						// 0x76C8B0 (the assert 'kernel == NULL')  // NAME 2: what it does
	void Set_Movie_Name(const char *moviename);			// 0x76CD00 (the assert '!moviename')  // NAME 2: what it does
	void Handle_Input(KernelEventClass *event);			// 0x76CF60 (inlined in Pre_Run)  // NAME 2: what it does

	char *Movie_Name;									// +0x0C NAME 3: the asserts' 'moviename'
	std::vector<MovieClientClass *> Clients;			// +0x10 (the asserts 'Clients.size() == 0')
	int Script_Number;									// +0x20 NAME 2: the next script's number ('%s%d.txt')
};

#endif
