// Earth & Beyond client (reconstruction): app\client\ui_friends.cpp (the client's __FILE__
// 'e:\build\beta\G\app\Client\ui_friends.cpp', 0x58F090-0x5928AF by its asserts and its string order).
// Every function from 0x58F0F0 written (FR1, FR2: the constructor, the destructor, slots +0x54 / +0x68, 0x591C30); the near misses carry NOT EXACT.
#define __INTRFACE_MEMBERS	// intrface.h's members (0x591EE0 calls InterfaceClass's 0x541930)
#include <string.h>
#include <wchar.h>
#include <string>
#include <vector>
#include <list>
#include "ui_friends.h"
#include "ui_group.h"
#include "gadget.h"
#include "gd_slider.h"
#include "fontmanager.h"
#include "client.h"
#include "..\input\input.h"
#include "..\packets\pkclientchat.h"
#include "co.h"
#include "gd_robj.h"
#include "local_interface_sort.h"
#include "option.h"
#include "..\audio\soundlib.h"
#include "..\shared\g_assert.h"

// The client at InterfaceClass +0x40, as ui_chat_panel.cpp declares it
class LineDisplayClientClass  // NAME 1: neutral (D10)
{
public:
	virtual void Unknown_00(); virtual void Unknown_04(); virtual void Unknown_08(); virtual void Unknown_0C();
	virtual void Unknown_10(); virtual void Unknown_14();
	virtual SClient *As_Space_Client();					// +0x18 (SClient's returns itself)  // NAME 3: ui_manu.cpp's FAIL_IF's
};

// 0x58F0F0
UIFriendsPlayerInfoClass::UIFriendsPlayerInfoClass(std::string name, std::string location)
{
	Name = name;
	Location = location;
}

// One player deleted (inline)
static inline void Unknown_Delete_Player(UIFriendsPlayerInfoClass *info)	// NAME 1 [Unknown_Delete_Player, info]: neutral
{
	delete info;
}

// ui_manu.cpp's check button, as far as this window uses it (0x68 bytes, table 0xB075E4)
class UIManuCheckButtonClass : public GadgetClass	// NAME 1: neutral (D10), as ui_manu.cpp's
{
public:
	void Unknown_7E8030(bool on);						// 0x7E8030: checked or not  // NAME 1 [on]: neutral

	char Unknown30[0x64 - 0x30];						// +0x30 (GadgetClass is 0x30 bytes)
	bool Checked;										// +0x64  // NAME 2: what 0x7E8030 sets
};

extern FontManagerClass *Unknown_BE67C4;				// NAME 1 [Unknown_BE67C4]: 0xBE67C4

// The buttons' models and tooltips (0xB7F268)
static const char *Unknown_B7F268[] = { "exit", "add", "remove", "add_target", "send_message", "name", "status", "add_group" };	// NAME 1: neutral (D10)

// 0x58F210
UIFriendsClass::UIFriendsClass(SClient *client) :
	ChatPanelUIBase(client),
	Slider(NULL),
	Top(0),
	Selection(-1),
	Unknown0AC(true),
	Unknown0AD(false),
	Unknown0AE(false),
	PlayersInfo(0),
	Unknown0C4(0)
{
	memset(Unknown0D0, 0, sizeof(Unknown0D0));
	memset(Unknown0F0, 0, sizeof(Unknown0F0));
	memset(Unknown10C, 0, sizeof(Unknown10C));
	for (int i = 0; i < 7; i++) {
		Unknown114[i] = NULL;
		Unknown130[i] = NULL;
	}
	if (OptionClass::Instance) {
		Unknown0AD = OptionClass::Instance->Get_Bool("RestrictedStatus");
	}
}

// NOT EXACT: inline budget (explain.py): ours inlines 0x5906C0 (the players' ??_G) inside the inlined slot +0x5C; the client calls 0x591030 / 0x5906C0 / 0x590810 out of line
// 0x58F450
UIFriendsClass::~UIFriendsClass()
{
	Unknown0B0->Unknown_5C();
	Unknown0B0 = NULL;
	Unknown_5C();
	if (Slider) {
		Slider->Unknown_66E460(RefCountPtr<RenderObjClass>(), 0, 0);
	}
	OptionClass *options = OptionClass::Instance;
	if (options) {
		BoolOptionStruct *option = options->Find_Bool("RestrictedStatus");
		if (option && option->Value != Unknown0AD) {
			option->Value = Unknown0AD;
			options->Save();
		}
	}
}

