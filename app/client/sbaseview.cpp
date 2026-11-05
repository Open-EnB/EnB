// Earth & Beyond client (reconstruction)
#include "sbaseview.h"
#include "wwmath.h"
#include "..\shared\unknown_debug.h"
#include "..\..\ww3d2\rendobj.h"
#include "..\..\ww3d2\scene.h"
#include "..\..\ww3d2\hanim.h"
#include "..\..\ww3d2\assetmgr.h"
#include "..\..\ww3d2\ww3d.h"
#include "..\..\wwaudio\AudibleSound.h"
#include "..\audio\soundlib.h"
#include "..\..\ww3d2\camera.h"
#include "fontdatabase.h"
#include "fontmanager.h"
#include "ui_group.h"
#include "local_interface_sort.h"
#include "..\..\ww3d2\light.h"
#include <string>
#include "option.h"
#include "interface_scene.h"
#include "tasks\tsk_login.h"
#include "..\input\input.h"
#include "..\..\ww3d2\predlod.h"
#include "..\packets\pkaction.h"
#include "..\..\ww3d2\layer.h"
#include "ui_buff.h"
#include "ui_ship_cust.h"	// ShipUI (+0x1A0)
#include "avatar.h"	// CharacterViewAvatarClass (0x69DC80 / 0x69DFC0)
#include "ui_manu.h"	// UIManuClass (+0x18C, 0x69F8F0)
#include "ui_chat_panel.h"	// SClient's ChatPanel (0x69F8F0)
#include "..\packets\pkstarbasetow.h"	// StarbaseRequestPacketClass (0x69F8F0)
#include "ini.h"	// INIClass (the lights loader 0x6A0590)
// the +0x70 scene (0xC8 bytes, constructor 0x761C80; 0x69B5E0 sets its ambient light)
class SBaseViewUnknown70Class : public SimpleSceneClass { public: SBaseViewUnknown70Class(); char Unknown98[0xC8 - 0x98]; };	// NAME 1: neutral (D10)

float Unknown_77B7D0(float t);	// 0x77B7D0: an easing of 0..1 (0x69ED80)  // NAME 1 [Unknown_77B7D0, t]: neutral (D10)
SBaseViewUnknown1B4Class *Unknown_6987A0(LoungeNPCPacketClass *packet);	// 0x6987A0 (sbase_info.cpp)  // NAME 1 [packet]: what 0x6A0FD0 passes

// ui_talkt_space.cpp's talk tree (sclient.cpp declares it so); 0x69CDE0 resets SClient's to it
class UITalkTreeSpaceClass : public InterfaceClass	// NAME 3: class-from-file (D24) for ui_talkt_space.cpp
{
public:
	char Unknown84[0x184 - 0x84];
};

// 0x69AC20
SBaseViewChannelStruct::SBaseViewChannelStruct() :
	Mode(0),
	Frame(0),
	Backward(false)
{
}

// 0x69AC50
SBaseViewChannelStruct::~SBaseViewChannelStruct()
{
	Anim = NULL;
	Model = NULL;
}

// 0x69AD30
SBaseViewClass::SBaseViewClass(ClientTaskClass *task, int arg) :
	Unknown65B810ViewClass(task, 4),
	Unknown50(-1),
	Unknown54(-1),
	Unknown58(-1),
	Unknown5C(-1),
	Unknown68(NULL),
	Unknown7C(0),
	Unknown80(0),
	Unknown84(true),
	UnknownB4(true),
	UnknownE4(0),
	UnknownE8(0),
	Unknown170(false),
	Unknown1B0(false),
	Unknown1B4(NULL),
	Unknown1BC(0),
	Unknown1C4(0),
	Unknown1C8(0),
	Unknown1D4(0.0f, 0.0f, 0.0f),
	Unknown1E0(0.0f, 0.0f, 0.0f),
	Unknown1EC(false),
	Unknown1ED(false),
	Unknown1EE(false),
	Unknown1EF(false),
	Unknown1F0(false),
	Unknown1F4(arg)
{
	for (int i = 0; i < 3; i++) {
		ViewRObjs[i] = NULL;
	}
	Unknown_BE67BC->Load("fa_sbase.ini");
}

// 0x69B070
SimpleSceneClass *SBaseViewClass::Unknown_5C()
{
	return(Unknown70.Peek());
}

// 0x69B0C0
SBaseViewClass::~SBaseViewClass()
{
	if (Unknown1B4) {
		delete Unknown1B4;
		Unknown1B4 = NULL;
	}
	Unknown_BE67BC->Unload("fa_sbase.ini");
	RKey = NULL;
	RCamera = NULL;
}

// 0x69B5E0: Init (the base's first): the scenes and cameras, the layers, the pointer's text object, the trade, group
// and chat interfaces and the two lights
// NOT EXACT: shape (explain.py; 0.84): the trade and group assignments store the member before the temporary's release (the
// other Create_NEW assignments match), and the second light's temporary reuses the first slot
void SBaseViewClass::Unknown_04()
{
	UIViewClass::Unknown_04();
	InterfaceSortClass sort(this, true);
	Unknown68 = new ScriptedMovieViewerSettingsStruct;
	Unknown70 = Create_NEW(new SBaseViewUnknown70Class);
	Unknown70->Set_Ambient_Light(Vector3(0.5f, 0.5f, 0.5f));
	Unknown6C = Create_NEW(new CameraClass);
	Unknown6C->Set_Position(Vector3(0.0f, 0.0f, 100.0f));
	Unknown6C->Set_View_Plane(DEG_TO_RADF(65.0f), -1.0f);
	Unknown6C->Set_Clip_Planes(1.0f, 2000.0f);
	Unknown78 = Create_NEW(new SimpleSceneClass);
	Unknown78->Set_Ambient_Light(Vector3(1.0f, 1.0f, 1.0f));
	Unknown74 = Create_NEW(new CameraClass);
	Unknown74->Set_Position(Vector3(0.0f, 0.0f, 640.0f));
	Unknown74->Set_View_Plane(DEG_TO_RADF(45.0f), -1.0f);
	Unknown74->Set_Clip_Planes(1.0f, 10000.0f);
	Unknown7C = new LayerClass(Unknown1C, Unknown20.Peek());
	Unknown80 = new LayerClass(Unknown70.Peek(), Unknown6C.Peek());
	Unknown1B8 = Create_NEW(new FontSys("MouseText_SBASE", 0, ""));
	Unknown1B8->Add(Unknown1C);
	PTrade = Create_NEW(new UIPTradeClass(Client));
	PTrade->Unknown_54();
	PTrade->Unknown_7C(this, 0);
	Unknown1A8 = Create_NEW(new UIGroupClass(Client));
	Unknown1A8->Unknown0F1 = true;
	Unknown1A8->Unknown_54();
	Unknown1A8->Unknown_7C(this, 0);
	Unknown1AC = Create_NEW(new UIBuffClass(this, LayerClass(Unknown1C, Unknown20.Peek())));
	Unknown1AC->Unknown_54();
	Unknown_6B4940(Unknown1AC.Peek());
	RKey = Create_NEW(new LightClass(LightClass::DIRECTIONAL));
	RKey->Set_Ambient(Vector3(0.0f, 0.0f, 0.0f));
	RKey->Set_Diffuse(Vector3(0.0f, 0.0f, 0.0f));
	RKey->Set_Specular(Vector3(0.0f, 0.0f, 0.0f));
	RCamera = Create_NEW(new LightClass(LightClass::DIRECTIONAL));
	RCamera->Set_Ambient(Vector3(0.0f, 0.0f, 0.0f));
	RCamera->Set_Diffuse(Vector3(0.0f, 0.0f, 0.0f));
	RCamera->Set_Specular(Vector3(0.0f, 0.0f, 0.0f));
	if (Unknown1A8) {
		Unknown1A8->Unknown_58();
	}
	if (Unknown1AC) {
		Unknown1AC->Unknown_58();
	}
}

