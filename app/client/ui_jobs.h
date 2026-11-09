// Earth & Beyond client (reconstruction): app\client\ui_jobs.h, the job board window (the client's ui_jobs.cpp per its
// asserts; this header's name is provisional, NAME 1). Laid out from the constructor 0x5A1170 and the destructor 0x5A1540
// (JB1, 2026-11-09).
#ifndef UI_JOBS_H  // NAME 1 [ui_jobs.h]: provisional header name for ui_jobs.cpp
#define UI_JOBS_H

#include "sclient.h"
#include "..\..\wwlib\ref_ptr.h"
#include <string>
#include <vector>
#include <list>

class GadgetSlider;				// gd_slider.h's slider (+0xAC)
class ButtonGadgetClass;		// unknown_675120.h's buttons (+0xF8, +0x124)
class FontSys;					// fontmanager.h's text objects (+0x14C)
class JobListPacketClass;		// pkjoblist.h's packets
class JobDescriptionPacketClass;
class JobAcceptReplyPacketClass;

// ui_inv.cpp's interface (0xE0 bytes, constructor 0x59EDC0) the window holds one of (+0xB0 'JNEWS00.C00'), as far as
// this file calls it (as ui_analyze.h's UIAnalyzeInvClass)
class UIJobsInvClass : public InterfaceClass	// NAME 1: neutral (D10)
{
public:
	UIJobsInvClass(SClient *client, RenderObjClass *model, int count, const char *format, int unknown);	// 0x59EDC0 (slot +0x54: 1 'JNEWS00.C00' 18)  // NAME 1 [client, model, count, format, unknown]: as ui_manu.h's

	char Unknown84[0xE0 - 0x84];						// +0x84
};

// One job as the window keeps it (0x48 bytes; made from a JOB_LIST 147 JobStruct by 0x5A3BC0, copied by 0x5A2C50)
struct UIJobsInfoStruct	// NAME 1: neutral (D10)
{
	UIJobsInfoStruct() : Unknown00(0), Unknown04(""), Unknown14(""), Unknown24(""), Unknown34(""), Unknown44(0) {}	// (inline: 0x5A2C50, 0x5A3BC0)
	~UIJobsInfoStruct() { Unknown04.erase(); Unknown24.erase(); Unknown34.erase(); }	// 0x5A2250 (out of line after slot +0x58, the first caller)

	int Unknown00;										// +0x00: the category (JobStruct +0x04; 0 combat, 1 trade, 2 explore: what +0xD4 picks)
	std::string Unknown04;								// +0x04: the first column's text (JobStruct +0x10)
	std::string Unknown14;								// +0x14: the level (JobStruct +0x0C as text)
	std::string Unknown24;								// +0x24: the sponsor (JobStruct +0x20)
	std::string Unknown34;								// +0x34: the reward (JobStruct +0x30)
	int Unknown44;										// +0x44: the job id (JobStruct +0x00)
};

// One row of the list (0x10 bytes, ten at +0x14C; constructor 0x5A1390 and destructor 0x5A13B0 out of line after the
// window's constructor, for its vector constructor)
struct UIJobsRowStruct	// NAME 1: neutral (D10)
{
	~UIJobsRowStruct() { Type = NULL; Level = NULL; Sponsor = NULL; Reward = NULL; }	// 0x5A13B0

	RefCountPtr<FontSys> Type;							// +0x00 'Type%02d_JNEWS00'  // NAME 2: its style's name
	RefCountPtr<FontSys> Level;							// +0x04 'Level%02d_JNEWS00'  // NAME 2: its style's name
	RefCountPtr<FontSys> Sponsor;						// +0x08 'Sponsor%02d_JNEWS00'  // NAME 2: its style's name
	RefCountPtr<FontSys> Reward;						// +0x0C 'Reward%02d_JNEWS00'  // NAME 2: its style's name
};

