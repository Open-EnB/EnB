// Earth & Beyond client (reconstruction): app\Client\tutorial_manager.cpp. Notes at the end of the file.
#include <stdio.h>
#include <string.h>
#include "tutorial_manager.h"
#include "..\shared\unknown_debug.h"
#include "..\..\wwlib\ini.h"
#include "..\..\wwlib\wwstring.h"

void Unknown_4BC000(const char *sender, const char *category, const char *file, int line, const char *format, ...);	// NAME 1 [Unknown_4BC000]: 0x4BC000, E&B's message reporter (tcpconn.cpp)
void Unknown_4BC260(const char *sender, const char *category, const char *file, int line, const char *format, ...);	// NAME 1 [Unknown_4BC260]: 0x4BC260, 0x4BC000's sibling

class StandardFileClass  // NAME 4: Westwood's wnd_file.cpp (Generals' Tools/Autorun/Wnd_File.h), the client's 0x6B61B0-0x6B6880
{
public:
	StandardFileClass();																	// 0x6B61B0
	~StandardFileClass();																// 0x6B61D0
	bool Open(const char *file_name, int open_mode);							// 0x6B6240
	bool Close();																			// 0x6B63B0
	int Write(void *buffer, unsigned long int bytes_to_write);				// 0x6B65A0

private:
	char Data[0x130];																		// FILE *, struct stat, the name, Currently_Open
};
bool Full_Path_File_Exists(char const *file_name);								// 0x6B6880 (wnd_file.cpp)

TutorialManagerClass *TutorialManagerClass::Instance;

TutorialStepStruct::~TutorialStepStruct()
{
	delete [] Name;
	Name = 0;
	for (std::vector<TutorialLineStruct *>::iterator i = Lines.begin(); i != Lines.end(); ++i) {
		delete *i;
	}
	Lines.clear();
}

void TutorialStepStruct::Add_Line(const char *speech, const char *text, bool skip_blend)
{
	Lines.push_back(new TutorialLineStruct(speech, text, skip_blend));
}

TutorialLineStruct *TutorialStepStruct::Get_Line(int index)
{
	if (index < (int)Lines.size()) {
		return Lines[index];
	}
	return 0;
}







































// NOT EXACT: inline budget (explain.py: inline-budget), the client calls the list's _Buynode 0x6AE690 out of line and inlines the key string's destructor after Steps[]; ours the reverse
TutorialManagerClass::TutorialManagerClass() : Steps(100), CurrentStep(0), CurrentLine(0), LastFinished(0)
{
	INIClass ini("tutorialdef.ini");
	if (!ini.Is_Loaded()) {
		Unknown_4BC260("Tutorial Manager", "", __FILE__, __LINE__, "tutorialdef.ini file not found");
	} else {





		int index = 0;
		while (true) {
			{
				char section[64];
				sprintf(section, "TUTORIAL-%d", index);
				if (ini.Find_Section(section) == 0) {
					break;
				}
				StringClass name;
				ini.Get_String(name, section, "Step", "");
				TutorialStepStruct *step = new TutorialStepStruct(name);
				if (Find(step->Name) != 0) {
					delete step;
					Unknown_4BC000("Tutorial Manager", "", __FILE__, __LINE__, "Duplicate tutorial step definition encountered");
				} else {
					int line = 0;
					while (true) {
						{
							char entry[64];
							sprintf(entry, "Speech%d", line);
							if (ini.Find_Entry(section, entry) == 0) {
								break;
							}
							StringClass speech;
							ini.Get_String(speech, section, entry, "");
							sprintf(entry, "Text%d", line);
							StringClass text;
							ini.Get_String(text, section, entry, "");
							bool skip_blend = ini.Get_Bool(section, "SkipBlend", false);
							step->Add_Line(speech, text, skip_blend);
						}
						line++;
					}
					Steps[std::string(step->Name)] = step;
				}
			}
			index++;
		}
		Read_Completed_File(false);
	}
}
// The hash_map iterator declared before the loop and assigned from begin() keeps it in registers as the client (0x5A7E80, 0x5A8100, 0x677410, 0x6ACD10) (one padding line below removed for Start_Step's __LINE__)
TutorialManagerClass::~TutorialManagerClass()
{
	std::hash_map<std::string, TutorialStepStruct *>::iterator i;
	for (i = Steps.begin(); i != Steps.end(); ++i) {
		delete (*i).second;
	}
	Steps.clear();
	for (std::list<char *>::iterator j = Completed.begin(); j != Completed.end(); ++j) {
		delete [] *j;
	}
	Completed.clear();
	delete [] LastFinished;
	LastFinished = 0;
}

