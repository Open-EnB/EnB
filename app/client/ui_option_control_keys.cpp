// Earth & Beyond client (reconstruction): app\client\ui_option_control_keys.cpp, the options' controls page ('tsi31a').
// The client's asserts name the file; the asserts sit on its lines (D25). Notes at the end.
#include <stdio.h>
#include <string.h>
#include "ui_option_control_keys.h"
#include "ui_chat_panel.h"
#include "client.h"
#include "gadget.h"
#include "gd_robj.h"
#include "gd_slider.h"
#include "unknown_66b770.h"
#include "fontmanager.h"
#include "control_keys_manager.h"
#include "..\input\inpkeys.h"
#include "..\audio\soundlib.h"
#include "..\shared\g_assert.h"

extern FontManagerClass *Unknown_BE67C4;	// NAME 1: neutral (D10)

// gd_key_press.cpp's gadget (0x48 bytes, table 0xAFA548), as far as this page uses it
class KeyPressGadget : public GadgetClass	// NAME 5: the client's (the queue's class name for gd_key_press.cpp)
{
public:
	KeyPressGadget();									// 0x66A700 (mask 0xFFFF; off)
	virtual bool Unknown_30(float x, float y);			// +0x30 (GadgetClass's pure slot)  // NAME 1 [x, y]: GadgetClass's
	void Unknown_66AAD0(const RefCountPtr<FontSys> &font);	// 0x66AAD0 (the text the key is shown in)  // NAME 1 [font]: its type

	RefCountPtr<FontSys> Unknown30;						// +0x30
	KernelEventClass Event;								// +0x34 (the key taken)  // NAME 2: what it holds
	int Unknown44;										// +0x44
};

// The buttons Unknown_54 makes, ids 1-11 (0xB82F58)
static const char *Unknown_B82F58[CONTROL_KEYS_ROWS] = { "exit", "BUTTON03", "BUTTON00", "BUTTON01", "BUTTON02", "TOP_BUTTON00", "TOP_BUTTON01", "TOP_BUTTON02", "TOP_BUTTON03", "TOP_BUTTON04", "TOP_BUTTON05" };	// NAME 1: neutral (D10)

// kernel.h's input event (KernelEventClass), as ui_option_main.cpp's (Unknown_54's Escape)










static int Unknown_BE5D1C;								// 0xBE5D1C (Set_Key_Text: the buttons' width, measured once)  // NAME 1: neutral (D10)

// 0x5CA010
UIOptionControlKeysClass::UIOptionControlKeysClass(SClient *client, bool mode) :
	ChatPanelUIBase(client),
	Context(0),
	Slider(NULL),
	TopRow(0),
	KeyPress(NULL),
	EditKey(NULL),
	EditPrimary(false),
	DialogBoxID(NULL),
	Mode(mode)
{
	memset(Primary, 0, sizeof(Primary));
	memset(Secondary, 0, sizeof(Secondary));
	memset(Buttons, 0, sizeof(Buttons));
}

// 0x5CA1B0
UIOptionControlKeysClass::~UIOptionControlKeysClass()
{
	Slider = NULL;
	KeyPress = NULL;
	for (int lp = 0; lp < CONTROL_KEYS_ROWS; lp++) {
		KeyName[lp] = NULL;
		HardCodedKey[lp] = NULL;
		Primary[lp] = NULL;
		Secondary[lp] = NULL;
	}
}

