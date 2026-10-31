// Earth & Beyond client (reconstruction): app\Client\ui_scut.cpp. Notes at the end of the file.
#include <stdio.h>
#include <string.h>
#include <algorithm>
#include <vector>
#include "ui_scut.h"
#include "client.h"
#include "..\audio\soundlib.h"
#include "rendobj.h"
#include "..\shared\unknown_debug.h"
#include "control_keys_manager.h"

// fontdatabase.h's FontStringFilesClass at 0xBE67BC and fontmanager.h's FontManagerClass at 0xBE67C4, as far as this
// file calls them (fontdatabase.h's 0xBE67B8 clashes with tsk_game.h's, so neither header can be included here)
class FontStringFilesClass { public: void Load(const char *name); void Unload(const char *name); };	// NAME 2: fontdatabase.h's
class FontManagerClass { public: void Add(RenderObjClass *object); };	// NAME 3: fontmanager.h's
extern FontManagerClass *Unknown_BE67C4;	// NAME 1: neutral (D10)

// local_interface_sort.cpp's sort object (8 bytes; view.cpp, mainview.cpp and ui_line_display.cpp declare it too)
class LineDisplaySortClass  // NAME 1: neutral (D10)
{
public:
	LineDisplaySortClass(InterfaceClass *inter);		// 0x541980  // NAME 1 [inter]: view.cpp's
	~LineDisplaySortClass();							// 0x6E18B0

private:
	char Unknown00[8];
};

// gadget.cpp's gadget, as far as the shortcut bars call it
class GadgetClass	// NAME 3: class-from-file (D24) for app\client\gadget.cpp
{
public:
	virtual void Unknown_00(); virtual void Unknown_04();
	virtual GadgetClass *Unknown_08();					// +0x08 (0x610A40: what the mouse holds)
	virtual void Unknown_0C(); virtual void Unknown_10();
	virtual GadgetClass *Unknown_14();					// +0x14 (0x610A40: the proxy button of what the mouse holds)
	virtual void Unknown_18(); virtual void Unknown_1C(); virtual void Unknown_20(); virtual void Unknown_24(); virtual void Unknown_28();
	virtual void Unknown_2C(KernelEventClass *event, float x, float y, unsigned short flags);	// +0x2C (0x6120E0: a zero event, the proxy's position, its flags)  // NAME 1 [event, x, y, flags]: neutral
	virtual void Unknown_30(); virtual void Unknown_34(); virtual void Unknown_38(); virtual void Unknown_3C();
	virtual void Unknown_40(bool on);					// +0x40 (0x611BD0: a new shortcut on; the bank's 0x6127E0)  // NAME 1 [on]: neutral
	virtual void Unknown_44(); virtual void Unknown_48(); virtual void Unknown_4C();
	virtual void Unknown_50(); virtual void Unknown_54(); virtual void Unknown_58(); virtual void Unknown_5C();
	virtual void Unknown_60(); virtual void Unknown_64(); virtual void Unknown_68(); virtual void Unknown_6C(); virtual void Unknown_70();
	virtual void Unknown_74(); virtual void Unknown_78();
	virtual void Unknown_7C(UIScutBarClass *bar);		// +0x7C (0x611BD0: onto the bar)  // NAME 1 [bar]: neutral
	virtual void Unknown_80(UIScutBarClass *bar);		// +0x80 (0x611BD0: off the bar)  // NAME 1 [bar]: neutral
	virtual void Unknown_84(); virtual void Unknown_88(); virtual void Unknown_8C(); virtual void Unknown_90();
	virtual void Unknown_94(RenderObjClass *object);	// +0x94 0x66C000 (the bank's constructor: its 'bank_%d' model)  // NAME 1 [object]: ui_group.cpp's
	virtual void Unknown_98(); virtual void Unknown_9C(); virtual void Unknown_A0();
	virtual void Unknown_A4(GadgetClass *gadget);		// +0xA4 (the proxy buttons: the shortcut shown)  // NAME 1 [gadget]: neutral

