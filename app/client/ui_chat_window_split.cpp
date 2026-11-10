// Earth & Beyond client (reconstruction): app\Client\ui_chat_window_split.cpp (__FILE__ 0xB7D9B0). Notes at the end of the file.
#define __INTRFACE_MEMBERS
#include "ui_chat_window_split.h"
#include "hanim.h"
#include "assetmgr.h"
#include "gd_resize.h"
#include "gadget.h"
#include "option.h"
#include "..\shared\g_assert.h"

extern unsigned int Unknown_BE5F34;		// 0xBE5F34 (gadget.cpp's: the event mask; 0x10 for slot +0x68's keys 0x65 / 0x66)  // NAME 1: neutral (D10)
#define InterfaceMesh Unknown84		// the base's +0x84 (ui_chat_window.cpp's name)
void Unknown_496AE0(RenderObjClass *robj, bool make_geometry_unique);	// 0x496AE0 (ro_funcs.cpp, cdecl: effect.ini's ResetMappers)  // NAME 1 [robj, make_geometry_unique]: what the helpers pass
static const char *Unknown_B7D938[] = { "tsi05f", "tsi05d", "tsi05e" };	// 0xB7D938 (the kinds' models: slot +0x98's names, the animations '<model>.<model>-1' / '-2')  // NAME 1: neutral (D10)

// 0x571F00
UIChatWindowSplitClass::UIChatWindowSplitClass(SClient *client, Unknown67D430Class *history, ChatGadgetClass *edit, int kind) :
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











































// 0x572040  // NOT EXACT: register-only (W2, 0.76): the second Remove_Anim_Combo_Data argument is loaded before the first call (esi); the client loads it into ecx after it
UIChatWindowSplitClass::~UIChatWindowSplitClass()
{
	std::string name; name = Unknown11C;
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
		HAnimComboDataClass *size = UnknownF0.Unknown00;
		HAnimComboDataClass *split = Unknown104.Unknown00;
		UnknownEC->Remove_Anim_Combo_Data(size);
		UnknownEC->Remove_Anim_Combo_Data(split);
	}
	delete UnknownEC;
	UnknownEC = NULL;
	name.size(); name.size(); name.size(); name.size();
}





