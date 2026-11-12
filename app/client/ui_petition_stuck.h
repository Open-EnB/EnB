// Earth & Beyond client (reconstruction): app\client\ui_petition_stuck.h, the petition's 'stuck' page. The client's
// asserts name the file (e:\build\beta\G\app\Client\ui_petition_stuck.cpp); this header's name follows it (NAME 1,
// provisional).
#ifndef UI_PETITION_STUCK_H  // NAME 1 [ui_petition_stuck.h]: provisional header name for ui_petition_stuck.cpp
#define UI_PETITION_STUCK_H

#include "ui_petition_entry.h"

// The stuck page (0xB8 bytes, tables 0xAF55F4 / 0xAF55E8 / 0xAF55DC): the entry window whose submit sends a
// PetitionStuckPacketClass with the subject and the details
class UIPetitionStuckClass : public UIPetitionEntryClass	// NAME 1: neutral, after the file
{
public:
	UIPetitionStuckClass(SClient *client, int unknown0B4);	// 0x5D7370 (deleting 0x5D73C0)  // NAME 1 [client, unknown0B4]: neutral
	virtual ~UIPetitionStuckClass();					// 0x5D73F0
	virtual bool Unknown_A0();							// +0xA0 0x5D7420 (FAIL_IF '!Get_Client()': the petition sent)

	int Unknown0B4;										// +0xB4 (the packet's second GameID)
};

#endif // UI_PETITION_STUCK_H
