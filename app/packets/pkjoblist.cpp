// Earth & Beyond client (reconstruction): app\packets\pkjoblist.cpp. Notes at the end of the file.
#include "pkjoblist.h"
#include "..\shared\unknown_debug.h"
#include "nstrdup.h"
#include <string.h>
#include <stdio.h>


JobListPacketClass::JobListPacketClass() :
	Count(0)
{
}

JobListPacketClass::JobListPacketClass(RawPacketClass *raw)
{
	raw->Get(&Count, sizeof(Count));
	JobList.resize(Count);
	for (int i = 0; i < Count; i++) {
		JobStruct *job = new JobStruct;
		raw->Get(&job->Unknown00, sizeof(job->Unknown00));
		raw->Get(&job->Unknown04, sizeof(job->Unknown04));
		raw->Get(&job->Unknown08, sizeof(job->Unknown08));
		raw->Get(&job->Unknown0C, sizeof(job->Unknown0C));
		char *string = raw->Get_String();
		job->Unknown10 = string;
		delete [] string;
		string = raw->Get_String();
		job->Unknown20 = string;
		delete [] string;
		string = raw->Get_String();
		job->Unknown30 = string;
		delete [] string;
		JobList[i] = job;
	}
}

JobListPacketClass::~JobListPacketClass()
{
	for (std::vector<JobStruct *>::iterator it = JobList.begin(); it != JobList.end(); ++it) {
		delete *it;
	}
	JobList.clear();
}

RawPacketClass *JobListPacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(147, Get_Data_Size());
	raw->Add(&Count, sizeof(Count));
	for (int i = 0; i < Count; i++) {
		JobStruct *job = JobList[i];


















		FAIL_IF(!job) {
			delete raw;
			return(NULL);
		}
		raw->Add(&job->Unknown00, sizeof(job->Unknown00));
		raw->Add(&job->Unknown04, sizeof(job->Unknown04));
		raw->Add(&job->Unknown08, sizeof(job->Unknown08));
		raw->Add(&job->Unknown0C, sizeof(job->Unknown0C));
		raw->Add_String(job->Unknown10.c_str());
		raw->Add_String(job->Unknown20.c_str());
		raw->Add_String(job->Unknown30.c_str());
	}
	return(raw);
}

int JobListPacketClass::Get_Data_Size()
{
	int size = sizeof(Count);
	for (int i = 0; i < Count; i++) {
		JobStruct *job = JobList[i];
		FAIL_IF(!job) {
			return(0);
		}
		size += sizeof(job->Unknown00) + sizeof(job->Unknown04) + sizeof(job->Unknown08) + sizeof(job->Unknown0C) + job->Unknown10.length() + 1 + job->Unknown20.length() + 1 + job->Unknown30.length() + 1;
	}
	return(size);
}

int JobListPacketClass::Print(char *buffer, int len)
{
	return(_snprintf(buffer, len, "%s", "JobList"));
}

void JobListPacketClass::Add(int unknown00, int unknown04, const char *unknown10, int unknown0c, const char *unknown20, int unknown08, const char *unknown30)
{
	JobStruct *job = new JobStruct;
	job->Unknown00 = unknown00;
	job->Unknown04 = unknown04;
	job->Unknown08 = unknown08;
	job->Unknown0C = unknown0c;
	job->Unknown10 = unknown10;
	job->Unknown20 = unknown20;
	job->Unknown30 = unknown30;
	JobList.push_back(job);
	Count++;
}


JobDescriptionPacketClass::JobDescriptionPacketClass(unsigned int job_id, bool still_avail, const char *title, const char *description) :
	JobID(job_id),
	StillAvail(still_avail),
	Title(NULL),
	Description(NULL)
{
	Title = nstrdup(title);
	Description = nstrdup(description);
}

JobDescriptionPacketClass::JobDescriptionPacketClass(RawPacketClass *raw) :
	JobID(0),
	StillAvail(false),
	Title(NULL),
	Description(NULL)
{
	raw->Get(&JobID, sizeof(JobID));
	raw->Get(&StillAvail, sizeof(StillAvail));
	Title = raw->Get_String();
	Description = raw->Get_String();
}

JobDescriptionPacketClass::~JobDescriptionPacketClass()
{
	delete [] Title;
	Title = NULL;
	delete [] Description;
	Description = NULL;
}

RawPacketClass *JobDescriptionPacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(148, Get_Data_Size());
	raw->Add(&JobID, sizeof(JobID));
	raw->Add(&StillAvail, sizeof(StillAvail));
	raw->Add_String(Title);
	raw->Add_String(Description);
	return(raw);
}



















int JobDescriptionPacketClass::Get_Data_Size()
{
	FAIL_IF(!Title || !Description) {
		return(0);
	}
	return(strlen(Description) + strlen(Title) + 2 + sizeof(JobID) + sizeof(StillAvail));
}

int JobDescriptionPacketClass::Print(char *buffer, int len)
{
	return(_snprintf(buffer, len, "%s {JobID: %u StillAvail: %s Title: %s Description: %s}", "JobDescription", JobID, StillAvail ? "yes" : "no", Title, Description));
}

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
**	* Opcode 148 written by hand (builder P4, 2026-10-27); its Get_Data_Size's FAIL_IF is on the client's
**	  line 177 (blank lines above it).
**	* Opcode 147 written by hand (builder P5, 2026-10-27), 8 of 8 exact: JobStruct's third string is
**	  constructed out of line in the raw constructor (0x603BB0) and inline in Add, by budget alone. The
**	  vector<JobStruct *> insert 0x8971A0 and erase 0x897440 at the end of the object are pointer
**	  instances: the same bytes as every other (D54's ambiguous column).
*/
