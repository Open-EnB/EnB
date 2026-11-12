// Earth & Beyond client (reconstruction): app\client\ui_petition_entry.h, the petition's entry window (its model
// 'hsi02a', 'fa_hsi02a.ini'; the tooltips' section 'UI_PETITION_ENTRY'). The client's asserts name the file
// (e:\build\beta\G\app\Client\ui_petition_entry.cpp); this header's name follows it (NAME 1, provisional).
#ifndef UI_PETITION_ENTRY_H  // NAME 1 [ui_petition_entry.h]: provisional header name for ui_petition_entry.cpp
#define UI_PETITION_ENTRY_H

#include <string>
#include "ui_chat_panel_base.h"

class GadgetSlider;
class TextGadgetClass;
class TextEditorGadgetClass;
class ViewUnknown34Class;

// The dialog's answers (ViewUnknown34Class::Unknown_5890B0; 0 none yet): the asserts 'choice == USER_SELECT_NO' (0x5D4CDA
// compares 2) and 'choice == USER_SELECT_OK' (0x5D64C0 compares 1). inv_transaction.cpp's drop dialog gives NO 1.
enum { USER_SELECT_YES = 1, USER_SELECT_OK = 1, USER_SELECT_NO = 2 };	// NAME 3 [USER_SELECT_NO, USER_SELECT_OK]: the asserts'; NAME 1 [USER_SELECT_YES]: neutral

// The entry window (0xB4 bytes, tables 0xAF50D4 / 0xAF50C8 / 0xAF50BC): a subject line and a details editor with
// its slider, exit / back / submit. Leaving with text typed asks first (the dialog 'PETITION_ENTRY_CANCEL'); submit
// without enough text says so ('PETITION_ENTRY_NO_DATA'). The petition's pages derive from it (ui_petition_stuck.cpp
// 0x5D7370, ui_petition_player_entry.cpp 0x5D6080) and send it with slot +0xA0.
class UIPetitionEntryClass : public ChatPanelUIBase	// NAME 1: neutral, after the file and its tooltips' section 'UI_PETITION_ENTRY'
{
public:
	UIPetitionEntryClass(SClient *client);				// 0x5D46E0 (deleting 0x5D4770)  // NAME 1 [client]: ChatPanelUIBase's
	virtual ~UIPetitionEntryClass();					// 0x5D47A0
	virtual void Unknown_54();							// +0x54 0x5D4890 ('hsi02a': the buttons, the slider, the details editor, the subject line)
	virtual void Unknown_58();							// +0x58 0x5D4C20 (Pending cleared; the base's)
	virtual void Unknown_5C();							// +0x5C 0x5D4C40 (InterfaceClass's)
	virtual void Unknown_60();							// +0x60 0x5D4C60 (FAIL_IF '!SubjectEditor'; the editors' frame, the dialog's answer)
	virtual void Unknown_64();							// +0x64 0x5D4DC0 (empty)
	virtual bool Unknown_68(int key);					// +0x68 0x5D4DE0 (1 exit, 2 back, 3 submit, 4 / 6 the subject's focus, 7 the slider)  // NAME 1 [key]: InterfaceClass's
	virtual void Unknown_94();							// +0x94 0x5D4F60 (exit: asks first when text was typed)
	virtual bool Unknown_A0() { return(true); }			// +0xA0 0x5D4750 (submit: the pages send the petition; ui_petition_stuck.cpp's 0x5D7420)
	virtual bool Show_Dialog(const char *type);			// +0xA4 0x5D53E0 (Confirm made over the view with the dialog type; the editors off)  // NAME 1 [Show_Dialog, type]: what it does; ViewUnknown34Class's
	virtual bool Close_Dialog();						// +0xA8 0x5D55E0 (FAIL_IF '!Confirm': Confirm released, the details editor on)  // NAME 1: what it does
	virtual bool Has_Text(bool warn);					// +0xAC 0x5D4FD0 (subject and details typed, more than 3 printable characters each; else 'PETITION_ENTRY_NO_DATA' when warn)  // NAME 1 [Has_Text, warn]: what it does

	void Get_Text(std::string &subject, std::string &details);	// 0x5D5200 (ui_petition_player_entry.cpp 0x5D6550, ui_petition_stuck.cpp 0x5D7420)  // NAME 1 [Get_Text, subject, details]: what it does
	static bool Has_Printable(const char *text, unsigned int count);	// 0x5D5670 (more than count characters above ' '; ui_petition_player_entry.cpp calls it, 0x5D4FD0 inlines it)  // NAME 1 [Has_Printable, text, count]: what it does
	LineDisplayClientClass *Get_Client() { return(Client); }	// (inline: the pages' FAIL_IFs '!Get_Client()', 0x5D6550 / 0x5D7420, as UICinfoClass's)  // NAME 3: the FAIL_IFs'

	GadgetSlider *Slider;								// +0xA0 (the details' slider, id 7)  // NAME 2: what it is
	TextGadgetClass *SubjectEditor;						// +0xA4 'Entry_Field00' (id 8, 'SUBJECT_EDIT_hsi02a')  // NAME 3: the FAIL_IF '!SubjectEditor'
	TextEditorGadgetClass *DetailsEditor;				// +0xA8 'message' (id 6, the tooltip 'DETAILS')  // NAME 2: its tooltip
	RefCountPtr<ViewUnknown34Class> Confirm;			// +0xAC (the dialog Show_Dialog opens)  // NAME 3: the FAIL_IF '!Confirm'
	int Pending;										// +0xB0 (what Closed becomes when the cancel dialog says yes)  // NAME 2: what it holds
};

#endif // UI_PETITION_ENTRY_H
