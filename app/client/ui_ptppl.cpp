// Earth & Beyond client (reconstruction): app\Client\ui_ptppl.cpp. Notes at the end of the file.
#include <stdio.h>

#include "rendobj.h"
#include "layer.h"
#include "refcount.h"
#include "texture.h"
#include "..\shared\g_assert.h"
#include "gadget.h"
#include "fontmanager.h"
#include "unknown_675120.h"
#include "local_interface_sort.h"
#include "assetmgr.h"
#include "ui_ptppl.h"

extern FontManagerClass *Unknown_BE67C4;			// NAME 1 [Unknown_BE67C4]: 0xBE67C4

// The list's scroll bar (0xD0 bytes, table 0xAFBA80 over unknown_675120.h's base; constructor 0x675370)
class Unknown675370GadgetClass : public Unknown_675120Class	// NAME 1: neutral (D10)
{
public:
	Unknown675370GadgetClass(RenderObjClass *screen, const char *pull, const char *bar, const LayerClass *layer, int a, int b);	// 0x675370  // NAME 1 [screen, pull, bar, layer, a, b]: what the pages pass
	virtual void Unknown_A4();							// +0xA4 0x675D30
	virtual int Unknown_A8();							// +0xA8 0x675460 (the first row shown)
	void Unknown_675CF0(int count, int a, int b);		// 0x675CF0  // NAME 1 [count, a, b]: neutral

	char Unknown60[0xAC - 0x60];						// +0x60
	int Unknown0AC;										// +0xAC (the count)
	int Unknown0B0;										// +0xB0
	int Unknown0B4;										// +0xB4
	char Unknown0B8[0xD0 - 0xB8];						// +0xB8
};

#include "pt_data.h"



























// 0x5E09D0
UIPTPplClass::UIPTPplClass(UIViewClass *view, LayerClass *layer) :
	InterfaceClass(view, (int)layer->Peek_Scene()),
	Layer(*layer),
	Unknown0C4(NULL),
	Unknown0C8(NULL),
	Unknown0CC(0),
	Unknown0D0(NULL),
	Unknown0D4(NULL),
	Unknown0D8(-1),
	Unknown0DC(-1),
	Unknown0E0(false)
{
	ListScreen = WW3DAssetManager::Get_Instance()->Create_Render_Obj("TLI08A"); G_ASSERT(ListScreen);
	DescScreen = WW3DAssetManager::Get_Instance()->Create_Render_Obj("TLI09A"); G_ASSERT(DescScreen);
	Unknown0C4 = new Unknown675370GadgetClass(ListScreen, "pull_cargo02", "bar_cargo03", &Layer, 0, 0);
	Unknown64.Unknown_6870E0(Unknown0C4, 1, View, 1, 1);
	Unknown0D0 = WW3DAssetManager::Get_Instance()->Get_Texture("IF_TERR.TGA", MIP_LEVELS_1, WW3D_FORMAT_UNKNOWN, true);
	Unknown0D4 = WW3DAssetManager::Get_Instance()->Get_Texture("IF_CIR20.TGA", MIP_LEVELS_1, WW3D_FORMAT_UNKNOWN, true);
	Unknown0B0 = new Unknown696CD0Class;
}

// 0x5E0C60
UIPTPplClass::~UIPTPplClass()
{
	Unknown_5C();
	GadgetClass::Destroy(Unknown0C4);
	Unknown0C4 = NULL;
	delete Unknown0B0;
	REF_PTR_RELEASE(Unknown0D0);
	REF_PTR_RELEASE(Unknown0D4);
	ListScreen->Release_Ref();
	DescScreen->Release_Ref();
}

// 0x5E0DE0
void UIPTPplClass::Unknown_54()
{
	Unknown_10();
}

inline void UIPTPplClass::Unknown_Make_Rows()
{
	char name[32];
	int i = 0;
	while (true) {
		sprintf(name, "Name_%02d_PT_PEOPLE", i);
		FontSys *person = Unknown_BE67C4->Get_Style(name, ListScreen);
		if (!person) break;
		UIPTPplRowStruct *row = new UIPTPplRowStruct;
		Unknown0B4.push_back(row);
		row->Name = person;
		sprintf(name, "Prof_%02d_PT_PEOPLE", i);
		row->Prof = Unknown_BE67C4->Get_Style(name, ListScreen);
		sprintf(name, "Loc_%02d_PT_PEOPLE", i);
		row->Loc = Unknown_BE67C4->Get_Style(name, ListScreen);
		sprintf(name, "Level_%02d_PT_PEOPLE", i);
		row->Level = Unknown_BE67C4->Get_Style(name, ListScreen);
		i++;
	}
}