void TutorialManagerClass::Set_Instance(TutorialManagerClass *manager)
{
	Instance = manager;
}
























void TutorialManagerClass::Start_Step(const char *name)
{
	std::hash_map<std::string, TutorialStepStruct *>::const_iterator i = Steps.find(std::string(name));
	if (i == Steps.end()) {
		Unknown_4BC260("Tutorial Manager", "", __FILE__, __LINE__, "Tutorial step %s not found!", name);
		CurrentStep = 0;
	} else {
		CurrentStep = (*i).second;
	}
	CurrentLine = 0;
}

void TutorialManagerClass::Stop_Step()
{
	CurrentStep = 0;
	CurrentLine = 0;
}

void TutorialManagerClass::Set_Completed(const char *name)
{
	Completed.push_back(nstrdup(name));
}

bool TutorialManagerClass::Is_Completed(const char *name)
{
	for (std::list<char *>::iterator i = Completed.begin(); i != Completed.end(); ++i) {
		if (strcmp(*i, name) == 0) {
			return true;
		}
	}
	return false;
}

bool TutorialManagerClass::Was_Just_Finished(const char *name)
{
	if (LastFinished != 0 && name != 0 && strcmp(name, LastFinished) == 0) {
		delete [] LastFinished;
		LastFinished = 0;
		return true;
	}
	return false;
}

TutorialStepStruct *TutorialManagerClass::Find(const char *name)
{
	std::hash_map<std::string, TutorialStepStruct *>::iterator i = Steps.find(name);
	if (i == Steps.end()) return 0;
	return (*i).second;
}

TutorialLineStruct *TutorialManagerClass::Next_Line()
{
	if (CurrentStep == 0) {
		return 0;
	}
	TutorialLineStruct *line = CurrentStep->Get_Line(CurrentLine);
	if (line != 0) {
		CurrentLine++;
	} else {
		delete [] LastFinished;
		LastFinished = nstrdup(CurrentStep->Name);
		CurrentStep = 0;
		CurrentLine = 0;
	}
	return line;
}

void TutorialManagerClass::Write_Completed_File()
{
	if (!Full_Path_File_Exists("tutorial.dat")) {
		StandardFileClass file;
		file.Open("tutorial.dat", 1);
		file.Write((void *)"Introductory Tutorial Completed", sizeof("Introductory Tutorial Completed"));
		file.Close();
	}
}
// NOT EXACT: inline budget (explain.py: register-only), the client calls the list's insert 0x6AE5A0 out of line for the first nine names; ours for seven, then alternates
bool TutorialManagerClass::Read_Completed_File(bool ignore)
{
	if (Full_Path_File_Exists("tutorial.dat")) {
		if (!ignore) {
			Completed.push_back(nstrdup("Welcome"));
			Completed.push_back(nstrdup("WelcomeReg"));
			Completed.push_back(nstrdup("ClassSelectionIntro"));
			Completed.push_back(nstrdup("ClassSelectionIntro"));
			Completed.push_back(nstrdup("AvatarCreationIntro"));
			Completed.push_back(nstrdup("AvatarCreationHead"));
			Completed.push_back(nstrdup("AvatarCreationBody"));
			Completed.push_back(nstrdup("ShipCreationGeometry"));
			Completed.push_back(nstrdup("ShipCreationColor"));
			Completed.push_back(nstrdup("ShipCreationDecal"));
			Completed.push_back(nstrdup("ShipCreationName"));
			Completed.push_back(nstrdup("ChooseShard"));
		}
		return true;
	}
	return false;
}















































































