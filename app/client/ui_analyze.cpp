// Earth & Beyond client (reconstruction): app\Client\ui_analyze.cpp. Notes at the end of the file.
#include <stdio.h>
#include <string.h>
#include "sclient.h"
#include "client.h"
#include "gadget.h"
#include "unknown_675120.h"
#include "layer.h"
#include "rendobj.h"
#include "assetmgr.h"
#include "texture.h"
#include "textureloader.h"
#include "..\..\wwlib\ref_ptr.h"
#include "..\packets\pkmanufactureitemcatagorypacket.h"
#include "..\shared\g_assert.h"
#include "..\shared\auxdata\auxdata.h"
#include "aux_roots.h"
#include "sbaseview.h"
#include "local_interface_sort.h"
#include "..\audio\soundlib.h"
#include "ui_analyze.h"
#include "..\..\ww3d2\ww3d.h"	// WW3D::Get_Frame_Time (read inline, as ui_manu.cpp's)

class SBaseViewClass;

// The client at InterfaceClass +0x40, as this file calls it (as ui_manu.cpp declares it)
class LineDisplayClientClass  // NAME 1: neutral (D10)
{
public:
	virtual void Unknown_00(); virtual void Unknown_04(); virtual void Unknown_08(); virtual void Unknown_0C();
	virtual void Unknown_10(); virtual void Unknown_14();
	virtual SClient *As_Space_Client();					// +0x18 (ClientClass::Unknown_18; SClient's returns itself)  // NAME 3: ui_manu.cpp's FAIL_IF's
};

bool Unknown_495FD0(RenderObjClass *robj, TextureClass *texture, int unknown);	// 0x495FD0 (ro_funcs.cpp, cdecl): the texture onto the model  // NAME 1 [robj, texture, unknown]: as ui_manu.cpp's

#include "fontmanager.h"	// FontSys, FontManagerClass (0xBE67C4) and fontdatabase.h's FontStringFilesClass (0xBE67BC)

extern FontManagerClass *Unknown_BE67C4;	// NAME 1: neutral (D10)

class UIAnalyzePDAClass { public: void Unknown_695A00(int a, int b); bool Unknown_695780(bool arg); };	// NAME 1: neutral (D10): PDAInterfaceClass's 0x695780 / 0x695A00, as ui_manu.cpp's  // NAME 1 [a, b, arg]: neutral
class UIAnalyzeSClientClass { public: void Unknown_744560(int arg); };	// NAME 1: neutral (D10): SClient's 0x744560 (slot +0x58 passes 0x19, slot +0x5C 0x1A)  // NAME 1 [arg]: neutral

// The window's buttons (0xB79664: slot +0x54's ids 1..6; [1..3] the category columns, their icons '%sf')
static const char *Unknown_B79664[] = { "exit", "item00", "cat00", "sub00", "go", "inv" };	// NAME 1: neutral (D10)
// The names of the window's texts (0xB7967C)
static const char *Unknown_B7967C[] = { "Credits_tsm02a", "Go_tsm02a", "inv_tsm02a", "Cost_tsm02a", "Chance_tsm02a", "Critical_tsm02a", "Attempt_tsm02a", "Result_tsm02a", "Succ_tsm02a", "Crit_tsm02a", "" };	// NAME 1: neutral (D10)
// The texts shown (0xB796A8)
static const char *Unknown_B796A8[] = { "Analyzing Target", "Dismantling Target", "Critical Success!", "Attempt Succeeded", "Attempt Failed.  Item Damaged", "Attempt Failed", "Formula Added", "Target Dismantled", "Analyze", "Dismantle", "Retry", "Success:", "Critical:", "Recover One:", "Recover All:" };	// NAME 1: neutral (D10)
static const float Unknown_AEF0EC = 2.0f;			// NAME 1: neutral (D10): what 0x546090 stores (read from .rdata)

