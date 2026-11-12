// Earth & Beyond client (reconstruction): app\client\meshsnd.cpp, the sound on a model's bone (MeshSoundClass) and the
// sounds of one animation (MeshSoundListClass), 0x7ED150-0x7EDCA0. The assert sits on the client's line. Notes at the
// end of the file.
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "sclient.h"
#include "mv3dview.h"
#include "nstrdup.h"
#include "_mono.h"
#include "Sound3D.h"
#include "..\audio\soundlib.h"
#include "..\shared\g_assert.h"
#include "..\..\ww3d2\rendobj.h"

// 0x7ED150: a line of the sound's debug text on the mono screen at (x, y), when on
static void Mono_Printf(bool on, int x, int y, const char *format, ...)	// NAME 1: what it does
{
	if (on) {
		char buffer[256];
		va_list args;
		va_start(args, format);
		vsprintf(buffer, format, args);
		buffer[255] = '\0';
		Mono.Text_Print(buffer, x, y, MonoClass::NORMAL);
	}
}















































// 0x7ED1D0
MeshSoundClass::MeshSoundClass(const char *name, const char *bone_name) :
	Node(this),
	StartFrame(0),
	EndFrame(-1),
	Loop(false),
	PitchSpeed(-1.0f),
	PitchChange(1.0f),
	PlaybackRate(0),
	Volume(-1.0f),
	Priority(-1.0f),
	DropOffRadius(-1.0f),
	MaxVolRadius(-1.0f),
	BoneName(NULL),
	Model(NULL),
	BoneIndex(-1),
	Playing(false),
	Mono(false),
	Sound(NULL)
{
	if (bone_name != NULL && *bone_name != '\0') {
		BoneName = nstrdup(bone_name);
	}
	Name = nstrdup(name);
}

// 0x7ED300
MeshSoundClass::~MeshSoundClass()
{
	if (Sound != NULL) {
		Sound->Stop(true);
		Sound->Remove_From_Scene();
		Sound->Release_Ref();
		Sound = NULL;
	}
	if (Model != NULL) {
		Model->Release_Ref();
		Model = NULL;
	}
	if (BoneName != NULL) {
		delete BoneName;
	}
	delete Name;
}

// 0x7ED410
void MeshSoundClass::Unknown_7ED410(RenderObjClass *mesh)
{
	G_ASSERT(mesh);
	if (Model != mesh) {
		if (Model != NULL) {
			Model->Release_Ref();
		}
		Model = mesh;
		Model->Add_Ref();
	}
	if (Sound == NULL) {
		if (BoneName != NULL) {
			BoneIndex = Model->Get_Bone_Index(BoneName);
			Matrix3D tm = Model->Get_Bone_Transform(BoneIndex);
			Sound = SoundLibraryClass::Play_3D_Sound(Name, tm, 0, 1.0f);
		} else {
			Sound = (Sound3DClass *)SoundLibraryClass::Play_2D_Sound(Name, 0);
		}
	}
	if (Sound != NULL) {
		Sound->Set_Loop_Count(!Loop);
		if (Priority >= 0.0f) {
			Sound->Set_Priority(Priority);
		}
		if (Volume >= 0.0f) {
			Sound->Set_Volume(Volume);
		}
		if (DropOffRadius >= 0.0f) {
			Sound->Set_DropOff_Radius(DropOffRadius);
		}
		if (MaxVolRadius >= 0.0f) {
			Sound->Set_Max_Vol_Radius(MaxVolRadius);
		}
	}
}

// 0x7ED630
void MeshSoundClass::Unknown_7ED630(bool release)
{
	if (Sound != NULL) {
		Sound->Stop(true);
		Sound->Remove_From_Scene();
		if (release) {
			Sound->Release_Ref();
			Sound = NULL;
		}
	}
	if (release) {
		if (Model != NULL) {
			Model->Release_Ref();
			Model = NULL;
		}
	}
}

