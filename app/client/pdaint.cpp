// Earth & Beyond client (reconstruction): app\client\pdaint.cpp, the PDA at the lower left of the space view (the
// client's __FILE__ 'e:\build\beta\G\app\Client\pdaint.cpp', 0x693140-0x69602F by its asserts and its table 0xAFD148).
// Notes at the end.
#define __INTRFACE_MEMBERS	// intrface.h's members (the constructor 0x540D30, +0x4C's key list, +0x58's list)
#include "pdaint.h"
#include <string>
#include "sclient.h"
#include "gadget.h"
#include "unknown_675120.h"
#include "gd_uiback.h"		// UIBackGadget (0x674260)
#include "rendobj.h"
#include "assetmgr.h"
#include "control_keys_manager.h"
#include "ui_chat_panel.h"
#include "ui_cinfo.h"
#include "ui_starmap.h"
#include "ui_ping.h"
#include "..\audio\soundlib.h"
#include "..\..\wwaudio\AudibleSound.h"
#include "..\shared\g_assert.h"
#include "ui_experience.h"	// UIExperienceClass (+0xB4)
void Unknown_496AE0(RenderObjClass *robj, bool make_geometry_unique);	// 0x496AE0 (ro_funcs.cpp, cdecl)  // NAME 1 [robj, make_geometry_unique]: as eff_o.cpp's

// What InterfaceClass's +0x40 points to, as far as this file calls it (as ui_cinfo.cpp declares it)
class LineDisplayClientClass  // NAME 1: neutral (D10)
{
public:
	virtual void Unknown_00(); virtual void Unknown_04(); virtual void Unknown_08(); virtual void Unknown_0C();
	virtual void Unknown_10(); virtual void Unknown_14();
	virtual SClient *As_Space_Client();					// +0x18 (as ui_manu.cpp's)  // NAME 3: the FAIL_IF's in ui_manu.cpp
	virtual void Unknown_1C(); virtual void Unknown_20(); virtual void Unknown_24();
	virtual int Unknown_28();							// +0x28 (what the experience display shows)
};

// bridge_int.cpp's class (SClient +0x128C), as far as this file uses it
class BridgeInterfaceClass	// NAME 3: class-from-file (D24) for bridge_int.cpp
{
public:
	void Unknown_53C670(int mode);						// 0x53C670 (0 closed, 1 cargo, 2 character, 3 star map)  // NAME 1 [mode]: neutral
	bool Is_Busy() { return(Unknown1C != Unknown18); }	// (inline: 0x693CA0 and 0x695780 compare +0x1C with +0x18)  // NAME 1: neutral

	char Unknown00[0x18];
	int Unknown18;										// +0x18
	int Unknown1C;										// +0x1C
};

// 0xB90A38: the mesh's buttons, gadgets 1..4 (tooltips from section 'UI_CPIT')
static const char *PDAButtonNames[PDAInterfaceClass::NUM_BUTTONS] = {	// NAME 1: neutral (D10)
	"MAP_BUT",
	"CHARACTER_BUT",
	"SHIP_BUT",
	"OPTIONS_BUT"
};

// 0x693140
PDAInterfaceClass::PDAInterfaceClass(SClient *client) :
	InterfaceClass((LineDisplayClientClass *)client),
	Experience(NULL),
	UnknownBC(true)
{
	for (int i = 0; i < NUM_BUTTONS; i++) {
		Buttons[i] = NULL;
	}
}

// 0x693230
void PDAInterfaceClass::Unknown_54()
{
	InterfaceMesh = Create_Get(WW3DAssetManager::Get_Instance()->Create_Render_Obj("TSI14A"));
	Unknown58.Unknown_686CC0(InterfaceMesh.Peek(), NULL);

	SClient *client = Get_Client()->As_Space_Client();
	Equipment = Create_NEW(new UIPdaEquipClass(client));
	Equipment->Unknown_54();
	Cargo = Create_NEW(new UIPdaCargoClass(client, this));
	Cargo->Unknown_54();
	Vault = Create_NEW(new Unknown656FF0Class(Get_Client()->As_Space_Client()));
	Vault->Unknown_54();
	Character = Create_NEW(new UICinfoClass(client));
	Character->Unknown_54();
	Ping = Create_NEW(new UIPingClass((LineDisplayClientClass *)Get_Client()->As_Space_Client()));
	Ping->Unknown_54();
	Starmap = Create_NEW(new UIStarmapClass(client));
	Starmap->Unknown_54();

	for (int i = 0; i < NUM_BUTTONS; i++) {
		RenderObjClass *robj = InterfaceMesh->Get_Sub_Object_By_Name(PDAButtonNames[i]);
		Unknown_496AE0(robj, 0);
		Buttons[i] = new ButtonGadgetClass(robj, 0x55, 3);
		robj->Release_Ref();
		Unknown64.Unknown_6870E0(Buttons[i], i + 1, NULL, 1, 0);
		GadgetClass::Set_Tooltip(Buttons[i], "UI_CPIT", PDAButtonNames[i]);
	}

	Unknown64.Unknown_6870E0(new UIBackGadget(InterfaceMesh), 0xE, View, 1, 0);
	Experience = new UIExperienceClass(InterfaceMesh.Peek(), this);
	Unknown_10();
}




