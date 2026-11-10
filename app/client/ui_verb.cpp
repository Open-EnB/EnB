// Earth & Beyond client (reconstruction): app\client\ui_verb.cpp, the verb window. Notes at the end of the file.
#include <stdio.h>
#include <algorithm>
#include <set>
#include <vector>
#include "sclient.h"
#include "client.h"
#include "co.h"
#include "rendobj.h"
#include "gadget.h"
#include "assetmgr.h"
#include "unknown_675120.h"
#include "..\common\unknown_verb.h"
#include "..\packets\pkaction.h"
#include "..\packets\pkmove.h"
#include "..\audio\soundlib.h"
#include "..\shared\g_assert.h"
#include "verbmanager.h"
#include "ui_verb.h"

void Unknown_496AE0(RenderObjClass *robj, bool arg);	// 0x496AE0  // NAME 1 [robj, arg]: what ui_group.cpp passes

#include "camera_manager.h"







inline VerbEnum Verb_Type(VerbEnum verb) { return((VerbEnum)(verb & 0xFFFF0000)); }		// NAME 5: the client's assert

// The test std::find_if takes (0x659F80)
struct UIVerbButtonMatchStruct	// NAME 1: neutral (D10)
{
	UIVerbButtonMatchStruct(VerbEnum type) : Type(type) {}		// NAME 2 [type]: Verb_Type's
	bool operator () (UIVerbButtonClass *vb) { return(Verb_Type(vb->Get_Verb()) == Type); }
	VerbEnum Type;												// NAME 2: what it holds
};



















































UIVerbClass::UIVerbClass(UIViewClass *view, const LayerClass &layer, UIMeshClass *mesh, int arg) :
	InterfaceClass(view, (int)layer.Scene),
	InterfaceMesh(mesh),
	UnknownA0(arg),
	Layer(layer),
	VerbMangerPtr(NULL),
	Count(0)
{
	VerbMangerPtr = ((SClient *)((Unknown65B810ViewClass *)view)->Client->Unknown_18())->VerbManager;







	G_ASSERT(VerbMangerPtr);
}


UIVerbClass::~UIVerbClass() { Unknown_88(NULL); }


void UIVerbClass::Unknown_54() { Unknown_10(); }


void UIVerbClass::Unknown_58()
{
	if (!Is_Active()) {
		InterfaceClass::Unknown_58();
	}
}


void UIVerbClass::Unknown_5C()
{
	if (Is_Active()) {
		InterfaceClass::Unknown_5C();
	}
}


void UIVerbClass::Unknown_60() {}