inline void UIPTPplClass::Unknown_Make_Buttons()
{
	char name[32];
	if (!Unknown0E0) {
		unsigned int j;
		for (j = 0; j < Unknown0B4.size(); j++) {
			sprintf(name, "B_%02d", j);
			RenderObjClass *robj = ListScreen->Get_Sub_Object_By_Name(name);
			GadgetClass *button = new ButtonGadgetClass(robj, Layer, 0x55, 0);
			robj->Release_Ref();
			Unknown64.Unknown_6870E0(button, j + 6, View, 1, 0);
		}
		for (j = 0; j < 4; j++) {
			sprintf(name, "S_%02d", j);
			RenderObjClass *robj = ListScreen->Get_Sub_Object_By_Name(name);
			GadgetClass *button = new ButtonGadgetClass(robj, Layer, 0x55, 0);
			robj->Release_Ref();
			Unknown64.Unknown_6870E0(button, j + 2, View, 1, 0);
		}
		Unknown0E0 = true;
	}
}

// 0x5E0E00
void UIPTPplClass::Unknown_58()
{
	if (!Active) {
		InterfaceSortScopeClass sort(this);
		ListScreen->Add(Layer.Peek_Scene());
		DescScreen->Add(Layer.Peek_Scene());
		Unknown_BE67C4->Add(ListScreen);
		Unknown_BE67C4->Add(DescScreen);
		Unknown_Make_Rows();
		Unknown_Make_Buttons();
		InterfaceClass::Unknown_58();
		Unknown_5E1390(-1);
	}
}

// 0x5E11E0
void UIPTPplClass::Unknown_5C()
{
	if (Active) {
		Unknown_BE67C4->Remove(ListScreen);
		Unknown_BE67C4->Remove(DescScreen);
		ListScreen->Remove();
		DescScreen->Remove();
		for (std::vector<UIPTPplRowStruct *>::iterator it = Unknown0B4.begin(); it != Unknown0B4.end(); it++) {
			delete *it;
		}
		Unknown0B4.clear();
		if (Unknown0C8) Unknown0C8->Release_Ref();
		InterfaceClass::Unknown_5C();
	}
}

// 0x5E12F0
void UIPTPplClass::Unknown_60()
{
}

// 0x5E1310
bool UIPTPplClass::Unknown_68(int key)
{
	switch (key) {
		case 1:
			Unknown_5E15F0();
			return(true);
		case 2:
			return(true);
		case 3:
			return(true);
		case 4:
			return(true);
		case 5:
			return(true);
	}
	Unknown_5E1390(Unknown0C4->Unknown_A8() + key - 6);
	return(true);
}

// 0x5E1390
void UIPTPplClass::Unknown_5E1390(int index)
{
	if (index < Unknown0B0->Count()) {
		if (Unknown0D8 != index || index == -1) {
			Unknown0D8 = index;
			Unknown_5E15F0();
		}
		ManagedFontClass *font = Unknown_BE67C4->Find(DescScreen);
		FontSys *combat = Unknown_BE67C4->Get_Style("Combat_PT_PEOPLE", DescScreen);
		FontSys *trade = Unknown_BE67C4->Get_Style("Trade_PT_PEOPLE", DescScreen);
		FontSys *explore = Unknown_BE67C4->Get_Style("Explore_PT_PEOPLE", DescScreen);
		FontSys *profession = Unknown_BE67C4->Get_Style("Profession_PT_PEOPLE", DescScreen);
		FontSys *person = Unknown_BE67C4->Get_Style("Person_Name_PT_PEOPLE", DescScreen);
		if (Unknown0D8 <= -1) {
			font->Unknown_6D0A70("WRAP_Profile00_PT_PEOPLE", "Select Person");
			combat->Change_Text(" ", 0);
			trade->Change_Text(" ", 0);
			explore->Change_Text(" ", 0);
			profession->Change_Text(" ", 0);
			person->Change_Text(" ", 0);
		} else {
			Unknown696CD0RecordStruct *record = Unknown0B0->Unknown_6971B0(Unknown0D8);
			font->Unknown_6D0A70("WRAP_Profile00_PT_PEOPLE", record->Background);
			combat->Unknown_6CE420(0, "Combat: %d", record->Combat);
			trade->Unknown_6CE420(0, "Trade: %d", record->Trade);
			explore->Unknown_6CE420(0, "Explore: %d", record->Explore);
			profession->Change_Text(record->Profession, 0);
			person->Change_Text(record->Name, 0);
		}
	}
}

