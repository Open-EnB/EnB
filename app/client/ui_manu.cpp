// Earth & Beyond client (reconstruction): app\Client\ui_manu.cpp. Notes at the end of the file.
#include <stdio.h>
#include <string.h>
#include "ui_manu.h"
#include "client.h"
#include "..\packets\pkmanufactureitemcatagorypacket.h"
#include "rendobj.h"
#include "sbaseview.h"
#include "..\shared\g_assert.h"
#include "..\shared\auxdata\auxdata.h"
#include "..\audio\soundlib.h"
#include "assetmgr.h"
#include "texture.h"
#include "textureloader.h"
#include "..\shared\auth_product.h"
#include "fontmanager.h"	// FontSys, FontManagerClass (0xBE67C4) and fontdatabase.h's FontStringFilesClass (0xBE67BC)

















extern FontManagerClass *Unknown_BE67C4;	// NAME 1: neutral (D10)
#include "..\..\ww3d2\ww3d.h"	// WW3D::Get_Frame_Time / Get_Sync_Time (0xBFAE3C / 0xBFAE40: WW3D::SyncTime / PreviousSyncTime, written by WW3D::Sync 0x918200, read inline)

// gadget.cpp's gadget, gd_slider.cpp's slider, the buttons (gd_uimesh.h's ButtonGadgetClass) and the
// check buttons (unknown_7e7e00.h's CheckBoxGadgetClass)
#include "gadget.h"
#include "gd_slider.h"
#include "unknown_7e7e00.h"
#include "local_interface_sort.h"	// InterfaceSortScopeClass: the window's sort scope (0x541980, InterfaceSortClass's destructor 0x6E18B0)












































// The names of the window's texts (0xB81868)
static const char *Unknown_B81868[] = { "Go_tsm01a", "Cost_tsm01a", "Quality_Range_tsm01a", "Expected_Quality_tsm01a", "Result_tsm01a", "" };	// NAME 1: neutral (D10)
static inline __int64 Unknown_Filter_Seconds() { return 5; } static __int64 Unknown_BE5C20 = Unknown_Filter_Seconds();	// NAME 1: neutral (D10): the seconds before a filter request is given back (initialised at run time, 0x5AF9D0: a constant initialiser is static data, an inline call gives the $E; the helper is interim)
static unsigned int Unknown_B8183C = 9;					// NAME 1: neutral (D10): the filter requests allowed
static const float Unknown_AF2D18 = 1.0f;				// NAME 1: neutral (D10): what 0x5B3A70 stores (read from .rdata)


UIManuClass::UIManuClass(StarbaseViewClass *view, const LayerClass &layer) :
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
	Unknown_BE67BC->Load("FA_TSM01A.INI");
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
	InterfaceMesh->Unknown_5B9930(-1);
	Unknown0B4 = NULL;
	Unknown0B8 = NULL;
	Unknown0BC = NULL;
	Unknown_BE67BC->Unload("FA_TSM01A.INI");
}

// The control keys event slot +0x54 registers (0x10 bytes) and what InterfaceClass +0x4C is, as far as this file calls it
#include "control_key_list.h"	// ControlKeyListClass: InterfaceClass +0x4C (Add 0x721E60, Remove 0x721EF0), keyed by kernel.h's KernelEventClass










// (the button gadgets slot +0x54 makes, 0x675240 / 0x675170: gd_uimesh.h's ButtonGadgetClass)









// The window's buttons, the category columns' names and their ids (0xB81850, 0xB81840, 0xB818A8)
static const char *Unknown_B81850[] = { "exit", "item00", "cat00", "sub00", "recent00", "go" };	// NAME 1: neutral (D10)
static const char *Unknown_B81840[] = { "item", "cat", "sub", "not used" };	// NAME 1: neutral (D10)
static int Unknown_B818A8[] = { 18, 23, 28 };			// NAME 1: neutral (D10): the columns' first ids (slot +0x68's cases)

// The client at InterfaceClass +0x40, as this file calls it (the FAIL_IF '!Client->As_Space_Client()')
class LineDisplayClientClass  // NAME 1: neutral (D10)
{
public:
	virtual void Unknown_00(); virtual void Unknown_04(); virtual void Unknown_08(); virtual void Unknown_0C();
	virtual void Unknown_10(); virtual void Unknown_14();
	virtual SpaceClientClass *As_Space_Client();					// +0x18 (ClientClass::As_Space_Client; SpaceClientClass's returns itself)  // NAME 3: the FAIL_IF's
};









// 0x5B0B40
void UIManuClass::Unknown_7C(UIViewClass *view, bool arg)
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

// (lines kept: the asserts below carry the client's line numbers)









void UIManuClass::Unknown_58()
{
	SpaceClientClass *sclient = Client->As_Space_Client();
	int player_id; ((ClientClass *)Client)->Send_Packet(&ManufactureItemCatagoryPacketClass(player_id = sclient->Unknown1138, 1));	// NAME 1 [player_id]: neutral (D10)
	if (State == UI_CLOSED) {
		InterfaceSortScopeClass scope(this);
		State = UI_OPEN;
		Unknown0BAC = sclient->PDAWindow->Unknown_695780(false);
		Unknown_5B2820();






		RenderObjClass *mesh = InterfaceMesh->Peek_Mesh_Object(); Unknown_BE67C4->Add(mesh);	// NAME 1 [mesh]: neutral (D10)
		RefCountPtr<FontSys> name_text = Create_Peek(Unknown_BE67C4->Get_Style("Name_tsm01a", InterfaceMesh->Peek_Mesh_Object()));
		RefCountPtr<RenderObjClass> name_obj = Create_Get(InterfaceMesh->Peek_Object()->Get_Sub_Object_By_Name("name"));
		int name_sort_level = name_obj->Get_Sort_Level() - 1; name_text->Set_Sort_Level(name_sort_level);	// NAME 1 [name_sort_level]: neutral (D10)
		char buffer[256];
		for (int i = 0; i < 10; i++) {
			sprintf(buffer, "desc%02d_tsm01a", i);
			Unknown16C[i] = Create_Peek(Unknown_BE67C4->Get_Style(buffer, InterfaceMesh->Peek_Mesh_Object()));
			int row_sort_level = Unknown0E8[i]->RObj->Get_Sort_Level() + 1; Unknown16C[i]->Set_Sort_Level(row_sort_level);	// NAME 1 [row_sort_level]: neutral (D10)
			Unknown16C[i]->Change_Text("", 0);
			sprintf(buffer, "var%02d_tsm01a", i);
			Unknown194[i] = Create_Peek(Unknown_BE67C4->Get_Style(buffer, InterfaceMesh->Peek_Mesh_Object()));
			Unknown194[i]->Change_Text("", 0);
		}
		Unknown0AC->Unknown_69DC40();
		Unknown0B4->Unknown_58();
		Unknown0B8->Unknown_58();
		Unknown0BC->Unknown_58();
		Unknown5BB320Class::Unknown_58();
		ClientObjectClass *target = Client->As_Space_Client()->Unknown_74BF80();
		if (target) {
			Unknown0BA0 = (DeltaContainer *)target->AuxData;
		}
		Unknown_5B4B10();
		Unknown_5B3E00();
		Unknown_5B2480();
		Unknown_5B2BD0(true);
		Unknown_5B2230();
		sclient->Unknown_744560(0x17);
	}
}

