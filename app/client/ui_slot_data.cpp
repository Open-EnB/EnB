// Earth & Beyond client (reconstruction): app\client\ui_slot_data.cpp, the slot windows' base (the client's file per its
// asserts, 'e:\build\beta\G\app\Client\ui_slot_data.cpp' at 0xB8A4F4). Notes at the end of the file.
#include <stdio.h>
#define __INTRFACE_MEMBERS
#include "ui_slot_data.h"
#include "sclient.h"
#include "rendobj.h"
#include "assetmgr.h"
#include "slot_data.h"
#include "slot_data_render.h"
#include "gd_slot_data.h"
#include "gd_uiback.h"
#include "ui_scut_gadgets.h"
#include "ui_scut.h"
#include "local_interface_sort.h"
#include "..\shared\g_assert.h"

// What InterfaceClass's +0x40 points to, as far as this file calls it (as ui_inv.cpp declares it)
class LineDisplayClientClass  // NAME 1: neutral (D10)
{
public:
	virtual void Unknown_00(); virtual void Unknown_04(); virtual void Unknown_08(); virtual void Unknown_0C();
	virtual void Unknown_10(); virtual void Unknown_14();
	virtual SClient *As_Space_Client();					// +0x18
};

// drag_slot_data.cpp's drag of a slot (+0xA0), as this file calls it (as ui_inv.cpp declares it)
class DragSlotDataClass	// NAME 3: class-from-file (D24) for drag_slot_data.cpp (0x6851B0, 0x685500, 0x685610)
{
public:
	DragSlotDataClass(SClient *client, SlotDataRenderClass *render);	// 0x6851B0 (0x24 bytes)  // NAME 1 [client, render]: what 0x62DF90 passes
	virtual ~DragSlotDataClass();						// +0x00
	virtual void Unknown_04();
	virtual bool Unknown_08();							// +0x08 (a drop onto a gadget)
	virtual void Unknown_0C();							// +0x0C (the drag dropped)
};

// ui_slot_info.cpp's screen (+0x9C), as far as this file calls it
class UISlotInfoClass	// NAME 3: class-from-file (D24) for app\Client\ui_slot_info.cpp
{
public:
	virtual void Unknown_00(); virtual void Unknown_04(); virtual void Unknown_08(); virtual void Unknown_0C();
	virtual void Unknown_10(); virtual void Unknown_14(); virtual void Unknown_18(); virtual void Unknown_1C();
	virtual void Unknown_20(); virtual void Unknown_24(); virtual void Unknown_28(); virtual void Unknown_2C();
	virtual void Unknown_30(); virtual void Unknown_34(); virtual void Unknown_38(); virtual void Unknown_3C();
	virtual void Unknown_40(); virtual void Unknown_44(); virtual void Unknown_48(); virtual void Unknown_4C();
	virtual void Unknown_50(); virtual void Unknown_54(); virtual void Unknown_58(); virtual void Unknown_5C();
	virtual void Unknown_60(); virtual void Unknown_64(); virtual void Unknown_68(); virtual void Unknown_6C();
	virtual void Unknown_70(); virtual void Unknown_74(); virtual void Unknown_78();
	virtual void Unknown_7C(UIViewClass *view, bool arg);	// +0x7C (InterfaceClass's)  // NAME 1 [view, arg]: InterfaceClass's
	char Unknown04[0x38 - 0x04];
	RefCountClass Ref;									// +0x38 (InterfaceClass's RefCountClass)  // NAME 1: neutral (D10)
	void Unknown_630070(UIViewClass *view, int slot, bool a, bool b);	// 0x630070 (the slot's information shown)  // NAME 1 [view, slot, a, b]: what 0x62E8B0 passes
};
UISlotInfoClass *Unknown_630280(SClient *client, SlotDataClass *slot_data);	// 0x630280 (ui_slot_info.cpp: an item's examination or a skill's information)  // NAME 1 [client, slot_data]: what 0x62DF90 passes

