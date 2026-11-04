// Earth & Beyond client (reconstruction)
/*
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
#include "matrix3d.h"

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
	void Unknown_6435E0();	// 0x6435E0 (ui_avatar_cust.cpp 0x54EC30, once the new look is sent)
	virtual void Unknown_88();	// +0x88
	virtual void Unknown_8C();	// +0x8C (0x69CDE0 calls it when the view's interfaces go)
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

class SBaseViewUnknown1A0Class : public InterfaceClass { public: void Unknown_61F250(ShipCustDescClass *desc, ShipRecustomizationCostClass *cost, int a, int b, int c, int d); };	// NAME 1: neutral (D10): the starbase view's +0x1A0 (RECUSTOMIZE_SHIP_START 129: 0x61F250)  // NAME 1 [desc, cost, a, b, c, d]: the packet's fields
class SBaseViewUnknown19CClass : public InterfaceClass { public: void Unknown_551C40(RecustomizationCostClass *cost); };	// NAME 1: neutral (D10): the starbase view's +0x19C (RECUSTOMIZE_AVATAR_START 131: 0x551C40)  // NAME 1 [cost]: the packet's field

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
class SBaseViewUnknown190Class : public InterfaceClass { public: char Unknown84[0x160 - 0x84]; int Unknown160; };	// NAME 1: neutral (D10): the starbase view's +0x190 (0x69CCD0 tests its +0x160 with Active)
class SBaseViewUnknown1B4Class { public: ~SBaseViewUnknown1B4Class(); };	// NAME 1: neutral (D10): what sbase_info.cpp's 0x6987A0 returns (the starbase view's +0x1B4; the view's destructor 0x69B0C0 deletes it through 0x698370)
class SBaseViewUnknown174Class : public InterfaceClass { public: char Unknown84[0x18C - 0x84]; bool Unknown18C; };	// NAME 1: neutral (D10): the starbase view's +0x174 (SClient 0x74D950 sets its +0x18C; released as an interface by 0x69B0C0)
class HAnimClass;
class AudibleSoundClass;
// one of the starbase view's six animation channels (0x14 bytes at +0xF8; 0x69F480 / 0x69F710 / 0x69F850; the
// constructor 0x69AC20 and destructor 0x69AC50 are the view's vector constructor's)
struct SBaseViewChannelStruct	// NAME 1: neutral (D10)
{
	SBaseViewChannelStruct();	// 0x69AC20
	~SBaseViewChannelStruct();	// 0x69AC50

	RefCountPtr<HAnimClass> Anim;	// +0x00 (Get_Num_Frames, Get_Frame_Rate)  // NAME 2: what it holds
	RefCountPtr<RenderObjClass> Model;	// +0x04 (its Set_Animation)  // NAME 2: what it holds
	int Mode;	// +0x08: Set_Animation's mode; set until 0x69F710 starts the animation  // NAME 2: Set_Animation's argument
	float Frame;	// +0x0C  // NAME 2: what it holds
	bool Backward;	// +0x10: played from the last frame down  // NAME 2: what it does
};
// the starbase view's +0x1F4 (0x4C bytes: the view is 0x240, SClient 0x74B680)
class SBaseViewUnknown1F4Class	// NAME 1: neutral (D10): 0x441A50, starbasesound.cpp's neighbourhood
{
public:
	SBaseViewUnknown1F4Class(int arg);	// 0x441910 (the view's constructor passes its second argument)  // NAME 1 [arg]: neutral
	~SBaseViewUnknown1F4Class();	// 0x4419B0
	void Unknown_441A50(int arg);	// 0x441A50  // NAME 1 [arg]: what 0x6A2480 passes
	int Unknown00[0x4C / 4];	// +0x00 (0x441910 stores dwords up to +0x48)
};
class SBaseViewClass : public Unknown65B810ViewClass	// NAME 3: class-from-file (D24) for app\client\sbaseview.cpp
{
public:
	SBaseViewClass(ClientTaskClass *task, int arg);	// 0x69AD30 (table 0xAFD2FC; SClient 0x74B680 passes its +0x1384 task and +0x1324)  // NAME 1 [task, arg]: what the base / +0x1F4 get
	virtual ~SBaseViewClass();	// +0x00 0x69B0C0 (deleting 0x69B090)
	virtual void Unknown_04();	// +0x04 0x69B5E0 (not rebuilt)
	virtual int Unknown_0C(bool on);	// +0x0C 0x69C5E0 (not rebuilt: the render, WW3D::Begin_Render with on)
	virtual void Unknown_10();	// +0x10 0x69C0D0 (not rebuilt)
	virtual void Unknown_14();	// +0x14 0x69C8F0 (not rebuilt: the frame, the base's 0x6B4860 first)
	virtual int Unknown_30() { return((int)this); }	// +0x30 0x69B050 (out of line after the constructor)
	virtual SimpleSceneClass *Unknown_5C() { return(Unknown70.Peek()); }	// +0x5C 0x69B070 (the view's own slot; out of line after the constructor)
	virtual void Unknown_18(int arg);	// +0x18 0x69C5A0 (its byte: set, +0x54 = 0 and +0x58 = +0x50; else 0x69D030(-1))  // NAME 1 [arg]: neutral
	void Unknown_69D030(int arg);	// 0x69D030 (not rebuilt)  // NAME 1 [arg]: neutral
	void Unknown_6A1000(const char *name);	// 0x6A1000: the sound +0x1C0, looped  // NAME 1 [name]: what SoundLibraryClass::Play_2D_Sound gets
	void Unknown_6A1130();	// 0x6A1130: the sound +0x1C0 stopped
	void Unknown_6A2480();	// 0x6A2480 (FAIL_IF 'Client == NULL', l.1667)
	void Unknown_6A0FD0(LoungeNPCPacketClass *packet);	// 0x6A0FD0 (LOUNGE_NPC 82)  // NAME 1 [packet]: the handler's packet
	RefCountPtr<UIJobsClass> Get_Jobs() { return(Jobs); }	// 0x73A300 (out of line after JOB_LIST 147, the first caller)  // NAME 2: what it returns
	RefCountPtr<UIStarbaseClass> Get_Starbase() { return(Starbase); }	// 0x54EDA0 (out of line in another object)  // NAME 2: what it returns
	bool Unknown_69CBE0();	// 0x69CBE0: the interface +0x64 active (false while +0x1B0 is set)
	bool Unknown_69CC10();	// 0x69CC10: any of the interfaces +0x184..+0x1A0 active (not +0x190)
	bool Unknown_69CCD0();	// 0x69CCD0: any of the view's interfaces +0x184..+0x1A4 active (+0x190: or its +0x160 set)
	void Unknown_69CDE0();	// 0x69CDE0: +0x1B0 cleared; with an interface at +0x64, it and +0x184..+0x1A0 released
	void Unknown_69D930(int type);	// 0x69D930: ViewRObjs[type] out of its scene and released  // NAME 1 [type]: the assert's index
	void Unknown_69D9D0(int type, const char *name);	// 0x69D9D0 (G_ASSERT 'ViewRObjs[type]', l.738): the asset manager's name into ViewRObjs[type]  // NAME 1 [name]: what 0x69DC40 passes
	void Unknown_69DB50(int type);	// 0x69DB50: ViewRObjs[type] released
	void Unknown_69DBA0(int type, const RefCountPtr<RenderObjClass> &obj);	// 0x69DBA0: obj as ViewRObjs[type]  // NAME 1 [obj]: neutral
	void Unknown_69DC40();	// 0x69DC40: 'FE_COMP1' as ViewRObjs[2]
	void Unknown_69DC60();	// 0x69DC60: ViewRObjs[2] away
	void Unknown_69F440();	// 0x69F440: +0x1B0 set
	void Unknown_69F460(int arg);	// 0x69F460  // NAME 1 [arg]: neutral
	void Unknown_69F480(int index);	// 0x69F480: channel index stopped and released  // NAME 1 [index]: neutral
	void Unknown_69F710();	// 0x69F710: every channel one frame on
	bool Unknown_69F850(int index);	// 0x69F850: channel index at its end  // NAME 1 [index]: neutral
	void Unknown_6A0540();	// 0x6A0540: +0x68's object, +0x1CC and +0x1D0 out of their scenes
	void Unknown_6A0E00();	// 0x6A0E00: the text object +0x1B8 away
	void Unknown_6A0E50();	// 0x6A0E50: the text object hidden
	void Unknown_6A0E80(const char *text);	// 0x6A0E80: the text object's text, shown  // NAME 1 [text]: what FontSys::Change_Text gets
	void Unknown_6A20C0(const Vector3 &color, float start, float end);	// 0x6A20C0: the +0x70 scene's fog on  // NAME 2 [color, start, end]: SceneClass's Set_Fog_Color / Set_Fog_Range
	void Unknown_6A2100();	// 0x6A2100: the +0x70 scene's fog off
	void Unknown_6A23E0(bool a, bool b, bool c, bool d, bool e);	// 0x6A23E0: +0x1EC..+0x1F0  // NAME 1 [a, b, c, d, e]: neutral
	void Unknown_6A2430(bool *a, bool *b, bool *c, bool *d, bool *e);	// 0x6A2430: +0x1EC..+0x1F0 back  // NAME 1 [a, b, c, d, e]: neutral

	int Unknown50;	// +0x50 (0x69F460 copies it to +0x58; -1 from the constructor)
	int Unknown54;	// +0x54 (0x69F460's argument)
	int Unknown58;	// +0x58
	int Unknown5C;	// +0x5C
	RefCountPtr<UIStarbaseClass> Starbase;	// +0x60 ui_starbase.cpp's interface  // NAME 2 [Starbase]: what it holds
	RefCountPtr<InterfaceClass> Unknown64;	// +0x64 (0x69CDE0: its slot +0x7C with NULL, 0, then released)
	SBaseViewUnknown68Class *Unknown68;	// +0x68 (0x6A0540 removes its +0x00's render object; the destructor leaves it)
	RefCountPtr<CameraClass> Unknown6C;	// +0x6C (+0x70's camera: 0x69C5E0)
	RefCountPtr<SimpleSceneClass> Unknown70;	// +0x70 (ViewRObjs[1]'s scene; 0x6A20C0 / 0x6A2100: its fog)
	RefCountPtr<CameraClass> Unknown74;	// +0x74 (+0x78's camera: 0x69C5E0)
	RefCountPtr<SimpleSceneClass> Unknown78;	// +0x78 (ViewRObjs[2]'s scene)
	int Unknown7C;	// +0x7C
	int Unknown80;	// +0x80
	Matrix3D Unknown84;	// +0x84 (identity from the constructor)
	Matrix3D UnknownB4;	// +0xB4 (identity from the constructor)
	int UnknownE4;	// +0xE4
	int UnknownE8;	// +0xE8
	RefCountPtr<RenderObjClass> ViewRObjs[3];	// +0xEC: by type, in the scenes +0x1C / +0x70 / +0x78 (0x69D930)  // NAME 3: the assert's 'ViewRObjs[type]'
	SBaseViewChannelStruct Channels[6];	// +0xF8  // NAME 2: what it holds
	bool Unknown170;	// +0x170 (0x69C8F0: set, 0x69D030(-1) and cleared)
	RefCountPtr<SBaseViewUnknown174Class> Unknown174;	// +0x174
	RefCountPtr<InterfaceClass> Unknown178, Unknown17C, Unknown180;	// +0x178 (0x69C8F0 picks +0x174..+0x180 by +0x50)
	RefCountPtr<InterfaceClass> Unknown184, Unknown188, Unknown18C;	// +0x184 (0x69CDE0 releases them)
	RefCountPtr<SBaseViewUnknown190Class> Unknown190;	// +0x190
	RefCountPtr<UIJobsClass> Jobs; RefCountPtr<InterfaceClass> Unknown198; RefCountPtr<SBaseViewUnknown19CClass> AvatarUI; RefCountPtr<SBaseViewUnknown1A0Class> ShipUI;	// +0x194  // NAME 2 [Jobs]: what it holds; +0x19C / +0x1A0  // NAME 3 [AvatarUI, ShipUI]: the recustomize handlers' '!avatar_ui' / '!ship_ui' tests
	RefCountPtr<UIPTradeClass> PTrade;	// +0x1A4 (TRADE 31 in a starbase)  // NAME 3: its class's file (ui_ptrade.cpp)
	RefCountPtr<InterfaceClass> Unknown1A8;	// +0x1A8 (0x69C8F0: its slots +0x58 / +0x5C)
	RefCountPtr<InterfaceClass> Unknown1AC;	// +0x1AC
	bool Unknown1B0;	// +0x1B0 (0x69F440 sets it, 0x69CDE0 clears it)
	SBaseViewUnknown1B4Class *Unknown1B4;	// +0x1B4 (LOUNGE_NPC 82: sbase_info.cpp's 0x6987A0 of the packet; the destructor deletes it)
	RefCountPtr<FontSys> Unknown1B8;	// +0x1B8: a text object (0x6A0E80 changes its text)
	int Unknown1BC;	// +0x1BC
	RefCountPtr<AudibleSoundClass> Unknown1C0;	// +0x1C0: a 2-D sound (0x6A1000 plays it looped, 0x6A1130 stops it)
	int Unknown1C4;	// +0x1C4 (0x69C5E0 counts it down)
	int Unknown1C8;	// +0x1C8
	RefCountPtr<RenderObjClass> Unknown1CC;	// +0x1CC (0x6A0540 removes it)
	RefCountPtr<RenderObjClass> Unknown1D0;	// +0x1D0 (0x6A0540 removes it)
	Vector3 Unknown1D4;	// +0x1D4 (six zero dwords: as two Vector3s the constructor stores them before the +0x1F4 call, as the client)  // NAME 1 [Unknown1D4]: neutral; the type from the store order only
	Vector3 Unknown1E0;	// +0x1E0  // NAME 1 [Unknown1E0]: neutral; as Unknown1D4
	bool Unknown1EC; bool Unknown1ED; bool Unknown1EE; bool Unknown1EF; bool Unknown1F0;	// +0x1EC..+0x1F0 (0x6A23E0 / 0x6A2430)
	SBaseViewUnknown1F4Class Unknown1F4;	// +0x1F4 (0x6A2480)
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