// NOT EXACT: functional (M1): appfile.py's diff shows no instruction apart, only the four float-literal rows (-1.0 / 0.0f); verify does not pair it (attempts.py show 0x7ED6C0)
// 0x7ED6C0
void MeshSoundClass::Unknown_7ED6C0(int frame, unsigned int ms)
{
	if (Sound != NULL) {
		Mono_Printf(Mono, 45, 17, " %s:%s ", Name, BoneName ? BoneName : "nobone");
		Sound->Is_Playing();
		if (frame >= StartFrame && (frame <= EndFrame || EndFrame == -1.0)) {
			if (!Playing) {
				if (BoneIndex >= 0) {
					Sound->Add_To_Scene(true);
					Playing = true;
				} else {
					Sound->Play(true);
					Playing = true;
				}
			}
		} else if (EndFrame != -1.0 && Playing) {
			Sound->Stop(true);
			if (Loop) {
				Sound->Remove_From_Scene();
			}
			Playing = false;
		}
		Mono_Printf(Mono, 45, 18, " %s(%d<%d<%d)", "off", StartFrame, frame, EndFrame);
		if (BoneName != NULL) {
			Vector3 from = Sound->Get_Position();
			Matrix3D tm = Model->Get_Bone_Transform(BoneIndex);
			Sound->Set_Transform(tm);
			Vector3 to = tm.Get_Translation();
			Mono_Printf(Mono, 45, 19, " at(%1.1f,%1.1f,%1.1f->%1.1f,%1.1f,%1.1f)", from.X, from.Y, from.Z, to.X, to.Y, to.Z);
			if (PitchSpeed > 0.0f) {
				if (PlaybackRate == 0.0f) {
					PlaybackRate = Sound->Get_Playback_Rate();
				}
				if (PlaybackRate != 0) {
					float speed = Vector3::Distance(to, from);
					if (ms != 0) {
						speed = speed / ms;
					}
					float change = speed - PitchSpeed;
					int rate = change * PitchChange + PlaybackRate;
					Sound->Set_Playback_Rate(rate);
					Mono_Printf(Mono, 45, 20, " vel(%1.1f->%1.1f)bpr(%d->%d)", speed, change, PlaybackRate, rate);
				}
			}
		}
	} else {
		Mono_Printf(Mono, 45, 17, "%s:%s:Off", Name, BoneName ? BoneName : "nobone");
	}
}

// 0x7EDA60
bool MeshSoundClass::Unknown_7EDA60(const char *name)
{
	return(strcmp(name, Name) == 0 ? true : false);
}

// 0x7EDAD0
void MeshSoundListClass::Unknown_7EDAD0(int frame, RenderObjClass *mesh)
{
	for (DataNode<MeshSoundClass *> *node = Sounds.First_Valid(); node != NULL; node = node->Next_Valid()) {
		node->Get()->Unknown_7ED410(mesh);
	}
	Started = true;
	Frame = frame;
}

// 0x7EDB60
void MeshSoundListClass::Unknown_7EDB60()
{
	if (Started) {
		DataNode<MeshSoundClass *> *next;
		for (DataNode<MeshSoundClass *> *node = Sounds.First_Valid(); node != NULL; node = next) {
			MeshSoundClass *sound = node->Get();
			next = node->Next_Valid();
			delete sound;
		}
	}
	Started = false;
	Frame = 0;
}

// 0x7EDBE0
void MeshSoundListClass::Unknown_7EDBE0(unsigned int ms)
{
	if (Started) {
		for (DataNode<MeshSoundClass *> *node = Sounds.First_Valid(); node != NULL; node = node->Next_Valid()) {
			node->Get()->Unknown_7ED6C0(Frame, ms);
		}
	}
}

/*
**	Notes (M1, run 2026-11-11b)
**
**	The object runs 0x7ED150-0x7EDCA0: 0x7ED2B0 is GenericNode's destructor and 0x7EDC50 DataNode<MeshSoundClass *>'s
**	deleting destructor (table 0xB07968), both copies the constructor's node member brings. A 2-D sound
**	(Play_2D_Sound's AudibleSoundClass) is kept in the Sound3DClass pointer as the client does: 0x7ED410 calls
**	Set_Max_Vol_Radius (+0x198) on either.
*/