// ui_slot_scut.cpp's shortcut gadgets of a slot interface (0x60 bytes), as this file calls it (ui_slot_scut.h declares it whole)
class SlotDataShortcutInterface : public UIScutGadgetsClass	// NAME 5: the client's RTTI
{
public:
	SlotDataShortcutInterface(SlotDataInterface *slot_iface, const std::string &name, SClient *sclient);	// 0x630490  // NAME 3 [slot_iface]: the FAIL_IF's; NAME 2 [name, sclient]: the base's
	virtual RObjGadgetClass *Unknown_0C(int slot);		// +0x0C 0x631000  // NAME 2 [slot]: as ui_slot_scut.h's
	virtual bool Unknown_10(RObjGadgetClass *gadget, std::string &key);	// +0x10 0x631540  // NAME 2 [gadget, key]: as ui_slot_scut.h's
	virtual RObjGadgetClass *Unknown_14(const char *key);	// +0x14 0x631270  // NAME 3 [key]: as ui_slot_scut.h's
	virtual int Unknown_18();							// +0x18 0x6315C0
};

// slot_data_render.cpp's factory, as it declares itself
class Unknown6A90A0Interface;
SlotDataRenderClass *Unknown_6A90A0(int kind, Unknown6A90A0Interface *iface, RenderObjClass *robj);	// 0x6A90A0  // NAME 1 [kind, iface, robj]: as slot_data_render.cpp's
void Unknown_496AE0(RenderObjClass *robj, bool make_geometry_unique);	// 0x496AE0 (as avatar.cpp declares it)  // NAME 1: neutral (D10)

// What a button's render's slot +0x18 answers, as 0x62F620 writes it
struct SlotDataRenderUnknown18Struct	// NAME 1: neutral (D10)
{
	char Unknown00[0x44];
	std::string Unknown44;								// +0x44  // NAME 1: neutral (D10)
};

// 0x62DD20
// NOT EXACT: functional (D64), from the pseudocode
SlotDataInterface::SlotDataInterface(UIViewClass *view, int unknown) :
	InterfaceClass(view, unknown),
	ShortcutUI(NULL),
	SlotData(NULL),
	Unknown9C(NULL),
	SlotDrag(NULL),
	SelectedHilight(NULL),
	UnknownA8(0),
	VisibleButtons(0),
	UnknownB0(-1),
	UnknownB4(-1)
{
	UnknownB8_0 = false;
	UnknownB8_1 = false;
	UnknownB8_2 = false;
	UnknownB8_3 = true;
}

// 0x62DE30
// NOT EXACT: functional (D64), from the pseudocode
bool SlotDataInterface::Unknown_9C(int slot, bool arg)
{
	return(false);
}

// 0x62DE80
// NOT EXACT: functional (D64), from the pseudocode
SlotDataInterface::SlotDataInterface(SClient *client) :
	InterfaceClass((LineDisplayClientClass *)client),
	ShortcutUI(NULL),
	SlotData(NULL),
	Unknown9C(NULL),
	SlotDrag(NULL),
	SelectedHilight(NULL),
	UnknownA8(0),
	VisibleButtons(0),
	UnknownB0(-1),
	UnknownB4(-1)
{
	UnknownB8_0 = false;
	UnknownB8_1 = false;
	UnknownB8_2 = false;
	UnknownB8_3 = true;
}


















// 0x62DF90
// NOT EXACT: functional (D64), from the pseudocode
void SlotDataInterface::Unknown_54()
{
	InterfaceClass::Unknown_54();
	G_ASSERT(!SlotData);
	SlotData = Unknown_88();
	SlotDataRenderClass *render = Unknown_6A90A0(2, (Unknown6A90A0Interface *)this, NULL);
	SlotDrag = new DragSlotDataClass(Client->As_Space_Client(), render);
	if (render) {
		render->Release_Ref();
	}
	SelectedHilight = WW3DAssetManager::Get_Instance()->Create_Render_Obj("select01");



	G_ASSERT(SelectedHilight);
	Unknown9C = Unknown_630280(Client->As_Space_Client(), SlotData);
	Unknown9C->Unknown_54();
	SlotDataClass::Unknown_6A7AB0().connect(SlotData, this, &SlotDataInterface::Slot_Data_Changed);
}



// 0x62E150
// NOT EXACT: functional (D64), from the pseudocode
void SlotDataInterface::Unknown_62E150()
{
	FAIL_IF(ShortcutUI) return;
	G_ASSERT(!Is_Active());
	ShortcutUI = new SlotDataShortcutInterface(this, std::string(Unknown_A8()), Client->As_Space_Client());
}

