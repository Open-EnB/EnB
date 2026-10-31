// Earth & Beyond client (reconstruction): app\Client\ui_manu.cpp. Notes at the end of the file.
#include <stdio.h>
#include <string.h>
#include "ui_manu.h"
#include "client.h"
#include "..\packets\pkmanufactureitemcatagorypacket.h"
#include "rendobj.h"
#include "sbaseview.h"
#include "..\shared\unknown_debug.h"
#include "..\shared\auxdata\auxdata.h"

// fontmanager.h's text object and FontManagerClass at 0xBE67C4, fontdatabase.h's FontStringFilesClass at 0xBE67BC, as far as
// this file calls them (as ui_group.cpp declares them: neither header can be included next to sclient.h)
class FontStyleClass : public RenderObjClass	// NAME 1 [FontStyleClass]: fontmanager.h's
{
public:
	void Unknown_6CDBC0(const char *text, int unknown);	// 0x6CDBC0: the text set  // NAME 1 [text, unknown]: what this file passes
	void Unknown_6CE910(bool on);						// 0x6CE910: highlighted  // NAME 1 [on]: neutral

	char Unknown_Pad[0x2FC - sizeof(RenderObjClass)];
	float Unknown2FC;									// +0x2FC (0x5B3A70: 0.33f)
	char Unknown300[0x4C5 - 0x300];						// +0x300
	bool Unknown4C5;									// +0x4C5 (0x5B3A70: false)
	char Unknown4C6[0x4E4 - 0x4C6];						// +0x4C6
	bool Unknown4E4;									// +0x4E4 (0x5B2230: set on the level 3 rows)
};
class FontStringFilesClass { public: void Load(const char *name); void Unload(const char *name); };	// NAME 2: fontdatabase.h's
class FontManagerClass { public: void Add(RenderObjClass *object); void Remove(unsigned int id); FontStyleClass *Get_Style(const char *name, unsigned int id); };	// NAME 3: fontmanager.h's
extern FontManagerClass *Unknown_BE67C4;	// NAME 1: neutral (D10)

// gadget.cpp's gadget, as far as this file calls it
class GadgetClass	// NAME 3: class-from-file (D24) for app\client\gadget.cpp
{
public:
	virtual ~GadgetClass(); virtual void Unknown_04(); virtual void Unknown_08(); virtual void Unknown_0C();
	virtual void Unknown_10(); virtual void Unknown_14(); virtual void Unknown_18(); virtual void Unknown_1C();
	virtual void Unknown_20(); virtual void Unknown_24(); virtual void Unknown_28();
	virtual bool Unknown_2C(int a, int b, int c, int d);	// +0x2C (the check button's 0x7E7FC0: true when it toggled)  // NAME 1 [a, b, c, d]: neutral
	virtual void Unknown_30(); virtual void Unknown_34(); virtual void Unknown_38(); virtual void Unknown_3C();
	virtual void Unknown_40(bool on);					// +0x40  // NAME 1 [on]: neutral
	virtual void Unknown_44(); virtual void Unknown_48(); virtual void Unknown_4C(); virtual void Unknown_50();
	virtual void Unknown_54(); virtual void Unknown_58(); virtual void Unknown_5C();
	virtual bool Unknown_60();							// +0x60 (0x5B2060: the entry picked)
	virtual void Unknown_64(); virtual void Unknown_68(); virtual void Unknown_6C(); virtual void Unknown_70();
	virtual void Unknown_74(); virtual void Unknown_78(); virtual void Unknown_7C(); virtual void Unknown_80();
	virtual void Unknown_84(); virtual void Unknown_88();
	virtual void Unknown_8C(bool on);					// +0x8C  // NAME 1 [on]: neutral
	virtual void Unknown_90(); virtual void Unknown_94(); virtual void Unknown_98(); virtual void Unknown_9C();
	virtual void Unknown_A0();
	virtual void Unknown_A4(float value, int unknown);	// +0xA4 (the slider's: its position)  // NAME 1 [value, unknown]: neutral
	void Unknown_65FFA0(int arg);						// 0x65FFA0 (0x5B6410 passes -1)  // NAME 1 [arg]: neutral

	char Unknown04[0x2C - 0x04];						// +0x04
};

// gd_slider.cpp's slider, as far as this file calls it
class UIManuSliderClass : public GadgetClass	// NAME 1: neutral (D10)
{
public:
	void Unknown_66E460(const RefCountPtr<RenderObjClass> &model, int a, int b);	// 0x66E460 (the destructor passes none, 0, 0)  // NAME 1 [model, a, b]: neutral
	void Unknown_66FA90(float value, float rows, bool update);	// 0x66FA90 (this file: a row, 10.0f, true)  // NAME 1 [value, rows, update]: neutral