// The control keys event slot +0x54 registers (0x10 bytes) and what InterfaceClass +0x4C is (as ui_manu.cpp's)
struct UIAnalyzeKeyEventStruct	// NAME 1: neutral (D10): kernel.h's input event
{
	UIAnalyzeKeyEventStruct(short key) : Key(key), Unknown02(0), Unknown04(0), Unknown08(0), Unknown0C(0) {}	// NAME 1 [key]: neutral
	short Key;											// +0x00 (0x1B)  // NAME 1: neutral (D10)
	short Unknown02;									// +0x02
	int Unknown04;										// +0x04
	int Unknown08;										// +0x08
	int Unknown0C;										// +0x0C
};
class UIAnalyzeKeysClass { public: void Unknown_721E60(const UIAnalyzeKeyEventStruct *event, bool on); };	// NAME 1: neutral (D10): InterfaceClass +0x4C's 0x721E60  // NAME 1 [event, on]: what slot +0x54 passes
void Unknown_496AE0(RenderObjClass *robj, bool make_geometry_unique);	// 0x496AE0 (ro_funcs.cpp, cdecl)  // NAME 1 [robj, make_geometry_unique]: as ui_manu.cpp's

// The item's fields this file reads (as ui_manu.cpp's)
// 'Validity': DeltaManufacturingLabClass's DeltaTemplateClass<UnknownB01394Enum> (aux_roots.h; its per-class global 0xC41640)

template<class C> static inline C *Unknown_Find_Root_Field(DeltaCompoundClass *root, NameClass &name) { if (!root) return(NULL); C *t = Unknown_Find_Field<C>(root, name); if (!t) return(NULL); return(t); }	// NAME 1: neutral (D10): the find with its own root test (0x546090 calls the Cast out of line inside it, as the client)  // NAME 1 [root, name]: neutral
// The field if it is set, else NULL, through two inline layers: with them the client's finds 0x546780 / 0x5467F0 stay out of line in 0x545E40 (the root copied after the NameClass: the client's registers; IB1)
template<class C> inline C *Unknown_Find_Valid_Field(DeltaCompoundClass *const &root, const char *name) { NameClass n(name); DeltaCompoundClass *r = root; C *t = Unknown_Find_Field<C>(r, n); if (t && t->Unknown70) return(t); return(NULL); }	// NAME 1: neutral (D10)  // NAME 1 [root, name, n, r, t]: neutral
template<class C> inline C *Unknown_Find_Valid(DeltaCompoundClass *const &root, const char *name) { return(Unknown_Find_Valid_Field<C>(root, name)); }	// NAME 1: neutral (D10)  // NAME 1 [root, name]: neutral

// 'Mode': DeltaManufacturingLabClass's DeltaTemplateClass<UnknownB013ECEnum> (aux_roots.h; its per-class global 0xC41634)







// @0x5435D0
UIAnalyzeClass::UIAnalyzeClass(SBaseViewClass *view, const LayerClass &layer) :
	Unknown5BB320Class(new UIMeshClass("TSM02A", true), (UIViewClass *)view, layer),
	Unknown0AC(layer),
	Unknown0D0(view),
	State(UI_CLOSED),
	Unknown10C(0),
	Unknown110(false),
	Unknown114(0),
	Unknown118(false)
{
	memset(Unknown0E0, 0, sizeof(Unknown0E0));
	memset(Unknown0FC, 0, sizeof(Unknown0FC));
	Unknown_BE67BC->Load("FA_TSM02A.INI");
}
















// NOT EXACT: a relocation target (explain.py: identical once stack offsets, targets and relocations are masked; the unwind funclets look the same) (0x5437C0)
// @0x5437C0
UIAnalyzeClass::~UIAnalyzeClass()
{
	G_ASSERT(State == UI_CLOSED);
	for (int i = 0; i < 3; i++) {
		Unknown0FC[i] = NULL;
	}
	Unknown0D4 = NULL;
	Unknown0D8 = NULL;
	Unknown0DC = NULL;
	Unknown_BE67BC->Unload("FA_TSM02A.INI");
}

