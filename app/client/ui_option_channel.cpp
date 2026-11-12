// Earth & Beyond client (reconstruction): app\client\ui_option_channel.cpp, the options' channels page ('opt06a').
// No string names the file (no asserts): the name is provisional (NAME 1, D137). Notes at the end.
#include <stdio.h>
#include <string.h>
#include <string>
#include "ui_option_channel.h"
#include "ui_chat_panel.h"
#include "client.h"
#include "gadget.h"
#include "gd_robj.h"
#include "option.h"
#include "control_keys_manager.h"
#include "local_interface_sort.h"
#include "..\packets\pkclientchat.h"
#include "..\audio\soundlib.h"

// The chat channels' names (.data 0xBA8C20-0xBA8C50, also read by ui_chat_panel.cpp's initialiser 0x568700)
extern char *Unknown_BA8C20;	// NAME 1: neutral (D10): 'General'
extern char *Unknown_BA8C24;	// NAME 1: neutral (D10): 'Out of Context'
extern char *Unknown_BA8C28;	// NAME 1: neutral (D10): 'Market'
extern char *Unknown_BA8C2C;	// NAME 1: neutral (D10): 'New Players'
extern char *Unknown_BA8C30;	// NAME 1: neutral (D10): 'Terran'
extern char *Unknown_BA8C34;	// NAME 1: neutral (D10): 'Jenquai'
extern char *Unknown_BA8C38;	// NAME 1: neutral (D10): 'Progen'
extern char *Unknown_BA8C3C;	// NAME 1: neutral (D10): 'Explorers'
extern char *Unknown_BA8C40;	// NAME 1: neutral (D10): 'Warriors'
extern char *Unknown_BA8C44;	// NAME 1: neutral (D10): 'Tradesmen'
extern char *Unknown_BA8C48;	// NAME 1: neutral (D10): 'Sentinels'
extern char *Unknown_BA8C4C;	// NAME 1: neutral (D10): 'Defenders'
extern char *Unknown_BA8C50;	// NAME 1: neutral (D10): 'Enforcers'

// The channel each check joins or leaves (0xBE5CB8, filled by the initialiser 0x5C8CC0), NULL for none
static const char *Unknown_BE5CB8[21] = {	// NAME 1: neutral (D10)
	NULL, Unknown_BA8C20, Unknown_BA8C3C, NULL, Unknown_BA8C24, Unknown_BA8C40, NULL,
	Unknown_BA8C28, Unknown_BA8C44, NULL, Unknown_BA8C2C, Unknown_BA8C48, NULL, Unknown_BA8C30,
	Unknown_BA8C4C, NULL, Unknown_BA8C34, Unknown_BA8C50, NULL, Unknown_BA8C38, NULL,
};

// The buttons Unknown_54 makes: their models' names and their texts (0xB82C94 / 0xB82CA0)
static const char *Unknown_B82C94[3] = { "exit", "ok", "cancel" };	// NAME 1: neutral (D10)
static const char *Unknown_B82CA0[3] = { NULL, "OK_opt06a", "CANCEL_opt06a" };	// NAME 1: neutral (D10)

// The option each check shows (0xB82CAC; option.cpp's 'Channel_*' bools), NULL for an unused check
static const char *Unknown_B82CAC[21] = {	// NAME 1: neutral (D10)
	"Channel_Broadcast", "Channel_General", "Channel_Explorers", "Channel_Private", "Channel_OOC",
	"Channel_Warriors", "Channel_Group", "Channel_Market", "Channel_Traders", "Channel_Guild",
	"Channel_Newbie", "Channel_Sentinels", "Channel_Local", "Channel_Terran", "Channel_Defenders",
	NULL, "Channel_Jenquai", "Channel_Enforcers", NULL, "Channel_Progen", NULL,
};

// The check box (unknown_7e7e00.h: 0x68 bytes, table 0xB075E4)
#include "unknown_7e7e00.h"








// kernel.h's input event, as ui_option_account.cpp's (Unknown_54's Escape)










// 0x5C8DB0
UIOptionChannelClass::UIOptionChannelClass(SClient *client, bool mode) :
	ChatPanelUIBase(client),
	Mode(mode)
{
	memset(Buttons, 0, sizeof(Buttons));
	memset(Checks, 0, sizeof(Checks));
}

// 0x5C8E60
UIOptionChannelClass::~UIOptionChannelClass()
{
	if (Active) {
		InterfaceClass::Unknown_5C();
	}
}

// 0x5C8EF0
void UIOptionChannelClass::Unknown_54()
{
	if (Unknown_98("opt06a", "fa_opt06a.ini")) {
		int i;
		for (i = 0; i < 3; i++) {
			Buttons[i] = Add_Button(i + 1, Unknown_B82C94[i], Unknown_B82CA0[i]);
			GadgetClass::Set_Tooltip(Buttons[i], "UI_OPTION_CHANNEL", Unknown_B82C94[i]);
		}
		for (i = 0; i < 21; i++) {
			char name[32];
			_snprintf(name, sizeof(name), "check%02d", i);
			Checks[i] = Add_Check_Button(i + 5, name, NULL);
			if (!Unknown_B82CAC[i]) {
				Checks[i]->Unknown_40(false);
				((RObjGadgetClass *)Checks[i])->Unknown_8C(true);
			}
		}
		Add_Back_Gadget(0, Vector3(0.0f, 0.0f, 0.0f));
		((ControlKeyListClass *)&Unknown4C)->Add(KernelEventClass(0x1B), (void *)1);
	}
}