	char Unknown2C[0xD4 - 0x2C];						// +0x2C
	float Unknown0D4;									// +0xD4: the position (the first row shown)
};

// The client at InterfaceClass +0x40, as this file calls it (the FAIL_IF '!Client->As_Space_Client()')
class LineDisplayClientClass  // NAME 1: neutral (D10)
{
public:
	virtual void Unknown_00(); virtual void Unknown_04(); virtual void Unknown_08(); virtual void Unknown_0C();
	virtual void Unknown_10(); virtual void Unknown_14();
	virtual SClient *As_Space_Client();					// +0x18 (ClientClass::Unknown_18; SClient's returns itself)  // NAME 3: the FAIL_IF's
};

// SClient's functions this file calls that sclient.h does not declare yet
class UIManuSClientClass	// NAME 1: neutral (D10): SClient, as far as this file calls it
{
public:
	void Unknown_744560(int arg);						// 0x744560 (slot +0x5C passes 0x18)  // NAME 1 [arg]: neutral
	void Unknown_74D990(int color, const char *text, int a);	// 0x74D990 (as sclient.h's)  // NAME 1 [color, text, a]: as sclient.h's
};

// ui_mesh.cpp's and pdaint.cpp's functions this file calls that sclient.h does not declare yet
class UIManuMeshClass { public: void Unknown_5B9930(int arg); };	// NAME 1: neutral (D10): UIMeshClass's 0x5B9930 (the destructor passes -1)  // NAME 1 [arg]: neutral
class UIManuPDAClass { public: void Unknown_695A00(int a, int b); };	// NAME 1: neutral (D10): PDAInterfaceClass's 0x695A00 (slot +0x5C passes 0, 1)  // NAME 1 [a, b]: neutral

// The names of the window's texts (0xB81868)
static const char *Unknown_B81868[] = { "Go_tsm01a", "Cost_tsm01a", "Quality_Range_tsm01a", "Expected_Quality_tsm01a", "Result_tsm01a", "" };	// NAME 1: neutral (D10)
static unsigned int Unknown_B8183C = 9;					// NAME 1: neutral (D10): the filter requests allowed
static const float Unknown_AF2D18 = 1.0f;				// NAME 1: neutral (D10): what 0x5B3A70 stores (read from .rdata)

// NOT EXACT: order (the store of +0xB80's address before the vtables, ours before +0xB90's initialisers) (0x5AFA00)
UIManuClass::UIManuClass(SBaseViewClass *view, const LayerClass &layer) :
	Unknown5BB320Class(new UIMeshClass("TSM01A", true), (UIViewClass *)view, layer),
	Unknown0AC(view),
	Unknown0B0(NULL),
	Unknown0B90(0),
	Unknown0B94(0),
	Unknown0B98(-1),
	State(UI_CLOSED),
	Unknown0BA0(0),
	Unknown0BA4(0),
	Unknown0BA8(0),
	Unknown0BAC(false),
	Unknown0BAD(false),
	Unknown0BAE(false),
	Unknown0BB0(-1),
	Unknown0BB4(false),
	Unknown0BB8(0),
	Unknown0BBC(0),
	Unknown0BC0(0)
{
	memset(Unknown0B80, -1, sizeof(Unknown0B80));
	memset(Unknown0C0, 0, sizeof(Unknown0C0));
	memset(Unknown0CC, 0, sizeof(Unknown0CC));
	memset(Unknown0E8, 0, sizeof(Unknown0E8));
	memset(Unknown110, 0, sizeof(Unknown110));
	memset(Unknown16C, 0, sizeof(Unknown16C));
	memset(Unknown194, 0, sizeof(Unknown194));
	memset(Unknown1BC, 0, sizeof(Unknown1BC));
	memset(Unknown98C, 1, sizeof(Unknown98C));
	memset(Unknown11C, 0, sizeof(Unknown11C));
	((FontStringFilesClass *)Unknown_BE67BC)->Load("FA_TSM01A.INI");
}