// NOT EXACT: register (after the button loop the client keeps the zero in ebp and the space client in ebx, ours the other way round, and loads the name table's pointer before the gadget pointer; 0-based loop, named key event, the icon loop 1-based, the buffer at the top: no) (0x5439F0)
// @0x5439F0
void UIAnalyzeClass::Unknown_54()
{
	for (int id = 1; id < 7; id++) {
		RefCountPtr<RenderObjClass> robj = Create_Get(InterfaceMesh->Peek_Object()->Get_Sub_Object_By_Name(Unknown_B79664[id - 1]));
		Unknown_496AE0(robj.Peek(), 0);
		Unknown0E0[id] = new ButtonGadgetClass(robj.Peek(), Unknown0AC, 0x55, 3);
		Unknown64.Unknown_6870E0(Unknown0E0[id], id, View, 1, 0);
		Unknown_5BB4F0(Unknown0E0[id]);
	}
	GadgetClass::Set_Tooltip(Unknown0E0[5], "UI_ANALYZE", "ICON_GO");
	GadgetClass::Set_Tooltip(Unknown0E0[6], "UI_ANALYZE", "ICON_INV");
	((UIAnalyzeKeysClass *)Unknown4C)->Unknown_721E60(&UIAnalyzeKeyEventStruct(0x1B), true);
	SClient *sclient = Client->As_Space_Client();
	Unknown0D4 = Create_Get(new UIAnalyzeInvClass(sclient, InterfaceMesh->Peek_Object(), 6, "TSM02A.E%02d", 12));
	Unknown0D4->Unknown_54();
	Unknown0D4->Unknown_7C(View, 0);
	Unknown0D8 = Create_Get(new UIAnalyzeInvClass(sclient, InterfaceMesh->Peek_Object(), 1, "TSM02A.RESULT", 13));
	Unknown0D8->Unknown_54();
	Unknown0D8->Unknown_7C(View, 0);
	Unknown0DC = Create_Get(new UIAnalyzeInvClass(sclient, InterfaceMesh->Peek_Object(), 1, "TSM02A.OVER", 14));
	Unknown0DC->Unknown_54();
	Unknown0DC->Unknown_7C(View, 0);
	char name[64];
	for (int i = 0; i < 3; i++) {
		sprintf(name, "%sf", Unknown_B79664[i + 1]);
		Unknown0FC[i] = Create_Get(InterfaceMesh->Peek_Object()->Get_Sub_Object_By_Name(name));
		Unknown0FC[i]->Set_Hidden(true);
	}
	RefCountPtr<RenderObjClass> tutorial = Create_Get(InterfaceMesh->Peek_Object()->Get_Sub_Object_By_Name("tutorial"));
	if (tutorial) {
		tutorial->Set_Hidden(true);
	}
	Unknown_10();
}

static inline int Analyze_Player_Id(SClient *sclient) { return sclient->Unknown1138; }  // NAME 2: the player id read as an inline accessor (as ui_manu.cpp's Manu_Player_Id)

// @0x544020
void UIAnalyzeClass::Unknown_7C(UIViewClass *view, bool arg)
{
	InterfaceClass::Unknown_7C(view, arg);
	if (Unknown0D4) {
		Unknown0D4->Unknown_7C(view, arg);
	}
	if (Unknown0D8) {
		Unknown0D8->Unknown_7C(view, arg);
	}
	if (Unknown0DC) {
		Unknown0DC->Unknown_7C(view, arg);
	}
}

// @0x544090
void UIAnalyzeClass::Unknown_58()
{
	InterfaceSortScopeClass scope(this);
	SClient *sclient = Client->As_Space_Client();
	((ClientClass *)Client)->Send_Packet(&ManufactureItemCatagoryPacketClass(Analyze_Player_Id(sclient), 2));
	if (State == UI_CLOSED) {
		State = UI_OPEN;
		Unknown118 = ((UIAnalyzePDAClass *)sclient->PDAInterface)->Unknown_695780(false);
		Unknown_BE67C4->Add(InterfaceMesh->Peek_Mesh_Object());
		Unknown_545610();
		Unknown0D0->Unknown_69DC40();
		Unknown0D4->Unknown_58();
		Unknown0D8->Unknown_58();
		Unknown0DC->Unknown_58();
		Unknown5BB320Class::Unknown_58();
		Unknown_544D80();
		ClientObjectClass *target = Client->As_Space_Client()->Unknown_74BF80();
		if (target) {
			Unknown10C = (DeltaCompoundClass *)target->AuxData;
		}
		Unknown0E0[2]->Unknown_40(false);
		Unknown0E0[3]->Unknown_40(false);
		Unknown0E0[4]->Unknown_40(false);
		((UIAnalyzeSClientClass *)sclient)->Unknown_744560(0x19);
	}
}

