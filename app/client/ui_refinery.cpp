// Earth & Beyond client (reconstruction): app\Client\ui_refinery.cpp. Notes at the end of the file.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ui_refinery.h"
#include "client.h"
#include "sbaseview.h"
#include "gadget.h"
#include "unknown_675120.h"
#include "gd_slider.h"
#include "local_interface_sort.h"
#include "rendobj.h"
#include "control_keys_manager.h"
#include "..\packets\pkmanufactureitemcatagorypacket.h"
#include "..\shared\g_assert.h"
#include "..\shared\auxdata\auxdata.h"
#include "..\shared\money.h"
#include "..\audio\soundlib.h"
#include "fontmanager.h"	// FontSys, FontManagerClass (0xBE67C4) and fontdatabase.h's FontStringFilesClass (0xBE67BC)
#include "..\..\ww3d2\ww3d.h"	// WW3D::Get_Frame_Time (read inline)

extern FontManagerClass *Unknown_BE67C4;	// NAME 1: neutral (D10)

// The client at InterfaceClass +0x40, as this file calls it (as ui_manu.cpp declares it)
class LineDisplayClientClass  // NAME 1: neutral (D10)
{
public:
	virtual void Unknown_00(); virtual void Unknown_04(); virtual void Unknown_08(); virtual void Unknown_0C();
	virtual void Unknown_10(); virtual void Unknown_14();
	virtual SClient *As_Space_Client();					// +0x18 (ClientClass::As_Space_Client; SClient's returns itself)  // NAME 3: ui_manu.cpp's FAIL_IF's
};

void Unknown_496AE0(RenderObjClass *robj, bool make_geometry_unique);	// 0x496AE0 (ro_funcs.cpp, cdecl)  // NAME 1 [robj, make_geometry_unique]: as ui_manu.cpp's
RefCountPtr<RenderObjClass> Unknown_497300(RenderObjClass *robj);	// 0x497300 (cdecl, ro_funcs.cpp's neighbourhood)  // NAME 1 [robj]: as ui_avatr.cpp's
#include "..\shared\unknown_4a1590.h"

// The control keys event slot +0x54 registers (0x10 bytes; as ui_jobs.cpp's)










enum
{
	UI_CLOSED = 0,										// (the destructor's G_ASSERT 'State == UI_CLOSED'; slot +0x5C sets it)  // NAME 3: the assert's
	UI_OPEN = 2											// (slot +0x58 sets it)  // NAME 1: neutral (D10)
};

// The window's buttons (0xB85C38: slot +0x54's ids 1..3), the names of its texts (0xB85C48) and its messages (0xB85C60)
static const char *Unknown_B85C38[] = { "not used", "exit", "go", "r_stack" };	// NAME 1: neutral (D10)
static const char *Unknown_B85C48[] = { "Credits_TSM03a", "Go_TSM03a", "Stack_TSM03a", "Cost_TSM03a", "Result_TSM03a", "" };	// NAME 1: neutral (D10)
static const char *Unknown_B85C60[] = { "Refining Resource", "Attempt Succeeded" };	// NAME 1: neutral (D10)

// @0x5EB810
UIRefineryClass::UIRefineryClass(SBaseViewClass *view, const LayerClass &layer) :
	Unknown5BB320Class(new UIMeshClass("TSM03A", true), (UIViewClass *)view, layer),
	Unknown0AC(view),
	Unknown0B0(NULL),
	Unknown0B4(0),
	Unknown0B8(0),
	Unknown0AE0(0),
	Unknown0AE4(0),
	State(UI_CLOSED),
	Unknown0AEC(0),
	Unknown0AF0(0),
	Unknown0AF4(0),
	Unknown0AF8(-1),
	Unknown0AFC(false),
	Unknown0AFD(false),
	Unknown0B00(-1),
	Unknown0B04(false),
	Unknown0B05(false)
{
	memset(Unknown0BC, 0, sizeof(Unknown0BC));
	memset(Unknown0CC, 0, sizeof(Unknown0CC));
	for (int i = 0; i < 10; i++) {
		Unknown0F4[i] = NULL;
	}
	memset(Unknown11C, 0, sizeof(Unknown11C));
	memset(Unknown8EC, 1, sizeof(Unknown8EC));
	Unknown_BE67BC->Load("FA_TSM03A.INI");
}



