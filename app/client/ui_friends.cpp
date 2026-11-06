// Earth & Beyond client (reconstruction): app\client\ui_friends.cpp (the client's __FILE__
// 'e:\build\beta\G\app\Client\ui_friends.cpp', 0x58F090-0x5928AF by its asserts and its string order).
// So far only 0x5908B0-0x59102F (FR1); the file's other functions, with their asserts (lines 586-722),
// are not rebuilt yet.
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

// One player deleted (inline)
static inline void Unknown_Delete_Player(UIFriendsPlayerInfoClass *info)	// NAME 1 [Unknown_Delete_Player, info]: neutral
{
	delete info;
}

// PlayersInfo's players deleted and the list emptied (inline: no copy in the client)
inline void UIFriendsClass::Unknown_Clear_Players()
{
	std::vector<UIFriendsPlayerInfoClass *>::iterator it;
	for (it = PlayersInfo.begin(); it != PlayersInfo.end(); it++) {
		Unknown_Delete_Player(*it);
	}
	PlayersInfo.erase(PlayersInfo.begin(), PlayersInfo.end());
}

// NOT EXACT: register allocation (the client keeps 0 in ebx and the insert position in a stack slot; ours holds the position in ebx; frame 0x18 vs 0x14)
// 0x5908B0
void UIFriendsClass::Unknown_5908B0()
{
	Unknown_Clear_Players();

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
	int count = PlayersInfo.size();
	Slider->Unknown_66FA90((float)count, 7.0f, true);
	int top = (int)Slider->Value;
	if (Top != top) {
		Top = top;
		Unknown_590550();
	}
	Slider->Unknown_66FD80(false);
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

// NOT EXACT: register allocation (the client holds this in ebx and 0 in ebp, ours the reverse; the packet's lea comes before the Unknown1138 load)
// 0x590E50
void UIFriendsClass::Unknown_590E50()
{
	if (SymbolChatUI && SymbolChatUI->Active) {
		int state = SymbolChatUI->Unknown_64A220();
		if (state) {
			int length;
			if (state == 2 && (length = strlen(SymbolChatUI->Unknown_64A240())) != 0) {
				ClientChatRequestPacketClass packet(8, ((ClientClass *)Client)->Unknown1138);
				packet.Unknown14 = std::string(SymbolChatUI->Unknown_64A240());
				((ClientClass *)Client)->Send_Packet(&packet);
			}
			SymbolChatUI->Unknown_5C();
		}
	}
}
