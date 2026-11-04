// Earth & Beyond client (reconstruction): app\client\ui_buff.cpp, the buff display (the client's file per its
// asserts, 'e:\build\beta\G\app\Client\ui_buff.cpp'). Notes at the end of the file.
#include <stdio.h>
#include <string>
#include <vector>
#include "sclient.h"
#include "client.h"
#include "rendobj.h"
#include "assetmgr.h"
#include "gadget.h"
#include "unknown_675120.h"
#include "..\shared\unknown_debug.h"
#include "..\shared\duration.h"
#include "..\audio\soundlib.h"
#include "buff_database.h"
#include "fontdatabase.h"
#include "ui_buff.h"
#include "..\shared\auxdata\permstr.h"	// (its PermStringInitClass static: 0xBE5B64, initialised by 0x561D80)
void Unknown_496AE0(RenderObjClass *robj, bool make_geometry_unique);	// 0x496AE0 (ro_funcs.cpp; as avatar.cpp declares it)
// gd_buffgadg.cpp's values of one buff element (0x30 bytes; the ship's BuffArray.N.Elements.N); the members the aux fields'
struct UIBuffElementStruct	// NAME 1: neutral (D10)
{
	std::string SourceEntity; std::string SourceObject;	// +0x00 / +0x10  // NAME 2: the aux fields'
	int Magnitude; bool IsActive; DurationClass ExpirationTime;	// +0x20 / +0x24 / +0x28  // NAME 2: the aux fields'
	UIBuffElementStruct(const std::string &source_entity, const std::string &source_object, int magnitude, const DurationClass &expiration_time, bool is_active);	// 0x665940  // NAME 1 [source_entity, source_object, magnitude, expiration_time, is_active]: the aux fields 0x5615C0 passes
};
// gd_buffgadg.cpp's values of one buff (0x30 bytes, constructor 0x665180; the ship's BuffArray.N)
struct UIBuffInfoStruct	// NAME 1: neutral (D10)
{
	UIBuffInfoStruct(); void Unknown_6651C0(const std::string &name); void Unknown_665340(bool permanent); void Unknown_665360(const DurationClass &time); void Unknown_665390(const UIBuffElementStruct &element); void Unknown_665A10();	// 0x665180; 0x6651C0 / 0x665340 / 0x665360 the ScrubTypeName / IsPermanent / BuffRemovalTime 0x5615C0 passes, 0x665390 an element added, 0x665A10 after the elements  // NAME 1 [name, permanent, time, element]: what 0x5615C0 passes
	std::string ScrubTypeName; bool IsPermanent; DurationClass BuffRemovalTime;	// +0x00 / +0x10 / +0x18  // NAME 2: the aux fields'
	std::vector<UIBuffElementStruct> Elements;			// +0x20  // NAME 3: gd_buffgadg.cpp's assert 'index >= Elements.size()'
};
// gd_buffgadg.cpp's gadget (0xD8 bytes, constructor 0x665A40, table 0xAF9E70), as far as this file calls it: over
// gadget.cpp's 0x80-byte gadget (constructor 0x671300) over the button gadget
class UIBuffGadgetClass : public ButtonGadgetClass	// NAME 1: neutral (D10)
{
public:
	UIBuffGadgetClass(LineDisplayClientClass *client, BuffDefStruct *buffinfo, const UIBuffInfoStruct &info, RenderObjClass *robj, int unknown, const LayerClass &layer);	// 0x665A40  // NAME 1 [client, info, robj, unknown, layer]: what 0x560D20 passes; NAME 3 [buffinfo]: the asserts'
	bool Unknown_6661F0(const UIBuffInfoStruct &info, const DurationClass &now);	// 0x6661F0 (the values taken: true when they changed)  // NAME 1 [info, now]: what 0x560BC0 passes
	void Unknown_666440(bool good); bool Unknown_666470(); void Unknown_6664E0(bool on); void Unknown_671630(); virtual void Unknown_A4(); virtual void Unknown_A8(); virtual void Unknown_AC();	// 0x666440; 0x666470 (true: the buff is over); 0x6664E0; 0x671630 (0x671300's, after slot +0xAC); slots +0xA4 / +0xA8 0x671650 / 0x671670, +0xAC 0x665F20  // NAME 1 [good, on]: neutral
	bool Unknown64; char Unknown65[0x80 - 0x65]; BuffDefStruct *Unknown80;	// +0x64 (0x671300's: held); +0x80 (the asserts' 'buffinfo')
	bool Unknown84; char Unknown85[0xD8 - 0x85];		// +0x84 / +0x85
};
// 0x5608D0
UIBuffClass::UIBuffClass(UIViewClass *view, const LayerClass &layer) : Unknown5BB320Class(new UIMeshClass("TSI02A", 1), view, layer), UnknownAC(0)
{
	((int *)Unknown70)[1] = 2;	// InterfaceClass's +0x74 (typed only for intrface.cpp)
	Unknown_BE67BC->Load("fc_buff.ini");
	for (int i = 0; i < MAX_BUFFS; i++) BuffTable[i] = NULL;
}