// 0x58F6B0
void UIFriendsClass::Unknown_54()
{
	if (Unknown_98("tsi28a", "fa_tsi28a.ini")) {
		int i;
		for (i = 0; i < 8; i++) {
			Unknown0D0[i] = Unknown_56D510(i + 1, Unknown_B7F268[i], NULL);
			GadgetClass::Set_Tooltip(Unknown0D0[i], "UI_FRIENDS", Unknown_B7F268[i]);
		}
		char name[0x20];
		for (i = 0; i < 7; i++) {
			_snprintf(name, sizeof(name), "name%02d", i);
			Unknown0F0[i] = Unknown_56D510(i + 0xD, name, NULL);
			Unknown0F0[i]->Unknown2C_0A = true;
			_snprintf(name, sizeof(name), "name%02d_TSI28A", i);
			Unknown114[i] = Create_Peek(Unknown_BE67C4->Get_Style(name, Peek_Unknown84()));
			_snprintf(name, sizeof(name), "LOCATION%02d_TSI28A", i);
			Unknown130[i] = Create_Peek(Unknown_BE67C4->Get_Style(name, Peek_Unknown84()));
		}
		for (i = 0; i < 2; i++) {
			_snprintf(name, sizeof(name), "check%02d", i);
			Unknown10C[i] = Unknown_56D6F0(i + 10, name, NULL);
		}
		Unknown10C[0]->Unknown_7E8030(!Unknown0AD);
		Unknown10C[1]->Unknown_7E8030(Unknown0AD);
		Slider = Unknown_56D8D0(0x15, 0);
		Slider->Unknown_66E460(*(const RefCountPtr<RenderObjClass> *)&Unknown84, 0, 0);
		Unknown0B0 = Create_Get(new UIPTradeCreditsClass(Client->As_Space_Client(), 1, 0x14));
		Unknown0B0->Unknown_54();
		Unknown0B0->Unknown0CD = true;
		Unknown0B0->Unknown_64A270("Friend Player");
		GadgetClass::Set_Tooltip(Unknown0D0[1], "UI_FRIEND", "ADD_PLAYER");
		GadgetClass::Set_Tooltip(Unknown0D0[2], "UI_FRIEND", "REMOVE_PLAYER");
		GadgetClass::Set_Tooltip(Unknown0D0[3], "UI_FRIEND", "ADD_TARGET");
		GadgetClass::Set_Tooltip(Unknown0D0[4], "UI_FRIEND", "CHAT_WITH");
		GadgetClass::Set_Tooltip(Unknown0D0[7], "UI_FRIEND", "ADD_GROUP");
		Unknown_56D970(1, Vector3(0.0f, 0.0f, 0.0f));
	}
}

// NOT EXACT: inline budget (the client calls the vector's erase, the list's begin and erase out of line and expands the list's ??_G; ours the reverse)
// 0x58FBA0
void UIFriendsClass::Unknown_58()
{
	if (!Is_Active()) {
		InterfaceSortScopeClass sort(this);
		ChatPanelUIBase::Unknown_58();
		std::vector<UIFriendsPlayerInfoClass *>::iterator it;
		for (it = PlayersInfo.begin(); it != PlayersInfo.end(); it++) {
			delete *it;
		}
		PlayersInfo.erase(PlayersInfo.begin(), PlayersInfo.end());
		Unknown_590810();
		Top = -1;
		Unknown_590480(0);
		int id = ((ClientClass *)Client)->Unknown1138;
	ClientChatRequestPacketClass packet(0x18, id);
		((ClientClass *)Client)->Send_Packet(&packet);
		Unknown_591030(true);
		if (Unknown0D0[0]) {
			((RObjGadgetClass *)Unknown0D0[0])->Unknown_66C360();
		}
	}
}

// 0x58FDD0
void UIFriendsClass::Unknown_5C()
{
	if (Active) {
		Unknown_591030(false);
		Unknown_5906C0();
		Unknown_590810();
		InterfaceClass::Unknown_5C();
	}
}

