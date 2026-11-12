#include "ui_item_examination_options.h"
#include "dynamic_window_text.h"
#include "dynamic_window_toggle.h"
#include "gd_dynamicelement.h"
#include "gadget_collection_interface.h"
#include "unknown_signalbase.h"
#include "..\shared\g_assert.h"
#include <algorithm>

UIItemExaminationOptionsClass::UIItemExaminationOptionsClass(GadgetCollectionInterface *listener) :
	DynamicWindowVerticalClass(0x82),
	Listener(listener)
{
	Frame = Create_NEW(new DynamicWindowBackgroundClass("ITEM_EXAMINATION_OPTIONS", 2));
	RefCountPtr<DynamicWindowVerticalClass> contents = Create_NEW(new DynamicWindowVerticalClass(6));
	RefCountPtr<DynamicWindowTextClass> title = DynamicWindowTextClass::Create("OPTIONS_IEI00", "Options", 0);
	contents->Add_Element(title);
	List = Create_NEW(new DynamicWindowVerticalClass(6));
	contents->Add_Element(List);
	Frame->Set_Contents(contents);
}

UIItemExaminationOptionsClass::~UIItemExaminationOptionsClass()
{
	List->Remove_All_Children(true);
	Remove_All_Children(true);
	Options.clear();
	List = NULL;
}

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

UIItemExaminationOptionClass::UIItemExaminationOptionClass(const RefCountPtr<ClientItemPropertyOptionClass> &option, GadgetCollectionInterface *listener) :
	DynamicWindowHorizontalClass(6),
	Option(option),
	Listener(listener),
	Gadget(NULL)
{
	static Vector2 min_size(24.0f, 0.0f);
	static Vector2 button_size(16.0f, 16.0f);
	std::vector<std::string> textures;
	textures.size(); textures.size(); textures.size(); textures.size();	// dead reads: the client's inline budget (MemberCallback's constructor 0x536F40 out of line)
	textures.push_back("if_check_off.tga");
	textures.push_back("if_check_on.tga");
	Toggle = ToggleButtonDynamicWindow::Create(textures, button_size, listener, 0);
	Toggle->Set_Min_Size(min_size);
	Toggle->Set_State(option->Unknown_14() ? 1 : 0);
	Unknown_4F7EF0().Add(Toggle.Peek(), this, &UIItemExaminationOptionClass::On_Toggle_State, ToggleButtonStateEvent());
	Add_Element(Toggle);
	RefCountPtr<DynamicWindowTextClass> label = DynamicWindowTextClass::Create("OPTION00_IEI00", option->Label, 0x12);
	Add_Element(label);
	std::string tooltip = option->Get_Tooltip();
	if (!tooltip.empty()) {
		Gadget = new DynamicElementGadgetClass(this);
		Gadget->Set_Unknown14(tooltip.c_str());
		GadgetClass::Set_Tooltip_Name(Gadget, "ItemOptionObject");
		Listener->Unknown84_08(Gadget, Listener->Unknown84_10(), -1);
	}
}

UIItemExaminationOptionClass::~UIItemExaminationOptionClass()
{
	Unknown_4F7EF0().disconnect(Toggle.Peek(), this, &UIItemExaminationOptionClass::On_Toggle_State, ToggleButtonStateEvent());
	if (Gadget != NULL) {
		Listener->Unknown84_0C(Gadget);
		GadgetClass::Destroy(Gadget);
		Gadget = NULL;
	}
	Remove_All_Children(true);
}














































































void UIItemExaminationOptionClass::On_Toggle_State(ToggleButtonStateEvent *event, ToggleButtonDynamicWindow *toggle)
{
	FAIL_IF(!event) return;
	FAIL_IF(!Option) return;
	Option->Unknown_18(event->State != 0);
}

/*
**	Reconstruction notes (from the retail client.exe, SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Client\ui_item_examination_options.cpp (its __FILE__: the
**	option row's FAIL_IFs '!event' / '!Option', lines 175 / 176, 0x536450).
**
**	Partial (IE1, run 2026-11-05-night): the options panel's constructor 0x5352D0, destructor 0x5357E0 (deleting 0x5357B0),
**	Add_Option 0x535970, Rebuild 0x535A10 and Clear 0x535BF0 exact, with
**	the copies they bring: Create_NEW 0x536610, std::_Sort_0 0x536B20 / _Sort 0x537080 / copy_backward
**	0x536ED0 over the options and RefCountPtr<ClientItemPropertyOptionClass>'s copy 0x536EA0, assignment
**	0x536A80 and destructor 0x5365E0. IE3 (run 2026-11-10): the row's destructor 0x5362F0 (deleting 0x5362C0) and
**	its toggle callback 0x536450 exact with the MemberCallback's slots 0x536F70-0x537050, and the row's constructor
**	0x535CB0 (the MemberCallback's constructor 0x536F40 out of line: four dead size() reads spend the client's inline
**	budget; SignalMap::Add forwarding to the template connect does it too but loses radar_data 0x7BB1D0 and ui_scan
**	0x6100C0). The toggle's event is built through ToggleButtonStateEvent's out-of-line
**	constructor 0x51E7F0 (dynamic_window_toggle.cpp).
*/