// 0x560A10
UIBuffClass::~UIBuffClass()
{
	Unknown_5C();
	for (int i = 0; i < MAX_BUFFS; i++) Unknown_560FA0(i);	// (inlined)
	Unknown_BE67BC->Unload("fc_buff.ini");
}

// 0x560B30
void UIBuffClass::Unknown_58()
{
	if (UnknownAC == 0) {
		Unknown5BB320Class::Unknown_58();
		UnknownAC = 1;
	}
}

// 0x560B60
void UIBuffClass::Unknown_5C()
{
	if (UnknownAC == 1) {
		Unknown5BB320Class::Unknown_5C();
		UnknownAC = 0;
	}
}

// 0x560B90
void UIBuffClass::Unknown_60()
{
	Unknown_561030();
	Unknown5BB320Class::Unknown_60();
}

// 0x560BC0
void UIBuffClass::Unknown_560BC0(const UIBuffInfoStruct &info, int slot)
{
	FAIL_IF(!BuffTable[slot]) return;

	FAIL_IF(!Client) return;

	UIBuffGadgetClass *buff_gadget = BuffTable[slot];
	FAIL_IF(!buff_gadget) return;

	BuffDefStruct *buffinfo = buff_gadget->Unknown80;
	FAIL_IF(!buffinfo) return;
	if (buff_gadget->Unknown_6661F0(info, DurationClass((unsigned int)((ClientClass *)Client)->Unknown10BC))) {
		bool good = buffinfo->IsGoodBuff;
		buff_gadget->Unknown_666440(good);
		Unknown_561B40(good);
	}
}






// 0x560D20
void UIBuffClass::Unknown_560D20(const char *type, const UIBuffInfoStruct &info, int slot)
{


	FAIL_IF(BuffTable[slot]) return;
	FAIL_IF(!Client) return;

	// a buff that has run out is not shown
	DurationClass now((unsigned int)((ClientClass *)Client)->Unknown10BC);
	bool permanent = info.IsPermanent;
	if (!permanent && info.BuffRemovalTime < now) {
		return;
	}








	BuffDefStruct *buffinfo = BuffDatabaseClass::Instance->Find(type);
	FAIL_IF(!buffinfo) return;

	InterfaceClass::Unknown_5C();

	const char *asset = buffinfo->Asset;
	RenderObjClass *buff_obj = WW3DAssetManager::Get_Instance()->Create_Render_Obj(asset);
	FAIL_IF(!buff_obj) {
	} else {
		Unknown_496AE0(buff_obj, false);
		char name[32];
		sprintf(name, "buff%02d", slot);
		Mesh->Peek_Object()->Add_Sub_Object_To_Bone(buff_obj, name);
		BuffTable[slot] = new UIBuffGadgetClass(Client, buffinfo, info, buff_obj, 0, Layer);
		Unknown64.Unknown_6870E0(BuffTable[slot], slot, View, 1, 1);
		Unknown_5BB4F0(BuffTable[slot]);
		buff_obj->Release_Ref();
		BuffTable[slot]->Unknown_666440(buffinfo->IsGoodBuff);
		Unknown_561B40(buffinfo->IsGoodBuff);
	}
	InterfaceClass::Unknown_58();
}