	char Unknown04[0x10 - 0x04];						// +0x04
	unsigned int Unknown10;								// +0x10 (0x6120E0: bits 0x01 / 0x04)
	char Unknown14[0x1C - 0x14];						// +0x14
	int Unknown1C;										// +0x1C: the control id (0x610F80 sets the bank's)
	char Unknown20[0x2C - 0x20];						// +0x20
	unsigned int Unknown2C;								// +0x2C (0x612820: bit 0x20)
	char Unknown30[0x64 - 0x30];						// +0x30
};
void Unknown_65EFB0(GadgetClass *gadget, const char *section, const char *name);	// 0x65EFB0 (cdecl, gadget.cpp): the button's text from a string section  // NAME 1 [section, name]: ui_group.cpp's
void Unknown_65F340(GadgetClass *gadget, const char *section);	// 0x65F340 (cdecl, gadget.cpp): the help section  // NAME 1 [gadget, section]: neutral
GadgetClass *Unknown_65F5F0(int source, int a, int b);	// 0x65F5F0 (cdecl, gadget.cpp; SClient +0x1278, 1, 1)  // NAME 1 [source, a, b]: neutral
bool Unknown_42EC60(int key, int arg);					// 0x42EC60 (cdecl: the input object *0xBDAA70's slot +0x0C; 0x610980 passes 0x12, 0)  // NAME 1 [key, arg]: neutral

// gadget.cpp's button gadget (100 bytes, constructor 0x675240, destructor 0x675350), the bank's base
class UIScutButtonGadgetClass : public GadgetClass	// NAME 1: neutral (D10)
{
public:
	UIScutButtonGadgetClass(RenderObjClass *object, LayerClass *layer, int unknown1, int unknown2);	// 0x675240 (the bank passes NULL, the layer, 0x55, 4)  // NAME 1 [object, layer, unknown1, unknown2]: ui_group.cpp's
	~UIScutButtonGadgetClass();							// 0x675350
	virtual void Unknown_40(bool on);					// +0x40 0x66B840  // NAME 1 [on]: neutral
};

// gd_proxy.cpp's proxy button (0x80 bytes, constructor 0x66C9C0): what a shortcut button shows
class UIScutProxyGadgetClass : public GadgetClass	// NAME 1: neutral (D10)
{
public:
	UIScutProxyGadgetClass(UIScutBarClass *owner, RenderObjClass *object, LayerClass *layer, int unknown, int index);	// 0x66C9C0 (the bar, its 'bttn_%d_%02d' model, the layer, 0x55, the button)  // NAME 1 [object, layer, unknown, index]: what 0x610F80 passes; NAME 3 [owner]: the assert's 'Owner'
	void Unknown_66B420(int arg);						// 0x66B420 (0x6120E0: 0 down, 1 up)  // NAME 1 [arg]: neutral
	void Unknown_66B4A0();								// 0x66B4A0 (0x6120E0: after the event)
	GadgetClass *Unknown_66C6C0();						// 0x66C6C0 (0x6120E0: on the shortcut, for its position)
	void Unknown_66CB50();								// 0x66CB50 (0x612430: every frame)

	GadgetClass *Unknown64;								// +0x64: the shortcut shown
	char Unknown68[0x6C - 0x68];						// +0x68
	bool Unknown6C;										// +0x6C: pressed (0x6120E0)
	char Unknown6D[0x78 - 0x6D];						// +0x6D
	UIScutBarClass *Owner;								// +0x78 (G_ASSERT 'ActiveShortcut->Owner == this' l.904)  // NAME 3: the assert's
	int Unknown7C;										// +0x7C: the button
};
// What the software mouse holds at +0x2C (0x612430: its +0x10 is 2 when a shortcut is let go)
class UIScutMouseUnknown2CClass { public: char Unknown00[0x10]; int Unknown10; };	// NAME 1: neutral (D10)
// The software mouse (SClient +0x1274; the view's slot +0x44), as far as the shortcut bars use it
class UIScutMouseClass { public: char Unknown00[0x28]; int Unknown28; UIScutMouseUnknown2CClass *Unknown2C; };	// NAME 1: neutral (D10): +0x28 a nesting count (0x6120E0)