// NOT EXACT: register (the loop's two pointers in edi / ebp the other way round) (0x5AFCD0)
UIManuClass::~UIManuClass()
{
	G_ASSERT(State == UI_CLOSED);
	for (int i = 0; i < 3; i++) {
		delete Unknown0C0[i];
		Unknown110[i] = NULL;
	}
	if (Unknown0B0) {
		Unknown0B0->Unknown_66E460(RefCountPtr<RenderObjClass>(), 0, 0);
	}
	((UIManuMeshClass *)Mesh)->Unknown_5B9930(-1);
	Unknown0B4 = NULL;
	Unknown0B8 = NULL;
	Unknown0BC = NULL;
	((FontStringFilesClass *)Unknown_BE67BC)->Unload("FA_TSM01A.INI");
}

// 0x5B0B40
void UIManuClass::Unknown_7C(UIViewClass *view, int arg)
{
	InterfaceClass::Unknown_7C(view, arg);
	if (Unknown0B4) {
		Unknown0B4->Unknown_7C(view, arg);
	}
	if (Unknown0B8) {
		Unknown0B8->Unknown_7C(view, arg);
	}
	if (Unknown0BC) {
		Unknown0BC->Unknown_7C(view, arg);
	}
}

// NOT EXACT: register (the packets' player id in eax, the three sub-windows' slot +0x5C calls one register off) (0x5B10A0)
void UIManuClass::Unknown_5C()
{
	State = UI_CLOSED;
	Unknown0AC->Unknown_69DC60();
	Unknown0BAE = false;
	SClient *sclient = Client->As_Space_Client();
	((ClientClass *)Client)->Send_Packet(&ManufactureActionPacketClass(sclient->Unknown1138, 0));
	((ClientClass *)Client)->Send_Packet(&ManufactureItemCatagoryPacketClass(sclient->Unknown1138, 0));
	Unknown0B4->Unknown_5C();
	Unknown0B8->Unknown_5C();
	Unknown0BC->Unknown_5C();
	if (Unknown0BAC) {
		((UIManuPDAClass *)sclient->PDAInterface)->Unknown_695A00(0, 1);
	}
	Unknown0BA0 = 0;
	Unknown_BE67C4->Remove((unsigned int)Mesh->Peek_Mesh_Object());
	Unknown5BB320Class::Unknown_5C();
	((UIManuSClientClass *)sclient)->Unknown_744560(0x18);
	for (int i = 0; i < 10; i++) {
		if (Unknown16C[i]) {
			Unknown16C[i]->Unknown_6CDBC0("", 0);
		}
		if (Unknown194[i]) {
			Unknown194[i]->Unknown_6CDBC0("", 0);
		}
	}
}

// The check button (0x68 bytes; constructor 0x7E7E70 over a render object and a layer, destructor 0x7E7FA0, table 0xB075E4)
class UIManuCheckButtonClass : public GadgetClass	// NAME 1: neutral (D10)
{
public:
	UIManuCheckButtonClass(RenderObjClass *robj, LayerClass *layer, int arg);	// 0x7E7E70 (0x5B6410: 0, or 0x55 for the filter buttons)  // NAME 1 [robj, layer, arg]: neutral
	virtual ~UIManuCheckButtonClass();					// 0x7E7FA0
	virtual bool Unknown_2C(int a, int b, int c, int d);	// +0x2C 0x7E7FC0
	void Unknown_7E8030(bool on);						// 0x7E8030: checked or not  // NAME 1 [on]: neutral

	char Unknown2C[0x64 - 0x2C];						// +0x2C
	bool Checked;										// +0x64  // NAME 2: what 0x7E8030 sets
};

// The tech level filter buttons: one per level (0x70 bytes, table 0xAF2DF8) and the "all levels" one (0x6C bytes,
// table 0xAF2EBC), made by 0x5B6410; a toggle calls the window back
class UIManuTechLevelButtonClass : public UIManuCheckButtonClass	// NAME 1: neutral (D10)
{
public:
	UIManuTechLevelButtonClass(RenderObjClass *robj, LayerClass *layer, UIManuClass *owner, int tech_level) : UIManuCheckButtonClass(robj, layer, 0x55), Owner(owner), TechLevel(tech_level) {}	// NAME 1 [robj, layer, owner]: neutral; NAME 3 [tech_level]: the FAIL_IF's
	virtual bool Unknown_2C(int a, int b, int c, int d)	// +0x2C 0x5B6680: 0x5B67C0 when it toggled (inline: emitted after 0x5B6410)
	{
		bool result = UIManuCheckButtonClass::Unknown_2C(a, b, c, d);
		if (result) {
			Owner->Unknown_5B67C0(TechLevel, Checked, USER_INPUT);
		}
		return(result);
	}