// 0x58FF00
void UIFriendsClass::Unknown_60()
{
	if (Unknown0AE) {
		Unknown0AE = false;
		Unknown_591350();
	}
	Unknown_590500();
	Unknown_590C70();
	Unknown_590E50();
	SClient *sclient = Client->As_Space_Client();
	SClient *space = Client->As_Space_Client();
	ClientObjectClass *target;
	if (!space->Unknown_744B60() && space && space->MainView && (target = space->MainView->Unknown_5C()) != NULL && target->Unknown_6F5A40()) {
		Unknown0D0[3]->Unknown_40(true);
	} else {
		Unknown0D0[3]->Unknown_40(false);
	}
	if (sclient->Unknown_74D570()) {
		Unknown0D0[7]->Unknown_40(true);
	} else {
		Unknown0D0[7]->Unknown_40(false);
	}
}

// 0x590020
bool UIFriendsClass::Unknown_68(int key)
{
	switch (key) {
		case 1:
			SoundLibraryClass::Play_2D_Sound("Roll_Select02b.wav", 2);
			Unknown_5C();
			Unknown98 = 1;
			return(true);
		case 2:
			SoundLibraryClass::Play_2D_Sound("Roll_Select02b.wav", 2);
			Unknown0B0->Unknown_58();
			Unknown0B0->Unknown_64A2B0("");
			return(true);
		case 3:
			SoundLibraryClass::Play_2D_Sound("Roll_Select02b.wav", 2);
			Unknown_5910D0();
			return(true);
		case 4:
			SoundLibraryClass::Play_2D_Sound("Roll_Select02b.wav", 2);
			Unknown_591A30();
			return(true);
		case 5:
			SoundLibraryClass::Play_2D_Sound("Roll_Select02b.wav", 2);
			Unknown_591EE0();
			return(true);
		case 10:
			SoundLibraryClass::Play_2D_Sound("fontclick.wav", 2);
			Unknown10C[0]->Unknown_7E8030(true);
			Unknown10C[1]->Unknown_7E8030(false);
			if (Unknown0AD) {
				Unknown0AD = false;
				int id = ((ClientClass *)Client)->Unknown1138; ((ClientClass *)Client)->Send_Packet(&ClientChatRequestPacketClass(0x1D, id));
			}
			return(true);
		case 11:
			SoundLibraryClass::Play_2D_Sound("fontclick.wav", 2);
			Unknown10C[0]->Unknown_7E8030(false);
			Unknown10C[1]->Unknown_7E8030(true);
			if (!Unknown0AD) {
				Unknown0AD = true;
				int id = ((ClientClass *)Client)->Unknown1138; ((ClientClass *)Client)->Send_Packet(&ClientChatRequestPacketClass(0x1C, id));
			}
			return(true);
		case 6:
			SoundLibraryClass::Play_2D_Sound("fontclick.wav", 2);
			Unknown0AC = false;
			Unknown_5908B0();
			Unknown_590550();
			return(true);
		case 7:
			SoundLibraryClass::Play_2D_Sound("fontclick.wav", 2);
			Unknown0AC = true;
			Unknown_5908B0();
			Unknown_590550();
			return(true);
		case 8:
			SoundLibraryClass::Play_2D_Sound("fontclick.wav", 2);
			Unknown_591C30();
			return(true);
		case 13:
		case 14:
		case 15:
		case 16:
		case 17:
		case 18:
		case 19:
			SoundLibraryClass::Play_2D_Sound("fontclick.wav", 2);
			Unknown_590DB0(key - 13);
			return(true);
	}
	return(false);
}

// 0x590430
void UIFriendsClass::Unknown_7C(UIViewClass *view, bool arg)
{
	if (View != view) {
		if (Unknown0B0) {
			Unknown0B0->Unknown_7C(view, false);
		}
		Unknown0AE = true;
		InterfaceClass::Unknown_7C(view, arg);
	}
}

// 0x590480
void UIFriendsClass::Unknown_590480(int count)
{
	Slider->Unknown_66FA90((float)count, 7.0f, true);
	Unknown_590500();
}

// 0x590500
void UIFriendsClass::Unknown_590500()
{
	int top = (int)Slider->Value;
	if (Top != top) {
		Top = top;
		Unknown_590550();
	}
	Slider->Unknown_66FD80(false);
}