// 0x5CA330
void UIOptionControlKeysClass::Unknown_54()
{
	if (Unknown_98("tsi31a", "tsi31a.ini")) {
		((ControlKeyListClass *)&Unknown4C)->Add(KernelEventClass(0x1B), (void *)1);
		std::string text;
		for (int i = 0; i < CONTROL_KEYS_ROWS; i++) {
			int id = i + 1;								// NAME 1: neutral (D10)
			if (id == 1) {
				Buttons[i] = Add_Button(id, Unknown_B82F58[i], NULL);
			} else {
				text = Unknown_B82F58[i];
				text += "_tsi31a";
				Buttons[i] = Add_Button(id, Unknown_B82F58[i], text.c_str());
			}
			GadgetClass::Set_Tooltip(Buttons[i], "UI_OPTION_CONTROL_KEYS", Unknown_B82F58[i]);
		}
		for (int lp = 0; lp < CONTROL_KEYS_ROWS; lp++) {
			char name[64];
			char bttn_name[64];
			sprintf(name, "LABEL_TEXT%02d_tsi31a", lp);
			KeyName[lp] = Create_Peek(Unknown_BE67C4->Get_Style(name, InterfaceMesh.Peek()));
			sprintf(bttn_name, "Prime_Key%02d", lp);
			sprintf(name, "PRIME_KEY%02d_tsi31a", lp);
			Primary[lp] = Add_Button(lp + 15, bttn_name, name);
			((Unknown_66B770Class *)Primary[lp])->Unknown60 |= 5;
			HardCodedKey[lp] = Create_Peek(Unknown_BE67C4->Get_Style(name, InterfaceMesh.Peek()));
			sprintf(bttn_name, "Alt_Key%02d", lp);
			sprintf(name, "ALT_KEY%02d_tsi31a", lp);
			Secondary[lp] = Add_Button(lp + 26, bttn_name, name);
			((Unknown_66B770Class *)Secondary[lp])->Unknown60 |= 5;
		}
		Slider = Add_Slider(13, false);
		Slider->Unknown_66E460(InterfaceMesh, false, 0);
		KeyPress = new KeyPressGadget;
		Unknown64.Unknown_6870E0(KeyPress, 14, View, true, false);
		Add_Back_Gadget(0, Vector3(0.0f, 0.0f, 0.0f));
		InterfaceClass::Unknown_54();
		Set_Context(0);
	}
}











































// 0x5CA910
void UIOptionControlKeysClass::Unknown_58()
{
	if (!Active) {
		ControlKeysManager *key_manager = ControlKeysManager::Get((SClient *)((ClientClass *)Client)->Unknown_18());
		FAIL_IF(!key_manager) return;
		ChatPanelUIBase::Unknown_58();
	}
}

// 0x5CA980
void UIOptionControlKeysClass::Unknown_5C()
{
	if (Active) {
		if (DialogBoxUI) {
			DialogBoxUI->Unknown_5C();
			DialogBoxUI = NULL;
			DialogBoxID = NULL;
		}
		InterfaceClass::Unknown_5C();
	}
}
// NOT EXACT: register allocation (similarity 0.69): the client computes the row (key - 15 / key - 26) with lea into eax, this form adds into ebx; the rest aligns
// 0x5CA9F0
bool UIOptionControlKeysClass::Unknown_68(int key)
{
	if (DialogBoxUI) return(true);

	SClient *sclient = (SClient *)((ClientClass *)Client)->Unknown_18();	// NAME 1: neutral (D10)
	ChatPanelInterface *chat = sclient->ChatPanel.Peek();	// NAME 1: neutral (D10)
	if (key == 14) return(true);

	if (EditKey) {
		KeyPress->Unknown_40(false);
		EditKey = NULL;
		Refresh();
	}











	ControlKeysManager *key_manager = ControlKeysManager::Get((SClient *)((ClientClass *)Client)->Unknown_18());
	FAIL_IF(!key_manager) return(true);
	if (key != 13) {
		SoundLibraryClass::Play_2D_Sound("Roll_Select02b.wav", 2);
	}
	switch (key) {
		case 1:
			key_manager->Restore_Keys();
			Unknown_5C();
			Closed = 1;
			return(true);
		case 2:
			key_manager->Restore_Keys();
			Unknown_5C();
			Closed = 1;
			if (!Mode) {
				chat->Unknown_565F30(1);
			}
			return(true);
		case 3:
			key_manager->Save();
			Unknown_5C();
			Closed = 1;
			if (!Mode) {
				chat->Unknown_565F30(1);
			}
			return(true);
		case 4:
			key_manager->Set_Default_PSW();
			Set_Context(Context);
			break;
		case 5:
			key_manager->Set_Default_SIM();
			Set_Context(Context);
			break;
		case 6:
		case 7:
		case 8:
		case 9:
		case 10:
		case 11:
			Set_Context(Key_To_Context(key));
			break;
		case 13:
			if ((int)Slider->Get_Value() != TopRow) {
				Refresh();
			}
			break;
	}

	GadgetClass *bttn;									// NAME 1: neutral (D10)
	int lp;												// NAME 1: neutral (D10)
	if (key >= 15 && key <= 25) {
		bttn = Primary[key - 15];
		lp = key - 15;
		EditPrimary = true;
	} else if (key >= 26 && key <= 36) {
		bttn = Secondary[key - 26];
		lp = key - 26;
		EditPrimary = false;
	} else {
		return(false);
	}
	if (!bttn) return(false);















	std::vector<KeyMappingStruct *>::iterator keys_it = CurrentKeys.begin();
	int count = (int)(Slider->Get_Value() + lp);		// NAME 1: neutral (D10)
	while (count--) {
		FAIL_IF(keys_it == CurrentKeys.end()) break;
		keys_it++;
	}

	if (keys_it != CurrentKeys.end()) {
		EditKey = *keys_it;
		if (EditKey) {
			KeyPress->Unknown_66AAD0(Create_Peek(((RObjGadgetClass *)bttn)->Unknown34));
			KeyPress->Unknown_40(true);
			return(true);
		}
		G_ASSERT(false);
	}
	return(true);
}










