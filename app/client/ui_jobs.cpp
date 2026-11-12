// Earth & Beyond client (reconstruction): app\Client\ui_jobs.cpp. Notes at the end of the file.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ui_jobs.h"
#include "client.h"
#include "sbaseview.h"
#include "gadget.h"
#include "unknown_675120.h"
#include "gd_slider.h"
#include "local_interface_sort.h"
#include "rendobj.h"
#include "control_keys_manager.h"
#include "..\packets\pkjoblist.h"
#include "..\packets\pkstarbasetow.h"
#include "..\shared\g_assert.h"
#include "..\audio\soundlib.h"
#include "fontmanager.h"	// FontSys, FontManagerClass (0xBE67C4) and fontdatabase.h's FontStringFilesClass (0xBE67BC)

extern FontManagerClass *Unknown_BE67C4;	// NAME 1: neutral (D10)

// The client at InterfaceClass +0x40, as this file calls it (as ui_manu.cpp declares it)
class LineDisplayClientClass  // NAME 1: neutral (D10)
{
public:
	virtual void Unknown_00(); virtual void Unknown_04(); virtual void Unknown_08(); virtual void Unknown_0C();
	virtual void Unknown_10(); virtual void Unknown_14();
	virtual SClient *As_Space_Client();					// +0x18 (ClientClass::As_Space_Client; SClient's returns itself)  // NAME 3: ui_manu.cpp's FAIL_IF's
};

void Unknown_496AE0(RenderObjClass *robj, bool make_geometry_unique);	// 0x496AE0 (ro_funcs.cpp, cdecl)  // NAME 1 [robj, make_geometry_unique]: as ui_manu.cpp's
RefCountPtr<RenderObjClass> Unknown_497300(RenderObjClass *robj);	// 0x497300 (cdecl, ro_funcs.cpp's neighbourhood)  // NAME 1 [robj]: as ui_avatr.cpp's

// The control keys event slot +0x54 registers (0x10 bytes; as ui_avatr.cpp's)










enum
{
	UI_CLOSED = 0,										// (the destructor's G_ASSERT 'State == UI_CLOSED'; slot +0x5C sets it)  // NAME 3: the assert's
	UI_OPEN = 1											// (slot +0x58 sets it)  // NAME 1: neutral (D10)
};

// The window's buttons (0xB8077C: slot +0x54's ids 1..10) and the names of its texts (0xB807A8)
static const char *Unknown_B8077C[] = { "not used", "exit", "exit_button", "button00", "button01", "button02", "name", "Level", "Sponsor", "Reward", "accept" };	// NAME 1: neutral (D10)
static const char *Unknown_B807A8[] = { "ITEM_NAME_JNEWS00", "" };	// NAME 1: neutral (D10)

// @0x5A1170
UIJobsClass::UIJobsClass(SClient *client) :
	Unknown5BB320Class(new UIMeshClass("JNEWS00", true), client),
	Unknown0AC(NULL),
	Unknown0B4(0),
	Unknown0B8(0),
	State(UI_CLOSED),
	Unknown0C0(0),
	Unknown0C4(0),
	Unknown0C8(0),
	Unknown0CC(-1),
	Unknown0D0(0),
	Unknown0D4(-1),
	Unknown0D8(false)
{
	memset(Unknown0F8, 0, sizeof(Unknown0F8));
	memset(Unknown124, 0, sizeof(Unknown124));
	Unknown1EC[0] = 0;
	Unknown1EC[1] = 1;
	Unknown1EC[2] = 2;
	Unknown_BE67BC->Load("FA_JNEWS00.INI");
	Unknown_BE67C4->Add(InterfaceMesh->Peek_Mesh_Object());
	Unknown_5A3930(NULL, NULL, NULL);
}
















