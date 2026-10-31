// Earth & Beyond client (reconstruction): app\Client\ui_group.cpp. Notes at the end of the file.
#include <stdio.h>
#include <string.h>
#include "ui_group.h"
#include "client.h"
#include "..\packets\pkclientchat.h"
#include "..\packets\pkaction.h"
#include "..\audio\soundlib.h"
#include "rendobj.h"
#include "control_keys_manager.h"
#include "..\shared\auxdata\auxdata.h"
#include "aux_roots.h"				// (after auxdata.h: its Type_Name(int *) for kv_pair.h, C2665 without)
#include "..\shared\unknown_debug.h"

// fontdatabase.h's FontStringFilesClass at 0xBE67BC and fontmanager.h's FontManagerClass at 0xBE67C4, as far as this
// file calls them (fontdatabase.h's 0xBE67B8 clashes with tsk_game.h's, so neither header can be included here)
class FontStyleClass : public RenderObjClass {};	// NAME 1 [FontStyleClass]: fontmanager.h's (a DynamicMeshClass there)
class FontStringFilesClass { public: void Load(const char *name); void Unload(const char *name); };	// NAME 2: fontdatabase.h's
class FontManagerClass { public: void Add(RenderObjClass *object); void Remove(unsigned int id); FontStyleClass *Get_Style(const char *name, unsigned int id); };	// NAME 3: fontmanager.h's
extern FontManagerClass *Unknown_BE67C4;	// NAME 1: neutral (D10)

// gadget.cpp's gadget, as far as the group window calls it (the formation buttons, the members' buttons)
class GadgetClass	// NAME 3: class-from-file (D24) for app\client\gadget.cpp
{
public:
	virtual void Unknown_00(); virtual void Unknown_04(); virtual void Unknown_08(); virtual void Unknown_0C();
	virtual void Unknown_10(); virtual void Unknown_14(); virtual void Unknown_18(); virtual void Unknown_1C();
	virtual void Unknown_20(); virtual void Unknown_24();
	virtual void Unknown_28(LayerClass *layer);			// +0x28 (0x597080: the window's layer)  // NAME 1 [layer]: what 0x597080 passes
	virtual void Unknown_2C();
	virtual void Unknown_30(); virtual void Unknown_34(); virtual void Unknown_38(); virtual void Unknown_3C();
	virtual void Unknown_40(bool on);					// +0x40  // NAME 1 [on]: neutral
	virtual void Unknown_44(); virtual void Unknown_48(); virtual void Unknown_4C(); virtual void Unknown_50();
	virtual void Unknown_54(); virtual void Unknown_58(); virtual void Unknown_5C(); virtual void Unknown_60();
	virtual void Unknown_64(); virtual void Unknown_68(); virtual void Unknown_6C(); virtual void Unknown_70();
	virtual void Unknown_74(); virtual void Unknown_78(); virtual void Unknown_7C(); virtual void Unknown_80();
	virtual void Unknown_84(); virtual void Unknown_88();
	virtual void Unknown_8C(bool on);					// +0x8C  // NAME 1 [on]: neutral
	void Unknown_6603A0(int arg, UIViewClass *view);	// 0x6603A0  // NAME 1 [arg, view]: neutral
	void Unknown_6607D0();								// 0x6607D0

	char Unknown04[0x2C - 0x04];						// +0x04
	unsigned int Unknown2CLow : 11;						// +0x2C
	unsigned int Unknown2C800 : 1;						// +0x2C bit 0x800 (0x598200 sets it with 0x6603A0, clears it with 0x6607D0)
};
void Unknown_65EED0(GadgetClass *gadget);				// 0x65EED0 (cdecl, gadget.cpp): the gadget deleted  // NAME 1 [gadget]: neutral
void Unknown_65EFB0(GadgetClass *gadget, const char *section, const char *name);	// 0x65EFB0 (cdecl, gadget.cpp): the button's text from a string section ('UI_FORMATION', 'GRP_LEAVE')  // NAME 1 [section, name]: what this file passes

