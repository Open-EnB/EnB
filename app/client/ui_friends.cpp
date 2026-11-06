// Earth & Beyond client (reconstruction): app\client\ui_friends.cpp (the client's __FILE__
// 'e:\build\beta\G\app\Client\ui_friends.cpp', 0x58F090-0x5928AF by its asserts and its string order).
// So far 0x58F0F0, 0x58FBA0-0x591A2F but 0x590020, and 0x591EE0-0x59219F (FR1); not yet: 0x58F210 / 0x58F450 / 0x58F6B0 / 0x590020 / 0x591C30.
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
#include "..\shared\unknown_debug.h"

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

// NOT EXACT: inline budget (the client calls the vector's erase, the list's begin and erase out of line and expands the list's ??_G; ours the reverse)
// 0x58FBA0
void UIFriendsClass::Unknown_58()
{
	if (!Is_Active()) {
		InterfaceSortScopeClass sort(this);
		UIChatPanelBaseClass::Unknown_58();
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

// 0x590430
void UIFriendsClass::Unknown_7C(UIViewClass *view, bool arg)
{
	if (View != view) {
		if (SymbolChatUI) {
			SymbolChatUI->Unknown_7C(view, false);
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

// NOT EXACT: register allocation (the client loads Top into ecx and forms Top + index with lea after the size; edx / eax swapped for the two slot +0x40 calls)
// 0x590DB0
void UIFriendsClass::Unknown_590DB0(int index)
{
	if (index >= 0 && index < 7) {
		int size = PlayersInfo.size();
		int selection = index + Top;
		if (selection < size) {
			if (Selection == selection && Unknown0F0[index]->Is_Unknown2C_00()) {
				Unknown_591EE0();
				return;
			}
			Selection = selection;
			Unknown0D0[2]->Unknown_40(true);
			Unknown0D0[4]->Unknown_40(true);
		}
	}
}

// NOT EXACT: register allocation (the client holds this in ebx and 0 in ebp, ours the reverse)
// 0x590E50
void UIFriendsClass::Unknown_590E50()
{
	if (SymbolChatUI && SymbolChatUI->Active) {
		int state = SymbolChatUI->Unknown_64A220();
		if (state) {
			int length;
			if (state == 2 && (length = strlen(SymbolChatUI->Unknown_64A240())) != 0) {
				int id = ((ClientClass *)Client)->Unknown1138;
	ClientChatRequestPacketClass packet(8, id);
				packet.Unknown14 = std::string(SymbolChatUI->Unknown_64A240());
				((ClientClass *)Client)->Send_Packet(&packet);
			}
			SymbolChatUI->Unknown_5C();
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

// NOT EXACT: inline budget (the client calls the name's _Tidy(false) out of line in its default constructor; ours expands it)
// 0x591A30
void UIFriendsClass::Unknown_591A30()
{
	SClient *sclient = Client->As_Space_Client();
	ClientObjectClass *target;
	if (!sclient->Unknown_744B60() && sclient && sclient->MainView && (target = sclient->MainView->Unknown_5C()) != NULL && target->Unknown_6F5A40()) {
		std::string name;
		target->Get_Name(name);
		int id = ((ClientClass *)Client)->Unknown1138;
		ClientChatRequestPacketClass packet(8, id);
		packet.Unknown14 = std::string(name.c_str());
		((ClientClass *)Client)->Send_Packet(&packet);
	}
}



















































// 0x591EE0
void UIFriendsClass::Unknown_591EE0()
{
	FAIL_IF(Selection < 0 || Selection >= (int)PlayersInfo.size()) return;

	SClient *sclient = Client->As_Space_Client();
	FAIL_IF(!sclient) return;

	UIChatPanelClass *cpanel = sclient->ChatPanel.Peek();
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
