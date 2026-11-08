// Earth & Beyond client (reconstruction): app\Client\ui_chat_window_split.cpp (__FILE__ 0xB7D9B0). Notes at the end of the file.
#define __INTRFACE_MEMBERS
#include "ui_chat_window_split.h"
#include "hanim.h"
#include "gd_resize.h"
#include "gadget.h"
#include "option.h"
#include "..\shared\g_assert.h"

extern unsigned int Unknown_BE5F34;		// 0xBE5F34 (gadget.cpp's: the event mask; 0x10 for slot +0x68's keys 0x65 / 0x66)  // NAME 1: neutral (D10)
#define InterfaceMesh Unknown84		// the base's +0x84 (ui_chat_window.cpp's name)

// 0x571F00
UIChatWindowSplitClass::UIChatWindowSplitClass(SClient *client, Unknown67D430Class *history, Unknown65BFD0Class *edit, int kind) :
	ChatWindowInterface(client, history, edit),
	UnknownE8(kind),
	UnknownEC(NULL),
	Unknown118(kind == 2),
	Unknown11C(NULL)
{
	switch (UnknownE8) {
		case 0:
			Unknown11C = "ChatWinSplit_None";
			break;
		case 1:
			Unknown11C = "ChatWinSplit_Vert";
			break;
		case 2:
			Unknown11C = "ChatWinSplit_Horz";
			break;
	}
}














































// 0x572040  // NOT EXACT: functional (CW3): ours inlines the second assign and append and the first assign's copy; the client calls the first assign and both appends (std::string budget, 0.37)
UIChatWindowSplitClass::~UIChatWindowSplitClass()
{
	std::string name(Unknown11C);
	name += "Size";
	IntOptionStruct *option = OptionClass::Instance->Find_Int(name.c_str());
	if (option) {
		option->Value = UnknownF0.Unknown08;
	} else {
		G_ASSERT(false);
	}

	name = Unknown11C;
	name += "Split";
	option = OptionClass::Instance->Find_Int(name.c_str());
	if (option) {
		option->Value = Unknown104.Unknown08;
	} else {
		G_ASSERT(false);
	}
	if (UnknownEC) {
		UnknownEC->Remove_Anim_Combo_Data(UnknownF0.Unknown00);
		UnknownEC->Remove_Anim_Combo_Data(Unknown104.Unknown00);
	}
	delete UnknownEC;
	UnknownEC = NULL;
}

#pragma auto_inline(off)

// 0x572DE0
void UIChatWindowSplitClass::Unknown_58()
{
	ChatWindowInterface::Unknown_58();
}

// 0x572E00
void UIChatWindowSplitClass::Unknown_5C()
{
	ChatWindowInterface::Unknown_5C();
}

// 0x572E20
void UIChatWindowSplitClass::Unknown_60()
{
	if (UnknownF0.Unknown04->Unknown88) {
		Unknown_5732A0((int)UnknownF0.Unknown04->Unknown68.Y, Unknown104.Unknown08);
	}
	if (Unknown104.Unknown04->Unknown88) {
		switch (UnknownE8) {
			case 1:
				Unknown_5732A0(UnknownF0.Unknown08, (int)Unknown104.Unknown04->Unknown68.X);
				break;
			case 2: {
				int split = (int)Unknown104.Unknown04->Unknown68.Y;
				Unknown_5732A0(UnknownF0.Unknown08 - Unknown104.Unknown08 + split, split);
				break;
			}
		}
	}
	ChatWindowInterface::Unknown_60();
}

// 0x572EE0  // NOT EXACT: functional (CW3): the client tests the key mask loaded once before the switch and spends the std::string budget differently (_Tidy out of line, assign inline; 0.41-0.56)
bool UIChatWindowSplitClass::Unknown_68(int key)
{
	unsigned int mask = Unknown_BE5F34;
	switch (key) {
		case 0x66:
			if (mask & 0x10) {
				std::string name(Unknown11C);
				name += "Split";
				int def = OptionClass::Instance->Get_Default_Int(name.c_str());
				if (Unknown104.Unknown08 == def) {
					Unknown_5732A0(UnknownF0.Unknown08, Unknown104.Unknown0C);
				} else {
					Unknown104.Unknown0C = Unknown104.Unknown08;
					Unknown_5732A0(UnknownF0.Unknown08, def);
				}
			}
			break;
		case 0x65:
			if (mask & 0x10) {
				std::string name(Unknown11C);
				name += "Size";
				int def = OptionClass::Instance->Get_Default_Int(name.c_str());
				if (UnknownF0.Unknown08 == def) {
					Unknown_5732A0(UnknownF0.Unknown0C, Unknown104.Unknown08);
				} else {
					UnknownF0.Unknown0C = UnknownF0.Unknown08;
					Unknown_5732A0(def, Unknown104.Unknown08);
				}
			}
			break;
		default:
			return(ChatWindowInterface::Unknown_68(key));
	}
	return(true);
}