// @0x5EBA50
UIRefineryClass::~UIRefineryClass()
{
	G_ASSERT(State == UI_CLOSED);
	if (Unknown0B0) {
		Unknown0B0->Unknown_66E460(RefCountPtr<RenderObjClass>(), 0, 0);
	}
	Unknown0B4 = NULL;
	Unknown0B8 = NULL;
	Unknown_BE67BC->Unload("FA_TSM03A.INI");
}

// @0x5EBC30
void UIRefineryClass::Unknown_54()
{
	for (int id = 1; id < 4; id++) {
		RefCountPtr<RenderObjClass> robj = Create_Get(InterfaceMesh->Peek_Object()->Get_Sub_Object_By_Name(Unknown_B85C38[id]));
		if (robj) {
			Unknown_496AE0(robj.Peek(), 0);
			Unknown0BC[id] = new ButtonGadgetClass(robj.Peek(), Layer, 0x55, 3);
			Unknown64.Unknown_6870E0(Unknown0BC[id], id, View, 1, 0);
			Unknown_5BB4F0(Unknown0BC[id]);
		}
	}
	char name[64];
	for (int row = 0; row < 10; row++) {
		_snprintf(name, sizeof(name), "desc%02d", row);
		Unknown0CC[row] = new ButtonGadgetClass(InterfaceMesh, name, Layer, 0x55, 3);
		Unknown64.Unknown_6870E0(Unknown0CC[row], row + 5, View, 1, 0);
		Unknown_5BB4F0(Unknown0CC[row]);
	}
	GadgetClass::Set_Tooltip(Unknown0BC[2], "UI_REFINE", "ICON_GO");
	GadgetClass::Set_Tooltip(Unknown0BC[3], "UI_REFINE", "ICON_STACK");
	((ControlKeyListClass *)&Unknown4C)->Add(KernelEventClass(0x1B), (void *)1);
	Unknown0B0 = new GadgetSlider(false, &Layer);
	Unknown0B0->Unknown_66E460(Unknown_497300(InterfaceMesh->Peek_Object()), 0, 1);
	Unknown64.Unknown_6870E0(Unknown0B0, 15, View, 1, 0);
	Unknown_5BB4F0(Unknown0B0);
	Unknown0B4 = Create_Get(new UIInvRobjStripClass(Client->As_Space_Client(), InterfaceMesh->Peek_Object(), 6, "TSM03A.E%02d", 12));
	Unknown0B4->Unknown_54();
	Unknown0B4->Unknown_7C(View, 0);
	Unknown0B8 = Create_Get(new UIInvRobjStripClass(Client->As_Space_Client(), InterfaceMesh->Peek_Object(), 1, "TSM03A.RESULT", 13));
	Unknown0B8->Unknown_54();
	Unknown0B8->Unknown_7C(View, 0);
	RefCountPtr<RenderObjClass> tutorial = Create_Get(InterfaceMesh->Peek_Object()->Get_Sub_Object_By_Name("tutorial"));
	if (tutorial) {
		tutorial->Set_Hidden(true);
	}
	Unknown_10();
}

// @0x5EC220
void UIRefineryClass::Unknown_7C(UIViewClass *view, bool arg)
{
	InterfaceClass::Unknown_7C(view, arg);
	Unknown0B4->Unknown_7C(view, arg);
	Unknown0B8->Unknown_7C(view, arg);
}

static inline int Refinery_Player_Id(SClient *sclient) { return sclient->Unknown1138; }  // NAME 2: the player id read as an inline accessor (as ui_manu.cpp's Manu_Player_Id)