extern unsigned int Unknown_BFAE3C; extern unsigned int Unknown_BFAE40;	// NAME 1: neutral (D10): the time now and the last frame's, in ms (mainview.cpp, scriptedmovie.cpp)

// The member rows' bars (0xB7F4B8 / 0xB7F4C4): 0x597310 sorts them above or below the row's 'main'
static const char *Unknown_B7F4B8[] = { "hull_bar0", "shield_bar0", NULL };	// NAME 1: neutral (D10)
static const char *Unknown_B7F4C4[] = { "hull_bar1", "shield_bar1", NULL };	// NAME 1: neutral (D10)

// The SignalMap the group events go to: a function-static instance (co.cpp's 0x4F7EF0), not rebuilt
SignalMap &Unknown_4F7EF0();
class ClientGroupEvent	// NAME 5: the client's RTTI .?AVClientGroupEvent@@
{
public:
	ClientGroupEvent(bool value) : Value(value) {}
	bool Value;											// +0x00  // NAME 1: neutral
};





























































































































































































































































// 0x593200
UIGroupMemberClass::UIGroupMemberClass(UIGroupClass *owner) :
	Owner(owner),
	Name(""),
	ID(0),
	Unknown18(-1),
	Unknown1C(true),
	Unknown1D(false),
	Unknown20(0),
	Unknown24(0),
	Unknown28(NULL),
	Unknown2C(NULL),
	Unknown3C(NULL),
	Unknown40(NULL),
	Unknown44(NULL),
	Unknown48(NULL),
	Unknown4C(NULL)
{
}





// 0x5932E0
// NOT EXACT: inline budget (the client calls RefCountPtr's operator=(NULL) 0x5988A0 out of line for +0x38 / +0x30 here, 0x593790 expands it)
UIGroupMemberClass::~UIGroupMemberClass()
{
	bool active = Get_Interface()->Is_Active();
	if (active) {
		Get_Interface()->Unknown_5C();
	}
	if (Unknown3C) {
		Get_Interface()->Unknown64.Unknown_687150(Unknown3C);
		Unknown_65EED0(Unknown3C);
		Unknown3C = NULL;
	}
	if (Unknown38) {
		Unknown38->Remove();
		Unknown38 = NULL;
	}
	if (Unknown2C) {
		Unknown2C->Unknown_5DCAE0();
		delete Unknown2C;
		Unknown2C = NULL;
	}
	Unknown30 = NULL;
	if (Unknown28) {
		delete Unknown28;
		Unknown28 = NULL;
	}
	Unknown_5934A0();
	if (active) {
		Get_Interface()->Unknown_58();
	}
}




























// 0x5934A0
void UIGroupMemberClass::Unknown_5934A0()
{
	G_ASSERT(Get_Interface()->Is_Partially_Deactivated() || !Get_Interface()->Is_Active());
	if (Unknown40) {
		Get_Interface()->Unknown64.Unknown_687150(Unknown40);
		Unknown_65EED0(Unknown40);
		Unknown40 = NULL;
	}
	if (Unknown44) {
		Get_Interface()->Unknown64.Unknown_687150(Unknown44);
		Unknown_65EED0(Unknown44);
		Unknown44 = NULL;
	}
	if (Unknown48) {
		Get_Interface()->Unknown64.Unknown_687150(Unknown48);
		Unknown_65EED0(Unknown48);
		Unknown48 = NULL;
	}
	if (Unknown4C) {
		Get_Interface()->Unknown64.Unknown_687150(Unknown4C);
		Unknown_65EED0(Unknown4C);
		Unknown4C = NULL;
	}
	if (Unknown34) {
		Unknown34->Remove();
		Unknown34 = NULL;
	}
}

