// @0x544270
void UIAnalyzeClass::Unknown_5C()
{
	if (State == UI_OPEN) {
		State = UI_CLOSED;
		((ClientClass *)Client)->Send_Packet(&ManufactureActionPacketClass(Analyze_Player_Id(Client->As_Space_Client()), 0));
		Unknown_546000();
		SClient *sclient = Client->As_Space_Client();
		((ClientClass *)Client)->Send_Packet(&ManufactureItemCatagoryPacketClass(Analyze_Player_Id(sclient), 0));
		Unknown0D0->Unknown_69DC60();
		Unknown0D4->Unknown_5C();
		Unknown0D8->Unknown_5C();
		Unknown0DC->Unknown_5C();
		if (Unknown118) {
			((UIAnalyzePDAClass *)sclient->PDAInterface)->Unknown_695A00(0, 1);
		}
		Unknown_BE67C4->Remove(InterfaceMesh->Peek_Mesh_Object());
		Unknown5BB320Class::Unknown_5C();
		((UIAnalyzeSClientClass *)sclient)->Unknown_744560(0x1A);
	}
}





















































































// NOT EXACT: inline budget (the client calls the first 'Validity' find out of line, 0x5465C0, and the 'Validity' / 'Mode' Casts in the second and third, 0x5468D0 / 0x546920, but inlines the last find whole; its blocks also come in another order) (0x5443F0)
// @0x5443F0
void UIAnalyzeClass::Unknown_60()
{
	Unknown_545190();
	if (Unknown114 != 0.0f) {
		Unknown114 -= (float)(__int64)(WW3D::Get_Frame_Time()) * 0.001f;
		if (Unknown114 < 0.0f) {
			Unknown114 = 0;
			Unknown_544C50(6, true, Unknown_B7967C[10]);
			Unknown_544C50(7, true, Unknown_B7967C[10]);
			if (!Unknown110) {
				if (Unknown0D8) {
					Unknown0D8->Unknown_62F5B0(true);
				}
				if (Unknown0DC) {
					Unknown0DC->Unknown_62F5B0(true);
				}
			}
			DeltaTemplateClass<UnknownB01394Enum> *validity = Unknown_Find_Field<DeltaTemplateClass<UnknownB01394Enum> >(Unknown10C, NameClass("Validity"));
			if (validity && validity->Unknown70 && validity->Value == 15) {
				((ClientClass *)Client)->Send_Packet(&ManufactureActionPacketClass(((SClient *)Client)->Unknown1138, 1));
			}
		}
	} else if (!Unknown110) {
		if (Unknown10C->Unknown70 > 1) {
			Unknown_545610();
		}
	} else {
		DeltaTemplateClass<UnknownB01394Enum> *validity = Unknown_Find_Field<DeltaTemplateClass<UnknownB01394Enum> >(Unknown10C, NameClass("Validity"));
		if (validity && validity->Unknown70 && validity->Value != 13) {
			Unknown110 = false;
			Unknown114 = Unknown_AEF0EC;
			Unknown_545610();
			DeltaTemplateClass<UnknownB013ECEnum> *mode = Unknown_Find_Field<DeltaTemplateClass<UnknownB013ECEnum> >(Unknown10C, NameClass("Mode"));
			if (mode && mode->Unknown70) {
				bool analyze = mode->Value == 2;
				const char *result = NULL;
				const char *text;
				const char *sound;
				bool success;
				switch (validity->Value) {
					case 14:
						result = Unknown_B796A8[5];
						text = "";
						sound = "failure_1.wav";
						success = false;
						break;
					case 15:
						result = Unknown_B796A8[4];
						text = "";
						sound = "failure_1.wav";
						success = false;
						break;
					case 16:
						result = Unknown_B796A8[3];
						text = analyze ? Unknown_B796A8[6] : Unknown_B796A8[7];
						sound = "success_1.wav";
						success = true;
						break;
					case 17:
						result = Unknown_B796A8[2];
						text = analyze ? Unknown_B796A8[6] : Unknown_B796A8[7];
						sound = "success_1.wav";
						success = true;
						break;
					default:
						G_ASSERT(0);
						break;
				}
				if (result) {
					Unknown_544C50(6, true, result);
					Unknown_544C50(7, true, text);
					if (analyze && success) {
						Unknown_545E40();
					}
					SoundLibraryClass::Play_2D_Sound(sound, 2);
				}
			}
		}
	}
	DeltaTemplateClass<UnknownB01394Enum> *validity = Unknown_Find_Field<DeltaTemplateClass<UnknownB01394Enum> >(Unknown10C, NameClass("Validity"));
	if (validity && validity->Unknown70 > 1) {
		Unknown_545610();
		Unknown_546090();
	}
	Unknown5BB320Class::Unknown_60();
}