// 0x62E320
// NOT EXACT: functional (D64), from the pseudocode
SlotDataInterface::~SlotDataInterface()
{
	SlotDataInterface::Unknown_7C(NULL, false);
	if (ShortcutUI) {
		delete ShortcutUI;
		ShortcutUI = NULL;
	}
	if (Unknown9C) {
		Unknown9C->Ref.Release_Ref();
	}
	Unknown9C = NULL;
	if (SlotDrag) {
		delete SlotDrag;
	}
	SlotDrag = NULL;
	int count = Buttons.size();
	for (int i = 0; i < count; i++) {
		Unknown64.Unknown_687150((GadgetClass *)Buttons[i]);
		GadgetClass::Destroy((GadgetClass *)Buttons[i]);
		Buttons[i] = NULL;
	}
	Buttons.erase(Buttons.begin(), Buttons.end());
	SlotDataClass::Unknown_6A7AB0().disconnect(SlotData, this, &SlotDataInterface::Slot_Data_Changed, SlotDataEventDataChange(0));
	REF_PTR_RELEASE(SelectedHilight);
	REF_PTR_RELEASE(SlotData);
}

// 0x62E580
void SlotDataInterface::Unknown_CC()
{
	UnknownA8 = 0;
	Unknown_B0();
}

// 0x62E5B0
void SlotDataInterface::Unknown_D0()
{
	Unknown_B8(false);
}














// 0x62E5D0
// NOT EXACT: functional (D64), from the pseudocode
void SlotDataInterface::Slot_Data_Changed(SlotDataEventDataChange *event, SlotDataClass *slot_data)
{
	FAIL_IF(!event) return;
	FAIL_IF(SlotData != slot_data) return;
	if (event->Value == 1) {
		Unknown_D0();
	} else if (event->Value == 2) {
		Unknown_CC();
	}
}

// 0x62E660
// NOT EXACT: functional (D64), from the pseudocode
void SlotDataInterface::Unknown_7C(UIViewClass *view, bool arg)
{
	if (View == view) {
		InterfaceClass::Unknown_7C(view, arg);
		return;
	}
	MainViewClass *main_view = Client->As_Space_Client()->MainView;
	if (ShortcutUI) {
		ShortcutUI->Clear_UI();
	}
	InterfaceClass::Unknown_7C(view, arg);
	if (Unknown9C) {
		Unknown9C->Unknown_7C(NULL, false);
	}
	if (ShortcutUI) {
		if (!view) return;
		if (view == main_view) {
			ShortcutUI->Set_UI(Create_Peek((UIScutClass *)main_view->Unknown_514400()));
		}
	}
	if (view) {
		SlotData->Unknown_6A7860();
	}
}

// 0x62E7C0
void SlotDataInterface::Unknown_58()
{
	InterfaceClass::Unknown_58();
	if (!Unknown_94()) {
		SlotData->Unknown_6A7860();
	}
}

// 0x62E800
void SlotDataInterface::Unknown_5C()
{
	Unknown9C->Unknown_5C();
	SlotDrag->Unknown_0C();
	UnknownB4 = -1;
	Unknown_C8();
	InterfaceClass::Unknown_5C();
}

// 0x62E850
bool SlotDataInterface::Unknown_68(int key)
{
	return(true);
}

// 0x62E870
void SlotDataInterface::Unknown_60()
{
	SlotData->Unknown_6A7730();
}

// 0x62E890
void SlotDataInterface::Unknown_64()
{
	SlotData->Unknown_6A7730();
}

// 0x62E8B0
// NOT EXACT: functional (D64), from the pseudocode
bool SlotDataInterface::Unknown_98(SlotDataGadgetClass *gadget, KernelEventClass *event, unsigned int mask)
{
	if ((mask & 0xF0) && (((unsigned short *)event)[1] & 0x1000)) {
		if (Unknown_A0(gadget)) {
			Unknown9C->Unknown_630070(View, gadget->Unknown_670910(), true, false);
			Client->As_Space_Client()->Unknown_744560(0xE);
			return(true);
		}
	}
	return(false);
}






























































































