// @0x5A1540
UIJobsClass::~UIJobsClass()
{
	G_ASSERT(State == UI_CLOSED);
	if (Unknown0AC) {
		Unknown0AC->Unknown_66E460(RefCountPtr<RenderObjClass>(), 0, 0);
	}
	Unknown0B0 = NULL;
	Unknown_BE67C4->Remove(InterfaceMesh->Peek_Mesh_Object());
	Unknown_BE67BC->Unload("FA_JNEWS00.INI");
}

// @0x5A1760
void UIJobsClass::Unknown_54()
{
	for (int id = 1; id < 11; id++) {
		RefCountPtr<RenderObjClass> robj = Create_Get(InterfaceMesh->Peek_Object()->Get_Sub_Object_By_Name(Unknown_B8077C[id]));
		if (robj) {
			Unknown_496AE0(robj.Peek(), 0);
			Unknown0F8[id] = new ButtonGadgetClass(robj.Peek(), 0x55, 3);
			Unknown64.Unknown_6870E0(Unknown0F8[id], id, View, 1, 0);
			Unknown_5BB4F0(Unknown0F8[id]);
		}
		if (id == 10) {
			RefCountPtr<FontSys> text = Create_Peek(Unknown_BE67C4->Get_Style("Accept_JNEWS00", InterfaceMesh->Peek_Object()));
			Unknown0F8[10]->Unknown_98(text.Peek());
		}
	}
	char name[64];
	for (int i = 0; i < 10; i++) {
		_snprintf(name, sizeof(name), "name%02d", i + 1);
		Unknown124[i] = new ButtonGadgetClass(InterfaceMesh, name, 0x55, 3);
		Unknown64.Unknown_6870E0(Unknown124[i], i + 12, View, 1, 0);
		Unknown_5BB4F0(Unknown124[i]);
		_snprintf(name, sizeof(name), "Type%02d_JNEWS00", i + 1);
		Unknown14C[i].Type = Create_Peek(Unknown_BE67C4->Get_Style(name, InterfaceMesh->Peek_Object()));
		_snprintf(name, sizeof(name), "Level%02d_JNEWS00", i + 1);
		Unknown14C[i].Level = Create_Peek(Unknown_BE67C4->Get_Style(name, InterfaceMesh->Peek_Object()));
		_snprintf(name, sizeof(name), "Sponsor%02d_JNEWS00", i + 1);
		Unknown14C[i].Sponsor = Create_Peek(Unknown_BE67C4->Get_Style(name, InterfaceMesh->Peek_Object()));
		_snprintf(name, sizeof(name), "Reward%02d_JNEWS00", i + 1);
		Unknown14C[i].Reward = Create_Peek(Unknown_BE67C4->Get_Style(name, InterfaceMesh->Peek_Object()));
	}
	Unknown0F8[1]->Unknown_40(false);
	Unknown0F8[1]->Unknown_8C(true);
	Unknown0AC = new GadgetSlider(false);
	Unknown0AC->Unknown_66E460(Unknown_497300(InterfaceMesh->Peek_Mesh_Object()), 0, 0);
	Unknown64.Unknown_6870E0(Unknown0AC, 0x16, View, 1, 0);
	Unknown_5BB4F0(Unknown0AC);
	Unknown0B0 = Create_Get(new UIInvRobjStripClass(Client->As_Space_Client(), InterfaceMesh->Peek_Object(), 1, "JNEWS00.C00", 0x12));
	Unknown0B0->Unknown_54();
	((ControlKeyListClass *)&Unknown4C)->Add(KernelEventClass(0x1B), (void *)2);
	RefCountPtr<RenderObjClass> tutorial = Create_Get(InterfaceMesh->Peek_Object()->Get_Sub_Object_By_Name("tutorial"));
	if (tutorial) {
		tutorial->Set_Hidden(true);
	}
	Unknown_10();
}

// @0x5A2010
void UIJobsClass::Unknown_7C(UIViewClass *view, bool arg)
{
	Unknown5BB320Class::Unknown_7C(view, arg);
	if (Unknown0B0) {
		Unknown0B0->Unknown_7C(view, false);
	}
}