// 0x560FA0
void UIBuffClass::Unknown_560FA0(int slot)
{
	if (BuffTable[slot]) {
		InterfaceClass::Unknown_5C();
		RenderObjClass *robj = BuffTable[slot]->Peek_Render_Obj();
		Mesh->Peek_Object()->Remove_Sub_Object(robj);
		Unknown64.Unknown_687150(BuffTable[slot]);
		GadgetClass::Destroy(BuffTable[slot]);
		BuffTable[slot] = NULL;
		InterfaceClass::Unknown_58();
	}
}











#include <string.h>
#include "..\shared\auxdata\auxdata.h"
#include "..\shared\auxdata\delta_string.h"
#include "aux_roots.h"				// (after auxdata.h, as ui_cpit.cpp)
#include "co.h"
#include "tooltip.h"
void Unknown_42ECF0(float *x, float *y);				// 0x42ECF0 (the pointer's position, 0..1; as gadget.cpp declares it)  // NAME 1 [x, y]: neutral
template<class C> inline C *Unknown_Find_Changed(DeltaCompoundClass *root, const char *name) { NameClass n(name); C *t = Unknown_Find_Field<C>(root, n); if (t && t->Unknown70 > 1) return(t); return(NULL); }	// NAME 1: neutral (D10), as ui_avatar_cust.cpp's
enum { MAX_BUFF_ELEMENTS = 5 };	// NAME 3: the assert's (the ship's BuffArray.N.Elements count)

// 0x5615C0: the ship's DeltaBuffClass to a UIBuffInfoStruct (static: called directly)
// NOT EXACT: inline-budget (the client calls string::assign out of line for the first two strings, inlines the third)
static void Unknown_5615C0(UIBuffInfoStruct &info, DeltaBuffClass *buff)	// NAME 1 [Unknown_5615C0, info, buff]: neutral (D10)
{
	info.Unknown_6651C0(std::string(buff->ScrubTypeName.Get()));
	info.Unknown_665340(buff->IsPermanent.Value);
	info.Unknown_665360(DurationClass(buff->BuffRemovalTime.Value));
	unsigned int buff_element_count = buff->Elements.Children.size();
	G_ASSERT(buff_element_count == MAX_BUFF_ELEMENTS);
	for (unsigned int i = 0; i < buff_element_count; i++) {
		DeltaElementClass *element = (DeltaElementClass *)buff->Elements.Children[i];
		std::string source_entity(element->SourceEntity.Get());
		std::string source_object(element->SourceObject.Get());
		int magnitude = element->Magnitude.Value;


		G_ASSERT(source_object.empty() == (magnitude == 0));
		if (!source_object.empty()) {
			info.Unknown_665390(UIBuffElementStruct(source_entity, source_object, magnitude, element->ExpirationTime.Value, element->IsActive.Value));
		}
	}
	info.Unknown_665A10();
}

