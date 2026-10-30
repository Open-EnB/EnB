// Earth & Beyond client (reconstruction)
// The start-up tasks on the rebuilt kernel (docs/kernel-map.md 6, step 7): app\client\tasks'
// tsk_movie.cpp, tsk_smovie.cpp and tsk_menu.cpp with kernel.cpp, on kerneltest's clock.
//
// The movie task plays through a BINKMovie defined here: with binkw32.dll and a .bik (the game's
// eb_ws_logo.bik, copied by kerneltest.py) it decodes the real file frame by frame, without sound
// or drawing; without them a 5-frame stand-in. WW3D's Begin_Render / End_Render, mclient.cpp's
// movie client, client_lego_impl.cpp's asset calls and LoginTaskClass are stand-ins that log.
//
// Cases: the movie task takes the focus when added, plays its movies in order (Render only while
// focused), refuses to yield until the last is done, removes itself (Pending_Delete); a key in the
// first 3 s is ignored, after 3 s it skips (timeGetTime: a real wait); the scripted movie task
// makes one client per script file found (moviescript2.txt, moviescript3.txt; not 4), loads the
// next script on each focus, deletes finished clients and removes itself after the last; MenuTask
// starts the login task, hands it the focus and removes itself; its key 0x51 stops the kernel.
#include <stdio.h>
#include <string.h>
#include <string>
#include <windows.h>
#include "kernel.h"
#include "..\..\..\app\client\tasks\tsk_movie.h"
#include "..\..\..\app\client\tasks\tsk_smovie.h"
#include "..\..\..\app\client\tasks\tsk_menu.h"
#include "..\..\..\app\client\tasks\tsk_login.h"
#include "ww3d.h"

extern unsigned int Clock;
extern KernelEventClass Pending;
extern bool Has_Pending;
static FILE *Out;

// ------------------------------------------------------------------------- BINKMovie
// The client's library is Renegade's BinkMovie (not rebuilt); this one decodes without drawing.
// Its declaration as tsk_movie.cpp's (E&B's Play takes a volume).
class BINKMovie
{
public:
	static void Play(const char *filename, const char *subtitlename, int volume);
	static void Stop();
	static void Update();
	static void Render();
	static void Init();
	static void Shutdown();
	static bool Is_Complete();
};

typedef void *(__stdcall *BinkOpenProc)(const char *, unsigned int);
typedef int (__stdcall *BinkFrameProc)(void *);
typedef void (__stdcall *BinkVoidProc)(void *);
typedef void (__stdcall *BinkTrackProc)(unsigned int);
static HMODULE Bink_Dll;
static BinkOpenProc Bink_Open;
static BinkFrameProc Bink_Do_Frame;
static BinkVoidProc Bink_Next_Frame;
static BinkVoidProc Bink_Close;
static BinkTrackProc Bink_Set_Sound_Track;
struct BinkHead { unsigned int Width, Height, Frames, FrameNum; };
static BinkHead *Movie;					// the real one, or Fake
static BinkHead Fake;
static bool Playing;
static int Rendered;
static const char *Real_File;			// kerneltest's argument: the .bik to play instead of each name

void BINKMovie::Init()
{
	fprintf(Out, "bink init\n");
}

void BINKMovie::Shutdown()
{
	fprintf(Out, "bink shutdown\n");
	Stop();
}

void BINKMovie::Stop()
{
	if (Playing) {
		if (Movie->FrameNum >= Movie->Frames) {
			fprintf(Out, "bink stop at the end\n");
		} else {
			fprintf(Out, "bink stop before the end\n");
			fprintf(Out, "bink frame %u of %u\n", Movie->FrameNum, Movie->Frames);
		}
		if (Movie != &Fake) {
			Bink_Close(Movie);
		}
		Playing = false;
	}
}

void BINKMovie::Play(const char *filename, const char *subtitlename, int volume)
{
	Stop();
	Movie = &Fake;
	if (Real_File != NULL && Bink_Open != NULL) {
		Bink_Set_Sound_Track(0xFFFFFFFF);					// BINKNOSOUND: no sound in a test
		BinkHead *bink = (BinkHead *)Bink_Open(Real_File, 0x00004000);	// BINKSNDTRACK
		if (bink != NULL) {
			Movie = bink;
		}
	}
	if (Movie == &Fake) {
		Fake.Width = 640;
		Fake.Height = 480;
		Fake.Frames = 5;
		Fake.FrameNum = 1;
	}
	Playing = true;
	fprintf(Out, "bink play %s (%s) volume 0x%X\n", filename, subtitlename == filename ? "same" : subtitlename, volume);
	fprintf(Out, "bink file: %ux%u, %u frames, %s\n", Movie->Width, Movie->Height, Movie->Frames,
		Movie == &Fake ? "stand-in" : "real");
}

