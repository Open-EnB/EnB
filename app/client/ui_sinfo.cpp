// Earth & Beyond client (reconstruction): app\client\ui_sinfo.cpp, the MFD's corner texts (the client's file per its
// assert, 'e:\build\beta\G\app\Client\ui_sinfo.cpp' at 0xB8A2CC, line 92). Notes at the end of the file.
#include "ui_sinfo.h"
#include "..\..\ww3d2\rendobj.h"
#include "fontmanager.h"
#include "..\shared\g_assert.h"
#include <stdio.h>

// 0x62B020
UISInfoClass::UISInfoClass(RenderObjClass *object) :
	Unknown0(0),
	Model(object),
	Corners(3)
{
	Model->Add_Ref();
}

// 0x62B0B0
UISInfoClass::~UISInfoClass()
{
	int count;
	while ((count = Corners.Count()) != 0) {
		delete Corners[count - 1];
		Corners.Delete(count - 1);
	}
	Model->Release_Ref();
}

// 0x62B200
int UISInfoClass::Add_Corner(int corner)
{
	UISInfoCornerClass *item = new UISInfoCornerClass(corner, Model);
	int index = Corners.Count();
	Corners.Add(item);
	return(index);
}







































// 0x62B2E0
UISInfoCornerClass::UISInfoCornerClass(int corner, RenderObjClass *object) :
	Unknown0(0)
{
	const char *name;
	switch (corner) {
		case 1:
			name = "tr";
			break;
		case 2:
			name = "br";
			break;
		case 3:
			name = "bl";
			break;
		default:
			G_ASSERT(false);
		case 0:
			name = "tl";
			break;
	}

	char buffer[32];
	sprintf(buffer, "MESSAGE_MFD_%s_TITLE", name);
	Title = new FontSys(buffer, object, NULL);
	sprintf(buffer, "MESSAGE_MFD_%s_VAL", name);
	Title = new FontSys(buffer, object, NULL);
}

// 0x62B450
UISInfoCornerClass::~UISInfoCornerClass()
{
	Title->Release_Ref();
	Value->Release_Ref();
}

/*
** Reconstruction notes (E&B, D23): the object 0x62B020-0x62B8CF, by its boundaries: the guarded atexit initialiser
** 0x62AFA0 ends the object before it; the guarded std::ctype<unsigned short>::id initialiser 0x62B8B0 (from <string>,
** through fontmanager.h) ends this one; ui_skill_info.cpp's object opens with its PermStringInitClass static 0x62B8D0.
** In address order: UISInfoClass's constructor 0x62B020, destructor 0x62B0B0, Add_Corner 0x62B200, the corner's
** constructor 0x62B2E0 (its default case asserts 'false' on line 92 and falls into 'tl') and destructor 0x62B450,
** then the instances of DynamicVectorClass<UISInfoCornerClass *> and VectorClass<UISInfoCornerClass *> (tables
** 0xAF7B74 / 0xAF7B90; destructors 0x62B1C0 / 0x62B810 / 0x62B880, Clear 0x62B490 / 0x62B630, ID 0x62B4D0 /
** 0x62B670 / 0x62B6C0 / 0x62B7D0, operator== 0x62B500, Resize 0x62B550 / 0x62B6F0). Nothing in the client calls the
** constructor or Add_Corner: the class is dead code. The corner's constructor stores both texts at +0x4 (the client's
** two 'mov [esi+4], eax'), so Value at +0x8 is never set though the destructor releases it: kept as the client has it.
** The FontSys texts are fontmanager.h's (constructor 0x6D47C0, 0x4E8 bytes).
*/
