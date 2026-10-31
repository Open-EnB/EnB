// Earth & Beyond client (reconstruction): app\client\ui_manu.h, the manufacturing window (the client's ui_manu.cpp per its
// asserts; this header's name is provisional, NAME 1). Laid out from the constructor 0x5AFA00 and the destructor 0x5AFCD0
// (MU1, 2026-10-31).
#ifndef UI_MANU_H  // NAME 1 [ui_manu.h]: provisional header name for ui_manu.cpp
#define UI_MANU_H

#include "sclient.h"

class GadgetClass;			// gadget.cpp's
class UIManuCheckButtonClass;	// the check buttons (the tech level filters at +0x11C)
class FontStyleClass;		// fontmanager.h's text objects (+0x16C, +0x194)
class SBaseViewClass;		// sbaseview.cpp's starbase view (+0xAC)
class UIManuSliderClass;	// gd_slider.cpp's slider (+0xB0)
class UIManuClass;
class UIManuListClass;		// a formula list of the target item's aux data
class NameClass;

// What +0xC0..+0xC8 point to (0x4C bytes; constructor 0x5B73C0 over the window, destructor 0x5B7430; its functions follow
// ui_manu.cpp's): five gadgets and five text objects that fade in and out
class UIManuUnknownC0Class	// NAME 1: neutral (D10)
{
public:
	~UIManuUnknownC0Class();							// 0x5B7430
	void Unknown_5B74F0(bool on);						// 0x5B74F0: hidden at once (true), else faded  // NAME 1 [on]: neutral
	void Unknown_5B7570(bool *disabled, int primary, int secondary);	// 0x5B7570 (0x5B2230: the window's +0x98C and the first two picks)  // NAME 1 [disabled, primary, secondary]: neutral

	GadgetClass *Gadgets[5];							// +0x00 (slot +0x60: picked)  // NAME 2: what it holds
	RenderObjClass *Unknown14[5];						// +0x14 (released by the destructor; slot +0x184)
	int IDs[5];											// +0x28 (-1 for none; what 0x5B2060 picks)  // NAME 2: what it holds
	int Unknown3C;										// +0x3C: fading in (1) or out (-1)
	int Unknown40;										// +0x40
	float Unknown44;									// +0x44
	UIManuClass *Unknown48;								// +0x48 (the constructor's window)
};

// One tech level filter (8 bytes, ten at +0x11C: levels 1..9, then all)
struct UIManuRowStruct	// NAME 1: neutral (D10)
{
	UIManuCheckButtonClass *Gadget;						// +0x00 (slots +0x40 / +0x8C)  // NAME 2: what it holds
	int Unknown04;										// +0x04
};

// The levels of the category tree that 0x5B3B90 / 0x5B3CA0 / 0x5B3D70 walk, as far as they read them
class UIManuItemClass	// NAME 1: neutral (D10): the bottom level
{
public:
	char Unknown000[0x84];								// +0x000
	std::vector<void *> Unknown084;						// +0x084 (its size bounds the flag test)
	char Unknown094[0x1A8 - 0x094];						// +0x094
	int Unknown1A8;										// +0x1A8: the item id (0x5B2FE0 sends it)
	char Unknown1AC[0x230 - 0x1AC];						// +0x1AC
	bool Unknown230;									// +0x230
};
class UIManuLevel2Class	// NAME 1: neutral (D10)
{
public:
	char Unknown000[0x230];								// +0x000
	std::vector<UIManuItemClass *> Unknown230;			// +0x230
};
class UIManuLevel1Class	// NAME 1: neutral (D10)
{
public:
	char Unknown000[0x1A8];								// +0x000
	std::vector<UIManuLevel2Class *> Unknown1A8;		// +0x1A8
};
class UIManuNodeClass	// NAME 1: neutral (D10): the tree's root
{
public:
	char Unknown000[0x84];								// +0x000
	std::vector<UIManuLevel1Class *> Unknown084;		// +0x084
};

#define NUM_TECH_LEVEL_FILTER_CHECK_BUTTONS	10	// the FAIL_IF's (0x5B67C0 compares with 9 and 1)

// What +0xBC holds, as far as this file calls it
class UIManuUnknownBCClass : public InterfaceClass	// NAME 1: neutral (D10)
{
public:
	void Unknown_62F5B0(bool on);						// 0x62F5B0 (0x5B3A70 passes false)  // NAME 1 [on]: neutral
};