// @0x5EC270
void UIRefineryClass::Unknown_58()
{
	Unknown_Find_Field<DeltaTemplateClass<int> >(NULL, *(NameClass *)this); Unknown_Find_Field<DeltaTemplateClass<int> >(NULL, *(NameClass *)this); Unknown_Find_Field<DeltaTemplateClass<int> >(NULL, *(NameClass *)this);	// not the client's (D130): dead finds spending the inline budget the client spent (it calls 0x5ED310 out of line here; 2 are not enough)
	SClient *sclient = Client->As_Space_Client();
	((ClientClass *)Client)->Send_Packet(&ManufactureItemCatagoryPacketClass(Refinery_Player_Id(sclient), 4));
	if (State == UI_CLOSED) {
		InterfaceSortScopeClass scope(this);
		State = UI_OPEN;
		Unknown0AFC = sclient->PDAInterface->Unknown_695780(false);
		Unknown_5ED230();
		Unknown_BE67C4->Add(InterfaceMesh->Peek_Mesh_Object());
		char buffer[256];
		for (int i = 0; i < 10; i++) {
			_snprintf(buffer, sizeof(buffer), "desc%02d_TSM03a", i);
			Unknown0F4[i] = Create_Peek(Unknown_BE67C4->Get_Style(buffer, InterfaceMesh->Peek_Mesh_Object()));
			Unknown0F4[i]->Set_Sort_Level(Unknown0CC[i]->RObj->Get_Sort_Level() - 1);
			Unknown0F4[i]->Change_Text("", 0);
		}
		Unknown0AC->Unknown_69DC40();
		Unknown0B4->Unknown_58();
		Unknown0B8->Unknown_58();
		Unknown5BB320Class::Unknown_58();
		Unknown_5EDCC0();
		ClientObjectClass *target = Client->As_Space_Client()->Unknown_74BF80();
		if (target) {
			Unknown0AEC = (DeltaCompoundClass *)target->AuxData;
		}
		Unknown_5ED440();
		Unknown_5ECEF0();
		Unknown_5ECE20();
		sclient->Unknown_744560(0x1D);
	}
}

// @0x5EC570
void UIRefineryClass::Unknown_5C()
{
	State = UI_CLOSED;
	Unknown0AC->Unknown_69DC60();
	SClient *sclient = Client->As_Space_Client();
	((ClientClass *)Client)->Send_Packet(&ManufactureActionPacketClass(Refinery_Player_Id(sclient), 0));
	((ClientClass *)Client)->Send_Packet(&ManufactureItemCatagoryPacketClass(Refinery_Player_Id(sclient), 0));
	Unknown0B4->Unknown_5C();
	Unknown0B8->Unknown_5C();
	if (Unknown0AFC) {
		sclient->PDAInterface->Unknown_695A00(0, 1);
	}
	Unknown0AEC = 0;
	Unknown_BE67C4->Remove(InterfaceMesh->Peek_Mesh_Object());
	Unknown5BB320Class::Unknown_5C();
	sclient->Unknown_744560(0x1E);
}

// ui_manu.cpp's aux data classes, as this file reads them (the same classes: their globals are defined there)
class UIManuEntryClass : public DeltaCompoundClass	// NAME 1: neutral (D10): as ui_manu.cpp's
{
public:
	char Unknown09C[0x120 - 0x9C];						// +0x09C
	const char *Unknown120;								// +0x120: the name ("" ends the list)
	char Unknown124[0x1A8 - 0x124];						// +0x124
	int Unknown1A8;										// +0x1A8: the item id (0x5ED530's)
};
class UIManuListClass : public DeltaCompoundClass	// NAME 1: neutral (D10): as ui_manu.cpp's ('KnownFormula')
{
public:
	static int UnknownGlobal;							// the per-class global at +0x04 (0xC41628)
};
class UIManuComponentsClass : public DeltaCompoundClass	// NAME 1: neutral (D10): as ui_manu.cpp's ('ManufactureComponents', 'ManufactureTarget')
{
public:
	static int UnknownGlobal;							// the per-class global at +0x04
};
class UIManuTechLevelFilterClass : public DeltaClass	// NAME 1: neutral (D10): as ui_manu.cpp's (here 'AdditionalIterations')
{
public:
	static int UnknownGlobal;							// the per-class global at +0x04 (0xC4163C)
	int Value;											// +0x84  // NAME 2: what it holds
};
#include "aux_roots.h"				// (after auxdata.h, as ui_manu.cpp): 'Validity' is DeltaTemplateClass<UnknownB01394Enum>