// @0x544BB0
bool UIAnalyzeClass::Unknown_68(int key)
{
	if (!Unknown110) {
		switch (key) {
			case 1:
				SoundLibraryClass::Play_2D_Sound("Roll_Select02b.wav", 2);
				Unknown0D0->Unknown_69F440();
				return(true);
			case 5:
				SoundLibraryClass::Play_2D_Sound("analyze_1.wav", 2);
				Unknown_5458B0();
				return(true);
			case 6:
				SoundLibraryClass::Play_2D_Sound("Roll_Select02b.wav", 2);
				Unknown_545D30();
				return(true);
		}
	}
	return(false);
}























































// @0x544C50
void UIAnalyzeClass::Unknown_544C50(int index, bool show, const char *text)
{
	const char *name = Unknown_B7967C[index]; RefCountPtr<FontSys> font = Create_Peek(Unknown_BE67C4->Get_Style(name, InterfaceMesh->Peek_Mesh_Object()));
	FAIL_IF(!font) return;
	font->Change_Text(text, 0);
	font->Set_Hidden(!show);
}

// NOT EXACT: inline budget (the client calls the 'NegotiatedCost' and 'CriticalSuccessProbability' finds out of line, Unknown_Find_Field 0x417D82 / 0x4138E0 thunks, and the 'SuccessProbability' Cast; ours inlines them) (0x544D80)
// @0x544D80
void UIAnalyzeClass::Unknown_544D80()
{
	MoneyClass cost;
	DeltaTemplateClass<MoneyClass> *negotiated = Unknown_Find_Field<DeltaTemplateClass<MoneyClass> >(Unknown10C, NameClass("NegotiatedCost"));
	if (negotiated && negotiated->Unknown70) {
		cost = negotiated->Value;
	}
	Unknown_544C50(3, true, cost.Unknown_4F00C0(false).c_str());
	char text[32];
	float success = 0;
	DeltaTemplateClass<float> *success_probability = Unknown_Find_Field<DeltaTemplateClass<float> >(Unknown10C, NameClass("SuccessProbability"));
	if (success_probability && success_probability->Unknown70) {
		success = success_probability->Value * 100.0f;
	}
	sprintf(text, "%1.0f%%", success);
	Unknown_544C50(4, true, text);
	float critical = 0;
	DeltaTemplateClass<float> *critical_probability = Unknown_Find_Field<DeltaTemplateClass<float> >(Unknown10C, NameClass("CriticalSuccessProbability"));
	if (critical_probability && critical_probability->Unknown70) {
		critical = critical_probability->Value * 100.0f;
	}
	sprintf(text, "%1.0f%%", critical);
	Unknown_544C50(5, true, text);
}

