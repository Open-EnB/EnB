// Earth & Beyond client (reconstruction): app\client\mclient.cpp, the movie client (MovieClientClass, 0x7EC3E0-0x7ED140):
// the ClientClass tsk_smovie.cpp's task plays a movie script with. The assert sits on the client's line. Notes at the
// end of the file.
#include <windows.h>
#include "eff_o.h"							// before client.h, which declares EffectObjectClass only as far as the lists need it
#include "sclient.h"
#include "mclient.h"
#include "argv.h"
#include "nstrdup.h"
#include "thread.h"
#include "textureloader.h"
#include "dx8wrapper.h"
#include "sortingrenderer.h"
#include "camera.h"
#include "ww3d.h"
#include "WWAudio.h"
#include "sound_manager.h"
#include "..\audio\soundlib.h"
#include "..\shared\g_assert.h"

extern HWND Unknown_Main_Window;						// 0xBDAA7C (input.cpp's neighbourhood)  // NAME 2: what Unknown_10 redraws
extern SoundManagerClass *SoundManager;					// 0xBE82A4  // NAME 2: what it holds
unsigned int Unknown_Get_Ticks();						// 0x4A3B40, the aux-data clock (unknown_root.h)

// The scripted movie view's link back to the movie client (vtable 0xB078D4, 8 bytes; Load_Script makes it), as SClient's
// SClientMovieCallbackClass (sclient.h) is for SClient
class MovieClientCallbackClass : public ScriptedMovieInterfaceClass	// NAME 1: after SClientMovieCallbackClass
{
public:
	MovieClientCallbackClass(MovieClientClass *client) { Client = client; }
	virtual ClientClass *Unknown_00() { return(Client); }			// +0x00 0x7ECD60
	virtual void Unknown_04(CameraClass *camera) { Client->Set_Camera(camera); }	// +0x04 0x7ECD80

	MovieClientClass *Client;									// +0x04  // NAME 2: what it holds
};
































// 0x7EC3E0
MovieClientClass::MovieClientClass(ScriptedMovieTaskClass *task, KernelClass *kernel) :
	View(NULL),
	Task(task),
	Unknown1158(0),
	Camera(NULL),
	Synced(false),
	Paused(false),
	PauseStart(0),
	PausedTime(0),
	CreateMovie(false),
	MovieFrame(0),
	MovieName(NULL)
{
	Kernel = kernel;
	G_ASSERT(RSLRegistry);
	RSLRegistry->Unknown_763E70("rs.ini");
	ArgvClass args;
	if (args.Find("-CREATEMOVIE")) {
		const char *name = args.Find_Value("-CREATEMOVIE");
		if (name) {
			MovieName = nstrdup(name);
		} else {
			MovieName = nstrdup("Frames");
		}
		CreateMovie = true;
	}
}

// 0x7EC570
MovieClientClass::~MovieClientClass()
{
	if (View) {
		View->Unknown_10();
		delete View;
		View = NULL;
	}

	// the effects left: out of the list, then eff_o.cpp's 0x6E6A90 for each
	HashNodeClass<EffectObjectClass *> *node = EffectList.First_Valid();
	while (node) {
		HashNodeClass<EffectObjectClass *> *next = node->Next_Valid();
		EffectList.Remove(node);
		Unknown_6E6A90(node->Get_Record());		// (the client reads the record after Remove, which may have deleted the node)

		node = next;
	}

	RSLRegistry->Unknown_763FF0();
	if (MovieName) {
		delete MovieName;
		MovieName = NULL;
	}
}

// 0x7EC780
void MovieClientClass::Init()
{
}

// 0x7EC7A0
void MovieClientClass::Shutdown()
{
}

// NOT EXACT: functional (M1): register allocation, the client keeps the next effect node in ebx where this keeps the flag constant 1 (two forms; attempts.py show 0x7EC7C0)
// 0x7EC7C0
void MovieClientClass::Unknown_08()
{
	// every object's per-frame update (co.cpp's slot +0x20 0x6F3C60)
	HashNodeClass<ClientObjectClass *> *node = ObjectList.First_Valid();
	while (node && node->Next()) {
		node->Get_Record()->Unknown_20();
		node = node->Next();
	}

	// the effects that are done: out of the list, then eff_o.cpp's 0x6E6A90
	HashNodeClass<EffectObjectClass *> *enode = EffectList.First_Valid();
	while (enode) {
		HashNodeClass<EffectObjectClass *> *next = enode->Next(); if (!next) break; EffectObjectClass *effect = enode->Get_Record();

		if (!effect->Unknown_6E6AD0()) {
			EffectList.Remove(enode);
			Unknown_6E6A90(effect);
		}
		enode = next;
	}
}

// 0x7EC970
void MovieClientClass::Process_Packets()
{
}