void BINKMovie::Update()
{
	if (!Playing) {
		return;
	}
	if (Movie == &Fake) {
		Fake.FrameNum++;
	} else {
		Bink_Do_Frame(Movie);
		Bink_Next_Frame(Movie);
	}
}

void BINKMovie::Render()
{
	Rendered++;
}

bool BINKMovie::Is_Complete()
{
	if (!Playing) {
		return true;
	}
	return Movie->Frames <= Movie->FrameNum;	// as the client's 0xA162D0
}

static int Begin_Renders;
WW3DErrorType WW3D::Begin_Render(bool, bool, const Vector3 &, void (*)(void)) { Begin_Renders++; return WW3D_ERROR_OK; }
WW3DErrorType WW3D::End_Render(bool) { return WW3D_ERROR_OK; }

// ------------------------------------------------------------------------- the stand-ins

void Unknown_Debug_Print(const char *text) { fprintf(Out, "log: %s\n", text); }
void Unknown_Debug_Begin_Step(const char *text) { fprintf(Out, "step: %s\n", text); }
void Unknown_Debug_End_Step(const char *text) { fprintf(Out, "step end: %s\n", text); }
bool Unknown_5FAE70() { return false; }
bool Unknown_5FAD50() { return false; }
void Unknown_Fail_If_Report(const char *expr, const char *, int line) { fprintf(Out, "fail_if %s (line %d)\n", expr, line); }
void Unknown_Load_Assets(const std::string &ini) { fprintf(Out, "load assets %s\n", ini.c_str()); }
void Unknown_Free_Assets() { fprintf(Out, "free assets\n"); }

static LoginTaskClass *Login;
bool LoginTaskClass::Accept_Focus() { fprintf(Out, "login accepts focus\n"); return true; }
TicketClass::TicketClass(int) {}
LoginCharacterClass::LoginCharacterClass() {}
LoginCharacterClass::~LoginCharacterClass() {}
AvatarDescClass::AvatarDescClass() {}
ShipCustSettingsClass::ShipCustSettingsClass() {}
void ShipCustSettingsClass::Unknown_00(DeltaChildVisitorClass *) {}
void ShipCustPartClass::Unknown_00(DeltaChildVisitorClass *) {}
bool LoginCharacterClass::Write(LinearBufferClass *) { return true; }
bool LoginCharacterClass::Read(LinearBufferClass *) { return true; }
int LoginCharacterClass::Get_Size() { return 0; }
void LoginCharacterClass::Unknown_00(DeltaChildVisitorClass *) {}
bool AvatarDescClass::Write(LinearBufferClass *) { return true; }
bool AvatarDescClass::Write_Short(LinearBufferClass *) { return true; }
bool AvatarDescClass::Read_Short(LinearBufferClass *) { return true; }
int AvatarDescClass::Get_Short_Size() { return 0; }
bool AvatarDescClass::Read(LinearBufferClass *) { return true; }
int AvatarDescClass::Get_Size() { return 0; }
void AvatarDescClass::Unknown_00(DeltaChildVisitorClass *) {}
bool ShipCustDescClass::Write(LinearBufferClass *) { return true; }
bool ShipCustDescClass::Read(LinearBufferClass *) { return true; }
int ShipCustDescClass::Get_Size() { return 0; }
bool ShipCustDescClass::Write_Short(LinearBufferClass *) { return true; }
bool ShipCustDescClass::Read_Short(LinearBufferClass *) { return true; }
int ShipCustDescClass::Get_Short_Size() { return 0; }
void ShipCustDescClass::Unknown_00(DeltaChildVisitorClass *) {}
void ShipCustDescClass::Set_Race(int) {}
void ShipCustDescClass::Reset_Parts() {}
void ShipCustDescClass::Set_Profession(int) {}
bool LoginGalaxyClass::Write(LinearBufferClass *) { return true; }
bool LoginGalaxyClass::Read(LinearBufferClass *) { return true; }
int LoginGalaxyClass::Get_Size() { return 0; }
TicketClass::~TicketClass() {}
bool TicketClass::Write(LinearBufferClass *) { return true; }
bool TicketClass::Read(LinearBufferClass *) { return true; }
int TicketClass::Get_Size() { return 0; }
LoginTaskClass::LoginTaskClass(int mode) : Ticket(0)
{
	Login = this;
	fprintf(Out, "login task mode %d\n", mode);
}
int LoginTaskClass::Get_Interval() { return 0; }
// the rest of tsk_login.h's virtuals (not reached here)
LoginTaskClass::~LoginTaskClass() {}
bool LoginTaskClass::Yield_Focus() { return true; }
void LoginTaskClass::Run() {}
void LoginTaskClass::Pre_Run() {}
void LoginTaskClass::Render() {}
void LoginTaskClass::Unknown_48(AvatarDescClass *, ShipCustDescClass *, int, int) {}
bool LoginTaskClass::Unknown_4C() { return false; }
void LoginTaskClass::Unknown_50() {}
void LoginTaskClass::Go_Next_Step() {}
void LoginTaskClass::Go_Back_Step() {}
void LoginTaskClass::Unknown_5C() {}
void LoginTaskClass::Set_Step(int, bool) {}
bool LoginTaskClass::Unknown_64() { return false; }
void LoginTaskClass::Process_Packets() {}
void LoginTaskClass::Unknown_6C() {}
void LoginTaskClass::Create_Character_View() {}