// The bank toggle of a bar (0x70 bytes, table 0xAF7490; constructor 0x6125B0, destructor 0x612740): its group models
class UIScutBankGadgetClass : public UIScutButtonGadgetClass	// NAME 2: its 'bank_%d' model
{
public:
	UIScutBankGadgetClass(int bar, UIMeshClass *mesh, LayerClass *layer);	// 0x6125B0  // NAME 1 [bar, mesh, layer]: what 0x610F80 passes
	virtual ~UIScutBankGadgetClass();					// 0x612740 (slot 0x612710)
	virtual void Unknown_40(bool on);					// +0x40 0x6127E0  // NAME 1 [on]: the base's
	void Unknown_612820(int num);						// 0x612820 (FAIL_IFs l.1004 / 1005 / 1009): the group shown  // NAME 3 [num]: the FAIL_IFs'

	RenderObjClass *GroupRObj[NUM_GROUPS];				// +0x64 (FAIL_IF '!GroupRObj[lp]' l.978)  // NAME 3: the FAIL_IF's
	int Group;											// +0x6C  // NAME 2: what it holds
};


UIScutClass::UIScutClass(UIViewClass *view, LayerClass *layer) :
	Unknown5BB320Class(new UIMeshClass("tsi01a_short", 2), view, *layer),
	UnknownB4(false)
{
	for (int lp = 0; lp < NUM_SHORTCUT_BARS; lp++) {
		Bars[lp] = NULL;
	}
	((FontStringFilesClass *)Unknown_BE67BC)->Load("fc_scut.ini");
	Unknown_BE67C4->Add(Mesh->Peek_Object());
	int id = 2;
	for (lp = 0; lp < NUM_SHORTCUT_BARS; lp++) {
		Bars[lp] = new UIScutBarClass(this, Mesh, layer, lp, &id, lp);
	}
	for (lp = 0; lp < NUM_SHORTCUT_BARS; lp++) {
		Bars[lp]->Unknown_6112B0();
	}
}


void UIScutClass::Unknown_70(ControlKeysManagerClass *manager)
{
	for (int lp = 0; lp < NUM_SHORTCUT_BARS; lp++) {
		Bars[lp]->Unknown_611300(manager);
	}
}


void UIScutClass::Unknown_74(ControlKeysManagerClass *manager)
{
	for (int lp = 0; lp < NUM_SHORTCUT_BARS; lp++) {
		Bars[lp]->Unknown_611710(manager);
	}
}


UIScutClass::~UIScutClass()
{
	for (int lp = 0; lp < NUM_SHORTCUT_BARS; lp++) {
		delete Bars[lp];
	}
	((FontStringFilesClass *)Unknown_BE67BC)->Unload("fc_scut.ini");
}


void UIScutClass::Unknown_54()
{
	Unknown_10();
	for (int lp = 0; lp < NUM_SHORTCUT_BARS; lp++) {
		Bars[lp]->Unknown_611F60(0);
	}
}


void UIScutClass::Unknown_60()
{
	for (int lp = 0; lp < NUM_SHORTCUT_BARS; lp++) {
		Bars[lp]->Unknown_612430();
	}
	bool down = Unknown_42EC60(0x12, 0);
	if (down != UnknownB4) {
		UnknownB4 = down;
		if (down) {
			for (lp = 0; lp < NUM_SHORTCUT_BARS; lp++) {
				Bars[lp]->Unknown_611F60(Bars[lp]->Group + 1);
			}
		} else {
			for (lp = 0; lp < NUM_SHORTCUT_BARS; lp++) {
				Bars[lp]->Unknown_611F60(Bars[lp]->Group - 1);
			}
		}
	}
	Unknown5BB320Class::Unknown_60();
}






































































bool UIScutClass::Unknown_68(int key)
{
	switch (key) {
		default: {
			for (int lp = 0; lp < NUM_SHORTCUT_BARS; lp++) {
				if (Bars[lp]->Unknown_6120E0(key)) {
					return(true);
				}
			}
			return(false);
		}
		case 1: {
			UIScutProxyGadgetClass *proxy = Unknown_610F30();
			if (proxy) {
				proxy->Owner->Unknown_611BD0(NULL, proxy->Unknown7C, CURRENT_GROUP, true);
			}
			return(false);
		}
	}
}