static inline int Manu_Player_Id(SpaceClientClass *sclient) { return sclient->Unknown1138; }  // NAME 2: the player id read as the client's inline accessor
void UIManuClass::Unknown_5C()
{
	State = UI_CLOSED;
	Unknown0AC->Unknown_69DC60();
	Unknown0BAE = false;
	SpaceClientClass *sclient = Client->As_Space_Client();
	((ClientClass *)Client)->Send_Packet(&ManufactureActionPacketClass(Manu_Player_Id(sclient), 0));
	((ClientClass *)Client)->Send_Packet(&ManufactureItemCatagoryPacketClass(Manu_Player_Id(sclient), 0));
	Unknown0B4->Unknown_5C();
	Unknown0B8->Unknown_5C();
	Unknown0BC->Unknown_5C();
	if (Unknown0BAC) {
		sclient->PDAWindow->Unknown_695A00(0, 1);
	}
	Unknown0BA0 = 0;
	Unknown_BE67C4->Remove(InterfaceMesh->Peek_Mesh_Object());
	Unknown5BB320Class::Unknown_5C();
	sclient->Unknown_744560(0x18);
	for (int i = 0; i < 10; i++) {
		if (Unknown16C[i]) {
			Unknown16C[i]->Change_Text("", 0);
		}
		if (Unknown194[i]) {
			Unknown194[i]->Change_Text("", 0);
		}
	}
}

// (the check button, 0x7E7E70: unknown_7e7e00.h's CheckBoxGadgetClass)












// The tech level filter buttons: one per level (0x70 bytes, table 0xAF2DF8) and the "all levels" one (0x6C bytes,
// table 0xAF2EBC), made by 0x5B6410; a toggle calls the window back
class UIManuTechLevelButtonClass : public CheckBoxGadgetClass	// NAME 1: neutral (D10)
{
public:
	UIManuTechLevelButtonClass(RenderObjClass *robj, LayerClass *layer, UIManuClass *owner, int tech_level) : CheckBoxGadgetClass(robj, *layer, 0x55), Owner(owner), TechLevel(tech_level) {}	// NAME 1 [robj, layer, owner]: neutral; NAME 3 [tech_level]: the FAIL_IF's
	virtual bool Unknown_2C(KernelEventClass *event, float x, float y, unsigned int mask)	// +0x2C 0x5B6680: 0x5B67C0 when it toggled (inline: emitted after 0x5B6410)
	{
		bool result = CheckBoxGadgetClass::Unknown_2C(event, x, y, mask);
		if (result) {
			Owner->Unknown_5B67C0(TechLevel, Checked, USER_INPUT);
		}
		return(result);
	}

	UIManuClass *Owner;									// +0x68  // NAME 2: what it holds
	int TechLevel;										// +0x6C (1..9)  // NAME 3: the FAIL_IF's
};
class UIManuAllLevelsButtonClass : public CheckBoxGadgetClass	// NAME 1: neutral (D10)
{
public:
	UIManuAllLevelsButtonClass(RenderObjClass *robj, LayerClass *layer, UIManuClass *owner) : CheckBoxGadgetClass(robj, *layer, 0x55), Owner(owner) {}	// NAME 1 [robj, layer, owner]: neutral
	virtual bool Unknown_2C(KernelEventClass *event, float x, float y, unsigned int mask)	// +0x2C 0x5B6720: 0x5B6A80 when it toggled (inline: emitted after 0x5B6410)
	{
		bool result = CheckBoxGadgetClass::Unknown_2C(event, x, y, mask);
		if (result) {
			Owner->Unknown_5B6A80(Checked, USER_INPUT);
		}
		return(result);
	}

	UIManuClass *Owner;									// +0x68  // NAME 2: what it holds
};

void Unknown_496AE0(RenderObjClass *robj, bool make_geometry_unique);	// 0x496AE0 (ro_funcs.cpp, cdecl)  // NAME 1 [robj, make_geometry_unique]: what the callers pass
bool Unknown_495FD0(RenderObjClass *robj, TextureClass *texture, int unknown);	// 0x495FD0 (ro_funcs.cpp, cdecl): the texture onto the model  // NAME 1 [robj, texture, unknown]: what 0x5B42F0 passes
// 0x65EFB0 is GadgetClass::Set_Tooltip (the static above, variadic as gadget.h declares it: the client pops each call's arguments on its own)

// The formula lists under the target item's aux data ('KnownFormula', 'PreviousAttempts'; their class's global at +0x04 is
// 0xC41628) and their entries, as far as this file reads them
class UIManuEntryClass : public DeltaContainer	// NAME 1: neutral (D10)
{
public:
	char Unknown09C[0x120 - 0x9C];						// +0x09C
	const char *Unknown120;								// +0x120: the name ("" ends the list)
	char Unknown124[0x1A8 - 0x124];						// +0x124
	int Unknown1A8;										// +0x1A8: the item id (0x5B39B0's)
};
#include "aux_manu.h"				// (after auxdata.h, as ui_cpit.cpp): 'Validity' is DeltaManufacturingLabClass's DeltaTemplateClass<UnknownB01394Enum> (0xC41640)
class UIManuListClass : public DeltaItemArrayClass<DeltaKnownFormulaClass>	// NAME 1: neutral (D10)
{
public:
	// UnknownGlobal: the base's (0xC41628, numbered by 0x5B71A0, stored by 0x6FA6F0)
};

class UIManuCategoriesClass : public DeltaItemArrayClass<DeltaPrimaryCategoryClass>	// NAME 1: neutral (D10): 'PrimaryCategories' (the tree UIManuNodeClass reads)
{
public:
	// UnknownGlobal: the base's (0xC4162C)
};







class UIManuCurrentCategoryClass : public DeltaTemplateClass<UnknownB0122CEnum>	// NAME 1: neutral (D10): 'CurrentItemCategory' (an item id)
{
public:
	// UnknownGlobal: the base's (0xC41630, numbered by 0x5B6FD0, stored by 0x6FA6F0); Value: the base's (+0x84)

};

class UIManuComponentsClass : public DeltaItemArrayClass<DeltaItemClass>	// NAME 1: neutral (D10): 'ManufactureComponents' (its global is the item arrays', 0xC41624)
{
public:
	// UnknownGlobal: the base's (0xC41624)
};

