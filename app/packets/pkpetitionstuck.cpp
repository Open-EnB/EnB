// Earth & Beyond client (reconstruction): app\packets\pkpetitionstuck.cpp. Notes at the end of the file.
#include "pkpetitionstuck.h"
#include "nstrdup.h"
#include <stdio.h>
#include <string.h>

PetitionStuckPacketClass::PetitionStuckPacketClass(int game_id, int game_id2, const char *subject, const char *complaint, const char *player_list) :
	GameID(game_id),
	GameID2(game_id2),
	Subject(NULL),
	Complaint(NULL),
	PlayerList(NULL)
{
	Subject = nstrdup(subject != NULL ? subject : "Error");
	Complaint = nstrdup(complaint != NULL ? complaint : "Error");
	PlayerList = nstrdup(player_list != NULL ? player_list : "");
}

PetitionStuckPacketClass::PetitionStuckPacketClass(RawPacketClass *raw)
{
	raw->Get(&GameID, sizeof(GameID));
	raw->Get(&GameID2, sizeof(GameID2));
	Subject = raw->Get_String();
	Complaint = raw->Get_String();
	PlayerList = raw->Get_String();
}

PetitionStuckPacketClass::~PetitionStuckPacketClass()
{
	delete [] Subject;
	Subject = NULL;
	delete [] Complaint;
	Complaint = NULL;
	delete [] PlayerList;
	PlayerList = NULL;
}

RawPacketClass *PetitionStuckPacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(136, Get_Data_Size());
	raw->Add(&GameID, sizeof(GameID));
	raw->Add(&GameID2, sizeof(GameID2));
	raw->Add_String(Subject);
	raw->Add_String(Complaint);
	raw->Add_String(PlayerList);
	return(raw);
}

int PetitionStuckPacketClass::Get_Data_Size()
{
	return(strlen(PlayerList) + strlen(Complaint) + strlen(Subject) + 3 + sizeof(GameID) + sizeof(GameID2));
}

int PetitionStuckPacketClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "PetitionStuck {GameID=%d GameID=%d Subject = %0.20s Complaint = %0.20s PlayerList = %0.20s}", GameID, GameID2, Subject, Complaint, PlayerList));
}


/*
**	Reconstruction notes (pkpetitionstuck.cpp)
**
**	* Client 0x89F740-0x89FB20 (0x89F710 is the previous object's guarded static, 0xC416FC; this one's is 0x89FB20):
**	  opcode 136, written by hand (builder P4, 2026-10-27). The value constructor comes first.
**	* The file name is provisional (no assert names it).
*/
