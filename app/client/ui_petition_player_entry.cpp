// Earth & Beyond client (reconstruction): app\client\ui_petition_player_entry.cpp, the petition's page about other
// players over the entry window (ui_petition_entry.cpp) and its name box. The client's asserts name the file; they
// sit on the client's lines (D25). Notes at the end.
#include <string>
#include "ui_petition_player_entry.h"
#include "client.h"
#include "co.h"
#include "gadget.h"
#include "textgadg.h"
#include "..\shared\auxdata\auxdata.h"
#include "..\shared\auxdata\delta_string.h"
#include "..\shared\unknown_4be9d0.h"
#include "..\packets\pkpetitionstuck.h"
#include "..\audio\soundlib.h"
#include "..\shared\g_assert.h"

extern const int Unknown_B08C9C;			// 0xB08C9C (8: textgadg.h's flags, as ui_registration.cpp declares it)  // NAME 1: neutral (D10)

// 0x5D6080
UIPetitionPlayerEntryClass::UIPetitionPlayerEntryClass(SClient *client, int unknown0B8) :
	UIPetitionEntryClass(client),
	Unknown0B8(unknown0B8)
{
}

// 0x5D6100
UIPetitionPlayerEntryClass::~UIPetitionPlayerEntryClass()
{
	PlayerName = NULL;
}

// 0x5D61E0
void UIPetitionPlayerEntryClass::Unknown_54()
{
	UIPetitionEntryClass::Unknown_54();
	PlayerName = Create_NEW(new UIPetitionPlayerNameClass((SClient *)((ClientClass *)Client)->Unknown_18()));
	PlayerName->Unknown_5417D0(this);
	PlayerName->Unknown_54();
	PlayerName->NameEditor->Next = SubjectEditor;
	if (View) {
		View->Unknown_6B4940(PlayerName.Peek());
	}
}

// 0x5D6370
void UIPetitionPlayerEntryClass::Unknown_88(const RefCountPtr<RenderObjClass> &robj, const char *bone_name)
{
	PlayerName->Unknown_88(robj, bone_name);
	ChatPanelUIBase::Unknown_88(PlayerName->InterfaceMesh, "TRANS_BONE");
}

// 0x5D63C0
void UIPetitionPlayerEntryClass::Unknown_8C(const RefCountPtr<RenderObjClass> &robj)
{
	PlayerName->Unknown_8C(robj);
	ChatPanelUIBase::Unknown_8C(PlayerName->InterfaceMesh);
}

// 0x5D6400
void UIPetitionPlayerEntryClass::Unknown_7C(UIViewClass *view, bool arg)
{
	PlayerName->Unknown_7C(view, 0);
	InterfaceClass::Unknown_7C(view, arg);
}

// 0x5D6440
void UIPetitionPlayerEntryClass::Unknown_58()
{
	if (!Active) {
		UIPetitionEntryClass::Unknown_58();
		if (Active) {
			PlayerName->Unknown_58();
		}
	}
}

// 0x5D6480
void UIPetitionPlayerEntryClass::Unknown_5C()
{
	if (Active) {
		UIPetitionEntryClass::Unknown_5C();
		if (!Active) {
			PlayerName->Unknown_5C();
		}
	}
}
























































































// 0x5D64C0
void UIPetitionPlayerEntryClass::Unknown_60()
{
	if (Confirm) {
		int choice = Confirm->Unknown_5890B0("PETITION_PLAYER_ENTRY_NO_DATA");	// NAME 3: the assert's
		if (choice) {
			G_ASSERT(choice == USER_SELECT_OK);
			Close_Dialog();
		}
	}
	UIPetitionEntryClass::Unknown_60();
}



// 0x5D6530
void UIPetitionPlayerEntryClass::Unknown_64()
{
	UIPetitionEntryClass::Unknown_64();
}

// NOT EXACT: register (the client loads the client's +0x1130 into eax, ours edx; as ui_petition_stuck.cpp's 0x5D7420)
// 0x5D6550
bool UIPetitionPlayerEntryClass::Unknown_A0()
{
	FAIL_IF(!Get_Client()) return(false);
	std::string names;	// NAME 1: neutral (D10)
	if (!PlayerName->Get_Names(names)) {
		Show_Dialog("PETITION_PLAYER_ENTRY_NO_DATA");
		return(false);
	}
	std::string subject;
	std::string details;
	Get_Text(subject, details);
	((ClientClass *)Get_Client())->Send_Packet(&PetitionStuckPacketClass(((ClientClass *)Get_Client())->ShipID, Unknown0B8, subject.c_str(), details.c_str(), names.c_str()));
	return(true);
}

// 0x5D6840
bool UIPetitionPlayerEntryClass::Show_Dialog(const char *type)
{
	return(UIPetitionEntryClass::Show_Dialog(type) ? true : false);
}

// 0x5D6870
bool UIPetitionPlayerEntryClass::Close_Dialog()
{
	return(UIPetitionEntryClass::Close_Dialog());
}

