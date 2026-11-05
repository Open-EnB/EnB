// Earth & Beyond client (reconstruction): app\client\ui_faction.cpp, the character info window's faction tab (the client's
// file per its asserts, 'e:\build\beta\G\app\Client\ui_faction.cpp'). Notes at the end of the file.
#include <string>
#include "sclient.h"
#include "rendobj.h"
#include "texture.h"
#include "..\..\wwlib\ref_ptr.h"
#include "gadget.h"
#include "gd_robj.h"
#include "unknown_675120.h"
#include <stdio.h>
#include "unknown_674260.h"		// the gadget 0x674260 makes (0x60 bytes)
#include "assetmgr.h"
#include "gd_slider.h"
#include "fontmanager.h"
#include "local_interface_sort.h"
#include "..\shared\unknown_debug.h"
#include "cfaction_database.h"
#include "ui_faction.h"

#define InterfaceMesh Mesh		// the FAIL_IFs' name for the base's +0x84 (0x58D400 l.118; ui_attrb_skills.cpp's l.121): Unknown5BB320Class::Mesh  // NAME 3: the FAIL_IF's
extern FontManagerClass *Unknown_BE67C4;			// NAME 1 [Unknown_BE67C4]: 0xBE67C4

// What InterfaceClass's +0x40 points to, as far as this file calls it (as ui_cinfo.cpp declares it)
class LineDisplayClientClass  // NAME 1: neutral (D10)
{
public:
	virtual void Unknown_00(); virtual void Unknown_04(); virtual void Unknown_08(); virtual void Unknown_0C();
	virtual void Unknown_10(); virtual void Unknown_14();
	virtual SClient *As_Space_Client();					// +0x18 (as ui_manu.cpp's)  // NAME 3: the FAIL_IF's in ui_manu.cpp
};

// ui_faction_panel.cpp's panel (200 bytes, constructor 0x58EB50), as far as this file calls it
class Unknown58EB50Class : public Unknown5BB320Class	// NAME 1: neutral (D10)
{
public:
	Unknown58EB50Class(SClient *client);				// 0x58EB50  // NAME 1 [client]: as UIBioClass's
	void Unknown_58EFC0(const std::string &faction);	// 0x58EFC0 (a faction button's +0x64)  // NAME 1 [faction]: neutral
	char UnknownAC[200 - 0xAC];							// +0xAC
};

class Unknown669570Class;	// gd_gauge.cpp's gauge (declared before the button's constructor)
// The faction buttons (0x7C bytes, tables 0xAF1820; 0x58D400 makes six): the faction's name and two gauges
class Unknown58E210Class : public ButtonGadgetClass  // NAME 1: neutral (D10)
{
public:
	Unknown58E210Class(UIMeshClass *mesh, int index);	// 0x58E210 (FAIL_IFs '!uimesh' / '!robj' / '!sobj': 'HIGHLIGHT%02d', 'LOVE_BAR%02d', 'HATE_BAR%02d', 'NAME%02d_TSI30A')  // NAME 1 [mesh, index]: neutral
	virtual ~Unknown58E210Class();						// 0x58E620 (deleting 0x58E5F0)
	virtual void Unknown_40(bool on);					// +0x40 0x58E6F0 (the gauges hidden when off, then the base's)  // NAME 1 [on]: GadgetClass's

	std::string Unknown64;								// +0x64 (the faction: what 0x58EFC0 takes)
	Unknown669570Class *Unknown74;						// +0x74 ('LOVE_BAR%02d')
	Unknown669570Class *Unknown78;						// +0x78 ('HATE_BAR%02d')
};

// 0x58D080
UIFactionClass::UIFactionClass(SClient *client) :
	Unknown5BB320Class(new UIMeshClass("tsi30a", 2), client),
	UnknownBC(NULL),
	UnknownC0(0),
	UnknownC4(-1)
{
	CFactionDatabaseClass::Init();
	memset(UnknownC8, 0, sizeof(UnknownC8));


	Unknown_BE67BC->Load("fa_tsi30a.ini");
}

// 0x58D1E0
UIFactionClass::~UIFactionClass()
{
	Unknown_BE67C4->Remove(Mesh->Peek_Mesh_Object());
	Unknown_BE67BC->Unload("fa_tsi30a.ini");
	if (UnknownBC) {
		UnknownBC->Unknown_66E460(RefCountPtr<RenderObjClass>(), 0, 0);
	}
	UnknownAC = NULL;
	CFactionDatabaseClass::Shutdown();
}

// 0x58D3C0
void UIFactionClass::Unknown_7C(UIViewClass *view, bool arg)
{
	UnknownAC->Unknown_7C(view, false);
	Unknown5BB320Class::Unknown_7C(view, arg);
}



























// 0x58D400
void UIFactionClass::Unknown_54()
{
	FAIL_IF(!InterfaceMesh) return;
	RefCountPtr<RenderObjClass> mesh = Create_Peek(InterfaceMesh->Peek_Mesh_Object());
	FAIL_IF(!mesh) return;

	UnknownAC = Create_NEW(new Unknown58EB50Class(Client->As_Space_Client()));
	UnknownAC->Unknown_54();

	Unknown_BE67C4->Add(InterfaceMesh->Peek_Mesh_Object());
	UnknownB8 = Create_Peek(Unknown_BE67C4->Get_Style("AFFILIATED_TSI30A", InterfaceMesh->Peek_Mesh_Object()));

	Unknown58E210Class *button;
	for (int i = 0; i < 6; i++) {
		button = new Unknown58E210Class(InterfaceMesh, i);
		UnknownC8[i] = button;
		Unknown64.Unknown_6870E0(button, i + 1, View, 1, 0);
		Unknown_5BB4F0(button);
	}



	StandardBttnTexture = Create_Peek(button->Unknown_66C4A0());
	G_ASSERT(StandardBttnTexture);


	SelectedBttnTexture = Create_Get(WW3DAssetManager::Get_Instance()->Get_Texture("I_blank_SEL.tga", MIP_LEVELS_1, WW3D_FORMAT_UNKNOWN, true));
	G_ASSERT(SelectedBttnTexture);

	RefCountPtr<RenderObjClass> frame = Create_Get(mesh->Get_Sub_Object_By_Name("new_frame"));
	GadgetClass *gadget = new Unknown_674260Class(frame);
	Unknown64.Unknown_6870E0(gadget, 9, View, 1, 0);

	UnknownBC = new GadgetSlider(false);
	UnknownBC->Unknown_66E460(Create_Peek(InterfaceMesh->Peek_Object()), 0, 0);
	Unknown64.Unknown_6870E0(UnknownBC, 8, View, 1, 0);

	Unknown_10();
}




