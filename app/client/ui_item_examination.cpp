#include "..\shared\unknown_debug.h"
#include "ui_item_examination.h"
#include "ui_item_examination_attributes.h"
#include "ui_item_examination_options.h"
#include "gd_dynamicwindow.h"
#include "gadget.h"
#include "fontdatabase.h"
#include "control_keys_manager.h"

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
class SClient;	// The callees of Unknown_94 from files the tree does not have yet (interim here, D137)
class LineDisplayClientClass  // NAME 1: neutral (D10), as ui_cinfo.cpp declares it
{
public:
	virtual void Unknown_00(); virtual void Unknown_04(); virtual void Unknown_08(); virtual void Unknown_0C();
	virtual void Unknown_10(); virtual void Unknown_14();
	virtual SClient *As_Space_Client();							// +0x18 (as ui_manu.cpp's)  // NAME 3: the FAIL_IF's in ui_manu.cpp
};
class UISlotInfoItemUnknown54Class { public: int *Unknown_68AAA0(int slot, bool arg); };	// the item's +0x54 (inv_d*.cpp's 0x68AAA0)  // NAME 1 [slot, arg]: neutral
class Unknown537830Class  // NAME 1: neutral (D10): 0x1C bytes, tables 0xAEE6CC / 0xAEE6A0, the panels it fills
{
public:
	Unknown537830Class(UIItemExaminationTitleClass &title, UIItemExaminationAttributesClass &attributes, UIItemExaminationSpecialsClass &specials, UIItemExaminationOptionsClass &options, UIItemExaminationDescriptionClass &description);	// 0x537830  // NAME 1 [title, attributes, specials, options, description]
	~Unknown537830Class();																	// 0x5378C0
	char Unknown00[0x1C];
};
class Unknown521B80Class { public: Unknown521B80Class(SClient *space_client, int *entry, const RefCountPtr<UISlotInfoItemClass> &item, int slot); ~Unknown521B80Class(); void Unknown_521BD0(Unknown537830Class &visitor); int Unknown00[4]; };	// NAME 1: neutral (D10): 0x521B80 / 0x521BB0 / 0x521BD0  // NAME 1 [space_client, entry, item, slot, visitor]

bool UIItemExaminationClass::Unknown_94()
{
	FAIL_IF(State != UI_CLOSED) return(false);
	UISlotInfoItemUnknown54Class *holder = Item->Unknown_688830();	// NAME 1 [holder, slot, entry]: neutral
	int slot = UnknownAC;
	int *entry = holder->Unknown_68AAA0(slot, false);
	if (entry == NULL || *entry == -1 || *entry == -2) {
		return(false);
	}







	LineDisplayClientClass *client = Client;
	FAIL_IF(!client) return(false);
	SClient *space_client = client->As_Space_Client();
	FAIL_IF(!space_client) return(false);
	Unknown521B80Class info(space_client, entry, Item, slot);	// NAME 1 [info]: neutral
	Unknown537830Class visitor(*Title, *Attributes, *Specials, *Options, *Description);	// NAME 1 [visitor]: neutral
	info.Unknown_521BD0(visitor);
	Attributes->Rebuild();
	Specials->Rebuild();
	Options->Rebuild();
	return(true);
}

/*
**	Reconstruction notes (from the retail client.exe, SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Client\ui_item_examination.cpp (its __FILE__ 0xB786DC: FAIL_IF '!main_gadget'
**	line 82, G_ASSERT 'CloseButton' line 90, G_ASSERT '0' line 178, FAIL_IFs 'State != UI_CLOSED' / '!client' /
**	'!space_client' lines 221 / 236 / 238).
**
**	IE2 (run 2026-11-05-night): the object 0x5315E0-0x532B9F, all exact: the close button's size 0x531630, the
**	constructor 0x531660, the destructor 0x5321C0 (deleting 0x532190; the slot +0x5C inlined), the slots 0x532460-0x532640,
**	the Create_NEW copies 0x532930-0x532AB0 and the adjustor thunks 0x532B70 / 0x532B80 / 0x532B90. The bases
**	(ui_slot_info.cpp's, ui_dynamicwindow.cpp's) and the title, description and specials panels are declared only.
**	Unknown_94's lookup needs the call before the slot read (one expression loaded UnknownAC first).
*/