// ui_jobs.cpp's job board window (0x1F8 bytes, tables 0xAF27E4 / 0xAF27D8 / 0xAF27CC; made by 0x69F8F0, sbaseview.cpp,
// case 8: the starbase view's +0x194)
class UIJobsClass : public Unknown5BB320Class	// NAME 3: class-from-file (D24) for app\client\ui_jobs.cpp
{
public:
	UIJobsClass(SClient *client);						// 0x5A1170 (the starbase view's 0x69F8F0, case 8)  // NAME 1 [client]: what 0x69F8F0 passes
	virtual ~UIJobsClass();								// 0x5A1540 (G_ASSERT 'State == UI_CLOSED', l.98; deleting 0x5A1510)
	virtual void Unknown_54();							// 0x5A1760
	virtual void Unknown_58();							// 0x5A2050
	virtual void Unknown_5C();							// 0x5A2480
	virtual void Unknown_60();							// 0x5A2560
	virtual bool Unknown_68(int key);					// 0x5A25B0  // NAME 1 [key]: InterfaceClass's
	virtual void Unknown_7C(UIViewClass *view, bool arg);	// 0x5A2010  // NAME 1 [view, arg]: InterfaceClass's
	void Unknown_5A2730(int row);						// 0x5A2730: row picked (FAIL_IFs l.358 / l.359), its description asked for  // NAME 1 [row]: neutral
	void Unknown_5A2900(int index, bool show, const char *text);	// 0x5A2900 (FAIL_IF '!font', l.380; no caller)  // NAME 1 [index, show, text]: as ui_manu.cpp's 0x5B1AF0
	void Unknown_5A2A30(int column);					// 0x5A2A30: the sort column picked (again: the order turned round)  // NAME 1 [column]: neutral
	int Unknown_5A2AA0(UIJobsInfoStruct *a, UIJobsInfoStruct *b, int column);	// 0x5A2AA0: a against b on column  // NAME 1 [a, b, column]: neutral
	void Unknown_5A2C50();								// 0x5A2C50: the jobs of the category, sorted, into JobInfo
	void Unknown_5A2FB0(int row);						// 0x5A2FB0: the row's texts highlighted when it is the pick  // NAME 1 [row]: neutral
	void Unknown_5A3090();								// 0x5A3090: the rows refilled
	void Unknown_5A3240();								// 0x5A3240: the buttons by the pick
	void Unknown_5A32B0();								// 0x5A32B0: the slider to the top, the texts and the rows emptied
	void Unknown_5A3390(int count);						// 0x5A3390: the slider over count rows  // NAME 1 [count]: neutral
	void Unknown_5A3400();								// 0x5A3400: the first row from the slider (the rows refilled when it moved)
	void Unknown_5A3440(int category);					// 0x5A3440: a category picked ('COMBAT JOBS' / 'TRADE JOBS' / 'EXPLORE JOBS')  // NAME 1 [category]: neutral
	void Unknown_5A36F0();								// 0x5A36F0: the pick accepted (StarbaseRequestPacketClass 8)
	void Unknown_5A37D0();								// 0x5A37D0: JobInfo emptied
	void Unknown_5A3890();								// 0x5A3890: the job list emptied
	void Unknown_5A3930(const char *title, const char *detail, const char *item);	// 0x5A3930: the description's texts  // NAME 1 [title, detail, item]: neutral
	void Unknown_5A3AF0();								// 0x5A3AF0: the list rebuilt for the category
	void Unknown_5A3BC0(JobListPacketClass *packet);	// 0x5A3BC0 (JOB_LIST 147)  // NAME 1 [packet]: the handler's packet
	void Unknown_5A3F10(JobDescriptionPacketClass *packet);	// 0x5A3F10 (JOB_DESCRIPTION 148)  // NAME 1 [packet]: the handler's packet
	void Unknown_5A3FB0(JobAcceptReplyPacketClass *packet);	// 0x5A3FB0 (JOB_ACCEPT_REPLY 150; FAIL_IF '!jobtitle', l.826)  // NAME 1 [packet]: the handler's packet
	void Unknown_Remove_Job(int job_id);					// (inline in 0x5A3FB0 / 0x5A4430) the job taken out of the list  // NAME 1: neutral (D10)  // NAME 1 [job_id]: neutral
	void Unknown_5A4430(int job_id);					// 0x5A4430 (JOB_DELETE 149)  // NAME 1 [job_id]: the packet's JobID
	static int Unknown_5A45A0(int category);			// 0x5A45A0 (cdecl; G_ASSERT(0), l.921): the category's button  // NAME 1 [category]: neutral

	GadgetSlider *Unknown0AC;							// +0x0AC (the list's slider)
	RefCountPtr<UIJobsInvClass> Unknown0B0;				// +0x0B0 'JNEWS00.C00'
	int Unknown0B4;										// +0x0B4 the first row shown
	int Unknown0B8;										// +0x0B8
	int State;											// +0x0BC (UI_CLOSED when shut)  // NAME 3: the assert's
	int Unknown0C0;										// +0x0C0
	int Unknown0C4;										// +0x0C4: a request pending (2 a description, 3 an accept; slot +0x68 waits)
	int Unknown0C8;										// +0x0C8
	int Unknown0CC;										// +0x0CC the pick in JobInfo (-1 for none)
	int Unknown0D0;										// +0x0D0 the pick's job id (0 for none)
	int Unknown0D4;										// +0x0D4 the category shown (-1 for none)
	bool Unknown0D8;									// +0x0D8 the order turned round
	std::vector<UIJobsInfoStruct *> JobInfo;			// +0x0DC the category's jobs, sorted  // NAME 3: the FAIL_IF's
	std::list<UIJobsInfoStruct *> Unknown0EC;			// +0x0EC every job (JOB_LIST 147 adds, JOB_DELETE 149 takes out)
	ButtonGadgetClass *Unknown0F8[11];					// +0x0F8 slot +0x54's buttons 1..10 ('exit', 'exit_button', 'button00'..'button02', 'name', 'Level', 'Sponsor', 'Reward', 'accept')
	ButtonGadgetClass *Unknown124[10];					// +0x124 the rows' buttons ('name%02d')
	UIJobsRowStruct Unknown14C[10];						// +0x14C the rows' texts
	int Unknown1EC[3];									// +0x1EC the sort columns, first the main one
};

#endif // UI_JOBS_H
