// Earth & Beyond client (reconstruction): app\Client\ui_string_list.cpp. Notes at the end of the file.
#include <stdio.h>
#include <string.h>
#include "rendobj.h"
#include "layer.h"
#include "refcount.h"
#include "..\shared\g_assert.h"
#include "gadget.h"
#include "fontmanager.h"
#include "unknown_675120.h"
#include "ui_mesh.h"
#include "ui_string_list.h"

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



UIStringListClass::UIStringListClass(UIMeshClass *mesh, UIViewClass *view, LayerClass *layer, const char *format, bool owns_strings, int unknown) :
	InterfaceClass(view, (int)layer->Scene),
	TopIndex(0),
	Unknown0B8(0),
	OwnsStrings(owns_strings),
	Dirty(true),
	ScrollBar(NULL),
	Unknown0C4(0),
	Unknown0C8(0),
	Selected(NULL)
{
	char name[256];
	int i = -1;
	do {
		i++;
		sprintf(name, "DESC%02d", i);
	} while (mesh->Unknown_5B9160(name));
	int count = i;
	Lines.resize(count);

	for (i = 0; i < count; i++) {
		sprintf(name, "DESC%02d", i);
		sprintf(name, format, i);
		FontSys *string = Unknown_BE67C4->Get_Style(name, mesh->Peek_Object());

		G_ASSERT(string);
		if (string) {
			string->Add_Ref();
		}
		Lines[i] = new UIStringListLineStruct(string);
	}

	ScrollBar = new Unknown675370GadgetClass(mesh->Peek_Object(), "pull_cargo01", "bar_cargo01", layer, 0, 0);
	Unknown64.Unknown_6870E0(ScrollBar, 1, view, 1, 0);
}


UIStringListClass::~UIStringListClass()
{
	Shown.clear();

	Clear();








	for (std::vector<UIStringListLineStruct *>::iterator line = Lines.begin(); line != Lines.end(); line++) {
		(*line)->Text->Release_Ref();
		delete *line;
		*line = NULL;
	}
	Lines.clear();
}




void UIStringListClass::Clear()
{
	for (std::vector<UIStringListItemStruct *>::iterator it = Items.begin(); it != Items.end(); it++) {
		if (OwnsStrings) {
			delete [] (*it)->String;
		}
		delete *it;
		*it = NULL;
	}
	Items.clear();
}


void UIStringListClass::Unknown_54()
{
}




void UIStringListClass::Set_Dirty()
{
	Dirty = true;
}


void UIStringListClass::Add(int id, const char *string)
{
	char *copy;
	if (OwnsStrings) {
		copy = new char[strlen(string) + 1];
		strcpy(copy, string);
	} else {
		copy = (char *)string;
	}
	Items.push_back(new UIStringListItemStruct(id, copy));
	Dirty = true;
	if (Active) {
		Update_Scroll_Bar();
	}
}



void UIStringListClass::Unknown_58()
{
	if (!Active) {
		Refresh();
		Update_Scroll_Bar();
		InterfaceClass::Unknown_58();
	}
}


void UIStringListClass::Unknown_5C()
{
	if (Active) {
		InterfaceClass::Unknown_5C();
	}
}


void UIStringListClass::Unknown_60()
{
	Refresh();
}


bool UIStringListClass::Unknown_68(int key)
{
	switch (key) {
		case 1:
			TopIndex = ScrollBar->Unknown_A8();
			if (TopIndex < 0) {
				TopIndex = 0;
			}
			Dirty = true;
			return(true);
		default:
			key -= 2;
			if (key < (int)Lines.size()) {
				UIStringListItemStruct *item = Shown[key];
				if (Selected != item) {
					Selected = item;
				}
			}
	}
	return(false);
}

bool UIStringListClass::Refresh()
{
	if (!Dirty) {
		return(false);
	}

	Shown.clear();
	std::vector<UIStringListItemStruct *>::iterator it = Items.begin() + TopIndex;
	std::vector<UIStringListLineStruct *>::iterator line_it = Lines.begin();
	for (; line_it != Lines.end(); ) {
		if (it == Items.end()) {
			break;
		}
		UIStringListLineStruct *line = *line_it++;
		G_ASSERT(line);
		if (line) {

			UIStringListItemStruct *item = *it++;


			G_ASSERT(item);
			if (item) {
				line->Text->Change_Text(item->String, (item == Selected) ? 2 : 0);
				Shown.push_back(item);
			}
		}
	}





	for (; line_it != Lines.end(); ) {
		UIStringListLineStruct *line = *line_it++;
		G_ASSERT(line);
		if (line) {
			line->Text->Change_Text(" ", 0);
		}
	}
	Dirty = false;
	return(true);
}







int UIStringListClass::Find(int id)
{
	std::vector<UIStringListItemStruct *>::iterator iterator;
	for (iterator = Items.begin(); iterator != Items.end(); iterator++) {
		if ((*iterator)->Id == id) {
			return(iterator - Items.begin());
		}
	}
	G_ASSERT(iterator != Items.end());
	return(0);
}


// (an inline helper: Add and Unknown_58 inline it; 0x649090 is the wrapper below)
inline void UIStringListClass::Update_Scroll_Bar()
{
	if (ScrollBar->Unknown0B0 != 0) {
		ScrollBar->Unknown_675CF0(ScrollBar->Unknown0AC, 0, ScrollBar->Unknown0B4);
	}
	unsigned int lines = Lines.size();
	unsigned int items = Items.size();
	if (lines >= items) {
		if (ScrollBar->Unknown0AC != 0) {
			ScrollBar->Unknown_675CF0(0, ScrollBar->Unknown0B0, ScrollBar->Unknown0B4);
		}
	} else {
		int count = items - lines;
		if (count != ScrollBar->Unknown0AC) {
			ScrollBar->Unknown_675CF0(count, ScrollBar->Unknown0B0, ScrollBar->Unknown0B4);
		}
	}
}


void UIStringListClass::Unknown_649090()
{
	Update_Scroll_Bar();
}


/*
**	Reconstruction notes
**
**	Client 0x6483D0-0x649860 (the object's guarded <string> initialiser at 0x649840; the adjustor
**	thunks 0x649870 / 0x649880 to the deleting destructor follow it; 0x649890, 'TRADE01A', is the
**	next object). Own functions 0x6483D0-0x649090, then the vector instances 0x649170-0x649800:
**	~vector 0x649170 (items) / 0x6491B0 (lines), the items' erase 0x6491F0, _Destroy 0x649240,
**	insert 0x649520 and copy 0x6497C0, the lines' insert 0x649260, _Destroy 0x649500 and copy 0x649800.
**
**	Forms the bytes settle: the destructor calls Clear() (its erase stays out of line, 0x6491F0) and
**	clear()s the shown items; Update_Scroll_Bar is inline (Add and Unknown_58 inline it, insert stays
**	out of line) and 0x649090 is an uncalled out-of-line wrapper of it; the constructor counts the
**	bones in one variable and keeps the count in another, and takes a sixth, unused argument
**	(ret 0x18); Unknown_68 is a switch (dec / je).
*/