// A free button of a bar, for the sort (0x10 bytes: 0x610B00 sorts one a bar)
struct ShortcutSlotStruct	// NAME 1: neutral (D10)
{
	ShortcutSlotStruct() : Found(false), Group(0), Bttn(0), Bar(0) {}
	bool Found;											// +0x00  // NAME 1: neutral
	int Group;											// +0x04  // NAME 1: neutral
	int Bttn;											// +0x08  // NAME 1: neutral
	int Bar;											// +0x0C  // NAME 3: the assert's 'a.Bar'
};

// The sort's order (0x613090, out of line in the sort): found first, then the lower group, then the lower bar
struct ShortcutSlotCompareStruct	// NAME 1: neutral (D10)
{
	int operator () (const ShortcutSlotStruct &a, const ShortcutSlotStruct &b) const
	{
		G_ASSERT(a.Bar != b.Bar);
		if (!a.Found && b.Found) return(false);
		if (a.Found && !b.Found) return(true);
		if (a.Group < b.Group) return(true);
		if (a.Group > b.Group) return(false);
		return(a.Bar < b.Bar);
	}
};


bool UIScutClass::Unknown_610B00(GadgetClass *gadget)
{
	std::vector<ShortcutSlotStruct> slots(NUM_SHORTCUT_BARS, ShortcutSlotStruct());
	for (int lp = 0; lp < NUM_SHORTCUT_BARS; lp++) {
		slots[lp].Found = Bars[lp]->Unknown_611B10(&slots[lp].Group, &slots[lp].Bttn);
		slots[lp].Bar = lp;
	}
	std::sort(slots.begin(), slots.end(), ShortcutSlotCompareStruct());
	if (slots[0].Found) {
		return(Bars[slots[0].Bar]->Unknown_611BD0(gadget, slots[0].Bttn, slots[0].Group, true));
	}
	return(false);
}










bool UIScutClass::Unknown_610C70(GadgetClass *gadget, unsigned int bar)
{
	FAIL_IF(bar >= NUM_SHORTCUT_BARS) return(false);
	int group, bttn_idx;
	if (Bars[bar]->Unknown_611B10(&group, &bttn_idx)) {
		return(Bars[bar]->Unknown_611BD0(gadget, bttn_idx, group, true));
	}
	return(false);
}


bool UIScutClass::Unknown_610D00(GadgetClass *gadget, unsigned int bar, int group, int bttn_idx)
{
	FAIL_IF(bar >= NUM_SHORTCUT_BARS) return(false);
	return(Bars[bar]->Unknown_611BD0(gadget, bttn_idx, group, false));
}


bool UIScutClass::Unknown_610D70(GadgetClass *gadget)
{
	bool found = false;
	for (int lp = 0; lp < NUM_SHORTCUT_BARS; lp++) {
		if (Bars[lp]->Unknown_611E20(gadget)) found = true;
	}
	return(found);
}


bool UIScutClass::Unknown_610E10(GadgetClass *gadget, int *bar, int *group, int *bttn_idx)
{
	FAIL_IF(!gadget) return(false);
	FAIL_IF(!bttn_idx) return(false);
	FAIL_IF(!group) return(false);
	FAIL_IF(!bar) return(false);
	for (int lp = 0; lp < NUM_SHORTCUT_BARS; lp++) {
		if (Bars[lp]->Unknown_611E70(gadget, group, bttn_idx)) {
			*bar = lp;
			return(true);
		}
	}
	return(false);
}

extern GadgetClass *Unknown_6123F0(UIViewClass *view);	// 0x6123F0 (below)


UIScutProxyGadgetClass *UIScutClass::Unknown_610F30()
{
	GadgetClass *gadget = Unknown_6123F0(View);
	if (gadget) {
		return((UIScutProxyGadgetClass *)gadget->Unknown_14());
	}
	return(NULL);
}


