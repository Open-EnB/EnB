// Earth & Beyond client (reconstruction): app\client\chatgadg.cpp. Notes at the end of the file.
#include <stdio.h>
#include <string.h>
#include <string>
#define __CHATGADG_MEMBERS	// pkclientchat.h's 0x878D90
#include "chatgadg.h"
#include "sclient.h"
#include "client.h"
#include "co.h"
#include "avatar.h"
#include "ui_chat_panel.h"
#include "control_keys_manager.h"
#include "..\common\stredit.h"
#include "..\kernel\kernel.h"
#include "..\audio\soundlib.h"
#include "..\packets\pkclientchat.h"
#include "..\packets\pkunknown_87dec0.h"
#include "..\shared\auxdata\auxdata.h"
#include "..\shared\auxdata\delta_string.h"
#include "..\shared\g_assert.h"
#include "ini.h"

#include "..\shared\unknown_8b4560.h"

#define NUM_QUICK_MESSAGES 9	// the FAIL_IFs' constant (the keys 'Save / Send Quick Message 1..9')
extern const int Unknown_B08CA8;	// 0xB08CA8 (0x40: StrEditClass's enter flag; the text gadgets' constants, textgadg.h)  // NAME 1: neutral (D10), as ui_log.cpp
// The 'chatgadg' module's control keys (ControlKeysManager): the dynamic initialiser 0x65BEA0, atexit 0x65BF60
static std::string ModuleName("chatgadg");	// 0xBE5E78  // NAME 1: neutral (D10): what it holds

// 0x65BFD0  // NOT EXACT: functional (D64)
ChatGadgetClass::ChatGadgetClass(LineDisplayClientClass *client) :
	TextBaseGadgetClass(0xFF, Unknown_B08CA8, NULL),
	Client((ClientClass *)client),
	HistoryNext(0),
	HistoryIndex(0),
	Unknown7C(false)
{
	static bool first = true;
	if (first) {
		first = false;
		ControlKeysManager::Register_Key(ModuleName, "Channel Message", 0);
		ControlKeysManager::Register_Key(ModuleName, "Radio Message", 1);
		ControlKeysManager::Register_Key(ModuleName, "Group Message", 2);
		ControlKeysManager::Register_Key(ModuleName, "Guild Message", 3);
		ControlKeysManager::Register_Key(ModuleName, "Local Message", 4);
		ControlKeysManager::Register_Key(ModuleName, "Sector Message", 5);
		ControlKeysManager::Register_Key(ModuleName, "Command Message", 6);
		ControlKeysManager::Register_Key(ModuleName, "Target Message", 7);
		ControlKeysManager::Register_Key(ModuleName, "Reply Last Message", 8);
		ControlKeysManager::Register_Key(ModuleName, "Reply Menu", 9);
		ControlKeysManager::Register_Key(ModuleName, "Target Position", 10);
		ControlKeysManager::Register_Key(ModuleName, "Player Position", 11);
		for (int i = 1; i < 10; i++) {
			char name[64];
			sprintf(name, "Save Quick Message %d", i);
			ControlKeysManager::Register_Key(ModuleName, name, i + 11);
			sprintf(name, "Send Quick Message %d", i);
			ControlKeysManager::Register_Key(ModuleName, name, i + 20);
		}
	}
	for (int i = 0; i < 10; i++) {
		History[i] = NULL;
	}
	QuickMessages = new INIClass;
	QuickMessages->Load("chat.ini");
}

// 0x65C970 (deleting 0x65C940)
ChatGadgetClass::~ChatGadgetClass()
{
	delete QuickMessages;
	for (int i = 0; i < 10; i++) {
		if (History[i] != NULL) {
			delete History[i];
		}
	}
}

