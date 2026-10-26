// Earth & Beyond client (reconstruction): app\packets\pkjoblist.cpp. Notes at the end of the file.
#include "pkjoblist.h"
#include <stdio.h>


JobDeletePacketClass::JobDeletePacketClass(int job_id) : JobID(job_id)
{
}

JobDeletePacketClass::JobDeletePacketClass(RawPacketClass *raw) : JobID(0)
{
	raw->Get(&JobID, sizeof(JobID));
}

RawPacketClass *JobDeletePacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(149, Get_Data_Size());
	raw->Add(&JobID, sizeof(JobID));
	return(raw);
}

int JobDeletePacketClass::Get_Data_Size()
{
	return(sizeof(JobID));
}

int JobDeletePacketClass::Print(char *buffer, int len)
{
	return(_snprintf(buffer, len, "%s {JobID: %u}", "JobDelete", JobID));
}

JobAcceptReplyPacketClass::JobAcceptReplyPacketClass(int job_id, int accept_status, int seconds_remaining) : JobID(job_id), AcceptStatus(accept_status), SecondsRemaining(seconds_remaining)
{
}

JobAcceptReplyPacketClass::JobAcceptReplyPacketClass(RawPacketClass *raw)
{
	raw->Get(&JobID, sizeof(JobID));
	raw->Get(&AcceptStatus, sizeof(AcceptStatus));
	raw->Get(&SecondsRemaining, sizeof(SecondsRemaining));
}

RawPacketClass *JobAcceptReplyPacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(150, Get_Data_Size());
	raw->Add(&JobID, sizeof(JobID));
	raw->Add(&AcceptStatus, sizeof(AcceptStatus));
	raw->Add(&SecondsRemaining, sizeof(SecondsRemaining));
	return(raw);
}

int JobAcceptReplyPacketClass::Get_Data_Size()
{
	return(sizeof(JobID) + sizeof(AcceptStatus) + sizeof(SecondsRemaining));
}

int JobAcceptReplyPacketClass::Print(char *buffer, int len)
{
	return(_snprintf(buffer, len, "%s {JobID: %u AcceptStatus: %d SecondsRemaining: %u}", "JobAcceptReply", JobID, AcceptStatus, SecondsRemaining));
}


/*
**	Reconstruction notes (pkjoblist.cpp)
**
**	* Client 0x895F00-0x897490 (the object ends with the guarded static every <string> includer has,
**	  0xC416FC, atexit 0x42B320). Drafted from the client by the packet decoder (tools/re/pkdecode.py,
**	  pkemit.py): members from the raw message's Get / Build's Add (sizes) and Print (types,
**	  names: 'Name=%d'), the constructors' stores in the initialiser list.
**	* Not rebuilt yet: opcode 147, 148 (declared in pk_classes.h).
*/
