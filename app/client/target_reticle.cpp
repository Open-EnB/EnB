// target_reticle.cpp: the reticle round a target (0x7C50B0-0x7C60F0); notes at the end
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "target_reticle.h"
#include "sclient.h"
#include "client.h"
#include "co.h"
#include "co_data.h"
#include "radar_data.h"
#include "gutter_data.h"
#include "scene.h"
#include "assetmgr.h"
#include "matinfo.h"
#include "wwmath.h"
#include "fontmanager.h"
#include "fontdatabase.h"
#include "..\shared\g_assert.h"
#include "..\shared\auxdata\auxdata.h"
#include "..\shared\auxdata\delta_interpolator.h"

void Unknown_496AE0(RenderObjClass *robj, bool make_geometry_unique);	// 0x496AE0 (ro_funcs.cpp, cdecl)  // NAME 1 [robj, make_geometry_unique]: as eff_o.cpp's
float Unknown_7E6060(ClientObjectClass *a, ClientObjectClass *b);	// 0x7E6060 (distance_calc_client.cpp's)  // NAME 1: neutral (D10)

// (two inline layers, NameClass and the root copy in the inner one (ui_analyze.cpp's form): Update calls the find out of line, 0x5BF2B0, and inlines Get_Value, as the client)
static inline DeltaInterpolatorClass *Find_L2(DeltaCompoundClass *const &root, const char *name) { NameClass n(name); DeltaCompoundClass *r = root; DeltaInterpolatorClass *t = Unknown_Find_Field<DeltaInterpolatorClass>(r, n); return(t); } static inline DeltaInterpolatorClass *Find_L1(DeltaCompoundClass *const &root, const char *name) { return(Find_L2(root, name)); }	// NAME 1: neutral (D10)
// The four corners' bones and sub-objects in hud01a, the client's 0xB9F818
static const char *Corner_Names[TargetReticleClass::CORNER_COUNT] = { "UL", "UR", "BR", "BL" };	// NAME 2: what it holds

// @0x7C5100
TargetReticleClass::TargetReticleClass(SClient *client, const RefCountPtr<SceneClass> &scene, int sort_level, bool show_text) :
	Client(client),
	Scene(scene),
	ClientObject(NULL)
{
	Unknown_BE67BC->Load("fc_radar.ini");
	TargetRObj = Create_NEW(WW3DAssetManager::Get_Instance()->Create_Render_Obj("hud01a"));












































	FAIL_IF(!TargetRObj) return;

	for (int i = 0; i < CORNER_COUNT; i++) {
		Corners[i].BoneIndex = TargetRObj->Get_Bone_Index(Corner_Names[i]);
		TargetRObj->Capture_Bone(Corners[i].BoneIndex);
		RenderObjClass *corner = TargetRObj->Get_Sub_Object_By_Name(Corner_Names[i]);
		Unknown_496AE0(corner, false);
		corner->Set_Sort_Level(sort_level);
		Corners[i].Material = Create_NEW(corner->Get_Material_Info());
		Corners[i].VertexMaterial = Create_NEW(Corners[i].Material->Get_Vertex_Material(0));
		corner->Release_Ref();
	}

	if (show_text) {
		Text = Create_NEW(new FontSys("RADAR_DIST", 0, 0));
	}
}

// @0x7C55A0
TargetReticleClass::~TargetReticleClass()
{
	Set_Client_Object(NULL);
	Unknown_BE67BC->Unload("fc_radar.ini");
}