template<class C> inline C *Unknown_Find_Valid(DeltaCompoundClass *root, const NameClass &name) { C *t = Unknown_Find_Field<C>(root, name); if (t && t->Unknown70) return(t); return(NULL); }	// NAME 1: neutral (D10): as ui_manu.cpp's
template<class C> inline C *Unknown_Manu_Raw_Field(DeltaCompoundClass *const &root, const char *name) { NameClass n(name); DeltaCompoundClass *r = root; return(Unknown_Find_Field<C>(r, n)); } template<class C> inline C *Unknown_Manu_Raw(DeltaCompoundClass *const &root, const char *name) { return(Unknown_Manu_Raw_Field<C>(root, name)); }	// NAME 1: neutral (D10): as ui_manu.cpp's
template<class C> C *Unknown_Find_Changed(DeltaCompoundClass *root, const NameClass &name) { C *t = Unknown_Find_Field<C>(root, name); if (t && t->Unknown70 > 1) return(t); return(NULL); }	// NAME 1: neutral (D10): 0x5EEBA0 (out of line after 0x5EDFD0)

// @0x5EC6D0
void UIRefineryClass::Unknown_60()
{
	Unknown_Find_Field<DeltaTemplateClass<int> >(NULL, *(NameClass *)this); Unknown_Find_Field<DeltaTemplateClass<int> >(NULL, *(NameClass *)this); Unknown_Find_Field<DeltaTemplateClass<int> >(NULL, *(NameClass *)this); Unknown_Find_Field<DeltaTemplateClass<int> >(NULL, *(NameClass *)this); Unknown_Find_Field<DeltaTemplateClass<int> >(NULL, *(NameClass *)this);	// not the client's (D130): dead finds spending the inline budget the client spent (both Casts out of line, 0x5B7210 / 0x5468D0; 4 are not enough)
	Unknown_5EDFD0();
	if (Unknown0AF4 != 0.0f) {
		Unknown0AF4 -= (float)(__int64)(WW3D::Get_Frame_Time()) * 0.001f;
		if (Unknown0AF4 >= 0.0f) {
			goto slider;
		}
		Unknown0AF4 = 0;
		Unknown_5ECBB0(4, true, Unknown_B85C48[5]);
	} else if (Unknown0AF0 != 0) {
		if (Unknown0AF0 == 1) {
			NameClass name("ManufactureComponents");
			UIManuComponentsClass *components = Unknown_Find_Field<UIManuComponentsClass>(Unknown0AEC, name);
			if (components && components->Unknown70 > 1) {
				Unknown0AF0 = 0;
				Unknown_5ECEF0();
				if (!Unknown0AFD) {
					InterfaceMesh->Unknown_5B8DF0(0, "tsm03a.tsm03a-1", false, 0, 0, 0, -1);
					Unknown0AFD = true;
				}
			}
		}
		if (Unknown0AF0 == 3) {
			NameClass name("Validity");
			DeltaTemplateClass<UnknownB01394Enum> *found = Unknown_Find_Field<DeltaTemplateClass<UnknownB01394Enum> >(Unknown0AEC, name); DeltaTemplateClass<UnknownB01394Enum> *validity = (found && found->Unknown70) ? found : NULL;
			int value = validity->Value;
			if (Is_Validity_Attempt((UnknownB01394Enum)value)) {
				Unknown0AF0 = 0;
				Unknown_5ECBB0(4, true, Unknown_B85C60[1]);
				SoundLibraryClass::Play_2D_Sound("success_1.wav", 2);
				Unknown_5ED530(Unknown0B00);
				goto refresh;
			}
		}
		goto slider;
	} else if (((DeltaClass *)Unknown0AEC)->Unknown70 <= 1) {
		goto slider;
	}
refresh:
	Unknown_5ECEF0();
slider:
	Unknown_5ED310();
	Unknown5BB320Class::Unknown_60();
}

// @0x5EC990
bool UIRefineryClass::Unknown_68(int key)
{
	Unknown_Find_Field<DeltaTemplateClass<int> >(NULL, *(NameClass *)this); Unknown_Find_Field<DeltaTemplateClass<int> >(NULL, *(NameClass *)this); Unknown_Find_Field<DeltaTemplateClass<int> >(NULL, *(NameClass *)this); Unknown_Find_Field<DeltaTemplateClass<int> >(NULL, *(NameClass *)this);	// not the client's (D130): dead finds spending the inline budget the client spent (its inlined 0x5ECCE0 calls the Cast out of line; 3 are not enough)
	if (Unknown0AF4 != 0.0f) {
		return(true);
	}
	Unknown0B05 = false;
	switch (key) {
		case 1:
			SoundLibraryClass::Play_2D_Sound("Roll_Select02b.wav", 2);
			Unknown0AC->Unknown_69F440();
			return(true);
		case 5:
		case 6:
		case 7:
		case 8:
		case 9:
		case 10:
		case 11:
		case 12:
		case 13:
		case 14:
			SoundLibraryClass::Play_2D_Sound("fontclick.wav", 2);
			Unknown_5ECCE0(key - 5);
			return(true);
		case 2:
			SoundLibraryClass::Play_2D_Sound("manufacture_1.wav", 2);
			Unknown0B05 = true;
			Unknown_5ED5F0();
			return(true);
		case 3:
			SoundLibraryClass::Play_2D_Sound("manufacture_1.wav", 2);
			Unknown0B05 = true;
			Unknown_5ED800();
			return(true);
	}
	return(false);
}






























































