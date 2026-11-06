// Earth & Beyond client (reconstruction): app\client\ui_talkt_space.cpp (the client's __FILE__ 0xB8BDFC). Notes at the end.
#include <string>
#include "ui_talkt_space.h"
#include "avatar_pip.h"
#include "avatar.h"
#include "..\shared\avatar_desc.h"
#include "..\shared\g_assert.h"

// The client at InterfaceClass +0x40, as ui_chat_panel.cpp declares it
class LineDisplayClientClass  // NAME 1: neutral (D10)
{
public:
	virtual void Unknown_00(); virtual void Unknown_04(); virtual void Unknown_08(); virtual void Unknown_0C();
	virtual void Unknown_10(); virtual void Unknown_14();
	virtual SClient *As_Space_Client();					// +0x18 (SClient's returns itself)  // NAME 3: ui_manu.cpp's FAIL_IF's
};

// 0x64F310
UITalkTreeSpaceClass::UITalkTreeSpaceClass(SClient *client) :
	UITalkTreeClass(client, 0),
	AvatarPIP(NULL)
{
}

// 0x64F390
UITalkTreeSpaceClass::~UITalkTreeSpaceClass()
{
	delete AvatarPIP;
	AvatarPIP = NULL;
}



































































// 0x64F430
void UITalkTreeSpaceClass::Unknown_54()
{
	UITalkTreeClass::Unknown_54();
	FAIL_IF(AvatarPIP) return;
	AvatarPIP = new AvatarPipClass;
}

// 0x64F4F0
void UITalkTreeSpaceClass::Unknown_5C()
{
	UITalkTreeClass::Unknown_5C();
	AvatarPIP->Unknown_7E4350();
}

// 0x64F520
void UITalkTreeSpaceClass::Unknown_90(TalkTreePacketClass *packet)
{
	if (!Active) {
		AvatarPIP->Unknown_7E36B0(Client->As_Space_Client(), Create_NEW(InterfaceMesh->Unknown_5B8B40("PICTURE")), 0);
	}
	UITalkTreeClass::Unknown_90(packet);
}

// 0x64F600
void UITalkTreeSpaceClass::Unknown_60()
{
	UITalkTreeClass::Unknown_60();
	if (Active) {
		Unknown_64F630();
	}
}

// 0x64F630: the NPC's picture and full name (0x64CF90 returns the name, or NULL)
void UITalkTreeSpaceClass::Unknown_64F630()
{
	const char *name = (const char *)Unknown_64CF90();	// NAME 1: what 0x7E3A20 takes
	if (AvatarPIP->Unknown_7E3A20(name, 0, NULL)) {
		RefCountPtr<CharacterViewAvatarClass> &avatar = AvatarPIP->Unknown02C;	// NAME 2: the PIP's +0x2C avatar
		std::string full_name;	// NAME 2: what Get_Full_Name fills
		avatar->Desc.Get_Full_Name(full_name);
		Unknown_64C7B0(full_name);
	}
	if (name) {
		Unknown_64CFF0();
	}
}

/*
** Reconstruction notes. Placement: 0x64F430's FAIL_IF names this file (__FILE__ 0xB8BDFC, 'AvatarPIP', line 102).
** Object: 0x64F310 (constructor, table 0xAF8CA0) to the <string> initialiser at 0x64F740; adjustor thunks 0x64F770 and
** 0x64F780 for the slot 0x64F360 follow. 0x64F4F0, 0x64F520, 0x64F600 are this table's slots +0x5C, +0x90, +0x60
** (base table 0xAF8B4C: 0x64C8D0, 0x64CC90, 0x64D770); the next object starts at 0x64F790.
*/
