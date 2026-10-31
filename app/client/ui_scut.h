// Earth & Beyond client (reconstruction): app\client\ui_scut.h, the shortcut bars (the client's ui_scut.cpp per its
// asserts; this header's name is provisional, NAME 1). Moved out of ui_cpit.h and laid out from the constructors
// 0x610570 / 0x610F80 / 0x6125B0 and the destructor 0x610800 (SC2, 2026-10-31). ui_cpit.h includes it.
#ifndef UI_SCUT_H  // NAME 1 [ui_scut.h]: provisional header name for ui_scut.cpp
#define UI_SCUT_H

#include "sclient.h"

class GadgetClass;			// gadget.cpp's
class RenderObjClass;
class UIScutClass;
class UIScutBarClass;
class UIScutBankGadgetClass;
class UIScutProxyGadgetClass;
class ControlKeysManagerClass;

enum {
	NUM_SHORTCUT_BARS = 2,	// (FAIL_IF 'bar >= NUM_SHORTCUT_BARS' l.325 / 336; the constructor makes two bars)  // NAME 3: the FAIL_IF's
	NUM_GROUPS = 2,			// (FAIL_IF 'num >= NUM_GROUPS' l.1005; two groups of buttons a bar)  // NAME 3: the FAIL_IF's
	NUM_SHORTCUT_BUTTONS = 3,	// three buttons a group (the 'bttn_%d_%02d' bones)  // NAME 1: after NUM_SHORTCUT_BARS
	FIRST_AVAIL = -1,		// (G_ASSERT 'bttn_idx == FIRST_AVAIL' l.590)  // NAME 3: the assert's
	CURRENT_GROUP = -1		// (G_ASSERT 'group == CURRENT_GROUP' l.603)  // NAME 3: the assert's
};

// A listener base (8 bytes, vtable 0xAEEA88: slot +0x00 pure, +0x04 0x53B310 first emitted in bridge_int.cpp; bridge_int.cpp's
// interface 0x53B040 and the 3-D movie view 0x7EEF90 derive from it too): +0x04 what the subclass is given
class UnknownAEEA88Class	// NAME 1: neutral (D10)
{
public:
	UnknownAEEA88Class(void *owner) : Unknown04(owner) {}	// NAME 1 [owner]: neutral
	virtual void Unknown_00(void *source, int *event) = 0;	// +0x00 (the shortcut bar's 0x612510)  // NAME 1 [source, event]: neutral
	virtual void Unknown_04(DynamicVectorClass<UnknownAEEA88Class *> *list) { list->Delete(this); }	// +0x04 0x53B310: out of the list  // NAME 1 [list]: neutral

	void *Unknown04;									// +0x04
};

// One shortcut bar (0x58 bytes, table 0xAF7484; constructor 0x610F80, destructor 0x611220): three proxy buttons over two
// groups of three shortcuts, and the bank toggle that shows the group
class UIScutBarClass : public UnknownAEEA88Class	// NAME 2: what UIScutClass's Bars holds (the FAIL_IFs' 'bar')
{
public:
	UIScutBarClass(UIScutClass *owner, UIMeshClass *mesh, LayerClass *layer, int bar, int *id, int index);	// 0x610F80 (the shortcut interface's constructor: itself, its mesh and layer, the bar's number twice and the next control id)  // NAME 1 [owner, mesh, layer, id, index]: what 0x610570 passes; NAME 3 [bar]: the FAIL_IFs'
	~UIScutBarClass();									// 0x611220
	virtual void Unknown_00(void *source, int *event);	// +0x00 0x612510: the buttons' shortcuts shown again  // NAME 1 [source, event]: the base's
	UIScutClass *Get_Owner() { return((UIScutClass *)Unknown04); }	// NAME 1: what it returns
	void Unknown_6112B0();								// 0x6112B0 (FAIL_IF '!ProxyButtons[lp]' l.479): the proxy buttons' help section
	void Unknown_611300(ControlKeysManagerClass *manager);	// 0x611300: the 'Use Slot %d' keys added  // NAME 1 [manager]: the interface's slot +0x70's
	void Unknown_611710(ControlKeysManagerClass *manager);	// 0x611710: the 'Use Slot %d' keys removed  // NAME 1 [manager]: the interface's slot +0x74's
	bool Unknown_611B10(int *group, int *bttn_idx);		// 0x611B10 (G_ASSERT 'false' l.564): the first free button from the current group on  // NAME 3 [group, bttn_idx]: 0x611BD0's
	bool Unknown_611BD0(GadgetClass *gadget, int bttn_idx, int group, bool show);	// 0x611BD0 (G_ASSERTs l.590 / 591 / 603): the shortcut at a button  // NAME 3 [gadget, bttn_idx, group]: the asserts'; NAME 1 [show]: neutral
	bool Unknown_611E20(GadgetClass *gadget);			// 0x611E20: the gadget's shortcuts cleared  // NAME 1 [gadget]: 0x611E70's
	bool Unknown_611E70(GadgetClass *gadget, int *group_ptr, int *bttn_idx_ptr);	// 0x611E70 (FAIL_IFs l.682-684): where the gadget is  // NAME 3 [gadget, group_ptr, bttn_idx_ptr]: the FAIL_IFs'
	void Unknown_611F60(int group);						// 0x611F60: the group shown ('bank_toggle_a/b_sn_1.wav')  // NAME 1 [group]: neutral
	bool Unknown_6120E0(int key);						// 0x6120E0 (G_ASSERT 'mouse' l.804): a control key  // NAME 1 [key]: the interface's slot +0x68's
	void Unknown_612430();								// 0x612430 (G_ASSERTs l.904 / 907): the active shortcut dropped
	void Unknown_6124F0(UIScutProxyGadgetClass *gadget);	// 0x6124F0: the active shortcut set  // NAME 1 [gadget]: neutral