enum
{
	USER_INPUT = 0,										// (the G_ASSERTs 'source != USER_INPUT'; the buttons pass 0)  // NAME 3: the asserts'
	UI_CLOSED = 0										// (the destructor's G_ASSERT 'State == UI_CLOSED'; slot +0x5C sets it)  // NAME 3: the assert's
};

// ui_manu.cpp's manufacturing window (0xBC4 bytes, tables 0xAF2D4C / 0xAF2D40 / 0xAF2D34; made by 0x69F8F0, sbaseview.cpp)
class UIManuClass : public Unknown5BB320Class	// NAME 3: class-from-file (D24) for app\client\ui_manu.cpp
{
public:
	UIManuClass(SBaseViewClass *view, const LayerClass &layer);	// 0x5AFA00 (0xBC4 bytes)  // NAME 1 [view, layer]: what 0x69F8F0 passes
	virtual ~UIManuClass();								// 0x5AFCD0 (slot 0x5AFCA0)
	virtual void Unknown_54();							// 0x5AFFC0
	virtual void Unknown_58();							// 0x5B0BB0
	virtual void Unknown_5C();							// 0x5B10A0
	virtual void Unknown_60();							// 0x5B1230
	virtual bool Unknown_68(int key);					// 0x5B16C0  // NAME 1 [key]: InterfaceClass's
	virtual void Unknown_7C(UIViewClass *view, int arg);	// 0x5B0B40 (InterfaceClass's, then the three sub-windows')  // NAME 1 [view, arg]: InterfaceClass's
	void Unknown_5B1AF0(int index, bool show, const char *text);	// 0x5B1AF0 (FAIL_IF '!font', l.651): a text of Unknown_B81868 set and shown  // NAME 1 [index, show, text]: neutral
	void Unknown_5B1ED0(int row);						// 0x5B1ED0: row picked on level 3, its item id asked for  // NAME 1 [row]: neutral
	void Unknown_5B2060(int index);						// 0x5B2060: entry index of the current level picked, the next level shown  // NAME 1 [index]: neutral
	void Unknown_5B21A0(int row);						// 0x5B21A0: the row's two texts highlighted when it is the current level's pick  // NAME 1 [row]: neutral
	void Unknown_5B2230();								// 0x5B2230: the rows refilled (level 3: names; else the category buttons)
	void Unknown_5B2820();								// 0x5B2820: no level, no picks, the slider to the top
	void Unknown_5B28E0(int top);						// 0x5B28E0: the slider to row top  // NAME 1 [top]: neutral
	void Unknown_5B2950();								// 0x5B2950: the first row from the slider (the rows refilled when it moved)
	void Unknown_5B2990();								// 0x5B2990: the slider moved to show the level 3 pick
	void Unknown_5B2A80(char *name, char *text, int index);	// 0x5B2A80: list entry index's name, else ""  // NAME 1 [name, text, index]: neutral
	void Unknown_5B2B40();								// 0x5B2B40: the list emptied
	void Unknown_5B2BD0(bool arg);						// 0x5B2BD0 ('PrimaryCategories')  // NAME 1 [arg]: what 0x5B2060 passes
	void Unknown_5B2FE0();								// 0x5B2FE0 ('PrimaryCategories')
	void Unknown_5B2480();								// 0x5B2480 (FAIL_IF '!font' inlined twice)
	void Unknown_5B2F20();								// 0x5B2F20: the known formulas listed (level 3)
	void Unknown_5B3260(UIManuListClass *list);			// 0x5B3260: the list filled from a formula list  // NAME 1 [list]: neutral
	void Unknown_5B3580(bool force);					// 0x5B3580: the previous attempts listed  // NAME 1 [force]: neutral
	void Unknown_5B3640(bool all);						// 0x5B3640: to the previous attempts  // NAME 1 [all]: neutral
	void Unknown_5B3850();								// 0x5B3850: the level 3 pick's item id asked for
	void Unknown_5B4850();								// 0x5B4850 (FAIL_IF '!font' inlined twice)
	UIManuListClass *Unknown_Find_List(const char *field);	// (inline) the formula list named field  // NAME 1: neutral (D10)
	UIManuListClass *Unknown_Get_List();				// (inline in 0x5B1ED0 / 0x5B3850) the previous attempts, else the known formulas  // NAME 1: neutral (D10)
	void Unknown_5B39B0(int item_id);					// 0x5B39B0: MANUFACTURE_ITEM_ID 123 sent  // NAME 2 [item_id]: the packet's ItemID
	void Unknown_5B3A70();								// 0x5B3A70: MANUFACTURE_ACTION 126 of 2 sent for the level 3 pick
	bool Unknown_5B3B90(UIManuNodeClass *tree, int a);	// 0x5B3B90: no item under the primary category a is flagged  // NAME 1 [tree, a]: neutral
	bool Unknown_5B3CA0(UIManuNodeClass *tree, int a, int b);	// 0x5B3CA0: the same under a / b  // NAME 1 [tree, a, b]: neutral
	bool Unknown_5B3D70(UIManuNodeClass *tree, int a, int b, int c);	// 0x5B3D70: the same for a / b / c  // NAME 1 [tree, a, b, c]: neutral
	UIManuCheckButtonClass *Unknown_5B6410(int id, const char *bttn_name, int tech_level, bool plain);	// 0x5B6410: a check button over the mesh's sub-object (FAIL_IFs '!bttn_name || !*bttn_name' l.1722, '!robj' l.1725)  // NAME 3 [bttn_name]: the FAIL_IF's; NAME 1 [id, tech_level, plain]: neutral
	void Unknown_5B67C0(int tech_level, bool on, int source);	// 0x5B67C0: a tech level filter set (FAIL_IF l.1746, G_ASSERT 'source != USER_INPUT' l.1755)  // NAME 3 [tech_level, source]: the asserts'; NAME 1 [on]: neutral
	void Unknown_5B6A80(bool on, int source);			// 0x5B6A80: every tech level filter set (G_ASSERT l.1808)  // NAME 3 [source]: the assert's; NAME 1 [on]: neutral
	void Unknown_5B6CA0();								// 0x5B6CA0: the rows enabled at level 3 when nothing is pending
	void Unknown_5B6D30();								// 0x5B6D30: the filter request warning (FAIL_IFs '!Client' l.1834, '!Client->As_Space_Client()' l.1835)

