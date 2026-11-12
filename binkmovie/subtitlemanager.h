/*
**	Command & Conquer Renegade(tm)
**	Copyright 2025 Electronic Arts Inc.
**	Modified for the Earth & Beyond client reconstruction: each change is recorded at the end of this file.
**	This program is free software: you can redistribute it and/or modify
**	it under the terms of the GNU General Public License as published by
**	the Free Software Foundation, either version 3 of the License, or
**	(at your option) any later version.
**
**	This program is distributed in the hope that it will be useful,
**	but WITHOUT ANY WARRANTY; without even the implied warranty of
**	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**	GNU General Public License for more details.
**
**	You should have received a copy of the GNU General Public License
**	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/****************************************************************************
*
* FILE
*     $Archive: /Commando/Code/BinkMovie/subtitlemanager.h $
*
* DESCRIPTION
*     Subtitling manager
*
* PROGRAMMER
*     Denzil E. Long, Jr.
*
* VERSION INFO
*     $Author: Denzil_l $
*     $Modtime: 1/15/02 9:13p $
*     $Revision: 2 $
*
****************************************************************************/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef _SUBTITLEMANAGER_H_
#define _SUBTITLEMANAGER_H_

#include "always.h"
#include <wwlib\vector.h>
#include "wwstring.h"
#include <windows.h>
#include "render2dsentence.h"

class SubTitleClass;
class Surface;

class SubTitleManagerClass
{
public:
	// Instantiate a subtitle manager
	static SubTitleManagerClass* Create(const char* filename);

	// Destroy subtitle manager
	~SubTitleManagerClass();

	// Check if there are subtitles.
	bool Has_Sub_Titles(void) const { return (mSubTitles != NULL); }

	// Reset subtitles to start
	void Reset(void);

	// Process subtitles
	bool Process(unsigned long movieTime);
	void Render();

private:
	// Prevent direct creation
	SubTitleManagerClass();

	bool Load_Sub_Titles(const char* moviename);
	void Draw_Sub_Title(const SubTitleClass* subtitle);

	DynamicVectorClass<class SubTitleClass*>* mSubTitles;
	int mSubTitleIndex;
	SubTitleClass* mActiveSubTitle;
	Render2DSentenceClass* Renderer;
};

#endif // _SUBTITLEMANAGER_H_

// Earth & Beyond client (reconstruction): changes to Renegade's subtitlemanager.h
// EnB: SubTitleManagerClass::Create: one argument, the movie's name (0xA16E20 reads [esp+4] only; the caller
//       0xA16444 pushes one argument and pops 4); the subtitle file and the font come from subtitlemanager.cpp's statics
// EnB: SubTitleManagerClass::Set_Font: removed (the client has no such function between the destructor 0xA16F80 and
//       Load_Sub_Titles 0xA16FE0; the constructor 0xA16F10 sets the font)
// EnB: SubTitleManagerClass::Load_Sub_Titles: one argument (0xA16FE0 ends ret 4)
// EnB: SubTitleManagerClass::Renderer: a pointer, newed by the constructor (0xA16F10: new 0x140, Render2DSentenceClass's
//       constructor, stored at +0xC; the object is 0x10 bytes, Create's new 0x10)
