// Earth & Beyond client (reconstruction): app\client\ui_guild.cpp, the guild window (no __FILE__; client 0x8192C0-0x81E02F
// after time_mgr.cpp's end 0x8192A7, its data 0xBA7F10-0xBA81FF after time_mgr.cpp's __FILE__). So far the printf helper,
// the list helpers and the chat commands (TM1); not yet: the constructor 0x819420, the destructor 0x8196D0, the slots
// +0x54-+0x68, 0x81C380 and its helper 0x81D290. Notes at the end.
// PLACEMENT: provisional (D137): the file name from the tooltip section 'UI_GUILD' (the windows' UI_<file> pattern) and the model 'tsi35a'; a __FILE__ or an assert would place it
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <wchar.h>
#include <string>
#include <vector>
#include <list>
#include "ui_chat_panel.h"
#include "sclient.h"
#include "gadget.h"
#include "gd_slider.h"
#include "fontmanager.h"
#include "client.h"
#include "..\input\input.h"
#include "..\packets\pkclientchat.h"
#include "..\packets\pkguildranknamesrequestclient.h"

// One member of the list (0x20 bytes; the window's +0xCC and +0xDC hold them)
class UIGuildMemberClass	// NAME 1: neutral, after ui_friends.h's UIFriendsPlayerInfoClass
{
public:
	UIGuildMemberClass() : Name(""), Location("") {}	// (inline: 0x81AEB0, 0x81C380)
	~UIGuildMemberClass() {}							// 0x81AD80 (inline, called out of line; deleting 0x81AD50)
	std::string Name;									// +0x00 (the rows' 'name%02d_TSI35a')  // NAME 2: its style's
	std::string Location;								// +0x10 ('LOCATION%02d_TSI35a'; '(offline)')  // NAME 2: its style's
};

// The guild window (0x190 bytes; constructor 0x819420 over ui_chat_panel_base.cpp's 0x56D280, destructor 0x8196D0,
// tables 0xB081A4 / 0xB08198 / 0xB0818C), ui_friends.cpp's twin; declared as far as the rebuilt functions use it
class UIGuildClass : public ChatPanelUIBase	// NAME 1: neutral, after its tooltip section 'UI_GUILD'
{
public:
	virtual void Unknown_7C(UIViewClass *view, bool arg);	// +0x7C 0x81AA50 (the three entries moved too)  // NAME 1 [view, arg]: InterfaceClass's
	void Unknown_81AAD0(int count);						// 0x81AAD0 (the slider over count rows, 7 shown)  // NAME 1 [count]: neutral
	void Unknown_81AB50();								// 0x81AB50 (Top from the slider's value; the rows refreshed when it moved)
	void Unknown_81ABA0();								// 0x81ABA0 (the rows from Top)
	void Unknown_81AC90();								// 0x81AC90 (the members deleted, the vector emptied)
	void Unknown_81AE10();								// 0x81AE10 (the list's members deleted, the list emptied)
	void Unknown_81AEB0();								// 0x81AEB0 (the vector rebuilt from the list, '(offline)' last, by name; the selection kept by name)
	void Unknown_81B350();								// 0x81B350 (the rows under the pointer or selected lit)
	void Unknown_81B3F0(int index);						// 0x81B3F0 (a row clicked: its name copied, the buttons updated)  // NAME 1 [index]: neutral
	void Unknown_81B610(int key);						// 0x81B610 (the selected member's /gc remove, promote, demote, contributions)  // NAME 1 [key]: slot +0x68's
	void Unknown_81BA60(bool on);						// 0x81BA60 (the buttons' slot +0x40 and tooltips by the rank's rights)  // NAME 1 [on]: neutral
	void Unknown_81BF40();								// 0x81BF40 ('/gc list all')
	int Unknown_81C930(std::string rank);				// 0x81C930 (a rank name's number: the rank names' vector)  // NAME 1 [rank]: neutral
	void Unknown_81BFD0();								// 0x81BFD0 (the rank names requested)
	void Unknown_81CB90();								// 0x81CB90 (slot +0x60: an entry answered, its /gc recruit, tag, message, stats or create sent)
	void Unknown_81D130();								// 0x81D130 (no caller: every member's '/gc contributions')
	void Unknown_81C5E0(int rights, int unknown1, int unknown2, int unknown3);	// 0x81C5E0 (sclient.cpp: the rank's rights and limits)  // NAME 1 [rights, unknown1-3]: neutral