// 0x69C0D0: the interfaces, the render objects, the channels and the scenes released (UIViewClass's 0x65B4A0
// first, not the base's 0x65B8B0)
void SBaseViewClass::Unknown_10()
{
	UIViewClass::Unknown_10();
	Starbase = NULL;
	Unknown64 = NULL;
	Unknown174 = NULL;
	Unknown178 = NULL;
	Unknown17C = NULL;
	Unknown180 = NULL;
	Unknown184 = NULL;
	Unknown188 = NULL;
	Unknown18C = NULL;
	Unknown190 = NULL;
	Jobs = NULL;
	Unknown198 = NULL;
	AvatarUI = NULL;
	ShipUI = NULL;
	Unknown_6A1130();
	Unknown_6A0540();
	if (Unknown68) {
		Unknown68->Unknown00->Remove();
		delete Unknown68;
		Unknown68 = NULL;
	}
	if (RKey) {
		RKey->Remove();
		RKey = NULL;
	}
	if (RCamera) {
		RCamera->Remove();
		RCamera = NULL;
	}
	for (int i = 0; i < 3; i++) {
		Unknown_69D930(i);
	}
	for (i = 0; i < 6; i++) {
		Unknown_69F480(i);
	}
	if (PTrade) {
		PTrade->Unknown_5C();
		PTrade = NULL;
	}
	if (Unknown1A8) {
		Unknown1A8->Unknown_5C();
		Unknown1A8 = NULL;
	}
	if (Unknown1AC) {
		Unknown1AC->Unknown_5C();
		Unknown1AC = NULL;
	}
	Unknown_6A0E00();
	Unknown70 = NULL;
	Unknown6C = NULL;
	Unknown78 = NULL;
	Unknown74 = NULL;
	if (Unknown80) {
		delete Unknown80;
		Unknown80 = NULL;
	}
	if (Unknown7C) {
		delete Unknown7C;
		Unknown7C = NULL;
	}
}

// 0x69C5A0
void SBaseViewClass::Unknown_18(bool arg)
{
	if (arg) {
		Unknown54 = 0;
		Unknown58 = Unknown50;
	} else {
		Unknown_69D030(-1);
	}
}

// 0x69C5E0: the render (the text object follows the pointer, 0.02 up a frame with cursor 5, else 0.01 down)
int SBaseViewClass::Unknown_0C(bool on)
{
	if (Unknown1C4 > 0) {
		Unknown1C4--;
	}
	if (Unknown1C4 == 0) {
		Unknown_69ED80();
	}
	Unknown_69F710();
	Unknown_6A0EC0();
	WW3D::Begin_Render(on, true, Vector3(0.0f, 0.0f, 0.0f));
	PredictiveLODOptimizerClass::Clear();
	Unknown70->Visibility_Check(Unknown6C.Peek());
	Unknown1C->Visibility_Check(Unknown20.Peek());
	Unknown24->Visibility_Check(Unknown28);
	int budget;
	if (OptionClass::Instance->Get_Bool("Graphics_Default")) {
		budget = Client->PolyBudget;
	} else {
		budget = (int)(8000.0f + (OptionClass::Instance->Get_Float("Graphics_Space_Poly") * 0.01f) * (40000.0f - 8000.0f));
	}
	PredictiveLODOptimizerClass::Optimize_LODs(budget * 0.67f);
	if (ViewRObjs[2]) {
		WW3D::Render(Unknown78.Peek(), Unknown74.Peek(), false, false, Vector3(0.0f, 0.0f, 0.0f));
	} else {
		WW3D::Render(Unknown70.Peek(), Unknown6C.Peek(), false, false, Vector3(0.0f, 0.0f, 0.0f));
	}
	((InterfaceSceneClass *)Unknown1C)->Render_With_PIP_Scenes(Unknown20);
	WW3D::Render(Unknown24, Unknown28, false, false, Vector3(0.0f, 0.0f, 0.0f));
	Unknown_6A1180();
	WW3D::End_Render(true);
	return(0);
}

// the client reads these through inline functions: as plain member reads 0x69C8F0 loads them in another order
inline unsigned Unknown_Avatar_ID(SClient *client) { return(((ClientClass *)((ClientClass *)client)->Unknown_18())->AvatarID); }	// NAME 1: neutral (D10), as ui_cpit.cpp's
inline int Unknown_1134(SClient *client) { return(client->Unknown1134); }	// NAME 1: neutral (D10)
inline int Unknown_84(UIStarbaseClass *starbase) { return(starbase->Unknown84); }	// NAME 1: neutral (D10)