// 0x5935C0
void UIGroupMemberClass::Unknown_5935C0()
{
	if (Unknown2C) {
		Unknown2C->Unknown_10();
	}
	float elapsed = (Unknown_BFAE3C - Unknown_BFAE40) * 0.001f;
	if (Unknown24 > 0.0f) {
		Unknown24 -= elapsed;
		if (Unknown24 < 0.0f) {
			G_ASSERT(Get_Interface()->Is_Active());
			Get_Interface()->Unknown_5C();
			if (Unknown3C) {
				Get_Interface()->Unknown64.Unknown_687150(Unknown3C);
				Unknown_65EED0(Unknown3C);
				Unknown3C = NULL;
			}
			if (Unknown38) {
				Unknown38->Remove();
				Unknown38 = NULL;
			}
			Get_Interface()->Unknown_58();
			if (Unknown28) {
				delete Unknown28;
				Unknown28 = NULL;
			}
			Unknown24 = 0.0f;
			Unknown1D = false;
		}
	}
	if (Unknown20 > 0.0f) {
		Unknown20 -= elapsed;
		if (Unknown20 < 0.0f) {
			Unknown20 = 0.0f;
			Unknown48->Unknown2C800 = false;
			Unknown48->Unknown_6607D0();
		}
	}
}




















// 0x593750
bool UIGroupMemberClass::Unknown_593750()
{
	ClientClass *client = (ClientClass *)Get_Interface()->Client;
	ClientObjectClass *object = client->Find_Object(ID);
	if ((Unknown1C && object == NULL) || (!Unknown1C && object != NULL)) {
		return(true);
	}
	return(false);
}



// 0x593790
void UIGroupMemberClass::Unknown_593790()
{
	if (Unknown3C) {
		Get_Interface()->Unknown64.Unknown_687150(Unknown3C);
		Unknown_65EED0(Unknown3C);
		Unknown3C = NULL;
	}
	if (Unknown38) {
		Unknown38->Remove();
		Unknown38 = NULL;
	}
	if (Unknown2C) {
		Unknown2C->Unknown_5DCAE0();
		delete Unknown2C;
		Unknown2C = NULL;
	}
	Unknown30 = NULL;
}





// 0x593830
void UIGroupMemberClass::Unknown_593830(const RefCountPtr<RenderObjClass> &object)
{
	Unknown30 = object;
}





















































// 0x593BD0
UIGroupClass::UIGroupClass(SClient *client) :
	Unknown5BB320Class(new UIMeshClass("TSI03A", true), client),
	Members(0),
	Unknown0E8(NULL),
	Unknown0EC(0),
	Unknown0F0(false),
	Unknown0F1(false)
{
	for (int i = 0; i < 9; i++) {
		Unknown0C0[i] = NULL;
	}
	((FontStringFilesClass *)Unknown_BE67BC)->Load("FA_TSI03A.INI");
	Unknown_BE67C4->Add(Mesh->Peek_Mesh_Object());
}

// 0x593D70
UIGroupClass::~UIGroupClass()
{
	Unknown_5C();
	for (int i = 0; i < 9; i++) {
		if (Unknown0C0[i]) {
			Unknown_65EED0(Unknown0C0[i]);
			Unknown0C0[i] = NULL;
		}
	}
	SymbolChatUI = NULL;
	for (std::vector<UIGroupMemberClass *>::iterator it = Members.begin(); it != Members.end(); it++) {
		delete *it;
	}
	Members.clear();
	Unknown_5954C0();
	Unknown_BE67C4->Remove((unsigned int)Mesh->Peek_Mesh_Object());
	((FontStringFilesClass *)Unknown_BE67BC)->Unload("FA_TSI03A.INI");
	delete Unknown0E8;
	Unknown0E8 = NULL;
}



































// 0x5944A0
RefCountPtr<RenderObjClass> UIGroupUnknownE8Class::Unknown_5944A0()
{
	return(Unknown04);
}



