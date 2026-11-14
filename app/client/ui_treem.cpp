// Earth & Beyond client (reconstruction): app\Client\ui_treem.cpp. Notes at the end of the file.
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ww3d.h"
#include "rendobj.h"
#include "layer.h"
#include "refcount.h"
#include "texture.h"
#include "assetmgr.h"
#include "..\shared\g_assert.h"
#include "gadget.h"
#include "fontmanager.h"
#include "gd_uisld.h"
#include "local_interface_sort.h"
#include "ui_mesh.h"
#include "ui_treem.h"
#include "..\audio\soundlib.h"

extern FontManagerClass *Unknown_BE67C4;			// NAME 1 [Unknown_BE67C4]: 0xBE67C4



































UITreeMClass::UITreeMClass(UIMeshClass &mesh, UIViewClass *view, LayerClass *layer, const char *format, bool owns_strings) :
	InterfaceClass(view, (int)layer->Scene),
	Lines(16),
	Opened(NULL),
	TopIndex(0),
	ShownCount(0),
	TopCount(0),
	OwnsStrings(owns_strings),
	Unknown0D5(true),
	Dirty(true),
	ScrollBar(NULL),
	ClickTime(0),
	Selected(NULL)
{
	char name[256];
	int id = 2;
	int i = 0;
	while (true) {
		sprintf(name, "DESC%02d", i);
		if (!mesh.Get_Bone_Index(name)) {
			break;
		}
		UITreeMLineStruct *line = Lines.Uninitialized_Add();
		line->Desc = new ButtonGadgetClass(&mesh, name, *layer, 0x55, 5);
		Unknown64.Unknown_6870E0(line->Desc, id++, view, 1, 0);

		sprintf(name, "PM%02d", i);
		G_ASSERT(mesh.Get_Bone_Index(name));
		line->PlusMinus = new ButtonGadgetClass(&mesh, name, *layer, 0x55, 4);
		Unknown64.Unknown_6870E0(line->PlusMinus, id++, view, 1, 0);

		sprintf(name, format, i);
		line->String = Unknown_BE67C4->Get_Style(name, mesh.Peek_Object());


		G_ASSERT(line->String);
		line->String->Add_Ref();
		i++;
	}

	MinusTexture = WW3DAssetManager::Get_Instance()->Get_Texture("IF_MINUS.TGA", MIP_LEVELS_1, WW3D_FORMAT_UNKNOWN, true);
	PlusTexture = WW3DAssetManager::Get_Instance()->Get_Texture("IF_PLUS.TGA", MIP_LEVELS_1, WW3D_FORMAT_UNKNOWN, true);
	NoneTexture = WW3DAssetManager::Get_Instance()->Get_Texture("IF_NONE.TGA", MIP_LEVELS_1, WW3D_FORMAT_UNKNOWN, true);

	ScrollBar = new UISliderGadget(mesh.Peek_Object(), "pull_cargo01", "bar_cargo01", layer, 0, 0);
	Unknown64.Unknown_6870E0(ScrollBar, 1, view, 1, 0);
}


void UITreeMClass::Unknown_54()
{
}


UITreeMClass::~UITreeMClass()
{
	if (OwnsStrings) {
		for (int i = 0; i < Items.Count(); i++) {
			free(Items[i].String);
		}
	}
	for (int j = 0; j < Lines.Count(); j++) {
		UITreeMLineStruct &line = Lines[j];
		line.String->Release_Ref();
	}
	NoneTexture->Release_Ref();
	MinusTexture->Release_Ref();
	PlusTexture->Release_Ref();
}


void UITreeMClass::Add(int id, const char *string)
{
	UITreeMItemStruct *item = Items.Uninitialized_Add();
	item->Id = id;
	if (OwnsStrings) {
		string = strdup(string);
	}
	item->String = (char *)string;
	item->Level = 0;
	item->Flags = 0x10;
	ShownCount++;
	TopCount++;
	Dirty = true;
	if (Active) {
		Update_Scroll_Bar();
	}
}


void UITreeMClass::Add_Child(int id, const char *string, int parent)
{
	int index = Find(parent);
	UITreeMItemStruct &item = Items[index];
	item.Flags |= 1;
	UITreeMItemStruct child;
	if (OwnsStrings) {
		child.String = strdup(string);
	} else {
		child.String = (char *)string;
	}
	child.Id = id;
	child.Level = item.Level + 1;
	child.Flags = 0;
	Items.Insert(index + 1, child);
	Dirty = true;
}


