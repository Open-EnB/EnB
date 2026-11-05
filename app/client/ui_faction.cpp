// Earth & Beyond client (reconstruction): app\client\ui_faction.cpp, the character info window's faction tab (the client's
// file per its asserts, 'e:\build\beta\G\app\Client\ui_faction.cpp'). Notes at the end of the file.
#include "sclient.h"
#include "rendobj.h"
#include "texture.h"
#include "..\..\wwlib\ref_ptr.h"
#include "gadget.h"
#include "gd_slider.h"
#include "fontmanager.h"
#include "local_interface_sort.h"
#include "..\shared\unknown_debug.h"
#include "cfaction_database.h"
#include "ui_faction.h"

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
	void Unknown_58EFC0(void *faction);					// 0x58EFC0 (a faction button's +0x64)  // NAME 1 [faction]: neutral
	char UnknownAC[200 - 0xAC];							// +0xAC
};

// The faction buttons (0x7C bytes, constructor 0x58E210), as far as this file reads them
class Unknown58E210Class  // NAME 1: neutral (D10)
{
public:
	char Unknown00[0x64];								// +0x00
	char Unknown64[0x7C - 0x64];						// +0x64 (what 0x58EFC0 takes)
};

inline void UIFactionClass::Unknown_Select(int index)
{
	if (UnknownC4 != -1) {
		UnknownAC->Unknown_5C();
	}
	UnknownC4 = index;
	if (index != -1) {
		UnknownAC->Unknown_58EFC0(UnknownC8[index]->Unknown64);
	}
}

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

/*
**	Notes (TB1, 2026-11-05)
**
**	The client's object 0x58D080-0x58EB4F: the constructor 0x58D080 first (its strings 'tsi30a' 0xB7F0B4 and
**	'fa_tsi30a.ini' 0xB7F0A4 come before the __FILE__ string 0xB7F120), after ui_experience.cpp's 0x58CF60;
**	ui_faction_panel.cpp's constructor 0x58EB50 follows the adjustor thunks 0x58EB30 (-4) / 0x58EB40 (-0x38) of the
**	deleting destructor 0x58D1B0. 0x5738B0 (ui_cinfo.cpp) makes it, the character info window's third tab.
*/