// @0x5ECBB0
void UIRefineryClass::Unknown_5ECBB0(int index, bool show, const char *text)
{
	const char *name = Unknown_B85C48[index]; RefCountPtr<FontSys> font = Create_Peek(Unknown_BE67C4->Get_Style(name, InterfaceMesh->Peek_Mesh_Object()));
	FAIL_IF(!font) return;
	font->Change_Text(text, 0);
	font->Set_Hidden(!show);
}

// @0x5ECCE0
void UIRefineryClass::Unknown_5ECCE0(int row)
{
	if (Unknown0AE4 != 0 && row < Unknown0AE4) {
		Unknown0AF8 = Unknown0AE0 + row;
		Unknown_5ED470();
		Unknown_5ECE20();
		Unknown_5ECEF0();
	}
}

// @0x5ECDD0
void UIRefineryClass::Unknown_5ECDD0(int row)
{
	if (Unknown0AF8 == Unknown0AE0 + row) {
		Unknown0F4[row]->Unknown_6CE910(true);
	} else {
		Unknown0F4[row]->Unknown_6CE910(false);
	}
}

// @0x5ECE20
void UIRefineryClass::Unknown_5ECE20()
{
	if (Unknown0AEC) {
		for (int i = 0; i < 10; i++) {
			char text[64];
			int index = Unknown0AE0 + i;
			if (index < Unknown0AE4 && Unknown11C[index]) {
				Bounded_Strcpy(text, sizeof(text), Unknown11C[index]);
			} else {
				text[0] = '\0';
			}
			Unknown0F4[i]->Change_Text(text, 0);
			Unknown0F4[i]->Unknown4E4 = true;
			Unknown_5ECDD0(i);
		}
	}
}

// NOT EXACT: register (the 'AdditionalIterations' pointer kept in ecx and its local, the two flags' tests in another order; 0x5465C0 / 0x5B7130 out of line as the client) (0x5ECEF0)
void UIRefineryClass::Unknown_5ECEF0()
{
	RefCountPtr<FontSys> go;
	go = Create_Peek(Unknown_BE67C4->Get_Style(Unknown_B85C48[1], InterfaceMesh->Peek_Mesh_Object()));
	RefCountPtr<FontSys> stack = Create_Peek(Unknown_BE67C4->Get_Style(Unknown_B85C48[2], InterfaceMesh->Peek_Mesh_Object()));
	DeltaTemplateClass<UnknownB01394Enum> *v = Unknown_Manu_Raw<DeltaTemplateClass<UnknownB01394Enum> >(Unknown0AEC, "Validity");
	DeltaTemplateClass<UnknownB01394Enum> *validity = (v && v->Unknown70) ? v : NULL;
	UIManuTechLevelFilterClass *n = Unknown_Manu_Raw<UIManuTechLevelFilterClass>(Unknown0AEC, "AdditionalIterations");
	UIManuTechLevelFilterClass *iterations = (n && n->Unknown70) ? n : NULL;
	bool can_go;
	bool can_stack;
	if (validity && validity->Value == 13) {
		can_go = true;
		can_stack = iterations && (unsigned int)iterations->Value > 0;
	} else {
		can_go = false;
		can_stack = false;
	}
	if (Unknown0AF0 != 0) {
		can_stack = false;
	}
	if (Unknown0AF0 == 0 && can_go) {
		Unknown0BC[2]->Unknown_40(true);
		go->Unknown_Set_2FC(1.0f);
	} else {
		Unknown0BC[2]->Unknown_40(false);
		Unknown0BC[2]->Unknown_40(false);
		go->Unknown_Set_2FC(0.33f);
	}
	if (can_stack) {
		Unknown0BC[3]->Unknown_40(true);
		stack->Unknown_Set_2FC(1.0f);
		char text[52];
		sprintf(text, "Refine %d", iterations->Value + 1);
		stack->Change_Text(text, 0);
	} else {
		Unknown0BC[3]->Unknown_40(false);
		stack->Unknown_Set_2FC(0.33f);
		stack->Change_Text("Refine Stack", 0);
	}
	Unknown0BC[1]->Unknown_40(true);
}