static inline int Jobs_Player_Id(SClient *sclient) { return sclient->Unknown1138; }  // NAME 2: the player id read as an inline accessor (as ui_manu.cpp's Manu_Player_Id)

// @0x5A2050
void UIJobsClass::Unknown_58()
{
	SClient *sclient = Client->As_Space_Client();
	if (State == UI_CLOSED) {
		InterfaceSortScopeClass scope(this);
		State = UI_OPEN;
		Unknown0CC = -1;
		Unknown0D0 = 0;
		Unknown0D4 = -1;
		Unknown0D8 = false;
		Unknown_5A32B0();
		Unknown0B0->Unknown_7C(View, true);
		if (sclient->Unknown135C) {
			((SBaseViewClass *)sclient->Unknown135C)->Unknown_69DC40();
		}
		Unknown5BB320Class::Unknown_58();
		Unknown_5A3090();
		Unknown_5A3890();
		sclient->Unknown_744560(0x1B);
		sclient->Send_Packet(&StarbaseRequestPacketClass(Jobs_Player_Id(sclient), 6, 0));
		Unknown_5A3440(0);
	}
}

// @0x5A2480
void UIJobsClass::Unknown_5C()
{
	SClient *sclient = Client->As_Space_Client();
	if (State == UI_OPEN) {
		State = UI_CLOSED;
		if (sclient->Unknown135C) {
			((SBaseViewClass *)sclient->Unknown135C)->Unknown_69DC60();
		}
		sclient->Send_Packet(&StarbaseRequestPacketClass(Jobs_Player_Id(sclient), 9, 0));
		Unknown0B0->Unknown_5C();
		Unknown5BB320Class::Unknown_5C();
		sclient->Unknown_744560(0x1C);
	}
}

// @0x5A2560
void UIJobsClass::Unknown_60()
{
	Unknown_5A3400();
	Unknown5BB320Class::Unknown_60();
}

// @0x5A25B0
bool UIJobsClass::Unknown_68(int key)
{
	if (!Unknown0C4) {
		switch (key) {
			case 2:
				SoundLibraryClass::Play_2D_Sound("INTERFACE_MenuSlide01C.wav", 2);
				((SBaseViewClass *)View->Unknown_30())->Unknown_69F440();
				return(true);
			case 3:
				Unknown_5A3440(0);
				return(true);
			case 4:
				Unknown_5A3440(1);
				return(true);
			case 5:
				Unknown_5A3440(2);
				return(true);
			case 6:
			case 7:
			case 8:
			case 9:
				Unknown_5A2A30(key - 6);
				Unknown_5A2C50();
				return(true);
			case 12:
			case 13:
			case 14:
			case 15:
			case 16:
			case 17:
			case 18:
			case 19:
			case 20:
			case 21:
				Unknown_5A2730(key - 12);
				return(true);
			case 10:
				Unknown_5A36F0();
				return(true);
		}
	}
	return(false);
}

































































































// @0x5A2730
void UIJobsClass::Unknown_5A2730(int row)
{
	int index = Unknown0B4 + row;
	FAIL_IF(index >= (int)JobInfo.size()) return;
	FAIL_IF(index < 0) return;
	int job_id = JobInfo[index]->Unknown44;
	Unknown0CC = index;
	Unknown0D0 = job_id;
	Unknown_5A3090();
	Unknown_5A3240();
	Client->As_Space_Client()->Send_Packet(&StarbaseRequestPacketClass(Jobs_Player_Id(Client->As_Space_Client()), 7, JobInfo[Unknown0CC]->Unknown44));
	Unknown0C4 = 2;
}








// @0x5A2900
void UIJobsClass::Unknown_5A2900(int index, bool show, const char *text)
{
	const char *name = Unknown_B807A8[index]; RefCountPtr<FontSys> font = Create_Peek(Unknown_BE67C4->Get_Style(name, InterfaceMesh->Peek_Mesh_Object()));
	FAIL_IF(!font) return;
	font->Change_Text(text, 0);
	font->Set_Hidden(!show);
}

