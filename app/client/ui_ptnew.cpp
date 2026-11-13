// Earth & Beyond client (reconstruction): app\Client\ui_ptnew.cpp. Notes at the end of the file.
#include <stdio.h>

#include "rendobj.h"
#include "layer.h"
#include "refcount.h"
#include "..\shared\g_assert.h"
#include "gadget.h"
#include "fontmanager.h"
#include "unknown_675120.h"
#include "local_interface_sort.h"
#include "assetmgr.h"
#include "ui_ptnew.h"

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

















































// 0x5DF970
UIPTNewClass::UIPTNewClass(UIViewClass *view, LayerClass *layer) :
	InterfaceClass(view, (int)layer->Peek_Scene()),
	Layer(*layer),
	Unknown0B0(NULL),
	Unknown0C8(-1),
	Unknown0CC(-1),
	Unknown0D0(false)
{
	ListScreen = WW3DAssetManager::Get_Instance()->Create_Render_Obj("TLI11A"); G_ASSERT(ListScreen);
	DescScreen = WW3DAssetManager::Get_Instance()->Create_Render_Obj("TLI07A"); G_ASSERT(DescScreen);
	Unknown0B0 = new Unknown675370GadgetClass(ListScreen, "pull_cargo02", "bar_cargo03", &Layer, 0, 0);
	Unknown64.Unknown_6870E0(Unknown0B0, 1, View, 1, 1);
	Unknown0B4 = new Unknown696810Class;
}

// 0x5DFBB0
UIPTNewClass::~UIPTNewClass()
{
	Unknown_5C();
	ListScreen->Release_Ref();
	DescScreen->Release_Ref();
	GadgetClass::Destroy(Unknown0B0);
	Unknown0B0 = NULL;
	delete Unknown0B4;
}

// 0x5DFDC0
void UIPTNewClass::Unknown_54()
{
	Unknown_10();
}

inline void UIPTNewClass::Unknown_Make_Rows()
{
	char name[32];
	int i = 0;
	while (true) {
		sprintf(name, "Date_%02d_PT_NEWS", i);
		FontSys *date = Unknown_BE67C4->Get_Style(name, ListScreen);
		if (!date) break;
		UIPTNewRowStruct *row = new UIPTNewRowStruct;
		Unknown0B8.push_back(row);
		row->Date = date;
		sprintf(name, "Location_%02d_PT_NEWS", i);
		row->Location = Unknown_BE67C4->Get_Style(name, ListScreen);
		sprintf(name, "Title_%02d_PT_NEWS", i);
		row->Title = Unknown_BE67C4->Get_Style(name, ListScreen);
		i++;
	}
}

inline void UIPTNewClass::Unknown_Make_Buttons()
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

// 0x5DFDE0
void UIPTNewClass::Unknown_58()
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
		Unknown_5E0470();
	}
}

// 0x5E0180
void UIPTNewClass::Unknown_5C()
{
	if (Active) {
		Unknown_BE67C4->Remove(ListScreen);
		Unknown_BE67C4->Remove(DescScreen);
		ListScreen->Remove();
		DescScreen->Remove();
		for (std::vector<UIPTNewRowStruct *>::iterator it = Unknown0B8.begin(); it != Unknown0B8.end(); it++) {
			delete *it;
		}
		Unknown0B8.clear();
		InterfaceClass::Unknown_5C();
	}
}

// 0x5E0260
void UIPTNewClass::Unknown_60()
{
}

// 0x5E0280
bool UIPTNewClass::Unknown_68(int key)
{
	switch (key) {
		case 1:
			Unknown_5E0470();
			return(true);
		case 2:
			return(true);
		case 3:
			return(true);
		case 4:
			return(true);
	}
	Unknown_5E03A0(Unknown0B0->Unknown_A8() + key - 5);
	return(true);
}

// 0x5E03A0
void UIPTNewClass::Unknown_5E03A0(int index)
{
	if (index < Unknown0B4->Count()) {
		if (Unknown0C8 != index || index == -1) {
			Unknown0C8 = index;
			Unknown_5E0470();
		}
		ManagedFontClass *font = Unknown_BE67C4->Find(DescScreen);
		if (Unknown0C8 <= -1) {
			font->Unknown_6D0A70("WRAP_NEWS00_PT_NEWS", "Select News");
		} else {
			font->Unknown_6D0A70("WRAP_NEWS00_PT_NEWS", Unknown0B4->Unknown_696C60(Unknown0C8)->Story);
		}
	}
}

// 0x5E0470
void UIPTNewClass::Unknown_5E0470()
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
	for (std::vector<UIPTNewRowStruct *>::iterator it = Unknown0B8.begin(); it != Unknown0B8.end(); it++) {
		if (old == row) {
			(*it)->Date->Unknown_6CE910(false);
			(*it)->Location->Unknown_6CE910(false);
			(*it)->Title->Unknown_6CE910(false);
		}
		if (Unknown0CC == row) {
			(*it)->Date->Unknown_6CE910(true);
			(*it)->Location->Unknown_6CE910(true);
			(*it)->Title->Unknown_6CE910(true);
		}
		row++;
		Unknown696810RecordStruct *record = Unknown0B4->Unknown_696C60(index++);
		if (record) {
			(*it)->Date->Change_Text(record->Date, 0);
			(*it)->Location->Change_Text(record->Location, 0);
			(*it)->Title->Change_Text(record->Headline, 0);
		} else {
			(*it)->Date->Change_Text(" ", 0);
			(*it)->Location->Change_Text(" ", 0);
			(*it)->Title->Change_Text(" ", 0);
		}

	}
}

/*
**	Reconstruction notes (app\Client\ui_ptnew.cpp, the petition terminal's news page: ui_pterm.cpp's 0x5DE030
**	makes it, 0xD4 bytes).
**
**	The client's __FILE__ (0xB84E6C) and the constructor's G_ASSERTs on lines 92 / 93 ('ListScreen' 'TLI11A',
**	'DescScreen' 'TLI07A') name the file and the two screens; the class name is class-from-file (D24). The object
**	runs from the constructor 0x5DF970 through the adjustor thunks 0x5E09B0 / 0x5E09C0; ui_ptppl.cpp's
**	constructor follows at 0x5E09D0. The page is ui_ptmis.cpp's pattern (tables 0xAF6244 / 0xAF6238 /
**	0xAF622C; template copies 0x5E0680 ~vector, 0x5E06C0 _Destroy, 0x5E06E0 insert); it differs in its strings,
**	in releasing the screens before the scroll bar and the list in its destructor, and in opening on the list
**	(0x5E0470) instead of a cleared selection.
**
**	pt_data.cpp's news list (0x696810) comes from pt_data.h (included where its local declaration stood); the
**	scroll bar class 0x675370 is declared here as far as this page uses it (neutral name, D10). The class is
**	declared in ui_ptnew.h (PROVISIONAL name, D137).
*/