void UITreeMClass::Unknown_58()
{
	if (!Active) {
		InterfaceSortScopeClass sort(this);
		Refresh();
		Update_Scroll_Bar();
		InterfaceClass::Unknown_58();
	}
}


void UITreeMClass::Unknown_5C()
{
	if (Active) {
		InterfaceClass::Unknown_5C();
	}
}


void UITreeMClass::Unknown_60()
{
	Refresh();
}


// NOT EXACT (0.27, inline-budget): the client calls Next_Sibling (0x655DE0) out of line in the inlined Open / Close loops; ours inlines it
bool UITreeMClass::Unknown_68(int key)
{
	switch (key) {
		case 1:
			TopIndex = ScrollBar->Unknown_A8();
			Dirty = true;
			return(true);
		default:
			key -= 2;
			bool plus_minus = (key % 2) == 1;
			int index = key / 2;
			if (index < Lines.Count()) {
				UITreeMItemStruct *item = Shown[index];
				if (!plus_minus && Selected != item) {
					Selected = item;
				}
				if (item->Flags & 1) {
					if (!plus_minus) {
						unsigned int time = WW3D::Get_Frame_Time();
						if (time - ClickTime > 1000000) {
							ClickTime = time;
							ClickIndex = index;
							return(true);
						}
						if (index != ClickIndex) {
							return(true);
						}
					}
					Toggle(item);
					SoundLibraryClass::Play_2D_Sound("TextClick01.wav", 2);
					return(true);
				}
			}
	}
	return(false);
}











































// NOT EXACT (0.80, order-only): the client loads the line offset before Lines.Vector (0x6557D7); the rest is the same
bool UITreeMClass::Refresh()
{
	if (Dirty) {
	Shown.Clear();
	Shown.Resize(Lines.Count());
	UITreeMItemStruct *item = &Items[0];
	int i;
	for (i = 0; i < TopIndex; i++) {
		item = Next_Sibling(item);
		G_ASSERT(item);
	}

	for (i = 0; i < Lines.Count(); i++) {
		UITreeMLineStruct *line = &Lines[i];
		if (item != NULL) {
			char text[256];
			memset(text, ' ', sizeof(text));
			text[item->Level] = '\0';
			strcat(text, item->String);
			if (Opened != NULL && Parent(item) == Opened) {
				line->String->Change_Text(text, 2);
			} else {
				line->String->Change_Text(text, 0);
			}
			if (item->Flags & 1) {
				line->PlusMinus->Unknown_40(true);
				line->Desc->Unknown_40(true);
				if (item->Flags & 2) {
					line->PlusMinus->Unknown_90(MinusTexture);
				} else {
					line->PlusMinus->Unknown_90(PlusTexture);
				}
			} else {
				if (item->Level != 0) {
					line->PlusMinus->Unknown_40(false);
				} else {
					line->PlusMinus->Unknown_90(NoneTexture);
					line->PlusMinus->Unknown_40(true);
				}
				line->Desc->Unknown_40(true);
			}
			Shown[i] = item;
			item = Next_Shown(item);
		} else {
			line->String->Change_Text(" ", 0);
			line->PlusMinus->Unknown_40(false);
			line->Desc->Unknown_40(false);
			Shown[i] = NULL;
		}
	}
	Opened = NULL;
	Unknown0D5 = false;
	Dirty = false;
	return(true);
	}
	return(false);
}


UITreeMItemStruct *UITreeMClass::Prev(UITreeMItemStruct *item)
{
	if (item != NULL && item > &Items[0]) {
		return(item - 1);
	}
	return(NULL);
}


UITreeMItemStruct *UITreeMClass::Next(UITreeMItemStruct *item)
{
	if (item != NULL && item < &Items[Items.Count() - 1]) {
		return(item + 1);
	}
	return(NULL);
}




















UITreeMItemStruct *UITreeMClass::Next_Shown(UITreeMItemStruct *item)
{
	UITreeMItemStruct *next;
	if (item->Flags & 2) {
		next = Next(item);
		G_ASSERT(item->Level + 1 == next->Level);
		return(next);
	}

	next = Next_Sibling(item);
	if (next != NULL) {


		G_ASSERT(item->Level == next->Level);
		return(next);
	}

	UITreeMItemStruct *parent = item;
	next = NULL;
	while ((parent = Parent(parent)) != NULL) {
		next = Next_Sibling(parent);
		if (next != NULL) {
			break;
		}
	}
	return(next);
}