bool UIVerbClass::Unknown_68(int event)
{
	SClient *client = (SClient *)((Unknown65B810ViewClass *)View->Unknown_20())->Get_Client()->Unknown_18();
	MainViewClass *main_view = (MainViewClass *)View->As_Main_View();
	UIVerbButtonClass *vb = Find_Button((VerbEnum)event);









	G_ASSERT(vb);
	if (vb == NULL) {
		return(false);
	}
	VerbEnum verb = vb->Get_Verb();

	G_ASSERT(Verb_Type(verb) == (VerbEnum) event);
	if ((verb & 0xFFFF) == 0) {
		switch (event) {
			case VERB_SCAN:
				client->Send_Packet(&ActionPacketClass(client->Get_Avatar_ID(), 0x1E, client->Get_Avatar_ID(), 0));
				SoundLibraryClass::Play_2D_Sound("SR_scifielectronic40.wav", 2);
				break;
			case VERB_LAND:
				client->Send_Packet(&ActionPacketClass(client->Get_Avatar_ID(), 0x1D, Get_Target()->Get_Game_ID(), 0));
				client->CameraManager->Unknown_79B2B0(8);
				client->Unknown13B8 = Get_Target()->Get_Game_ID();
				SoundLibraryClass::Play_2D_Sound("Landing_Init", 2);
				client->Get_Ship()->Unknown_6FC950(true);
				client->Get_Ship()->Unknown_6FC880();
				break;
			case VERB_LOOT:
				main_view->Cpit->Unknown_581E60();
				SoundLibraryClass::Play_2D_Sound("SR_scifielectronic40.wav", 2);
				break;
			case VERB_GROUP:
				client->Send_Packet(&ActionPacketClass(client->Get_Avatar_ID(), 10, Get_Target()->Get_Game_ID(), 0));
				SoundLibraryClass::Play_2D_Sound("SR_scifielectronic40.wav", 2);
				break;
			case VERB_MESSAGE:
				client->Unknown_744E70(false);
				SoundLibraryClass::Play_2D_Sound("message_1.wav", 2);
				break;
			case VERB_TRADE:
				if (client->Get_Avatar_ID() == Get_Target()->Get_Game_ID()) {
					client->Unknown_74D990(0x11, "Cannot trade with yourself.", 0);
					SoundLibraryClass::Play_2D_Sound("SR_scifielectronic40.wav", 2);
				} else if (!client->Unknown_744B60() && client->MainView != NULL && client->MainView->Unknown1E4 != NULL && !client->MainView->Unknown1E4->Is_Active()) {
					client->Send_Packet(&ActionPacketClass(client->Get_Avatar_ID(), 0x14, Get_Target()->Get_Game_ID(), 0));
					SoundLibraryClass::Play_2D_Sound("SR_scifielectronic40.wav", 2);
				}
				break;
			case VERB_TRACTOR:
				client->Send_Packet(&ActionPacketClass(client->Get_Avatar_ID(), 1, client->Get_Avatar_ID(), 0));
				SoundLibraryClass::Play_2D_Sound("SR_scifielectronic40.wav", 2);
				break;
			case VERB_DOCK:
				client->Send_Packet(&ActionPacketClass(client->Get_Avatar_ID(), 0x1C, Get_Target()->Get_Game_ID(), 0));
				client->Get_Ship()->Unknown_6FC880();
				break;
			case VERB_PROSPECT:
				client->Send_Packet(&ActionPacketClass(client->Get_Avatar_ID(), 0x11, Get_Target()->Get_Game_ID(), 0));
				SoundLibraryClass::Play_2D_Sound("SR_scifielectronic40.wav", 2);
				break;
			case VERB_GATE:
				{
					bool on = !client->CameraManager->Unknown_79B4F0(3);
					client->Send_Packet(&ActionPacketClass(client->Get_Avatar_ID(), 0x12, Get_Target()->Get_Game_ID(), on));
				}
				SoundLibraryClass::Play_2D_Sound("SR_scifielectronic40.wav", 2);
				client->Get_Ship()->Unknown_6FC880();
				break;
			case VERB_REGISTER:
				client->Send_Packet(&ActionPacketClass(client->Get_Avatar_ID(), 0x19, Get_Target()->Get_Game_ID(), 0));
				SoundLibraryClass::Play_2D_Sound("SR_scifielectronic40.wav", 2);
				break;
			case VERB_JUMPSTART:
				client->Send_Packet(&ActionPacketClass(client->Get_Avatar_ID(), 0x1A, Get_Target()->Get_Game_ID(), 0));
				SoundLibraryClass::Play_2D_Sound("SR_scifielectronic40.wav", 2);
				break;
			case VERB_FOLLOW:
				if (Get_Target() != NULL && Get_Target()->Get_Game_ID() == client->Get_Avatar_ID()) {
					client->Unknown_74D990(0x11, "Cannot autofollow self.", 0);
				} else {
					client->Send_Packet(&MovePacketClass(client->Get_Avatar_ID(), 0xC));
					SoundLibraryClass::Play_2D_Sound("SR_scifielectronic40.wav", 2);
				}
				break;
			default:





















				G_ASSERT(false);
				return(false);
		}
		vb->Unknown_6607D0();
		return(true);
	}
	char text[129];
	Value_To_String(text, 128, verb);
	client->Unknown_74D990(0x13, text, 0);
	return(false);
}

// NOT EXACT: inline budget (the first Clear_Buttons copy: the client calls size(), inlines erase); the second copy matches
void UIVerbClass::Unknown_88(ClientObjectClass *object)
{
	ClientObjectClass *old = Object; Buttons.size();	// (dead: VC6's inline budget)
	if (object != old) {
		if (old != NULL) {
			Unknown_5C();
			Clear_Buttons();
			VerbMangerPtr->Unknown_81F6A0(old->Get_Game_ID(), this);
		}
		UnknownObjectWatchClass::operator = (object);
		if (object != NULL) {


















			G_ASSERT(Buttons.empty());
			Clear_Buttons();
			VerbMangerPtr->Unknown_81F610(object->Get_Game_ID(), this);
			Unknown_58();
		}
	}
}


void UIVerbClass::Unknown_658F40(const std::set<VerbEnum> *verbs)
{
	bool is_active = Is_Active();






	G_ASSERT(is_active);
	if (is_active) {
		Unknown_5C();
	}
	for (std::set<VerbEnum>::const_iterator it = verbs->begin(); it != verbs->end(); it++) {
		Add_Verb(*it);
	}
	if (is_active) {
		Unknown_58();
	}
}


void UIVerbClass::Unknown_659010(const std::set<VerbEnum> *adds, const std::set<VerbEnum> *drops)
{
	bool is_active = Is_Active();
	if (is_active) {
		Unknown_5C();
	}
	for (std::set<VerbEnum>::const_iterator it = drops->begin(); it != drops->end(); it++) {
		int arg = (*it) & 0xFFFF;
		if (arg == 0 || arg >= 0xFFFF) {
			arg = 0xFFFF;
		}
		Update_Verb((VerbEnum)(Verb_Type(*it) + arg));
	}
	for (std::set<VerbEnum>::const_iterator it2 = adds->begin(); it2 != adds->end(); it2++) {
		Add_Verb(*it2);
	}
	if (is_active) {
		Unknown_58();
	}
}


