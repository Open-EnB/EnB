// Earth & Beyond client (reconstruction): app\Client\ui_manu_util.cpp. Notes at the end of the file.
#include <stdio.h>
#include "ui_manu.h"
#include "unknown_675120.h"	// ButtonGadgetClass (over gd_robj.h's RObjGadgetClass: the model at +0x30, slot +0x8C)
#include "rendobj.h"
#include "assetmgr.h"
#include "texture.h"
#include "textureloader.h"
#include "..\shared\g_assert.h"
#include "..\..\ww3d2\ww3d.h"	// WW3D::Get_Frame_Time (0xBFAE3C - 0xBFAE40, read inline)

bool Unknown_495FD0(RenderObjClass *robj, TextureClass *texture, int unknown);	// 0x495FD0 (ro_funcs.cpp, cdecl): the texture onto the model  // NAME 1 [robj, texture, unknown]: as ui_manu.cpp

template <class T> inline void Set_All(T *array, const T &value) { for (int i = 0; i < 5; i++) array[i] = value; }	// NAME 1: neutral (D10): the constructor's three arrays
// NOT EXACT: shape (explain.py: the client keeps a pointer per array, lea ecx, [this + 0x14] / [this + 0x28], and stores the first element of the second through this) (0x5B73C0)
UIManuUnknownC0Class::UIManuUnknownC0Class(int column) :
	Unknown3C(0),
	Unknown40(0),
	Unknown48(column)
{
	Set_All(Gadgets, (ButtonGadgetClass *)NULL);
	Set_All(Unknown14, (RenderObjClass *)NULL);
	Set_All(IDs, -1);

}

UIManuUnknownC0Class::~UIManuUnknownC0Class()
{
	for (int i = 0; i < 5; i++) {
		if (Gadgets[i] != NULL) {
			GadgetClass::Destroy(Gadgets[i]);
			Gadgets[i] = NULL;
		}
		REF_PTR_RELEASE(Unknown14[i]);
	}
}

void UIManuUnknownC0Class::Unknown_5B74A0()
{
	for (int i = 0; i < 5; i++) {
		if (Gadgets[i] != NULL) {
			Gadgets[i]->Unknown_8C(false);
		}
		if (Unknown14[i] != NULL) {
			Unknown14[i]->Set_Hidden(false);
		}
	}
}

void UIManuUnknownC0Class::Unknown_5B74F0(bool on)
{
	if (on) {
		for (int i = 0; i < 5; i++) {
			if (Gadgets[i] != NULL) {
				Gadgets[i]->Unknown_40(false);
				Gadgets[i]->Unknown_8C(true);
			}
			if (Unknown14[i] != NULL) {
				Unknown14[i]->Set_Hidden(true);
			}
		}
		return;
	}
	Unknown3C = -1;
	Unknown44 = 0.0f;
}



























void UIManuUnknownC0Class::Unknown_5B7570(bool *disabled, int primary, int category)
{
	int count = 0;
	for (int i = 0; i < 5; i++) {
		if (!disabled[i] && Gadgets[i] != NULL) {
			char name[64];
			char file[64];
			switch (Unknown48) {
				case 0:
					sprintf(name, "ICON_%d", i);
					sprintf(file, "IM_%d.tga", i);
					break;
				case 1:
					G_ASSERT(primary >= 0);
					sprintf(name, "ICON_%d%d", primary, i);
					sprintf(file, "IM_%d%d.tga", primary, i);
					break;

				case 2:
					G_ASSERT(primary >= 0);
					G_ASSERT(category >= 0);
					sprintf(name, "ICON_%d%d%d", primary, category, i);
					sprintf(file, "IM_%d%d%d.tga", primary, category, i);
					break;
			}
			Gadgets[count]->Unknown2C_06 = 0;
			GadgetClass::Set_Tooltip(Gadgets[count], "UI_MANUFACTURE", name);
			TextureClass *texture = WW3DAssetManager::Get_Instance()->Get_Texture(file, MIP_LEVELS_ALL, WW3D_FORMAT_UNKNOWN, true);
			if (texture != NULL) {
				if (!texture->Is_Initialized()) {
					TextureLoader::Request_Foreground_Loading(texture);
				}
				if (!texture->Is_Missing_Texture() && Gadgets[count] != NULL) {
					Unknown_495FD0(Gadgets[count]->RObj, texture, 0);
				}
			}
			IDs[count] = i;
			if (Gadgets[count] != NULL) {
				Gadgets[count]->Unknown_40(true);
				Gadgets[count]->Unknown_8C(true);
			}
			if (Unknown14[count] != NULL) {
				Unknown14[count]->Set_Hidden(true);
			}
			count++;
			if (texture != NULL) {
				texture->Release_Ref();
			}
		}
	}
	for (int j = count; j < 5; j++) {
		IDs[j] = -1;
		if (Gadgets[j] != NULL) {
			Gadgets[j]->Unknown_40(false);
			Gadgets[count]->Unknown_8C(true);
		}
		if (Unknown14[j] != NULL) {
			Unknown14[j]->Set_Hidden(true);
		}
	}
	Unknown3C = 1;
	Unknown40 = 0;
	Unknown44 = 0;
}

void UIManuUnknownC0Class::Unknown_5B7880()
{
	if (Unknown3C != 0) {
		Unknown44 += (float)WW3D::Get_Frame_Time() * 0.001f;
		if (Unknown44 >= 0.1f) {
			Unknown44 -= 0.1f;
			if (Unknown3C == 1) {
				if (Unknown40 < 5 && Gadgets[Unknown40] != NULL && Gadgets[Unknown40]->Unknown_60()) {
					Gadgets[Unknown40]->Unknown_8C(false);
					Unknown14[Unknown40]->Set_Hidden(false);
					Unknown40++;
					return;
				}
				Unknown3C = 0;
				return;
			}
			if (Unknown3C == -1) {
				Unknown40--;
				if (Unknown40 >= 0) {
					Gadgets[Unknown40]->Unknown_40(false);
					Gadgets[Unknown40]->Unknown_8C(true);
					Unknown14[Unknown40]->Set_Hidden(true);
					return;
				}
				Unknown3C = 0;
				Unknown40 = 0;
			}
		}
	}
}

/*
**	Reconstruction notes (RECONSTRUCTED, 2026-11-13, O12).
**
**	The client's ui_manu_util.cpp per the asserts' __FILE__ (lines 107, 113, 114 in 0x5B7570). Its object,
**	0x5B7370-0x5B7A8A: it opens with permstr.h's PermStringInitClass static (0x5B7370, 0xBE5C48) after ui_manu.cpp's
**	object (whose guarded <string> ids initialiser is 0x5B71A0, followed by its COMDATs 0x5B7210-0x5B7360), then
**	UIManuUnknownC0Class's six functions 0x5B73C0-0x5B7880, then the header ids initialisers 0x5B79C0 / 0x5B7A40
**	(the guarded std::ctype<unsigned short>::id one, 0xC416FC). mu01's object follows at 0x5B7AB0.
**	The class itself was ui_manu.h's (MU1); it moved to ui_manu_util.h with this file.
*/