// 0x65CA10
bool ChatGadgetClass::Unknown_2C(KernelEventClass *event, float x, float y, unsigned int mask)
{
	SClient *sclient = (SClient *)Client->Unknown_18();
	if (sclient->LoadingView != NULL && sclient->LoadingView->Unknown68) {
		return(true);
	}
	if (!(mask & 0x100)) {
		return(false);
	}
	if (!Active) {
		if (Unknown_65DE90(event)) {
			GadgetClass::Unknown_2C(event, x, y, mask);
			return(true);
		}
		return(false);
	}
	if (!event->Is_Up()) {
		switch (event->Key) {
			case 9:
				if (sclient != NULL && sclient->ChatPanel.Peek() != NULL) {
					ChatPanelInterface *chatpanel = sclient->ChatPanel.Peek();
					chatpanel->Unknown_565120();
					if (event->Is_Shift()) {
						chatpanel->Unknown_565390(true);
					} else {
						chatpanel->Unknown_565390(false);
					}
				}
				break;
			case 0x0D:
				Unknown_65CCD0();
				break;
			case 0x1B:
				Unknown_74(false);
				Edit->Clear();
				break;
			case 0x28:
				if (History[HistoryIndex] != NULL) {
					do {
						if (++HistoryIndex >= 10) {
							HistoryIndex = 0;
						}
					} while (History[HistoryIndex] == NULL);
					Edit->Clear();
					Edit->Insert(History[HistoryIndex]);
					Changed = true;
					Unknown7C = true;
				}
				break;
			case 0x26:
				if (History[HistoryIndex] != NULL) {
					if (Unknown7C) {
						do {
							if (--HistoryIndex < 0) {
								HistoryIndex = 9;
							}
						} while (History[HistoryIndex] == NULL);
					}
					Edit->Clear();
					Edit->Insert(History[HistoryIndex]);
					Changed = true;
					Unknown7C = true;
				}
				break;
			default:
				if (Edit->Process_Key(event)) {
					SoundLibraryClass::Play_2D_Sound("fontclick.wav", 2);
					Changed = true;
				} else {
					Unknown_65DE90(event);
				}
				break;
		}
	}
	GadgetClass::Unknown_2C(event, x, y, mask);
	return(true);
}





















































































































