bool Is_Validity_Attempt(UnknownB01394Enum validity);				// 0x846210 (manufacturing_defs.h keeps it from aux_roots.cpp's declaration count)
static const char *Unknown_B81880[] = { "Manufacturing Target", "Attempt Succeeded", "N/A", " " };	// NAME 1: neutral (D10)
template<class C> inline C *Unknown_Find_Valid(DeltaContainer *root, const AuxDataName &name) { C *t = Unknown_Find_Field<C>(root, name); if (t && t->Unknown70) return(t); return(NULL); }	// NAME 1: neutral (D10): as co.cpp's
template<class C> inline C *Unknown_Find_Valid_Field(DeltaContainer *const &root, const char *name) { AuxDataName n(name); DeltaContainer *r = root; C *t = Unknown_Find_Field<C>(r, n); if (t && t->Unknown70) return(t); return(NULL); } template<class C> inline C *Unknown_Find_Valid(DeltaContainer *const &root, const char *name) { return(Unknown_Find_Valid_Field<C>(root, name)); }	// NAME 1: neutral (D10): as ui_analyze.cpp's (two inline layers: the client's finds 0x546780 / 0x5467F0 out of line in 0x5B4680)
// 0x5AFFC0
void UIManuClass::Unknown_54()
{
	for (int id = 1; id < 7; id++) { ButtonGadgetClass *&button = Unknown0CC[id];
		RefCountPtr<RenderObjClass> robj = Create_Get(InterfaceMesh->Peek_Object()->Get_Sub_Object_By_Name(Unknown_B81850[id - 1]));
		if (robj) {
			Unknown_496AE0(robj.Peek(), 0);
			button = new ButtonGadgetClass(robj.Peek(), Layer, 0x55, 3);
			Unknown64.Unknown_6870E0(button, id, View, 1, 0);
			Unknown_5BB4F0(button);
		}
	}
	RefCountPtr<TextureClass> recent = Create_Get(WW3DAssetManager::Get_Instance()->Get_Texture("IM_recent.tga", MIP_LEVELS_ALL, WW3D_FORMAT_UNKNOWN, true));
	if (recent) { RenderObjClass *mesh = Unknown0CC[5]->RObj;
		Unknown_495FD0(mesh, recent.Peek(), 0);
		Unknown0CC[5]->Unknown2C_06 = 0;
		GadgetClass::Set_Tooltip(Unknown0CC[5], "UI_MANUFACTURE", "ICON_RECENT");
	}
	GadgetClass::Set_Tooltip(Unknown0CC[6], "UI_MANUFACTURE", "ICON_GO");


	for (int row = 0; row < 10; row++) {
		char name[64]; sprintf(name, "desc%02d", row);
		Unknown0E8[row] = new ButtonGadgetClass(InterfaceMesh, name, Layer, 0x55, 3);
		Unknown64.Unknown_6870E0(Unknown0E8[row], row + 8, View, 1, 0);
		Unknown_5BB4F0(Unknown0E8[row]);
	}
	for (int level = 1; level <= NUM_TECH_LEVEL_FILTER_CHECK_BUTTONS; level++) {
		UIManuRowStruct &row = Unknown11C[level - 1]; char name[32]; _snprintf(name, 32, "T_LEV_ON_%02d", level);
		row.Gadget = Unknown_5B6410(level + 0x20, name, level, false);
		row.Gadget->Unknown_40(true);
		row.Gadget->Unknown_8C(false);
		row.Gadget->Unknown_7E8030(false);
		_snprintf(name, 32, "T_LEV_OFF_%02d", level);
		row.Unknown04 = Unknown_5B6410(level + 0x2A, name, level, true);
		row.Unknown04->Unknown_40(false);
		row.Unknown04->Unknown_8C(true);
		row.Unknown04->Unknown_7E8030(false);
	}

	((ControlKeyListClass *)Unknown4C)->Add(KernelEventClass(0x1B), (void *)1);
	Unknown0B0 = new GadgetSlider(false, &Layer);
	Unknown0B0->Unknown_66E460(Create_Peek(InterfaceMesh->Peek_Object()), 0, 1);
	Unknown64.Unknown_6870E0(Unknown0B0, 0x35, View, 1, 0);
	SpaceClientClass *sclient = Client->As_Space_Client();
	Unknown0B4 = Create_Get(new UIInvRobjStripClass(sclient, InterfaceMesh->Peek_Object(), 6, "TSM01A.E%02d", 12));
	Unknown0B4->Unknown_54();
	Unknown0B4->Unknown_7C(View, 0);
	Unknown0B8 = Create_Get(new UIInvRobjStripClass(sclient, InterfaceMesh->Peek_Object(), 1, "TSM01A.RESULT", 13));
	char buffer[64];
	for (int k = 0; k < 3; k++) {
		Unknown0C0[k] = new UIManuUnknownC0Class(k);
		for (int i = 0; i < 5; i++) {
			sprintf(buffer, "%s%02d", Unknown_B81840[k], i + 1);
			RefCountPtr<RenderObjClass> robj = Create_Get(InterfaceMesh->Peek_Object()->Get_Sub_Object_By_Name(buffer));
			if (robj) {
				Unknown_496AE0(robj.Peek(), 0);
				Unknown0C0[k]->Gadgets[i] = new ButtonGadgetClass(robj.Peek(), Layer, 0x55, 3);
				Unknown0C0[k]->Gadgets[i]->Unknown_65FFA0(-1);
				Unknown64.Unknown_6870E0(Unknown0C0[k]->Gadgets[i], Unknown_B818A8[k] + i, View, 1, 1);
				Unknown_5BB4F0(Unknown0C0[k]->Gadgets[i]);
			}
			sprintf(buffer, "%s%02df", Unknown_B81840[k], i + 1);
			Unknown0C0[k]->Unknown14[i] = InterfaceMesh->Peek_Object()->Get_Sub_Object_By_Name(buffer);
			Unknown_496AE0(Unknown0C0[k]->Unknown14[i], 0);
		}
		Unknown0C0[k]->Unknown_5B74F0(true);
		sprintf(buffer, "%sf", Unknown_B81850[k + 1]);
		Unknown110[k] = Create_Get(InterfaceMesh->Peek_Object()->Get_Sub_Object_By_Name(buffer));
		Unknown110[k]->Set_Hidden(true);
	}
	Unknown0B8->Unknown_54();
	Unknown0B8->Unknown_7C(View, 0);
	Unknown0BC = Create_Get(new UIInvRobjStripClass(sclient, InterfaceMesh->Peek_Object(), 1, "TSM01A.OVER", 14));
	Unknown0BC->Unknown_54();
	Unknown0BC->Unknown_7C(View, 0);
	RefCountPtr<RenderObjClass> tutorial = Create_Get(InterfaceMesh->Peek_Object()->Get_Sub_Object_By_Name("tutorial"));
	if (tutorial) {
		tutorial->Set_Hidden(true);
	}
	Unknown_10();
}
template<class C> C *Unknown_Find_Changed(DeltaContainer *root, const AuxDataName &name) { C *t = Unknown_Find_Field<C>(root, name); if (t && t->Unknown70 > 1) return(t); return(NULL); }	// NAME 1 [Unknown_Find_Changed]: neutral (D10); existing ui_refinery.cpp predicate
// NOT EXACT: inline budget (the nested PreviousAttempts lookup/Name boundary still differs; ManufactureComponents and Validity Casts 0x5B7210 / 0x5468D0 are recovered) (0x5B1230)
void UIManuClass::Unknown_60()
{
	Unknown_5B5320();
	if (Unknown0BA8 != 0.0f) {
		Unknown0BA8 -= (float)(__int64)(WW3D::Get_Frame_Time()) * 0.001f;
		if (Unknown0BA8 >= 0.0f) {
			goto slider;
		}
		Unknown0BA8 = 0;
		Unknown_5B1AF0(4, true, Unknown_B81868[5]);
	} else if (Unknown0BA4 != 0) {
		bool done = false;
		if (Unknown0BA4 == 1) {
			AuxDataName name("ManufactureComponents");
			UIManuComponentsClass *components = Unknown_Find_Changed<UIManuComponentsClass>((DeltaContainer *)Unknown0BA0, name);
			if (components) {
				Unknown0BA4 = 0;
				done = true;
				if (Unknown0BAD) {
					Unknown_5B4680();
				}
				if (!Unknown0BAE) {
					InterfaceMesh->Unknown_5B8DF0(0, "tsm01a.tsm01a-1", false, 0, 0, 0, -1);
					Unknown0BAE = true;
				}
				if (Unknown0BC) {
					Unknown0BC->Unknown_62F5B0(true);
				}
			}
		}
		if (Unknown0BA4 == 2) {
			AuxDataName name("Validity");
			DeltaTemplateClass<UnknownB01394Enum> *validity = Unknown_Find_Valid<DeltaTemplateClass<UnknownB01394Enum> >((DeltaContainer *)Unknown0BA0, name);
			if (Is_Validity_Attempt(validity->Value)) {
				Unknown0BA4 = 0;
				Unknown_5B1AF0(4, true, Unknown_B81880[1]);
				SoundLibrary::Play_2D_Sound("success_1.wav", 2);
				if (Unknown0BAD) {
					Unknown_5B3640(false);
				}
				Unknown_5B39B0(Unknown0BB0);
				Unknown_5B2230();
				goto refresh;
			}
		}
		if (!done) {
			goto slider;
		}
	}
refresh:
	Unknown_5B2480();
slider:
	Unknown_5B2950();
	Unknown5BB320Class::Unknown_60();
	for (int i = 0; i < 3; i++) {
		Unknown0C0[i]->Unknown_5B7880();
	}
	DurationClass now; now = DurationClass((unsigned int)((ClientClass *)Client)->Unknown10BC);
	if (now.Time > Unknown0BB8.Time && Unknown0BC0 > 0) {
		Unknown0BB8 = DurationClass(Unknown_BE5C20 * 1000 + now.Time);
		if (--Unknown0BC0 < Unknown_B8183C) {
			Unknown_5B6CA0();
		}
	}
}