// NOT EXACT: inline budget (the client calls all four finds out of line, the 0x417D82 / 0x4138E0 thunks and 0x5465C0, as ui_manu.cpp's 0x5B5320; ours inlines them) (0x545190)
// @0x545190
void UIAnalyzeClass::Unknown_545190()
{
	MoneyClass cost;
	DeltaTemplateClass<MoneyClass> *negotiated = Unknown_Find_Field<DeltaTemplateClass<MoneyClass> >(Unknown10C, NameClass("NegotiatedCost"));
	if (negotiated && negotiated->Unknown70 > 1) {
		cost = negotiated->Value;
		Unknown_544C50(3, true, cost.Unknown_4F00C0(false).c_str());
	}
	char text[32];
	DeltaTemplateClass<float> *success_probability = Unknown_Find_Field<DeltaTemplateClass<float> >(Unknown10C, NameClass("SuccessProbability"));
	if (success_probability && success_probability->Unknown70 > 1) {
		sprintf(text, "%1.0f%%", success_probability->Value * 100.0f);
		Unknown_544C50(4, true, text);
	}
	DeltaTemplateClass<float> *critical_probability = Unknown_Find_Field<DeltaTemplateClass<float> >(Unknown10C, NameClass("CriticalSuccessProbability"));
	if (critical_probability && critical_probability->Unknown70 > 1) {
		sprintf(text, "%1.0f%%", critical_probability->Value * 100.0f);
		Unknown_544C50(5, true, text);
	}
	DeltaTemplateClass<UnknownB01394Enum> *validity = Unknown_Find_Field<DeltaTemplateClass<UnknownB01394Enum> >(Unknown10C, NameClass("Validity"));
	if (validity && validity->Unknown70 > 1) {
		const char *message = NULL;
		switch (validity->Value) {
			case 1:
				message = "Item may not be analyzed, dismantled or manufactured.";
				break;
			case 2:
				message = "Item may not be dismantled.";
				break;
			case 4:
				message = "Insufficient credits to analyze or dismantle item.";
				break;
			case 6:
				message = "More empty cargo space required to dismantle item.";
				break;
			case 7:
				message = "Not qualified to analyze or dismantle this item.";
				break;
			case 8:
				message = "Item too difficult to analyze or dismantle.";
				break;
			case 12:
				message = "Dismantling may result in a duplicate unique item.";
				break;
		}
		if (message) {
			Client->As_Space_Client()->Unknown_74D990(0x11, (char *)message, 0);
		}
	}
}

// NOT EXACT: inline budget (the client calls the 'Validity' and 'Mode' Casts out of line, 0x5468D0 / 0x546920; #pragma inline_depth(1) at the file's end does that but costs 0x5437C0, 0x5439F0 and 0x544C50) (0x545610)
// @0x545610
void UIAnalyzeClass::Unknown_545610()
{
	Unknown0E0[6]->Unknown_40(true);
	Unknown0E0[1]->Unknown_40(true);
	Unknown0E0[2]->Unknown_8C(true);
	Unknown0E0[3]->Unknown_8C(true);
	Unknown0E0[4]->Unknown_8C(true);
	Unknown0FC[0]->Set_Hidden(true);
	Unknown0FC[1]->Set_Hidden(true);
	Unknown0FC[2]->Set_Hidden(true);
	NameClass name("Validity");
	DeltaTemplateClass<UnknownB01394Enum> *validity = Unknown_Find_Field<DeltaTemplateClass<UnknownB01394Enum> >(Unknown10C, name);
	if (validity && validity->Unknown70) {
		bool analyze;
		NameClass mode_name("Mode");
		DeltaTemplateClass<UnknownB013ECEnum> *mode = Unknown_Find_Field<DeltaTemplateClass<UnknownB013ECEnum> >(Unknown10C, mode_name);
		if (mode && mode->Unknown70 && mode->Value == 2) {
			analyze = true;
			Unknown_544C50(8, true, Unknown_B796A8[11]);
			Unknown_544C50(9, true, Unknown_B796A8[12]);
		} else {
			analyze = false;
			Unknown_544C50(8, true, Unknown_B796A8[13]);
			Unknown_544C50(9, true, Unknown_B796A8[14]);
		}
		bool on = false;
		const char *go = NULL;
		switch (validity->Value) {
			case 13:
				on = true;
				go = analyze ? Unknown_B796A8[8] : Unknown_B796A8[9];
				break;
			case 15:
				on = false;
				go = Unknown_B796A8[10];
				break;
		}
		Unknown_544C50(1, true, go);
		Unknown0E0[5]->Unknown_40(on);
		return;
	}
	Unknown0E0[5]->Unknown_40(false);
	Unknown_544C50(1, true, "");
}