// NOT EXACT: inline budget (explain.py: inline-budget), the destructor's iterator loop and the constructor's string inlining (see both)
void TutorialManagerClass::Reload()
{
	Stop_Step();
	for (std::hash_map<std::string, TutorialStepStruct *>::iterator i = Steps.begin(); i != Steps.end(); ++i) {
		delete (*i).second;
	}
	Steps.clear();
	for (std::list<char *>::iterator j = Completed.begin(); j != Completed.end(); ++j) {
		delete [] *j;
	}
	Completed.clear();
	if (LastFinished != 0) {
		delete [] LastFinished;
		LastFinished = 0;
	}

	INIClass ini("tutorialdef.ini");
	if (!ini.Is_Loaded()) {
		Unknown_4BC260("Tutorial Manager", "", __FILE__, __LINE__, "tutorialdef.ini file not found");
	} else {





		int index = 0;
		while (true) {
			{
				char section[64];
				sprintf(section, "TUTORIAL-%d", index);
				if (ini.Find_Section(section) == 0) {
					break;
				}
				StringClass name;
				ini.Get_String(name, section, "Step", "");
				TutorialStepStruct *step = new TutorialStepStruct(name);
				if (Find(step->Name) != 0) {
					delete step;
					Unknown_4BC000("Tutorial Manager", "", __FILE__, __LINE__, "Duplicate tutorial step definition encountered");
				} else {
					int line = 0;
					while (true) {
						{
							char entry[64];
							sprintf(entry, "Speech%d", line);
							if (ini.Find_Entry(section, entry) == 0) {
								break;
							}
							StringClass speech;
							ini.Get_String(speech, section, entry, "");
							sprintf(entry, "Text%d", line);
							StringClass text;
							ini.Get_String(text, section, entry, "");
							bool skip_blend = ini.Get_Bool(section, "SkipBlend", false);
							step->Add_Line(speech, text, skip_blend);
						}
						line++;
					}
					Steps[std::string(step->Name)] = step;
				}
			}
			index++;
		}
	}
}

/*
**	Reconstruction notes
**
**	client.exe 0x6AC280-0x6AD7B0 (then the table's and the list's instances to about 0x6AEE60):
**	TutorialStepStruct's destructor 0x6AC280, Add_Line 0x6AC340 and Get_Line 0x6AC5C0 (both without a
**	caller, inlined where used), the manager's constructor 0x6AC600 (the table built as Steps(100)), the
**	destructor 0x6ACD10, Set_Instance 0x6ACF30 (0xBE66C8), Start_Step 0x6ACF50, Stop_Step 0x6AD080,
**	Set_Completed 0x6AD0A0, Is_Completed 0x6AD120, Was_Just_Finished 0x6AD1A0, Find 0x6AD230 (no caller;
**	identical bytes to buff_database.cpp's 0x677660), Next_Line 0x6AD350, Write_Completed_File 0x6AD3E0
**	and Read_Completed_File 0x6AD4B0 (tutorial.dat through wnd_file.cpp's StandardFileClass), Reload
**	0x6AD7B0 (no caller). The loaders read sections TUTORIAL-0, TUTORIAL-1, ... until one is missing, and
**	in each the entries Speech0/Text0, Speech1/Text1, ... until a Speech%d is missing; the reports sit on
**	the client's lines 94 / 114 (constructor), 190 (Start_Step) and 382 / 402 (Reload) (D25). Forms as
**	in buff_database.cpp: the indices outside their loops, incremented after an inner block; the
**	duplicate test first. Callers: 0x526DE0 (new, Set_Instance, delete), 0x783B50 / 0x784C90 (Start_Step,
**	Next_Line, Stop_Step), 31 callers each of Set_Completed and Is_Completed.
*/