	UIManuClass *Owner;									// +0x68  // NAME 2: what it holds
	int TechLevel;										// +0x6C (1..9)  // NAME 3: the FAIL_IF's
};
class UIManuAllLevelsButtonClass : public UIManuCheckButtonClass	// NAME 1: neutral (D10)
{
public:
	UIManuAllLevelsButtonClass(RenderObjClass *robj, LayerClass *layer, UIManuClass *owner) : UIManuCheckButtonClass(robj, layer, 0x55), Owner(owner) {}	// NAME 1 [robj, layer, owner]: neutral
	virtual bool Unknown_2C(int a, int b, int c, int d)	// +0x2C 0x5B6720: 0x5B6A80 when it toggled (inline: emitted after 0x5B6410)
	{
		bool result = UIManuCheckButtonClass::Unknown_2C(a, b, c, d);
		if (result) {
			Owner->Unknown_5B6A80(Checked, USER_INPUT);
		}
		return(result);
	}

	UIManuClass *Owner;									// +0x68  // NAME 2: what it holds
};

void Unknown_496AE0(RenderObjClass *robj, int unknown);	// 0x496AE0 (ro_funcs.cpp, cdecl)  // NAME 1 [robj, unknown]: what the callers pass

// The formula lists under the target item's aux data ('KnownFormula', 'PreviousAttempts'; their class's global at +0x04 is
// 0xC41628) and their entries, as far as this file reads them
class UIManuEntryClass : public DeltaCompoundClass	// NAME 1: neutral (D10)
{
public:
	char Unknown09C[0x120 - 0x9C];						// +0x09C
	const char *Unknown120;								// +0x120: the name ("" ends the list)
	char Unknown124[0x1A8 - 0x124];						// +0x124
	int Unknown1A8;										// +0x1A8: the item id (0x5B39B0's)
};
class UIManuListClass : public DeltaCompoundClass	// NAME 1: neutral (D10)
{
public:
	static int UnknownGlobal;							// the per-class global at +0x04 (0xC41628)
};
int UIManuListClass::UnknownGlobal;
class UIManuCategoriesClass : public DeltaCompoundClass	// NAME 1: neutral (D10): 'PrimaryCategories' (the tree UIManuNodeClass reads)
{
public:
	static int UnknownGlobal;							// the per-class global at +0x04 (0xC4162C)
};
int UIManuCategoriesClass::UnknownGlobal;



























































































































































































































































































































































































// 0x5B1AF0
void UIManuClass::Unknown_5B1AF0(int index, bool show, const char *text)
{
	const char *name = Unknown_B81868[index]; RefCountPtr<FontStyleClass> font = Create_Peek(Unknown_BE67C4->Get_Style(name, (unsigned int)Mesh->Peek_Mesh_Object()));
	FAIL_IF(!font) return;
	font->Unknown_6CDBC0(text, 0);
	font->Set_Hidden(!show);
}

inline UIManuListClass *UIManuClass::Unknown_Find_List(const char *field) { NameClass name(field); return(Unknown_Find_Field<UIManuListClass>((DeltaCompoundClass *)Unknown0BA0, name)); }
inline UIManuListClass *UIManuClass::Unknown_Get_List()
{
	if (Unknown0BAD) {
		return(Unknown_Find_List("PreviousAttempts"));
	}
	return(Unknown_Find_List("KnownFormula"));
}

// NOT EXACT: inline budget (the client calls rtti.h's Cast out of line, 0x5B7260, in its 'PreviousAttempts' find and returns from each failed find) (0x5B1ED0)
void UIManuClass::Unknown_5B1ED0(int row)
{
	if (Unknown0B94 != 0 && row < Unknown0B94 && Unknown0B98 == 3) {
		int pick = Unknown0B90 + row;
		Unknown0B80[3] = pick;
		if (pick >= 0) {
			UIManuListClass *list = Unknown_Get_List();
			if (list && list->Unknown70) {
				Unknown_5B39B0(((UIManuEntryClass *)list->Children[pick])->Unknown1A8);
			}
		}
		Unknown_5B2230();
		Unknown_5B2480();
	}
}

// 0x5B2060
void UIManuClass::Unknown_5B2060(int index)
{
	if (Unknown0B98 < 3) {
		if (Unknown0C0[Unknown0B98]->Gadgets[index]->Unknown_60()) {
			Unknown0B80[Unknown0B98] = Unknown0C0[Unknown0B98]->IDs[index];
			for (int i = Unknown0B98 + 1; i <= 3; i++) {
				Unknown0B80[i] = -1;
			}
			Unknown0B98++;
			Unknown_5B6CA0();
			if (Unknown0B98 == 3) {
				Unknown_5B2FE0();
			}
		}
	}
	Unknown_5B2BD0(true);
	Unknown_5B2230();
	Unknown_5B2480();
}

