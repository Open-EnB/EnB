/*
**	Earth & Beyond client (reconstruction)
**	sbaseview.h: sbaseview.cpp's starbase view (SClient +0x135C) and the interfaces it holds, declared as far as
**	cpacket.cpp's handlers call them (moved out of cpacket.cpp; this header's name is provisional, NAME 1). Notes
**	at the end.
*/
#if defined(_MSC_VER)
#pragma once
#endif
#ifndef SBASEVIEW_H  // NAME 1 [sbaseview]: provisional, the view's file (sbaseview.cpp, D24)
#define SBASEVIEW_H

#include "sclient.h"
#include "ui_line_display.h"
#include "ref_ptr.h"
#include "vector3.h"

class JobListPacketClass;
class JobDescriptionPacketClass;
class JobAcceptReplyPacketClass;
class LoungeNPCPacketClass;
class AvatarDescClass;
class ShipCustDescClass;
class ShipRecustomizationCostClass;
class RecustomizationCostClass;

// ui_starbase.cpp's starbase interface (the starbase view's +0x60), declared as far as the starbase avatar packets call it
class UIStarbaseClass : public InterfaceClass	// NAME 3: class-from-file (D24) for app\client\ui_starbase.cpp
{
public:
	void Unknown_63CF30(int avatar_id, int new_room);	// 0x63CF30 (STARBASE_AVATAR_ROOM_UPDATE 160)  // NAME 1 [avatar_id, new_room]: the packet's fields
	void Unknown_63E5C0(int avatar_id, float orient, Vector3 *position, int action);	// 0x63E5C0 (STARBASE_AVATAR_UPDATE 158)  // NAME 1 [avatar_id, orient, position, action]: the packet's fields
	char Unknown84[0xAC - 0x84];	// +0x84 (+0x7C Active: both handlers test it)
	int Room;	// +0xAC (158 moves an avatar only in this room)  // NAME 2: what the handler compares with the packet's Room
	void Unknown_640150(); void Unknown_6425D0(const char *name, const Vector3 &color);	// 0x640150 (SClient 0x74ABC0: '/starbase reset'); 0x6425D0 (SClient 0x750F70: the chat-event colour)  // NAME 1 [name, color]: what 0x750F70 passes
	void Unknown_6437A0(int unknown, AvatarDescClass *desc);	// 0x6437A0 (RECUSTOMIZE_AVATAR_UPDATE 133)  // NAME 1 [unknown, desc]: the packet's fields
	void Unknown_640110(int state);	// 0x640110 (TRADE 31: 8 when a trade opens, 0 when it ends)  // NAME 1 [state]: neutral
};

// ui_ptrade.cpp's player trade interface: the main view's +0x1E4 (sclient.h's MainViewUnknown1E4Class) and the
// starbase view's +0x1A4, declared as far as TRADE 31 calls it (it adds no members)
class UIPTradeClass : public MainViewUnknown1E4Class	// NAME 3: class-from-file (D24) for app\client\ui_ptrade.cpp
{
public:
	void Unknown_5E2F70(bool on);	// 0x5E2F70 (TRADE 31 types 3 / 4)  // NAME 1 [on]: what the handler passes
	void Unknown_5E3050(bool on);	// 0x5E3050 (TRADE 31 types 5 / 6)  // NAME 1 [on]: what the handler passes
	void Unknown_5E3130();	// 0x5E3130 (TRADE 31 type 2)
};

class SBaseViewUnknown174Class { public: char Unknown00[0x18C]; bool Unknown18C; };	// NAME 1: neutral (D10): the starbase view's +0x174 (SClient 0x74D950 sets its +0x18C)
class SBaseViewUnknown1A0Class { public: void Unknown_61F250(ShipCustDescClass *desc, ShipRecustomizationCostClass *cost, int a, int b, int c, int d); };	// NAME 1: neutral (D10): the starbase view's +0x1A0 (RECUSTOMIZE_SHIP_START 129: 0x61F250)  // NAME 1 [desc, cost, a, b, c, d]: the packet's fields
class SBaseViewUnknown19CClass { public: void Unknown_551C40(RecustomizationCostClass *cost); };	// NAME 1: neutral (D10): the starbase view's +0x19C (RECUSTOMIZE_AVATAR_START 131: 0x551C40)  // NAME 1 [cost]: the packet's field