	UIMeshClass *Mesh;									// +0x08 (its listener list at +0x04)  // NAME 2: what it holds
	int Index;											// +0x0C  // NAME 1: neutral
	UIScutProxyGadgetClass *ProxyButtons[NUM_SHORTCUT_BUTTONS];	// +0x10 (FAIL_IF '!ProxyButtons[lp]' l.479)  // NAME 3: the FAIL_IF's
	GadgetClass *Shortcuts[NUM_GROUPS][NUM_SHORTCUT_BUTTONS];	// +0x1C  // NAME 1: neutral
	int Counts[NUM_GROUPS];								// +0x34: the shortcuts a group holds  // NAME 1: neutral
	int Bar;											// +0x3C  // NAME 3: the FAIL_IFs' 'bar'
	UIScutBankGadgetClass *Bank;						// +0x40 (0x6125B0)  // NAME 2: its 'bank_%d' model
	RenderObjClass *Unknown44;							// +0x44 (the destructor releases it)
	UIScutProxyGadgetClass *ActiveShortcut;				// +0x48 (G_ASSERT 'ActiveShortcut->Owner == this' l.904)  // NAME 3: the assert's
	int Group;											// +0x4C: the group shown  // NAME 2: what it holds
	int FirstId;										// +0x50: the buttons' control ids  // NAME 1: neutral
	int SecondId;										// +0x54: the second set (FirstId + 3)  // NAME 1: neutral
};

// ui_scut.cpp's interface (0xB8 bytes, constructor 0x610570, tables 0xAF73D8 / 0xAF73CC / 0xAF73C0)
class UIScutClass : public Unknown5BB320Class	// NAME 3: class-from-file (D24) for ui_scut.cpp
{
public:
	UIScutClass(UIViewClass *view, LayerClass *layer);	// 0x610570 (0x57DD20 passes the view and the cockpit's layer)  // NAME 1 [view, layer]: what 0x57DD20 passes
	virtual ~UIScutClass();								// 0x610800 (slot 0x610750)
	virtual void Unknown_54();							// 0x610940
	virtual void Unknown_60();							// 0x610980
	virtual bool Unknown_68(int key);					// 0x610A40  // NAME 1 [key]: the base's
	virtual void Unknown_70(ControlKeysManagerClass *manager);	// 0x610780  // NAME 1 [manager]: the base's
	virtual void Unknown_74(ControlKeysManagerClass *manager);	// 0x6107C0  // NAME 1 [manager]: the base's
	void Unknown_5417D0(InterfaceClass *owner);		// 0x5417D0 (intrface.cpp's; the set-up passes the cockpit)  // NAME 1 [owner]: neutral
	bool Unknown_610B00(GadgetClass *gadget);			// 0x610B00: the gadget onto the first free button of either bar  // NAME 1 [gadget]: 0x611BD0's
	bool Unknown_610C70(GadgetClass *gadget, unsigned int bar);	// 0x610C70 (FAIL_IF l.325): onto a bar's first free button  // NAME 3 [bar]: the FAIL_IF's; NAME 1 [gadget]: 0x611BD0's
	bool Unknown_610D00(GadgetClass *gadget, unsigned int bar, int group, int bttn_idx);	// 0x610D00 (FAIL_IF l.336)  // NAME 3 [bar]: the FAIL_IF's; NAME 1 [gadget, group, bttn_idx]: 0x611BD0's
	bool Unknown_610D70(GadgetClass *gadget);			// 0x610D70 (no caller): the gadget's shortcuts cleared on both bars  // NAME 1 [gadget]: 0x611E20's
	bool Unknown_610E10(GadgetClass *gadget, int *bar, int *group, int *bttn_idx);	// 0x610E10 (FAIL_IFs l.353-356): where the gadget is  // NAME 3 [gadget, bar, group, bttn_idx]: the FAIL_IFs'
	UIScutProxyGadgetClass *Unknown_610F30();			// 0x610F30 (no caller): the proxy button the mouse holds

	UIScutBarClass *Bars[NUM_SHORTCUT_BARS];			// +0xAC  // NAME 2: what it holds
	bool UnknownB4;										// +0xB4: key 0x12 was down (0x610980)
};

#endif // UI_SCUT_H
