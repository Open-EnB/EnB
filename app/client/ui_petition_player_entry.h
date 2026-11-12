// Earth & Beyond client (reconstruction): app\client\ui_petition_player_entry.h, the petition's page about other
// players (the entry window and a name box, 'hsi03a'). The client's asserts name the file
// (e:\build\beta\G\app\Client\ui_petition_player_entry.cpp); this header's name follows it (NAME 1, provisional).
#ifndef UI_PETITION_PLAYER_ENTRY_H  // NAME 1 [ui_petition_player_entry.h]: provisional header name for ui_petition_player_entry.cpp
#define UI_PETITION_PLAYER_ENTRY_H

#include <string>
#include "ui_petition_entry.h"

// The players' name box (0xA4 bytes, tables 0xAF5494 / 0xAF5488 / 0xAF547C; model 'hsi03a', 'fa_hsi03a.ini'): a text
// box the target's name can be imported into (button 1 'exit_Import'), kept beside the page
class UIPetitionPlayerNameClass : public ChatPanelUIBase	// NAME 1: neutral, after its member NameEditor
{
public:
	UIPetitionPlayerNameClass(SClient *client);			// 0x5D6AA0 (deleting 0x5D6AF0; inlined in 0x5D61E0)  // NAME 1 [client]: ChatPanelUIBase's
	virtual ~UIPetitionPlayerNameClass();				// 0x5D6B20
	virtual void Unknown_54();							// +0x54 0x5D6BC0 ('hsi03a': 'exit_Import', 'Entry_Field01', the name box)
	virtual void Unknown_60();							// +0x60 0x5D6DC0 (FAIL_IF '!NameEditor': the name box's text through 0x4BE9D0 while it has the focus)
	virtual bool Unknown_68(int key);					// +0x68 0x5D6EE0 (FAIL_IF '!NameEditor', '!sclient': 1 the target's name imported, 2 the focus)  // NAME 1 [key]: InterfaceClass's

	bool Get_Names(std::string &names);					// 0x5D7150 (the names typed, when more than 2 printable characters; inlined at 0x5D6550 / 0x5D6890)  // NAME 1 [Get_Names, names]: what it does

	TextGadgetClass *NameEditor;						// +0xA0 'Entry_Field01' (id 8, 'PLAYER_NAME_hsi03a')  // NAME 3: the FAIL_IF '!NameEditor'
};

// The players page (0xBC bytes, tables 0xAF53A8 / 0xAF539C / 0xAF5390): the entry window with the name box; its submit
// sends a PetitionStuckPacketClass with the names as the player list
class UIPetitionPlayerEntryClass : public UIPetitionEntryClass	// NAME 1: neutral, after the file and its tooltips' section 'UI_PETITION_PLAYER_ENTRY'
{
public:
	UIPetitionPlayerEntryClass(SClient *client, int unknown0B8);	// 0x5D6080 (deleting 0x5D60D0)  // NAME 1 [client, unknown0B8]: neutral
	virtual ~UIPetitionPlayerEntryClass();				// 0x5D6100
	virtual void Unknown_54();							// +0x54 0x5D61E0 (the entry window's, then the name box over it)
	virtual void Unknown_58();							// +0x58 0x5D6440 (with the name box)
	virtual void Unknown_5C();							// +0x5C 0x5D6480 (with the name box)
	virtual void Unknown_60();							// +0x60 0x5D64C0 (the dialog's answer: G_ASSERT 'choice == USER_SELECT_OK')
	virtual void Unknown_64();							// +0x64 0x5D6530 (the entry window's)
	virtual void Unknown_7C(UIViewClass *view, bool arg);	// +0x7C 0x5D6400 (the name box off the view first)  // NAME 1 [view, arg]: intrface.h's
	virtual void Unknown_88(const RefCountPtr<RenderObjClass> &robj, const char *bone_name);	// +0x88 0x5D6370 (the name box first)
	virtual void Unknown_8C(const RefCountPtr<RenderObjClass> &robj);	// +0x8C 0x5D63C0 (the name box first)
	virtual bool Unknown_A0();							// +0xA0 0x5D6550 (FAIL_IF '!Get_Client()': the petition sent with the names)
	virtual bool Show_Dialog(const char *type);			// +0xA4 0x5D6840 (the entry window's)
	virtual bool Close_Dialog();						// +0xA8 0x5D6870 (the entry window's)
	virtual bool Has_Text(bool warn);					// +0xAC 0x5D6890 (the names first; else 'PETITION_PLAYER_ENTRY_NO_DATA' when warn)

	RefCountPtr<UIPetitionPlayerNameClass> PlayerName;	// +0xB4 (the name box)  // NAME 1: what it holds
	int Unknown0B8;										// +0xB8 (the packet's second GameID)
};

#endif // UI_PETITION_PLAYER_ENTRY_H