// 0x590550
void UIFriendsClass::Unknown_590550()
{
	for (int i = 0; i < 7; i++) {
		unsigned int index = Top + i;
		if (index < PlayersInfo.size()) {
			UIFriendsPlayerInfoClass *info = PlayersInfo[index];
			Unknown114[i]->Change_Text(info->Name.c_str(), 0);
			Unknown130[i]->Change_Text(info->Location.c_str(), 0);
			Unknown0F0[i]->Unknown_40(true);
			if (stricmp(info->Location.c_str(), "offline") == 0) {
				Unknown114[i]->Unknown_6D8970(false);
				Unknown114[i]->Unknown_Set_2FC(0.5f);
				Unknown130[i]->Unknown_Set_2FC(0.5f);
			} else {
				Unknown114[i]->Unknown_6D8970(true);
				Unknown114[i]->Unknown_Set_2FC(1.0f);
				Unknown130[i]->Unknown_Set_2FC(1.0f);
			}
		} else {
			Unknown114[i]->Change_Text(" ", 0);
			Unknown130[i]->Change_Text(" ", 0);
			Unknown0F0[i]->Unknown_40(false);
		}
	}
}

// 0x5906C0
void UIFriendsClass::Unknown_5906C0()
{
	std::vector<UIFriendsPlayerInfoClass *>::iterator it;
	for (it = PlayersInfo.begin(); it != PlayersInfo.end(); it++) {
		Unknown_Delete_Player(*it);
	}
	PlayersInfo.erase(PlayersInfo.begin(), PlayersInfo.end());
}

// 0x590810
void UIFriendsClass::Unknown_590810()
{
	std::list<UIFriendsPlayerInfoClass *>::iterator it;
	for (it = Unknown0C4.begin(); it != Unknown0C4.end(); it++) {
		Unknown_Delete_Player(*it);
	}
	Unknown0C4.clear();
}

// NOT EXACT: register allocation (the client keeps 0 in ebx and the insert position in a stack slot; ours holds the position in ebx; frame 0x18 vs 0x14)
// 0x5908B0
void UIFriendsClass::Unknown_5908B0()
{
	Unknown_5906C0();

	for (std::list<UIFriendsPlayerInfoClass *>::iterator node = Unknown0C4.begin(); node != Unknown0C4.end(); node++) {
		UIFriendsPlayerInfoClass *info = *node;
		std::vector<UIFriendsPlayerInfoClass *>::iterator pos;
		for (pos = PlayersInfo.begin(); pos != PlayersInfo.end(); pos++) {
			UIFriendsPlayerInfoClass *other = *pos;
			if (Unknown0AC) {
				if (stricmp(info->Location.c_str(), "offline") == 0 && stricmp(other->Location.c_str(), "offline") != 0) {
					continue;
				}
				int result = wcscoll((const wchar_t *)info->Location.c_str(), (const wchar_t *)other->Location.c_str());
				if (result < 0) {
					break;
				}
				if (result == 0 && wcscoll((const wchar_t *)info->Name.c_str(), (const wchar_t *)other->Name.c_str()) < 0) {
					break;
				}
			} else {
				if (wcscoll((const wchar_t *)info->Name.c_str(), (const wchar_t *)other->Name.c_str()) < 0) {
					break;
				}
			}
		}
		PlayersInfo.insert(pos, new UIFriendsPlayerInfoClass(info->Name, info->Location));
	}

	Top = -1;
	Unknown_590480(PlayersInfo.size());
	Selection = -1;
	Unknown0D0[2]->Unknown_40(false);
	Unknown0D0[4]->Unknown_40(false);
}

// 0x590C70
void UIFriendsClass::Unknown_590C70()
{
	float x;
	float y;
	Unknown_42ECF0(&x, &y);
	for (int i = 0; i < 7; i++) {
		int selected = (Top + i == Selection);
		if (Unknown0F0[i]->Unknown_30(x, y) || selected) {
			Unknown114[i]->Unknown_6CE910(true);
			Unknown114[i]->Unknown_Set_2FC(1.0f);
			if (selected) {
				Unknown130[i]->Unknown_6CE910(true);
				Unknown130[i]->Unknown_Set_2FC(1.0f);
			}
		} else {
			Unknown114[i]->Unknown_6CE910(false);
			Unknown130[i]->Unknown_6CE910(false);
			if (stricmp(Unknown130[i]->Unknown_6D87D0(), "offline") == 0) {
				Unknown114[i]->Unknown_Set_2FC(0.5f);
				Unknown130[i]->Unknown_Set_2FC(0.5f);
			}
		}
	}
}