UITreeMItemStruct *UITreeMClass::Parent(UITreeMItemStruct *item)
{
	if (item->Level == 0) {
		return(NULL);
	}
	for (UITreeMItemStruct *prev = Prev(item); prev != NULL; prev = Prev(prev)) {
		if (prev->Level < item->Level) {
			G_ASSERT(prev->Level + 1 == item->Level);
			return(prev);
		}
	}

	G_ASSERT(false);
	return(NULL);
}


UITreeMItemStruct *UITreeMClass::Next_Sibling(UITreeMItemStruct *item)
{
	for (UITreeMItemStruct *next = Next(item); next != NULL; next = Next(next)) {
		if (next->Level == item->Level) {
			return(next);
		}
		if (next->Level < item->Level) {
			break;
		}
	}
	return(NULL);
}







void UITreeMClass::Open(UITreeMItemStruct *item)
{
	if (item->Flags & 1) {
		item->Flags |= 2;
		Opened = item;
		for (UITreeMItemStruct *next = Next(item); next != NULL; next = Next_Sibling(next)) {
			G_ASSERT(item->Level + 1 == next->Level);
			ShownCount++;
			next->Flags |= 0x10;
		}
		Dirty = true;
	}
}






void UITreeMClass::Close(UITreeMItemStruct *item)
{
	if (item->Flags & 1) {
		item->Flags &= ~2;
		for (UITreeMItemStruct *next = Next(item); next != NULL; next = Next_Sibling(next)) {
			G_ASSERT(item->Level + 1 == next->Level);
			Close(next);
			ShownCount--;
			next->Flags &= ~0x10;
		}
		Dirty = true;
	}
}


void UITreeMClass::Toggle(UITreeMItemStruct *item)
{
	if (item->Flags & 2) {
		Close(item);
	} else {
		Open(item);
	}
}










int UITreeMClass::Find(int id)
{
	for (int i = 0; i < Items.Count(); i++) {
		if (Items[i].Id == id) {
			return(i);
		}
	}
	G_ASSERT(false);
	return(0);
}


void UITreeMClass::Update_Scroll_Bar()
{
	if (ScrollBar->Unknown0B0 != 0) {
		ScrollBar->Unknown_675CF0(ScrollBar->Unknown0AC, 0, ScrollBar->Unknown0B4);
	}
	int count = TopCount - 1;
	if (count != ScrollBar->Unknown0AC) {
		ScrollBar->Unknown_675CF0(count, ScrollBar->Unknown0B0, ScrollBar->Unknown0B4);
	}
}

/*
**	Reconstruction notes
**
**	Client 0x654860-0x656FF0 (__FILE__ e:\build\beta\G\app\Client\ui_treem.cpp, 0xB8C240). The object
**	starts with the constructor 0x654860 (0x6547C0-0x654850 are ui_trade.cpp's: its RefCountPtr
**	assignment and its adjustor thunks to 0x650790) and ends with the adjustor thunks 0x656FD0 / 0x656FE0
**	to the deleting destructor, after the guarded <string> initialiser 0x656FA0; 0x656FF0 ('Inv_Sort_Vault',
**	'fa_vault00a.ini') is ui_vault.cpp's. Own functions 0x654860-0x656350, then wwlib vector.h's
**	instances 0x6563D0-0x656F30 for the items (DynamicVectorClass, 16 bytes), the lines
**	(DynamicVectorClass, 12 bytes) and the shown items (VectorClass of pointers); their ID / operator==
**	compare elements by address, so the element structs' operators do.
**
**	No constructor call to the class survives in the client (the constructor and Add / Add_Child have no
**	callers; the tables 0xAF9110 / 0xAF9104 / 0xAF90F8 are stored only by the constructor and destructor).
**	Prev, Next, Parent, Toggle, Find and Update_Scroll_Bar are uncalled out-of-line copies of functions
**	the earlier ones inline; Next_Sibling (0x655DE0) and Close (0x655F70) are also called.
**
**	Forms the bytes settle: the constructor's bone loop is a while (true) with the count kept in i; the
**	destructor releases the textures in the order none, minus, plus; Add_Child builds the new item in a
**	stack temporary before DynamicVectorClass::Insert; Unknown_68 tests 'plus / minus' once as a bool and
**	reads the frame time once; Refresh's early-out is the else of 'if (Dirty)'; Next_Shown walks up with a
**	separate parent variable and returns one result variable.
*/