// 0x5B21A0
void UIManuClass::Unknown_5B21A0(int row)
{
	if (Unknown0B80[Unknown0B98] == Unknown0B90 + row) {
		Unknown16C[row]->Unknown_6CE910(true);
		Unknown194[row]->Unknown_6CE910(true);
	} else {
		Unknown16C[row]->Unknown_6CE910(false);
		Unknown194[row]->Unknown_6CE910(false);
	}
}

// 0x5B2230
void UIManuClass::Unknown_5B2230()
{
	if (Unknown0BA0) {
		if (Unknown0B98 == 3) {
			for (int row = 0; row < 10; row++) {
				char name[64];
				char text[64];
				Unknown_5B2A80(name, text, Unknown0B90 + row);
				Unknown16C[row]->Unknown_6CDBC0(name, 0);
				Unknown16C[row]->Unknown4E4 = true;
				Unknown194[row]->Unknown_6CDBC0(text, 0);
				Unknown_5B21A0(row);
			}
			for (int i = 0; i < 3; i++) {
				Unknown0C0[i]->Unknown_5B74F0(false);
			}
			return;
		}
		for (int row = 0; row < 10; row++) {
			Unknown16C[row]->Unknown4E4 = false;
			Unknown16C[row]->Unknown_6CDBC0("", 0);
		}
		for (int i = 0; i < 3; i++) {
			if (i == Unknown0B98) {
				Unknown0C0[i]->Unknown_5B7570(Unknown98C, Unknown0B80[0], Unknown0B80[1]);
			} else {
				Unknown0C0[i]->Unknown_5B74F0(false);
			}
		}
	}
}

// 0x5B2820
void UIManuClass::Unknown_5B2820()
{
	Unknown0B98 = -1;
	for (int i = 0; i < 10; i++) {
		Unknown11C[i].Gadget->Unknown_40(false);
		Unknown11C[i].Gadget->Unknown_8C(true);
	}
	for (int j = 0; j < 4; j++) {
		Unknown0B80[j] = -1;
	}
	Unknown0B0->Unknown_66FA90(0, 10.0f, true);
	Unknown_5B2950();
}

// 0x5B28E0
void UIManuClass::Unknown_5B28E0(int top)
{
	Unknown0B0->Unknown_66FA90((float)top, 10.0f, true);
	Unknown_5B2950();
}

// 0x5B2950
void UIManuClass::Unknown_5B2950()
{
	int top = (int)Unknown0B0->Unknown0D4;
	if (Unknown0B90 != top) {
		Unknown0B90 = top;
		Unknown_5B2230();
	}
}

// 0x5B2990
void UIManuClass::Unknown_5B2990()
{
	UIManuSliderClass *slider = Unknown0B0;
	int top = (int)slider->Unknown0D4;
	int bottom = top + 9;
	int pick = Unknown0B80[3];
	if (pick >= 0) {
		if (pick < top) {
			slider->Unknown_A4((float)pick, 0);
			Unknown_5B2950();
		} else if (pick > bottom) {
			slider->Unknown_A4((float)(pick - bottom + top), 0);
			Unknown_5B2950();
		}
	}
}

// 0x5B2A80
void UIManuClass::Unknown_5B2A80(char *name, char *text, int index)
{
	const char *entry;
	if (index >= Unknown0B94 || (entry = Unknown1BC[index]) == NULL) {
		sprintf(name, "");
		sprintf(text, "");
		return;
	}
	strcpy(name, entry);
	strcpy(text, "");
}

// 0x5B2B40
void UIManuClass::Unknown_5B2B40()
{
	for (unsigned int i = 0; i < 500; i++) {
		Unknown1BC[i] = "";
		Unknown98C[i] = true;
	}
	Unknown0B94 = 0;
	Unknown0B0->Unknown_66FA90(0, 10.0f, true);
	Unknown_5B2950();
}

// 0x5B2F20
void UIManuClass::Unknown_5B2F20()
{
	if (!Unknown0BAD && Unknown0B98 == 3) {
		NameClass name("KnownFormula");
		UIManuListClass *list = Unknown_Find_Field<UIManuListClass>((DeltaCompoundClass *)Unknown0BA0, name);
		if (list && list->Unknown70 > 1) {
			Unknown_5B3260(list);
		}
	}
}

