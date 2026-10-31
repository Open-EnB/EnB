// Earth & Beyond client (reconstruction)
#include "sbaseview.h"
#include "..\shared\unknown_debug.h"
#include "..\..\ww3d2\rendobj.h"
#include "..\..\ww3d2\scene.h"
#include "..\..\ww3d2\hanim.h"
#include "..\..\ww3d2\assetmgr.h"
#include "..\..\ww3d2\ww3d.h"
#include "..\..\wwaudio\AudibleSound.h"
#include "..\audio\soundlib.h"

// fontsys.cpp's text object (a render object), as far as the starbase view calls it
class FontSys : public RenderObjClass	// NAME 3: the client's profile label 'FontSys::Change_Text' (0x6CDBC0)
{
public:
	void Change_Text(const char *text, int unknown);	// 0x6CDBC0  // NAME 1 [text, unknown]: what 0x6A0E80 passes
};

SBaseViewUnknown1B4Class *Unknown_6987A0(LoungeNPCPacketClass *packet);	// 0x6987A0 (sbase_info.cpp)  // NAME 1 [packet]: what 0x6A0FD0 passes

// ui_talkt_space.cpp's talk tree (sclient.cpp declares it so); 0x69CDE0 resets SClient's to it
class UITalkTreeSpaceClass : public InterfaceClass	// NAME 3: class-from-file (D24) for ui_talkt_space.cpp
{
public:
	char Unknown84[0x184 - 0x84];
};

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

// 0x69CDE0: the interfaces released. AvatarUI and ShipUI are RefCountPtrs in the client (released and cleared as
// the others are, the store before the next load); cpacket.cpp reads them as plain pointers, so they are cast here
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
		(RefCountPtr<SBaseViewUnknown19CClass> &)AvatarUI = NULL;
		(RefCountPtr<SBaseViewUnknown1A0Class> &)ShipUI = NULL;
		((SClient *)((ClientClass *)Client)->Unknown_18())->Unknown_74C9D0(RefCountPtr<UITalkTreeSpaceClass>());
		if (Starbase) {
			Starbase->Unknown_8C();
			SClient *client = (SClient *)((ClientClass *)Client)->Unknown_18();
			client->PDAInterface->Unknown_695FE0(true);
		}
	}
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

// 0x69F480: channel index stopped and released
void SBaseViewClass::Unknown_69F480(int index)
{
	if (Channels[index].Model && Channels[index].Anim) {
		Channels[index].Model->Set_Animation(Channels[index].Anim, 0.0f, 0);
	}
	REF_PTR_RELEASE(Channels[index].Model);
	REF_PTR_RELEASE(Channels[index].Anim);
	Channels[index].Mode = 0;
	Channels[index].Frame = 0.0f;
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
				channel.Model->Set_Animation(channel.Anim, frames - 1.0f, channel.Mode);
				channel.Frame = frames;
			} else {
				channel.Model->Set_Animation(channel.Anim, 0.0f, channel.Mode);
				channel.Frame = 0.0f;
			}
			channel.Mode = 0;
		} else if (channel.Anim) {
			float step = channel.Anim->Get_Frame_Rate() * time;
			if (channel.Backward) {
				channel.Frame -= step;
				channel.Model->Set_Animation(channel.Anim, channel.Frame, channel.Mode);
			} else {
				channel.Frame += step;
			}
		}
	}
}

// 0x69F850: channel index at its end (no animation counts as ended)
bool SBaseViewClass::Unknown_69F850(int index)
{
	HAnimClass *anim = Channels[index].Anim;
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

// 0x6A0540: the render objects out of their scenes
void SBaseViewClass::Unknown_6A0540()
{
	if (Unknown68) {
		Unknown68->Unknown00->Remove();
	}
	if (Unknown1CC) {
		Unknown1CC->Remove();
	}
	if (Unknown1D0) {
		Unknown1D0->Remove();
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
**	loader 0x6A0590, not written) are left free. The constructor and destructor are not in the window; not rebuilt
**	either: 0x69D030, 0x69DC80-0x69F3B0, 0x69F540, 0x69F8F0, 0x6A0590, 0x6A0EC0, 0x6A1180-0x6A1C90, 0x6A2120.
**
**	The object starts before the window (TG3, 2026-10-31): SBaseViewClass's constructor 0x69AD30 and destructor
**	0x69B0C0 store its table 0xAFD2FC (slots 0x69B090, 0x69B5E0, UIViewClass's 0x08, 0x69C5E0, 0x69C0D0, 0x69C8F0,
**	0x69C5A0, ...), and 0x69C8F0 calls 0x69CDE0 / 0x69D030: the view's code runs from 0x69AD30 at the latest
**	(sbase_info.cpp's last assert function is 0x6987A0). 0x69CCD0 and 0x69CDE0 are written above 0x69D930, in the
**	client's order (their lines taken from the blank stretch).
*/