// 0x69C8F0: the base's frame, then the talk interface's choice, the pending view change, the sounds, the trade
// and the group window
void SBaseViewClass::Unknown_14()
{
	UIViewClass::Unknown_14();
	if (Unknown5C == 11) {
		if (Unknown190->Unknown_64CF90()) {
			switch (Unknown50) {
				case 0:
					if (Unknown174) Unknown174->Unknown_639030(Unknown190->Unknown_64CF90());
					break;
				case 1:
					if (Unknown178) Unknown178->Unknown_639030(Unknown190->Unknown_64CF90());
					break;
				case 3:
					if (Unknown17C) Unknown17C->Unknown_639030(Unknown190->Unknown_64CF90());
					break;
				case 2:
					if (Unknown180) Unknown180->Unknown_639030(Unknown190->Unknown_64CF90());
					break;
			}
			Unknown190->Unknown_64CFF0();
		}
		if (Unknown190->Unknown_64D3E0()) {
			Unknown_69CDE0();
		}
	}
	if (Unknown1B0) {
		Unknown_69CDE0();
	}
	if (Unknown54 != Unknown50) {
		Unknown_69D030(Unknown54);
	}
	if (Unknown170) {
		Unknown_69D030(-1);
		((SClient *)((ClientClass *)Client)->Unknown_18())->Unknown_74BA30(1, 0, 0);
		Unknown170 = false;
	}
	Unknown_6A1B80();
	if (Client && Starbase) {
		Unknown1F4.Unknown_441A90(Client->Unknown10BC, Unknown_84(Starbase.Peek()));
	}
	if (Unknown64 && Unknown64->Is_Active() && PTrade && PTrade->Is_Active()) {
		((ClientClass *)((ClientClass *)Client)->Unknown_18())->Send_Packet(&ActionPacketClass(Unknown_Avatar_ID(Client), 22, Unknown_1134(Client), 0));
		PTrade->Unknown_5C();
	}
	if ((Unknown64 && Unknown64->Is_Active()) || (Unknown190 && (Unknown190->Is_Active() || Unknown190->Unknown_64EEA0())) || (PTrade && PTrade->Is_Active())) {
		if (Unknown1A8 && Unknown1A8->Is_Active()) {
			Unknown1A8->Unknown_5C();
		}
	} else {
		if (Unknown1A8 && !Unknown1A8->Is_Active()) {
			Unknown1A8->Unknown_58();
			Unknown1A8->Unknown_596770(true);
		}
	}
}

// 0x69CBE0
bool SBaseViewClass::Unknown_69CBE0()
{
	if (Unknown1B0) return(false);
	if (!Unknown64) return(false);
	return(Unknown64->Is_Active());
}

// 0x69CC10: as 0x69CCD0 without PTrade and +0x190
bool SBaseViewClass::Unknown_69CC10()
{
	if (Unknown184 && Unknown184->Is_Active()) return(true);
	if (Unknown188 && Unknown188->Is_Active()) return(true);
	if (Unknown18C && Unknown18C->Is_Active()) return(true);
	if (Jobs && Jobs->Is_Active()) return(true);
	if (Unknown198 && Unknown198->Is_Active()) return(true);
	if (AvatarUI != NULL && AvatarUI->Is_Active()) return(true);
	if (ShipUI != NULL && ShipUI->Is_Active()) return(true);
	return(false);
}

// 0x69CCD0: any of the view's interfaces active
bool SBaseViewClass::Unknown_69CCD0()
{
	if (PTrade != NULL && PTrade->Is_Active()) return(true);
	if (Unknown184 && Unknown184->Is_Active()) return(true);
	if (Unknown188 && Unknown188->Is_Active()) return(true);
	if (Unknown18C && Unknown18C->Is_Active()) return(true);
	if (Unknown190 && (Unknown190->Is_Active() || Unknown190->Unknown160 != 0)) return(true);
	if (Jobs && Jobs->Is_Active()) return(true);
	if (Unknown198 && Unknown198->Is_Active()) return(true);
	if (AvatarUI != NULL && AvatarUI->Is_Active()) return(true);
	if (ShipUI != NULL && ShipUI->Is_Active()) return(true);
	return(false);
}

// 0x69CDE0: the interfaces released (AvatarUI and ShipUI too: RefCountPtrs, released and cleared as the others are)
void SBaseViewClass::Unknown_69CDE0()
{
	Unknown1B0 = false;
	if (Unknown64) {
		Unknown64->Unknown_7C(NULL, 0);
		Unknown64 = NULL;
		Unknown5C = -1;
		Unknown184 = NULL;
		Unknown188 = NULL;
		Unknown18C = NULL;
		Unknown190 = NULL;
		Jobs = NULL;
		Unknown198 = NULL;
		AvatarUI = NULL;
		ShipUI = NULL;
		((SClient *)((ClientClass *)Client)->Unknown_18())->Unknown_74C9D0(RefCountPtr<UITalkTreeClass>());
		if (Starbase) {
			Starbase->Unknown_8C();
			SClient *client = (SClient *)((ClientClass *)Client)->Unknown_18();
			client->PDAInterface->Unknown_695FE0(true);
		}
	}
}

// 0x69D030: starbase interface index made (once, with 0x6A2120's name) and activated as Starbase, the listener
// turned to the +0x6C camera; -1 (allowed without +0x1B4) leaves none. The cases in the client's order (3 before 2)
// NOT EXACT: literal (explain.py; 0.87): Rotate_Z's first row multiplies c * x before s * y in the client
void SBaseViewClass::Unknown_69D030(int index)
{
	if (!Unknown1B4 && index != -1) return;
	if (index == Unknown50) return;
	if (Starbase) {
		Starbase->Unknown_5C();
	}
	Unknown50 = index;
	switch (index) {
		case 0:
			if (!Unknown174) {
				std::string name = Unknown_6A2120(0);
				Unknown174 = Create_Get(new UIStarbaseClass(this, Unknown80, 0, name.c_str()));
				Unknown174->Unknown_54();
				Unknown_6B4940(Unknown174.Peek());
				((SClient *)((ClientClass *)Client)->Unknown_18())->Unknown_74CA70();
			}
			Unknown174->Unknown_58();
			Starbase = Unknown174;
			break;
		case 1:
			if (!Unknown178) {
				std::string name = Unknown_6A2120(1);
				Unknown178 = Create_Get(new UIStarbaseClass(this, Unknown80, 1, name.c_str()));
				Unknown178->Unknown_54();
				Unknown_6B4940(Unknown178.Peek());
				((SClient *)((ClientClass *)Client)->Unknown_18())->Unknown_74CA70();
			}
			Unknown178->Unknown_58();
			Starbase = Unknown178;
			break;
		case 3:
			if (!Unknown17C) {
				std::string name = Unknown_6A2120(3);
				Unknown17C = Create_Get(new UIStarbaseClass(this, Unknown80, 3, name.c_str()));
				Unknown17C->Unknown_54();
				Unknown_6B4940(Unknown17C.Peek());
				((SClient *)((ClientClass *)Client)->Unknown_18())->Unknown_74CA70();
			}
			Unknown17C->Unknown_58();
			Starbase = Unknown17C;
			break;
		case 2:
			if (!Unknown180) {
				std::string name = Unknown_6A2120(2);
				Unknown180 = Create_Get(new UIStarbaseClass(this, Unknown80, 2, name.c_str()));
				Unknown180->Unknown_54();
				Unknown_6B4940(Unknown180.Peek());
				((SClient *)((ClientClass *)Client)->Unknown_18())->Unknown_74CA70();
			}
			Unknown180->Unknown_58();
			Starbase = Unknown180;
			break;
		default:
			Starbase = NULL;
			if (index == -1) goto done;
	}
	if (Unknown6C) {
		Matrix3D tm = Unknown6C->Get_Transform();
		tm.Rotate_X(-WWMATH_PI / 2.0f);
		tm.Rotate_Z(WWMATH_PI / 2.0f);
		SoundLibraryClass::Set_Listener_Transform(tm);
	}
	Unknown1F4.Unknown_441A30(index);
done:
	Unknown54 = Unknown50;
}