// 0x5E15F0
void UIPTPplClass::Unknown_5E15F0()
{
	if (Unknown0C4->Unknown0B0) {
		Unknown0C4->Unknown_675CF0(Unknown0C4->Unknown0AC, 0, Unknown0C4->Unknown0B4);
	}
	int count = Unknown0B0->Count() < 0 ? 0 : Unknown0B0->Count();

	if (count != Unknown0C4->Unknown0AC) {
		Unknown0C4->Unknown_675CF0(count, Unknown0C4->Unknown0B0, Unknown0C4->Unknown0B4);
	}
	int index = Unknown0C4->Unknown_A8();
	int old = Unknown0DC;
	int row = 0;
	Unknown0DC = Unknown0D8 - index;
	for (std::vector<UIPTPplRowStruct *>::iterator it = Unknown0B4.begin(); it != Unknown0B4.end(); it++) {
		if (old == row) {
			(*it)->Name->Unknown_6CE910(false);
			(*it)->Prof->Unknown_6CE910(false);
			(*it)->Loc->Unknown_6CE910(false);
			(*it)->Level->Unknown_6CE910(false);
		}
		if (Unknown0DC == row) {
			(*it)->Name->Unknown_6CE910(true);
			(*it)->Prof->Unknown_6CE910(true);
			(*it)->Loc->Unknown_6CE910(true);
			(*it)->Level->Unknown_6CE910(true);
		}
		row++;
		Unknown696CD0RecordStruct *record = Unknown0B0->Unknown_6971B0(index++);
		if (record) {
			(*it)->Name->Change_Text(record->Name, 0);
			(*it)->Prof->Change_Text(record->Profession, 0);
			(*it)->Loc->Change_Text(record->Location, 0);
			(*it)->Level->Unknown_6CE420(0, "%d", record->Level);
		} else {
			(*it)->Name->Change_Text(" ", 0);
			(*it)->Prof->Change_Text(" ", 0);
			(*it)->Loc->Change_Text(" ", 0);
			(*it)->Level->Change_Text(" ", 0);
		}

	}
}

/*
**	Reconstruction notes (app\Client\ui_ptppl.cpp, the petition terminal's people page: ui_pterm.cpp's 0x5DE030
**	makes it, 0xE4 bytes).
**
**	The client's __FILE__ (0xB84F3C) and the constructor's G_ASSERTs on lines 75 / 76 ('ListScreen' 'TLI08A',
**	'DescScreen' 'TLI09A') name the file and the two screens; the class name is class-from-file (D24). The object
**	runs from the constructor 0x5E09D0 through the adjustor thunks 0x5E1B70 / 0x5E1B80; ui_ptrade.cpp's object
**	opens with its PermStringInitClass static at 0x5E1B90. The page is ui_ptmis.cpp's pattern (tables 0xAF6388 /
**	0xAF637C / 0xAF6370; template copies 0x5E1860 ~vector, 0x5E18A0 insert) with four styles a row, the two
**	textures 'IF_TERR.TGA' / 'IF_CIR20.TGA' (REF_PTR_RELEASE in the destructor), +0xC8 released on close
**	(nothing in the file sets it) and a profile ('Combat: %d', 'Trade: %d', 'Explore: %d') under the list.
**
**	pt_data.cpp's people list (0x696CD0) comes from pt_data.h (included where its local declaration stood); the
**	scroll bar class 0x675370 is declared here as far as this page uses it (neutral name, D10). The class is
**	declared in ui_ptppl.h (PROVISIONAL name, D137).
*/