// NOT EXACT: inline budget (the client calls the 'Mode' find out of line, 0x546630, and the last text of key 5's dismantle branch, 0x544C50) (0x5458B0)
// @0x5458B0
void UIAnalyzeClass::Unknown_5458B0()
{
	Unknown110 = true;
	Unknown114 = Unknown_AEF0EC;
	if (Unknown0D8) {
		Unknown0D8->Unknown_62F5B0(false);
	}
	if (Unknown0DC) {
		Unknown0DC->Unknown_62F5B0(false);
	}
	DeltaTemplateClass<UnknownB013ECEnum> *mode = Unknown_Find_Field<DeltaTemplateClass<UnknownB013ECEnum> >(Unknown10C, NameClass("Mode"));
	if (mode && mode->Unknown70) {
		if (mode->Value == 2) {
			Unknown_544C50(6, true, Unknown_B796A8[0]);
			Unknown_544C50(8, true, Unknown_B796A8[11]);
			Unknown_544C50(9, true, Unknown_B796A8[12]);
		} else if (mode->Value == 3) {
			Unknown_544C50(6, true, Unknown_B796A8[1]);
			Unknown_544C50(8, true, Unknown_B796A8[13]);
			Unknown_544C50(9, true, Unknown_B796A8[14]);
		}
	}
	((ClientClass *)Client)->Send_Packet(&ManufactureActionPacketClass(Analyze_Player_Id(Client->As_Space_Client()), 2));
	Unknown_546000();
	Unknown0E0[5]->Unknown_40(false);
	Unknown_544C50(1, true, "");
}

// @0x545D30
void UIAnalyzeClass::Unknown_545D30()
{
	int player = Client->As_Space_Client()->Unknown1138;
	((ClientClass *)Client)->Send_Packet(&ManufactureActionPacketClass(player, 0));
	Unknown0E0[2]->Unknown_8C(true);
	Unknown0E0[3]->Unknown_8C(true);
	Unknown0E0[4]->Unknown_8C(true);
	Unknown0FC[0]->Set_Hidden(true);
	Unknown0FC[1]->Set_Hidden(true);
	Unknown0FC[2]->Set_Hidden(true);
}

// The finds through Unknown_Find_Valid (two inline layers, the file's top): out of line as the client's
// @0x545E40
void UIAnalyzeClass::Unknown_545E40()
{
	DeltaTemplateClass<UnknownB0122CEnum> *current = Unknown_Find_Valid<DeltaTemplateClass<UnknownB0122CEnum> >(Unknown10C, "CurrentItemCategory");
	if (current) {
		DeltaItemArrayClass<DeltaPrimaryCategoryClass> *categories = Unknown_Find_Valid<DeltaItemArrayClass<DeltaPrimaryCategoryClass> >(Unknown10C, "PrimaryCategories");
		if (categories) {
			for (unsigned int a = 0; a < categories->Children.size(); a++) {
				DeltaPrimaryCategoryClass *primary = (DeltaPrimaryCategoryClass *)categories->Children[a];
				for (unsigned int b = 0; b < primary->Categories.Children.size(); b++) {
					DeltaCategoryClass *category = (DeltaCategoryClass *)primary->Categories.Children[b];
					for (unsigned int c = 0; c < category->Subcategories.Children.size(); c++) {
						if (current->Value == ((DeltaSubCategoryClass *)category->Subcategories.Children[c])->ID.Value) {
							Unknown_5461C0(0, a, -1, -1);
							Unknown_5461C0(1, a, b, -1);
							Unknown_5461C0(2, a, b, c);
							return;
						}
					}
				}
			}
		}
	}
}



#pragma auto_inline(off)
// @0x546000
void UIAnalyzeClass::Unknown_546000()
{
	Unknown0E0[2]->Unknown_8C(true);
	Unknown0E0[3]->Unknown_8C(true);
	Unknown0E0[4]->Unknown_8C(true);
	Unknown0FC[0]->Set_Hidden(true);
	Unknown0FC[1]->Set_Hidden(true);
	Unknown0FC[2]->Set_Hidden(true);
}
#pragma auto_inline(on)

