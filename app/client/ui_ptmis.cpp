// Earth & Beyond client (reconstruction): app\Client\ui_ptmis.cpp. Notes at the end of the file.
#include <stdio.h>

#include "rendobj.h"
#include "layer.h"
#include "refcount.h"
#include "..\shared\g_assert.h"
#include "gadget.h"
#include "fontmanager.h"
#include "gd_uisld.h"
#include "local_interface_sort.h"
#include "assetmgr.h"
#include "ui_ptmis.h"

extern FontManagerClass *Unknown_BE67C4;			// NAME 1 [Unknown_BE67C4]: 0xBE67C4

















#include "pt_data.h"




























// 0x5DE8A0
UIPTMisClass::UIPTMisClass(UIViewClass *view, LayerClass *layer) :
	InterfaceClass(view, (int)layer->Peek_Scene()),
	Layer(*layer),
	Unknown0B0(NULL),
	Unknown0C8(-1),
	Unknown0CC(-1),
	Unknown0D0(false)
{
	ListScreen = WW3DAssetManager::Get_Instance()->Create_Render_Obj("TLI03A"); G_ASSERT(ListScreen);
	DescScreen = WW3DAssetManager::Get_Instance()->Create_Render_Obj("TLI06A"); G_ASSERT(DescScreen);
	Unknown0B0 = new UISliderGadget(ListScreen, "pull_cargo02", "bar_cargo03", &Layer, 0, 0);
	Unknown64.Unknown_6870E0(Unknown0B0, 1, View, 1, 1);
	Unknown0B4 = new Unknown696320Class;
}

// 0x5DEAE0
UIPTMisClass::~UIPTMisClass()
{
	Unknown_5C();
	GadgetClass::Destroy(Unknown0B0);
	Unknown0B0 = NULL;
	delete Unknown0B4;
	ListScreen->Release_Ref();
	DescScreen->Release_Ref();
}

// 0x5DECF0
void UIPTMisClass::Unknown_54()
{
	Unknown_10();
}

inline void UIPTMisClass::Unknown_Make_Rows()
{
	char name[32];
	int i = 0;
	while (true) {
		sprintf(name, "Mission_%02d_PT_MIS", i);
		FontSys *mission = Unknown_BE67C4->Get_Style(name, ListScreen);
		if (!mission) break;
		UIPTMisRowStruct *row = new UIPTMisRowStruct;
		Unknown0B8.push_back(row);
		row->Mission = mission;
		sprintf(name, "Request_%02d_PT_MIS", i);
		row->Request = Unknown_BE67C4->Get_Style(name, ListScreen);
		sprintf(name, "Payoutval_%02d_PT_MIS", i);
		row->Payout = Unknown_BE67C4->Get_Style(name, ListScreen);
		i++;
	}
}

inline void UIPTMisClass::Unknown_Make_Buttons()
{
	char name[32];
	if (!Unknown0D0) {
		unsigned int j;
		for (j = 0; j < Unknown0B8.size(); j++) {
			sprintf(name, "B_%02d", j);
			RenderObjClass *robj = ListScreen->Get_Sub_Object_By_Name(name);
			GadgetClass *button = new ButtonGadgetClass(robj, Layer, 0x55, 0);
			robj->Release_Ref();
			Unknown64.Unknown_6870E0(button, j + 5, View, 1, 0);
		}
		for (j = 0; j < 3; j++) {
			sprintf(name, "S_%02d", j);
			RenderObjClass *robj = ListScreen->Get_Sub_Object_By_Name(name);
			GadgetClass *button = new ButtonGadgetClass(robj, Layer, 0x55, 0);
			robj->Release_Ref();
			Unknown64.Unknown_6870E0(button, j + 2, View, 1, 0);
		}
		Unknown0D0 = true;
	}
}

// 0x5DED10
void UIPTMisClass::Unknown_58()
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
		Unknown_5DF340(-1);
	}
}

// 0x5DF120
void UIPTMisClass::Unknown_5C()
{
	if (Active) {
		Unknown_BE67C4->Remove(ListScreen);
		Unknown_BE67C4->Remove(DescScreen);
		ListScreen->Remove();
		DescScreen->Remove();
		for (std::vector<UIPTMisRowStruct *>::iterator it = Unknown0B8.begin(); it != Unknown0B8.end(); it++) {
			delete *it;
		}
		Unknown0B8.clear();
		InterfaceClass::Unknown_5C();
	}
}

// 0x5DF200
void UIPTMisClass::Unknown_60()
{
}

// 0x5DF220
bool UIPTMisClass::Unknown_68(int key)
{
	switch (key) {
		case 1:
			Unknown_5DF410();
			return(true);
		case 2:
			return(true);
		case 3:
			return(true);
		case 4:
			return(true);
	}
	Unknown_5DF340(Unknown0B0->Unknown_A8() + key - 5);
	return(true);
}