// NOT EXACT: inline budget (the client calls rtti.h's Cast out of line, 0x5B72B0; the item id read before the client) (0x5B2FE0)
void UIManuClass::Unknown_5B2FE0()
{
	if (Unknown0B98 == 3) {
		int a = Unknown0B80[0];
		int b = Unknown0B80[1];
		int c = Unknown0B80[2];
		NameClass name("PrimaryCategories");
		UIManuCategoriesClass *categories = Unknown_Find_Field<UIManuCategoriesClass>((DeltaCompoundClass *)Unknown0BA0, name);
		if (categories && categories->Unknown70) {
			int id = ((UIManuNodeClass *)categories)->Unknown084[a]->Unknown1A8[b]->Unknown230[c]->Unknown1A8;
			((ClientClass *)Client)->Send_Packet(&ManufactureItemCatagoryPacket_122Class(Client->As_Space_Client()->Unknown1138, id));
			Unknown_5B2B40();
			Unknown0BAD = false;
			Unknown_5B6CA0();
		}
	}
}

// 0x5B3260
void UIManuClass::Unknown_5B3260(UIManuListClass *list)
{
	int i = 0;
	int pick = -1;
	if (list) {
		Unknown0B94 = list->Children.size();
		for (i = 0; i < Unknown0B94; i++) {
			UIManuEntryClass *entry = (UIManuEntryClass *)list->Children[i];
			const char *name = entry->Unknown120;
			if (strcmp(name, "") == 0) {
				Unknown0B94 = i;
				break;
			}
			Unknown1BC[i] = name;
			Unknown98C[i] = false;
			if (Unknown0BB0 != -1 && Unknown0BB0 == entry->Unknown1A8) {
				pick = i;
			}
		}
	} else {
		Unknown0B94 = i;
	}
	for (i = Unknown0B94; i < 500; i++) {
		Unknown1BC[i] = NULL;
		Unknown98C[i] = true;
	}
	Unknown0B80[3] = pick;
	Unknown_5B28E0(Unknown0B94);
	Unknown_5B2990();
	Unknown0B98 = 3;
	Unknown_5B2230();
	Unknown_5B6CA0();
}

// 0x5B3580
void UIManuClass::Unknown_5B3580(bool force)
{
	if (Unknown0BAD) {
		NameClass name("PreviousAttempts");
		UIManuListClass *list = Unknown_Find_Field<UIManuListClass>((DeltaCompoundClass *)Unknown0BA0, name);
		if (list && list->Unknown70 && (force || list->Unknown70 > 1)) {
			Unknown_5B3260(list);
		}
	}
}

// NOT EXACT: inline budget (the client calls rtti.h's Cast out of line, 0x5B7260, in both copies of 0x5B3580; registers) (0x5B3640)
void UIManuClass::Unknown_5B3640(bool all)
{
	if (!Unknown0BAD) {
		Unknown0BB0 = -1;
	}
	Unknown0BAD = true;
	Unknown_5B3580(true);
	Unknown_5B6CA0();
	if (all) {
		for (int i = 0; i < 4; i++) {
			Unknown0B80[i] = -1;
		}
	} else {
		Unknown0B80[3] = -1;
	}
	Unknown_5B3580(true);
	Unknown_5B2480();
	Unknown_5B4850();
}

// NOT EXACT: inline budget (as 0x5B1ED0) (0x5B3850)
void UIManuClass::Unknown_5B3850()
{
	int pick = Unknown0B80[3];
	if (pick >= 0) {
		UIManuListClass *list = Unknown_Get_List();
		if (list && list->Unknown70) {
			Unknown_5B39B0(((UIManuEntryClass *)list->Children[pick])->Unknown1A8);
		}
	}
}

// 0x5B39B0
void UIManuClass::Unknown_5B39B0(int item_id)
{
	if (item_id != -1) {
		int player = Client->As_Space_Client()->Unknown1138;
		((ClientClass *)Client)->Send_Packet(&ManufactureItemIdPacketClass(player, item_id));
		Unknown0BA4 = 1;
		Unknown0BB0 = item_id;
	}
}

// 0x5B3A70
void UIManuClass::Unknown_5B3A70()
{
	if (Unknown0B80[3] >= 0) {
		int player = Client->As_Space_Client()->Unknown1138;
		((ClientClass *)Client)->Send_Packet(&ManufactureActionPacketClass(player, 2));
		Unknown0BA4 = 2;
		Unknown0BA8 = Unknown_AF2D18;
		if (Unknown0BC) {
			Unknown0BC->Unknown_62F5B0(false);
		}
		Unknown0CC[6]->Unknown_40(false);
		FontStyleClass *style = Unknown_BE67C4->Get_Style(Unknown_B81868[0], (unsigned int)Mesh->Peek_Mesh_Object());
		style->Unknown2FC = 0.33f;
		style->Unknown4C5 = false;
	}
}