// 0x62E940
// NOT EXACT: functional (D64), from the pseudocode
bool SlotDataInterface::Unknown_62E940(unsigned int notches, unsigned int notch_size)
{
	FAIL_IF(notch_size == 0) return(false);
	FAIL_IF((VisibleButtons % notch_size) != 0) return(false);
	if (notches * notch_size - UnknownA8 != 0) {
		UnknownA8 = -1;
		return(Unknown_62EAA0(notches, notch_size));
	}
	return(false);
}
















// 0x62E9E0
bool SlotDataInterface::Unknown_62E9E0(int notches, unsigned int notch_size)
{
	FAIL_IF(notch_size == 0) return(false);
	FAIL_IF((VisibleButtons % notch_size) != 0) return(false);
	int old = UnknownA8;
	UnknownA8 -= notches * notch_size;
	if (UnknownA8 < 0) {
		UnknownA8 = 0;
	}
	if (UnknownA8 != old) {
		Unknown_B0();
		return(true);
	}
	return(false);
}















// 0x62EAA0
// NOT EXACT: functional (D64), from the pseudocode
bool SlotDataInterface::Unknown_62EAA0(unsigned int notches, unsigned int notch_size)
{
	FAIL_IF(notch_size == 0) return(false);
	FAIL_IF((VisibleButtons % notch_size) != 0) return(false);
	unsigned int visible = VisibleButtons / notch_size;
	int top = UnknownA8;
	unsigned int total = (SlotData->Unknown_6A7990() - 1 + (notch_size - top)) / notch_size;
	if (visible < total) {
		total -= visible;
		if (notches > total) {
			notches = total;
		}
		if (top == -1) {
			top = notches * notch_size;
		} else {
			if (notches == 0) {
				return(false);
			}
			top += notches * notch_size;
		}
		UnknownA8 = top;
		Unknown_B0();
		return(true);
	}
	return(false);
}


























// 0x62EB90
void SlotDataInterface::Unknown_B4(int slot, int index)
{
	SlotDataGadgetClass *gadget = Buttons[index];
	FAIL_IF(!gadget) return;
	if (index < VisibleButtons && slot != -1) {
		gadget->Unknown_40(true);
		gadget->Unknown_8C(false);
		gadget->Unknown_AC(slot);
	} else {
		gadget->Unknown_40(false);
		if (UnknownB8_3) {
			gadget->Unknown_8C(true);
		}
		gadget->Unknown_AC(-1);
	}
	std::string name;
	Unknown_A4(name, slot, index);
	GadgetClass::Set_Tooltip_Name(gadget, name.c_str());
}

// 0x62ED40
// NOT EXACT: functional (D64), from the pseudocode
void SlotDataInterface::Unknown_B0()
{
	int count = Buttons.size();
	for (int i = 0; i < count; i++) {
		Unknown_B4(SlotData->Unknown_6A7A00(UnknownA8 + i), i);
	}
	Unknown_C8();
}

// 0x62EDC0
void SlotDataInterface::Unknown_62EDC0(int count)
{
	int size = Buttons.size();
	if (count > size) {
		count = size;
	}
	VisibleButtons = count;
	Unknown_B0();
}














































// 0x62EE10
void SlotDataInterface::Unknown_B8(bool arg)
{
	int count = Buttons.size();
	for (int i = 0; i < count; i++) {
		SlotDataGadgetClass *gadget = Buttons[i];
		FAIL_IF(!gadget) continue;
		gadget->Unknown_A8(arg);
	}
}

// 0x62EEA0
void SlotDataInterface::Unknown_BC()
{
	int count = Buttons.size();
	for (int i = 0; i < count; i++) {
		Unknown64.Unknown_687150((GadgetClass *)Buttons[i]);
		GadgetClass::Destroy((GadgetClass *)Buttons[i]);
		Buttons[i] = NULL;
	}
	Buttons.erase(Buttons.begin(), Buttons.end());
}



