// NOT EXACT: inline budget (in the inlined 0x5B1ED0 the client calls both finds out of line, 0x5B70C0) (0x5B16C0)
bool UIManuClass::Unknown_68(int key)
{
	if (Unknown0BA8 != 0.0f) {
		return(true);
	}
	Unknown0BB4 = false;
	switch (key) {
		case 1:
			SoundLibrary::Play_2D_Sound("Roll_Select02b.wav", 2);
			Unknown0AC->Unknown_69F440();
			return(true);
		case 2:
		case 3:
		case 4:
			SoundLibrary::Play_2D_Sound("fontclick.wav", 2);
			Unknown_5B1C20(key - 2);
			return(true);
		case 8:
		case 9:
		case 10:
		case 11:
		case 12:
		case 13:
		case 14:
		case 15:
		case 16:
		case 17:
			SoundLibrary::Play_2D_Sound("fontclick.wav", 2);
			Unknown_5B1ED0(key - 8);
			return(true);
		case 18:
		case 19:
		case 20:
		case 21:
		case 22:
			SoundLibrary::Play_2D_Sound("fontclick.wav", 2);
			Unknown_5B2060(key - 18);
			return(true);
		case 23:
		case 24:
		case 25:
		case 26:
		case 27:
			SoundLibrary::Play_2D_Sound("fontclick.wav", 2);
			Unknown_5B2060(key - 23);
			return(true);
		case 28:
		case 29:
		case 30:
		case 31:
		case 32:
			SoundLibrary::Play_2D_Sound("fontclick.wav", 2);
			Unknown_5B2060(key - 28);
			return(true);
		case 5:
			SoundLibrary::Play_2D_Sound("fontclick.wav", 2);
			if (!Unknown0BAD) {
				Unknown_5B3640(true);
			}
			return(true);
		case 6:
			SoundLibrary::Play_2D_Sound("manufacture_1.wav", 2);
			Unknown0BB4 = true;
			Unknown_5B3A70();
			return(true);
	}
	return(false);
}






























template<class C> inline C *Unknown_Manu_Raw_Field(DeltaContainer *const &root, const char *name) { AuxDataName n(name); DeltaContainer *r = root; return(Unknown_Find_Field<C>(r, n)); } template<class C> inline C *Unknown_Manu_Raw(DeltaContainer *const &root, const char *name) { return(Unknown_Manu_Raw_Field<C>(root, name)); }	// NAME 1: neutral (D10): auxdata.h's find through two inline layers (ui_analyze.cpp's form): out of line as the client in 0x5B5320 (0x5465C0 / 0x5466A0 / 0x546710 / 0x5B7130 / 0x5B6DB0), 0x5B4B10 (0x5466A0) and 0x5B2480 (0x5465C0)


// 0x5B1AF0
void UIManuClass::Unknown_5B1AF0(int index, bool show, const char *text)
{
	int i = index; const char *name = Unknown_B81868[i]; RefCountPtr<FontSys> font = Create_Peek(Unknown_BE67C4->Get_Style(name, InterfaceMesh->Peek_Mesh_Object()));	// the index local: the same code out of line, but its inline copies in 0x5B5320 / 0x5B4850 load the name first as the client's
	FAIL_IF(!font) return;
	font->Change_Text(text, 0);
	font->Set_Hidden(!show);
}

// 0x5B1C20
void UIManuClass::Unknown_5B1C20(int level)
{
	Unknown0BAD = false;
	Unknown_5B2B40();
	if (!Unknown0BA0) {
		return;
	}
	if (Unknown0B98 != level) {
		Unknown0B98 = level;
		Unknown_5B6CA0();
		Unknown_5B2BD0(true);
		Unknown_5B2230();
	} else {
		Unknown0B98 = 3;
		Unknown_5B6CA0();
		Unknown0C0[level]->Unknown_5B74F0(false);
		for (int i = 0; i < 10; i++) {
			Unknown16C[i]->Unknown4E4 = true;
		}
		AuxDataName name("KnownFormula");
		UIManuListClass *list = Unknown_Find_Valid<UIManuListClass>((DeltaContainer *)Unknown0BA0, name);







		G_ASSERT(list);
		Unknown_5B3260(list);
	}
	Unknown_5B2480();
	Unknown_5B4850();
}

inline UIManuListClass *UIManuClass::Unknown_Find_List(const char *field) { AuxDataName name(field); return(Unknown_Find_Field<UIManuListClass>((DeltaContainer *)Unknown0BA0, name)); }
template<class C> inline C *Unknown_Find_Set_List(DeltaContainer *root, const AuxDataName &name) { C *t = Unknown_Find_Field<C>(root, name); return((t != NULL && t->Unknown70 != 0) ? t : NULL); }	// NAME 1: neutral (D10): the set test as one conditional expression (0x5B3850's jumps go straight to the end) with the explicit compares (their cost sends the 'PreviousAttempts' Cast out of line as the client's 0x5B7260)
inline UIManuListClass *UIManuClass::Unknown_Find_Valid_List(const char *field) { AuxDataName name(field); DeltaContainer *root = (DeltaContainer *)Unknown0BA0; return(Unknown_Find_Set_List<UIManuListClass>(root, name)); }	// the root read into a local after the name (its cost sends both Casts out of line in 0x5B1ED0's inlined 0x5B3850 as the client's 0x5B7260; 0x5B3850 stays exact)
static inline UIManuListClass *Unknown_Previous_Attempts(UIManuClass *manu) { return(manu->Unknown_Find_Valid_List("PreviousAttempts")); }	// NAME 1: neutral (D10): the extra layer of the 'PreviousAttempts' list (0x5B3850 calls its Cast out of line, the 'KnownFormula' one inline)
inline UIManuListClass *UIManuClass::Unknown_Get_List()
{
	if (Unknown0BAD) {
		return(Unknown_Previous_Attempts(this));
	}
	return(Unknown_Find_Valid_List("KnownFormula"));
}

// 0x5B1ED0
void UIManuClass::Unknown_5B1ED0(int row)
{
	if (Unknown0B94 != 0 && row < Unknown0B94 && Unknown0B98 == 3) {
		Unknown0B80[Unknown0B98] = Unknown0B90 + row;
		Unknown_5B3850();




		Unknown_5B2230();
		Unknown_5B2480();
	}
}