// @0x5A2A30
void UIJobsClass::Unknown_5A2A30(int column)
{
	if (Unknown1EC[0] == column) {
		Unknown0D8 = !Unknown0D8;
		return;
	}
	for (int i = 2; i > 0; i--) {
		Unknown1EC[i] = Unknown1EC[i - 1];
	}
	Unknown1EC[0] = column;
}

// @0x5A2AA0
int UIJobsClass::Unknown_5A2AA0(UIJobsInfoStruct *a, UIJobsInfoStruct *b, int column)
{
	int result = 0;
	switch (column) {
		case 0:
			result = strcoll(a->Unknown04.c_str(), b->Unknown04.c_str());
			break;
		case 1:
			{
				int a_len = strlen(a->Unknown14.c_str());
				int b_len = strlen(b->Unknown14.c_str());
				if (a_len > b_len) {
					result = 1;
				} else if (a_len < b_len) {
					result = -1;
				} else {
					result = strcoll(a->Unknown14.c_str(), b->Unknown14.c_str());
				}
			}
			break;
		case 2:
			result = strcoll(a->Unknown24.c_str(), b->Unknown24.c_str());
			break;
		case 3:
			{
				int a_reward = atoi(a->Unknown34.c_str());
				int b_reward = atoi(b->Unknown34.c_str());
				if (a_reward > b_reward) {
					result = 1;
				} else if (a_reward < b_reward) {
					result = -1;
				}
			}
			break;
	}
	if (Unknown0D8) {
		result = -result;
	}
	return(result);
}

// @0x5A2C50
void UIJobsClass::Unknown_5A2C50()
{
	Unknown_5A37D0();
	for (std::list<UIJobsInfoStruct *>::iterator it = Unknown0EC.begin(); it != Unknown0EC.end(); it++) {
		UIJobsInfoStruct *job = *it;
		if (Unknown0D4 == job->Unknown00) {
			std::vector<UIJobsInfoStruct *>::iterator pos;
			for (pos = JobInfo.begin(); pos != JobInfo.end(); pos++) {
				UIJobsInfoStruct *other = *pos;
				int result = Unknown_5A2AA0(job, other, Unknown1EC[0]);
				if (result == 0) {
					result = Unknown_5A2AA0(job, other, Unknown1EC[1]);
					if (result == 0) {
						result = Unknown_5A2AA0(job, other, Unknown1EC[2]);
					}
				}
				if (result < 0) {
					break;
				}
			}
			UIJobsInfoStruct *copy = new UIJobsInfoStruct;
			copy->Unknown04 = job->Unknown04;
			copy->Unknown14 = job->Unknown14;
			copy->Unknown24 = job->Unknown24;
			copy->Unknown34 = job->Unknown34;
			copy->Unknown44 = job->Unknown44;
			JobInfo.insert(pos, copy);
		}
	}
	if (Unknown0D0 > 0) {
		Unknown0CC = -1;
		int index = 0;
		for (std::vector<UIJobsInfoStruct *>::iterator v = JobInfo.begin(); v != JobInfo.end(); v++, index++) {
			if (Unknown0D0 == (*v)->Unknown44) {
				Unknown0CC = index;
			}
		}
		if (Unknown0CC < 0) {
			Unknown0D0 = 0;
		}
	}
	Unknown_5A3090();
}

// @0x5A2FB0
void UIJobsClass::Unknown_5A2FB0(int row)
{
	if (Unknown0CC == Unknown0B4 + row) {
		Unknown14C[row].Type->Unknown_6CE910(true);
		Unknown14C[row].Level->Unknown_6CE910(true);
		Unknown14C[row].Sponsor->Unknown_6CE910(true);
		Unknown14C[row].Reward->Unknown_6CE910(true);
	} else {
		Unknown14C[row].Type->Unknown_6CE910(false);
		Unknown14C[row].Level->Unknown_6CE910(false);
		Unknown14C[row].Sponsor->Unknown_6CE910(false);
		Unknown14C[row].Reward->Unknown_6CE910(false);
	}
}

