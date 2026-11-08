// Earth & Beyond client (reconstruction): app\Client\ui_chat_window_split.cpp (__FILE__ 0xB7D9B0). Notes at the end of the file.
#define __INTRFACE_MEMBERS
#include "ui_chat_window_split.h"
#include "hanim.h"
#include "gd_resize.h"
#include "gadget.h"
#include "option.h"
#include "..\shared\g_assert.h"

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
**	destructor 0x573430. Not rebuilt: the set-up 0x5723B0 (G_ASSERT 'anim' ll.129 / 136), 0x572E20, 0x572EE0,
**	0x573220, 0x5732A0: the POC (programs/clientpoc/poc_world.cpp) stands in for slots +0x54, +0x60, +0x68, +0xA8.
**	The destructor saves the two handles' values as the int options '<prefix>Size' and '<prefix>Split', the
**	prefix being +0x11C ('ChatWinSplit_None' / '_Vert' / '_Horz' by the kind).
*/