// 0x5B2060
void UIManuClass::Unknown_5B2060(int index)
{
	if (Unknown0B98 < 3) {
		if (Unknown0C0[Unknown0B98]->Gadgets[index]->Is_Enabled()) {
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
				Unknown16C[row]->Change_Text(name, 0);
				Unknown16C[row]->Unknown4E4 = true;
				Unknown194[row]->Change_Text(text, 0);
				Unknown_5B21A0(row);
			}
			for (int i = 0; i < 3; i++) {
				Unknown0C0[i]->Unknown_5B74F0(false);
			}
			return;
		}
		for (int row = 0; row < 10; row++) {
			Unknown16C[row]->Unknown4E4 = false;
			Unknown16C[row]->Change_Text("", 0);
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

// The 'Validity' find out of line as the client (0x5465C0) through two inline layers; both branches end in Unknown0CC[1]'s slot +0x40 (0x5B271A)
void UIManuClass::Unknown_5B2480()
{
	RefCountPtr<FontSys> go;
	for (int i = 0; i < 3; i++) {
		Unknown_5B42F0(i);
		if (Unknown0B98 == i) {
			Unknown0CC[i + 2]->Unknown_6603A0(0, 0);
			Unknown0CC[i + 2]->Unknown2C_0B = 1;
		} else {
			Unknown0CC[i + 2]->Unknown_6607D0();
			Unknown0CC[i + 2]->Unknown2C_0B = 0;
		}
	}
	go = Create_Peek(Unknown_BE67C4->Get_Style(Unknown_B81868[0], InterfaceMesh->Peek_Mesh_Object()));

	DeltaTemplateClass<UnknownB01394Enum> *validity = Unknown_Manu_Raw<DeltaTemplateClass<UnknownB01394Enum> >(Unknown0BA0, "Validity");
	bool valid = (validity && validity->Unknown70 && validity->Value == 13) ? true : false;
	if (Unknown0BA4 == 0 && valid) {
		Unknown0CC[6]->Unknown_40(true);
		go->Unknown_Set_2FC(1.0f);
	} else {
		Unknown0CC[6]->Unknown_40(false);
		go->Unknown_Set_2FC(0.33f);
		Unknown_5B1AF0(2, true, "");
		Unknown_5B1AF0(3, true, "");
	}
	Unknown0CC[1]->Unknown_40(true);
}

// 0x5B2820
void UIManuClass::Unknown_5B2820()
{
	Unknown0B98 = -1;
	Unknown_5B6CA0();


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
	int top = (int)Unknown0B0->Value;
	if (Unknown0B90 != top) {
		Unknown0B90 = top;
		Unknown_5B2230();
	}
}

// 0x5B2990
void UIManuClass::Unknown_5B2990()
{
	GadgetSlider *slider = Unknown0B0;
	int top = (int)slider->Value;
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

// The find out of line as the client (0x5467F0) through two inline layers; the B98 test a switch (sub / dec); the dead size reads (D272) send 0x5B2950 and the inlined checks' sizes out of line
void UIManuClass::Unknown_5B2BD0(bool force)
{
	if (!Unknown0BAD && Unknown0B98 != 3) {
		UIManuCategoriesClass *categories = Unknown_Manu_Raw<UIManuCategoriesClass>(Unknown0BA0, "PrimaryCategories");
		unsigned int i;
		if (categories && categories->Unknown70 && (force || categories->Unknown70 > 1)) {
			UIManuNodeClass *tree = (UIManuNodeClass *)categories;
			Unknown_5B2B40(); tree->Unknown084[0]->Unknown1A8.size();
			unsigned int count;
			switch (Unknown0B98) {
				case 0:
					count = tree->Unknown084.size();
					for (i = 0; i < count; i++) {
						Unknown98C[i] = Unknown_5B3B90(tree, i);
					}
					tree->Unknown084[0]->Unknown1A8.size(); break;
				case 1: { int a = Unknown0B80[0];
					count = tree->Unknown084[a]->Unknown1A8.size();
					for (i = 0; i < count; i++) {
						Unknown98C[i] = Unknown_5B3CA0(tree, a, i);
					}
					tree->Unknown084[0]->Unknown1A8.size(); tree->Unknown084[0]->Unknown1A8.size(); tree->Unknown084[0]->Unknown1A8.size(); break; }
				case 2: { int a = Unknown0B80[0]; int b = Unknown0B80[1];
					count = tree->Unknown084[a]->Unknown1A8[b]->Unknown230.size();
					for (i = 0; i < count; i++) {
						Unknown98C[i] = !tree->Unknown084[a]->Unknown1A8[b]->Unknown230[i]->Unknown230;
					}
					break; }
			}
			if (count < 500) {
				memset(&Unknown98C[count], 1, 500 - count);
			}
			Unknown_5B2230();
		}
	}
}

// 0x5B2F20
void UIManuClass::Unknown_5B2F20()
{
	if (!Unknown0BAD && Unknown0B98 == 3) {
		AuxDataName name("KnownFormula");
		UIManuListClass *list = Unknown_Find_Field<UIManuListClass>((DeltaContainer *)Unknown0BA0, name);
		if (list && list->Unknown70 > 1) {
			Unknown_5B3260(list);
		}
	}
}

// NOT EXACT: order (the find and its out-of-line Cast 0x5B72B0 as the client's through the set test; the client loads Client and its vtable while it walks to the item id) (0x5B2FE0)
void UIManuClass::Unknown_5B2FE0()
{
	if (Unknown0B98 == 3) {
		int a = Unknown0B80[0];
		int b = Unknown0B80[1];
		int c = Unknown0B80[2];
		AuxDataName name("PrimaryCategories");
		UIManuCategoriesClass *categories = Unknown_Find_Set_List<UIManuCategoriesClass>((DeltaContainer *)Unknown0BA0, name);
		if (categories) {
			int id = ((UIManuNodeClass *)categories)->Unknown084[a]->Unknown1A8[b]->Unknown230[c]->Unknown1A8;
			int player = Client->As_Space_Client()->Unknown1138; ((ClientClass *)Client)->Send_Packet(&ManufactureSetItemCategoryPacketClass(player, id));
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
		AuxDataName name("PreviousAttempts");
		UIManuListClass *list = Unknown_Find_Set_List<UIManuListClass>((DeltaContainer *)Unknown0BA0, name);
		if (list && (force || list->Unknown70 > 1)) {
			Unknown_5B3260(list);
		}
	}
}

// NOT EXACT: inline budget (the client calls rtti.h's Cast out of line, 0x5B7260, in both finds: ours calls the first find 0x5B70C0 and inlines the second Cast) (0x5B3640)
void UIManuClass::Unknown_5B3640(bool all)
{
	if (!Unknown0BAD) {
		Unknown0BB0 = -1;
	}
	Unknown0BAD = true;
	UIManuListClass *list = Unknown_Get_List();	// Unknown0BAD was just set: the 'KnownFormula' branch folds away (the client has no such code here)
	if (list) {
		Unknown_5B3260(list);
	}
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

// 0x5B3850
void UIManuClass::Unknown_5B3850()
{
	int pick = Unknown0B80[3];
	if (pick >= 0) {
		UIManuListClass *list = Unknown_Get_List();
		if (list) {
			Unknown_5B39B0(((UIManuEntryClass *)list->Children[pick])->Unknown1A8);
		}
	}
}

// 0x5B39B0
void UIManuClass::Unknown_5B39B0(int item_id)
{
	if (item_id != -1) {
		int player = Client->As_Space_Client()->Unknown1138;
		((ClientClass *)Client)->Send_Packet(&ManufactureSetItemIDPacketClass(player, item_id));
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
		FontSys *style = Unknown_BE67C4->Get_Style(Unknown_B81868[0], InterfaceMesh->Peek_Mesh_Object());
		style->Unknown2F4[2] = 0.33f;
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

// NOT EXACT: inline budget (the find out of line as the client's, 0x5467F0, through two inline layers; the first two passes are 0x5B3B90 / 0x5B3CA0 inlined, calling 0x5B3CA0 / 0x5B3D70 as the client; left: the third pass's inlined 0x5B3D70, whose vector size the client calls out of line) (0x5B3E00)
void UIManuClass::Unknown_5B3E00()
{

	UIManuCategoriesClass *categories = Unknown_Manu_Raw<UIManuCategoriesClass>(Unknown0BA0, "PrimaryCategories");
	if (!categories || !categories->Unknown70) {
		return;
	}
	UIManuNodeClass *tree = (UIManuNodeClass *)categories;
	int found = 0;
	int pick = -1;
	int count = tree->Unknown084.size();
	for (int a = 0; a < count; a++) {
		if (!Unknown_5B3B90(tree, a)) {
			found++;
			pick = a;
		}
	}




	if (found == 0) {
		Unknown0B80[0] = -2;
		return;
	}
	if (found != 1) {
		return;
	}
	Unknown0B98 = 0;
	Unknown_5B2BD0(true);
	Unknown0B80[0] = pick;
	Unknown_5B42F0(0);
	Unknown0B98 = 1;
	Unknown_5B6CA0();
	pick = -1;
	found = 0;
	count = tree->Unknown084[Unknown0B80[0]]->Unknown1A8.size();



	for (int b = 0; b < count; b++) {
		if (!Unknown_5B3CA0(tree, Unknown0B80[0], b)) {
			found++;
			pick = b;
		}
	}





	if (found != 1) {
		return;
	}
	Unknown_5B2BD0(true);
	Unknown0B80[1] = pick;
	Unknown_5B42F0(1);
	Unknown0B98 = 2;
	Unknown_5B6CA0();
	pick = -1;
	found = 0;
	count = tree->Unknown084[Unknown0B80[0]]->Unknown1A8[Unknown0B80[1]]->Unknown230.size();



	for (int c = 0; c < count; c++) {
		if (!Unknown_5B3D70(tree, Unknown0B80[0], Unknown0B80[1], c)) {
			pick = c;
			found++;
		}
	}
	if (found != 1) {
		return;
	}
	Unknown_5B2BD0(true);
	Unknown0B80[2] = pick;
	Unknown_5B42F0(2);
	Unknown0B98 = 3;
	Unknown_5B6CA0();
	Unknown_5B2FE0();
}

// 0x5B42F0
void UIManuClass::Unknown_5B42F0(int level)
{
	char name[64];
	char file[64];
	switch (level) {
		case 2:
			if (Unknown0B80[0] >= 0 && Unknown0B80[1] >= 0 && Unknown0B80[2] < 0) {
				sprintf(name, "ICON_DEFAULT");
				sprintf(file, "IM_default.tga");
			} else {
				sprintf(name, "ICON_%d%d%d", Unknown0B80[0], Unknown0B80[1], Unknown0B80[2]);
				sprintf(file, "IM_%d%d%d.tga", Unknown0B80[0], Unknown0B80[1], Unknown0B80[2]);
			}
			break;
		case 1:
			if (Unknown0B80[0] >= 0 && Unknown0B80[1] < 0) {
				sprintf(name, "ICON_DEFAULT");
				sprintf(file, "IM_default.tga");
			} else {
				sprintf(name, "ICON_%d%d", Unknown0B80[0], Unknown0B80[1]);
				sprintf(file, "IM_%d%d.tga", Unknown0B80[0], Unknown0B80[1]);
			}
			break;
		case 0:
			if (Unknown0B80[0] < 0 && Unknown0B80[0] != -2) {
				sprintf(name, "ICON_DEFAULT");
				sprintf(file, "IM_default.tga");
			} else {
				sprintf(name, "ICON_%d", Unknown0B80[0]);
				sprintf(file, "IM_%d.tga", Unknown0B80[0]);
			}
			break;
	}
	Unknown0CC[level + 2]->Unknown2C_06 = 0;
	GadgetClass::Set_Tooltip(Unknown0CC[level + 2], "UI_MANUFACTURE", name);
	RefCountPtr<TextureClass> texture = Create_Get(WW3DAssetManager::Get_Instance()->Get_Texture(file, MIP_LEVELS_ALL, WW3D_FORMAT_UNKNOWN, true));
	if (texture) {
		if (!texture->Is_Initialized()) {
			TextureLoader::Request_Foreground_Loading(texture.Peek());
		}
	}
	if (texture && !texture->Is_Missing_Texture()) {
		RenderObjClass *robj = Unknown0CC[level + 2]->RObj; Unknown_495FD0(robj, texture.Peek(), 0);
		Unknown0CC[level + 2]->Unknown_40(true);
		Unknown0CC[level + 2]->Unknown_8C(false);
		Unknown110[level]->Set_Hidden(false);
	} else {
		Unknown0CC[level + 2]->Unknown_40(false);
		Unknown0CC[level + 2]->Unknown_8C(true);
		Unknown110[level]->Set_Hidden(true);
	}
}

// The finds through Unknown_Find_Valid (two inline layers, above): out of line as the client's (0x5B4680)
void UIManuClass::Unknown_5B4680()
{
	UIManuCurrentCategoryClass *current = Unknown_Find_Valid<UIManuCurrentCategoryClass>(Unknown0BA0, "CurrentItemCategory");
	if (current) {
		UIManuCategoriesClass *categories = Unknown_Find_Valid<UIManuCategoriesClass>(Unknown0BA0, "PrimaryCategories");
		if (categories) {
			UIManuNodeClass *tree = (UIManuNodeClass *)categories;
			for (unsigned int a = 0; a < tree->Unknown084.size(); a++) {
				UIManuLevel1Class *level1 = tree->Unknown084[a];
				for (unsigned int b = 0; b < level1->Unknown1A8.size(); b++) {
					UIManuLevel2Class *level2 = level1->Unknown1A8[b];
					for (unsigned int c = 0; c < level2->Unknown230.size(); c++) {
						if (current->Value == level2->Unknown230[c]->Unknown1A8) {
							Unknown0B80[0] = a;
							Unknown0B80[1] = b;
							Unknown0B80[2] = c;
							Unknown_5B42F0(0);
							Unknown_5B42F0(1);
							Unknown_5B42F0(2);
							return;
						}
					}
				}
			}
		}
	}
}



// NOT EXACT: register (as 0x5B2480) (0x5B4850)
void UIManuClass::Unknown_5B4850()
{
	int player = Client->As_Space_Client()->Unknown1138;
	((ClientClass *)Client)->Send_Packet(&ManufactureActionPacketClass(player, 0));
	Unknown0CC[6]->Unknown_40(false);
	RefCountPtr<FontSys> go = Create_Peek(Unknown_BE67C4->Get_Style(Unknown_B81868[0], InterfaceMesh->Peek_Mesh_Object()));
	go->Unknown_Set_2FC(0.33f);
	Unknown_5B1AF0(2, true, "");
	Unknown_5B1AF0(3, true, "");
}

inline bool UIManuClass::Unknown_All_Levels_Checked()
{
	int i;
	for (i = 1; i < NUM_TECH_LEVEL_FILTER_CHECK_BUTTONS; i++) {
		if (!Unknown11C[i - 1].Gadget->Checked) {
			break;
		}
	}
	return(i == NUM_TECH_LEVEL_FILTER_CHECK_BUTTONS);
}






































































































































// The money and float fields 0x5B4B10 / 0x5B5320 read (here, not at the top: the asserts sit on the client's lines)


// 'TechLevelFilterBitfield' (bit n - 1 set: tech level n shown)
class UIManuTechLevelFilterClass : public DeltaTemplateClass<unsigned int>	// NAME 1: neutral (D10)
{
public:
	// UnknownGlobal: the base's (0xC4163C, numbered by 0x5B6FD0, stored by 0x6FA6F0); Value: the base's (+0x84)

};


// NOT EXACT: frame (the NegotiatedCost and ExpectedQuality finds out of line as the client, 0x5466A0 / 0x546710, through two inline layers, and the Casts as the client's; the stack slots differ) (0x5B4B10)
void UIManuClass::Unknown_5B4B10()
{
	MoneyClass cost;
	DeltaTemplateClass<MoneyClass> *negotiated = Unknown_Manu_Raw<DeltaTemplateClass<MoneyClass> >(Unknown0BA0, "NegotiatedCost");
	if (negotiated && negotiated->Unknown70) {
		cost = negotiated->Value;
	}
	Unknown_5B1AF0(1, true, cost.Amount_As_Text(false).c_str());

	DeltaTemplateClass<float> *min = Unknown_Find_Valid<DeltaTemplateClass<float> >((DeltaContainer *)Unknown0BA0, AuxDataName("MinimumQuality"));
	DeltaTemplateClass<float> *max = Unknown_Find_Valid<DeltaTemplateClass<float> >((DeltaContainer *)Unknown0BA0, AuxDataName("MaximumQuality"));
	DeltaTemplateClass<float> *expected = Unknown_Find_Valid<DeltaTemplateClass<float> >(Unknown0BA0, "ExpectedQuality");





	char text[64];
	if (min && max) {
		float min_quality = min->Value; float max_quality = max->Value;
		if (min_quality != -1.0f && max_quality != -1.0f) {
		FAIL_IF(min_quality < 0.0f) min_quality = 0.0f;
		FAIL_IF(max_quality > 3.0f) max_quality = 3.0f;
		FAIL_IF(min_quality > max_quality) min_quality = max_quality;
		if (min_quality == max_quality) {
			sprintf(text, "%2.0f", min_quality * 100.0f);
		} else {
			sprintf(text, "%2.0f%%-%2.0f%%", min_quality * 100.0f, max_quality * 100.0f);
		}
	} else strcpy(text, Unknown_B81880[2]); } else {
		strcpy(text, Unknown_B81880[2]);
	}
	Unknown_5B1AF0(2, true, text);

	if (expected) {
		float expected_quality = expected->Value;
		if (expected_quality == -1.0f) {
			sprintf(text, Unknown_B81880[2]);
		} else {



			FAIL_IF(expected_quality < 0.0f) expected_quality = 0.0f;
			FAIL_IF(expected_quality > 3.0f) expected_quality = 3.0f;
			sprintf(text, "%2.0f", expected_quality * 100.0f);
		}
	} else {
		strcpy(text, Unknown_B81880[2]);
	}
	Unknown_5B1AF0(3, true, text);

	UIManuTechLevelFilterClass *filter = Unknown_Find_Valid<UIManuTechLevelFilterClass>((DeltaContainer *)Unknown0BA0, AuxDataName("TechLevelFilterBitfield"));
	if (filter) {
		for (int i = 1; i < 10; i++) {
			Unknown_5B67C0(i, (filter->Value & (1 << (i - 1))) != 0, 1);
		}
	}
}


Unknown831340Class *Unknown_8318B0(int item_template_id, ClientClass *client);	// item_template.cpp's (client.h's item template)  // NAME 1: neutral (D10)  // NAME 3 [item_template_id]: the FAIL_IF's
// 0x5B5CD0
static std::string Unknown_5B5CD0(DeltaContainer *auxdata, LineDisplayClientClass *client)	// NAME 1: neutral (D10)  // NAME 3 [auxdata, client]: the FAIL_IFs'
{
	static std::string unknown_item("Unknown Item");
	FAIL_IF(!auxdata) return(unknown_item);
	FAIL_IF(!client) return(unknown_item);
	AuxDataName name("ManufactureTarget");
	UIManuComponentsClass *target_item_auxdata_set = Unknown_Find_Valid<UIManuComponentsClass>(auxdata, name);
	FAIL_IF(!target_item_auxdata_set) return(unknown_item);
	target_item_auxdata_set->Children.begin();	// not the client's: a dead read (D130 / D272) spends the inline budget the client spent (the find out of line, 0x5B7050; begin() costs one expansion, Get_Size two: with Get_Size the '!item_template' copy's _Tidy stays a call)


	int item_template_id = ((DeltaItemClass *)target_item_auxdata_set->Children[0])->ItemTemplateID.Value;
	FAIL_IF(item_template_id == -1) return(unknown_item);

	Unknown831340Class *item_template = Unknown_8318B0(item_template_id, (ClientClass *)client);
	FAIL_IF(!item_template) return(unknown_item);
	return(item_template->Get_Name());
}

// 0x5B6060
static std::string Unknown_5B6060(int validity)	// NAME 1: neutral (D10)  // NAME 1 [validity]: the 'Validity' value
{
	switch (validity) {
		case 4:
			return("Missing sufficient credits");
		case 6:
			return("Missing sufficient cargo room");
		case 12:
			return("Unique item already owned");
		case 5:
			return("Missing required component(s)");
		case 8:
			return("Too difficult");
		case 7:
			return("Missing required skill level");
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
	Unknown_Find_Field<DeltaTemplateClass<int> >(NULL, *(AuxDataName *)&validity);	// not the client's: a dead find (root NULL, unreachable: no code) spends the inline budget the client spent (D130), so the first case calls _Eos out of line as the client's
}

// @0x5B5320
void UIManuClass::Unknown_5B5320()
{		DeltaTemplateClass<UnknownB01394Enum> *validity = Unknown_Manu_Raw<DeltaTemplateClass<UnknownB01394Enum> >(Unknown0BA0, "Validity");
	if (validity && validity->Unknown70 > 1) {
		int value = validity->Value; std::string reason = Unknown_5B6060(value);
		if (reason.length() && !Unknown0BB4) {
			LineDisplayClientClass *client = Client; std::string item = Unknown_5B5CD0((DeltaContainer *)Unknown0BA0, client);
			std::string message = "Unable to create " + item + std::string(". ") + reason + ".";
			((SpaceClientClass *)((ClientClass *)Client)->As_Space_Client())->Unknown_74D990(0x11, (char *)message.c_str(), 0);
		}
	}

	MoneyClass cost;		DeltaTemplateClass<MoneyClass> *negotiated = Unknown_Manu_Raw<DeltaTemplateClass<MoneyClass> >(Unknown0BA0, "NegotiatedCost");
	if (negotiated && negotiated->Unknown70 > 1) {
		cost = negotiated->Value;
		Unknown_5B1AF0(1, true, cost.Amount_As_Text(false).c_str());
	}
		DeltaTemplateClass<float> *min_f = Unknown_Manu_Raw<DeltaTemplateClass<float> >(Unknown0BA0, "MinimumQuality");
	DeltaTemplateClass<float> *min = (min_f && min_f->Unknown70 > 1) ? min_f : NULL;

		DeltaTemplateClass<float> *max_f = Unknown_Manu_Raw<DeltaTemplateClass<float> >(Unknown0BA0, "MaximumQuality");
	DeltaTemplateClass<float> *max = (max_f && max_f->Unknown70 > 1) ? max_f : NULL;

		DeltaTemplateClass<float> *expected_f = Unknown_Manu_Raw<DeltaTemplateClass<float> >(Unknown0BA0, "ExpectedQuality");
	DeltaTemplateClass<float> *expected = (expected_f && expected_f->Unknown70 > 1) ? expected_f : NULL;



	char text[32];
	if (min && max) {
		float min_quality = min->Value;
		float max_quality = max->Value;
		if (min_quality != -1.0f && max_quality != -1.0f) {






























			FAIL_IF(min_quality < 0.0f) min_quality = 0.0f;
			FAIL_IF(max_quality > 3.0f) max_quality = 3.0f;
			FAIL_IF(min_quality > max_quality) min_quality = max_quality;
			if (min_quality == max_quality) {
				sprintf(text, "%2.0f%%", min_quality * 100.0f);
			} else {
				sprintf(text, "%2.0f%% to %2.0f%%", min_quality * 100.0f, max_quality * 100.0f);
			}
		} else {
			strcpy(text, Unknown_B81880[2]);
		}
		Unknown_5B1AF0(2, true, text);
	}

	if (expected) {
		float expected_quality = expected->Value;
		if (expected_quality == -1.0f) {
			strcpy(text, Unknown_B81880[2]);
		} else {

			FAIL_IF(expected_quality < 0.0f) expected_quality = 0.0f;
			FAIL_IF(expected_quality > 3.0f) expected_quality = 3.0f;
			sprintf(text, "%2.0f%%", expected_quality * 100.0f);
		}
		Unknown_5B1AF0(3, true, text);
	}
		UIManuTechLevelFilterClass *filter = Unknown_Manu_Raw<UIManuTechLevelFilterClass>(Unknown0BA0, "TechLevelFilterBitfield");
	if (filter && filter->Unknown70 > 1) {
		int bits = filter->Value; for (int i = 1; i < 10; i++) {
			Unknown_5B67C0(i, (bits & (1 << (i - 1))) != 0, 1);
		}
	}

	if (Unknown0BAD) {				Unknown_5B3580(false);



	} else if (Unknown0B98 == 3) {
		Unknown_5B2F20();
	} else {
		Unknown_5B2BD0(false);
	}
}



















































// 0x5B6410
CheckBoxGadgetClass *UIManuClass::Unknown_5B6410(int id, const char *bttn_name, int tech_level, bool plain)
{
	FAIL_IF(!bttn_name || !*bttn_name) return(NULL);

	RefCountPtr<RenderObjClass> robj = Create_Get(InterfaceMesh->Peek_Object()->Get_Sub_Object_By_Name(bttn_name));
	FAIL_IF(!robj) return(NULL);
	Unknown_496AE0(robj.Peek(), 0);
	CheckBoxGadgetClass *button;
	if (plain) {
		button = new CheckBoxGadgetClass(robj.Peek(), Layer, 0);
	} else if (tech_level == NUM_TECH_LEVEL_FILTER_CHECK_BUTTONS) {
		button = new UIManuAllLevelsButtonClass(robj.Peek(), &Layer, this);
	} else {
		button = new UIManuTechLevelButtonClass(robj.Peek(), &Layer, this, tech_level);
	}
	Unknown64.Unknown_6870E0(button, id, View, 1, 0);
	button->Unknown_65FFA0(-1);
	return(button);
}





void UIManuClass::Unknown_5B67C0(int tech_level, bool on, int source)
{
	FAIL_IF(tech_level < 1 || tech_level >= NUM_TECH_LEVEL_FILTER_CHECK_BUTTONS) return;
	bool checked = Unknown11C[tech_level - 1].Gadget->Checked;
	bool changed = checked != on;
	if (changed) {





		G_ASSERT(source != USER_INPUT);
		Unknown11C[tech_level - 1].Gadget->Unknown_7E8030(on);
	} if (source == USER_INPUT) {
		unsigned int bit = 1 << (tech_level - 1);
		ClientClass *client = (ClientClass *)Client;
		ManufactureSetTechLevelFilterPacketClass packet(client->Unknown1138, on, bit);
		((ClientClass *)Client)->Send_Packet(&packet);
		Unknown0BC0++; if (Unknown0BC0 > Unknown_B8183C) {
			Unknown_5B6D30();
			Unknown_5B6CA0(); } } if (!on) {
		Unknown11C[NUM_TECH_LEVEL_FILTER_CHECK_BUTTONS - 1].Gadget->Unknown_7E8030(false);
	} else {
		Unknown11C[NUM_TECH_LEVEL_FILTER_CHECK_BUTTONS - 1].Gadget->Unknown_7E8030(Unknown_All_Levels_Checked());
	}
}






















// 0x5B6A80
void UIManuClass::Unknown_5B6A80(bool on, int source)
{
	if (source == USER_INPUT) {
		int player = ((ClientClass *)Client)->Unknown1138;
		ManufactureSetTechLevelFilterPacketClass packet(player, on, 0x1FF);
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
	Client->As_Space_Client()->Unknown_74D990(17, (char *)"Warning: Exceeded filter request count.  Please wait.", 0);
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
**	0x5B1C20's G_ASSERT 'list' is line 685. The functions without asserts between them are placed in the client's
**	address order; their lines are free.
**
**	Forms: the four picks at +0xB80 are reset by a loop (VC6 unrolls it into four stores of one register; separate
**	statements or a chained assignment keep -1 in a register across the rows' loop, memset goes through a pointer),
**	while the constructor's are a memset. The tree checks 0x5B3B90 / 0x5B3CA0 call the next level's check (inlined: the
**	level pointer hoisted, the vector indexed again on every pass). 0x5B1AF0 reads the name into a local before the
**	mesh. The formula lists are found through one inline helper holding the AuxDataName (one stack slot for both
**	names, as co.cpp's Unknown_Find_Aux); 0x5B3640's first find is such a helper over a local Unknown_Find_Valid (the
**	client's out-of-line copy of that template is 0x5B6DB0). The player id the packets carry is the space client's
**	+0x1138 (ClientClass::Unknown1138).
**
**	The target's texts 0x5B4B10 (lines 1473-1495) and 0x5B5320 (1625-1646) read the money and float fields through
**	aux_roots.h, included on a blank line before them; 0x5B5320's static helpers 0x5B5CD0 (the target's item name,
**	FAIL_IFs 1517-1529, its static 'Unknown Item' string destroyed by 0x5B5FF0) and 0x5B6060 (the Validity's message)
**	are defined before it, and the client emits them after it.
**	The constructor, destructor, slots +0x54 / +0x58 / +0x5C / +0x60 / +0x68, 0x5B2480, 0x5B2BD0, 0x5B2FE0,
**	0x5B3640, 0x5B3E00, 0x5B4680, 0x5B4850, 0x5B4B10, 0x5B5320 and 0x5B67C0 compile but are not exact (see their NOT
**	EXACT notes; most are the client's inline budget: it calls finds, casts or the tree checks out of line where this
**	compile inlines them). The static __int64 0x5AF9D0 initialises is the filter request interval.
*/
template UIManuListClass *Unknown_Find_Field<UIManuListClass>(DeltaContainer *root, const AuxDataName &name);	// D164: not the client's form; emits auxdata.h's find for the lists out of line (0x5B70C0, which the client's 0x5B16C0 calls) now that 0x5B5320's 'PreviousAttempts' find is 0x5B3580's set find (0x5B6DB0)
template UIManuComponentsClass *Unknown_Find_Field<UIManuComponentsClass>(DeltaContainer *root, const AuxDataName &name);	// D164: not the client's form; emits auxdata.h's 'ManufactureTarget' find out of line (0x5B7050, which the client's 0x5B5CD0 calls)
template class UnknownRTTIClass<UIManuTechLevelFilterClass>;	// D164: emits the filter Cast out of line (0x5B7300, which the client's 0x5B4B10 calls)

// 0x5B0BB0 is exact: retain a 256-byte label buffer, assign the player ID at its packet argument,
// name the font-manager mesh input and copy sort values before their setters. All original callbacks
// and reference-counted temporary lifetimes are preserved.

// 0x5B1230: the current tick is a zero-extended unsigned input carried by DurationClass. Its existing
// assignment layer recovers the retail Validity Cast boundary and this unit's exact 0x5468D0 holder;
// the enclosing handler remains partial. Signed timestamp operations and read timing are preserved.

// 0x5AFA00 is exact with the request timestamp represented by DurationClass. The eight-byte member
// retains its original offset, and the class size, alignment and every own-member offset are unchanged.
// All four ui_manu.h consumers preserve their complete native pair sets.

// 0x5B1230: reuse ui_refinery.cpp's changed-field predicate for ManufactureComponents, preserving
// the sole >1 state test and explicit name/root read order. This recovers the exact 0x5B7210 Cast holder.
// The timestamp store uses the existing DurationClass assignment; the enclosing handler remains partial.