// mclient.cpp's client: each script plays for 2 runs.
static int Clients_Made;
class MovieClientClass
{
public:
	MovieClientClass(ScriptedMovieTaskClass *task, KernelClass *kernel);
	~MovieClientClass();
	virtual void Unknown_00();
	virtual void Unknown_04();
	virtual void Unknown_08();
	virtual void Unknown_0C();
	virtual void Unknown_10();
	virtual void Unknown_14(bool focused);
	void Load_Script(const char *filename);
	void Handle_Input(KernelEventClass event);
	void Render();
	bool Is_Done();

	// the stand-in's own (the real one is 0x1178 bytes; only the task's `new` sees the size)
	int Number;
	int Runs;
	int Unfocused;
	bool Loaded;
	char Unknown[0x1178 - 4 - 16];
};

MovieClientClass::MovieClientClass(ScriptedMovieTaskClass *task, KernelClass *kernel) :
	Number(++Clients_Made), Runs(0), Unfocused(0), Loaded(false)
{
	fprintf(Out, "client %d made (task %s, kernel %s)\n", Number, task != NULL ? "ok" : "null", kernel == KernelClass::Get_Instance() ? "ok" : "other");
}
MovieClientClass::~MovieClientClass() { fprintf(Out, "client %d deleted\n", Number); }
void MovieClientClass::Unknown_00() {}
void MovieClientClass::Unknown_04() {}
void MovieClientClass::Unknown_08() {}
void MovieClientClass::Unknown_0C() {}
void MovieClientClass::Unknown_10() {}
void MovieClientClass::Unknown_14(bool focused)
{
	if (!focused) {
		Unfocused++;				// the task's Run in the background (while the intro plays)
		return;
	}
	if (Loaded) {
		Runs++;
	}
	fprintf(Out, "client %d run %d (focused)\n", Number, Runs);
}
void MovieClientClass::Load_Script(const char *filename)
{
	Loaded = true;
	fprintf(Out, "client %d loads %s (%s background runs before)\n", Number, filename, Unfocused > 0 ? "some" : "no");
}
void MovieClientClass::Handle_Input(KernelEventClass event) { fprintf(Out, "client %d key 0x%X\n", Number, event.Key); }
void MovieClientClass::Render() { fprintf(Out, "client %d render\n", Number); }
bool MovieClientClass::Is_Done() { return Runs >= 2; }

// ------------------------------------------------------------------------- the test

static const char *Focus_Name(KernelClass *kernel, TaskClass *movie, TaskClass *smovie, TaskClass *menu)
{
	TaskClass *f = kernel->Get_Focus();
	return f == NULL ? "none" : f == movie ? "movie" : f == smovie ? "smovie" : f == menu ? "menu"
		: f == Login && Login != NULL ? "login" : "other";
}

static bool Contains(KernelClass *kernel, TaskClass *task)
{
	return kernel->Contains_Task(task);
}

static void Make_Script(const char *name, bool make)
{
	if (make) {
		FILE *f = fopen(name, "w");
		if (f) {
			fprintf(f, "test script\n");
			fclose(f);
		}
	} else {
		DeleteFile(name);
	}
}

