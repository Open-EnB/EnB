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
};
// gd_buffgadg.cpp's values of one buff (0x30 bytes, constructor 0x665180; the ship's BuffArray.N)
struct UIBuffInfoStruct	// NAME 1: neutral (D10)
{
	UIBuffInfoStruct();									// 0x665180
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
	void Unknown_666440(bool good);						// 0x666440  // NAME 1 [good]: what it gets (BuffDefStruct::IsGoodBuff)
	char Unknown64[0x80 - 0x64]; BuffDefStruct *Unknown80;	// +0x64 (0x671300's); +0x80 (the asserts' 'buffinfo')
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
**	linked after ui_bio's 0x5608BF and before ui_chat_panel.cpp's constructor 0x561DD0. Not written yet: 0x561030 (the
**	ship's BuffArray, 'BuffArray' through auxdata.h's 0x561C40, to the slots: 0x560D20 for a new buff, 0x560BC0 for one
**	whose type is unchanged, 0x560FA0 otherwise; then each gadget's state, and the mesh's 'frame' hidden without buffs)
**	and its static helper 0x5615C0 (the DeltaBuffClass to a UIBuffInfoStruct; G_ASSERT lines 197 / 205), with the
**	COMDATs it brings: the UIBuffInfoStruct / UIBuffElementStruct destructors 0x561A60 / 0x5619D0 and the vector's
**	0x561B80 / 0x561BD0. After the slots: the adjustor thunks 0x561D60 (-4) / 0x561D70 (-0x38) of the deleting
**	destructor 0x5609E0, the STL locale ids 0x561CB0 / 0x561D00 and the PermStringInitClass static's initialiser
**	0x561D80 / atexit 0x561DB0 (0xBE5B64).
**
**	Forms the client needs: 0x560BC0 passes IsGoodBuff through a local (the client stores it and reloads it as the
**	argument); 0x560D20 reads IsPermanent into a local after the clock and the asset name into one before
**	Create_Render_Obj; 0x560FA0 takes the gadget's model before Peek_Object; 0x561B40 is a member (ret 4, no this).
*/