// 0x7EC990
void MovieClientClass::Unknown_10()
{
	MSG msg;
	while (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE)) {
		if (GetMessage(&msg, NULL, 0, 0)) {
			TranslateMessage(&msg);
			if (msg.message == WM_PAINT) {
				RedrawWindow(Unknown_Main_Window, NULL, NULL, RDW_INVALIDATE | RDW_ERASE);
			}
			DispatchMessage(&msg);
		}
	}
}

// 0x7ECA50
void MovieClientClass::Handle_Input(KernelEventClass event)
{
	if (View) {
		View->Unknown_802960(event);
	}
}

// 0x7ECAA0
int MovieClientClass::Do_Frame(bool focused)
{
	if (View == NULL) {
		return(0);
	}
	if (!focused) {
		return(0);
	}
	SoundManager->Unknown_75B320();
	if (!View->Unknown172) {
		View->Unknown_14();
		return(0);
	}
	unsigned int time = Unknown_Get_Ticks();
	if (!Synced) {
		Unknown10BC = time;
		WW3D::Sync(time);
		Synced = true;
	}

	if (View->Unknown168 && !Paused) {
		PauseStart = time;
		Paused = true;
	} else if (!View->Unknown168 && Paused) {
		Paused = false;
		PausedTime += time - PauseStart;
	}
	if (Paused) {
		WW3D::Sync(Unknown10BC);
	} else if (CreateMovie) {
		if (++MovieFrame == 3) {
			Unknown10BC += 34;
			MovieFrame = 0;
		} else {
			Unknown10BC += 33;
		}
		WW3D::Sync(Unknown10BC);
		WW3D::Make_Screen_Shot(MovieName, 1.3f, 0);
	} else {
		Unknown10BC = View->Unknown164 - PausedTime + time;
		WW3D::Sync(Unknown10BC);
	}
	View->Unknown_14();
	Unknown_08();
	return(0);
}

// 0x7ECC80
void MovieClientClass::Load_Script(const char *filename)
{
	View = new ScriptedMovieViewClass((ClientTaskClass *)Task, (int)filename, (SClientMovieCallbackClass *)new MovieClientCallbackClass(this));
	View->Unknown_18(true);
	Unknown1158 = 0;
	WWAudioClass::Get_Instance()->Set_Sound_Effects_Volume(1.0f);
}

// NOT EXACT: functional (M1): register and stack allocation, the client holds this in esi and lays the matrix over the saved flags (two forms; attempts.py show 0x7ECDC0)
// 0x7ECDC0
void MovieClientClass::Render()
{
	ThreadClass::Switch_Thread();
	if (View->Unknown172) {
		bool triangles = DX8Wrapper::_Is_Triangle_Draw_Enabled();
		bool sorted_triangles = SortingRendererClass::_Is_Triangle_Draw_Enabled();
		bool on = true;
		if (Unknown1158) {
			DX8Wrapper::_Enable_Triangle_Draw(false);
			SortingRendererClass::_Enable_Triangle_Draw(false);
			on = false;
		}
		View->Unknown_0C(on);
		if (Unknown1158) {
			Unknown1158--;
			DX8Wrapper::_Enable_Triangle_Draw(triangles);
			SortingRendererClass::_Enable_Triangle_Draw(sorted_triangles);
			TextureLoader::Flush_Pending_Load_Tasks();
		}
		RenderObjClass *camera = Camera;
		if (camera == NULL) {
			camera = (RenderObjClass *)View->Unknown_54();
		}
		if (camera != NULL) {
			Matrix3D tm = camera->Get_Transform();
			if (camera->Class_ID() == RenderObjClass::CLASSID_CAMERA) {
				tm.Rotate_X(-1.5707964f);
				tm.Rotate_Z(1.5707964f);
			}
			SoundLibraryClass::Set_Listener_Transform(tm);
			WWAudioClass::Get_Instance()->On_Frame_Update(0);
		}
	}
}

// 0x7ED090
bool MovieClientClass::Is_Done()
{
	if (View == NULL) {
		return(false);
	}
	return(View->Unknown_802930() ? true : false);
}

// 0x7ED0C0
void MovieClientClass::Set_Camera(CameraClass *camera)
{
	if (Camera) {
		Camera->Release_Ref();
	}
	Camera = camera;
	if (Camera) {
		Camera->Add_Ref();
	}
}

// 0x7ED100
void MovieClientClass::Unknown_7ED100()
{
	if (View) {
		View->Unknown_806AD0();
	}
}

/*
**	Notes (M1, run 2026-11-11b)
**
**	The object runs 0x7EC3E0-0x7ED140 (meshsnd.cpp's starts at 0x7ED150). MovieClientCallbackClass's two slots are its
**	inline members' copies (0x7ECD60 / 0x7ECD80, after Load_Script, which makes the vtable); 0x7EC550 is the inline
**	+0x1C's copy after the constructor. Render saves and clears DX8Wrapper's and SortingRendererClass's
**	_EnableTriangleDraw (0xBB9C40 / 0xBBB5D0) for the frames Unknown1158 counts, as SClient::Unknown_64 does.
*/