// 0x693950
PDAInterfaceClass::~PDAInterfaceClass()
{
	if (InterfaceMesh) {
		Unknown58.Unknown_686DD0(InterfaceMesh.Peek());
	}
	if (Experience) {
		Experience->Unknown_58CF60();
		delete Experience;
		Experience = NULL;
	}
	Equipment = NULL;
	Cargo = NULL;
	Vault = NULL;
	Character = NULL;
	Starmap = NULL;
	Ping = NULL;
}





















// 0x693CA0
void PDAInterfaceClass::Unknown_60()
{
	if (Active) {
		FAIL_IF(!Get_Client()) return;
		SClient *client = Get_Client()->As_Space_Client();
		if (Experience) {
			if ((int)Experience->Get_Object() != Get_Client()->Unknown_28()) {
				Experience->Unknown_08((ClientObjectClass *)Get_Client()->Unknown_28());
			}
			Experience->Unknown_10();
		}
		if (UnknownB0 && !client->BridgeInterface->Is_Busy()) {
			UnknownB0->Unknown_58();
			if (UnknownB0.Peek() == Cargo.Peek()) {
				UnknownB0 = NULL;
				if (OpenedCargoInterface.Peek() == Equipment.Peek()) {
					Unknown_695780(1);
					return;
				}
				if (OpenedCargoInterface.Peek() == Vault.Peek()) {
					Unknown_695780(3);
					return;
				}























				G_ASSERT(!OpenedCargoInterface);
				OpenedCargoInterface = NULL;
				return;
			}
			UnknownB0 = NULL;
		}
	}
}

// the chat panel's window (0x694AA0) put away when a PDA window opens (inlined in each of 0x693E70's cases)
inline void PDA_Close_Chat_Window(ChatPanelInterface *chat)	// NAME 1: neutral (D10)
{
	if (chat->Unknown_694AA0()) {
		RefCountPtr<UIChatPanelWindowClass> window = chat->Unknown_694AA0();
		window->Unknown_5C();
		window->Unknown98 = 1;
	}
}