// 0x69D930: ViewRObjs[type] out of its scene (0 the view's +0x1C, 1 +0x70, 2 +0x78), then released (the client
// calls it from 0x69D9D0 and 0x69DC60, never inlined: auto-inlining off for it)
#pragma auto_inline(off)
void SBaseViewClass::Unknown_69D930(int type)
{
	if (ViewRObjs[type]) {
		switch (type) {
			case 0:
				Unknown1C->Remove_Render_Object(ViewRObjs[0].Peek());
				break;
			case 1:
				Unknown70->Remove_Render_Object(ViewRObjs[1].Peek());
				break;
			case 2:
				Unknown78->Remove_Render_Object(ViewRObjs[2].Peek());
				break;
		}
		ViewRObjs[type] = NULL;
	}
}
#pragma auto_inline(on)














































































































































































































































// 0x69D9D0: the asset manager's name as ViewRObjs[type], into its scene
void SBaseViewClass::Unknown_69D9D0(int type, const char *name)
{
	Unknown_69D930(type);
	if (!ViewRObjs[type]) {
		ViewRObjs[type] = Create_Get(WW3DAssetManager::Get_Instance()->Create_Render_Obj(name));
		G_ASSERT(ViewRObjs[type]);
		if (!ViewRObjs[type]) {
			return;
		}
		switch (type) {
			case 0:
				Unknown1C->Add_Render_Object(ViewRObjs[0].Peek());
				break;
			case 1:
				Unknown70->Add_Render_Object(ViewRObjs[1].Peek());
				break;
			case 2:
				Unknown78->Add_Render_Object(ViewRObjs[2].Peek());
				break;
		}
	}
}

// 0x69DB50: ViewRObjs[type] released
void SBaseViewClass::Unknown_69DB50(int type)
{
	if (ViewRObjs[type]) {
		ViewRObjs[type] = NULL;
	}
}

// 0x69DBA0: obj as ViewRObjs[type]
void SBaseViewClass::Unknown_69DBA0(int type, const RefCountPtr<RenderObjClass> &obj)
{
	Unknown_69DB50(type);
	if (!ViewRObjs[type]) {
		ViewRObjs[type] = obj;
	}
}

// 0x69DC40
void SBaseViewClass::Unknown_69DC40()
{
	Unknown_69D9D0(2, "FE_COMP1");
}

// 0x69DC60
void SBaseViewClass::Unknown_69DC60()
{
	Unknown_69D930(2);
}
// 0x69DC80: the avatar at ViewRObjs[1]'s bone (offset: only at its position plus +0x1E0), kept in its +0x330
void SBaseViewClass::Unknown_69DC80(RefCountPtr<CharacterViewAvatarClass> avatar, const char *bone, bool offset)
{
	if (ViewRObjs[1] && avatar) {
		Matrix3D tm = ViewRObjs[1]->Get_Bone_Transform(bone);
		if (offset) {
			Vector3 pos = tm.Get_Translation() + Unknown1E0;
			Matrix3D m = avatar->Get_Transform();
			m.Set_Translation(pos);
			avatar->Set_Transform(m);
			avatar->Unknown330 = m;
		} else {
			avatar->Set_Transform(tm);
			avatar->Unknown330 = tm;
		}
	}
}

// 0x69DFC0: the avatar at tm (offset: only at tm's position plus +0x1E0), kept in its +0x330
void SBaseViewClass::Unknown_69DFC0(RefCountPtr<CharacterViewAvatarClass> avatar, const Matrix3D &tm, bool offset)
{
	if (avatar) {
		if (offset) {
			Vector3 pos = tm.Get_Translation() + Unknown1E0;
			Matrix3D m = avatar->Get_Transform();
			m.Set_Translation(pos);
			avatar->Set_Transform(m);
			avatar->Unknown330 = m;
		} else {
			avatar->Set_Transform(tm);
			avatar->Unknown330 = tm;
		}
	}
}

// 0x69E260: the +0x6C camera at ViewRObjs[1]'s bone, turned -90 degrees about y and z (+0x84 keeps it)
void SBaseViewClass::Unknown_69E260(const char *bone)
{
	if (ViewRObjs[1]) {
		Matrix3D tm = ViewRObjs[1]->Get_Bone_Transform(bone);
		tm.Rotate_Y(-WWMATH_PI / 2.0f);
		tm.Rotate_Z(-WWMATH_PI / 2.0f);
		Unknown84 = tm;
		Unknown6C->Set_Transform(tm);
		Unknown1BC = 0;
	}
}

// 0x69E510: the +0x6C camera at tm turned -90 degrees about y and z (with ViewRObjs[1] only)
void SBaseViewClass::Unknown_69E510(Matrix3D tm)
{
	if (ViewRObjs[1]) {
		Unknown84 = tm;
		Unknown84.Rotate_Y(-WWMATH_PI / 2.0f);
		Unknown84.Rotate_Z(-WWMATH_PI / 2.0f);
		Unknown6C->Set_Transform(Unknown84);
		UnknownE8 = 0;
		Unknown1BC = 0;
	}
}

// 0x69E6E0: the camera move's target +0xB4 at ViewRObjs[1]'s bone, turned as 0x69E260 (keep: +0x84's turn at the
// bone's position), lift added to its z; the move starts now over seconds
void SBaseViewClass::Unknown_69E6E0(const char *bone, int seconds, bool keep, float lift)
{
	if (ViewRObjs[1]) {
		Matrix3D tm = ViewRObjs[1]->Get_Bone_Transform(bone);
		tm.Rotate_Y(-WWMATH_PI / 2.0f);
		tm.Rotate_Z(-WWMATH_PI / 2.0f);
		if (keep) {
			Matrix3D m = Unknown84;
			m.Set_Translation(tm.Get_Translation());
			UnknownB4 = m;
		} else {
			UnknownB4 = tm;
		}
		if (lift != 0.0f) {
			UnknownB4[2][3] += lift;
		}
		UnknownE8 = 0;
		Unknown1BC = 0;
		Unknown1C4 = 3;
		UnknownE4 = (float)seconds;
	}
}

// 0x69EB20: the camera move's target +0xB4 (tm turned as 0x69E510) over seconds; restart starts it now
void SBaseViewClass::Unknown_69EB20(Matrix3D tm, int seconds, bool restart)
{
	if (ViewRObjs[1]) {
		UnknownB4 = tm;
		UnknownB4.Rotate_Y(-WWMATH_PI / 2.0f);
		UnknownB4.Rotate_Z(-WWMATH_PI / 2.0f);
		Unknown1BC = 0;
		UnknownE4 = (float)seconds;
		if (restart) {
			UnknownE8 = 0;
			Unknown1C4 = 3;
		}
	}
}

