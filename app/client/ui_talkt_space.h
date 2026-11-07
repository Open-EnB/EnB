// Earth & Beyond client (reconstruction): app\client\ui_talkt_space.cpp's talk tree in space (0x184 bytes, table
// 0xAF8CA0 / 0xAF8C94 / 0xAF8C88): UITalkTreeClass with an avatar picture-in-picture; SClient's +0x129C.
#ifndef UI_TALKT_SPACE_H
#define UI_TALKT_SPACE_H

#include "sclient.h"

class AvatarPipClass;

class UITalkTreeSpaceClass : public UITalkTreeClass	// NAME 3: class-from-file (D24) for app\client\ui_talkt_space.cpp
{
public:
	UITalkTreeSpaceClass(SClient *client);				// 0x64F310
	~UITalkTreeSpaceClass();							// 0x64F390 (slot 0x64F360)
	virtual void Unknown_54();							// +0x54 0x64F430
	virtual void Unknown_5C();							// +0x5C 0x64F4F0
	virtual void Unknown_60();							// +0x60 0x64F600
	virtual void Unknown_90(TalkTreePacketClass *packet);	// +0x90 0x64F520  // NAME 1 [packet]: the base's

	void Unknown_64F630();								// 0x64F630 (0x64F600, while Active)

	AvatarPipClass *AvatarPIP;							// +0x180  // NAME 3: the FAIL_IF's 'AvatarPIP' (0x64F430, l.102)
};

#endif