// 0x5944D0
// NOT EXACT: order (the client stores +0xEC = 0 before it reads +0xC0; '0.0f' and a named gadget local tried)
void UIGroupClass::Unknown_60()
{
	Unknown_596770(false);
	Unknown0E8->Unknown_592A80();
	Unknown_5966A0();
	Unknown_596DB0();
	Unknown_597650();
	if (Unknown0EC > 0.0f) {
		Unknown0EC -= (Unknown_BFAE3C - Unknown_BFAE40) * 0.001f;
		if (Unknown0EC < 0.0f) {
			Unknown0EC = 0;
			Unknown0C0[0]->Unknown2C800 = false;
			Unknown0C0[0]->Unknown_6607D0();
			Unknown5BB320Class::Unknown_60();
			return;
		}
		Unknown0C0[0]->Unknown_6603A0(0, NULL);
	}
	Unknown5BB320Class::Unknown_60();
}










































































































































































































































































































// 0x595480
void UIGroupClass::Unknown_7C(UIViewClass *view, int arg)
{
	if (View != view) {
		Unknown5BB320Class::Unknown_7C(view, arg);
		SymbolChatUI->Unknown_7C(view, 0);
	}
}


// 0x5954C0
void UIGroupClass::Unknown_5954C0()
{
	char name[32];
	for (int i = 0; i < 5; i++) {
		_snprintf(name, sizeof(name), "NAME%02d_TSI03A", i);
		Unknown_BE67C4->Get_Style(name, (unsigned int)Mesh->Peek_Mesh_Object())->Set_Hidden(1);
	}
}





































































































// 0x596040
int UIGroupClass::Unknown_596040(int id)
{
	for (unsigned int i = 0; i < Members.size(); i++) {
		if (Members[i]->ID == id) {
			return(i);
		}
	}
	return(-1);
}




// 0x596090
std::vector<UIGroupMemberClass *>::iterator UIGroupClass::Get_Member(const std::string &name)
{
	std::vector<UIGroupMemberClass *>::iterator it;
	for (it = Members.begin(); it != Members.end(); it++) {
		if ((*it)->Name == name) {
			break;
		}
	}
	return(it);
}















// 0x596130
// NOT EXACT: frame (the client keeps the new member in name's argument slot and the event in id's; ours the other way round)
void UIGroupClass::Add_Member(const std::string &name, int id, int unknown)
{
	UIGroupMemberClass *member;
	FAIL_IF(Get_Member(name) != Members.end()) return;


	FAIL_IF(Members.size() >= MAX_VISIBLE_GROUP_MEMBERS) return;
	if (Members.size() == 0) {
		Unknown_597BF0();
	}
	member = new UIGroupMemberClass(this);
	member->Name = name;
	member->ID = id;
	member->Unknown18 = unknown;
	Members.push_back(member);
	Unknown_597820(id);
}






// 0x596390
void UIGroupClass::Remove_Member(const std::string &name)
{
	Remove_Member(Get_Member(name));
}





// 0x596560
std::vector<UIGroupMemberClass *>::iterator UIGroupClass::Remove_Member(const std::vector<UIGroupMemberClass *>::iterator &it)
{
	FAIL_IF(it == Members.end()) return(it);
	int id = (*it)->ID;
	delete *it;
	std::vector<UIGroupMemberClass *>::iterator next = Members.erase(it);
	Unknown_597820(id);
	if (Members.size() == 0) {
		Unknown_597D10();
	}
	return(next);
}


// 0x5966A0
void UIGroupClass::Unknown_5966A0()
{
	int changed = false;
	std::vector<UIGroupMemberClass *>::iterator it;
	for (it = Members.begin(); it != Members.end() && !changed; it++) {
		changed = (*it)->Unknown_593750();
	}
	if (changed) {
		Unknown_595530();
	}
	for (it = Members.begin(); it != Members.end(); it++) {
		(*it)->Unknown_5935C0();
	}
}







































































































