#pragma auto_inline(off)
// 0x69ED80: the +0x6C camera between +0x84 and +0xB4 (0x77B7D0 eases the fraction); at the end it is +0xB4
void SBaseViewClass::Unknown_69ED80()
{
	if (UnknownE4 != 0.0f) {
		UnknownE8 += WW3D::Get_Frame_Time() * 0.001f;
		if (UnknownE8 > UnknownE4) {
			Unknown84 = UnknownB4;
			UnknownE4 = 0;
			Unknown6C->Set_Transform(Unknown84);
		} else {
			float factor = Unknown_77B7D0(UnknownE8 / UnknownE4);
			Matrix3D tm = Lerp(Unknown84, UnknownB4, factor);
			Unknown6C->Set_Transform(tm);
		}
	}
}
#pragma auto_inline(on)
// 0x69EF00: the +0x6C camera turned by the frame time (mode 0 / 1 about y, 2 / 3 its pitch +0x1BC up / down to
// 0.25), kept as +0x84 (the time's product in the client's order: parenthesised)
void SBaseViewClass::Unknown_69EF00(int mode, float speed)
{
	float angle = (((float)WW3D::Get_Frame_Time() * 0.001f) * speed) * 3.0f;
	Matrix3D tm = Unknown6C->Get_Transform();
	switch (mode) {
		case 0:
			tm.Rotate_X(-Unknown1BC);
			tm.Rotate_Y(angle);
			tm.Rotate_X(Unknown1BC);
			break;
		case 1:
			tm.Rotate_X(-Unknown1BC);
			tm.Rotate_Y(-angle);
			tm.Rotate_X(Unknown1BC);
			break;
		case 2:
			speed = WWMath::Min(angle + Unknown1BC, 0.25f);
			tm.Rotate_X(speed - Unknown1BC);
			Unknown1BC = speed;
			break;
		case 3:
			speed = WWMath::Max(Unknown1BC - angle, -0.25f);
			tm.Rotate_X(speed - Unknown1BC);
			Unknown1BC = speed;
			break;
	}
	Unknown6C->Set_Transform(tm);
	Unknown84 = tm;
}

// 0x69F440
void SBaseViewClass::Unknown_69F440()
{
	Unknown1B0 = true;
}

// 0x69F460
void SBaseViewClass::Unknown_69F460(int arg)
{
	Unknown54 = arg;
	Unknown58 = Unknown50;
}
#pragma auto_inline(off)
// 0x69F480: channel index stopped and released (never inlined: 0x69C0D0 and 0x69F540 call it)
void SBaseViewClass::Unknown_69F480(int index)
{
	if (Channels[index].Model && Channels[index].Anim) {
		Channels[index].Model->Set_Animation(Channels[index].Anim.Peek(), 0.0f, 0);
	}
	Channels[index].Model = NULL;
	Channels[index].Anim = NULL;
	Channels[index].Mode = 0;
	Channels[index].Frame = 0.0f;
}
#pragma auto_inline(on)
// 0x69F540: channel index started (stopped first): name's animation on model, mode 1 when once else 2; backward
// it starts at the last frame
void SBaseViewClass::Unknown_69F540(const RefCountPtr<RenderObjClass> &model, const char *name, int index, bool once, bool backward)
{
	if (model) {
		Unknown_69F480(index);
		Channels[index].Anim = Create_Get(WW3DAssetManager::Get_Instance()->Get_HAnim(name));
		if (Channels[index].Anim) {
			Channels[index].Model = model;
			if (once) {
				Channels[index].Mode = 1;
			} else {
				Channels[index].Mode = 2;
			}
			Channels[index].Backward = backward;
			if (backward) {
				int frames = Channels[index].Anim->Get_Num_Frames();
				Channels[index].Model->Set_Animation(Channels[index].Anim.Peek(), frames - 1.0f, Channels[index].Mode);
			}
		}
	}
}

// 0x69F710: every channel one frame on (a new one starts at its first or last frame)
// NOT EXACT: two store-order residues (the backward start stores Frame before Mode; the backward step stores
// Frame and its copy before the pushes); Mode = 0 in each branch with a named frame local scored lower
void SBaseViewClass::Unknown_69F710()
{
	float time = WW3D::Get_Frame_Time() * 0.001f;
	for (int i = 0; i < 6; i++) {
		SBaseViewChannelStruct &channel = Channels[i];
		if (channel.Mode && channel.Anim && channel.Model) {
			float frames = channel.Anim->Get_Num_Frames();
			if (channel.Backward) {
				channel.Model->Set_Animation(channel.Anim.Peek(), frames - 1.0f, channel.Mode);
				channel.Frame = frames;
			} else {
				channel.Model->Set_Animation(channel.Anim.Peek(), 0.0f, channel.Mode);
				channel.Frame = 0.0f;
			}
			channel.Mode = 0;
		} else if (channel.Anim) {
			float step = channel.Anim->Get_Frame_Rate() * time;
			if (channel.Backward) {
				channel.Frame -= step;
				channel.Model->Set_Animation(channel.Anim.Peek(), channel.Frame, channel.Mode);
			} else {
				channel.Frame += step;
			}
		}
	}
}

// 0x69F850: channel index at its end (no animation counts as ended)
bool SBaseViewClass::Unknown_69F850(int index)
{
	HAnimClass *anim = Channels[index].Anim.Peek();
	if (!anim) {
		return(true);
	}
	if (Channels[index].Backward && Channels[index].Frame <= 0.0f) {
		return(true);
	}
	if (Channels[index].Backward) {
		return(false);
	}
	if (Channels[index].Frame >= (float)anim->Get_Num_Frames()) {
		return(true);
	}
	return(false);
}