// 0x573220
void UIChatWindowSplitClass::Unknown_A8()
{
	ChatWindowInterface::Unknown_A8();
	if (UnknownE4) {
		UnknownF0.Unknown04->Unknown_40(false);
		if (UnknownE8) Unknown104.Unknown04->Unknown_40(false);
	} else {
		UnknownF0.Unknown04->Unknown_40(true);
		if (UnknownE8) Unknown104.Unknown04->Unknown_40(true);
	}
}









































































































































// 0x5732A0
void UIChatWindowSplitClass::Unknown_5732A0(unsigned int size, unsigned int split)
{
	FAIL_IF(!UnknownEC) return;
	if (size >= UnknownF0.Unknown10) size = UnknownF0.Unknown10;
	if (Unknown118) {
		if (split >= size) split = size;
	} else {
		unsigned int max = Unknown104.Unknown10;
		if (split < max) max = split;
		split = max;
	}
	if (UnknownF0.Unknown08 != size || Unknown104.Unknown08 != split) {
		UnknownF0.Unknown08 = size;
		Unknown104.Unknown08 = split;
		UnknownEC->Reset();
		UnknownF0.Unknown00->Set_Frame((float)UnknownF0.Unknown08);
		UnknownEC->Append_Anim_Combo_Data(UnknownF0.Unknown00);
		if (UnknownE8) {
			Unknown104.Unknown00->Set_Frame((float)Unknown104.Unknown08);
			UnknownEC->Append_Anim_Combo_Data(Unknown104.Unknown00);
		}
		InterfaceMesh->Set_Animation(UnknownEC);
		InterfaceMesh->Update_Sub_Object_Transforms();
		Unknown_A4();
	}
}

#pragma auto_inline(on)

// 0x573430 (out of line for the destructor's unwinding; inline in 0x572040)
inline ChatWinSplitResizeStruct::~ChatWinSplitResizeStruct()
{
	delete Unknown00;
	Unknown00 = NULL;
	GadgetClass::Destroy(Unknown04);
	Unknown04 = NULL;
}

/*
**	Reconstruction notes (CW3, 2026-11-08)
**
**	The object is placed by its __FILE__ string 0xB7D9B0 (the destructor's G_ASSERT 'false' on lines 88 and 97) and
**	its position after ui_chat_window.cpp (0x56F420-0x571EDF). Rebuilt: the constructor 0x571F00, the destructor
**	0x572040 (the deleting one 0x572010 and the adjustor thunks 0x5734B0 / 0x5734C0 are the compiler's), the slots
**	+0x58 0x572DE0 and +0x5C 0x572E00 (jumps to the base's, too short for the matcher) and the resize handle's
**	destructor 0x573430; slot +0x60 0x572E20 (the two drags), slot +0xA8 0x573220 and 0x5732A0 (FAIL_IF
**	'!AnimCombo' l.331: the values clamped, the frames set; slot 0x90 of the mesh is Update_Sub_Object_Transforms).
**	Slot +0x68 0x572EE0 is functional (the keys 0x65 / 0x66 with the event mask's 0x10 toggle the size / split
**	between the option's default and the last value). Not rebuilt: the set-up 0x5723B0 (G_ASSERT 'anim' ll.129 /
**	136); the POC (programs/clientpoc/poc_world.cpp) stands in for slot +0x54.
**	The destructor saves the two handles' values as the int options '<prefix>Size' and '<prefix>Split', the
**	prefix being +0x11C ('ChatWinSplit_None' / '_Vert' / '_Horz' by the kind).
*/
