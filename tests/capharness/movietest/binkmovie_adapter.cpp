// The movie player under the movie task (movietest): Renegade's BINKMovie (Code\BinkMovie, EA's
// GPL source, staged as binkmovie_renegade.inl by tools/harness/movietest.py; not in the tree) renamed to
// RenegadeBINKMovie, with binkw32.dll loaded at run time, and E&B's BINKMovie interface on top
// (tsk_movie.cpp's declaration: Play takes a volume where Renegade's takes a subtitle font).
// No sound: every movie is opened without a sound track. Screenshots come from the program's own
// back buffer after each movie frame is drawn (Movie_Frame_Drawn, movietest.cpp).
#include "bink.h"
#include <windows.h>

// ------------------------------------------------------------------ binkw32.dll at run time

typedef HBINK (__stdcall *OpenProc)(const char *, u32);
typedef void (__stdcall *CloseProc)(HBINK);
typedef s32 (__stdcall *WaitProc)(HBINK);
typedef s32 (__stdcall *DoFrameProc)(HBINK);
typedef void (__stdcall *NextFrameProc)(HBINK);
typedef s32 (__stdcall *CopyProc)(HBINK, void *, s32, u32, u32, u32, u32);
typedef void (__stdcall *TrackProc)(u32);
static HMODULE Dll;
static OpenProc P_Open;
static CloseProc P_Close;
static WaitProc P_Wait;
static DoFrameProc P_Do_Frame;
static NextFrameProc P_Next_Frame;
static CopyProc P_Copy;
static TrackProc P_Track;

bool Bink_Load(void)
{
	Dll = LoadLibraryA("binkw32.dll");
	if (Dll == NULL) {
		return false;
	}
	P_Open = (OpenProc)GetProcAddress(Dll, "_BinkOpen@8");
	P_Close = (CloseProc)GetProcAddress(Dll, "_BinkClose@4");
	P_Wait = (WaitProc)GetProcAddress(Dll, "_BinkWait@4");
	P_Do_Frame = (DoFrameProc)GetProcAddress(Dll, "_BinkDoFrame@4");
	P_Next_Frame = (NextFrameProc)GetProcAddress(Dll, "_BinkNextFrame@4");
	P_Copy = (CopyProc)GetProcAddress(Dll, "_BinkCopyToBuffer@28");
	P_Track = (TrackProc)GetProcAddress(Dll, "_BinkSetSoundTrack@4");
	return P_Open && P_Close && P_Wait && P_Do_Frame && P_Next_Frame && P_Copy && P_Track;
}

static HBINK Last_Bink;					// the open movie (the player's own handle is private)
static HBINK __stdcall Dyn_BinkOpen(const char *name, u32 flags)
{
	P_Track(BINKNOSOUND);
	Last_Bink = P_Open(name, flags | BINKSNDTRACK);
	return Last_Bink;
}
static void __stdcall Dyn_BinkClose(HBINK b)
{
	if (b == Last_Bink) {
		Last_Bink = NULL;
	}
	P_Close(b);
}
static s32 __stdcall Dyn_BinkWait(HBINK b) { return P_Wait(b); }
static s32 __stdcall Dyn_BinkDoFrame(HBINK b) { return P_Do_Frame(b); }
static void __stdcall Dyn_BinkNextFrame(HBINK b) { P_Next_Frame(b); }
static s32 __stdcall Dyn_BinkCopyToBuffer(HBINK b, void *d, s32 p, u32 h, u32 x, u32 y, u32 f) { return P_Copy(b, d, p, h, x, y, f); }
static s32 __stdcall Dyn_BinkSetSoundSystem(BINKSNDSYSOPEN, u32) { return 0; }

#define BinkOpen Dyn_BinkOpen
#define BinkClose Dyn_BinkClose
#define BinkWait Dyn_BinkWait
#define BinkDoFrame Dyn_BinkDoFrame
#define BinkNextFrame Dyn_BinkNextFrame
#define BinkCopyToBuffer Dyn_BinkCopyToBuffer
#define BinkSetSoundSystem Dyn_BinkSetSoundSystem
#undef BinkSoundUseDirectSound
#define BinkSoundUseDirectSound(x) Dyn_BinkSetSoundSystem(0, 0)

// ------------------------------------------------------------------ Renegade's player, renamed

#define BINKMovie RenegadeBINKMovie
#include "binkmovie_renegade.inl"
#undef BINKMovie

// ------------------------------------------------------------------ E&B's interface (tsk_movie.cpp)

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

void Movie_Log(const char *format, ...);
void Movie_Frame_Drawn(void);

void BINKMovie::Play(const char *filename, const char *, int volume)
{
	RenegadeBINKMovie::Play(filename, NULL, NULL);
	Movie_Log("BINKMovie::Play(%s, volume 0x%X): %s\n", filename, volume, Last_Bink ? "open" : "FAILED");
	if (Last_Bink) {
		Movie_Log("  %ux%u, %u frames, %u/%u fps\n", Last_Bink->Width, Last_Bink->Height, Last_Bink->Frames,
			Last_Bink->FrameRate, Last_Bink->FrameRateDiv);
	}
}
void BINKMovie::Stop() { RenegadeBINKMovie::Stop(); }
void BINKMovie::Update() { RenegadeBINKMovie::Update(); }
void BINKMovie::Render()
{
	RenegadeBINKMovie::Render();
	Movie_Frame_Drawn();
}
void BINKMovie::Init() { Movie_Log("BINKMovie::Init (no sound system: the test plays no sound)\n"); }
void BINKMovie::Shutdown() { RenegadeBINKMovie::Shutdown(); }
bool BINKMovie::Is_Complete() { return RenegadeBINKMovie::Is_Complete(); }

unsigned Movie_Frame_Number(void)
{
	return Last_Bink ? Last_Bink->FrameNum : 0;
}
