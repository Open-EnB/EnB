// Earth & Beyond client (reconstruction): the proof-of-concept client's stand-ins on the way into the world (D179). NOT THE CLIENT'S CODE.
/*
**	The objects SClient::Init news whose classes the tree does not have yet. The generated stand-ins of their
**	constructors leave operator new's memory as it is and no vtable, so the first virtual call or member read
**	crashes: these give each the real size's zeroed memory (and InterfaceClass's own table for the windows: no
**	window, no keys). Delete one here when its real class lands in app/.
*/

#define __INTRFACE_MEMBERS
#include <string.h>
#include "..\..\stubs\stub_log.h"
#include "..\..\app\client\sclient.h"

// ---------------------------------------------------------------- ui_hotkeys.cpp (0x59C6F0, table 0xAF20D0): 0x84 bytes, no members
class UIHotKeysClass : public InterfaceClass
{
public:
	UIHotKeysClass(SClient *client);
};
UIHotKeysClass::UIHotKeysClass(SClient *client) : InterfaceClass((LineDisplayClientClass *)client)
{
	STUB_CALLED("POC UIHotKeysClass: an empty InterfaceClass");
}

// ---------------------------------------------------------------- pdaint.cpp (0x693140, table 0xAFD148): 0xC0 bytes
PDAInterfaceClass::PDAInterfaceClass(SClient *client) : InterfaceClass((LineDisplayClientClass *)client)
{
	STUB_CALLED("POC PDAInterfaceClass: an empty InterfaceClass");
	memset(Unknown84, 0, sizeof(Unknown84));
	UnknownA8 = NULL;
	memset(UnknownAC, 0, sizeof(UnknownAC));
}

// ---------------------------------------------------------------- bridge (0x53B040): 0xC4 bytes, zero
class BridgeInterfaceClass
{
public:
	BridgeInterfaceClass(SClient *client);
	~BridgeInterfaceClass();
	char Unknown00[0xC4];
};
BridgeInterfaceClass::BridgeInterfaceClass(SClient *client)
{
	STUB_CALLED("POC BridgeInterfaceClass: zero");
	memset(this, 0, sizeof(*this));
}
BridgeInterfaceClass::~BridgeInterfaceClass()
{
}

// ---------------------------------------------------------------- camera_manager.cpp (0x798D20, 0x792320-0x79BB80's modules): 0x4C bytes, zero
class CameraManagerClass
{
public:
	CameraManagerClass(SClient *client);
	char Unknown00[0x4C];
};
CameraManagerClass::CameraManagerClass(SClient *client)
{
	STUB_CALLED("POC CameraManagerClass: zero");
	memset(this, 0, sizeof(*this));
}