// @0x5A3090
void UIJobsClass::Unknown_5A3090()
{
	for (int i = 0; i < 10; i++) {
		unsigned int index = Unknown0B4 + i;
		if (index < JobInfo.size()) {
			Unknown14C[i].Type->Change_Text(JobInfo[index]->Unknown04.c_str(), 0);
			Unknown14C[i].Level->Change_Text(JobInfo[index]->Unknown14.c_str(), 0);
			Unknown14C[i].Sponsor->Change_Text(JobInfo[index]->Unknown24.c_str(), 0);
			Unknown14C[i].Reward->Change_Text(JobInfo[index]->Unknown34.c_str(), 0);
			Unknown124[i]->Unknown_40(true);
		} else {
			Unknown14C[i].Type->Change_Text("", 0);
			Unknown14C[i].Level->Change_Text("", 0);
			Unknown14C[i].Sponsor->Change_Text("", 0);
			Unknown14C[i].Reward->Change_Text("", 0);
			Unknown124[i]->Unknown_40(false);
		}
		Unknown_5A2FB0(i);
	}
}

// @0x5A3240
void UIJobsClass::Unknown_5A3240()
{
	if (Unknown0CC >= 0) {
		Unknown0F8[10]->Unknown_40(true);
	} else {
		Unknown0F8[10]->Unknown_40(false);
	}
	Unknown0F8[2]->Unknown_40(true);
	Unknown0F8[1]->Unknown_40(false);
	Unknown0F8[1]->Unknown_8C(true);
}

// @0x5A32B0
void UIJobsClass::Unknown_5A32B0()
{
	Unknown_5A3390(0);
	Unknown_5A3930(NULL, NULL, NULL);
	Unknown0F8[10]->Unknown_40(false);
	Unknown_5A3090();
	Unknown_5A3240();
}

// @0x5A3390
void UIJobsClass::Unknown_5A3390(int count)
{
	Unknown0AC->Unknown_66FA90((float)count, 10.0f, true);
	Unknown_5A3400();
}

// @0x5A3400
void UIJobsClass::Unknown_5A3400()
{
	int top = (int)Unknown0AC->Get_Value();
	if (Unknown0B4 != top) {
		Unknown0B4 = top;
		Unknown_5A3090();
	}
}

// @0x5A3440
void UIJobsClass::Unknown_5A3440(int category)
{
	if (Unknown0D4 == category) {
		return;
	}
	Unknown_5A32B0();
	Unknown0D4 = category;
	for (int i = 0; i < 3; i++) {
		int id = Unknown_5A45A0(i);
		if (Unknown0D4 == i) {
			Unknown0F8[id]->Unknown2C_0B = true;
			Unknown0F8[id]->Unknown_6603A0(false, NULL);
		} else {
			Unknown0F8[id]->Unknown2C_0B = false;
			Unknown0F8[id]->Unknown_6607D0();
		}
	}
	RefCountPtr<FontSys> title = Create_Peek(Unknown_BE67C4->Get_Style("TITLE_JNEWS00", InterfaceMesh->Peek_Mesh_Object()));
	switch (Unknown0D4) {
		case 0:
			title->Change_Text("COMBAT JOBS", 0);
			break;
		case 1:
			title->Change_Text("TRADE JOBS", 0);
			break;
		case 2:
			title->Change_Text("EXPLORE JOBS", 0);
			break;
	}
	Unknown_5A3AF0();
}