// 0x69F8F0: interface type made once (5 +0x184, 6 +0x188, 7 the manufacturing window +0x18C, 11 the talk tree
// +0x190, 8 Jobs, 9 +0x198, 14 AvatarUI, 13 ShipUI; the cases in the client's order) and shown as +0x64; 11 also
// hands the talk tree to the client and sends STARBASE_REQUEST action 4 with arg
// NOT EXACT: register (0.84): case 11 loads Unknown1138 into edx (the client eax), case 8 the task's table into eax (the client edx)
void SBaseViewClass::Unknown_69F8F0(int type, int arg)
{
	Unknown_69CDE0();
	switch (type) {
		case 5:
			if (!Unknown184) {
				Unknown184 = Create_Get(new SBaseViewUnknown184Class(this, Unknown7C));
				Unknown184->Unknown_54();
				Unknown_6B4940(Unknown184.Peek());
				Unknown184->Unknown_58();
				((SClient *)((ClientClass *)Client)->Unknown_18())->Unknown_74CA70();
			}
			Unknown64 = Unknown184;
			Unknown5C = type;
			break;
		case 6:
			if (!Unknown188) {
				Unknown188 = Create_Get(new SBaseViewUnknown188Class(this, Unknown7C));
				Unknown188->Unknown_54();
				Unknown_6B4940(Unknown188.Peek());
				Unknown188->Unknown_58();
				((SClient *)((ClientClass *)Client)->Unknown_18())->Unknown_74CA70();
			}
			Unknown64 = Unknown188;
			Unknown5C = type;
			break;
		case 7:
			if (!Unknown18C) {
				Unknown18C = Create_Get(new UIManuClass(this, *Unknown7C));
				Unknown18C->Unknown_54();
				Unknown_6B4940(Unknown18C.Peek());
				Unknown18C->Unknown_58();
				((SClient *)((ClientClass *)Client)->Unknown_18())->Unknown_74CA70();
			}
			Unknown64 = Unknown18C;
			Unknown5C = type;
			break;
		case 11:
		{
			SClient *client = (SClient *)((ClientClass *)Client)->Unknown_18();
			if (!Unknown190) {
				Unknown190 = Create_Get(new UITalkTreeClass(client, 1));
				Unknown190->Unknown_54();
				Unknown190->Unknown_7C(this, false);
				((SClient *)((ClientClass *)Client)->Unknown_18())->Unknown_74CA70();
			}
			client->Unknown_74C9D0(Unknown190);
			Unknown64 = Unknown190;
			Unknown5C = type;
			client->Send_Packet(&StarbaseRequestPacketClass(client->Unknown1138, 4, arg));
			break;
		}
		case 8:
			if (!Jobs) {
				Jobs = Create_Get(new UIJobsClass((SClient *)((ClientClass *)Client)->Unknown_18()));
				Jobs->Unknown_54();
				Jobs->Unknown_7C(this, true);
				((SClient *)((ClientClass *)Client)->Unknown_18())->Unknown_74CA70();
			}
			Unknown64 = Jobs;
			Unknown5C = type;
			break;
		case 9:
			{
				SClient *client = (SClient *)((ClientClass *)Client)->Unknown_18();
				client->ChatPanel->Unknown_5685C0();
			}
			{
				SClient *client = (SClient *)((ClientClass *)Client)->Unknown_18();
				client->ChatPanel->Unknown_568690();
			}
			if (!Unknown198) {
				Unknown198 = Create_Get(new SBaseViewUnknown198Class((SClient *)((ClientClass *)Client)->Unknown_18()));
				Unknown198->Unknown_54();
				Unknown198->Unknown_7C(this, true);
				((SClient *)((ClientClass *)Client)->Unknown_18())->Unknown_74CA70();
			}
			Unknown64 = Unknown198;
			Unknown5C = type;
			break;
		case 14:
			{
				SClient *client = (SClient *)((ClientClass *)Client)->Unknown_18();
				client->ChatPanel->Unknown_5685C0();
			}
			{
				SClient *client = (SClient *)((ClientClass *)Client)->Unknown_18();
				client->ChatPanel->Unknown_568690();
			}
			if (!AvatarUI) {
				AvatarUI = Create_Get(new SBaseViewUnknown19CClass(this, Unknown7C));
				AvatarUI->Unknown_54();
				Unknown_6B4940(AvatarUI.Peek());
				AvatarUI->Unknown_7C(this, true);
				((SClient *)((ClientClass *)Client)->Unknown_18())->Unknown_74CA70();
			}
			Unknown64 = AvatarUI;
			Unknown5C = type;
			{
				SClient *client = (SClient *)((ClientClass *)Client)->Unknown_18();
				client->PDAInterface->Unknown_695FE0(false);
			}
			break;
		case 13:
			{
				SClient *client = (SClient *)((ClientClass *)Client)->Unknown_18();
				client->ChatPanel->Unknown_5685C0();
			}
			{
				SClient *client = (SClient *)((ClientClass *)Client)->Unknown_18();
				client->ChatPanel->Unknown_568690();
			}
			if (!ShipUI) {
				ShipUI = Create_Get(new StarbaseShipCustomizeInterface(this, Unknown7C));
				Unknown_6B4940(ShipUI.Peek());
				ShipUI->Unknown_54();
				ShipUI->Unknown_7C(this, true);
				((SClient *)((ClientClass *)Client)->Unknown_18())->Unknown_74CA70();
			}
			Unknown64 = ShipUI;
			Unknown5C = type;
			{
				SClient *client = (SClient *)((ClientClass *)Client)->Unknown_18();
				client->PDAInterface->Unknown_695FE0(false);
			}
			break;
	}
	if (Starbase) {
		Starbase->Unknown_88();
	}
	Unknown58 = Unknown5C;
}

// 0x6A0540: the render objects out of their scenes
void SBaseViewClass::Unknown_6A0540()
{
	if (Unknown68) {
		Unknown68->Unknown00->Remove();
	}
	if (RKey) {
		RKey->Remove();
	}
	if (RCamera) {
		RCamera->Remove();
	}
}
















































// Matrix3D::Rotate_Vector without its alias test (0x6A1C90 reads +0x1D4 in place; the client sums rows 1 and 2
// from the Z term down to the X term, row 0 from X); the lights loader 0x6A0590 uses it too
static inline void Unknown_Rotate_Vector(const Matrix3D &A, const Vector3 &in, Vector3 *out)	// NAME 1: neutral (D10)
{
	out->X = (A[0][0] * in.X + A[0][1] * in.Y + A[0][2] * in.Z);
	out->Y = (A[1][0] * in.X + (A[1][1] * in.Y + A[1][2] * in.Z));
	out->Z = (A[2][0] * in.X + (A[2][1] * in.Y + A[2][2] * in.Z));
}

