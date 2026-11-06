#include "ui_item_examination_options.h"
#include "..\shared\unknown_debug.h"
#include <algorithm>

void UIItemExaminationOptionsClass::Add_Option(ClientItemPropertyOptionClass *option)
{
	RefCountPtr<ClientItemPropertyOptionClass> ptr = Create_Peek(option);
	Options.push_back(ptr);
}

void UIItemExaminationOptionsClass::Rebuild()
{
	Remove_All_Children(false);
	if (Options.size() != 0) {
		Add_Element(Frame);
		std::sort(Options.begin(), Options.end());
		for (std::vector< RefCountPtr<ClientItemPropertyOptionClass> >::iterator it = Options.begin(); it != Options.end(); ++it) {
			RefCountPtr<UIItemExaminationOptionClass> option = Create_NEW(new UIItemExaminationOptionClass(*it, Listener));
			List->Add_Element(option);
		}
	}
}

void UIItemExaminationOptionsClass::Clear()
{
	List->Remove_All_Children(true);
	Remove_All_Children(false);
	Options.clear();
}

/*
**	Reconstruction notes (from the retail client.exe, SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Client\ui_item_examination_options.cpp (its __FILE__: the
**	option row's FAIL_IFs '!event' / '!Option', lines 175 / 176, 0x536450).
**
**	Partial (IE1, run 2026-11-05-night): the options panel's Add_Option 0x535970, Rebuild 0x535A10 and Clear 0x535BF0 exact, with
**	the copies they bring: Create_NEW 0x536610, std::_Sort_0 0x536B20 / _Sort 0x537080 / copy_backward
**	0x536ED0 over the options and RefCountPtr<ClientItemPropertyOptionClass>'s copy 0x536EA0, assignment
**	0x536A80 and destructor 0x5365E0. Not yet here: the panel's constructor 0x5352D0, the
**	row's constructor 0x535CB0, destructor 0x5362F0 (deleting 0x5362C0) and 0x536450 / 0x5364D0.
*/