// ui_jobs.cpp's interface (an InterfaceClass: its reference count at +0x3C) and sbaseview.cpp's view
// (SClient +0x135C; LOUNGE_NPC 82 calls its 0x6A0FD0) that holds it, declared as far as the job packets call them
class UIJobsClass : public InterfaceClass	// NAME 3: class-from-file (D24) for app\client\ui_jobs.cpp
{
public:
	void Unknown_5A3BC0(JobListPacketClass *packet);	// 0x5A3BC0 (JOB_LIST 147)  // NAME 1 [packet]: the handler's packet
	void Unknown_5A3F10(JobDescriptionPacketClass *packet);	// 0x5A3F10 (JOB_DESCRIPTION 148)  // NAME 1 [packet]: the handler's packet
	void Unknown_5A3FB0(JobAcceptReplyPacketClass *packet);	// 0x5A3FB0 (JOB_ACCEPT_REPLY 150)  // NAME 1 [packet]: the handler's packet
	void Unknown_5A4430(int job_id);	// 0x5A4430 (JOB_DELETE 149)  // NAME 1 [job_id]: the packet's JobID
};
class FontSys;	// fontsys.cpp's text object, a render object (sbaseview.cpp declares it)
class SBaseViewUnknown68Class { public: RenderObjClass *Unknown00; };	// NAME 1: neutral (D10): what the starbase view's +0x68 points to (0x6A0540)
class SBaseViewUnknown1B4Class;	// NAME 1: neutral (D10): what sbase_info.cpp's 0x6987A0 returns (the starbase view's +0x1B4)
class SBaseViewClass : public UIViewClass	// NAME 3: class-from-file (D24) for app\client\sbaseview.cpp
{
public:
	void Unknown_6A0FD0(LoungeNPCPacketClass *packet);	// 0x6A0FD0 (LOUNGE_NPC 82)  // NAME 1 [packet]: the handler's packet
	RefCountPtr<UIJobsClass> Get_Jobs() { return(Jobs); }	// 0x73A300 (out of line after JOB_LIST 147, the first caller)  // NAME 2: what it returns
	RefCountPtr<UIStarbaseClass> Get_Starbase() { return(Starbase); }	// 0x54EDA0 (out of line in another object)  // NAME 2: what it returns
	void Unknown_69D930(int type);	// 0x69D930: ViewRObjs[type] out of its scene and released  // NAME 1 [type]: the assert's index
	void Unknown_69D9D0(int type, const char *name);	// 0x69D9D0 (G_ASSERT 'ViewRObjs[type]', l.738): the asset manager's name into ViewRObjs[type]  // NAME 1 [name]: what 0x69DC40 passes
	void Unknown_69DB50(int type);	// 0x69DB50: ViewRObjs[type] released
	void Unknown_69DC40();	// 0x69DC40: 'FE_COMP1' as ViewRObjs[2]
	void Unknown_69DC60();	// 0x69DC60: ViewRObjs[2] away
	void Unknown_69F440();	// 0x69F440: +0x1B0 set
	void Unknown_69F460(int arg);	// 0x69F460  // NAME 1 [arg]: neutral
	void Unknown_6A0540();	// 0x6A0540: +0x68's object, +0x1CC and +0x1D0 out of their scenes
	void Unknown_6A0E00();	// 0x6A0E00: the text object +0x1B8 away
	void Unknown_6A0E50();	// 0x6A0E50: the text object hidden
	void Unknown_6A0E80(const char *text);	// 0x6A0E80: the text object's text, shown  // NAME 1 [text]: what FontSys::Change_Text gets
	void Unknown_6A20C0(const Vector3 &color, float start, float end);	// 0x6A20C0: the +0x70 scene's fog on  // NAME 2 [color, start, end]: SceneClass's Set_Fog_Color / Set_Fog_Range
	void Unknown_6A2100();	// 0x6A2100: the +0x70 scene's fog off
	void Unknown_6A23E0(bool a, bool b, bool c, bool d, bool e);	// 0x6A23E0: +0x1EC..+0x1F0  // NAME 1 [a, b, c, d, e]: neutral
	void Unknown_6A2430(bool *a, bool *b, bool *c, bool *d, bool *e);	// 0x6A2430: +0x1EC..+0x1F0 back  // NAME 1 [a, b, c, d, e]: neutral