// 0x693E70 (NOT EXACT: shape (the chat window block, the sound tail and the cases' order: 0.17))
bool PDAInterfaceClass::Unknown_68(int key)
{
	if (!UnknownBC) {
		return(true);
	}
	ChatPanelInterface *chat = Get_Client()->As_Space_Client()->ChatPanel.Peek();
	SClient *client = Get_Client()->As_Space_Client();
	const char *sound;
	switch (key) {
	case 1:
		if (!Unknown_695C50(4)) {
			Unknown_695780(4);
			PDA_Close_Chat_Window(chat);
			sound = "SR_scifielectronic40.wav";
			goto play;
		}
		Unknown_695A00(4, true);
		break;
	case 2:
		if (!Unknown_695C50(2)) {
			Unknown_695780(2);
			PDA_Close_Chat_Window(chat);
			sound = "SR_scifielectronic40.wav";
			goto play;
		}
		Unknown_695A00(2, true);
		break;
	case 3:
	case 8:
		if (!Unknown_695C50(0)) {
			Unknown_695780(0);
			PDA_Close_Chat_Window(chat);
			sound = "SR_scifielectronic40.wav";
			goto play;
		}
		Unknown_695A00(0, true);
		Unknown_695A00(1, true);
		Unknown_695A00(3, true);
		break;
	case 4:
		if (chat) {
			if (!chat->Unknown_694AA0()) {
				chat->Unknown_565F30(1);
			} else {
				RefCountPtr<UIChatPanelWindowClass> window = chat->Unknown_694AA0();
				window->Unknown_5C();
				window->Unknown98 = 1;
			}
		}
		break;
	case 6:
		if (!Ping->Is_Active()) {
			Ping->Unknown_58();
		} else {
			Ping->Unknown_5C();
		}
		break;
	case 7:
		{
			RefCountPtr<InterfaceClass> window = Unknown_6956C0(1);
			if (window && window->Is_Active()) {
				window = NULL;
				Unknown_695A00(0, true);
				Unknown_695A00(1, true);
				break;
			}
		}
		if (!Unknown_695780(0)) {
			Unknown_695780(1);
		} else {
			OpenedCargoInterface = Equipment;
		}
		PDA_Close_Chat_Window(chat);
		break;
	case 9:
		if (!client->Unknown_744B60()) {
			return(true);
		}
		{
			RefCountPtr<InterfaceClass> window = Unknown_6956C0(3);
			if (window && window->Is_Active()) {
				window = NULL;
				Unknown_695A00(0, true);
				Unknown_695A00(3, true);
				break;
			}
		}
		if (!Unknown_695780(0)) {
			Unknown_695780(3);
		} else {
			OpenedCargoInterface = Vault;
		}
		PDA_Close_Chat_Window(chat);
		break;
	case 10:
		if (!Character->Is_Active()) {
			Character->Unknown_5742B0(0);
			Unknown_695780(2);
			PDA_Close_Chat_Window(chat);
		} else if (Character->Unknown_574220(0)) {
			Unknown_695A00(2, true);
		} else {
			Character->Unknown_5742B0(0);
		}
		break;
	case 11:
		if (!Character->Is_Active()) {
			Character->Unknown_5742B0(1);
			Unknown_695780(2);
			PDA_Close_Chat_Window(chat);
		} else if (Character->Unknown_574220(1)) {
			Unknown_695A00(2, true);
		} else {
			Character->Unknown_5742B0(1);
		}
		break;
	case 12:
		if (!Starmap->Is_Active()) {
			Starmap->Unknown_7C8C20(0);
			Unknown_695780(4);
		} else if (Starmap->Unknown_7C8BA0(0)) {
			Unknown_695A00(4, true);
			break;
		} else {
			Starmap->Unknown_7C8C20(0);
		}
		PDA_Close_Chat_Window(chat);
		break;
	case 13:
		if (!Starmap->Is_Active()) {
			Starmap->Unknown_7C8C20(1);
			Unknown_695780(4);
		} else if (Starmap->Unknown_7C8BA0(1)) {
			Unknown_695A00(4, true);
			break;
		} else {
			Starmap->Unknown_7C8C20(1);
		}
		PDA_Close_Chat_Window(chat);
		break;
	default:
		return(false);
	}
	sound = "SR_scifielectronic42.wav";
play:
	AudibleSoundClass *audible = SoundLibraryClass::Play_2D_Sound(sound, 0);
	if (audible) {
		audible->Set_Pan(0.5f);
		audible->Play(true);
		audible->Release_Ref();
	}
	return(true);
}

// 0x694AD0
void PDAInterfaceClass::Unknown_58()
{
	InterfaceClass::Unknown_58();
}

// 0x694AF0
void PDAInterfaceClass::Unknown_5C()
{
	if (Active) {
		InterfaceClass::Unknown_5C();
	}
}

// 0x694B10
void PDAInterfaceClass::Unknown_7C(UIViewClass *view, bool arg)
{
	if (View != view) {
		Equipment->Unknown_7C(view, false);
		Cargo->Unknown_7C(view, false);
		Vault->Unknown_7C(view, false);
		Character->Unknown_7C(view, false);
		Ping->Unknown_7C(view, false);
		Starmap->Unknown_7C(view, false);
		OpenedCargoInterface = NULL;
		InterfaceClass::Unknown_7C(view, arg);
	}
}
































