// 0x590DB0
void UIFriendsClass::Unknown_590DB0(int index)
{
	if (index >= 0 && index < 7) {
		int size = PlayersInfo.size();
		if (Top + index < size) {
			int selection = Top + index;
			if (Selection == selection && Unknown0F0[index]->Is_Unknown2C_00())
				Unknown_591EE0();
			else {
				Selection = selection;
				Unknown0D0[2]->Unknown_40(true);
				Unknown0D0[4]->Unknown_40(true);
			}
		}
	}
}


// 0x590E50
void UIFriendsClass::Unknown_590E50()
{
	if (Unknown0B0.Peek() != NULL && Unknown0B0->Is_Active()) {
		int state = Unknown0B0->Unknown_64A220();
		if (state) {
			int length;
			if (state == 2 && (length = strlen(Unknown0B0->Unknown_64A240())) != 0) {
				int id = ((ClientClass *)Client)->Unknown1138;
	ClientChatRequestPacketClass packet(8, id);
				packet.Unknown14 = std::string(Unknown0B0->Unknown_64A240());
				((ClientClass *)Client)->Send_Packet(&packet);
			}
			Unknown0B0->Unknown_5C();
		}
	}
}

// 0x591030
void UIFriendsClass::Unknown_591030(bool on)
{
	int i;
	for (i = 0; i < 8; i++) {
		if (Unknown0D0[i]) {
			Unknown0D0[i]->Unknown_40(on);
		}
	}
	for (i = 0; i < 7; i++) {
		if (Unknown0F0[i]) {
			Unknown0F0[i]->Unknown_40(on);
		}
	}
	if (on && Selection < 0) {
		Unknown0D0[2]->Unknown_40(false);
		Unknown0D0[4]->Unknown_40(false);
	}
}







































































































// 0x5910D0
void UIFriendsClass::Unknown_5910D0()
{
	int list_size = PlayersInfo.size();
	FAIL_IF(Selection < 0 || Selection >= list_size) return;
	UIFriendsPlayerInfoClass *info = PlayersInfo[Selection];
	int id = ((ClientClass *)Client)->Unknown1138;
	ClientChatRequestPacketClass packet(9, id);
	packet.Unknown14 = std::string(info->Name.c_str());
	((ClientClass *)Client)->Send_Packet(&packet);
}

// 0x591350
void UIFriendsClass::Unknown_591350()
{
	int id = ((ClientClass *)Client)->Unknown1138;
	ClientChatRequestPacketClass packet(0x18, id);
	((ClientClass *)Client)->Send_Packet(&packet);
}



// NOT EXACT: inline budget and registers (the client keeps the list iterator's ++ inline and calls the players' ??_G and list::clear out of line; names' first / last in edi / ebx)
// 0x5913E0
void UIFriendsClass::Unknown_5913E0(std::vector<std::string> names, std::vector<std::string> locations)
{
	int player_list_size = names.size();
	int location_list_size = locations.size();
	FAIL_IF(player_list_size != location_list_size) return;
	Unknown_590810();
	for (int i = 0; i < player_list_size; i++) {
		Unknown0C4.push_back(new UIFriendsPlayerInfoClass(names[i], locations[i]));
	}
	Unknown_5908B0();
	Unknown_590550();
	Unknown_591030(true);
}

// 0x5916B0
void UIFriendsClass::Unknown_5916B0(std::string name, std::string location)
{
	Unknown0C4.push_back(new UIFriendsPlayerInfoClass(name, location));
	Unknown_5908B0();
	Unknown_590550();
}

// 0x591880
void UIFriendsClass::Unknown_591880(std::string name)
{
	for (std::list<UIFriendsPlayerInfoClass *>::iterator it = Unknown0C4.begin(); it != Unknown0C4.end(); it++) {
		UIFriendsPlayerInfoClass *info = *it;
		if (strcmp(info->Name.c_str(), name.c_str()) == 0) {
			delete info;
			Unknown0C4.erase(it);
			break;
		}
	}
	Unknown_5908B0();
	Unknown_590550();
}