// @0x5A36F0
void UIJobsClass::Unknown_5A36F0()
{
	if (Unknown0CC >= 0 && Unknown0CC < (int)JobInfo.size()) {
		Client->As_Space_Client()->Send_Packet(&StarbaseRequestPacketClass(Jobs_Player_Id(Client->As_Space_Client()), 8, JobInfo[Unknown0CC]->Unknown44));
		Unknown0C4 = 3;
	}
}

// @0x5A37D0
void UIJobsClass::Unknown_5A37D0()
{
	for (std::vector<UIJobsInfoStruct *>::iterator it = JobInfo.begin(); it != JobInfo.end(); it++) {
		delete *it;
	}
	JobInfo.clear();
}

// @0x5A3890
void UIJobsClass::Unknown_5A3890()
{
	for (std::list<UIJobsInfoStruct *>::iterator it = Unknown0EC.begin(); it != Unknown0EC.end(); it++) {
		delete *it;
	}
	std::list<UIJobsInfoStruct *> &jobs = Unknown0EC; jobs.erase(jobs.begin(), jobs.end());
}

// @0x5A3930
void UIJobsClass::Unknown_5A3930(const char *title, const char *detail, const char *item)
{
	RefCountPtr<FontSys> title_text = Create_Peek(Unknown_BE67C4->Get_Style("JOB_TITLE_JNEWS00", InterfaceMesh->Peek_Object()));
	if (title && *title) {
		title_text->Change_Text(title, 0);
	} else {
		title_text->Change_Text(" ", 0);
	}
	ManagedFontClass *font = Unknown_BE67C4->Find(InterfaceMesh->Peek_Object());
	if (detail && *detail) {
		font->Unknown_6D0A70("WRAP_DETAIL_JNEWS00", detail);
	} else {
		font->Unknown_6D0A70("WRAP_DETAIL_JNEWS00", " ");
	}
	RefCountPtr<FontSys> item_text = Create_Peek(Unknown_BE67C4->Get_Style("ITEM_NAME_JNEWS00", InterfaceMesh->Peek_Object()));
	if (item && *item) {
		item_text->Change_Text(item, 0);
	} else {
		item_text->Change_Text(" ", 0);
	}
}

// @0x5A3AF0
void UIJobsClass::Unknown_5A3AF0()
{
	if (Unknown0D4 >= 0) {
		Unknown_5A2C50();
		Unknown_5A3090();
		Unknown_5A3390(JobInfo.size());
		Unknown0B4 = (int)Unknown0AC->Get_Value();
	}
}

// @0x5A3BC0
void UIJobsClass::Unknown_5A3BC0(JobListPacketClass *packet)
{
	for (std::vector<JobStruct *>::iterator it = packet->JobList.begin(); it != packet->JobList.end(); it++) {
		JobStruct *job = *it;
		UIJobsInfoStruct *info = new UIJobsInfoStruct;
		info->Unknown00 = job->Unknown04;
		info->Unknown04 = job->Unknown10;
		char buffer[16];
		info->Unknown14 = itoa(job->Unknown0C, buffer, 10);
		info->Unknown24 = job->Unknown20;
		info->Unknown34 = job->Unknown30;
		info->Unknown44 = job->Unknown00;
		Unknown0EC.push_back(info);
	}
	Unknown_5A3AF0();
}

// @0x5A3F10
void UIJobsClass::Unknown_5A3F10(JobDescriptionPacketClass *packet)
{
	if (packet->StillAvail) {
		Unknown_5A3930(packet->Title, packet->Description, JobInfo[Unknown0CC]->Unknown34.c_str());
		Unknown0F8[10]->Unknown_40(true);
		Unknown0C4 = 0;
	} else {
		Unknown_5A3930("Job no longer available", "Sorry, this job has either just been taken by another player or has been retracted by the sponsor", "");
		Unknown0C4 = 0;
	}
}





















































































