	int Unknown4C;	// +0x4C
	int Unknown50;	// +0x50 (0x69F460 copies it to +0x58)
	int Unknown54;	// +0x54 (0x69F460's argument)
	int Unknown58;	// +0x58
	int Unknown5C;	// +0x5C
	RefCountPtr<UIStarbaseClass> Starbase;	// +0x60 ui_starbase.cpp's interface  // NAME 2 [Starbase]: what it holds
	int Unknown64;	// +0x64
	SBaseViewUnknown68Class *Unknown68;	// +0x68 (0x6A0540 removes its +0x00's render object)
	int Unknown6C;	// +0x6C
	SimpleSceneClass *Unknown70;	// +0x70 (ViewRObjs[1]'s scene; 0x6A20C0 / 0x6A2100: its fog)
	int Unknown74;	// +0x74
	SimpleSceneClass *Unknown78;	// +0x78 (ViewRObjs[2]'s scene)
	char Unknown7C[0xEC - 0x7C];	// +0x7C
	RefCountPtr<RenderObjClass> ViewRObjs[3];	// +0xEC: by type, in the scenes +0x1C / +0x70 / +0x78 (0x69D930)  // NAME 3: the assert's 'ViewRObjs[type]'
	char UnknownF8[0x174 - 0xF8];	// +0xF8
	SBaseViewUnknown174Class *Unknown174;	// +0x174
	char Unknown178[0x194 - 0x178];	// +0x178
	RefCountPtr<UIJobsClass> Jobs; int Unknown198; SBaseViewUnknown19CClass *AvatarUI; SBaseViewUnknown1A0Class *ShipUI;	// +0x194  // NAME 2 [Jobs]: what it holds; +0x19C / +0x1A0  // NAME 3 [AvatarUI, ShipUI]: the recustomize handlers' '!avatar_ui' / '!ship_ui' tests
	UIPTradeClass *PTrade;	// +0x1A4 (TRADE 31 in a starbase)  // NAME 3: its class's file (ui_ptrade.cpp)
	char Unknown1A8[0x1B0 - 0x1A8];	// +0x1A8
	bool Unknown1B0;	// +0x1B0 (0x69F440 sets it, 0x69CDE0 clears it)
	SBaseViewUnknown1B4Class *Unknown1B4;	// +0x1B4 (LOUNGE_NPC 82: sbase_info.cpp's 0x6987A0 of the packet)
	RefCountPtr<FontSys> Unknown1B8;	// +0x1B8: a text object (0x6A0E80 changes its text)
	char Unknown1BC[0x1CC - 0x1BC];	// +0x1BC
	RenderObjClass *Unknown1CC;	// +0x1CC (0x6A0540 removes it)
	RenderObjClass *Unknown1D0;	// +0x1D0 (0x6A0540 removes it)
	char Unknown1D4[0x1EC - 0x1D4];	// +0x1D4
	bool Unknown1EC; bool Unknown1ED; bool Unknown1EE; bool Unknown1EF; bool Unknown1F0;	// +0x1EC..+0x1F0 (0x6A23E0 / 0x6A2430)
};

/*
**	Reconstruction notes (sbaseview.h)
**
**	The classes were declared in cpacket.cpp (CP1 moved them here for sclient.cpp's 0x74D950, P-SC1-1). The view's
**	layout is known at +0x60 (the starbase interface, handed out by the inline Get_Starbase, out of line at
**	0x54EDA0), +0x194 (the jobs interface, Get_Jobs out of line at 0x73A300), +0x19C and +0x1A0 (the recustomize
**	interfaces); the rest is padding up to what the handlers read.
*/
#endif // SBASEVIEW_H