// 0x6A0590: the lights loader: +0x68's scene light and the two lights back into the +0x70 scene, the ambient light
// and the RKEY / RCAMERA lights from the INI name ('LIGHTS'); RKey turned to the scene light's position plus its offset
// NOT EXACT: frame (0.72): the frame slots (the matrix rows, the cross products' temporaries) and the rotation's row order (the client computes Z, Y, X)
void SBaseViewClass::Unknown_6A0590(const char *name)
{
	if (Unknown68) {
		Unknown68->Unknown00->Remove();
	}
	if (RKey) {
		RKey->Remove();
	}
	if (RCamera) {
		RCamera->Remove();
	}
	{Vector3 color(0.0f, 0.0f, 0.0f);
	Unknown68->Unknown_4909D0(name, color);
	color.X = Unknown68->Unknown10.X; color.Y = Unknown68->Unknown10.Y; color.Z = Unknown68->Unknown10.Z;
	Unknown70->Set_Ambient_Light(color);}
	Unknown68->Unknown00->Set_Force_Visible(true);
	Unknown68->Unknown00->Set_Far_Attenuation_Range(1000000, 1000000);
	Unknown68->Unknown00->Add(Unknown70.Peek());
	FAIL_IF(!RKey.Peek()) {return;}
	FAIL_IF(!RCamera.Peek()) {return;}
	RKey->Add(Unknown70.Peek());
	RCamera->Add(Unknown70.Peek());
	INIClass ini;
	ini.Load(name);
	if (ini.Is_Loaded()) {
		Vector4 color;
		color.X = ini.Get_Float("LIGHTS", "RKEYR", 0);
		color.Y = ini.Get_Float("LIGHTS", "RKEYG", 0);
		color.Z = ini.Get_Float("LIGHTS", "RKEYB", 0);
		color.W = ini.Get_Float("LIGHTS", "RKEYI", 0);
		Vector3 offset;
		offset.X = ini.Get_Float("LIGHTS", "RKEYX", 0);
		offset.Y = ini.Get_Float("LIGHTS", "RKEYY", 0);
		offset.Z = ini.Get_Float("LIGHTS", "RKEYZ", 0);
		if (RKey) {
			RKey->Set_Diffuse(Vector3(color.X, color.Y, color.Z));
			RKey->Set_Intensity(color.W);
			Vector3 dir = Unknown68->Unknown00->Get_Position();
			dir.Normalize();
			dir = -dir;
			Vector3 right;
			Vector3::Cross_Product(Vector3(0.0f, 0.0f, 1.0f), dir, &right);
			if (right.Length2() < 0.0001f) {
				Vector3::Cross_Product(Vector3(0.0f, 1.0f, 1.0f), dir, &right);
			}
			right.Normalize();
			Vector3 up;
			Vector3::Cross_Product(dir, right, &up);
			up.Normalize();
			Matrix3D tm(dir, right, up, dir);
			Vector3 light;
			Unknown_Rotate_Vector(tm, offset, &light);
			dir = light + dir;
			dir.Normalize();
			RKey->Set_Spot_Direction(-dir);
		}
		color.X = ini.Get_Float("LIGHTS", "RCAMERAR", 0);
		color.Y = ini.Get_Float("LIGHTS", "RCAMERAG", 0);
		color.Z = ini.Get_Float("LIGHTS", "RCAMERAB", 0);
		color.W = ini.Get_Float("LIGHTS", "RCAMERAI", 0);
		Unknown1D4.X = ini.Get_Float("LIGHTS", "RCAMERAX", 0);
		Unknown1D4.Y = ini.Get_Float("LIGHTS", "RCAMERAY", 0);
		Unknown1D4.Z = ini.Get_Float("LIGHTS", "RCAMERAZ", 0);
		if (RCamera) {
			RCamera->Set_Diffuse(Vector3(color.X, color.Y, color.Z));
			RCamera->Set_Intensity(color.W);
		}
	}
}

// 0x6A0E00: the text object away
void SBaseViewClass::Unknown_6A0E00()
{
	if (Unknown1B8) {
		Unknown1B8->Remove();
		Unknown1B8 = NULL;
	}
}

// 0x6A0E50: the text object hidden
void SBaseViewClass::Unknown_6A0E50()
{
	if (Unknown1B8) {
		Unknown1B8->Set_Hidden(1);
	}
}

// 0x6A0E80: the text object's text, shown
void SBaseViewClass::Unknown_6A0E80(const char *text)
{
	if (Unknown1B8) {
		Unknown1B8->Change_Text(text, 0);
		Unknown1B8->Set_Hidden(0);
	}
}

// 0x6A0EC0: the text object follows the pointer (0.02 up a frame with cursor 5, else 0.01 down; 0x69C5E0 inlines it)
void SBaseViewClass::Unknown_6A0EC0()
{
	if (Unknown1B8) {
		float x;
		float y;
		Unknown_42ECF0(&x, &y);
		if (((SClient *)((ClientClass *)Client)->Unknown_18())->Unknown1274->Unknown_515320() == 5) {
			y = WWMath::Min(1.0f, y + 0.02f);
		} else {
			y = WWMath::Max(0.0f, y - 0.01f);
		}
		Unknown1B8->Set_Position(Vector3(x * 640.0f - 320.0f, 240.0f - y * 480.0f, 0.0f));
	}
}

// 0x6A0FD0: LOUNGE_NPC 82
void SBaseViewClass::Unknown_6A0FD0(LoungeNPCPacketClass *packet)
{
	Unknown1B4 = Unknown_6987A0(packet);
}

// 0x6A1000: the 2-D sound name as +0x1C0, looped
void SBaseViewClass::Unknown_6A1000(const char *name)
{
	if (Unknown1C0) {
		Unknown1C0->Stop();
		Unknown1C0 = NULL;
	}
	Unknown1C0 = Create_Get(SoundLibraryClass::Play_2D_Sound(name, 1));
	if (Unknown1C0) {
		Unknown1C0->Set_Loop_Count(0);
	}
}

// 0x6A1130: the sound stopped
void SBaseViewClass::Unknown_6A1130()
{
	if (Unknown1C0) {
		Unknown1C0->Stop();
		Unknown1C0 = NULL;
	}
}

// 0x6A1880: a text object of kind (0 name, 1 / 2 NPC, 3 guild) into text, added to the +0x1C scene
void SBaseViewClass::Unknown_6A1880(RefCountPtr<FontSys> &text, int kind)
{
	switch (kind) {
		case 0:
			text = Create_NEW(new FontSys("NameText_SBASE", 0, ""));
			break;
		case 1:
		case 2:
			text = Create_NEW(new FontSys("NPCText_SBASE", 0, ""));
			break;
		case 3:
			text = Create_NEW(new FontSys("GuildText_SBASE", 0, ""));
			break;
	}
	text->Add(Unknown1C);
}

// 0x6A1B80: the GROUP_INVITE box's answer (1 accept, 2 decline) sent, the box closed
void SBaseViewClass::Unknown_6A1B80()
{
	if (!Get_Unknown34()->Is_Active()) return;
	int button = Get_Unknown34()->Unknown_5890B0("GROUP_INVITE");
	if (button) {
		if (button == 1) {
			((ClientClass *)Client)->Send_Packet(&ActionPacketClass(((ClientClass *)Client)->AvatarID, 11, ((ClientClass *)Client)->AvatarID, 0));
		} else if (button == 2) {
			((ClientClass *)Client)->Send_Packet(&ActionPacketClass(((ClientClass *)Client)->AvatarID, 12, ((ClientClass *)Client)->AvatarID, 0));
		}
		Unknown34->Unknown_5C();
	}
}