// 0x5D6890
bool UIPetitionPlayerEntryClass::Has_Text(bool warn)
{
	std::string names;	// NAME 1: neutral (D10)
	if (!PlayerName->Get_Names(names)) {
		if (warn) Show_Dialog("PETITION_PLAYER_ENTRY_NO_DATA");
		return(false);
	}
	return(UIPetitionEntryClass::Has_Text(warn));
}


























// 0x5D6AA0
UIPetitionPlayerNameClass::UIPetitionPlayerNameClass(SClient *client) :
	ChatPanelUIBase(client),
	NameEditor(NULL)
{
}

// 0x5D6B20
UIPetitionPlayerNameClass::~UIPetitionPlayerNameClass()
{
	GadgetClass::Destroy(NameEditor);
	NameEditor = NULL;
}

// NOT EXACT: shape (the client stores the name box's +0x58 and +0x38 through one load; a local inline setter leaves only registers, CP3)
// 0x5D6BC0
void UIPetitionPlayerNameClass::Unknown_54()
{
	if (Unknown_98("hsi03a", "fa_hsi03a.ini")) {
		GadgetClass *button = Add_Button(1, "exit_Import", NULL);	// NAME 1: neutral (D10)
		GadgetClass::Set_Tooltip(button, "UI_PETITION_PLAYER_ENTRY", "IMPORT");
		button = Add_Button(2, "Entry_Field01", NULL);
		GadgetClass::Set_Tooltip(button, "UI_PETITION_PLAYER_ENTRY", "EDIT");
		Add_Back_Gadget(1, Vector3(0.0f, 0.0f, 0.0f));
		RefCountPtr<RenderObjClass> entry = Create_Get(InterfaceMesh->Get_Sub_Object_By_Name("Entry_Field01"));
		NameEditor = new TextGadgetClass(this, 0xFA, "PLAYER_NAME_hsi03a", entry.Peek(), Unknown_B08CB0 | Unknown_B08C9C, 0);
		NameEditor->Set_Text("Enter name or click Import Name", 0);
		NameEditor->Unknown3B = false;
		NameEditor->Unknown58 = 0.9f;
		NameEditor->Changed = true;
		Unknown64.Unknown_6870E0(NameEditor, 8, View, 1, 1);
	}
}

// 0x5D6DC0
void UIPetitionPlayerNameClass::Unknown_60()
{
	FAIL_IF(!NameEditor) return;
	if (NameEditor->Active) NameEditor->Set_Text(Unknown_4BE9D0(NameEditor->Get_Text()).c_str(), 0);
	NameEditor->Unknown_6AAD10();
}

// NOT EXACT: inline-budget (the client calls rtti.h's DeltaStringClass cast 0x4099A8 out of line at both lookups, ours inlines it; as co.cpp's 0x6FF4B0)
// 0x5D6EE0
bool UIPetitionPlayerNameClass::Unknown_68(int key)
{
	FAIL_IF(!NameEditor) return(false);
	switch (key) {
		case 2:
			NameEditor->Unknown_74(true);
			return(true);
		case 1:
			{

				SClient *sclient = (SClient *)((ClientClass *)Client)->Unknown_18();	// NAME 3: the FAIL_IF's
				FAIL_IF(!sclient) return(true);
				ClientObjectClass *target = sclient->Get_Target();	// NAME 1: neutral (D10)
				if (target && target->Type == 1) {
					SoundLibraryClass::Play_2D_Sound("fontclick.wav", 2);
					NameClass owner("Owner");
					DeltaStringClass *t = Unknown_Find_Field<DeltaStringClass>((DeltaCompoundClass *)target->AuxData, owner);	// NAME 1: neutral (D10)
					if (!t || !t->Unknown70) {
						NameClass field("Name");
						t = Unknown_Find_Field<DeltaStringClass>((DeltaCompoundClass *)target->AuxData, field);
					}
					const char *name = (t && t->Unknown70) ? t->String : "Unknown";	// NAME 1: neutral (D10)
					bool append = NameEditor->Unknown3B;	// NAME 1: neutral (D10)
					if (append) {
						NameEditor->Set_Text("; ", true);
					}
					NameEditor->Set_Text(name, append);
					NameEditor->Unknown3B = true;
					return(true);
				}
				sclient->Unknown_74D990(0x13, "Player target must be selected!", 0);
				SoundLibraryClass::Play_2D_Sound("failure_1.wav", 2);
			}
			return(true);
	}
	return(false);
}

// 0x5D7150
bool UIPetitionPlayerNameClass::Get_Names(std::string &names)
{
	if (!NameEditor->Unknown3B) return(false);
	names = NameEditor->Get_Text();
	if (!UIPetitionEntryClass::Has_Printable(names.c_str(), 2)) {
		names.erase();
	}
	return(names.length() != 0);
}

/*
**	Notes.
**	Object 0x5D6080-0x5D736F: 0x5D7290 is Create_NEW<UIPetitionPlayerNameClass> (out of line), 0x5D72C0 the
**	RefCountPtr's destructor (the unwinds'), 0x5D7330-0x5D7360 the adjustor thunks of both classes' second and third
**	tables; ui_petition_stuck.cpp's 0x5D7370 follows. The name box's constructor and Get_Names have no callers: the
**	page inlines them (0x5D61E0, 0x5D6550, 0x5D6890).
*/
