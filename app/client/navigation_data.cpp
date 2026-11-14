// Earth & Beyond client (reconstruction): app\client\navigation_data.cpp. Notes at the end of the file.
#include "navigation_data.h"
#include "..\packets\pknavdelete.h"
#include "..\shared\g_assert.h"

NavigationDataClass::NavigationDataClass(ClientObjectClass *co) :
	ClientObjectDataClass(co),
	Unknown08(0),
	Unknown0C(false),
	Unknown10(-1),
	Unknown14(false)
{
}

NavigationDataClass::~NavigationDataClass()
{
}



































void NavigationDataClass::Unknown_7BA940(NavigationPacketClass *packet)
{
	FAIL_IF(!packet) return;
	Unknown08 = packet->Signature;
	Unknown0C = packet->PlayerHasVisited;
	Unknown10 = packet->NavType;
	Unknown14 = packet->IsHuge;
}

/*
**	Notes (reconstruction)
**
**	The client's navigation_data.obj: 0x7BA8B0-0x7BA9B0, before pip_scene.cpp's PipSceneClass (0x7BA9C0).
**	__FILE__ is 'e:\build\beta\G\app\client\navigation_data.cpp' (0xB9EFD4): the FAIL_IF '!packet' on
**	line 55 (0x7BA940). The constructor 0x7BA8B0 sets the vtable 0xB065C8 (typeid co_data.cpp's
**	'.PAVNavigationDataClass@@'); its deleting destructor 0x7BA8F0 calls the destructor 0x7BA920. 0x7BA940
**	copies NAVIGATION 153's Signature, PlayerHasVisited, NavType and IsHuge (pknavdelete.h) to +0x08..+0x14.
**	The guarded initialiser 0x7BA990 (guard 0xC416FC, an empty destructor 0x42B320 through atexit) is not
**	reproduced.
*/