// 0x546090 finds through Unknown_Find_Root_Field (the file's top): the client's Cast out of line inside the inlined find
// @0x546090
void UIAnalyzeClass::Unknown_546090()
{
	NameClass name("FailureMessage");
	DeltaStringClass *failure = Unknown_Find_Root_Field<DeltaStringClass>(Unknown10C, name);
	if (failure && failure->Unknown70) {
		if (strcmp(failure->Get(), "") != 0) {
			Unknown_544C50(7, true, failure->Get());
			Unknown114 = Unknown_AEF0EC;
			if (Unknown0D8) {
				Unknown0D8->Unknown_62F5B0(false);
			}
			if (Unknown0DC) {
				Unknown0DC->Unknown_62F5B0(false);
			}
		}
	}
}

// @0x5461C0
void UIAnalyzeClass::Unknown_5461C0(int level, int a, int b, int c)
{
	char name[64];
	char file[64];
	switch (level) {
		case 0:
			sprintf(name, "ICON_%d", a);
			sprintf(file, "IM_%d.tga", a);
			break;
		case 1:
			sprintf(name, "ICON_%d%d", a, b);
			sprintf(file, "IM_%d%d.tga", a, b);
			break;
		case 2:
			sprintf(name, "ICON_%d%d%d", a, b, c);
			sprintf(file, "IM_%d%d%d.tga", a, b, c);
			break;
	}
	Unknown0E0[level + 2]->Unknown2C_06 = 0;
	GadgetClass::Set_Tooltip(Unknown0E0[level + 2], "UI_MANUFACTURE", name);
	RefCountPtr<TextureClass> texture = Create_Get(WW3DAssetManager::Get_Instance()->Get_Texture(file, MIP_LEVELS_ALL, WW3D_FORMAT_UNKNOWN, true));
	if (texture) {
		if (!texture->Is_Initialized()) {
			TextureLoader::Request_Foreground_Loading(texture.Peek());
		}
	}
	if (texture && !texture->Is_Missing_Texture()) {
		RenderObjClass *robj = Unknown0E0[level + 2]->RObj; Unknown_495FD0(robj, texture.Peek(), 0);
		Unknown0E0[level + 2]->Unknown_40(true);
		Unknown0E0[level + 2]->Unknown_8C(false);
		Unknown0FC[level]->Set_Hidden(false);
	} else {
		Unknown0E0[level + 2]->Unknown_40(false);
		Unknown0E0[level + 2]->Unknown_8C(true);
		Unknown0FC[level]->Set_Hidden(true);
	}
}

// Notes (AN1, 2026-11-06). Rebuilt: slots +0x58 / +0x5C / +0x68 / +0x7C (0x544090 / 0x544270 / 0x544BB0 / 0x544020,
// as ui_manu.cpp's), 0x545D30-0x5461C0, the constructor 0x5435D0 (with it the deleting destructor 0x543790)
// and the text helper 0x544C50 on its line (483); the destructor 0x5437C0 on its G_ASSERT's line 116.
// AN2 (2026-11-08): slots +0x54 0x5439F0 and +0x60 0x5443F0 (its default G_ASSERT on line 381) and the text and
// validity functions 0x544D80-0x5458B0 written (functional, NOT EXACT); the functions now in the client's order.
// The 'Validity' / 'Mode' fields are DeltaManufacturingLabClass's own types (aux_roots.h; per-class globals 0xC41640 /
// 0xC41634, VM1: ui_manu.cpp shares the Cast 0x5468D0). 0x545D30 repeats 0x546000's six hides (the client inlines them there but calls 0x546000 out of line
// from slot +0x5C; auto_inline(off) around 0x546000 gives both). UIAnalyzeClass is laid out from the constructor. The client calls the finds of
// 'CurrentItemCategory' / 'PrimaryCategories' out of line (0x546780 / 0x5467F0, Unknown_Find_Field over aux_roots.h's
// own types: ui_manu.cpp's 0x5B4680 calls the same copies, so it uses the same types) and inlines the 'FailureMessage'
// find but not its Cast: the inline budget of the whole file, not reproduced with only these functions (auto_inline(off)
// around 0x545E40, an explicit instantiation, dead inline calls 1..6: no change; inline_depth(0) around it puts the
// vector reads out of line too).