// 0x694BE0
void PDAInterfaceClass::Unknown_70(ControlKeysManager *manager)
{
	FAIL_IF(!manager) return;
	manager->Add_Keys(&Unknown4C, std::string("Toggle Ping"), (void *)6, 0);
	manager->Add_Keys(&Unknown4C, std::string("Equipment"), (void *)7, 0);
	manager->Add_Keys(&Unknown4C, std::string("Cargo"), (void *)8, 0);
	manager->Add_Keys(&Unknown4C, std::string("Vault"), (void *)9, 0);
	manager->Add_Keys(&Unknown4C, std::string("Character"), (void *)10, 0);
	manager->Add_Keys(&Unknown4C, std::string("Missions"), (void *)11, 0);
	manager->Add_Keys(&Unknown4C, std::string("Nav Map"), (void *)12, 0);
	manager->Add_Keys(&Unknown4C, std::string("Galaxy Map"), (void *)13, 0);
}

// 0x695110
void PDAInterfaceClass::Unknown_74(ControlKeysManager *manager)
{

	FAIL_IF(!manager) return;
	manager->Remove_Keys(&Unknown4C, std::string("Toggle Ping"), 0);
	manager->Remove_Keys(&Unknown4C, std::string("Equipment"), 0);
	manager->Remove_Keys(&Unknown4C, std::string("Cargo"), 0);
	manager->Remove_Keys(&Unknown4C, std::string("Vault"), 0);
	manager->Remove_Keys(&Unknown4C, std::string("Character"), 0);
	manager->Remove_Keys(&Unknown4C, std::string("Missions"), 0);
	manager->Remove_Keys(&Unknown4C, std::string("Nav Map"), 0);
	manager->Remove_Keys(&Unknown4C, std::string("Galaxy Map"), 0);
}

// 0x695630
RefCountPtr<UIPdaCargoClass> PDAInterfaceClass::Unknown_695630()
{
	if (Cargo->Is_Active()) {
		return(Cargo);
	}
	return(NULL);
}

// 0x695680
RefCountPtr<UIPdaCargoClass> PDAInterfaceClass::Unknown_695680()
{
	if (Cargo) {
		return(Cargo);
	}
	return(NULL);
}














// 0x6956C0
RefCountPtr<InterfaceClass> PDAInterfaceClass::Unknown_6956C0(int index)
{
	switch (index) {
	case 0: return(Cargo);
	case 1: return(Equipment);
	case 3: return(Vault);
	case 2: return(Character);
	case 4: return(Starmap);
	}
	G_ASSERT(false);
	return(NULL);
}


// 0x695780 (NOT EXACT: shape (the client's FAIL_IF returns share one tail: 0.15))
bool PDAInterfaceClass::Unknown_695780(int index)
{
	FAIL_IF(!Get_Client()) return(false);
	SClient *sc = Get_Client()->As_Space_Client();
	FAIL_IF(!sc) return(false);
	if (sc->BridgeInterface && sc->BridgeInterface->Is_Busy()) {
		return(false);
	}
	UnknownB0 = Unknown_6956C0(index);
	if (!UnknownB0) {
		return(false);
	}
	if (UnknownB0->Is_Active()) {
		UnknownB0 = NULL;
		return(false);
	}
	for (int i = 0; i < 5; i++) {
		if (i != index && ((index != 1 && index != 3) || i != 0)) {
			Unknown_695A00(i, false);
		}
	}
	switch (index) {
	case 0:
		sc->Unknown_744560(0);
		sc->BridgeInterface->Unknown_53C670(1);
		return(true);
	case 1:
		sc->Unknown_744560(0xF);
		return(true);
	case 2:
		sc->Unknown_744560(2);
		sc->BridgeInterface->Unknown_53C670(2);
		return(true);
	case 3:
		sc->Unknown_744560(0x11);
		return(true);
	case 4:
		sc->Unknown_744560(0x13);
		sc->BridgeInterface->Unknown_53C670(3);
		return(true);
	}
	return(true);
}





























// 0x695A00 (NOT EXACT: shape (cases 1 and 3 jump to the common return-true tail: 0.36))
bool PDAInterfaceClass::Unknown_695A00(int index, bool arg)
{
	RefCountPtr<InterfaceClass> window = Unknown_6956C0(index);
	if (!window || !window->Is_Active()) {
		return(false);
	}
	window->Unknown_5C();
	switch (index) {
	case 0:
		OpenedCargoInterface = NULL;
		Get_Client()->As_Space_Client()->Unknown_744560(1);
		Unknown_695A00(1, false);
		Unknown_695A00(3, false);
		break;
	case 1:
		Get_Client()->As_Space_Client()->Unknown_744560(0x10);
		return(true);
	case 2:
		Get_Client()->As_Space_Client()->Unknown_744560(3);
		break;
	case 3:
		Get_Client()->As_Space_Client()->Unknown_744560(0x12);
		return(true);
	case 4:
		Get_Client()->As_Space_Client()->Unknown_744560(0x14);
		break;
	}
	if (arg) {
		FAIL_IF(!Get_Client()) return(false);
		if (Get_Client()->As_Space_Client()->BridgeInterface) {
			Get_Client()->As_Space_Client()->BridgeInterface->Unknown_53C670(0);
		}
	}
	return(true);
}