// NOT EXACT: not yet matched (first draft from the pseudocode)
UIScutBarClass::UIScutBarClass(UIScutClass *owner, UIMeshClass *mesh, LayerClass *layer, int bar, int *id, int index) :
	UnknownAEEA88Class(owner),
	Mesh(mesh),
	Index(index),
	Bar(bar),
	Bank(NULL),
	Unknown44(NULL),
	ActiveShortcut(NULL),
	Group(0),
	FirstId(*id),
	SecondId(*id + 3)
{
	for (int lp = 0; lp < NUM_SHORTCUT_BUTTONS; lp++) {
		ProxyButtons[lp] = NULL;
	}
	memset(Shortcuts, 0, sizeof(Shortcuts));
	memset(Counts, 0, sizeof(Counts));
	memset(Shortcuts, 0, sizeof(Shortcuts));
	memset(Counts, 0, sizeof(Counts));
	char name[32];
	for (lp = 0; lp < NUM_SHORTCUT_BUTTONS; lp++) {
		sprintf(name, "bttn_%d_%02d", bar, lp);
		RenderObjClass *object = mesh->Unknown_5B8B40(name);
		ProxyButtons[lp] = new UIScutProxyGadgetClass(this, object, layer, 0x55, lp);
		Unknown_65EFB0(ProxyButtons[lp], "UI_SCUT", "SHORTCUTS");
		Get_Owner()->Unknown64.Unknown_6870E0(ProxyButtons[lp], FirstId + lp, Get_Owner()->View, 1, 0);
		object->Release_Ref();
	}
	*id = SecondId + 3;
	Bank = new UIScutBankGadgetClass(bar, mesh, layer);
	Get_Owner()->Unknown64.Unknown_6870E0(Bank, *id, Get_Owner()->View, 1, 0);
	Bank->Unknown1C = *id;
	(*id)++;
	((DynamicVectorClass<UnknownAEEA88Class *> *)((char *)Mesh + 4))->Add(this);
}


UIScutBarClass::~UIScutBarClass()
{
	UnknownAEEA88Class::Unknown_04((DynamicVectorClass<UnknownAEEA88Class *> *)((char *)Mesh + 4));
	REF_PTR_RELEASE(Unknown44);
}






















































void UIScutBarClass::Unknown_6112B0()
{
	for (int lp = 0; lp < NUM_SHORTCUT_BUTTONS; lp++) {
		FAIL_IF(!ProxyButtons[lp]) continue;
		Unknown_65F340(ProxyButtons[lp], "UI_SCUT_SHORTCUTS");
	}
}


// NOT EXACT: not yet matched (first draft from the pseudocode)
void UIScutBarClass::Unknown_611300(ControlKeysManagerClass *manager)
{
	char name[32];
	int slot = Bar * NUM_SHORTCUT_BUTTONS + 1;
	for (int lp = 0; lp < NUM_SHORTCUT_BUTTONS; lp++) {
		sprintf(name, "Use Slot %d", lp + slot);
		((ControlKeysManager *)manager)->Add_Keys((ControlKeyListClass *)Get_Owner()->Unknown4C, name, (void *)(FirstId + lp), 0);
		((ControlKeysManager *)manager)->Add_Keys((ControlKeyListClass *)Get_Owner()->Unknown4C, name, (void *)(SecondId + lp), 0x1000);
		((ControlKeysManager *)manager)->Add_Keys((ControlKeyListClass *)Get_Owner()->Unknown4C, std::string(name), (void *)(FirstId + lp), 0x400);
		((ControlKeysManager *)manager)->Add_Keys((ControlKeyListClass *)Get_Owner()->Unknown4C, std::string(name), (void *)(SecondId + lp), 0x1400);
	}
}


// NOT EXACT: not yet matched (first draft from the pseudocode)
void UIScutBarClass::Unknown_611710(ControlKeysManagerClass *manager)
{
	char name[32];
	int slot = Bar * NUM_SHORTCUT_BUTTONS + 1;
	for (int lp = 0; lp < NUM_SHORTCUT_BUTTONS; lp++) {
		sprintf(name, "Use Slot %d", lp + slot);
		((ControlKeysManager *)manager)->Remove_Keys((ControlKeyListClass *)Get_Owner()->Unknown4C, name, 1);
		((ControlKeysManager *)manager)->Remove_Keys((ControlKeyListClass *)Get_Owner()->Unknown4C, name, 0);
		((ControlKeysManager *)manager)->Remove_Keys((ControlKeyListClass *)Get_Owner()->Unknown4C, std::string(name), 0x1000);
		((ControlKeysManager *)manager)->Remove_Keys((ControlKeyListClass *)Get_Owner()->Unknown4C, std::string(name), 0x400);
		((ControlKeysManager *)manager)->Remove_Keys((ControlKeyListClass *)Get_Owner()->Unknown4C, std::string(name), 0x1400);
	}
}