// 0x65CCD0  // NOT EXACT: functional (D64)
void ChatGadgetClass::Unknown_65CCD0()
{
	SClient *sclient = (SClient *)Client->Unknown_18();
	FAIL_IF(!sclient) return;
	ChatPanelInterface *chatpanel = sclient->ChatPanel.Peek();
	int channel = 0;
	if (chatpanel != NULL) {
		channel = chatpanel->Unknown148;
	}
	char *text = Edit->Get_String();
	if (text != NULL) {
		if (*text == 0) {
			delete text;
		} else {
			if (*text != '/' || !Client->Unknown_34(text)) {
				switch (channel) {
					case 6:
					case 7:
					case 8:
					case 9:
						{
							CharacterViewAvatarClass *avatar = (CharacterViewAvatarClass *)sclient->Unknown_74C840(Client->AvatarID);
							if (avatar != NULL) {
								avatar->Unknown_781760(text);
								ClientChatRequestPacketClass packet(1, Client->Unknown1138);
								packet.Unknown14 = std::string(chatpanel->Unknown_565BA0());
								packet.Unknown_878D90((Unknown_ChatStreamClass *)avatar->Unknown_7817E0());
								packet.Unknown34 = std::string(text);
								Client->Send_Packet(&packet);
							} else {
								ClientChatRequestPacketClass packet(1, Client->Unknown1138);
								packet.Unknown14 = std::string(chatpanel->Unknown_565BA0());
								packet.Unknown34 = std::string(text);
								Client->Send_Packet(&packet);
							}
							if (*text != '/') {
								((SClient *)Client->Unknown_18())->Print(0x10, "To %s: %s", chatpanel->Unknown_565BA0(), text);
							}
						}
						break;
					case 1:
						if (sclient->Unknown_744B60()) {
							CharacterViewAvatarClass *avatar = sclient->Unknown_74C840(Client->AvatarID);
							if (avatar != NULL) {
								((CharacterViewAvatarClass *)avatar)->Unknown_781760(text);
								AvatarEmotePacketClass emote(Client->AvatarID, (Unknown_ChatStreamClass *)avatar->Unknown_7817E0(), 1, NULL);
								Client->Send_Packet(&emote);
								if (avatar->Unknown_7817E0()->Unknown_4C4940()) {
									char line[512] = "";
									avatar->Unknown_7817E0()->Unknown_4C48C0(line);
									ClientChatPacketClass chat(Client->AvatarID, line, 3, NULL);
									Client->Send_Packet(&chat);
								}
							}
						} else {
							ClientChatPacketClass chat(Client->AvatarID, text, 3, NULL);
							Client->Send_Packet(&chat);
						}
						break;
					case 0:
						if (sclient->Unknown_744B60()) {
							CharacterViewAvatarClass *avatar = sclient->Unknown_74C840(Client->AvatarID);
							if (avatar != NULL) {
								((CharacterViewAvatarClass *)avatar)->Unknown_781760(text);
								AvatarEmotePacketClass emote(Client->AvatarID, (Unknown_ChatStreamClass *)avatar->Unknown_7817E0(), 1, NULL);
								Client->Send_Packet(&emote);
								if (avatar->Unknown_7817E0()->Unknown_4C4940()) {
									char line[512] = "";
									avatar->Unknown_7817E0()->Unknown_4C48C0(line);
									ClientChatPacketClass chat(Client->AvatarID, line, 4, NULL);
									Client->Send_Packet(&chat);
								}
							}
						} else {
							ClientChatPacketClass chat(Client->AvatarID, text, 4, NULL);
							Client->Send_Packet(&chat);
						}
						break;
					case 4:
						if (!strcmp(chatpanel->Unknown_5682C0().c_str(), "")) {
							sclient->Unknown_74D990(0x11, "Not subscribed to any channels.", 0);
						} else {
							ClientChatRequestPacketClass packet(0, Client->Unknown1138);
							packet.Unknown24 = chatpanel->Unknown_5682C0();
							packet.Unknown34 = std::string(text);
							Client->Send_Packet(&packet);
						}
						break;
					case 5:
						if (!strcmp(chatpanel->Unknown_568440().c_str(), "")) {
							sclient->Unknown_74D990(0x11, "Not monitoring any public channels.", 0);
						} else {
							ClientChatRequestPacketClass packet(0, Client->Unknown1138);
							packet.Unknown24 = chatpanel->Unknown_568440();
							packet.Unknown34 = std::string(text);
							Client->Send_Packet(&packet);
						}
						break;
					case 3:
						if (((SClient *)Client->Unknown_18())->Unknown_74D570()) {
							ClientChatPacketClass chat(Client->AvatarID, text, 1, NULL);
							Client->Send_Packet(&chat);
						} else {
							((SClient *)Client->Unknown_18())->Unknown_74D990(0x11, "Not in a group.", 0);
						}
						break;
					case 2:
						if (((SClient *)Client->Unknown_18())->Unknown_728300()) {
							ClientChatPacketClass chat(Client->AvatarID, text, 2, NULL);
							Client->Send_Packet(&chat);
						} else {
							((SClient *)Client->Unknown_18())->Unknown_74D990(0x11, "Not in a guild.", 0);
						}
						break;
				}
			}
			if (History[HistoryIndex] == NULL || strcmp(History[HistoryIndex], text) != 0) {
				if (History[HistoryNext] != NULL) {
					delete History[HistoryNext];
				}
				History[HistoryNext] = text;
				HistoryIndex = HistoryNext;
				if (++HistoryNext == 10) {
					HistoryNext = 0;
				}
			} else {
				delete text;
			}
		}
	}
	Unknown7C = false;
	Unknown_74(false);
	Edit->Clear();
}





























