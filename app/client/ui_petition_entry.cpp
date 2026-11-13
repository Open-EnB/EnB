// Earth & Beyond client (reconstruction): app\client\ui_petition_entry.cpp, the petition's entry window ('hsi02a').
// The client's asserts name the file. The asserts sit on the client's lines (D25). Notes at the end.
#include <stdio.h>
#include <string.h>
#include <string>
#include "ui_petition_entry.h"
#include "ui_chat_panel.h"
#include "client.h"
#include "gadget.h"
#include "gd_slider.h"
#include "gd_text_editor.h"
#include "textgadg.h"
#include "..\audio\soundlib.h"
#include "..\shared\g_assert.h"

// The dialog's answers (USER_SELECT_YES, USER_SELECT_NO): ui_petition_entry.h

// The buttons Unknown_54 makes: their models' names and their font styles (0xB83EC8 / 0xB83EDC; the unreferenced
// dword between them, 0xB83ED8, is 'EXIT_BUTTON_hsi02a')
static const char *Unknown_B83EC8[4] = { "EXIT_BUTTON", "BACK", "SUBMIT", "Entry_Field00" };	// NAME 1: neutral (D10)
static const char *Unknown_B83EDC[4] = { "BACK_hsi02a", "SUBMIT_hsi02a", NULL, NULL };		// NAME 1: neutral (D10)

// 0x5D46E0
UIPetitionEntryClass::UIPetitionEntryClass(SClient *client) :
	ChatPanelUIBase(client),
	Slider(NULL),
	SubjectEditor(NULL),
	DetailsEditor(NULL),
	Pending(0)
{
}

// 0x5D47A0
UIPetitionEntryClass::~UIPetitionEntryClass()
{
	GadgetClass::Destroy(SubjectEditor);
	SubjectEditor = NULL;
	GadgetClass::Destroy(DetailsEditor);
	DetailsEditor = NULL;
	GadgetClass::Destroy(Slider);
	Slider = NULL;
}

// NOT EXACT: shape (the client stores the subject box's +0x58 and +0x38 through one load, as an inline setter would; the message model's Create_NEW and the flags' registers; 0.77 with a local setter, CP3)
// 0x5D4890
void UIPetitionEntryClass::Unknown_54()
{
	if (Unknown_98("hsi02a", "fa_hsi02a.ini")) {
		for (int i = 1; i < 5; i++) {
			GadgetClass *button = Add_Button(i, Unknown_B83EC8[i - 1], Unknown_B83EDC[i - 1]);	// NAME 1: neutral (D10)
			GadgetClass::Set_Tooltip(button, "UI_PETITION_ENTRY", Unknown_B83EC8[i - 1]);
		}
		Add_Back_Gadget(1, Vector3(0.0f, 0.0f, 0.0f));
		Slider = new GadgetSlider(false);
		Slider->Unknown_66E460(InterfaceMesh, 0, 0);
		Slider->Unknown_66FA90(0, 0, true);
		RefCountPtr<RenderObjClass> message = Create_NEW(InterfaceMesh->Get_Sub_Object_By_Name("message"));
		DetailsEditor = new TextEditorGadgetClass(this, message, "UI_PETITION_STUCK_EDIT_FONT", Slider, 0x400, "Click here and enter description of problem, then press Submit to send...");
		GadgetClass::Set_Tooltip(DetailsEditor, "UI_PETITION_ENTRY", "DETAILS");
		DetailsEditor->Client = (SClient *)((ClientClass *)Client)->As_Space_Client();
		RefCountPtr<RenderObjClass> entry = Create_Get(InterfaceMesh->Get_Sub_Object_By_Name("Entry_Field00"));
		SubjectEditor = new TextGadgetClass(this, 0xFA, "SUBJECT_EDIT_hsi02a", entry.Peek(), Unknown_B08CB0, 0);
		SubjectEditor->Set_Text("Enter Subject Here", 0);
		SubjectEditor->Unknown3B = false;
		SubjectEditor->Unknown58 = 0.95f;
		SubjectEditor->Changed = true;
		Unknown64.Unknown_6870E0(Slider, 7, View, 1, 1);
		Unknown64.Unknown_6870E0(DetailsEditor, 6, View, 1, 1);
		Unknown64.Unknown_6870E0(SubjectEditor, 8, View, 1, 1);
		Unknown_10();
	}
}

// 0x5D4C20
void UIPetitionEntryClass::Unknown_58()
{
	Pending = 0;
	ChatPanelUIBase::Unknown_58();
}

// 0x5D4C40
void UIPetitionEntryClass::Unknown_5C()
{
	InterfaceClass::Unknown_5C();
}


































































