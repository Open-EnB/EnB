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


// The names of the window's texts (0xB7967C)
static const char *Unknown_B7967C[] = { "Credits_tsm02a", "Go_tsm02a", "inv_tsm02a", "Cost_tsm02a", "Chance_tsm02a", "Critical_tsm02a", "Attempt_tsm02a", "Result_tsm02a", "Succ_tsm02a", "Crit_tsm02a", "" };	// NAME 1: neutral (D10)
static const float Unknown_AEF0EC = 2.0f;			// NAME 1: neutral (D10): what 0x546090 stores (read from .rdata)

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

// NOT EXACT: inline budget (the client calls both finds out of line, 0x546780 / 0x5467F0, as ui_manu.cpp's 0x5B4680) (0x545E40)
// @0x545E40
void UIAnalyzeClass::Unknown_545E40()
{
	NameClass name("CurrentItemCategory");
	DeltaTemplateClass<UnknownB0122CEnum> *current = Unknown_Find_Field<DeltaTemplateClass<UnknownB0122CEnum> >(Unknown10C, name);
	if (current && current->Unknown70) {
		NameClass categories_name("PrimaryCategories");
		DeltaItemArrayClass<DeltaPrimaryCategoryClass> *categories = Unknown_Find_Field<DeltaItemArrayClass<DeltaPrimaryCategoryClass> >(Unknown10C, categories_name);
		if (categories && categories->Unknown70) {
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

// NOT EXACT: inline budget (the client calls UnknownRTTIClass<DeltaStringClass>::Cast out of line; a #pragma inline_depth(1) at the file's end makes it exact but costs 0x5461C0) (0x546090)
// @0x546090
void UIAnalyzeClass::Unknown_546090()
{
	NameClass name("FailureMessage");
	DeltaStringClass *failure = Unknown_Find_Field<DeltaStringClass>(Unknown10C, name);
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
























































































































































// @0x544C50
void UIAnalyzeClass::Unknown_544C50(int index, bool show, const char *text)
{
	const char *name = Unknown_B7967C[index]; RefCountPtr<FontSys> font = Create_Peek(Unknown_BE67C4->Get_Style(name, InterfaceMesh->Peek_Mesh_Object()));
	FAIL_IF(!font) return;
	font->Change_Text(text, 0);
	font->Set_Hidden(!show);
}

// Notes (AN1, 2026-11-06). Rebuilt: slots +0x58 / +0x5C / +0x68 / +0x7C (0x544090 / 0x544270 / 0x544BB0 / 0x544020,
// as ui_manu.cpp's), 0x545D30-0x5461C0, the constructor 0x5435D0 (with it the deleting destructor 0x543790)
// and the text helper 0x544C50 on its line (483, after them for now); the destructor 0x5437C0 on its G_ASSERT's line 116.
// Not yet in this file: slots +0x54 0x5439F0 and +0x60 0x5443F0 and the text and validity functions 0x544D80-0x5458B0
// (they inline 0x544C50's FAIL_IF): when they come, the functions go
// into the client's order (0x544C50 between 0x544BB0 and 0x544D80). 0x545D30 repeats 0x546000's six hides (the client inlines them there but calls 0x546000 out of line
// from slot +0x5C; auto_inline(off) around 0x546000 gives both). UIAnalyzeClass is laid out from the constructor. The client calls the finds of
// 'CurrentItemCategory' / 'PrimaryCategories' out of line (0x546780 / 0x5467F0, Unknown_Find_Field over aux_roots.h's
// own types: ui_manu.cpp's 0x5B4680 calls the same copies, so it uses the same types) and inlines the 'FailureMessage'
// find but not its Cast: the inline budget of the whole file, not reproduced with only these functions (auto_inline(off)
// around 0x545E40, an explicit instantiation, dead inline calls 1..6: no change; inline_depth(0) around it puts the
// vector reads out of line too).