// 0x561030: the ship's BuffArray to the slots (a new buff made, a changed one updated, an emptied one taken off), then
// each gadget's state, and the mesh's 'frame' hidden without buffs (slot +0x60)
// NOT EXACT: inline-budget (the client calls auxdata.h's find out of line, 0x561C40, and ~vector 0x561B80 on the new-buff path)
void UIBuffClass::Unknown_561030()
{
	ClientObjectClass *avatar = ((ClientClass *)Client)->Get_Avatar();
	if (avatar && avatar->AuxData) {
		float x, y;
		Unknown_42ECF0(&x, &y);
		DeltaBuffsClass *buffs = Unknown_Find_Changed<DeltaBuffsClass>((DeltaCompoundClass *)avatar->AuxData, "BuffArray");
		bool shown = false;
		if (buffs) {
			for (int slot = 0; slot < MAX_BUFFS; slot++) {
				DeltaBuffClass *buff = (DeltaBuffClass *)buffs->Children[slot];
				if (buff->Unknown70 > 1) {
					const char *type = buff->BuffType.Get();
					if (strcmp(type, "") == 0) {
						Unknown_560FA0(slot);
					} else if (BuffTable[slot] && BuffTable[slot]->Unknown80 && strcmp(BuffTable[slot]->Unknown80->Type, type) == 0) {
						UIBuffInfoStruct info;
						Unknown_5615C0(info, buff);
						Unknown_560BC0(info, slot);
					} else {
						Unknown_560FA0(slot);
						UIBuffInfoStruct info;
						Unknown_5615C0(info, buff);
						Unknown_560D20(type, info, slot);
					}
				}
			}
		}
		for (int i = 0; i < MAX_BUFFS; i++) {
			if (BuffTable[i]) {
				if (BuffTable[i]->Unknown_666470()) {
					Unknown_560FA0(i);
				} else if (!BuffTable[i]->Unknown84 && BuffTable[i]->Unknown64) {
					View->Unknown30->Unknown_6AB8C0();
					BuffTable[i]->Unknown_6664E0(true);
				} else if (BuffTable[i]->Unknown84 && !BuffTable[i]->Unknown_30(x, y)) {
					BuffTable[i]->Unknown_6664E0(false);
				}
				if (BuffTable[i]) {
					BuffTable[i]->Unknown_AC();
					BuffTable[i]->Unknown_671630();
				}
				shown = true;
			}
		}
		RefCountPtr<RenderObjClass> frame = Create_Get(Mesh->Peek_Object()->Get_Sub_Object_By_Name("frame"));
		if (frame) {
			frame->Set_Hidden(!shown);
		}
	}
}

// 0x561B40
void UIBuffClass::Unknown_561B40(bool good)
{
	if (good) {
		SoundLibraryClass::Play_2D_Sound("click1c.wav", 2);
	} else {
		SoundLibraryClass::Play_2D_Sound("cost.wav", 2);
	}
}

/*
**	Notes (BUFF, 2026-11-04)
**
**	The client's object 0x5608D0-0x561DC0 (ui_buff.obj: its __FILE__ 'e:\build\beta\G\app\Client\ui_buff.cpp' at 0xB7C18C),
**	linked after ui_bio's 0x5608BF and before ui_chat_panel.cpp's constructor 0x561DD0. 0x561030 and its static helper
**	0x5615C0 are NOT EXACT (inline budget); the static is defined first (its asserts are on lines 197 / 205, and defined
**	after 0x561030 the UIBuffInfoStruct destructor 0x561A60 does not match). The COMDATs they bring: the destructors
**	0x561A60 / 0x5619D0 and the vector's _Destroy 0x561BD0 match; auxdata.h's find 0x561C40 and ~vector 0x561B80 are
**	inlined in ours. After the slots: the adjustor thunks 0x561D60 (-4) / 0x561D70 (-0x38) of the deleting
**	destructor 0x5609E0, the STL locale ids 0x561CB0 / 0x561D00 and the PermStringInitClass static's initialiser
**	0x561D80 / atexit 0x561DB0 (0xBE5B64).
**
**	Forms the client needs: 0x560BC0 passes IsGoodBuff through a local (the client stores it and reloads it as the
**	argument); 0x560D20 reads IsPermanent into a local after the clock and the asset name into one before
**	Create_Render_Obj; 0x560FA0 takes the gadget's model before Peek_Object; 0x561B40 is a member (ret 4, no this).
*/