	GadgetSlider *Slider;								// +0xA0  // NAME 1: as ui_friends.h's
	int Top;											// +0xA4 (the first row's index: the slider's value)  // NAME 1: as ui_friends.h's
	int Selection;										// +0xA8 (-1 from the constructor)  // NAME 1: as ui_friends.h's
	std::string Unknown0AC;								// +0xAC (the selected member's name)
	RefCountPtr<UIPTradeCreditsClass> Unknown0BC[3];	// +0xBC the text entries (ui_ptrade.cpp's credits entry)
	int Unknown0C8;										// +0xC8
	std::vector<UIGuildMemberClass *> Unknown0CC;		// +0xCC the members, sorted
	std::list<UIGuildMemberClass *> Unknown0DC;			// +0xDC the members as received
	GadgetClass *Unknown0E8[13];						// +0xE8 the buttons
	GadgetClass *Unknown11C[7];							// +0x11C the rows
	RefCountPtr<FontSys> Unknown138[7];					// +0x138 the rows' names ('name%02d_TSI35a')
	RefCountPtr<FontSys> Unknown154[7];					// +0x154 the rows' locations ('LOCATION%02d_TSI35a')
	unsigned int Unknown170;							// +0x170 the rank's rights (bits 0-11: the buttons)
	int Unknown174;										// +0x174 (-1 from the constructor)
	int Unknown178;										// +0x178 (-1 from the constructor)
	int Unknown17C;										// +0x17C (-1 from the constructor)
	std::vector<GuildRankNameStruct> Unknown180;		// +0x180 the rank names
};

// The client at InterfaceClass +0x40, as ui_chat_panel.cpp declares it
class LineDisplayClientClass  // NAME 1: neutral (D10)
{
public:
	virtual void Unknown_00(); virtual void Unknown_04(); virtual void Unknown_08(); virtual void Unknown_0C();
	virtual void Unknown_10(); virtual void Unknown_14();
	virtual SClient *As_Space_Client();					// +0x18 (SClient's returns itself)  // NAME 3: ui_manu.cpp's FAIL_IF's
};

// The buttons' tooltips (0xBA7F10)
static const char *Unknown_BA7F10[13] = { "EXIT", "RECRUIT", "REMOVE", "PROMOTE", "DEMOTE", "DISBAND", "TAG", "MESSAGE", "MAKE_PUBLIC", "MAKE_PRIVATE", "LEAVE", "CONTRIBUTION", "STATS" };	// NAME 1: neutral (D10)

// 0x8192C0
std::string Unknown_8192C0(const char *format, ...)	// NAME 1: neutral (D10): ui_log.cpp and the guild windows' printf into a std::string
{
	char buffer[256];
	va_list va;
	va_start(va, format);
	vsprintf(buffer, format, va);
	std::string result(buffer);
	return(result);
}

// One member deleted (inline)
static inline void Unknown_Delete_Member(UIGuildMemberClass *member)	// NAME 1 [Unknown_Delete_Member, member]: neutral
{
	delete member;
}

// 0x81AA50
void UIGuildClass::Unknown_7C(UIViewClass *view, bool arg)
{
	if (View != view) {
		if (Unknown0BC[0]) {
			Unknown0BC[0]->Unknown_7C(view, false);
		}
		if (Unknown0BC[1]) {
			Unknown0BC[1]->Unknown_7C(view, false);
		}
		if (Unknown0BC[2]) {
			Unknown0BC[2]->Unknown_7C(view, false);
		}
		InterfaceClass::Unknown_7C(view, arg);
	}
}

// 0x81AAD0
void UIGuildClass::Unknown_81AAD0(int count)
{
	Slider->Unknown_66FA90((float)count, 7.0f, true);
	Unknown_81AB50();
}

// 0x81AB50
void UIGuildClass::Unknown_81AB50()
{
	int top = (int)Slider->Value;
	if (Top != top) {
		Top = top;
		Unknown_81ABA0();
	}
	Slider->Unknown_66FD80(false);
}