// 0x65DE90  // NOT EXACT: functional (D64)
bool ChatGadgetClass::Unknown_65DE90(KernelEventClass *event)
{
	SClient *sclient = (SClient *)Client->Unknown_18();
	FAIL_IF(!sclient) return(false);
	int key = ControlKeysManager::Get_Key_Id(sclient, ModuleName, *(ControlKeyValueStruct *)event);
	if (key == -1) {
		return(false);
	}
	ChatPanelInterface *chatpanel = sclient->ChatPanel.Peek();
	switch (key) {
		case 0:
			if (Active) return(true);
			Edit->Clear();
			if (chatpanel == NULL) return(true);
			if (!chatpanel->Unknown_564F50()) return(true);
			break;
		case 1:
			if (chatpanel == NULL) return(true);
			if (!chatpanel->Unknown_5646A0(4)) return(true);
			break;
		case 2:
			if (chatpanel == NULL) return(true);
			if (!chatpanel->Unknown_5646A0(3)) return(true);
			break;
		case 3:
			if (chatpanel == NULL) return(true);
			if (!chatpanel->Unknown_5646A0(2)) return(true);
			break;
		case 4:
			if (chatpanel != NULL) {
				chatpanel->Unknown_5646A0(1);
			}
			break;
		case 6:
			if (Active) return(true);
			Edit->Insert("/");
			Unknown3B = true;
			break;
		case 7:
			if (Unknown_65E840()) break;
		case 5:
			if (chatpanel != NULL) {
				chatpanel->Unknown_5646A0(0);
			}
			break;
		case 8:
			if (chatpanel == NULL) return(true);
			if (!chatpanel->Unknown_5646A0(9)) return(true);
			break;
		case 9:
			if (chatpanel == NULL) return(true);
			chatpanel->Unknown_567540(1);
			return(true);
		case 10:
			Unknown_65E5E0();
			if (!Active) Unknown_65CCD0();
			return(true);
		case 11:
			Unknown_65E520();
			if (!Active) Unknown_65CCD0();
			return(true);
		default:
			if (key > 0x14 && key < 0x1E) {
				if (!Unknown_65E3E0(key - 0x14)) return(true);
				if (!Active) {
					Unknown_65CCD0();
					return(true);
				}
				Changed = true;
				return(true);
			}
			if (key < 0x0C || key > 0x14) {
				return(false);
			}
			if (Active) {
				Unknown_65E250(key - 0x0B);
				return(true);
			}
			Unknown_65E3E0(key - 0x0B);
			Unknown3B = true;
			break;
	}
	if (!Active) {
		SoundLibraryClass::Play_2D_Sound("message_1.wav", 2);
		Unknown_74(true);
	}
	Changed = true;
	return(true);
}


















































// 0x65E250  // NOT EXACT: functional (D64)
bool ChatGadgetClass::Unknown_65E250(int index)
{
	FAIL_IF(index < 1) return(false);
	FAIL_IF(index > NUM_QUICK_MESSAGES) return(false);

	SClient *sclient = (SClient *)Client->Unknown_18();
	FAIL_IF(!sclient) return(false);
	char message[0x100];
	char text[0x100];
	char entry[0x20];
	char section[0x18];
	sprintf(entry, "MSG_%d", index);
	Edit->Get_String(0x100, text);
	Unknown_8B44E0(sclient->Ticket.Character_ID, section);
	QuickMessages->Put_String(section, entry, text);
	QuickMessages->Save("chat.ini");
	sprintf(message, "Quick message %s has been saved in %s.", entry, "chat.ini");
	sclient->Unknown_74D990(0, message, 0);
	return(true);
}







// 0x65E3E0
bool ChatGadgetClass::Unknown_65E3E0(int index)
{
	FAIL_IF(index < 1) return(false);
	FAIL_IF(index > NUM_QUICK_MESSAGES) return(false);

	SClient *sclient = (SClient *)Client->Unknown_18();
	FAIL_IF(!sclient) return(false);
	char text[0x100];
	char section[0x18];
	char entry[0x14];
	sprintf(entry, "MSG_%d", index);
	Unknown_8B44E0(sclient->Ticket.Character_ID, section);
	QuickMessages->Get_String(section, entry, "", text, 0x100);
	Edit->Insert(text);
	return(true);
}

// 0x65E520
bool ChatGadgetClass::Unknown_65E520()
{
	ClientObjectClass *avatar = Client->Get_Avatar();
	if (avatar != NULL) {
		Matrix3D transform = avatar->Unknown_28(true); Vector3 position = transform.Get_Translation();
		char text[0x100];
		sprintf(text, "Position (%1.2f, %1.2f, %1.2f) ", position.X * 0.001f, position.Y * 0.001f, position.Z * 0.001f);
		Edit->Insert(text);
		Changed = true;
		return(true);
	}
	return(false);
}

