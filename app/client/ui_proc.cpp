// Earth & Beyond client (reconstruction): app\client\ui_proc.cpp, the mesh's processes (UIProcClass and the animation
// process, 0x5DCCE0-0x5DD960; the asserts sit on the client's lines). Notes at the end of the file (UP1, 2026-11-07).
#include <stdio.h>
#include <math.h>
#include "ui_mesh.h"
#include "argv.h"
#include "wwmath.h"
#include "..\shared\g_assert.h"
#include "..\..\ww3d2\hanim.h"
#include "..\..\ww3d2\assetmgr.h"

bool Unknown_42EC60(int button, int arg);				// 0x42EC60 (a key down: input.cpp's)  // NAME 1 [button, arg]: neutral

static bool FastAnim;									// 0xBE5D80: '-FASTANIM' on the command line  // NAME 1: after the switch

// @0x5DCCE0
void UIProcClass::Unknown_5DCCE0(int event, int arg)
{
	UIProcEventStruct info;
	info.Proc = this;
	info.Event = event;
	info.Arg = arg;
	UIMesh->Unknown_49EAB0(&info);
}

// @0x5DCD20
UIProc5DCD20Class::UIProc5DCD20Class(UIMeshClass *uimesh, const char *anim_name, bool cycle, bool delete_self, bool reverse, int start_frame, int end_frame) :
	UIProcClass(uimesh),
	Cycle(cycle),
	DeleteSelf(delete_self),
	Unknown26(reverse),
	AnimComboData(NULL),
	StartFrame(start_frame),
	EndFrame(end_frame)
{
	FAIL_IF(!uimesh) return;
	FAIL_IF(!anim_name) return;

	HAnimClass *anim = WW3DAssetManager::Get_Instance()->Get_HAnim(anim_name);
	if (anim == NULL) {
		char filename[256];
		sprintf(filename, "%s.w3d", anim_name);
		WW3DAssetManager::Get_Instance()->Load_3D_Assets(filename);
		WW3DAssetManager::Get_Instance()->Get_HAnim(anim_name);
	}






	FAIL_IF(!anim) return;
	Unknown_5DD030(anim);
}

// @0x5DCF00
UIProc5DCD20Class::UIProc5DCD20Class(UIMeshClass *uimesh, HAnimClass *morph_anim, bool cycle, bool delete_self, bool reverse, int start_frame, int end_frame) :
	UIProcClass(uimesh),
	Cycle(cycle),
	DeleteSelf(delete_self),
	Unknown26(reverse),
	AnimComboData(NULL),
	StartFrame(start_frame),
	EndFrame(end_frame)
{
	FAIL_IF(!uimesh) return;
	FAIL_IF(!morph_anim) return;
	Unknown_5DD030(morph_anim);
}




// @0x5DD030
void UIProc5DCD20Class::Unknown_5DD030(HAnimClass *anim)
{
	static bool first = true;
	if (first) {
		ArgvClass args;
		if (args.Find("-FASTANIM")) {
			FastAnim = true;
		}
		first = false;
	}
	FAIL_IF(Cycle && DeleteSelf) {
		Cycle = false;
	}
	AnimComboData = new HAnimComboDataClass(true);
	FAIL_IF(!AnimComboData) return;
	AnimComboData->Give_HAnim(anim);
	AnimComboData->Build_Active_Pivot_Map();
	StartFrame = WWMath::Clamp(StartFrame, 0.0f, anim->Get_Num_Frames() - 1);
	if (EndFrame == -1) {
		EndFrame = anim->Get_Num_Frames() - 1;
	} else {
		EndFrame = WWMath::Clamp(EndFrame, 0.0f, anim->Get_Num_Frames() - 1);
	}







	FAIL_IF(StartFrame > EndFrame) {
		int frame = StartFrame;
		StartFrame = EndFrame;
		EndFrame = frame;
	}
	if (Unknown26) {
		AnimComboData->Set_Frame(EndFrame);
	} else {
		AnimComboData->Set_Frame(StartFrame);
	}
}

// @0x5DD2E0
UIProc5DCD20Class::~UIProc5DCD20Class()
{
	if (AnimComboData) {
		delete AnimComboData;
		AnimComboData = NULL;
	}
}

