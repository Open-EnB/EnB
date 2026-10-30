// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Client\tasks\tsk_smovie.cpp (client 0x76C670-0x76CFBA)
**
**	The scripted movie task (tsk_smovie.h). The asserts sit on the client's lines (61, 88, 109,
**	175, 257, 283, 305, 374: D25). Notes at the end.
*/



#include "tsk_smovie.h"
#include "..\..\shared\unknown_debug.h"
#include "ffactory.h"
#include "wwfile.h"
#include "nstrdup.h"
#include <stdio.h>
#include <string>
#include "..\client.h"
// mclient.cpp's movie client (0x1178 bytes, vtable 0xB07864; mclient.cpp isn't rebuilt) on the client
// base ClientClass (client.h): declared here only, as far as this task uses it.
class MovieClientClass : public ClientClass	// NAME 2: mclient.cpp's class, the movie script's player
{
public:
	MovieClientClass(ScriptedMovieTaskClass *task, KernelClass *kernel);	// 0x7EC3E0
	~MovieClientClass();										// 0x7EC570 (not virtual: delete calls it directly)
	virtual void Init();										// +0x00 .. +0x14 (and +0x1C): its own, 0x7EC550-0x7ECAA0
	virtual void Shutdown();
	virtual void Unknown_08();
	virtual void Process_Packets();
	virtual void Unknown_10();
	virtual int Do_Frame(bool focused);							// +0x14: Run passes Is_Focused()
	void Load_Script(const char *filename);					// 0x7ECC80  // NAME 2: Accept_Focus's '%s%d.txt'
	void Handle_Input(KernelEventClass event);				// 0x7ECA50  // NAME 2: Pre_Run's events
	void Render();											// 0x7ECDC0  // NAME 2: called from Render
	bool Is_Done();											// 0x7ED090  // NAME 2: Run deletes the client then

	char Unknown1150[0x1178 - 0x1150];						// +0x1150
};

void Unknown_Load_Assets(const std::string &ini);	// NAME 2 [Unknown_Load_Assets]: 0x729BF0 (client_lego_impl.cpp), 'cbasset.ini'
void Unknown_Free_Assets();							// NAME 2 [Unknown_Free_Assets]: 0x729D60 (client_lego_impl.cpp)
bool Unknown_42EA80(KernelEventClass *event);		// NAME 1 [Unknown_42EA80]: the input queue's next event (input.cpp)
void Unknown_42E8A0(KernelEventClass *event);		// NAME 1 [Unknown_42E8A0]: hands the event on (input.cpp)











// 0x76C670
ScriptedMovieTaskClass::ScriptedMovieTaskClass(const char *moviename, int script_number) :
	Movie_Name(NULL),
	Script_Number(script_number)
{
	G_ASSERT(moviename);
	Set_Movie_Name(moviename);
}

















// 0x76C7D0
ScriptedMovieTaskClass::~ScriptedMovieTaskClass()
{
	if (Movie_Name != NULL) {
		delete Movie_Name;
		Movie_Name = NULL;
	}
	G_ASSERT(Clients.size() == 0);
}
















// 0x76C8B0
bool ScriptedMovieTaskClass::Init(KernelClass *kernel)
{
	FAIL_IF(kernel == NULL) {
		return false;
	}
	Clients.push_back(new MovieClientClass(this, kernel));
	// NOT EXACT: the client's loop keeps its test at the top (a jmp back to it); while, for, for (;;)
	// with a break, a goto and ++ in the call all give VC6's rotated loop (the test at the bottom)
	int number = Script_Number + 1;
	while (Movie_Name != NULL) {
		char filename[128];
		sprintf(filename, "%s%d.txt", Movie_Name, number);
		file_auto_ptr file(_TheFileFactory, filename);
		if (!file->Is_Available()) {
			break;
		}
		Clients.push_back(new MovieClientClass(this, kernel));
		number++;
	}
	Unknown_Load_Assets("cbasset.ini");
	return true;
}

// 0x76CB60
bool ScriptedMovieTaskClass::Shutdown()
{
	Unknown_Free_Assets();
	return true;
}




































// 0x76CB80
bool ScriptedMovieTaskClass::Accept_Focus()
{
	FAIL_IF(Clients.size() == 0) {
		return false;
	}
	if (Movie_Name != NULL) {
		char filename[128];
		sprintf(filename, "%s%d.txt", Movie_Name, Script_Number);
		Script_Number++;
		Clients[0]->Load_Script(filename);
		return true;
	}
	return false;
}

// 0x76CC20
bool ScriptedMovieTaskClass::Yield_Focus()
{
	if (Clients.size() > 1) {
		return false;
	}
	return true;
}

// 0x76CC50
int ScriptedMovieTaskClass::Get_Interval()
{
	if (Kernel != NULL && Kernel->Render_Enabled && Kernel->Device_Ok) {
		return 0;
	}
	return 500;
}

















































// 0x76CC80
void ScriptedMovieTaskClass::Render()
{
	FAIL_IF(Clients.size() == 0) {
		return;
	}
	if (!Clients[0]->Is_Done() && Is_Focused()) {
		Clients[0]->Render();
	}
}
















// 0x76CD00
void ScriptedMovieTaskClass::Set_Movie_Name(const char *moviename)
{
	FAIL_IF(!moviename) {
		return;
	}
	Movie_Name = nstrdup(moviename);
}














// 0x76CD50
void ScriptedMovieTaskClass::Run()
{
	FAIL_IF(Clients.size() == 0) {
		return;
	}
	Clients[0]->Do_Frame(Is_Focused());
	if (Clients[0]->Is_Done()) {
		delete Clients[0];
		Clients[0] = NULL;
		Clients.erase(Clients.begin());
		if (Clients.size() == 0) {
			if (Kernel != NULL) {
				Kernel->Remove_Task(this);
			}
		} else {
			Accept_Focus();
		}
	}
}

// 0x76CE70
void ScriptedMovieTaskClass::Pre_Run()
{
	KernelEventClass event;
	while (Unknown_42EA80(&event)) {
		Unknown_42E8A0(&event);
		if (event.Key != 0) {
			Handle_Input(&event);
		}
	}
}





































// 0x76CF60
void ScriptedMovieTaskClass::Handle_Input(KernelEventClass *event)
{
	FAIL_IF(Clients.size() == 0) {
		return;
	}
	Clients[0]->Handle_Input(*event);
}

/*
**	Reconstruction notes (client.exe 0x76C670-0x76CFBA; vtable 0xB04360, slots in tsk_smovie.h).
**	The constructor inlines Set_Movie_Name (its FAIL_IF on line 283 appears in both), Pre_Run
**	inlines Handle_Input (line 374 in both; 0x76CF60 has no caller of its own). Init makes one
**	client for the first script and one more for each further script file the file factory has
**	(moviescript2.txt ...); Accept_Focus loads the next script into the first client, and Run
**	deletes a finished client, so each client plays one script. Slot +0x18 returns this (the base's
**	returns 0): presumably a type query. After this file's functions: 0x76CFE0 (an unwind helper:
**	a vector's storage freed), 0x76D020 (vector<MovieClientClass *>::insert, the instantiation),
**	0x76D340 (another object's).
*/