// 0x65E5E0  // NOT EXACT: functional (D64)
bool ChatGadgetClass::Unknown_65E5E0()
{
	SClient *sclient = (SClient *)Client->Unknown_18();
	ClientObjectClass *target;
	if (sclient != NULL && sclient->MainView != NULL && (target = sclient->MainView->Unknown_5C()) != NULL) {
		char text[0x100];
		text[0] = 0;
		if (target->AuxData != NULL) {
			NameClass field("Name");
			DeltaStringClass *name = Unknown_Find_Field<DeltaStringClass>((DeltaCompoundClass *)target->AuxData, field);
			if (name != NULL && !name->Unknown70) name = NULL;
			const char *string = name->String;
			if (string != NULL && *string != 0) {
				sprintf(text, "Target '%s' at ", string);
			}
		}
		if (text[0] == 0) {
			sprintf(text, "Target at ");
		}
		Matrix3D transform = target->Unknown_28(true);
		sprintf(text + strlen(text), "(%1.2f, %1.2f, %1.2f) ", transform[0][3] * 0.001f, transform[1][3] * 0.001f, transform[2][3] * 0.001f);
		Edit->Insert(text);
		Changed = true;
		return(true);
	}
	Edit->Insert("No target.");
	Changed = true;
	return(true);
}

// 0x65E7C0
void ChatGadgetClass::Unknown_65E7C0(bool reply)
{
	if (!Active) {
		if (reply && !Unknown_65E840()) {
			return;
		}
		Unknown_74(true);
		Changed = true;
		Edit->Clear();
	}
}











// 0x65E840  // NOT EXACT: functional (D64)
bool ChatGadgetClass::Unknown_65E840()
{
	SClient *sclient = (SClient *)Client->Unknown_18();
	FAIL_IF(!sclient) return(false);

	ChatPanelInterface *chatpanel = sclient->ChatPanel.Peek();
	FAIL_IF(!chatpanel) return(false);
	ClientObjectClass *target;
	if (sclient->MainView != NULL && (target = sclient->MainView->Unknown_5C()) != NULL && target->Unknown_6F5A40() && target->AuxData != NULL) {
		NameClass field("Owner");
		DeltaStringClass *owner = Unknown_Find_Field<DeltaStringClass>((DeltaCompoundClass *)target->AuxData, field);
		if (owner != NULL && !owner->Unknown70) owner = NULL;
		const char *string = owner->String;
		if (string != NULL && *string != 0) {
			std::string name(string);
			unsigned int start = name.find('[');
			unsigned int end = name.find(']');
			if (start != std::string::npos && end != std::string::npos) {
				name.erase(start, end + 2);
			}
			chatpanel->Unknown_5658B0(name.c_str());
			return(true);
		}
	}
	chatpanel->Unknown_5646A0(1);
	return(false);
}



























// 0x65EBF0
void ChatGadgetClass::Unknown_65EBF0(char *print, int s_size, int &cursorpos, bool &insert)
{
	G_ASSERT(s_size > 10);
	G_ASSERT(print);
	Unknown_6AA340(print, s_size, &cursorpos, &insert, "", false);
}

/*
**	Reconstruction notes (ChatGadg.cpp, client 0x65BEA0-0x65ECCF; __FILE__ 0xB8C8A8 'e:\build\beta\G\app\Client\ChatGadg.cpp',
**	asserts on the client's lines 310-784, D25: blank lines keep them there).
**
**	ChatGadgetClass is the chat panel's text entry (ui_chat_panel.cpp makes it at the panel's +0x140; the chat window
**	gets it as its gadget 7). Idle, slot +0x2C hands the keys to the 'chatgadg' module's control keys (0x65DE90: the
**	channel keys open the entry on a channel, the command key types '/', the positions and the quick messages insert
**	text); active, enter sends the line (0x65CCD0) on the chat panel's channel +0x148: 0 / 1 a chat (or with
**	0x744B60 an emote through the avatar's chat stream), 2 guild, 3 group, 4 / 5 the channel requests, 6-9 a private
**	message to the panel's +0x248 name. A line not like the last one goes into the ten-line history.
**	The switch cases sit in the client's code order (6-9, 1, 0, 4, 5, 3, 2; 0x65DE90's 0-4, 6, 7, 5, 8-11).
**	NUM_QUICK_MESSAGES is 9 (the FAIL_IFs' constant; the keys 'Save / Send Quick Message 1..9').
*/