// 0x596DB0
void UIGroupClass::Unknown_596DB0()
{
	if (SymbolChatUI->Active) {
		char text[32];
		bool leader = false;
		if (SymbolChatUI->Unknown_64B2F0(text, &leader)) {
			Unknown_596E20(text, leader);
			SymbolChatUI->Unknown_5C();
		}
	}
}










// 0x596E20
// NOT EXACT: register (the format's and the packet's arguments in other registers; '? 1 : 0' and '(int)' for the flag tried)
void UIGroupClass::Unknown_596E20(const char *text, bool leader)
{
	if (!Unknown0F1) {
		ClientObjectClass *avatar = ((ClientClass *)Client)->Get_Avatar();
		FAIL_IF(!avatar) return;
		NameClass owner("Owner"); DeltaStringClass *d_name = Unknown_Find_Field<DeltaStringClass>((DeltaCompoundClass *)avatar->AuxData, owner);
		if (d_name && !d_name->Unknown70) d_name = NULL;
		FAIL_IF(!d_name) return;
		const char *name = d_name->String;
		const char *space = strchr(name, ' ');
		if (space) {
			name = space + 1;
		}
		char message[128];
		_snprintf(message, sizeof(message), "<S>%d%s`%s", leader ? 1 : 0, name, text);
		((ClientClass *)Client)->Send_Packet(&ClientChatPacketClass(((ClientClass *)Client)->AvatarID, message, 1, NULL));
	}
}


















// 0x597000
void UIGroupClass::Unknown_597000(bool on)
{
	for (std::vector<UIGroupMemberClass *>::iterator it = Members.begin(); it != Members.end(); it++) {
		UIGroupMemberClass *member = *it;
		member->Unknown40->Unknown_40(on);
		member->Unknown44->Unknown_40(on);
		member->Unknown48->Unknown_40(on);
		member->Unknown4C->Unknown_40(on);
	}
}

// 0x597080
void UIGroupClass::Unknown_597080(const char *text, const char *name, int index)
{
	if (text && name && index >= 0) {
		int i = 0;
		for (std::vector<UIGroupMemberClass *>::iterator it = Members.begin(); it != Members.end(); it++, i++) {
			UIGroupMemberClass *member = *it;
			if (member->Name == name) {
				InterfaceClass::Unknown_5C();
				member->Unknown1D = (index != 0);
				member->Unknown_5938A0(text, 5.0f, i);
				if (member->Unknown3C) {
					member->Unknown3C->Unknown_28(&Layer);
					Unknown_5BB4F0(member->Unknown3C);
				}
				InterfaceClass::Unknown_58();
				SoundLibraryClass::Play_2D_Sound("symbol_chat_1.wav", 2);
				return;
			}
		}
	}
}



// 0x597200
int UIGroupClass::Unknown_597200(RefCountPtr<RenderObjClass> model)
{
	if (model == NULL) {
		return(0);
	}
	int level = 0;
	RefCountPtr<RenderObjClass> main = Create_Get(model->Get_Sub_Object_By_Name("main"));
	if (main != NULL) {
		level = main->Get_Sort_Level();
	}
	return(level);
}


// 0x597310
// NOT EXACT: register (model and the sub-object temporary in each other's registers; the level offsets as two adds in the client)
void UIGroupClass::Unknown_597310(RefCountPtr<RenderObjClass> model, bool raise)
{
	if (model == NULL) {
		return;
	}
	int level = Unknown_597200(model);
	RefCountPtr<RenderObjClass> sub = Create_Get(model->Get_Sub_Object_By_Name("main"));
	const char **name;
	for (name = Unknown_B7F4B8; *name; name++) {
		sub = Create_Get(model->Get_Sub_Object_By_Name(*name));
		if (sub != NULL) {
			sub->Set_Sort_Level(level + (raise ? 2 : -2));
		}
	}
	for (name = Unknown_B7F4C4; *name; name++) {
		sub = Create_Get(model->Get_Sub_Object_By_Name(*name));
		if (sub != NULL) {
			sub->Set_Sort_Level(level + (raise ? 1 : -1));
		}
	}
}




