#pragma auto_inline(off)


































bool UIScutBarClass::Unknown_611B10(int *group, int *bttn_idx)
{
	*bttn_idx = NUM_SHORTCUT_BUTTONS;
	*group = NUM_GROUPS;
	for (int lp = 0; lp < NUM_GROUPS; lp++) {
		int g = (Group + lp) % NUM_GROUPS;
		if (Counts[g] != NUM_SHORTCUT_BUTTONS) {
			for (int b = 0; b < NUM_SHORTCUT_BUTTONS; b++) {
				if (Shortcuts[g][b] == NULL) {
					*bttn_idx = b;
					*group = g;
					return(true);
				}
			}
			G_ASSERT(false);
		}
	}
	return(false);
}
#pragma auto_inline(on)
















bool UIScutBarClass::Unknown_611BD0(GadgetClass *gadget, int bttn_idx, int group, bool show)
{
	LineDisplaySortClass sort(Get_Owner());
	if (bttn_idx < 0 || bttn_idx > NUM_SHORTCUT_BUTTONS) {
		G_ASSERT(bttn_idx == FIRST_AVAIL);
		G_ASSERT(gadget);
		if (!Unknown_611B10(&group, &bttn_idx)) {
			return(false);
		}
	}
	if (group < 0 || group >= NUM_GROUPS) {






		G_ASSERT(group == CURRENT_GROUP);
		group = Group;
	}
	if (Shortcuts[group][bttn_idx]) {
		Shortcuts[group][bttn_idx]->Unknown_80(this);
		Counts[group]--;
	}
	Shortcuts[group][bttn_idx] = gadget;
	if (Shortcuts[group][bttn_idx]) {
		Shortcuts[group][bttn_idx]->Unknown_7C(this);
		Counts[group]++;
	}
	if (group == Group) {
		ProxyButtons[bttn_idx]->Unknown_A4(gadget);
		Unknown_65EFB0(ProxyButtons[bttn_idx], "UI_SCUT", "SHORTCUTS");
	}
	if (show) {
		Unknown_611F60(group);
	} else if (gadget) {
		Bank->Unknown_40(true);
	}
	return(true);
}


bool UIScutBarClass::Unknown_611E20(GadgetClass *gadget)
{
	bool found = false;
	for (int g = 0; g < NUM_GROUPS; g++) {
		for (int b = 0; b < NUM_SHORTCUT_BUTTONS; b++) {
			if (Shortcuts[g][b] == gadget) {
				Unknown_611BD0(NULL, b, g, false);
				found = true;
			}
		}
	}
	return(found);
}







































bool UIScutBarClass::Unknown_611E70(GadgetClass *gadget, int *group_ptr, int *bttn_idx_ptr)
{
	FAIL_IF(!gadget) return(false);
	FAIL_IF(!bttn_idx_ptr) return(false);
	FAIL_IF(!group_ptr) return(false);
	for (int g = 0; g < NUM_GROUPS; g++) {
		for (int b = 0; b < NUM_SHORTCUT_BUTTONS; b++) {
			if (Shortcuts[g][b] == gadget) {
				*group_ptr = g;
				*bttn_idx_ptr = b;
				return(true);
			}
		}
	}
	return(false);
}


void UIScutBarClass::Unknown_611F60(int group)
{
	LineDisplaySortClass sort(Get_Owner());
	int groups = 1;
	for (int lp = 0; lp < NUM_GROUPS; lp++) {
		if (Counts[lp] != 0) {
			groups = lp + 2;
		}
		if (groups >= NUM_GROUPS) {
			groups = NUM_GROUPS;
		}
	}
	if (group < 0) {
		while (group < 0) {
			group += groups;
		}
	} else {
		group %= groups;
	}
	if (Group != group) {
		Group = group;
		Bank->Unknown_612820(group);
		SoundLibraryClass::Play_2D_Sound(Group != 0 ? "bank_toggle_a_sn_1.wav" : "bank_toggle_b_sn_1.wav", 2);
		Mesh->Peek_Object()->Set_Sub_Object_Transforms_Dirty(true);
		for (lp = 0; lp < NUM_SHORTCUT_BUTTONS; lp++) {
			ProxyButtons[lp]->Unknown_A4(Shortcuts[Group][lp]);
			Unknown_65EFB0(ProxyButtons[lp], "UI_SCUT", "SHORTCUTS");
		}
	}
	Bank->Unknown_40(groups > 1);
}




























































