// Earth & Beyond client (reconstruction): app\client\mclient.h, the movie client (the client's app\Client\mclient.cpp per
// its assert): the ClientClass that plays a movie script for tsk_smovie.cpp's task (0x1178 bytes, vtable 0xB07864).
#ifndef MCLIENT_H  // NAME 1 [mclient.h]: header name assumed from the file
#define MCLIENT_H
#include "client.h"
#include "..\kernel\kernel.h"
class ScriptedMovieTaskClass;
class ScriptedMovieViewClass;
class CameraClass;
class MovieClientClass : public ClientClass	// NAME 2: mclient.cpp's class, the movie script's player
{
public:
	MovieClientClass(ScriptedMovieTaskClass *task, KernelClass *kernel);	// 0x7EC3E0 (G_ASSERT 'RSLRegistry' l.83; -CREATEMOVIE: a screen shot of every frame)  // NAME 1 [task, kernel]: what tsk_smovie.cpp passes
	~MovieClientClass();										// 0x7EC570 (not virtual: delete calls it directly)
	virtual void Init();										// +0x00 0x7EC780: empty
	virtual void Shutdown();									// +0x04 0x7EC7A0: empty
	virtual void Unknown_08();									// +0x08 0x7EC7C0: every object's frame, the effects that are done removed (as SClient's)
	virtual void Process_Packets();								// +0x0C 0x7EC970: empty
	virtual void Unknown_10();									// +0x10 0x7EC990: the window messages (as SClient's)
	virtual int Do_Frame(bool focused);							// +0x14 0x7ECAA0: the clock (paused, or 30 frames a second for -CREATEMOVIE), the view's frame  // NAME 1 [focused]: the task's Is_Focused()
	virtual int Unknown_1C() { return((int)this); }				// +0x1C 0x7EC550
	void Handle_Input(KernelEventClass event);					// 0x7ECA50: the view's input  // NAME 2: Pre_Run's events
	void Load_Script(const char *filename);						// 0x7ECC80: the scripted movie view of the script  // NAME 2: Accept_Focus's '%s%d.txt'
	void Render();												// 0x7ECDC0: the view, then the listener on the camera  // NAME 2: called from Render
	bool Is_Done();												// 0x7ED090  // NAME 2: Run deletes the client then
	void Set_Camera(CameraClass *camera);						// 0x7ED0C0 (no caller; the view's interface inlines it)  // NAME 2: what it does
	void Unknown_7ED100();										// 0x7ED100 (no caller): the view's END_MOVIE (0x806AD0)

	ScriptedMovieViewClass *View;								// +0x1150  // NAME 1: what it holds
	ScriptedMovieTaskClass *Task;								// +0x1154  // NAME 1: the constructor's
	int Unknown1158;											// +0x1158: frames Render draws nothing (Load_Script clears it)
	CameraClass *Camera;										// +0x115C (referenced): the listener's, else the view's  // NAME 2: Set_Camera's
	bool Synced;												// +0x1160: WW3D synced to the first frame  // NAME 1: what Do_Frame does with it
	bool Paused;												// +0x1161 (the view's +0x168, the space key)  // NAME 1: what Do_Frame does with it
	unsigned int PauseStart;									// +0x1164  // NAME 1: what Do_Frame keeps in it
	unsigned int PausedTime;									// +0x1168  // NAME 1: what Do_Frame keeps in it
	bool CreateMovie;											// +0x116C  // NAME 2: the command line's '-CREATEMOVIE'
	int MovieFrame;												// +0x1170: 0..2 (33, 33, 34 ms)  // NAME 1: what Do_Frame counts
	char *MovieName;											// +0x1174: the screen shots' name ('-CREATEMOVIE' value, else 'Frames')  // NAME 1: what Make_Screen_Shot gets
};

#endif