// 0x695C50
bool PDAInterfaceClass::Unknown_695C50(int index)
{
	RefCountPtr<InterfaceClass> window = Unknown_6956C0(index);
	if (window) {
		return(window->Is_Active());
	}
	return(false);
}

// 0x695CE0
bool PDAInterfaceClass::Unknown_695CE0()
{
	if (Cargo && Cargo->Is_Active()) {
		return(true);
	}
	if (Equipment && Equipment->Is_Active()) {
		return(true);
	}
	if (Vault && Vault->Is_Active()) {
		return(true);
	}
	if (Character && Character->Is_Active()) {
		return(true);
	}
	if (Starmap && Starmap->Is_Active()) {
		return(true);
	}
	return(UnknownB0.Peek() ? true : false);
}

// 0x695D70
bool PDAInterfaceClass::Unknown_695D70(int index)
{
	if (Unknown_695C50(index)) {
		Unknown_695A00(index, true);
		return(false);
	}
	Unknown_695780(index);
	return(true);
}

// 0x695DC0
void PDAInterfaceClass::Unknown_695DC0(bool on)
{
	Cargo->Unknown_5D0A20(on);
	if (on) {
		Unknown_695780(0);
	} else {
		Unknown_695A00(0, true);
	}
}















// 0x695E10
void PDAInterfaceClass::Unknown_695E10(int type, int *num_items, int *num_slots)
{
	FAIL_IF(!num_items) return;
	FAIL_IF(!num_slots) return;
	*num_items = 0;
	*num_slots = 0;
	int items = 0;
	int slots = 0;
	Equipment->Unknown_5A0450(type, &items, &slots);
	*num_items += items;
	*num_slots += slots;
	Cargo->Unknown_5A0450(type, &items, &slots);
	*num_items += items;
	*num_slots += slots;
	Vault->Unknown_5A0450(type, &items, &slots);
	*num_items += items;
	*num_slots += slots;
}

// 0x695F40
void PDAInterfaceClass::Unknown_695F40()
{
	if (Starmap->Is_Active()) {
		Unknown_695A00(4, true);
	}
	if (Character->Is_Active()) {
		Unknown_695A00(2, true);
	}
	if (Vault->Is_Active()) {
		Unknown_695A00(3, true);
	}
	if (Cargo->Is_Active()) {
		Unknown_695A00(0, true);
	}
	if (Equipment->Is_Active()) {
		Unknown_695A00(1, true);
	}
}

// 0x695FE0 (NOT EXACT: block order (the client puts the off branch's own return first: 0.59))
void PDAInterfaceClass::Unknown_695FE0(bool on)
{
	if (UnknownBC != on) {
		UnknownBC = on;
		if (!on) {
			Unknown_695F40();
			Unknown_5C();
			return;
		}
		Unknown_58();
	}
}

/*
**	Reconstruction notes (PD1, 2026-11-08).
**
**	The object: the client's __FILE__ 'e:\build\beta\G\app\Client\pdaint.cpp' (0xB90AA8) in the asserts of
**	0x693CA0 (lines 146, 189), 0x694BE0 / 0x695110 (458, 473), 0x6956C0 (525), 0x695780 (533, 535), 0x695A00 (632)
**	and 0x695E10 (710, 711); the constructor 0x693140 sets the table 0xAFD148 whose slots are this file's functions.
**	SClient::Init news it (0xC0 bytes, +0x1284).
**
**	Output in the range besides these: the deleting destructor 0x693200, ChatPanelInterface's inline 0x694AA0 and
**	the Create_NEW instantiations 0x696030 / 0x6960A0 / 0x696110 / 0x696180 / 0x6961F0 / 0x696260.
**
**	The windows' slots called here are InterfaceClass's (+0x54 set-up, +0x58 on, +0x5C off, +0x7C onto a view);
**	the equipment, cargo and vault windows and the experience display are not in the tree (pdaint.h's views).
*/