// 0x5975D0
void UIGroupClass::Unknown_5975D0()
{
	if (((SClient *)((ClientClass *)Client)->Unknown_18())->Unknown_74D690()) {
		View->Unknown34->Unknown_588A70("GROUP_LEAVE", 0, "Are you sure you want to disband this group?", 320, 0, 0);
	} else {
		View->Unknown34->Unknown_588A70("GROUP_LEAVE", 0, "Are you sure you want to leave this group?", 320, 0, 0);
	}
}




// 0x597650
// NOT EXACT: register (the client tests View->Unknown34's Active in cl and reads the pointer again for 0x5890B0; Get_Unknown34() / Is_Active() tried)
void UIGroupClass::Unknown_597650()
{
	if (!View->Get_Unknown34()->Is_Active()) return;
	int button = View->Get_Unknown34()->Unknown_5890B0("GROUP_INVITE");
	if (button) {
		if (button == 1) {
			((ClientClass *)Client)->Send_Packet(&ActionPacketClass(((ClientClass *)Client)->AvatarID, 11, ((ClientClass *)Client)->AvatarID, 0));
		} else if (button == 2) {
			((ClientClass *)Client)->Send_Packet(&ActionPacketClass(((ClientClass *)Client)->AvatarID, 12, ((ClientClass *)Client)->AvatarID, 0));
		}
		View->Unknown34->Unknown_5C();
	}
	button = View->Get_Unknown34()->Unknown_5890B0("GROUP_LEAVE");
	if (button) {
		if (button == 1) {
			if (((SClient *)((ClientClass *)Client)->Unknown_18())->Unknown_74D690()) {
				((ClientClass *)Client)->Send_Packet(&ActionPacketClass(((ClientClass *)Client)->AvatarID, 13, ((ClientClass *)Client)->AvatarID, 0));
			} else {
				((ClientClass *)Client)->Send_Packet(&ActionPacketClass(((ClientClass *)Client)->AvatarID, 14, ((ClientClass *)Client)->AvatarID, 0));
			}
			SoundLibraryClass::Play_2D_Sound("fontclick.wav", 2);
		}
		View->Unknown34->Unknown_5C();
	}
}















// 0x597820
void UIGroupClass::Unknown_597820(unsigned int id)
{
	Unknown_597920(id, true);
}

// 0x5978A0
void UIGroupClass::Unknown_5978A0(unsigned int id)
{
	Unknown_597920(id, true);
}








// 0x597920
// NOT EXACT: register / stack slot (the client builds the event in id's argument slot, ours in value's)
void UIGroupClass::Unknown_597920(unsigned int id, bool value)
{
	ClientClass *client = (ClientClass *)Client;
	FAIL_IF(!client) return;
	ClientObjectClass *object = client->Find_Object(id);
	if (object) {
		Unknown_4F7EF0().emit(object, ClientGroupEvent(value));
	}
}






// 0x5979A0
bool UIGroupClass::Unknown_5979A0()
{
	SClient *sclient = (SClient *)((ClientClass *)Client)->Unknown_18();
	DeltaCompoundClass *aux = sclient->AuxData;
	if (aux) {
		DeltaGroupInfoClass *group = (DeltaGroupInfoClass *)Unknown_Find_Field<DeltaCompoundClass>(aux, "GroupInfo");
		if (group && group->Unknown70) {
			return(stricmp(group->FormationName.String, "") ? true : false);
		}
	}
	return(false);
}