UIVerbButtonClass *UIVerbClass::Find_Button(VerbEnum verb)
{
	std::vector<UIVerbButtonClass *>::iterator it = std::find_if(Buttons.begin(), Buttons.end(), UIVerbButtonMatchStruct(Verb_Type(verb)));
	if (it == Buttons.end()) {
		return(NULL);
	}
	return(*it);
}


void UIVerbClass::Add_Verb(VerbEnum verb)
{

















	G_ASSERT(!Is_Active()); Buttons.size();	// (dead: VC6's inline budget, the button's constructor)
	UIVerbButtonClass *vb = Find_Button(verb);
	if (vb == NULL) {
		char name[128];
		char text[64];
		Value_To_String(name, 128, Verb_Type(verb));
		sprintf(text, "vb_%s", name);
		RenderObjClass *robj = WW3DAssetManager::Get_Instance()->Create_Render_Obj(text);
		if (robj == NULL) {
			robj = WW3DAssetManager::Get_Instance()->Create_Render_Obj("vb_def");
		}







		G_ASSERT(robj);
		if (robj != NULL && Count < 5) {
			sprintf(text, "button%02d", Count);
			RenderObjClass *scan_obj = InterfaceMesh->Peek_Object();

			FAIL_IF(!scan_obj) {
				REF_PTR_RELEASE(robj);
				return;
			}
			scan_obj->Add_Sub_Object_To_Bone(robj, text);
			Unknown_496AE0(robj, 0);
			vb = new UIVerbButtonClass(verb, robj, Layer);
			Buttons.push_back(vb);
			Unknown64.Unknown_6870E0(vb, Verb_Type(verb), View, 1, 1);
			vb->Unknown2C_06 = 0;
			robj->Release_Ref();
			sprintf(text, "%s.%s-%d", scan_obj->Get_Name(), scan_obj->Get_Name(), Count);
			InterfaceMesh->Unknown_5B8EE0(Count, Count, text, 0, 0, 0, 0, -1);
			Count++;
		}
	}
	Update_Verb(verb);
}


void UIVerbClass::Update_Verb(VerbEnum verb)
{
	UIVerbButtonClass *vb = Find_Button(verb);








	G_ASSERT(vb);
	if (vb == NULL) {
		return;
	}
	vb->Set_Verb(verb);
	char text[129];
	Value_To_String(text, 128, verb);
	GadgetClass::Set_Tooltip_Name(vb, " ", text);
	GadgetClass::Set_Tooltip_Name(vb, "UI_VERB_%s", text);
	GadgetClass::Set_Tooltip(vb, "UI_VERB", "STRING", text);
}


void UIVerbClass::Clear_Buttons()
{
	if (Buttons.size() != 0) {
		for (std::vector<UIVerbButtonClass *>::iterator it = Buttons.begin(); it != Buttons.end(); it++) {
			InterfaceMesh->Peek_Object()->Remove_Sub_Object((*it)->Peek_Render_Obj());
			Unknown64.Unknown_687150(*it);
			GadgetClass::Destroy(*it);
		}
		Buttons.clear();
		Count = 0;
		InterfaceMesh->Unknown_5B9930(-1);
		InterfaceMesh->Unknown_5B9830();
	}
}


UIVerbButtonClass::UIVerbButtonClass(VerbEnum verb, RenderObjClass *robj, const LayerClass &layer) :
	ButtonGadgetClass(robj, layer, 0x55, 3),
	Flashed(false)
{
	Set_Verb(verb);
}


UIVerbButtonClass::~UIVerbButtonClass()
{
}


void UIVerbButtonClass::Set_Verb(VerbEnum verb)
{
	Verb = verb;
	if ((verb & 0xFFFF) == 0) {
		Unknown_40(true);
		Start_Flash();
	} else {
		Unknown_40(false);
		Stop_Flash();
	}
}


VerbEnum UIVerbButtonClass::Get_Verb()
{
	return(Verb);
}


void UIVerbButtonClass::Start_Flash()
{
	if (Verb == VERB_LOOT && !Flashed) {
		Unknown_6603A0(false, NULL);
		Flashed = true;
	}
}


void UIVerbButtonClass::Stop_Flash()
{
	Unknown_6607D0();
}

/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4), from its asserts'
**	'e:\build\beta\G\app\Client\ui_verb.cpp' (0xB8C43C). The object is 0x6580B0-0x65A01A: the literals start
**	with 'VerbMangerPtr' (0xB8C42C), the adjustor thunks 0x659FF0 / 0x65A000 / 0x65A010 close it before the
**	JWEB window (0x65A020). The thunks 0x658090 / 0x6580A0 before it go to 0x6572C0: ui_vault.cpp's object.
**	Clear_Buttons, Find_Button and Update_Verb have no callers: the client inlines them everywhere, and they stay
**	non-inline definitions. Clear_Buttons sits at VC6's auto-inline limit: Buttons.clear() keeps it inlinable,
**	erase(begin, end) or one more call stops it. char text[129] with a size of 128 gives the frame's 0x84.
*/