	SBaseViewClass *Unknown0AC;							// +0x0AC (the constructor's view)
	UIManuSliderClass *Unknown0B0;						// +0x0B0 (the list's slider: its +0xD4 the first row)
	RefCountPtr<InterfaceClass> Unknown0B4;				// +0x0B4 (slots +0x5C / +0x7C passed on)
	RefCountPtr<InterfaceClass> Unknown0B8;				// +0x0B8
	RefCountPtr<UIManuUnknownBCClass> Unknown0BC;		// +0x0BC
	UIManuUnknownC0Class *Unknown0C0[3];				// +0x0C0 (the destructor deletes them)
	GadgetClass *Unknown0CC[7];							// +0x0CC (+0xE4: the go button)
	int Unknown0E8[10];									// +0x0E8
	RefCountPtr<RenderObjClass> Unknown110[3];			// +0x110
	UIManuRowStruct Unknown11C[10];						// +0x11C the list's rows
	RefCountPtr<FontStyleClass> Unknown16C[10];			// +0x16C the rows' first texts
	RefCountPtr<FontStyleClass> Unknown194[10];			// +0x194 the rows' second texts
	const char *Unknown1BC[500];						// +0x1BC the list's names ("" when empty)
	bool Unknown98C[500];								// +0x98C
	int Unknown0B80[4];									// +0xB80 the pick on each level (-1 for none)
	int Unknown0B90;									// +0xB90 the first row shown
	int Unknown0B94;									// +0xB94 the entries in the list
	int Unknown0B98;									// +0xB98 the current level (-1 for none; 3 the items)
	int State;											// +0xB9C (UI_CLOSED when shut)  // NAME 3: the assert's
	void *Unknown0BA0;									// +0xBA0: the target item's aux data (a DeltaCompoundClass)
	int Unknown0BA4;									// +0xBA4 (1 while an item id is pending)
	float Unknown0BA8;									// +0xBA8 (0x5B3A70: 1.0f)
	bool Unknown0BAC;									// +0xBAC
	bool Unknown0BAD;									// +0xBAD
	bool Unknown0BAE;									// +0xBAE
	int Unknown0BB0;									// +0xBB0 the item id asked for (-1 for none)
	bool Unknown0BB4;									// +0xBB4
	int Unknown0BB8;									// +0xBB8
	int Unknown0BBC;									// +0xBBC
	unsigned int Unknown0BC0;							// +0xBC0 (compared with Unknown_B8183C)
};

#endif // UI_MANU_H