// 0x597A60
bool UIGroupClass::Unknown_597A60()
{
	SClient *sclient = (SClient *)((ClientClass *)Client)->Unknown_18();
	DeltaCompoundClass *aux = sclient->AuxData;
	if (aux) {
		DeltaGroupInfoClass *group = (DeltaGroupInfoClass *)Unknown_Find_Field<DeltaCompoundClass>(aux, "GroupInfo");
		if (group && group->Unknown70) {
			return(group->Formation.Value ? true : false);
		}
	}
	return(false);
}











// 0x597B00
bool UIGroupClass::Unknown_597B00()
{
	ClientClass *client = (ClientClass *)Client;
	FAIL_IF(!client) return(false);
	SClient *sclient = (SClient *)client->Unknown_18();

	FAIL_IF(!sclient) return(false);
	return(sclient->Unknown_74D690());
}



// 0x597B70
void UIGroupClass::Unknown_597B70(bool value)
{
	ClientClass *client = (ClientClass *)Client;
	FAIL_IF(!client) return;
	SClient *sclient = (SClient *)client->Unknown_18();

	FAIL_IF(!sclient) return;
	sclient->Unknown13C0 = value;
}









// 0x597BF0
void UIGroupClass::Unknown_597BF0()
{
	Unknown0E4->Set_Hidden(0);
	Unknown_598370();
	for (int i = 1; i <= 3; i++) {
		if (Unknown0F1) {
			if (i == 3) {
				Unknown0C0[2]->Unknown_40(true);
			} else {
				Unknown0C0[i - 1]->Unknown_40(false);
			}
		} else {
			Unknown0C0[i - 1]->Unknown_40(true);
		}
		Unknown0C0[i - 1]->Unknown_8C(false);
	}
	if (Unknown0C0[2] && ((SClient *)((ClientClass *)Client)->Unknown_18())->Unknown_74D690()) {
		Unknown_65EFB0(Unknown0C0[2], "UI_FORMATION", "GRP_DISBAND");
		Unknown_597E40(0);
		return;
	}
	Unknown_65EFB0(Unknown0C0[2], "UI_FORMATION", "GRP_LEAVE");
	Unknown_597E40(0);
}






// 0x597D10
void UIGroupClass::Unknown_597D10()
{
	FAIL_IF(!SymbolChatUI) return;
	if (SymbolChatUI->Active) {
		SymbolChatUI->Unknown_5C();
	}
	Unknown_598370();
	for (int i = 0; i < 3; i++) {
		Unknown0C0[i]->Unknown_40(false);
		Unknown0C0[i]->Unknown_8C(true);
	}
	if (Unknown0C0[0]) {
		Unknown0C0[0]->Unknown2C800 = false;
		Unknown0C0[0]->Unknown_6607D0();
	}
	Unknown0EC = 0;
	Unknown0E4->Set_Hidden(1);
	Unknown_597E40(0);
	Unknown_597B70(false);
}









































































// 0x5981B0
void UIGroupClass::Unknown_5981B0()
{
	if (!Unknown0F1) {
		if (Unknown0C0[0]) {
			Unknown0C0[0]->Unknown2C800 = false;
			Unknown0C0[0]->Unknown_6607D0();
		}
		Unknown0EC = 0;
		Unknown_597E40(0);
	}
}

// 0x598200
void UIGroupClass::Unknown_598200(bool on)
{
	if (!Unknown0F1) {
		if (Unknown0C0[0]) {
			Unknown0C0[0]->Unknown2C800 = on;
			if (on) {
				Unknown0C0[0]->Unknown_6603A0(0, NULL);
				Unknown0EC = 15.0f;
			} else {
				Unknown0C0[0]->Unknown_6607D0();
				Unknown0EC = 0;
				return;
			}
		}
		if (on) {
			Unknown_597E40(0);
		}
	}
}


// 0x5982A0
void UIGroupClass::Unknown_5982A0(int avatar_id)
{
	if (!Unknown0F1) {
		int index = Unknown_596040(avatar_id);
		if (index != -1) {
			if (index < (int)Members.size()) {
				UIGroupMemberClass *member = Members[index];
				if (member && member->Unknown48) {
					member->Unknown48->Unknown2C800 = true;
					member->Unknown48->Unknown_6603A0(0, NULL);
					member->Unknown20 = 15.0f;
				}
			}
		}
	}
}