// @0x5ED230
void UIRefineryClass::Unknown_5ED230()
{
	Unknown0AF8 = -1;
	Unknown0B0->Unknown_66FA90(0, 10.0f, true);
	Unknown_5ED310();
}

// @0x5ED2A0
void UIRefineryClass::Unknown_5ED2A0(int top)
{
	Unknown0B0->Unknown_66FA90((float)top, 10.0f, true);
	Unknown_5ED310();
}

// @0x5ED310
void UIRefineryClass::Unknown_5ED310()
{
	int top = (int)Unknown0B0->Get_Value();
	if (Unknown0AE0 != top) {
		Unknown0AE0 = top;
		Unknown_5ECE20();
	}
}

// @0x5ED350
void UIRefineryClass::Unknown_5ED350()
{
	GadgetSlider *slider = Unknown0B0;
	int top = (int)slider->Get_Value();
	int bottom = top + 9;
	int pick = Unknown0AF8;
	if (pick < top) {
		slider->Unknown_A4((float)pick, 0);
		Unknown_5ED310();
	} else if (pick > bottom) {
		slider->Unknown_A4((float)(pick - bottom + top), 0);
		Unknown_5ED310();
	}
}

// @0x5ED440
void UIRefineryClass::Unknown_5ED440()
{
	Unknown0B04 = false;
	Unknown0AF0 = 2;
}

// @0x5ED470
void UIRefineryClass::Unknown_5ED470()
{
	int pick = Unknown0AF8;
	if (pick >= 0) {
		NameClass name("KnownFormula");
		UIManuListClass *list = Unknown_Find_Field<UIManuListClass>(Unknown0AEC, name);
		if (list && list->Unknown70) {
			Unknown_5ED530(((UIManuEntryClass *)list->Children[pick])->Unknown1A8);
		}
	}
}

// @0x5ED530
void UIRefineryClass::Unknown_5ED530(int item_id)
{
	if (item_id != -1) {
		((ClientClass *)Client)->Send_Packet(&RefineryItemIdPacketClass(Refinery_Player_Id(Client->As_Space_Client()), item_id));
		Unknown0AF0 = 1;
		Unknown0B00 = item_id;
	}
}

// @0x5ED5F0
void UIRefineryClass::Unknown_5ED5F0()
{
	int pick = Unknown0AF8;
	if (pick >= 0) {
		((ClientClass *)Client)->Send_Packet(&ManufactureActionPacketClass(Refinery_Player_Id(Client->As_Space_Client()), 2));
		Unknown0AF0 = 3;
		Unknown0BC[2]->Unknown_40(false);
		Unknown0BC[3]->Unknown_40(false);
		RefCountPtr<FontSys> font = Create_Peek(Unknown_BE67C4->Get_Style(Unknown_B85C48[1], InterfaceMesh->Peek_Mesh_Object()));
		font->Unknown_Set_2FC(0.33f);
		font = Create_Peek(Unknown_BE67C4->Get_Style(Unknown_B85C48[2], InterfaceMesh->Peek_Mesh_Object()));
		font->Unknown_Set_2FC(0.33f);
		font->Change_Text("Refine Stack", 0);
	}
}

// @0x5ED800
void UIRefineryClass::Unknown_5ED800()
{
	int pick = Unknown0AF8;
	if (pick >= 0) {
		((ClientClass *)Client)->Send_Packet(&ManufactureActionPacketClass(Refinery_Player_Id(Client->As_Space_Client()), 3));
		Unknown0AF0 = 3;
		Unknown0BC[2]->Unknown_40(false);
		Unknown0BC[3]->Unknown_40(false);
		RefCountPtr<FontSys> font = Create_Peek(Unknown_BE67C4->Get_Style(Unknown_B85C48[1], InterfaceMesh->Peek_Mesh_Object()));
		font->Unknown_Set_2FC(0.33f);
		font = Create_Peek(Unknown_BE67C4->Get_Style(Unknown_B85C48[2], InterfaceMesh->Peek_Mesh_Object()));
		font->Unknown_Set_2FC(0.33f);
		font->Change_Text("Refine Stack", 0);
	}
}