// 0x5DF340
void UIPTMisClass::Unknown_5DF340(int index)
{
	if (index < Unknown0B4->Count()) {
		if (Unknown0C8 != index || index == -1) {
			Unknown0C8 = index;
			Unknown_5DF410();
		}
		ManagedFontClass *font = Unknown_BE67C4->Find(DescScreen);
		if (Unknown0C8 <= -1) {
			font->Unknown_6D0A70("WRAP_Mission_desc_PT_MIS", "Select Mission");
		} else {
			font->Unknown_6D0A70("WRAP_Mission_desc_PT_MIS", Unknown0B4->Unknown_6967A0(Unknown0C8)->Desc);
		}
	}
}

// 0x5DF410
void UIPTMisClass::Unknown_5DF410()
{
	if (Unknown0B0->Unknown0B0) {
		Unknown0B0->Unknown_675CF0(Unknown0B0->Unknown0AC, 0, Unknown0B0->Unknown0B4);
	}
	int count = Unknown0B4->Count() < 0 ? 0 : Unknown0B4->Count();

	if (count != Unknown0B0->Unknown0AC) {
		Unknown0B0->Unknown_675CF0(count, Unknown0B0->Unknown0B0, Unknown0B0->Unknown0B4);
	}
	int index = Unknown0B0->Unknown_A8();
	int old = Unknown0CC;
	int row = 0;
	Unknown0CC = Unknown0C8 - index;
	for (std::vector<UIPTMisRowStruct *>::iterator it = Unknown0B8.begin(); it != Unknown0B8.end(); it++) {
		if (old == row) {
			(*it)->Mission->Unknown_6CE910(false);
			(*it)->Request->Unknown_6CE910(false);
			(*it)->Payout->Unknown_6CE910(false);
		}
		if (Unknown0CC == row) {
			(*it)->Mission->Unknown_6CE910(true);
			(*it)->Request->Unknown_6CE910(true);
			(*it)->Payout->Unknown_6CE910(true);
		}
		row++;
		Unknown696320RecordStruct *record = Unknown0B4->Unknown_6967A0(index++);
		if (record) {
			(*it)->Mission->Change_Text(record->Type, 0);
			(*it)->Request->Change_Text(record->Sponsor, 0);
			(*it)->Payout->Change_Text(record->Payout, 0);
		} else {
			(*it)->Mission->Change_Text(" ", 0);
			(*it)->Request->Change_Text(" ", 0);
			(*it)->Payout->Change_Text(" ", 0);
		}

	}
}

/*
**	Reconstruction notes (app\Client\ui_ptmis.cpp, the petition terminal's missions page: ui_pterm.cpp's 0x5DE030
**	makes it, 0xD4 bytes).
**
**	The client's __FILE__ (0xB84D98) and the constructor's G_ASSERTs on lines 71 / 72 ('ListScreen' 'TLI03A',
**	'DescScreen' 'TLI06A') name the file and the two screens; the class name is class-from-file (D24), as
**	ui_ptrade.cpp's UIPTradeClass. The object runs from the constructor 0x5DE8A0 (after ui_pterm.cpp's last function
**	0x5DE880) through the adjustor thunks 0x5DF950 / 0x5DF960; ui_ptnew.cpp's constructor follows at 0x5DF970.
**	Its own slots (tables 0xAF6100 / 0xAF60F4 / 0xAF60E8 over InterfaceClass): the destructor, +0x54 (0x5DECF0,
**	the base's body emitted as the class's own), +0x58 open, +0x5C close, +0x60 empty, +0x68 the buttons.
**	std::vector<UIPTMisRowStruct *> gives the template copies 0x5DF620 (~vector, the constructor's unwind),
**	0x5DF660 (_Destroy) and 0x5DF680 (insert, push_back's). The guarded <string> initialiser 0x5DF920 ends
**	the object's code before the thunks (ui_ptnew.cpp's 0x5E0980, ui_ptppl.cpp's 0x5E1B40 likewise).
**
**	Forms: the rows are found by a while (true) loop (a for (;;) is rotated); the rows and the buttons are made
**	by two inline helpers, each with its own name buffer (one function's buffer leaves the row pointer and the
**	buttons' new temporaries in two stack slots, the frame 4 bytes larger); slot +0x68 has a return per case
**	(a shared break gives a compare chain, not the client's jump table); the description's test is
**	'<= -1'; the count clamp reads Count() twice.
**
**	pt_data.cpp's missions list (0x696320) comes from pt_data.h (included where its local declaration stood,
**	so no assert moves); the scroll bar 0x675370 is gd_uisld.h's UISliderGadget (included where unknown_675120.h
**	stood, its local declaration's lines left blank). The class is declared in ui_ptmis.h (PROVISIONAL name, D137).
*/