// 0x58D9F0
void UIFactionClass::Unknown_58()
{
	if (!Active) {
		InterfaceSortScopeClass sort(this);
		Unknown_58DC20(true);
		Unknown5BB320Class::Unknown_58();
	}
}

// 0x58DA80
void UIFactionClass::Unknown_5C()
{
	if (Active) {
		UnknownAC->Unknown_5C();
		Unknown5BB320Class::Unknown_5C();
	}
}

// 0x58DAB0
void UIFactionClass::Unknown_60()
{
	SClient *sclient = Client->As_Space_Client();
	FAIL_IF(!sclient) return;

	DeltaPlayerClass *ad_player = sclient->AuxData;
	FAIL_IF(!ad_player) return;

	Unknown_58DC20(false);
	Unknown5BB320Class::Unknown_60();
}

// 0x58DB30
bool UIFactionClass::Unknown_68(int key)
{
	if (key >= 1 && key <= 6) {
		int index = key - 1;
		if (index == UnknownC4) {
			Unknown_Select(-1);
		} else {
			Unknown_Select(index);
		}
		return(true);
	}
	switch (key) {
		case 8:
			{
				int value = UnknownBC->Value;
				if (UnknownC0 != value) {
					UnknownC0 = value;
					Unknown_58DC20(true);
				}
			}
			break;
		case 9:
			return(true);
	}
	return(false);
}























































































// 0x58E1C0
void UIFactionClass::Unknown_Select(int index)
{
	if (UnknownC4 != -1) {
		UnknownAC->Unknown_5C();
	}
	UnknownC4 = index;
	if (index != -1) {
		UnknownAC->Unknown_58EFC0(UnknownC8[index]->Unknown64);
	}
}


// gd_gauge.cpp's gauge (0x6C bytes, constructor 0x669570), as far as this file makes and sets it
class Unknown669570Class : public RObjGadgetClass	// NAME 1: neutral (D10)
{
public:
	Unknown669570Class(RenderObjClass *robj, const char *name);	// 0x669570  // NAME 1 [robj, name]: neutral
	virtual void Unknown_A4();							// +0xA4
	virtual float Unknown_A8(float value);				// +0xA8 (the gauge's value; 0x58E740 passes the reaction)  // NAME 1 [value]: neutral
	char Unknown60[0x6C - 0x60];						// +0x60
};

// 0x58E210
Unknown58E210Class::Unknown58E210Class(UIMeshClass *uimesh, int index) :
	ButtonGadgetClass((RenderObjClass *)NULL, 5, 5),
	Unknown74(NULL),
	Unknown78(NULL)
{
	FAIL_IF(!uimesh) return;

	char name[32];
	sprintf(name, "HIGHLIGHT%02d", index);

	RefCountPtr<RenderObjClass> robj = Create_Peek(uimesh->Peek_Object());
	FAIL_IF(!robj) return;

	RefCountPtr<RenderObjClass> sobj = Create_Get(robj->Get_Sub_Object_By_Name(name));
	FAIL_IF(!sobj) return;
	Set_Render_Obj(sobj.Peek());

	sprintf(name, "LOVE_BAR%02d", index);
	Unknown74 = new Unknown669570Class(robj.Peek(), name);
	Unknown74->Unknown_A8(0);
	sprintf(name, "HATE_BAR%02d", index);
	Unknown78 = new Unknown669570Class(robj.Peek(), name);
	Unknown78->Unknown_A8(0);

	sprintf(name, "NAME%02d_TSI30A", index);
	RefCountPtr<FontSys> font = Create_Peek(Unknown_BE67C4->Get_Style(name, robj.Peek()));
	Unknown_98(font.Peek());
}










// 0x58E620
Unknown58E210Class::~Unknown58E210Class()
{
	GadgetClass::Destroy(Unknown74);
	Unknown74 = NULL;
	GadgetClass::Destroy(Unknown78);
	Unknown78 = NULL;
}

// 0x58E6F0
void Unknown58E210Class::Unknown_40(bool on)
{
	Unknown74->Unknown_8C(!on);
	Unknown78->Unknown_8C(!on);
	ButtonGadgetClass::Unknown_40(on);
}

/*
**	Notes (TB1, 2026-11-05)
**
**	The client's object 0x58D080-0x58EB4F: the constructor 0x58D080 first (its strings 'tsi30a' 0xB7F0B4 and
**	'fa_tsi30a.ini' 0xB7F0A4 come before the __FILE__ string 0xB7F120), after ui_experience.cpp's 0x58CF60;
**	ui_faction_panel.cpp's constructor 0x58EB50 follows the adjustor thunks 0x58EB30 (-4) / 0x58EB40 (-0x38) of the
**	deleting destructor 0x58D1B0. 0x5738B0 (ui_cinfo.cpp) makes it, the character info window's third tab.
*/