// 0x62EF80
SlotDataGadgetClass *SlotDataInterface::Unknown_AC(RenderObjClass *robj, const int *slot)
{
	FAIL_IF(!robj) return(NULL);
	SlotDataRenderClass *slot_render = Unknown_6A90A0(0, (Unknown6A90A0Interface *)this, robj);

	FAIL_IF(!slot_render) return(NULL);
	int index = Buttons.size();
	SlotDataGadgetClass *bttn = new SlotDataGadgetClass((SlotDataIFaceClass *)this, slot_render);
	slot_render->Release_Ref();



	FAIL_IF(!bttn) return(NULL);
	Buttons.push_back(bttn);
	bttn->Unknown_65FFA0(UnknownB0);
	Unknown64.Unknown_6870E0(bttn, index + 1, View, true, true);
	VisibleButtons = Buttons.size();
	UnknownA8 = 0;
	if (slot) {
		Unknown_B4(*slot, index);
	} else {
		Unknown_B4(SlotData->Unknown_6A7A00(index), index);
	}
	return(bttn);
}






























// 0x62F190
bool SlotDataInterface::Unknown_C4(RenderObjClass *robj, const char *format, int count)
{
	for (int i = 0; i < count; i++) {
		char name[32];
		sprintf(name, format, i);
		RenderObjClass *bttn = robj->Get_Sub_Object_By_Name(name);
		FAIL_IF(!bttn) return(false);
		Unknown_496AE0(bttn, false);
		Unknown_AC(bttn, NULL);
		bttn->Release_Ref();
	}
	return(true);
}

// 0x62F260
bool SlotDataInterface::Unknown_C0(RefCountPtr<RenderObjClass> &robj, const char *format, int count)
{
	return(Unknown_C4(robj.Peek(), format, count));
}



// 0x62F290
SlotDataGadgetClass *SlotDataInterface::Unknown_62F290(unsigned int idx)
{
	FAIL_IF(idx >= Buttons.size()) return(NULL);
	return(Buttons[idx]);
}

// 0x62F300
void SlotDataInterface::Unknown_C8()
{
	if (SelectedHilight->Peek_Scene()) {
		SelectedHilight->Remove();
	}
	if (UnknownB4 != -1) {
		int index = SlotData->Unknown_6A79C0(UnknownB4) - UnknownA8;
		if (index >= 0 && index < VisibleButtons) {
			InterfaceSortScopeClass sort(this);
			Matrix3D tm = Buttons[index]->Unknown_66C6C0();
			SelectedHilight->Add((SceneClass *)Unknown44);
			SelectedHilight->Set_Sort_Level(Buttons[index]->RObj->Get_Sort_Level() - 3);
			tm[2][3] = -1.0f;
			SelectedHilight->Set_Transform(tm);
			SelectedHilight->Set_Hidden(false);
		}
	}
}
















// 0x62F4D0
void SlotDataInterface::Unknown_62F4D0(RefCountPtr<RenderObjClass> &frame, int id)
{
	FAIL_IF(!frame) return;
	Unknown64.Unknown_6870E0(new UIBackGadget(frame), id, View, true, false);
}

// 0x62F5B0
void SlotDataInterface::Unknown_62F5B0(bool on)
{
	UnknownB8_2 = on;
	for (std::vector<SlotDataGadgetClass *>::iterator it = Buttons.begin(); it != Buttons.end(); it++) {
		if (*it) {
			(*it)->Unknown_670C90(on);
		}
	}
}

// 0x62F620
void SlotDataInterface::Unknown_62F620(const char *name)
{
	if (!name) return;
	int count = Buttons.size();
	for (int i = 0; i < count; i++) {
		SlotDataGadgetClass *gadget = Buttons[i];
		if (gadget && gadget->SlotRender) {
			SlotDataRenderUnknown18Struct *data = (SlotDataRenderUnknown18Struct *)gadget->SlotRender->Unknown_18();
			if (data) {
				data->Unknown44 = name;
			}
		}
	}
}


/*
**	Notes (SK1, 2026-11-09).
**	The object runs from its PermStringInitClass static (0x62DCD0, 0xBE5E54) to the adjustor thunks 0x62FD80 / 0x62FD90;
**	the vector helpers 0x62F7A0-0x62FB00 and the callback's members 0x62FB40-0x62FD60 come with it. The class name is
**	the client's RTTI (MemberCallback<SlotDataClass, SlotDataInterface, ...>), as ui_slot_scut.h's declaration has it.
**	Written from the pseudocode, the asserts on the client's lines; the functions so marked (D64) are
**	functional only. 0x62DD20 (a second constructor, over InterfaceClass's (view, unknown)) has no caller found.
*/