// 0x5723B0  // NOT EXACT: register-only (W1): the last Release_Ref's vtable goes through edx where the client uses eax; every other byte is the client's
void UIChatWindowSplitClass::Unknown_54()
{
	if (!Unknown_98(Unknown_B7D938[UnknownE8], NULL)) return;
	ChatWindowInterface::Unknown_54();

	UnknownEC = new HAnimComboClass;
	Unknown104.Unknown00 = new HAnimComboDataClass(true);
	UnknownF0.Unknown00 = new HAnimComboDataClass(true);

	std::string name(Unknown_B7D938[UnknownE8]);
	name += '.';
	name += Unknown_B7D938[UnknownE8];
	name += "-1";
	HAnimClass *anim = WW3DAssetManager::Get_Instance()->Get_HAnim(name.c_str());
	G_ASSERT(anim);
	UnknownF0.Unknown00->Give_HAnim(anim);



	name[name.length() - 1] = '2';
	anim = WW3DAssetManager::Get_Instance()->Get_HAnim(name.c_str());
	G_ASSERT(anim);
	Unknown104.Unknown00->Give_HAnim(anim);

	RefCountPtr<RenderObjClass> robj;
	robj = Create_Get(InterfaceMesh->Get_Sub_Object_By_Name("GIZMO_HORIZ01"));
	Unknown_496AE0(robj.Peek(), 0);
	UnknownF0.Unknown04 = new ResizeGadgetClass(robj);
	GadgetClass::Set_Tooltip(UnknownF0.Unknown04, "UI_CHAT_WINDOW_SPLIT", "Resize");
	Unknown64.Unknown_6870E0(UnknownF0.Unknown04, 0x65, View, 1, 1);

	if (UnknownE8) {
		robj = Create_Get(InterfaceMesh->Get_Sub_Object_By_Name("GIZMO_HORIZ"));
		Unknown_496AE0(robj.Peek(), 0);
	} else {
		robj = NULL;
	}
	Unknown104.Unknown04 = new ResizeGadgetClass(robj);
	GadgetClass::Set_Tooltip(Unknown104.Unknown04, "UI_CHAT_WINDOW_SPLIT", Unknown11C);
	Unknown64.Unknown_6870E0(Unknown104.Unknown04, 0x66, View, 1, 1);

	UnknownF0.Unknown00->Build_Active_Pivot_Map();
	Unknown104.Unknown00->Build_Active_Pivot_Map();
	UnknownF0.Unknown10 = (UnknownF0.Unknown00->Peek_HAnim()->Get_Num_Frames() > 360) ? 360 : UnknownF0.Unknown00->Peek_HAnim()->Get_Num_Frames();
	Unknown104.Unknown10 = Unknown104.Unknown00->Peek_HAnim()->Get_Num_Frames();
	Unknown_5732A0(0, 0);
	UnknownF0.Unknown04->Unknown_66BDA0();
	Unknown104.Unknown04->Unknown_66BDA0();

	switch (UnknownE8) {
		case 0:
			UnknownF0.Unknown04->Set_Unknown78(0, UnknownF0.Unknown10 - 1);
			break;
		case 1:
			UnknownF0.Unknown04->Set_Unknown78(0, UnknownF0.Unknown10 - 1);
			Unknown104.Unknown04->Set_Unknown70(Unknown104.Unknown10 * 0.1, 0);
			Unknown104.Unknown04->Set_Unknown78(Unknown104.Unknown10 * 0.9, 0);
			Unknown104.Unknown0C = Unknown104.Unknown10 * 0.9;
			break;
		case 2:
			UnknownF0.Unknown04->Set_Unknown78(0, UnknownF0.Unknown10 - 1);
			Unknown104.Unknown04->Set_Unknown78(0, Unknown104.Unknown10 - 1);
			break;
	}

	std::string option;
	option = Unknown11C;
	option += "Size";
	int size = OptionClass::Instance->Get_Int(option.c_str());
	option = Unknown11C;
	option += "Split";
	Unknown_5732A0(size, OptionClass::Instance->Get_Int(option.c_str()));
	Unknown_A0((UnknownE8 != 0) + 1);
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

// 0x572EE0
bool UIChatWindowSplitClass::Unknown_68(int key)
{
	unsigned int mask = Unknown_BE5F34;
	switch (key) {
		case 0x65:
			if (mask & 0x10) {
				std::string name; name = Unknown11C;
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
		case 0x66:
			if (mask & 0x10) {
				std::string name; name.size(); name = Unknown11C;
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
**	Slot +0x68 0x572EE0 (the keys 0x65 / 0x66 with the event mask's 0x10 toggle the size / split between the
**	option's default and the last value; W1: the case strings default-constructed then assigned, case 0x65 first
**	for its EH state 0, and one dead size() in case 0x66 that sends its _Tidy out of line as the client's). The set-up 0x5723B0 (slot +0x54, G_ASSERT 'anim' ll.129 /
**	136; W1, 2026-11-08) is functional with a register-only residue: the kind's model by slot +0x98 with the names
**	0xB7D938, the base's set-up, the animations '<model>.<model>-1' / '-2' given to the size / split handles, the
**	resize gadgets on 'GIZMO_HORIZ01' and (split kinds) 'GIZMO_HORIZ' with their ranges, then the saved options.
**	Its forms: the anim string by its constructor (EH state after assign), the option string default-constructed
**	then assigned (EH state 9 before the inline assign), the frame clamp as a macro-style ternary (the call twice),
**	the ranges through inline setters on the gadget (gd_resize.h; Vector2::Set adds +0x78 to the pointer).
**	The destructor saves the two handles' values as the int options '<prefix>Size' and '<prefix>Split', the
**	prefix being +0x11C ('ChatWinSplit_None' / '_Vert' / '_Horz' by the kind).
*/