// 0x5B3B90
bool UIManuClass::Unknown_5B3B90(UIManuNodeClass *tree, int a)
{
	if (tree) {
		UIManuLevel1Class *level1 = tree->Unknown084[a];
		int count = level1->Unknown1A8.size();
		for (int i = 0; i < count; i++) {
			if (!Unknown_5B3CA0(tree, a, i)) {
				return(false);
			}
		}
	}
	return(true);
}

// 0x5B3CA0
bool UIManuClass::Unknown_5B3CA0(UIManuNodeClass *tree, int a, int b)
{
	if (tree) {
		UIManuLevel2Class *level2 = tree->Unknown084[a]->Unknown1A8[b];
		int count = level2->Unknown230.size();
		for (int i = 0; i < count; i++) {
			if (!Unknown_5B3D70(tree, a, b, i)) {
				return(false);
			}
		}
	}
	return(true);
}

// 0x5B3D70
bool UIManuClass::Unknown_5B3D70(UIManuNodeClass *tree, int a, int b, int c)
{
	if (tree) {
		UIManuItemClass *item = tree->Unknown084[a]->Unknown1A8[b]->Unknown230[c];
		int count3 = item->Unknown084.size();
		for (int k = 0; k < count3; k++) {
			if (item->Unknown230) {
				return(false);
			}
		}
	}
	return(true);
}








































































































































































































































































































































































































































































































































































































































































































































// 0x5B6410
UIManuCheckButtonClass *UIManuClass::Unknown_5B6410(int id, const char *bttn_name, int tech_level, bool plain)
{
	FAIL_IF(!bttn_name || !*bttn_name) return(NULL);

	RefCountPtr<RenderObjClass> robj = Create_Get(Mesh->Peek_Object()->Get_Sub_Object_By_Name(bttn_name));
	FAIL_IF(!robj) return(NULL);
	Unknown_496AE0(robj.Peek(), 0);
	UIManuCheckButtonClass *button;
	if (plain) {
		button = new UIManuCheckButtonClass(robj.Peek(), &Layer, 0);
	} else if (tech_level == NUM_TECH_LEVEL_FILTER_CHECK_BUTTONS) {
		button = new UIManuAllLevelsButtonClass(robj.Peek(), &Layer, this);
	} else {
		button = new UIManuTechLevelButtonClass(robj.Peek(), &Layer, this, tech_level);
	}
	Unknown64.Unknown_6870E0(button, id, View, 1, 0);
	button->Unknown_65FFA0(-1);
	return(button);
}




// NOT EXACT: shape (the client tests the button's state through a materialised bool, setne / test; registers) (0x5B67C0)
void UIManuClass::Unknown_5B67C0(int tech_level, bool on, int source)
{
	FAIL_IF(tech_level < 1 || tech_level >= NUM_TECH_LEVEL_FILTER_CHECK_BUTTONS) return;
	if (Unknown11C[tech_level - 1].Gadget->Checked != on) {







		G_ASSERT(source != USER_INPUT);
		Unknown11C[tech_level - 1].Gadget->Unknown_7E8030(on);
	}
	if (source == USER_INPUT) {
		unsigned int bit = 1 << (tech_level - 1);
		ManufactureTechLevelFilterPacketClass packet(((ClientClass *)Client)->Unknown1138, on, bit);
		((ClientClass *)Client)->Send_Packet(&packet);
		Unknown0BC0++;
		if (Unknown0BC0 > Unknown_B8183C) {
			Unknown_5B6D30();
			Unknown_5B6CA0();
		}
	}
	bool all;
	if (!on) {
		all = false;
	} else {
		int i;
		for (i = 1; i < NUM_TECH_LEVEL_FILTER_CHECK_BUTTONS; i++) {
			if (!Unknown11C[i - 1].Gadget->Checked) {
				break;
			}
		}
		all = (i == NUM_TECH_LEVEL_FILTER_CHECK_BUTTONS);
	}
	Unknown11C[NUM_TECH_LEVEL_FILTER_CHECK_BUTTONS - 1].Gadget->Unknown_7E8030(all);
}