// NOT EXACT: shape (similarity 0.58): functional from the client's strings and calls; the message strings' assembly and the refresh flag are guesses
// 0x5CB000
void UIOptionControlKeysClass::Unknown_60()
{
	if (EditKey && !DialogBoxUI && !KeyPress->Unknown_60()) {
		ControlKeysManager *key_manager = ControlKeysManager::Get((SClient *)((ClientClass *)Client)->Unknown_18());
		FAIL_IF(!key_manager) return;
		KernelEventClass &event = KeyPress->Event;		// NAME 1: neutral (D10)
		bool allowed = true;							// NAME 1: neutral (D10)
		switch (event.Key) {
			case 0:
			case 0x2C:
			case 0x5B:
			case 0x5C:
				allowed = false;
				break;
			default:
				if (event.Flags == 0x400) {
					const char *name = Unknown_42B6A0(event.Key).c_str();	// NAME 1: neutral (D10)
					if (name[0] != '\0' && name[1] == '\0') {
						if (name[0] >= 'A' && name[0] <= 'Z') allowed = false;
						if (name[0] >= '0' && name[0] <= '9') allowed = false;
					}
				}
				break;
		}
		bool refresh = true;							// NAME 1: neutral (D10)
		if (allowed) {
			int set = EditPrimary ? 0 : 1;				// NAME 3: control_keys_manager.cpp's 'set'
			if (event.Key == 0x1B && (event.Flags & 0x700) == 0) {
				KernelEventClass combo = *(ControlKeyValueStruct *)&EditKey->Keys[set];	// NAME 1: neutral (D10)
				if (combo.Key == 0) {
					EditKey = NULL;
					allowed = false;
				} else {
					std::string key_name;				// NAME 1: neutral (D10)
					Unknown_42B7E0(*(KernelEventClass *)&combo, key_name);
					std::string text;					// NAME 1: neutral (D10)
					text = "Do you wish to clear ";
					text += key_name;
					text += " from ";
					text += EditKey->Get_Unknown10();
					text += "?";
					Show_Dialog_Box("YES_NO", text.c_str());
					event = KernelEventClass();
				}
			} else {
				ControlKeyValueStruct value(*(ControlKeyValueStruct *)&event);	// NAME 1: neutral (D10)
				KeyMappingStruct *tied = key_manager->Set_Key(EditKey, set, *(ControlKeyStruct *)&value, false);	// NAME 1: neutral (D10)
				if (!tied) {
					EditKey = NULL;
				} else {
					std::string text;					// NAME 1: neutral (D10)
					Unknown_42B7E0(event, text);
					text += " is already tied to:\n";
					text += tied->Get_Unknown10() + "\nDo you want to change it to:\n" + EditKey->Get_Unknown10() + "?";
					refresh = !Show_Dialog_Box("YES_NO", text.c_str());
				}
			}
		}
		if (!allowed) {
			std::string text;							// NAME 1: neutral (D10)
			if (event.Key == 0) {
				text = "Ctrl, Alt, and Shift cannot be used alone.  They can be used as modifiers to other keys.";
			} else {
				Unknown_42B7E0(event, text);
				text += "\nis not allowed as a hot key!\n";
			}
			Show_Dialog_Box("OK", text.c_str());
			EditKey = NULL;
		}
		if (refresh) {
			Refresh();
		}
	}



































	if (DialogBoxUI) {
		FAIL_IF(!DialogBoxID) return;
		DialogBoxUI->Unknown_588870();
		int button = DialogBoxUI->Unknown_5890B0(DialogBoxID);	// NAME 1: tsk_login.h's
		if (button) {
			if (button == 1 && EditKey) {
				ControlKeysManager *key_manager = ControlKeysManager::Get((SClient *)((ClientClass *)Client)->Unknown_18());








				FAIL_IF(!key_manager) return;
				ControlKeyValueStruct value(*(ControlKeyValueStruct *)&KeyPress->Event);	// NAME 1: neutral (D10)
				key_manager->Set_Key(EditKey, !EditPrimary, *(ControlKeyStruct *)&value, true);
			}
			DialogBoxUI->Unknown_5C();
			DialogBoxUI = NULL;
			DialogBoxID = NULL;
			EditKey = NULL;
			Refresh();
		}
	}
}










