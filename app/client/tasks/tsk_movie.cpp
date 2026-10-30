// Earth & Beyond client (reconstruction)
/*
**	Original location: e:\build\beta\G\app\Client\tasks\tsk_movie.cpp (client 0x76BF40-0x76C660)
**	The movie task (tsk_movie.h). The assert sits on the client's line (87: D25). Notes at the end.
*/
#include "tsk_movie.h"
#include "..\..\shared\unknown_debug.h"
#include "ww3d.h"
#include "vector3.h"
#include <windows.h>
#include <mmsystem.h>

// Renegade's BINKMovie (Code\BinkMovie\BINKMovie.h, client 0xA161B0-0xA162D0), E&B's version: Play
// takes a volume (BinkSetVolume) where Renegade's takes a subtitle font. Declared here only.
class BINKMovie
{
public:
	static void Play(const char *filename, const char *subtitlename, int volume);	// 0xA161B0  // NAME 2 [volume]: BinkSetVolume's argument
	static void Stop();					// 0xA16220
	static void Update();				// 0xA16250
	static void Render();				// 0xA16280
	static void Init();					// 0xA16290
	static void Shutdown();				// 0xA162A0
	static bool Is_Complete();			// 0xA162D0
};
bool Unknown_42EA80(KernelEventClass *event);	// NAME 1 [Unknown_42EA80]: the input queue's next event (input.cpp)
void Unknown_42E8A0(KernelEventClass *event);	// NAME 1 [Unknown_42E8A0]: hands the event on (input.cpp)

// 0x76BF40
MovieTaskClass::MovieTaskClass() :
	Index(0),
	Start_Time(0),
	Starting(true)
{
	BINKMovie::Init();
}

// 0x76C030
MovieTaskClass::~MovieTaskClass()
{
	BINKMovie::Shutdown();
}

// 0x76C140
bool MovieTaskClass::Accept_Focus()
{
	if (Index <= Movies.size()) {
		Starting = true;
		BINKMovie::Play(Movies[Index].c_str(), Movies[Index].c_str(), Volumes[Index]);
	}
	return true;
}

// 0x76C1C0
bool MovieTaskClass::Yield_Focus()
{
	return BINKMovie::Is_Complete() && Index == Movies.size();
}

// 0x76C200
int MovieTaskClass::Get_Interval()
{
	if (Kernel != NULL && Kernel->Render_Enabled && Kernel->Device_Ok) {
		return 0;
	}
	return 500;
}

// 0x76C230
void MovieTaskClass::Render()
{
	if (!BINKMovie::Is_Complete() && Is_Focused()) {
		if (Starting) {
			Start_Time = timeGetTime();
			Starting = false;
		}
		BINKMovie::Update();
		WW3D::Begin_Render(true, true, Vector3(0, 0, 0));
		BINKMovie::Render();
		WW3D::End_Render(true);
	}
}

// 0x76C2C0
void MovieTaskClass::Add_Movie(const char *movie_name, int volume)
{
	FAIL_IF(!movie_name) {
		return;
	}
	std::string name(movie_name);
	Movies.push_back(name);
	Volumes.push_back(volume);
}

// 0x76C430
void MovieTaskClass::Run()
{
	if (BINKMovie::Is_Complete()) {
		if (++Index >= Movies.size()) {
			if (Kernel != NULL) {
				Kernel->Remove_Task(this);
			}
		} else {
			BINKMovie::Stop();
			Starting = true;
			BINKMovie::Play(Movies[Index].c_str(), Movies[Index].c_str(), Volumes[Index]);
		}
	}
}

// 0x76C4D0
void MovieTaskClass::Pre_Run()
{
	KernelEventClass event;
	while (Unknown_42EA80(&event)) {
		Unknown_42E8A0(&event);
		if (event.Key != 0) {
			Handle_Input(&event);
		}
	}
}

// 0x76C5A0
void MovieTaskClass::Handle_Input(KernelEventClass *event)
{
	if ((event->Flags & 0x1000) == 0 && !Starting && timeGetTime() - Start_Time >= 3000) {
		BINKMovie::Stop();
		if (Kernel != NULL) {
			Kernel->Remove_Task(this);
		}
	}
}

/*
**	Reconstruction notes (client.exe 0x76BF40-0x76C660; vtable 0xB04280, slots in tsk_movie.h).
**	Accept_Focus starts the movie at Index when Index <= Movies.size() (the client's test: one past
**	the end passes too). Both strings Play gets are the same movie's c_str(). Pre_Run's input loop
**	inlines Handle_Input (0x76C5A0, which has no caller of its own). The two static initialisers
**	after Handle_Input (0x76C5F0: a guarded static with atexit 0x42B320, which every tasks object
**	has; 0x76C620: a permstr.h PermStringInitClass at 0xBE82D8) come from headers the client's file
**	includes and aren't rebuilt.
*/