// 0x591A30
void UIFriendsClass::Unknown_591A30()
{
	SClient *sclient = Client->As_Space_Client();
	ClientObjectClass *target;
	if (!sclient->Unknown_744B60() && sclient && sclient->MainView && (target = sclient->MainView->Unknown_5C()) != NULL && target->Unknown_6F5A40()) {
		PlayersInfo.size(); PlayersInfo.size();		// dead reads: spend the inline budget so the name's _Tidy(false) stays out of line, as in the client
		std::string name;
		target->Get_Name(name);
		int id = ((ClientClass *)Client)->Unknown1138;
		ClientChatRequestPacketClass packet(8, id);
		packet.Unknown14 = std::string(name.c_str());
		((ClientClass *)Client)->Send_Packet(&packet);
	}
}

#include "..\shared\auxdata\auxdata.h"
#include "aux_roots.h"				// (after auxdata.h: its Type_Name(int *) for kv_pair.h, C2665 without)

// auxdata.h's find through one more inline level, as ui_group.cpp's; the dead size() reads here and in 0x591C30's loop spend the inline budget (D130, not the client's code) so the find 0x5927E0 and the name's _Eos stay out of line as in the client
template<class C> inline C *Unknown_Friends_Find(DeltaCompoundClass *root, const NameClass &name) { root->Children.size(); return(Unknown_Find_Field<C>(root, name)); }  // NAME 1: neutral (D10)

// 0x591C30
void UIFriendsClass::Unknown_591C30()
{
	DeltaCompoundClass *aux = Client->As_Space_Client()->AuxData;
	NameClass field("GroupInfo");
	DeltaGroupInfoClass *group = (DeltaGroupInfoClass *)Unknown_Friends_Find<DeltaCompoundClass>(aux, field);
	if (group && group->Unknown70) {
		for (int i = 0; i < 5; i++) {
			DeltaGroupMemberClass *member = (DeltaGroupMemberClass *)group->Members.Children[i];
			std::string name(member->Name.String);
			PlayersInfo.size();
			if (name.length()) {
				int id = ((ClientClass *)Client)->Unknown1138;
				ClientChatRequestPacketClass packet(8, id);
				packet.Unknown14 = std::string(name.c_str());
				((ClientClass *)Client)->Send_Packet(&packet);
			}
		}
	}
}
























// 0x591EE0
void UIFriendsClass::Unknown_591EE0()
{
	FAIL_IF(Selection < 0 || Selection >= (int)PlayersInfo.size()) return;

	SClient *sclient = Client->As_Space_Client();
	FAIL_IF(!sclient) return;

	ChatPanelInterface *cpanel = sclient->ChatPanel.Peek();
	FAIL_IF(!cpanel) return;


	UIFriendsPlayerInfoClass *friend_info = PlayersInfo[Selection];
	FAIL_IF(!friend_info) return;
	if (strcmp(friend_info->Location.c_str(), "offline") == 0) {
		sclient->Unknown_74D990(0x11, "Friend not online.", 0);
		return;
	}
	cpanel->Unknown_5658B0(friend_info->Name.c_str());
	sclient->Unknown_744E70(true);
	cpanel->Unknown_541930();
}

// 0x592090
void UIFriendsClass::Unknown_592090()
{
	SClient *sclient = Client->As_Space_Client();
	SClient *space = Client->As_Space_Client();
	ClientObjectClass *target;
	if (!space->Unknown_744B60() && space && space->MainView && (target = space->MainView->Unknown_5C()) != NULL && target->Unknown_6F5A40()) {
		Unknown0D0[3]->Unknown_40(true);
	} else {
		Unknown0D0[3]->Unknown_40(false);
	}
	if (sclient->Unknown_74D570()) {
		Unknown0D0[7]->Unknown_40(true);
	} else {
		Unknown0D0[7]->Unknown_40(false);
	}
}

// 0x592150
ClientObjectClass *UIFriendsClass::Unknown_592150()
{
	SClient *sclient = Client->As_Space_Client();
	if (sclient->Unknown_744B60()) {
		return(NULL);
	}
	ClientObjectClass *target = NULL;
	if (sclient && sclient->MainView) {
		target = sclient->MainView->Unknown_5C();
		if (target == NULL || !target->Unknown_6F5A40()) {
			return(NULL);
		}
	}
	return(target);
}