// (inline in 0x5A3FB0 and 0x5A4430: no copy of its own in the client)
inline void UIJobsClass::Unknown_Remove_Job(int job_id)
{
	for (std::list<UIJobsInfoStruct *>::iterator it = Unknown0EC.begin(); it != Unknown0EC.end(); ++it) {
		UIJobsInfoStruct *job = *it;
		if (job->Unknown44 == job_id) {
			if (Unknown0D0 == job_id) {
				Unknown0CC = -1;
				Unknown0D0 = 0;
			}
			delete job;
			Unknown0EC.erase(it++);
			break;
		}
	}
}

// NOT EXACT: inline budget (0x5A4430 inlined with Unknown_Remove_Job inside it: the client expands its delete to 0x5A2250 + operator delete and inlines the list's erase, ours calls the scalar deleting destructor and erase; attempts.csv) (0x5A3FB0)
// @0x5A3FB0
void UIJobsClass::Unknown_5A3FB0(JobAcceptReplyPacketClass *packet)
{
	RefCountPtr<FontSys> jobtitle = Create_Peek(Unknown_BE67C4->Get_Style("JOB_TITLE_JNEWS00", InterfaceMesh->Peek_Object()));
	FAIL_IF(!jobtitle) return;
	Unknown0C4 = 0;
	char buffer[128];
	switch (packet->AcceptStatus) {
		case 0:
			if (Unknown0D4 == 1) {
				_snprintf(buffer, sizeof(buffer), "%s mission will be added to your journal if you satisfy the cargo requirements of this mission.", jobtitle->Unknown_6D87D0());
				Unknown_5A3930("Acquiring Trade Job", buffer, "");
			} else {
				_snprintf(buffer, sizeof(buffer), "%s mission added to journal.", jobtitle->Unknown_6D87D0());
				Unknown_5A3930("Job Accepted", buffer, "");
			}
			break;
		case 1:
			_snprintf(buffer, sizeof(buffer), "The (%s) mission has either just been taken by another player or has been retracted by the sponsor.", jobtitle->Unknown_6D87D0());
			Unknown_5A3930("Job no longer available", buffer, "");
			break;
		case 2:
			Unknown_5A3930("Cannot Accept another mission", "You already have the maximum number of missions. Please complete them before requesting another job.", "");
			break;
		case 3:
			{
				int seconds = packet->SecondsRemaining;
				if (seconds > 60) {
					int minutes = seconds / 60;
					_snprintf(buffer, sizeof(buffer), "You have to wait another %d minute%s before your computer is ready to accept another mission.", minutes, (minutes > 1) ? "s" : "");
				} else {
					_snprintf(buffer, sizeof(buffer), "You have to wait another %d second%s before your computer is ready to accept another mission.", seconds, (seconds > 1) ? "s" : "");
				}
				Unknown_5A3930("Cannot Accept another mission now.", buffer, "");
			}
			break;
	}
	if (packet->AcceptStatus == 0 || packet->AcceptStatus == 1) {
		Unknown_5A4430(Unknown0D0);









		Unknown0CC = -1;
		Unknown0D0 = 0;
	}
	Unknown0F8[10]->Unknown_40(false);
	Unknown_5A3AF0();
}

// @0x5A4430
void UIJobsClass::Unknown_5A4430(int job_id)
{
	Unknown_Remove_Job(job_id);
	Unknown_5A3AF0();
}



























// @0x5A45A0
int UIJobsClass::Unknown_5A45A0(int category)
{
	switch (category) {
		case 0:
			return(3);
		case 1:
			return(4);
		case 2:
			return(5);
	}
	G_ASSERT(0);
	return(3);
}

#include "..\shared\auxdata\permstr.h"	// (its PermStringInitClass static: 0xBE5C14, initialised by 0x5A4B00)

/*
** Reconstruction notes (JB1, 2026-11-09)
** The window's functions 0x5A1170-0x5A45A0 in the client's order, then the STL instances 0x5A45F0-0x5A4A50, the
** thunks 0x5A4AE0 / 0x5A4AF0 and the PermStringInitClass static 0x5A4B00.
*/