// NOT EXACT: shape (case 0 tests the second answer with sete / test, ours jne; the closed value's store scheduled before pop edi in the client)
// 0x5D4C60
void UIPetitionEntryClass::Unknown_60()
{
	FAIL_IF(!SubjectEditor) return;
	DetailsEditor->Unknown_672AD0();
	SubjectEditor->Unknown_6AAD10();
	if (SubjectEditor->Unknown39) {
		SubjectEditor->Unknown39 = false;
		DetailsEditor->Take_Focus();
	}
	if (Confirm) {
		int choice = Confirm->Unknown_5890B0("PETITION_ENTRY_CANCEL");	// NAME 3: the assert's
		switch (choice) {
			case 0:
				if (Confirm->Unknown_5890B0("PETITION_ENTRY_NO_DATA") == USER_SELECT_YES) {
					Close_Dialog();
				}
				break;
			case USER_SELECT_YES:
				{
					((SClient *)((ClientClass *)Client)->As_Space_Client())->Unknown_74D990(0x13, "Petition canceled.", 0);
					int closed = Pending;	// NAME 1: neutral (D10)
					Unknown_5C();
					Closed = closed;
					Close_Dialog();
				}
				break;
			default:
				Pending = 0;





				G_ASSERT(choice == USER_SELECT_NO);
				Close_Dialog();
				break;
		}
	}
}

// 0x5D4DC0
void UIPetitionEntryClass::Unknown_64()
{
}

// 0x5D4DE0
bool UIPetitionEntryClass::Unknown_68(int key)
{
	if (Confirm) return(true);
	switch (key) {
		case 4:
			SubjectEditor->Unknown_74(true);
			return(true);
		case 2:
			if (Has_Text(false) && Show_Dialog("PETITION_ENTRY_CANCEL")) {
				Pending = 1;
				return(true);
			}
			Unknown_5C();
			Closed = 1;
			return(true);
		case 1:
			SoundLibraryClass::Play_2D_Sound("Roll_Select02b.wav", 2);
			Unknown_94();
			return(true);
		case 3:
			SoundLibraryClass::Play_2D_Sound("Roll_Select02b.wav", 2);
			if (Has_Text(true) && Unknown_A0()) {
				Unknown_5C();
				Closed = 2;
				return(true);
			}
			break;
		case 6:
			if (SubjectEditor->Active) {
				SubjectEditor->Unknown_74(false);
			}
			break;
		case 7:
			DetailsEditor->Unknown_672A60();
			return(false);
		default:
			return(false);
	}
	return(true);
}

// 0x5D4F60
void UIPetitionEntryClass::Unknown_94()
{
	if (Has_Text(false)) {
		if (Show_Dialog("PETITION_ENTRY_CANCEL")) {
			Pending = 2;
		}
	} else if (!Confirm) {
		Unknown_5C();
		Closed = 2;
	}
}

// 0x5D4FD0
bool UIPetitionEntryClass::Has_Text(bool warn)
{
	if (!DetailsEditor->Unknown60 && SubjectEditor->Unknown3B) {
		std::string text = DetailsEditor->Get_Text();	// NAME 1: neutral (D10)
		if (Has_Printable(text.c_str(), 3)) {
			text = SubjectEditor->Get_Text();
			if (Has_Printable(text.c_str(), 3)) {
				return(true);
			}
		}
	}
	if (warn) {
		Show_Dialog("PETITION_ENTRY_NO_DATA");
	}
	return(false);
}

// 0x5D5200
void UIPetitionEntryClass::Get_Text(std::string &subject, std::string &details)
{
	subject = SubjectEditor->Get_Text();
	details = DetailsEditor->Get_Text();
}

// 0x5D53E0
bool UIPetitionEntryClass::Show_Dialog(const char *type)
{
	if (Confirm) return(false);
	LayerClass layer((SceneClass *)View->Unknown_50(), (CameraClass *)View->Unknown_54());
	Confirm = Create_NEW(new ViewUnknown34Class(View, layer));
	Confirm->Unknown_54();
	View->Unknown_6B4940(Confirm.Peek());
	Confirm->Unknown_588A70(type, false, NULL, 0, false, false);
	DetailsEditor->Unknown_40(false);
	SubjectEditor->Unknown_74(false);
	return(true);
}





































// 0x5D55E0
bool UIPetitionEntryClass::Close_Dialog()
{
	FAIL_IF(!Confirm) return(false);
	Confirm->Unknown_7C(NULL, 0);
	Confirm = NULL;
	DetailsEditor->Unknown_40(true);
	return(true);
}

// 0x5D5670
bool UIPetitionEntryClass::Has_Printable(const char *text, unsigned int count)
{
	if (!text && !*text) return(false);
	unsigned int found = 0;	// NAME 1: neutral (D10)
	for (; *text; text++) {
		if (*text > ' ' && ++found > count) return(true);
	}
	return(false);
}

/*
**	Notes.
**	Object 0x5D46E0-0x5D573F: 0x5D56C0 is Create_NEW<ViewUnknown34Class> (out of line, Show_Dialog's), 0x5D5720 /
**	0x5D5730 the adjustor thunks of the second and third tables; ui_petition_player.cpp's 0x5D5740 follows. The
**	ui_option_main.cpp pattern (ChatPanelUIBase), the window's parts as ui_log.cpp's (the slider, the editor over
**	'message', a text gadget over 'Entry_Field00').
**	The dialog's answers (ViewUnknown34Class::Unknown_5890B0): 0 none yet, 1 yes, 2 no (the assert 'choice ==
**	USER_SELECT_NO' takes the default case past 0 and 1, the client compares 2; inv_transaction.cpp's enum gives 1 for
**	its drop dialog).
**	The button table names 'EXIT_BUTTON' with the font style 'BACK_hsi02a' and 'Entry_Field00' as button 4 (the
**	subject's focus): the client's tables as they are.
*/