// 0x81ABA0
void UIGuildClass::Unknown_81ABA0()
{
	for (int i = 0; i < 7; i++) {
		unsigned int index = Top + i;
		if (index < Unknown0CC.size()) {
			Unknown138[i]->Change_Text(Unknown0CC[index]->Name.c_str(), 0);
			Unknown154[i]->Change_Text(Unknown0CC[index]->Location.c_str(), 0);
			Unknown11C[i]->Unknown_40(true);
		} else {
			Unknown138[i]->Change_Text(" ", 0);
			Unknown154[i]->Change_Text(" ", 0);
			Unknown11C[i]->Unknown_40(false);
		}
	}
}

// 0x81AC90
void UIGuildClass::Unknown_81AC90()
{
	std::vector<UIGuildMemberClass *>::iterator it;
	for (it = Unknown0CC.begin(); it != Unknown0CC.end(); it++) {
		Unknown_Delete_Member(*it);
	}
	Unknown0CC.erase(Unknown0CC.begin(), Unknown0CC.end());
}

// 0x81AE10
void UIGuildClass::Unknown_81AE10()
{
	std::list<UIGuildMemberClass *>::iterator it;
	for (it = Unknown0DC.begin(); it != Unknown0DC.end(); it++) {
		Unknown_Delete_Member(*it);
	}
	Unknown0DC.clear();
}

// NOT EXACT: scheduling only (the client computes the vector's address for the inlined clean-up's erase before saving edi) (0x81AEB0)
// 0x81AEB0
void UIGuildClass::Unknown_81AEB0()
{
	Unknown_81AC90();
	for (std::list<UIGuildMemberClass *>::iterator node = Unknown0DC.begin(); node != Unknown0DC.end(); node++) {
		UIGuildMemberClass *info = *node;
		std::vector<UIGuildMemberClass *>::iterator pos;
		for (pos = Unknown0CC.begin(); pos != Unknown0CC.end(); pos++) {
			UIGuildMemberClass *other = *pos;
			bool offline = strstr(info->Location.c_str(), "(offline)") != NULL;
			bool other_offline = strstr(other->Location.c_str(), "(offline)") != NULL;
			if (offline == false && other_offline == true) {
				break;
			}
			if (offline == true && other_offline == false) {
				continue;
			}
			if (wcscoll((const wchar_t *)info->Name.c_str(), (const wchar_t *)other->Name.c_str()) < 0) {
				break;
			}
		}
		UIGuildMemberClass *member = new UIGuildMemberClass();
		member->Name = info->Name;
		member->Location = info->Location;
		Unknown0CC.insert(pos, member);
	}
	if (Unknown0AC.size() != 0) {
		int i = 0;
		for (;;) {
			if (i < (int)Unknown0CC.size()) {
				if (Unknown0AC == Unknown0CC[i]->Name) {
					Selection = i;
					break;
				}
				i++;
			} else {
				Unknown0AC = "";
				Selection = -1;
				break;
			}
		}
	} else {
		Selection = -1;
	}
	int last = Unknown0CC.size() - 1;
	Top = (Top < last) ? Top : last;
	Unknown_81AAD0(Unknown0CC.size());
	Unknown0E8[3]->Unknown_40(false);
	Unknown0E8[4]->Unknown_40(false);
	Unknown0E8[2]->Unknown_40(false);
	Unknown0E8[6]->Unknown_40(false);
	Unknown0E8[11]->Unknown_40(false);
}

// 0x81B350
void UIGuildClass::Unknown_81B350()
{
	float x;
	float y;
	Unknown_42ECF0(&x, &y);
	for (int i = 0; i < 7; i++) {
		if (Unknown11C[i]->Unknown_30(x, y) || Top + i == Selection) {
			Unknown138[i]->Unknown_6CE910(true);
			Unknown154[i]->Unknown_6CE910(true);
		} else {
			Unknown138[i]->Unknown_6CE910(false);
			Unknown154[i]->Unknown_6CE910(false);
		}
	}
}

// 0x81B3F0
void UIGuildClass::Unknown_81B3F0(int index)
{
	Selection = Top + index;
	if (Selection >= 0 && Selection < (int)Unknown0CC.size()) {
		Unknown0AC = Unknown0CC[Selection]->Name;
		Unknown_81BA60(true);
	}
}

