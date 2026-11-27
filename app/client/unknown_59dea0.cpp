// Earth & Beyond client (reconstruction): app\client\unknown_59dea0.cpp, the founders window ('founders00a').
// No string names the file (no asserts): the name is an interim one (D137). Notes at the end.
#include <string.h>
#include "unknown_59dea0.h"
#include "client.h"
#include "gadget.h"
#include "unknown_675120.h"
#include "rendobj.h"
#include "..\audio\soundlib.h"
#include "..\packets\pkclientchat.h"
#include "..\shared\auth_product.h"
void Unknown_496AE0(RenderObjClass *robj, bool make_geometry_unique);	// 0x496AE0 (ro_funcs.cpp, cdecl: effect.ini's ResetMappers)  // NAME 1 [robj, make_geometry_unique]: what the helpers pass

// The buttons' models (0xB802A0)
static const char *Unknown_B802A0[2] = { "yes", "no" };	// NAME 1: neutral (D10)

// @0x59DEA0
Unknown59DEA0Class::Unknown59DEA0Class(SClient *client) :
	ChatPanelUIBase(client)
{
	memset(Buttons, 0, sizeof(Buttons));
}

// @0x59DF20
Unknown59DEA0Class::~Unknown59DEA0Class()
{
	InterfaceClass::Unknown_5C();
}

// @0x59DFA0
void Unknown59DEA0Class::Unknown_54()
{
	if (Unknown_98("founders00a", "fa_founders00a.ini")) {
		for (int i = 0; i < 2; i++) {
			RefCountPtr<RenderObjClass> robj = Create_Get(InterfaceMesh->Get_Sub_Object_By_Name(Unknown_B802A0[i]));
			if (robj) {
				Unknown_496AE0(robj.Peek(), 0);
				Buttons[i] = new ButtonGadgetClass(robj.Peek(), 0x55, 3);
				Unknown64.Unknown_6870E0(Buttons[i], i + 1, View, 1, 0);
			}
		}
		Unknown_10();
	}
}

// @0x59E110
bool Unknown59DEA0Class::Unknown_68(int key)
{
	switch (key) {
		case 1:
			SoundLibraryClass::Play_2D_Sound("Roll_Select02b.wav", 2);
			((ClientClass *)Client)->Send_Packet(&ClientChatPacketClass(((ClientClass *)Client)->Get_Avatar_ID(), "/founder enable", 3, NULL));
			Unknown_5C();
			break;
		case 2:
			SoundLibraryClass::Play_2D_Sound("Roll_Select02b.wav", 2);
			Unknown_5C();
			break;
		default:
			return(false);
	}
	Closed = 1;
	return(true);
}

/*
** Reconstruction notes
** - The object: 0x59DEA0-0x59E1CF, then <string>'s guarded static initialiser 0x59E220 (guard 0xC416FC, as every
**   object that includes it) and the deleting destructor's thunks for the other tables (0x59E250 / 0x59E260); after
**   ui_hot_keys.cpp's (to 0x59DE90). Its data: the button names 0xB802A0 ('yes', 'no'), 'founders00a' 0xB802AC,
**   'fa_founders00a.ini' 0xB802BC, '/founder enable' 0xB802D4.
** - Nothing in the client constructs it (no caller of 0x59DEA0 or of its thunk): a window left in.
** - Its tables are its neighbours' (ui_option_quit.cpp's pattern): its own slots +0x00, +0x54 and +0x68; the destructor
**   calls InterfaceClass::Unknown_5C unconditionally (ui_option_quit.cpp's tests Active first).
** - The matcher's window for 0x59E110 runs to 0x59E220 (the initialiser's start is not a verified one): its 192 bytes
**   are the client's.
*/
