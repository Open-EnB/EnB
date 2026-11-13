// Earth & Beyond client (reconstruction): app\client\ui_hot_keys.cpp, the space client's hot keys window. Notes at the end
// of the file.
#define __INTRFACE_MEMBERS	// intrface.h's members (+0x40 the client, +0x4C the key list)
#include <string>
#include "ui_hot_keys.h"
#include "sclient.h"
#include "client.h"
#include "control_keys_manager.h"
#include "targeting.h"
#include "target_find.h"
#include "..\packets\pkmove.h"
#include "..\packets\pkaction.h"
#include "..\shared\g_assert.h"

// What InterfaceClass's +0x40 points to, as far as this file calls it (as pdaint.cpp declares it)
class LineDisplayClientClass  // NAME 1: neutral (D10)
{
public:
	virtual void Unknown_00(); virtual void Unknown_04(); virtual void Unknown_08(); virtual void Unknown_0C();
	virtual void Unknown_10(); virtual void Unknown_14();
	virtual SClient *As_Space_Client();					// +0x18 (as ui_manu.cpp's)  // NAME 3: the FAIL_IF's in ui_manu.cpp
};

inline unsigned Unknown_Avatar_ID(LineDisplayClientClass *client) { return(((ClientClass *)client)->AvatarID); }	// NAME 1: neutral (D10), as ui_cpit.cpp's

// @0x59C6F0
UIHotKeysClass::UIHotKeysClass(SClient *client) :
	InterfaceClass((LineDisplayClientClass *)client)
{
}

// @0x59C760
UIHotKeysClass::~UIHotKeysClass()
{
}

// @0x59C790
void UIHotKeysClass::Unknown_70(ControlKeysManager *manager)
{
	manager->Add_Keys(&Unknown4C, "Autofollow", (void *)0, 0);
	manager->Add_Keys(&Unknown4C, "Fire All Weapons", (void *)1, 0);
	manager->Add_Keys(&Unknown4C, "Next Context Target", (void *)2, 0);
	manager->Add_Keys(&Unknown4C, "Prev Context Target", (void *)3, 0);
	manager->Add_Keys(&Unknown4C, "Next Target", (void *)4, 0);
	manager->Add_Keys(&Unknown4C, "Prev Target", (void *)5, 0);
	manager->Add_Keys(&Unknown4C, "Target Group 1", (void *)6, 0);
	manager->Add_Keys(&Unknown4C, "Target Group 2", (void *)7, 0);
	manager->Add_Keys(&Unknown4C, "Target Group 3", (void *)8, 0);
	manager->Add_Keys(&Unknown4C, "Target Group 4", (void *)9, 0);
	manager->Add_Keys(&Unknown4C, "Target Group 5", (void *)10, 0);
	manager->Add_Keys(&Unknown4C, "Target Near Player", (void *)11, 0);
	manager->Add_Keys(&Unknown4C, "Target Near Attacker", (void *)12, 0);
	manager->Add_Keys(&Unknown4C, "Target Near Enemy", (void *)13, 0);
	manager->Add_Keys(&Unknown4C, "Target Near Friendly", (void *)14, 0);
	manager->Add_Keys(&Unknown4C, "Target Near Navigation", (void *)15, 0);
	manager->Add_Keys(&Unknown4C, "Target Near Object", (void *)16, 0);
	manager->Add_Keys(&Unknown4C, "Target Self", (void *)17, 0);
}

// @0x59D180
void UIHotKeysClass::Unknown_74(ControlKeysManager *manager)
{
	manager->Remove_Keys(&Unknown4C, "Autofollow", 0);
	manager->Remove_Keys(&Unknown4C, "Fire All Weapons", 0);
	manager->Remove_Keys(&Unknown4C, "Next Context Target", 0);
	manager->Remove_Keys(&Unknown4C, "Prev Context Target", 0);
	manager->Remove_Keys(&Unknown4C, "Next Target", 0);
	manager->Remove_Keys(&Unknown4C, "Prev Target", 0);
	manager->Remove_Keys(&Unknown4C, "Target Group 1", 0);
	manager->Remove_Keys(&Unknown4C, "Target Group 2", 0);
	manager->Remove_Keys(&Unknown4C, "Target Group 3", 0);
	manager->Remove_Keys(&Unknown4C, "Target Group 4", 0);
	manager->Remove_Keys(&Unknown4C, "Target Group 5", 0);
	manager->Remove_Keys(&Unknown4C, "Target Near Player", 0);
	manager->Remove_Keys(&Unknown4C, "Target Near Attacker", 0);
	manager->Remove_Keys(&Unknown4C, "Target Near Enemy", 0);
	manager->Remove_Keys(&Unknown4C, "Target Near Friendly", 0);
	manager->Remove_Keys(&Unknown4C, "Target Near Navigation", 0);
	manager->Remove_Keys(&Unknown4C, "Target Near Object", 0);
	manager->Remove_Keys(&Unknown4C, "Target Self", 0);
}

// @0x59DB10
bool UIHotKeysClass::Unknown_68(int key)
{




















	FAIL_IF(!Client) return(false);
	SClient *sclient = Client->As_Space_Client();
	FAIL_IF(!sclient) return(false);
	TargetingClass *targeting = sclient->Targeting;

	FAIL_IF(!targeting) return(false);
	if (sclient->CurrentView != (UIViewClass *)sclient->MainView) {
		return(false);
	}
	switch (key) {
		case 0:
			((ClientClass *)Client)->Send_Packet(&MovePacketClass(Unknown_Avatar_ID(Client), 12));
			return(true);
		case 1:
			((ClientClass *)Client)->Send_Packet(&ActionPacketClass(Unknown_Avatar_ID(Client), 9, Unknown_Avatar_ID(Client), 0));
			return(true);
		case 2:
			targeting->Select_Next_Target_Filtered();
			return(true);
		case 3:
			targeting->Select_Prev_Target_Filtered();
			return(true);
		case 4:
			targeting->Select_Next_Target();
			return(true);
		case 5:
			targeting->Select_Prev_Target();
			return(true);
		case 6:
		case 7:
		case 8:
		case 9:
		case 10:
			targeting->Select_Group_Member(key - 6);
			return(true);
		case 11:
			targeting->Select_Nearest_Avatar();
			return(true);
		case 12:
			targeting->Select_Nearest_Attacking();
			return(true);
		case 13:
			targeting->Select_Nearest_100();
			return(true);
		case 14:
			targeting->Select_Nearest_200();
			return(true);
		case 15:
			targeting->Select_Nearest_Navigation();
			return(true);
		case 16:
			targeting->Select_Nearest_Neutral();
			return(true);
		case 17:
			targeting->Select_Player();
			return(true);
	}
	return(false);
}

/*
**	Notes (O15, 2026-11-13)
**
**	The client's object 0x59C6F0-0x59DE9F: the constructor 0x59C6F0, the deleting destructor 0x59C730, the destructor
**	0x59C760, the slots +0x70 0x59C790 / +0x74 0x59D180 / +0x68 0x59DB10 (FAIL_IFs l.106 / 108 / 111, a jump table), the
**	adjustor thunks 0x59DE80 (-4) / 0x59DE90 (-0x38). Every key name is passed as a const char * (the implicit std::string):
**	the inline budget then leaves the first ten (add) / eleven (remove) assigns out of line, as the client does; explicit
**	std::string temporaries (pdaint.cpp's form) leave all eighteen out of line. sclient.cpp still declares the class itself
**	(O13 owns sclient.cpp today): its local declaration can give way to this header.
*/