// 0x81B610
void UIGuildClass::Unknown_81B610(int key)
{
	if (Selection >= 0 && Selection < (int)Unknown0CC.size()) {
		std::string name = Unknown0CC[Selection]->Name;
		switch (key) {
			case 3:
				((ClientClass *)Client)->Send_Packet(&ClientChatPacketClass(((ClientClass *)Client)->Get_Avatar_ID(), (char *)Unknown_8192C0("/gc remove %s", name.c_str()).c_str(), 3, NULL));
				break;
			case 4:
				((ClientClass *)Client)->Send_Packet(&ClientChatPacketClass(((ClientClass *)Client)->Get_Avatar_ID(), (char *)Unknown_8192C0("/gc promote %s", name.c_str()).c_str(), 3, NULL));
				break;
			case 5:
				((ClientClass *)Client)->Send_Packet(&ClientChatPacketClass(((ClientClass *)Client)->Get_Avatar_ID(), (char *)Unknown_8192C0("/gc demote %s", name.c_str()).c_str(), 3, NULL));
				break;
			case 12:
				((ClientClass *)Client)->Send_Packet(&ClientChatPacketClass(((ClientClass *)Client)->Get_Avatar_ID(), (char *)Unknown_8192C0("/gc contributions %s", name.c_str()).c_str(), 3, NULL));
				if (Unknown0BC[0]->Active) {
					Unknown0BC[0]->Unknown_5C();
				}
				break;
		}
	}
}

// 0x81BA60
void UIGuildClass::Unknown_81BA60(bool on)
{
	int i;
	for (i = 0; i < 13; i++) {
		if (Unknown0E8[i]) {
			Unknown0E8[i]->Unknown_40(on);
			if (on) {
				GadgetClass::Set_Tooltip(Unknown0E8[i], "UI_GUILD", Unknown_BA7F10[i]);
			} else {
				Unknown0E8[i]->Set_Unknown14(NULL);
			}
		}
	}
	for (i = 0; i < 7; i++) {
		if (Unknown11C[i]) {
			Unknown11C[i]->Unknown_40(on);
		}
	}
	if (on && Client->As_Space_Client()->Unknown_728300()) {
		Unknown0E8[12]->Unknown_40(Unknown170 & 1);
		Unknown0E8[10]->Unknown_40((Unknown170 >> 2) & 1);
		Unknown0E8[1]->Unknown_40((Unknown170 >> 3) & 1);
		Unknown0E8[2]->Unknown_40((Unknown170 >> 4) & 1);
		Unknown0E8[7]->Unknown_40((Unknown170 >> 5) & 1);
		Unknown0E8[5]->Unknown_40((Unknown170 >> 8) & 1);
		Unknown0E8[8]->Unknown_40((Unknown170 >> 10) & 1);
		Unknown0E8[9]->Unknown_40((Unknown170 >> 11) & 1);
	}
	if (Selection < 0) {
		Unknown0E8[2]->Unknown_40(false);
		Unknown0E8[3]->Unknown_40(false);
		Unknown0E8[4]->Unknown_40(false);
		Unknown0E8[6]->Unknown_40(false);
		Unknown0E8[11]->Unknown_40(false);
		Unknown_81ABA0();
		return;
	}
	int rank = Unknown_81C930(Unknown0CC[Selection]->Location);
	int own = Client->As_Space_Client()->Unknown_728370();
	Unknown0E8[3]->Unknown_40(rank < Unknown174 && rank < own);
	Unknown0E8[2]->Unknown_40(rank <= Unknown178 && rank <= own);
	Unknown0E8[4]->Unknown_40(rank > Unknown17C && rank < own);
	Unknown0E8[6]->Unknown_40((Unknown170 >> 9) & 1);
	Unknown0E8[11]->Unknown_40((Unknown170 >> 1) & 1);
	Unknown_81ABA0();
}

// 0x81BF40
void UIGuildClass::Unknown_81BF40()
{
	((ClientClass *)Client)->Send_Packet(&ClientChatPacketClass(((ClientClass *)Client)->Get_Avatar_ID(), "/gc list all", 3, NULL));
}

// 0x81BFD0
void UIGuildClass::Unknown_81BFD0()
{
	((ClientClass *)Client)->Send_Packet(&GuildRankNamesRequestClientPacketClass(((ClientClass *)Client)->Get_Avatar_ID()));
}

// 0x81C5E0
void UIGuildClass::Unknown_81C5E0(int rights, int unknown1, int unknown2, int unknown3)
{
	Unknown170 = rights;
	Unknown174 = unknown1;
	Unknown178 = unknown2;
	Unknown17C = unknown3;
	Unknown_81BA60(true);
}