// 0x5B6A80
void UIManuClass::Unknown_5B6A80(bool on, int source)
{
	if (source == USER_INPUT) {
		int player = ((ClientClass *)Client)->Unknown1138;
		ManufactureTechLevelFilterPacketClass packet(player, on, 0x1FF);
		((ClientClass *)Client)->Send_Packet(&packet);
		Unknown0BC0++;
		if (Unknown0BC0 > Unknown_B8183C) {
			Unknown_5B6D30();
			Unknown_5B6CA0();
		}
	}
	if (Unknown11C[NUM_TECH_LEVEL_FILTER_CHECK_BUTTONS - 1].Gadget->Checked != on) {


		G_ASSERT(source != USER_INPUT);
		Unknown11C[NUM_TECH_LEVEL_FILTER_CHECK_BUTTONS - 1].Gadget->Unknown_7E8030(on);
	}
	for (int i = 0; i < NUM_TECH_LEVEL_FILTER_CHECK_BUTTONS - 1; i++) {
		Unknown11C[i].Gadget->Unknown_7E8030(on);
	}
}

// 0x5B6CA0
void UIManuClass::Unknown_5B6CA0()
{
	bool on = (!Unknown0BAD && Unknown0B98 == 3 && Unknown0BC0 <= Unknown_B8183C) ? true : false;
	for (int i = 0; i < 10; i++) {
		Unknown11C[i].Gadget->Unknown_40(on);
		Unknown11C[i].Gadget->Unknown_8C(!on);
	}
}






// 0x5B6D30
void UIManuClass::Unknown_5B6D30()
{
	FAIL_IF(!Client) return;
	FAIL_IF(!Client->As_Space_Client()) return;
	((UIManuSClientClass *)Client->As_Space_Client())->Unknown_74D990(17, "Warning: Exceeded filter request count.  Please wait.", 0);
}

/*
**	Reconstruction notes (MU1, 2026-10-31)
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4). Original location:
**	e:\build\beta\G\app\Client\ui_manu.cpp (its __FILE__). Client 0x5AFA00-0x5B6D8E: the manufacturing window
**	UIManuClass (ui_manu.h), over sclient.h's interface base Unknown5BB320Class through its second constructor
**	0x5BB250 (a view and a layer). The constructor's three tables 0xAF2D4C / 0xAF2D40 / 0xAF2D34 are InterfaceClass's
**	three (slots +0x54 0x5AFFC0, +0x58 0x5B0BB0, +0x5C 0x5B10A0, +0x60 0x5B1230, +0x68 0x5B16C0, +0x7C 0x5B0B40).
**
**	The asserts sit on the client's lines (D25): 'State == UI_CLOSED' 138, '!font' 651 (0x5B1AF0, inlined in five
**	callers), the button maker's 1722 / 1725 (0x5B6410), the tech level filters' 1746 / 1755 (0x5B67C0) and 1808
**	(0x5B6A80), '!Client' 1834 and '!Client->As_Space_Client()' 1835 (0x5B6D30, inlined in 0x5B67C0 / 0x5B6A80).
**	The filter buttons' slot +0x2C overrides (0x5B6680, 0x5B6720) are inline in their classes: VC6 emits them, with
**	the implicit destructors (bare jumps to 0x7E7FA0), after 0x5B6410, which makes the buttons.
**	The functions without asserts between them are placed in the client's address order; their lines are free.
**
**	Forms: the four picks at +0xB80 are reset by a loop (VC6 unrolls it into four stores of one register; separate
**	statements or a chained assignment keep -1 in a register across the rows' loop, memset goes through a pointer),
**	while the constructor's are a memset. The tree checks 0x5B3B90 / 0x5B3CA0 call the next level's check (inlined: the
**	level pointer hoisted, the vector indexed again on every pass). 0x5B1AF0 reads the name into a local before the
**	mesh. The formula lists are found through one inline helper holding the NameClass (one stack slot for both
**	names, as co.cpp's Unknown_Find_Aux). The player id the packets carry is the space client's +0x1138 (ClientClass::Unknown1138).
**
**	Not rebuilt yet: slots +0x54 (0x5AFFC0, 0x92E bytes), +0x58, +0x60, +0x68, the list fillers 0x5B2230 / 0x5B2480 /
**	0x5B2BD0 / 0x5B3E00, 0x5B1C20 (line 685), 0x5B4680 ('CurrentItemCategory'), the icon loader 0x5B42F0 and the quality texts 0x5B4B10 / 0x5B5320
**	(lines 1473-1646). The constructor, destructor, slot +0x5C, 0x5B1ED0, 0x5B2FE0, 0x5B3640, 0x5B3850 and 0x5B67C0 compile but
**	are not exact (see their NOT EXACT notes).
*/
