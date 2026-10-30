// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Client\tutorial_manager.h (name assumed from
**	tutorial_manager.cpp, whose __FILE__ is 'e:\build\beta\G\app\Client\tutorial_manager.cpp')
**
**	The tutorial steps read from tutorialdef.ini (sections TUTORIAL-0, TUTORIAL-1, ..., each a Step name
**	and its Speech%d / Text%d lines) into an SGI hash_map keyed by the step name (libs/sgistl, D87), the
**	names of the completed tutorials, and the step being played. The client names neither the classes nor
**	the members (D10, D24); the ini keys and the messages ('Tutorial Manager') name what they can.
*/



#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __TUTORIAL_MANAGER_H  // NAME 3 [tutorial_manager.h]: header name assumed from the client's tutorial_manager.cpp
#define __TUTORIAL_MANAGER_H

#include <string>
#include <vector>
#include <list>
#include <hash_map.h>
#include "..\..\wwlib\nstrdup.h"

struct TutorialLineStruct  // NAME 2: what it holds (one Speech%d / Text%d pair, 0xC bytes)
{
	TutorialLineStruct(const char *speech, const char *text, bool skip_blend)
	{
		Speech = nstrdup(speech);
		Text = nstrdup(text);
		SkipBlend = skip_blend;
	}
	~TutorialLineStruct()
	{
		delete [] Speech;
		delete [] Text;
		Speech = 0;
		Text = 0;
	}

	char *Speech;																			// +0x00, Speech%d  // NAME 2: the ini key
	char *Text;																				// +0x04, Text%d  // NAME 2: the ini key
	bool SkipBlend;																		// +0x08, SkipBlend  // NAME 2: the ini key
};

struct TutorialStepStruct  // NAME 2: what it holds (one TUTORIAL-n section, 0x14 bytes)
{
	TutorialStepStruct(const char *name)
	{
		Name = nstrdup(name);
	}
	~TutorialStepStruct();																// 0x6AC280

	void Add_Line(const char *speech, const char *text, bool skip_blend);	// 0x6AC340 (no caller; inlined in the loaders)  // NAME 2: describes what it does
	TutorialLineStruct *Get_Line(int index);										// 0x6AC5C0 (no caller; inlined in Next_Line)  // NAME 2: describes what it does

	char *Name;																				// +0x00, Step  // NAME 2: the ini key
	std::vector<TutorialLineStruct *> Lines;										// +0x04  // NAME 2: describes what it holds
};

class TutorialManagerClass  // NAME 3: by convention (D24) from the client's file tutorial_manager.cpp ('Tutorial Manager' in its messages, 'starting up Tutorial Manager system')
{
public:
	TutorialManagerClass();																// 0x6AC600: loads tutorialdef.ini
	~TutorialManagerClass();															// 0x6ACD10

	static void Set_Instance(TutorialManagerClass *manager);					// 0x6ACF30 (0xBE66C8)  // NAME 2: describes what it does
	void Start_Step(const char *name);												// 0x6ACF50  // NAME 2: describes what it does
	void Stop_Step();																		// 0x6AD080  // NAME 2: describes what it does
	void Set_Completed(const char *name);											// 0x6AD0A0  // NAME 2: describes what it does
	bool Is_Completed(const char *name);											// 0x6AD120  // NAME 2: describes what it does
	bool Was_Just_Finished(const char *name);										// 0x6AD1A0  // NAME 2: describes what it does
	TutorialStepStruct *Find(const char *name);									// 0x6AD230 (no caller)  // NAME 2: describes what it does
	TutorialLineStruct *Next_Line();													// 0x6AD350  // NAME 2: describes what it does
	void Write_Completed_File();														// 0x6AD3E0: writes tutorial.dat  // NAME 2: describes what it does
	bool Read_Completed_File(bool ignore);											// 0x6AD4B0: reads tutorial.dat  // NAME 2: describes what it does
	void Reload();																			// 0x6AD7B0 (no caller)  // NAME 2: describes what it does

	static TutorialManagerClass *Instance;											// 0xBE66C8  // NAME 2: describes what it holds

private:
	std::hash_map<std::string, TutorialStepStruct *> Steps;					// +0x00  // NAME 2: describes what it holds
	std::list<char *> Completed;														// +0x18  // NAME 2: describes what it holds
	TutorialStepStruct *CurrentStep;													// +0x24  // NAME 2: describes what it holds
	int CurrentLine;																		// +0x28  // NAME 2: describes what it holds
	char *LastFinished;																	// +0x2C  // NAME 2: describes what it holds
};

#endif //__TUTORIAL_MANAGER_H