int Tasks_Test(FILE *out, const char *bik)
{
	Out = out;
	Real_File = bik;
	if (bik != NULL) {
		Bink_Dll = LoadLibrary("binkw32.dll");
		if (Bink_Dll != NULL) {
			Bink_Open = (BinkOpenProc)GetProcAddress(Bink_Dll, "_BinkOpen@8");
			Bink_Do_Frame = (BinkFrameProc)GetProcAddress(Bink_Dll, "_BinkDoFrame@4");
			Bink_Next_Frame = (BinkVoidProc)GetProcAddress(Bink_Dll, "_BinkNextFrame@4");
			Bink_Close = (BinkVoidProc)GetProcAddress(Bink_Dll, "_BinkClose@4");
			Bink_Set_Sound_Track = (BinkTrackProc)GetProcAddress(Bink_Dll, "_BinkSetSoundTrack@4");
			if (!Bink_Open || !Bink_Do_Frame || !Bink_Next_Frame || !Bink_Close || !Bink_Set_Sound_Track) {
				Bink_Open = NULL;
			}
		}
		fprintf(Out, "tasks: bink %s\n", Bink_Open != NULL ? "dll" : "missing");
	}
	Make_Script("moviescript2.txt", true);
	Make_Script("moviescript3.txt", true);
	Make_Script("moviescript4.txt", false);

	KernelClass *kernel = new KernelClass;
	kernel->Render_Enabled = true;

	// WinMain's order: the intro movie (added, focused), the scripted movie, the menu task
	MovieTaskClass *movie = new MovieTaskClass;
	movie->Add_Movie("..//data//client//mixfiles//eb_sizzle.bik", 0x8000);
	movie->Add_Movie("second.bik", 0x4000);
	movie->Add_Movie(NULL, 0);								// FAIL_IF line 87, not added
	fprintf(Out, "movies %d\n", (int)movie->Movies.size());
	kernel->Add_Task(movie);
	fprintf(Out, "set focus movie: %d\n", (int)kernel->Set_Focus(movie));
	ScriptedMovieTaskClass *smovie = new ScriptedMovieTaskClass("moviescript", 1);
	bool ok = smovie->Init(kernel);
	fprintf(Out, "smovie init: %d, clients %d\n", (int)ok, (int)smovie->Clients.size());
	kernel->Add_Task(smovie);
	MenuTaskClass *menu = new MenuTaskClass;
	kernel->Add_Task(menu);
	fprintf(Out, "focus %s\n", Focus_Name(kernel, movie, smovie, menu));
	fprintf(Out, "movie interval %d\n", movie->Get_Interval());

	// the movie plays: the focused task's Render advances it (the smovie's clients run unfocused)
	int cycles = 0;
	while (Contains(kernel, movie) && movie->Index == 0 && cycles < 100000) {
		Clock += 16;
		kernel->Run_One_Cycle();
		cycles++;
		if (cycles == 1) {
			fprintf(Out, "yield movie early: %d\n", (int)movie->Yield_Focus());
		}
	}
	fprintf(Out, "movie 1 done: index %u, %s frames rendered, renders %s\n", movie->Index, Rendered > 0 ? "some" : "no",
		Begin_Renders == Rendered ? "bracketed" : "unbracketed");
	// the second movie: a key in its first 3 s is ignored, one after 3 s skips it
	Clock += 16;
	kernel->Run_One_Cycle();									// its first Render: Start_Time
	Pending.Key = 0x20;
	Pending.Flags = 0;
	Has_Pending = true;
	Clock += 16;
	kernel->Run_One_Cycle();
	fprintf(Out, "key before 3 s: movie %s\n", Contains(kernel, movie) ? "kept" : "removed");
	Sleep(3100);
	Pending.Key = 0x20;
	Has_Pending = true;
	Clock += 16;
	kernel->Run_One_Cycle();									// skipped: Stop, then Run ends the list
	fprintf(Out, "key after 3 s: movie %s, focus %s\n", Contains(kernel, movie) ? "kept" : "removed",
		Focus_Name(kernel, movie, smovie, menu));

	// Remove_Task's Next_Focus handed the focus on: the scripted movie plays its three scripts, then
	// the menu task takes the focus and starts the login task in the same cycle
	Pending.Key = 0x41;
	Has_Pending = true;
	cycles = 0;
	while (Contains(kernel, menu) && cycles < 50) {
		Clock += 16;
		kernel->Run_One_Cycle();
		cycles++;
		if (!Contains(kernel, smovie) && smovie != NULL) {
			fprintf(Out, "smovie removed after %d cycles\n", cycles);
			smovie = NULL;
		}
	}
	fprintf(Out, "menu %s, login %s, focus %s\n", Contains(kernel, menu) ? "kept" : "removed",
		Login != NULL && Contains(kernel, Login) ? "added" : "missing", Focus_Name(kernel, movie, smovie, menu));

	// MenuTask's other key: 0x51 stops the kernel
	MenuTaskClass *menu2 = new MenuTaskClass;
	kernel->Add_Task(menu2);
	KernelEventClass quit;
	quit.Key = 0x51;
	menu2->Handle_Input(&quit);
	fprintf(Out, "running after 0x51: %d\n", (int)kernel->Is_Running());
	kernel->Remove_Task(menu2);
	kernel->Shutdown();
	delete kernel;
	if (Bink_Dll != NULL) {
		FreeLibrary(Bink_Dll);
	}
	fprintf(Out, "tasks done\n");
	return 0;
}