// NOT EXACT: not yet matched (first draft from the pseudocode)
bool UIScutBarClass::Unknown_6120E0(int key)
{
	if (Bank->Unknown1C == key) {
		SoundLibraryClass::Play_2D_Sound("SR_generalclick04.wav", 2);
		Unknown_611F60(Group + 1);
		return(true);
	}
	if (Get_Owner()->View == NULL) return(true);
	if (Get_Owner()->View->Unknown_20() == NULL) return(true);
	Unknown65B810ViewClass *view = (Unknown65B810ViewClass *)Get_Owner()->View->Unknown_20();
	if (view->Client == NULL) return(true);
	SClient *sclient = (SClient *)((ClientClass *)view->Client)->Unknown_18();
	if (sclient == NULL) return(true);
	UIScutMouseClass *mouse = (UIScutMouseClass *)Get_Owner()->View->Unknown_44();
	G_ASSERT(mouse);
	if (key >= FirstId && key < FirstId + NUM_SHORTCUT_BUTTONS) {
		int index = key - FirstId;
		GadgetClass *shortcut = ProxyButtons[index]->Unknown64;
		if (shortcut && !ProxyButtons[index]->Unknown6C) {
			if (sclient->Unknown_750B60()) {
				sclient->Unknown_74D990(0x11, "Cannot perform action while incapacitated.", 0);
				return(true);
			}
			mouse->Unknown28++;
			KernelEventClass event;
			ProxyButtons[index]->Unknown_66B420(0);
			GadgetClass *pos = ((UIScutProxyGadgetClass *)shortcut)->Unknown_66C6C0();
			shortcut->Unknown_2C(&event, *(float *)((char *)pos + 0xC), *(float *)((char *)pos + 0x1C), (unsigned short)((shortcut->Unknown10 & 1) | 0x1000));
			sclient->Unknown_743380(&event);
			ProxyButtons[index]->Unknown6C = true;
			ProxyButtons[index]->Unknown_66B4A0();
			mouse->Unknown28--;
		}
		return(true);
	}
	if (key >= SecondId && key < SecondId + NUM_SHORTCUT_BUTTONS) {
		int index = key - SecondId;
		GadgetClass *shortcut = ProxyButtons[index]->Unknown64;
		if (ProxyButtons[index]->Unknown6C) {
			mouse->Unknown28++;
			KernelEventClass event(0, 0x1000);
			ProxyButtons[index]->Unknown_66B420(1);
			if (shortcut) {
				GadgetClass *pos = ((UIScutProxyGadgetClass *)shortcut)->Unknown_66C6C0();
				shortcut->Unknown_2C(&event, *(float *)((char *)pos + 0xC), *(float *)((char *)pos + 0x1C), (unsigned short)((shortcut->Unknown10 & 4) | 0x1000));
			}
			sclient->Unknown_743380(&event);
			ProxyButtons[index]->Unknown6C = false;
			ProxyButtons[index]->Unknown_66B4A0();
			mouse->Unknown28--;
			return(true);
		}
	}
	return(false);
}


GadgetClass *Unknown_6123F0(UIViewClass *view)
{
	SClient *client = ((Unknown65B810ViewClass *)view->Unknown_20())->Client;
	GadgetClass *gadget = Unknown_65F5F0(client->Unknown1278, 1, 1);
	if (gadget) {
		return(gadget->Unknown_08());
	}
	return(NULL);
}









































// NOT EXACT: not yet matched (first draft from the pseudocode)
void UIScutBarClass::Unknown_612430()
{
	for (int lp = 0; lp < NUM_SHORTCUT_BUTTONS; lp++) {
		ProxyButtons[lp]->Unknown_66CB50();
	}
	if (ActiveShortcut) {
		G_ASSERT(ActiveShortcut->Owner == this);
		UIScutMouseClass *mouse = (UIScutMouseClass *)Get_Owner()->View->Unknown_44();

		G_ASSERT(mouse);
		if (mouse->Unknown2C) {
			if (mouse->Unknown2C->Unknown10 != 2) return;
			Unknown_611BD0(NULL, ActiveShortcut->Unknown7C, CURRENT_GROUP, true);
		}
		ActiveShortcut = NULL;
	}
}