// 0x6A1C90: RCamera turned along the +0x6C camera's view plus the offset +0x1D4 (the lights loader's RKey math)
void SBaseViewClass::Unknown_6A1C90()
{
	Vector3 dir = -Unknown6C->Get_Transform().Get_Z_Vector();
	Vector3 right;
	Vector3::Cross_Product(Vector3(0.0f, 0.0f, 1.0f), dir, &right);
	if (right.Length2() < 0.0001f) {
		Vector3::Cross_Product(Vector3(0.0f, 1.0f, 1.0f), dir, &right);
	}
	right.Normalize();
	Vector3 up;
	Vector3::Cross_Product(dir, right, &up);
	up.Normalize();
	Matrix3D tm(dir, right, up, dir);
	Vector3 light;
	Unknown_Rotate_Vector(tm, Unknown1D4, &light);
	dir = light + dir;
	dir.Normalize();
	RCamera->Set_Spot_Direction(-dir);
}

// 0x6A20C0: the +0x70 scene's fog on
void SBaseViewClass::Unknown_6A20C0(const Vector3 &color, float start, float end)
{
	Unknown70->Set_Fog_Enable(true);
	Unknown70->Set_Fog_Color(color);
	Unknown70->Set_Fog_Range(start, end);
}

// 0x6A2100: the +0x70 scene's fog off
void SBaseViewClass::Unknown_6A2100()
{
	Unknown70->Set_Fog_Enable(false);
}

// 0x6A2120's tables: the key 0x698730 looks up per index, the model names by index and +0x1B4's column
static int Unknown_B90F3C[4] = {0, 1, 3, 2};	// NAME 1: neutral (D10)
static const char *Unknown_B90F4C[4][10] = {	// NAME 1: neutral (D10)
	{"tsd01a", "jsd01a", "psd01a", "tsd07a", "tsd07a", "tsd07a", "tsd07a", "tsd07a", "tsd07a", "tsd07a"},
	{"tbar01_h", "jbar01_h", "pbar01_h", "nbar01_h", "nbar01_h", "nbar01_h", "nbar01_h", "nbar01_h", "nbar01_h", "nbar01_h"},
	{"tbar01_v", "jbar01_v", "pbar01_v", "nbar01_v", "nbar01_v", "nbar01_v", "nbar01_v", "nbar01_v", "nbar01_v", "nbar01_v"},
	{"tbar01", "jbar01", "pbar01", "nbar01", "nbar01", "nbar01", "nbar01", "nbar01", "nbar01", "nbar01"},
};

// 0x6A2120: the starbase interface's model name: the index's name in +0x1B4's column, '3' or '2' added by the
// entry 0x698730 finds; the plain name when no such model exists (the dead size() reads after each return: the
// client's inline budget, _Tidy out of line and _Eos inline in the last string constructor)
std::string SBaseViewClass::Unknown_6A2120(int index)
{
	std::string name;
	SBaseViewUnknown1B4Class *info_list = Unknown1B4;
	SBaseViewUnknown698730Struct *info = info_list->Unknown_698730(Unknown_B90F3C[index]);
	int column = info_list->Unknown18;
	name = Unknown_B90F4C[index][column];
	if (info->Unknown0B) {
		name += "3";
	} else if (info->Unknown0A) {
		name += "2";
	}
	RefCountPtr<RenderObjClass> obj = Create_Get(WW3DAssetManager::Get_Instance()->Create_Render_Obj(name.c_str()));
	if (obj) {
		return(name); name.size(); name.size();
	}
	return(std::string(Unknown_B90F4C[index][column])); name.size(); name.size();
}

// 0x6A23E0
void SBaseViewClass::Unknown_6A23E0(bool a, bool b, bool c, bool d, bool e)
{
	Unknown1EC = a;
	Unknown1ED = b;
	Unknown1EE = c;
	Unknown1EF = d;
	Unknown1F0 = e;
}

// 0x6A2430
void SBaseViewClass::Unknown_6A2430(bool *a, bool *b, bool *c, bool *d, bool *e)
{
	*a = Unknown1EC;
	*b = Unknown1ED;
	*c = Unknown1EE;
	*d = Unknown1EF;
	*e = Unknown1F0;
}



































































































































































// 0x6A2480
void SBaseViewClass::Unknown_6A2480()
{
	FAIL_IF(Client == NULL) {return;}
	Unknown1F4.Unknown_441A50(Unknown50);
}

/*
**	Notes (UI1, 2026-10-29)
**
**	sbaseview.cpp is the starbase view (SBaseViewClass, sbaseview.h; SClient +0x135C; over uiview.cpp's second view,
**	its Client at +0x4C), client 0x69CDE0-0x6A2530 (appfile.py show app\client\sbaseview.cpp; __FILE__
**	e:\build\beta\G\app\Client\sbaseview.cpp). The asserts sit on the client's lines (D25): 738 ('ViewRObjs[type]',
**	0x69D9D0) and 1667 ('Client == NULL', 0x6A2480); 1261 / 1262 ('!RKey.Peek()', '!RCamera.Peek()', the lights
**	loader 0x6A0590, not written) are left free. Not rebuilt
**	either: 0x69B5E0, 0x69C0D0, 0x69C5E0, 0x69C8F0, 0x69D030, 0x69F540, 0x69F8F0, 0x6A0590, 0x6A0EC0, 0x6A1180-0x6A1C90.
**
**	The object starts before the window (TG3, 2026-10-31): SBaseViewClass's constructor 0x69AD30 and destructor
**	0x69B0C0 store its table 0xAFD2FC (slots 0x69B090, 0x69B5E0, UIViewClass's 0x08, 0x69C5E0, 0x69C0D0, 0x69C8F0,
**	0x69C5A0, ...), and 0x69C8F0 calls 0x69CDE0 / 0x69D030: the view's code runs from 0x69AD30 at the latest
**	(sbase_info.cpp's last assert function is 0x6987A0). 0x69CBE0-0x69CDE0 are written above 0x69D930, in the
**	client's order (their lines taken from the blank stretch).
**
**	SBV (2026-11-04): the constructor, the destructor and the channels' constructor / destructor 0x69AC20 /
**	0x69AC50 (the view's vector iterators) give the layout: the destructor's release order and EH states name
**	every RefCountPtr (+0x60..+0x78 but +0x68, +0xEC, the channels' +0x00 / +0x04, +0x174..+0x1AC, +0x1B8,
**	+0x1C0, +0x1CC, +0x1D0) and +0x1B4 as a deleted pointer; 0x69C5A0 is slot +0x18. Their lines come from
**	the blank stretch above 0x69D9D0 (its assert stays on line 738).
*/
