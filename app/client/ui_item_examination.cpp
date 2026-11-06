#include "ui_item_examination.h"
#include "ui_item_examination_attributes.h"
#include "ui_item_examination_options.h"
#include "gd_dynamicwindow.h"
#include "gadget.h"
#include "fontdatabase.h"
#include "control_keys_manager.h"
#include "..\shared\unknown_debug.h"

extern FontStringFilesClass *Unknown_BE67BC;	// NAME 1: neutral (D10), as sclient.h declares it

static Vector2 CloseButtonSize(16.0f, 16.0f);	// 0xBE5A50 (0x531630)  // NAME 1: what it sizes


























































UIItemExaminationClass::UIItemExaminationClass(LineDisplayClientClass *client, const RefCountPtr<UISlotInfoItemClass> &item) :
	UISlotInfoClass(client, "ITEM_EXAMINATION_MAIN"),
	Item(item),
	State(UI_CLOSED)
{
	UnknownA8 = false;
	UnknownA9 = false;
	Unknown_BE67BC->Load("FA_IEI00.INI");
	RefCountPtr<DynamicWindowVerticalClass> left = Create_NEW(new DynamicWindowVerticalClass(6));	// NAME 1 [left, right]: neutral
	RefCountPtr<DynamicWindowVerticalClass> right = Create_NEW(new DynamicWindowVerticalClass(6));
	GadgetClass *main_gadget = Unknown84_04();
	FAIL_IF(!main_gadget) return;

	Title = Create_NEW(new UIItemExaminationTitleClass());
	Add_Element(Title, 0);

	CloseButton = DynamicWindowIconClass::Create("if_x.tga", CloseButtonSize, 0x60);


	G_ASSERT(CloseButton);
	if (CloseButton) {
		DynamicWindowGadgetClass *gadget = new DynamicWindowGadgetClass(CloseButton, 0x55, 3, false);
		gadget->Set_Unknown14("Close this window");
		gadget->Unknown2C_06 = false;
		Unknown84_08(gadget, 1, -2);
		gadget->Unknown_65FFA0(-1000);
		gadget->Unknown5C_03 = true;
	}
	Add_Element(CloseButton, 0);

	Attributes = Create_NEW(new UIItemExaminationAttributesClass(this));
	left->Insert_Element(Attributes, 0);
	Description = Create_NEW(new UIItemExaminationDescriptionClass());
	left->Insert_Element(Description, 1);
	Specials = Create_NEW(new UIItemExaminationSpecialsClass(this));
	right->Insert_Element(Specials, 0);
	Options = Create_NEW(new UIItemExaminationOptionsClass(this));
	right->Insert_Element(Options, 1);
	Add_Element(left, 1);
	Add_Element(right, 1);
	GadgetClass *mover = Unknown_51A9C0(this);	// NAME 1 [mover]: neutral
	Unknown84_08(mover, 2, -2);
	mover->Unknown_65FFC0(-1, main_gadget);
}

UIItemExaminationClass::~UIItemExaminationClass()
{
	UIItemExaminationClass::Unknown_5C();
	Remove_All(true);
	Unknown_58ADA0();
	Unknown_BE67BC->Unload("FA_IEI00.INI");
}

UISlotInfoItemClass *UIItemExaminationClass::Unknown_90()
{
	return(Item.Peek());
}

void UIItemExaminationClass::Unknown_54()
{
	Keys.Add(ControlKeyComboStruct(0x1B, 0), (void *)1);
	InterfaceClass::Unknown_54();
}

void UIItemExaminationClass::Unknown_58()
{
	if (State == UI_CLOSED) {
		State = 2;
		UISlotInfoClass::Unknown_58();
	}
}



















void UIItemExaminationClass::Unknown_5C()
{
	switch (State) {
		case 0:
		case 3:
			break;
		case 1:
		case 2:
			State = UI_CLOSED;
			UISlotInfoClass::Unknown_5C();
			Title->Clear();
			Attributes->Clear();
			Specials->Clear();
			Options->Clear();
			Description->Clear();
			break;
		default:
			G_ASSERT(0);
	}
}

void UIItemExaminationClass::Unknown_60()
{
	UISlotInfoClass::Unknown_60();
}

bool UIItemExaminationClass::Unknown_68(int key)
{
	switch (key) {
		case 1:
			Unknown_5C();
			return(true);
	}
	return(UISlotInfoClass::Unknown_68(key));
}

void UIItemExaminationClass::Unknown_7C(UIViewClass *view, bool arg)
{
	InterfaceClass::Unknown_7C(view, arg);
}