// 0x5CB8F0
void UIOptionControlKeysClass::Set_Context(int context)
{
	ControlKeysManager *key_manager = ControlKeysManager::Get((SClient *)((ClientClass *)Client)->Unknown_18());
	FAIL_IF(!key_manager) return;
	Context = context;
	key_manager->Find_Mappings(&CurrentKeys, context);
	Slider->Unknown_66FA90((float)CurrentKeys.size(), 11.0f, false);
	Slider->Unknown_A4(0.0f, false);
	Refresh();
}



























// 0x5CB9D0
void UIOptionControlKeysClass::Refresh()
{
	std::vector<KeyMappingStruct *>::iterator keys_it = CurrentKeys.begin();
	TopRow = (int)Slider->Get_Value();
	if (TopRow > 0) {
		keys_it += TopRow;
	}
	for (int lp = 0; lp < CONTROL_KEYS_ROWS; lp++) {
		FAIL_IF(!KeyName[lp]) continue;
		FAIL_IF(!Primary[lp]) continue;
		FAIL_IF(!Secondary[lp]) continue;
		FAIL_IF(!HardCodedKey[lp]) continue;
		if (keys_it == CurrentKeys.end()) {
			Primary[lp]->Unknown_40(false);
			Secondary[lp]->Unknown_40(false);
			HardCodedKey[lp]->Set_Hidden(true);
			KeyName[lp]->Change_Text("", 0);
			continue;
		}
		const std::string &hard_coded = (*keys_it)->Get_Unknown20();	// NAME 1: neutral (D10)
		if (hard_coded.length() == 0) {
			Primary[lp]->Unknown_40(true);
			Secondary[lp]->Unknown_40(true);
			std::string key_text;						// NAME 1: neutral (D10)
			Set_Key_Text(Primary[lp], (*keys_it)->Get_Key_String(0, key_text));
			Set_Key_Text(Secondary[lp], (*keys_it)->Get_Key_String(1, key_text));
		} else {
			Primary[lp]->Unknown_40(false);
			Secondary[lp]->Unknown_40(false);
			HardCodedKey[lp]->Change_Text(hard_coded.c_str(), 0);
			HardCodedKey[lp]->Set_Hidden(false);
			HardCodedKey[lp]->Unknown_6CE910(false);
		}
		KeyName[lp]->Change_Text((*keys_it)->Get_Unknown10().c_str(), 0);
		keys_it++;
	}
}