// NOT EXACT: 0x7C5690, register rotation only (13 rows at the inlined time == LastTime: eax / ecx for ecx / eax, edx / eax for ecx / eax, eax for ecx at LastValue); same length (1990 bytes)
// @0x7C5690
void TargetReticleClass::Update(const Vector3 &pos, float size)
{


























	FAIL_IF(!ClientObject) return;
	RadarDataClass *rdata = Get_Radar_Data(ClientObject, false);
	FAIL_IF(!rdata) return;

	Vector3 color = rdata->Color;
	if (TargetRObj->Peek_Scene() == NULL) {
		TargetRObj->Add(Scene.Peek());
	}

	if (size > 0.15f) {
		size = 0.15f;
	}
	size = size * 0.70710677f;
	float width = (float)floor(WWMath::Max(size * 640.0f, 2.0f));
	float height = WWMath::Max(size * 480.0f, 2.0f);
	width += 4.0f;
	height = (float)floor(height) + 4.0f;

	Vector3 center;
	center.Z = 0.0f;
	center.X = pos.X * 640.0f - 320.0f;
	center.Y = pos.Y * -480.0f;
	center.Y += 240.0f;

	// Shrink the box until it fits the 616 x 456 area round the screen's centre, or down to the smallest
	float overflow = 0.0f;
	while (true) {
		width -= overflow;
		height -= overflow;

		int at_smallest = 0;
		if (width <= 4.0f) {
			width = 4.0f;
			at_smallest = 1;
		}
		if (height <= 4.0f) {
			height = 4.0f;
			at_smallest++;
		}
		if (at_smallest == 2) {
			break;
		}

		float top = center.Y + height;
		overflow = center.X + width;
		if (overflow > 308.0f) {
			overflow -= 308.0f;
			if (overflow > 0.0001f) continue;
		}
		if (top > 228.0f) {
			overflow = top - 228.0f;
			if (overflow > 0.0001f) continue;
		}
		float bottom = center.Y - height;
		overflow = center.X - width;
		if (overflow < -308.0f) {
			overflow = -308.0f - overflow;
			if (overflow > 0.0001f) continue;
		}
		if (bottom < -228.0f) {
			overflow = -228.0f - bottom;
			if (overflow > 0.0001f) continue;
		}
		break;
	}

	Matrix3D tm(true);
	TargetRObj->Set_Position(center);

	tm.Translate_X(-width);
	tm.Translate_Y(height);
	TargetRObj->Control_Bone(Corners[0].BoneIndex, tm, false);

	tm.Translate_X(width * 2.0f);
	TargetRObj->Control_Bone(Corners[1].BoneIndex, tm, false);

	tm.Translate_Y(height * -2.0f);
	TargetRObj->Control_Bone(Corners[2].BoneIndex, tm, false);

	tm.Translate_X(width * -2.0f);
	TargetRObj->Control_Bone(Corners[3].BoneIndex, tm, false);

	for (int i = 0; i < CORNER_COUNT; i++) {
		Corners[i].VertexMaterial->Set_Emissive(color.X, color.Y, color.Z);
	}

	if (Text != NULL) {
		GutterDataClass *gdata = Get_Gutter_Data(ClientObject, false);








































		FAIL_IF(!gdata) return;

		float shield = -1.0f;
		DeltaCompoundClass *aux = (DeltaCompoundClass *)ClientObject->AuxData;
		if (aux != NULL) {


			DeltaInterpolatorClass *percent = Find_L1(aux, "ShieldPercent");
			if (percent != NULL && percent->Unknown70 != 0) {
				shield = percent->Get_Value(DurationClass((unsigned)Client->Unknown10BC));
			}
		}

		ClientObjectClass *avatar = Client->Get_Avatar();


		FAIL_IF(!avatar) return;

		float distance = Unknown_7E6060(ClientObject, avatar);
		char text[128];
		text[0] = 0;
		if (distance >= 1.0f) {
			sprintf(text + strlen(text), "%.2fk ", distance * 0.001f);
		}
		if (shield >= 0.0f) {
			sprintf(text + strlen(text), "(%d%%) ", (int)(shield * 100.0f));
		}

		if (text[0] != 0) {
			char *last = &text[strlen(text) - 1];
			if (*last == ' ') {
				*last = 0;
			}
			Text->Change_Text(text, 0);

			RefCountPtr<FontSys> label = gdata->NameLabel;
			RefCountPtr<FontSys> alt_label = gdata->GuildLabel;
			RefCountPtr<FontSys> shown = !alt_label->Unknown300 ? label : alt_label;

			int *to_justify = Text->Info.Justify; int j0 = shown->Info.Justify[0]; int j1 = shown->Info.Justify[1]; to_justify[0] = j0; to_justify[1] = j1;

			float text_width, text_height;
			Text->Unknown_6D5B10(text_width, text_height);
			Vector3 text_pos = shown->Unknown_6D8650();
			text_pos.Y -= text_height;
			Text->Set_Position(text_pos);
			Text->Set_Sort_Level(shown->Get_Sort_Level());
			if (Text->Peek_Scene() == NULL) {
				Text->Add(Scene.Peek());
			}
		} else if (Text->Peek_Scene() != NULL) {
			Text->Remove();
		}
	}
}

// @0x7C6060
void TargetReticleClass::Set_Client_Object(ClientObjectClass *co)
{
	if (co == ClientObject) {
		return;
	}
	if (ClientObject != NULL && co == NULL) {
		if (TargetRObj->Peek_Scene() != NULL) {
			TargetRObj->Remove();
		}
		if (Text != NULL && Text->Peek_Scene() != NULL) {
			Text->Remove();
		}
	}
	ClientObject = co;
	if (co != NULL) {
		if (TargetRObj->Peek_Scene() == NULL) {
			TargetRObj->Add(Scene.Peek());
		}
	}
}

/*
**	Notes
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\client\target_reticle.cpp (the __FILE__ 0xB9F85C, FAIL_IFs at lines 82, 137, 139,
**	265 and 281).
**
**	The object: it starts at permstr.h's PermStringInitClass static (0xBE87E0: initialiser 0x7C50B0, atexit 0x7C50E0, right
**	after target_find.cpp's last COMDAT 0x7C5040) and ends with <string>'s guarded initialiser 0x7C60F0, which precedes
**	targeting.cpp's 0x7C6120. Corners' implicit constructor and destructor are 0x7C54E0 / 0x7C5500 (the vector helpers'
**	arguments). The one known user is gutter_radar.cpp's 0x7B6BD0 (two of them, 0x44 bytes each).
**
**	Forms the bytes decide (Update): the 4.0f added between the two floor calls (width's fadd before height's floor); -480.0f
**	kept apart from the + 240.0f (one expression folds to 240 - 480 * y); Justify's copy through to_justify, with [0] written
**	first in the source and the client loading [1] first; the ShieldPercent find through two inline layers (IB2:
**	they replace the 16 dead no-code finds this file had, D130; same code).
*/