// 0x5C9070
void UIOptionChannelClass::Unknown_58()
{
	if (!Active) {
		InterfaceSortScopeClass sort(this);	// NAME 1: neutral (D10)
		Unknown_5C9340();
		ChatPanelUIBase::Unknown_58();
	}
}

// 0x5C9150
void UIOptionChannelClass::Unknown_5C()
{
	if (Active) {
		InterfaceClass::Unknown_5C();
	}
}

// 0x5C9170
bool UIOptionChannelClass::Unknown_68(int key)
{
	SClient *sclient = (SClient *)((ClientClass *)Client)->Unknown_18();	// NAME 1: neutral (D10)
	ChatPanelInterface *chat = sclient->ChatPanel.Peek();	// NAME 1: neutral (D10)
	switch (key) {
		case 1:
			SoundLibraryClass::Play_2D_Sound("Roll_Select02b.wav", 2);
			Unknown_5C();
			Closed = 1;
			return(true);
		case 2:
			SoundLibraryClass::Play_2D_Sound("Roll_Select02b.wav", 2);
			if (!Mode) {
				chat->Unknown_565F30(1);
			}
			Unknown_5C93B0();
			Unknown_5C();
			Closed = 1;
			return(true);
		case 3:
			SoundLibraryClass::Play_2D_Sound("Roll_Select02b.wav", 2);
			if (!Mode) {
				chat->Unknown_565F30(1);
			}
			break;
		case 5: case 6: case 7: case 8: case 9: case 10: case 11: case 12: case 13: case 14: case 15:
		case 16: case 17: case 18: case 19: case 20: case 21: case 22: case 23: case 24: case 25:
			SoundLibraryClass::Play_2D_Sound("fontclick.wav", 2);
			return(true);
		default:
			return(false);
	}
	Unknown_5C();
	Closed = 1;
	return(true);
}

// 0x5C92D0
void UIOptionChannelClass::Unknown_5C92D0(bool on)
{
	int i;
	for (i = 0; i < 3; i++) {
		if (Buttons[i]) {
			Buttons[i]->Unknown_40(on);
		}
	}
	for (i = 0; i < 21; i++) {
		if (Checks[i]) {
			Checks[i]->Unknown_40(on);
		}
	}
}

// 0x5C9340
void UIOptionChannelClass::Unknown_5C9340()
{
	OptionClass *options = OptionClass::Instance;	// NAME 1: neutral (D10)
	for (int i = 0; i < 21; i++) {
		if (Unknown_B82CAC[i]) {
			BoolOptionStruct *option = options->Find_Bool(Unknown_B82CAC[i]);	// NAME 1: neutral (D10)
			if (option) {
				Checks[i]->Unknown_7E8030(option->Get_Value());
			}
		}
	}
}

// 0x5C93B0
void UIOptionChannelClass::Unknown_5C93B0()
{
	OptionClass *options = OptionClass::Instance;	// NAME 1: neutral (D10)
	for (int i = 0; i < 21; i++) {
		if (Unknown_B82CAC[i]) {
			bool on = Checks[i]->Checked;	// NAME 1: neutral (D10)
			BoolOptionStruct *option = options->Find_Bool(Unknown_B82CAC[i]);	// NAME 1: neutral (D10)
			if (option && option->Value != on) {
				option->Value = on;
				const char *channel = Unknown_BE5CB8[i];	// NAME 1: neutral (D10)
				if (channel) {
					if (on) {
						Unknown_5C9580(std::string(channel));
					} else {
						Unknown_5C9740(std::string(channel));
					}
				}
			}
		}
	}
	SClient *sclient = (SClient *)((ClientClass *)Client)->Unknown_18();	// NAME 1: neutral (D10)
	ChatPanelInterface *chat = sclient->ChatPanel.Peek();	// NAME 1: neutral (D10)
	if (chat) {
		chat->Unknown_5689D0(0);
	}
}

// 0x5C9580
void UIOptionChannelClass::Unknown_5C9580(std::string channel)
{
	int id = ((ClientClass *)Client)->Unknown1138;	// NAME 1: neutral (D10)
	ClientChatRequestPacketClass packet(6, id);
	packet.Unknown24 = channel;
	packet.Unknown34 = std::string(" ");
	((ClientClass *)Client)->Send_Packet(&packet);
}

// 0x5C9740
void UIOptionChannelClass::Unknown_5C9740(std::string channel)
{
	int id = ((ClientClass *)Client)->Unknown1138;	// NAME 1: neutral (D10)
	ClientChatRequestPacketClass packet(7, id);
	packet.Unknown24 = channel;
	((ClientClass *)Client)->Send_Packet(&packet);
}

/*
**	Notes.
**	Object 0x5C8CC0-0x5C990F: the initialiser of Unknown_BE5CB8 (0x5C8CC0) opens it, after ui_option_account.cpp's
**	adjustor thunks; this page's adjustor thunks 0x5C98F0 / 0x5C9900 close it and the 'opt03a' page's constructor
**	0x5C9910 follows. The pattern is ui_option_account.cpp's (ChatPanelUIBase). 0x5C92D0 has no caller; 0x5C9340 has
**	none either and is inlined into Unknown_58.
*/