// 0x5CBCC0
int UIOptionControlKeysClass::Key_To_Context(int key)
{
	switch (key) {
		case 6: return(0);
		case 7: return(3);
		case 8: return(4);
		case 9: return(1);
		case 10: return(5);
		case 11: return(2);
		default:
			G_ASSERT(false);
			return(-1);
	}
}
// 0x5CBD50
int UIOptionControlKeysClass::Context_To_Key(int context)
{
	switch (context) {
		case 0: return(6);
		case 3: return(7);
		case 4: return(8);
		case 1: return(9);
		case 5: return(10);
		case 2: return(11);
		default:
			G_ASSERT(false);
	}
	return(6);
}

// 0x5CBDE0
bool UIOptionControlKeysClass::Show_Dialog_Box(const char *dialog_name, const char *text)
{
	FAIL_IF(!dialog_name) return(false);

	SClient *sclient = Client ? (SClient *)((ClientClass *)Client)->Unknown_18() : NULL;
	FAIL_IF(!sclient) return(false);
	FAIL_IF(!sclient->CurrentView) return(false);

	UIViewClass *uiview = sclient->CurrentView->Unknown_1C();
	FAIL_IF(!uiview) return(false);

	DialogBoxUI = Create_Peek(uiview->Get_Unknown34());
	FAIL_IF(!DialogBoxUI) return(false);
	DialogBoxID = dialog_name;
	DialogBoxUI->Unknown_588A70(dialog_name, false, text, 320, false, false);
	return(true);
}

// NOT EXACT: inline budget (similarity 0.56): the client calls the copy's _Tidy out of line (push 0, call) and keeps no zero register; this form inlines it
// 0x5CC030
void UIOptionControlKeysClass::Set_Key_Text(GadgetClass *bttn, const std::string &text)
{
	FAIL_IF(!bttn) return;

	FontSys *font = ((RObjGadgetClass *)bttn)->Unknown34;
	FAIL_IF(!font) return;




	if (Unknown_BE5D1C < 1.0f) {
		Unknown_BE5D1C = 320;
		FontStringStruct *sinfo = &font->Info;
		if (!sinfo) {
			G_ASSERT(sinfo);
		} else {
			RenderObjClass *sub_object = InterfaceMesh->Get_Sub_Object_By_Name(sinfo->Attach);
			if (!sub_object) {
				G_ASSERT(sub_object);
			} else {
				Unknown_BE5D1C = (int)(sub_object->Get_Bounding_Box().Extent.X * 2.0f);
			}
		}
	}

	std::string str(text);



	Font32Class *f32 = font->Font.Peek();
	G_ASSERT(f32);
	font->Change_Text(str.c_str(), 0);
}

/*
**	Notes.
**	Object 0x5CA010-0x5CC34F: the constructor 0x5CA010 opens it, after ui_option_check.cpp's adjustor thunks; the
**	template instances 0x5CC250 (the vector's _Tidy), 0x5CC290 / 0x5CC2C0 (RefCountPtr<ViewUnknown34Class>'s Create_Peek
**	and destructor) and this page's adjustor thunks 0x5CC330 / 0x5CC340 close it; ui_option_font.cpp's 0x5CC350 follows.
**	The chat panel's request 8 makes it (the options' main page's 'Control' button). Set_Context is inlined into
**	Unknown_54 and Unknown_68, Key_To_Context into Unknown_68; Context_To_Key has no caller.
*/