// 0x5985A0
void UIGroupClass::Unknown_70(ControlKeysManagerClass *manager)
{
	FAIL_IF(!manager) return;
	if (!Unknown0F1) {
		((ControlKeysManager *)manager)->Add_Keys((ControlKeyListClass *)Unknown4C, std::string("Formation"), (void *)47, 0);
	}
}



// 0x598720
void UIGroupClass::Unknown_74(ControlKeysManagerClass *manager)
{
	FAIL_IF(!manager) return;
	if (!Unknown0F1) {
		((ControlKeysManager *)manager)->Remove_Keys((ControlKeyListClass *)Unknown4C, std::string("Formation"), 0);
	}
}

/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Client\ui_group.cpp (its __FILE__)
**
**	The group window (ui_group.h's UIGroupClass, the main view's +0x1D8): the group's members (UIGroupMemberClass, one
**	0x54-byte object each, in Members), nine formation buttons, the symbol chat interface (+0xAC, FAIL_IF
**	'!SymbolChatUI') and the NAMEnn_TSI03A texts of FA_TSI03A.INI over the TSI03A mesh. The client's window runs
**	0x593180-0x598A60 (claims #27 / #28); the base under it (constructor 0x5BB320) has no file name yet.
**
**	Rebuilt (UG1, 2026-10-30): the member class's constructor 0x593200, 0x5934A0 (line 403), 0x5935C0 (456), 0x593750, 0x593790, 0x593830 and its
**	destructor 0x5932E0 (not exact), the constructor 0x593BD0 and destructor 0x593D70 (with the scalar deleting destructor
**	0x593D40), slot +0x7C 0x595480, 0x5954C0, 0x596040, Get_Member 0x596090, Add_Member 0x596130 (1165, 1168; not exact), Remove_Member 0x596390 / 0x596560 (1198), 0x5966A0, 0x596DB0, slot +0x60 0x5944D0 (not exact), the group chat line 0x596E20 (1388, 1391; not exact), 0x597000, the +0xE8 object's 0x5944A0, the symbol chat line 0x597080, 0x597200, 0x597310 (not exact), the leave question 0x5975D0, the dialog answers 0x597650 (not exact), the ClientGroupEvent emitters 0x597820 / 0x5978A0
**	/ 0x597920 (line 1594), the GroupInfo tests 0x5979A0 / 0x597A60, 0x597B00 (1652, 1655), 0x597B70 (1665, 1668), 0x597BF0, 0x597D10 (1714), 0x5981B0, 0x598200,
**	0x5982A0 and the control keys 0x5985A0 (1891) / 0x598720 (1902). The asserts sit on the client's lines (D25);
**	the functions without asserts sit between their neighbours' lines as the client orders them. Not yet: 0x593180,
**	0x5938A0 (563), Unknown_54 0x594040, slot +0x68 0x5946D0 (735, 751), 0x595530, 0x596770 (1260-1266),
**	0x597E40 (1753) and 0x598370.
**
**	Forms: the constructor zeroes the nine buttons with a loop (VC6 makes it the rep stosd; memset puts the lea
**	after the count); the members are initialised in the list, Members with a count of 0 (the new(0)); 0x597B00
**	ends in a jump to SClient::Unknown_74D690 (Ghidra shows the callee's body inline); 0x5979A0 / 0x597A60 test
**	the aux root through a named local before the NameClass and return '? true : false' (setne); 0x598200 tests on before off
**	and reads +0xC0 again for the 0x6603A0 call. intrface.h's slots +0x70 / +0x74 take a ControlKeysManagerClass,
**	control_keys_manager.h's class is the client's ControlKeysManager: the casts are ours.
*/
