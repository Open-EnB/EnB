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
class UIStarbaseClass : public UIUnknownBaseClass, public RefCountClass	// NAME 3: class-from-file (D24) for app\client\ui_starbase.cpp
{
public:
	void Unknown_63CF30(int avatar_id, int new_room);	// 0x63CF30 (STARBASE_AVATAR_ROOM_UPDATE 160)  // NAME 1 [avatar_id, new_room]: the packet's fields
	void Unknown_63E5C0(int avatar_id, float orient, Vector3 *position, int action);	// 0x63E5C0 (STARBASE_AVATAR_UPDATE 158)  // NAME 1 [avatar_id, orient, position, action]: the packet's fields
	char Unknown40[0x7C - 0x40]; bool Unknown7C; char Unknown7D[0xAC - 0x7D];	// +0x40; +0x7C both handlers test it
	int Room;	// +0xAC (158 moves an avatar only in this room)  // NAME 2: what the handler compares with the packet's Room
	void Unknown_640150();	// 0x640150 (SClient 0x74ABC0: '/starbase reset')
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
class SBaseViewClass : public ViewClass	// NAME 3: class-from-file (D24) for app\client\sbaseview.cpp
{
public:
	void Unknown_6A0FD0(LoungeNPCPacketClass *packet);	// 0x6A0FD0 (LOUNGE_NPC 82)  // NAME 1 [packet]: the handler's packet
	RefCountPtr<UIJobsClass> Get_Jobs() { return(Jobs); }	// 0x73A300 (out of line after JOB_LIST 147, the first caller)  // NAME 2: what it returns
	RefCountPtr<UIStarbaseClass> Get_Starbase() { return(Starbase); }	// 0x54EDA0 (out of line in another object)  // NAME 2: what it returns
	char Unknown38[0x60 - 0x38]; RefCountPtr<UIStarbaseClass> Starbase; char Unknown64[0x194 - 0x64];	// +0x38; +0x60 ui_starbase.cpp's interface  // NAME 2 [Starbase]: what it holds
	RefCountPtr<UIJobsClass> Jobs; int Unknown198; SBaseViewUnknown19CClass *AvatarUI; SBaseViewUnknown1A0Class *ShipUI;	// +0x194  // NAME 2 [Jobs]: what it holds; +0x19C / +0x1A0  // NAME 3 [AvatarUI, ShipUI]: the recustomize handlers' '!avatar_ui' / '!ship_ui' tests
	UIPTradeClass *PTrade;	// +0x1A4 (TRADE 31 in a starbase)  // NAME 3: its class's file (ui_ptrade.cpp)
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