// @0x5EDA10
void UIRefineryClass::Unknown_5EDA10(UIManuListClass *list)
{
	int i = 0;
	int pick = -1;
	if (list) {
		Unknown0AE4 = list->Children.size();
		for (i = 0; i < Unknown0AE4; i++) {
			UIManuEntryClass *entry = (UIManuEntryClass *)list->Children[i];
			const char *name = entry->Unknown120;
			if (strcmp(name, "") == 0) {
				Unknown0AE4 = i;
				break;
			}
			Unknown11C[i] = name;
			Unknown8EC[i] = false;
			if (Unknown0B00 != -1 && Unknown0B00 == entry->Unknown1A8) {
				pick = i;
			}
		}
	} else {
		Unknown0AE4 = i;
	}
	for (i = Unknown0AE4; i < 500; i++) {
		Unknown11C[i] = NULL;
		Unknown8EC[i] = true;
	}
	Unknown0AF8 = pick;
	Unknown_5ED2A0(Unknown0AE4);
	Unknown_5ED350();
	Unknown0B04 = (list != NULL);
}

// NOT EXACT: inline budget (the client calls the 'NegotiatedCost' Cast out of line; the 'Money' find out of line as the client; dead finds before it push the 0x5ECBB0 copies out too) (0x5EDCC0)
void UIRefineryClass::Unknown_5EDCC0()
{
	MoneyClass credits;
	DeltaTemplateClass<MoneyClass> *money = Unknown_Manu_Raw<DeltaTemplateClass<MoneyClass> >((DeltaCompoundClass *)Client->As_Space_Client()->AuxData, "Money");
	if (money && money->Unknown70) {
		credits = money->Value;
	}
	Unknown_5ECBB0(0, true, credits.Amount_As_Text(false).c_str());
	MoneyClass cost;
	DeltaTemplateClass<MoneyClass> *negotiated = Unknown_Find_Field<DeltaTemplateClass<MoneyClass> >(Unknown0AEC, NameClass("NegotiatedCost"));
	if (negotiated && negotiated->Unknown70) {
		cost = negotiated->Value;
	}
	Unknown_5ECBB0(3, true, cost.Amount_As_Text(false).c_str());
}

static std::string Unknown_5EE7F0(int validity);
static std::string Unknown_5EE460(DeltaCompoundClass *auxdata, LineDisplayClientClass *client);

// NOT EXACT: inline budget (the client calls all three finds' Unknown_Find_Field out of line, 0x5465C0 / 0x417D82, with the name in the caller; ours calls the inline layer out of line, or inlines the find; frame 0xBC vs 0xEC) (0x5EDFD0)
void UIRefineryClass::Unknown_5EDFD0()
{
	DeltaTemplateClass<UnknownB01394Enum> *validity = Unknown_Manu_Raw<DeltaTemplateClass<UnknownB01394Enum> >(Unknown0AEC, "Validity");
	if (validity && validity->Unknown70 > 1) {
		std::string reason = Unknown_5EE7F0(validity->Value);
		if (reason.length()) {
			if (Unknown0B05) {
				Unknown0AF8 = -1;
			} else {
				std::string item = Unknown_5EE460(Unknown0AEC, Client);
				std::string message = "Unable to create " + item + std::string(". ") + reason + ".";
				Client->As_Space_Client()->Unknown_74D990(0x11, (char *)message.c_str(), 0);
			}
		}
	}

	MoneyClass credits;
	DeltaTemplateClass<MoneyClass> *money = Unknown_Manu_Raw<DeltaTemplateClass<MoneyClass> >((DeltaCompoundClass *)Client->As_Space_Client()->AuxData, "Money");
	if (money && money->Unknown70 > 1) {
		credits = money->Value;
		Unknown_5ECBB0(0, true, credits.Amount_As_Text(false).c_str());
	}
	MoneyClass cost;
	DeltaTemplateClass<MoneyClass> *negotiated = Unknown_Manu_Raw<DeltaTemplateClass<MoneyClass> >(Unknown0AEC, "NegotiatedCost");
	if (negotiated && negotiated->Unknown70 > 1) {
		cost = negotiated->Value;
		Unknown_5ECBB0(3, true, cost.Amount_As_Text(false).c_str());
	}
	UIManuListClass *list = Unknown_Find_Changed<UIManuListClass>(Unknown0AEC, NameClass("KnownFormula"));
	if (list) {
		Unknown_5EDA10(list);
		Unknown_5ECE20();
		if (Unknown0AF0 == 0) {
			Unknown_5ECEF0();
		}
	}
}