// NOT EXACT: 0x5DD3C0 (0.81): block order, the cycle wrap's EndFrame arm sits after the return in the client (attempts.csv)
// @0x5DD3C0
bool UIProc5DCD20Class::Unknown_08(float time)
{
	FAIL_IF(!AnimComboData) return(false);
	HAnimClass *anim = AnimComboData->Peek_HAnim();



	FAIL_IF(!anim) return(false);
	float rate = anim->Get_Frame_Rate();
	float frame = AnimComboData->Get_Frame();
	if (FastAnim && Unknown_42EC60(0x10, 0) && Unknown_42EC60(0x11, 0) && Unknown_42EC60(0x12, 0)) {
		rate *= 4.0;
	}
	float step;
	if ((UIMesh->Unknown70 & 2) == 2) {
		step = 0.0f;
	} else if ((UIMesh->Unknown70 & 1) != 1) {
		step = rate * time;
	} else {
		step = 1.0f;
	}
	float next;
	bool done;
	if (Unknown26) {
		next = frame - step;
		done = next < StartFrame;
	} else {
		next = frame + step;
		done = next > EndFrame;
	}
	bool freed = false;
	if (done) {
		if (Slot != -1) {
			UIMesh->Unknown5C[Slot] = false;
			Slot = -1;
			freed = true;
		}
		if (Cycle) {
			float start = StartFrame;
			float rest = (float)fmod(next - start, EndFrame - StartFrame);
			next = ((rest >= 0.0f) ? start : EndFrame) + rest;




		} else if (Unknown26) {
			if (next < StartFrame) {
				next = StartFrame;
			}
		} else {
			if (next > EndFrame) {
				next = EndFrame;
			}
		}
	}
	bool moved = false;
	if (next != frame) {
		AnimComboData->Set_Frame(next);
		moved = true;
	}
	UIMesh->Unknown2C->Append_Anim_Combo_Data(AnimComboData);
	if (freed) {
		Unknown_5DCCE0(3, -1);
	}
	if (moved) {
		Unknown_5DCCE0(0, next);
	}
	bool finished = done && DeleteSelf;
	return(!finished);
}

// @0x5DD720
void UIProc5DCD20Class::Unknown_0C()
{
	UIMesh->Unknown2C->Remove_Anim_Combo_Data(AnimComboData);
	Unknown_5DCCE0(2, -1);
}

// @0x5DD770
bool UIProc5DD770Class::Unknown_04()
{
	UIMesh->Unknown_5B9A20(Unknown20, Unknown24);
	Unknown_5DCCE0(1, -1);
	return(false);
}

// @0x5DD7E0
bool UIProc5DD7E0Class::Unknown_04()
{
	UIMesh->Unknown_5B9550(Unknown20);
	Unknown_5DCCE0(1, -1);
	return(false);
}

// @0x5DD830
bool UIProc5DD830Class::Unknown_04()
{
	UIMesh->Unknown_5B9D80(Name, (TextureClass *)Unknown24);
	Unknown_5DCCE0(1, -1);
	return(false);
}

// @0x5DD8A0
bool UIProc5DD8A0Class::Unknown_04()
{
	UIMesh->Unknown_5B9FB0(Name, Unknown24);
	Unknown_5DCCE0(1, -1);
	return(false);
}

// @0x5DD910
bool UIProc5DD910Class::Unknown_04()
{
	UIMesh->Unknown_5BA270(Name);
	Unknown_5DCCE0(1, -1);
	return(false);
}

// Reconstruction notes (UP1, 2026-11-07)
// - The object: __FILE__ e:\build\beta\G\app\Client\ui_proc.cpp (0xB84AF8), FAIL_IFs on lines 36-135. Its .data opens with
//   0x5DD030's static (0xB84AD4); the strings before it (TALK_%s_ENERGY_LOW_1.wav, EnergyPercent, the bars) and the code
//   before 0x5DCCE0 (0x5DCB50-0x5DCBE0, a gd_gauge subclass, table 0xAF5CA0) are the previous object's, between
//   ui_popupmsg.cpp and here, with no __FILE__ of its own. After 0x5DD910 comes 0x5DD960, the face / target / orient
//   buttons' object (its strings follow these, no __FILE__ either).
// - 0x5DCD20 and 0x5DCF00 store the same table 0xAF5D94: one class with two constructors, by name and by animation.
// - 0x5DCD20 reloads the animation after a miss and drops the result: the FAIL_IF '!anim' (l.52) is reported always then.