// 0x81C930
int UIGuildClass::Unknown_81C930(std::string rank)
{
	if (Unknown180.size() == 0) {
		return(0);
	}
	std::string name = rank.substr(1, rank.find("\"", 1) - 1);
	for (int i = 0; i < (int)Unknown180.size(); i++) {
		if (name == Unknown180[i].Name) {
			return(Unknown180[i].Rank);
		}
	}
	return(-1);
}

// NOT EXACT: registers only (the stats case's c_str in ecx, ours eax) (0x81CB90)
// 0x81CB90
void UIGuildClass::Unknown_81CB90()
{
	RefCountPtr<UIPTradeCreditsClass> entry;
	if (Unknown0BC[2] && Unknown0BC[2]->Active) {
		entry = Unknown0BC[2];
	} else if (Unknown0BC[1] && Unknown0BC[1]->Active) {
		entry = Unknown0BC[1];
	}
	int state;
	if (entry && (state = entry->Unknown_64A220()) != 0) {
		if (state == 2) {
			switch (Unknown0C8) {
				case 2:
				((ClientClass *)Client)->Send_Packet(&ClientChatPacketClass(((ClientClass *)Client)->Get_Avatar_ID(), (char *)Unknown_8192C0("/gc recruit %s", entry->Unknown_64A240()).c_str(), 3, NULL));
					break;
				case 13:
				((ClientClass *)Client)->Send_Packet(&ClientChatPacketClass(((ClientClass *)Client)->Get_Avatar_ID(), (char *)Unknown_8192C0("/gc stats %s", entry->Unknown_64A240()).c_str(), 3, NULL));
					if (Unknown0BC[0]->Active) {
						Unknown0BC[0]->Unknown_5C();
					}
					break;
				case 8:
				((ClientClass *)Client)->Send_Packet(&ClientChatPacketClass(((ClientClass *)Client)->Get_Avatar_ID(), (char *)Unknown_8192C0("/gc message %s", entry->Unknown_64A240()).c_str(), 3, NULL));
					break;
				case 7:
				((ClientClass *)Client)->Send_Packet(&ClientChatPacketClass(((ClientClass *)Client)->Get_Avatar_ID(), (char *)Unknown_8192C0("/gc tag %s \"%s\"", Unknown0CC[Selection]->Name.c_str(), entry->Unknown_64A240()).c_str(), 3, NULL));
					break;
				case 24:
				((ClientClass *)Client)->Send_Packet(&ClientChatPacketClass(((ClientClass *)Client)->Get_Avatar_ID(), (char *)Unknown_8192C0("/gc create %s", entry->Unknown_64A240()).c_str(), 3, NULL));
					break;
			}
		}
		entry->Unknown_5C();
		if (Unknown0C8 == 24) {
			Unknown_5C();
			Unknown98 = 1;
		} else {
			Unknown_81BA60(true);
		}
	}
}

// 0x81D130
void UIGuildClass::Unknown_81D130()
{
	for (int i = 0; i < (int)Unknown0CC.size(); i++) {
		((ClientClass *)Client)->Send_Packet(&ClientChatPacketClass(((ClientClass *)Client)->Get_Avatar_ID(), (char *)Unknown_8192C0("/gc contributions %s", Unknown0CC[i]->Name.c_str()).c_str(), 3, NULL));
	}
}

/*
**	Reconstruction notes (TM1, 2026-11-09)
**
**	The object runs 0x8192C0-0x81C5FF: 0x8192C0 (a printf into a std::string, called from ui_log.cpp and the guild list
**	window 0x81CB90 too: it may be a header's inline vararg function emitted here), the constructor 0x819420, the deleting
**	destructor 0x8196A0, the destructor 0x8196D0, the slots +0x54 0x8199F0, +0x58 0x819FB0, +0x5C 0x81A200, +0x60 0x81A330,
**	+0x68 0x81A420, +0x7C 0x81AA50, the list helpers to 0x81C5E0, with the member's destructor 0x81AD80 (deleting 0x81AD50),
**	the rank packet's destructor 0x81C040, the vectors' insert helpers 0x81C060 / 0x81C1F0, the rank lookup 0x81C930, the
**	entries' answers 0x81CB90 (slot +0x60's) and 0x81D130 (no caller), then the templates to the deleting destructor's
**	adjustor entry 0x81E010 (table 0xB08198). The guild list window ('UI_GUILD_LIST', 'tsi36a') follows.
*/