class UIManuItemTemplateClass { public: char Unknown00[0x14]; std::string Name; }; struct UIManuTargetItemClass { char Unknown00[0x120]; int Unknown120; };	// NAME 1: neutral (D10): as ui_manu.cpp's
UIManuItemTemplateClass *Unknown_8318B0(int item_template_id, LineDisplayClientClass *client);	// NAME 1: neutral (D10)  // NAME 3 [item_template_id]: the FAIL_IF's









// NOT EXACT: register (ebx / ebp swapped and the static's _Eos inlined, as ui_manu.cpp's 0x5B5CD0) (0x5EE460)
static std::string Unknown_5EE460(DeltaCompoundClass *auxdata, LineDisplayClientClass *client)	// NAME 1: neutral (D10)  // NAME 3 [auxdata, client]: the FAIL_IFs'
{
	static std::string unknown_item("Unknown Item");
	FAIL_IF(!auxdata) return(unknown_item);
	FAIL_IF(!client) return(unknown_item);
	NameClass name("ManufactureTarget");
	UIManuComponentsClass *target_item_auxdata_set = Unknown_Find_Valid<UIManuComponentsClass>(auxdata, name);
	FAIL_IF(!target_item_auxdata_set) return(unknown_item);



	int item_template_id = (*(UIManuTargetItemClass ***)((char *)target_item_auxdata_set + 0x88))[0]->Unknown120;
	FAIL_IF(item_template_id == -1) return(unknown_item);

	UIManuItemTemplateClass *item_template = Unknown_8318B0(item_template_id, client);
	FAIL_IF(!item_template) return(unknown_item);
	return(item_template->Name);
}

// 0x5EE7F0
static std::string Unknown_5EE7F0(int validity)	// NAME 1: neutral (D10)  // NAME 1 [validity]: the 'Validity' value
{
	switch (validity) {
		case 4:
			return("Insufficient credits to refine item.");
		case 6:
			return("More empty cargo space required to refine item.");
		case 12:
			return("You cannot refine an unique resource you already possess.");
		case 5:
			return("Missing required component(s)");
		case 8:
			return("Item too difficult to refine.");
		case 7:
			return("Not qualified to refine item.");
		case 0:
		case 13:
		case 14:
		case 15:
		case 16:
		case 17:
			return("");
		default:
			return("Unexpected error.");
	}
	Unknown_Find_Field<DeltaTemplateClass<int> >(NULL, *(NameClass *)&validity);	// not the client's: a dead find (as ui_manu.cpp's 0x5B6060, D130) spends the inline budget the client spent
}

#include "..\shared\auxdata\permstr.h"	// (its PermStringInitClass static: 0xBE5D89, initialised by 0x5EB7C0)

/*
** Notes (RF1, 2026-11-09). The client's refinery window: ui_manu.cpp's manufacturing window cut down to one list (the
** known formulas of the target item's aux data), the go and stack buttons and two ui_inv.cpp sub-windows. Function order
** follows the client's: the constructor 0x5EB810, deleting destructor 0x5EBA20, destructor 0x5EBA50, slots +0x54
** 0x5EBC30, +0x7C 0x5EC220, +0x58 0x5EC270, +0x5C 0x5EC570, +0x60 0x5EC6D0, +0x68 0x5EC990, then the helpers to 0x5EDFD0,
** its static helpers 0x5EE460 / 0x5EE7F0 and the find 0x5EEBA0. The PermStringInitClass static 0x5EB7C0 (its atexit
** 0x5EB7F0) comes from permstr.h. 0x5EB7B0 (a this-adjusting thunk to 0x5EAB90) belongs to the file before.
*/