void UIScutBarClass::Unknown_00(void *source, int *event)
{
	if (source == (Mesh ? (char *)Mesh + 4 : NULL) && ((GadgetClass *)event[0])->Unknown1C == (int)this && event[1] == 3) {
		Mesh->Peek_Object()->Set_Sub_Object_Transforms_Dirty(true);
		for (int lp = 0; lp < NUM_SHORTCUT_BUTTONS; lp++) {
			ProxyButtons[lp]->Unknown_A4(Shortcuts[Group][lp]);
			Unknown_65EFB0(ProxyButtons[lp], "UI_SCUT", "SHORTCUTS");
		}
	}
}




































UIScutBankGadgetClass::UIScutBankGadgetClass(int bar, UIMeshClass *mesh, LayerClass *layer) :
	UIScutButtonGadgetClass(NULL, layer, 0x55, 4),
	Group(0)
{
	for (int lp = 0; lp < NUM_GROUPS; lp++) {
		GroupRObj[lp] = NULL;
	}
	char name[32];
	sprintf(name, "bank_%d", bar);
	RenderObjClass *object = mesh->Unknown_5B8B40(name);
	Unknown_94(object);
	object->Release_Ref();
	for (lp = 0; lp < NUM_GROUPS; lp++) {
		sprintf(name, "bank_%d_%d", bar, lp);
		GroupRObj[lp] = mesh->Unknown_5B8B40(name);
		FAIL_IF(!GroupRObj[lp]) continue;
		GroupRObj[lp]->Set_Hidden(true);
	}
	Unknown_612820(Group);
}


UIScutBankGadgetClass::~UIScutBankGadgetClass()
{
	for (int lp = 0; lp < NUM_GROUPS; lp++) {
		REF_PTR_RELEASE(GroupRObj[lp]);
	}
}


void UIScutBankGadgetClass::Unknown_40(bool on)
{
	GroupRObj[Group]->Set_Hidden(!on);
	UIScutButtonGadgetClass::Unknown_40(on);
}



// NOT EXACT: not yet matched (first draft from the pseudocode)
void UIScutBankGadgetClass::Unknown_612820(int num)
{
	FAIL_IF(num < 0) return;
	FAIL_IF(num >= NUM_GROUPS) return;
	if (Group != num) {


		FAIL_IF(!GroupRObj[num]) return;
		GroupRObj[Group]->Set_Hidden(true);
		Group = num;
		if (Unknown2C & 0x20) {
			GroupRObj[num]->Set_Hidden(false);
		}
	}
	char name[32];
	sprintf(name, "GROUP_%d", Group);
	Unknown_65EFB0(this, "UI_SCUT", name);
}

/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Client\ui_scut.cpp (the client's __FILE__ string).
**
**	The shortcut bars (ui_scut.h's UIScutClass, the cockpit's +0xDC; 0xB8 bytes over Unknown5BB320Class, the
**	tsi01a_short mesh and fc_scut.ini's texts): two bars (UIScutBarClass, 0x58 bytes), each with three proxy
**	buttons (gd_proxy.cpp's, 'bttn_%d_%02d') over two groups of three shortcuts, a bank toggle (UIScutBankGadgetClass,
**	'bank_%d', its group models 'bank_%d_%d') and the 'Use Slot %d' control keys (the first ids from 2, three a set,
**	two sets a bar, the bank's id after them). A bar listens to the mesh (the listener base UnknownAEEA88Class, the
**	mesh's list at its +0x04). Key 0x12 held shows the other group. The client's window runs 0x610570-0x6134F1
**	(claims #35 / #36): the interface, the bars, the bank, then the std::sort instances of ShortcutSlotStruct
**	(0x610B00 picks the bar with a free button: found first, then the lower group, then the lower bar).
**
**	Rebuilt (SC2, 2026-10-31), the asserts on the client's lines (D25), the functions without asserts between their
**	neighbours' lines in the client's order. Names are neutral where the client has none (Unknown_<address>).
*/
