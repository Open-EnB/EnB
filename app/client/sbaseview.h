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
#include "unknown_490900.h"
#include <list>		// the starbase interface's lists

class JobListPacketClass;
class JobDescriptionPacketClass;
class JobAcceptReplyPacketClass;
class LoungeNPCPacketClass;
class AvatarDescClass;
class ShipCustDescClass;
class ShipRecustomizationCostClass;
class RecustomizationCostClass;
class LayerClass;
class CharacterViewAvatarClass;
class Unknown78A910AvatarClass;	// NAME 1: neutral (D10)

class SBaseViewClass;
class LightClass;

// What the starbase interface holds at +0xB4, as far as 0x640D80 reads it
struct UIStarbaseUnknownB4Struct	// NAME 1: neutral (D10)
{
	char Unknown00[0x0A];	// +0x00
	bool Unknown0A;			// +0x0A (0x640D80: 2, else 1)
	bool Unknown0B;			// +0x0B (0x640D80: 3)
};

// The starbase interface's +0xD4: an SGI hash table of plain nodes (0x18 bytes: the hasher, key test and key getter
// bytes, the bucket vector, the count), its three-argument constructor 0x643C20 (100 buckets) and destructor 0x633AC0
// called out of line. A stand-in of hash_map's shape (an inline (n) constructor over the table's three-argument one)
// until its key and value types are known: as a real std::hash_map<unsigned, int *> ours inlines the table's
// constructor into 0x6333E0 and calls _M_initialize_buckets out of line (its vector _Ucopy then duplicates 0x644430)
struct UIStarbaseUnknownD4Hash {};	// NAME 1: neutral (D10): the hasher
struct UIStarbaseUnknownD4Equal {};	// NAME 1: neutral (D10): the key test
class UIStarbaseUnknownD4TableClass	// NAME 1: neutral (D10)
{
public:
	struct NodeStruct { NodeStruct *Next; };	// the SGI table's _Node (its value unknown)  // NAME 1: neutral (D10)
	UIStarbaseUnknownD4TableClass(unsigned int n, const UIStarbaseUnknownD4Hash &hf, const UIStarbaseUnknownD4Equal &eql);	// 0x643C20  // NAME 1 [n, hf, eql]: the SGI table's
	~UIStarbaseUnknownD4TableClass() { clear(); }	// as SGI's (the map's implicit destructor 0x633AC0 inlines it all; 0x633B50 calls clear 0x644AE0 and the vector's 0x643ED0)
	void clear()	// 0x644AE0 (SGI's)
	{
		for (unsigned int i = 0; i < Buckets.size(); ++i) {
			NodeStruct *cur = Buckets[i];
			while (cur != 0) {
				NodeStruct *next = cur->Next;
				operator delete(cur);
				cur = next;
			}
			Buckets[i] = 0;
		}
		NumElements = 0;
	}
	char Unknown00[4];	// +0x00 (the hasher, key test and key getter bytes)
	std::vector<NodeStruct *> Buckets;	// +0x04 (SGI's _M_buckets)  // NAME 1: neutral (D10)
	unsigned int NumElements;	// +0x14 (SGI's _M_num_elements)  // NAME 1: neutral (D10)
};
class UIStarbaseUnknownD4Class	// NAME 1: neutral (D10)
{
public:
	explicit UIStarbaseUnknownD4Class(unsigned int n) : Table(n, UIStarbaseUnknownD4Hash(), UIStarbaseUnknownD4Equal()) {}	// NAME 1 [n]: the SGI map's
	UIStarbaseUnknownD4TableClass Table;	// +0x00  // NAME 1: neutral (D10)
};
// What the starbase interface's +0xCC and +0x13C point to (ui_starbase.cpp's class with the destructor 0x632E50: RefCountPtrs at +0x04, +0x0C, ...)
class UIStarbaseUnknownCCClass	// NAME 1: neutral (D10)
{
public:
	~UIStarbaseUnknownCCClass();	// 0x632E50 (not rebuilt)
	int Unknown00;					// +0x00  // NAME 1: neutral
	RefCountPtr<CharacterViewAvatarClass> Avatar;	// +0x04 (0x63BDB0 / 0x63C080 / 0x63D2F0)  // NAME 1: what it holds
	RefCountPtr<Unknown78A910AvatarClass> Unknown08;	// +0x08 (0x63D2F0)  // NAME 1: neutral
	char Unknown0C[0x14 - 0x0C];	// +0x0C
};
// What the starbase interface's +0x190 points to (a virtual destructor; its +0x90 goes to the client's slot +0x44)
class UIStarbaseUnknown190Class	// NAME 1: neutral (D10)
{
public:
	virtual ~UIStarbaseUnknown190Class();	// +0x00
	char Unknown04[0x90 - 0x04];	// +0x04
	int Unknown90;	// +0x90 (0x633B50 hands it to the client's slot +0x44)
};

// ui_starbase.cpp's starbase interface (the starbase view's +0x60), declared as far as the starbase avatar packets call it
class UIStarbaseClass : public Unknown5BB320Class	// NAME 3: class-from-file (D24) for app\client\ui_starbase.cpp
{
public:
	UIStarbaseClass(UIViewClass *view, LayerClass *layer, int index, const char *name);	// 0x6333E0 (0x224 bytes; the starbase view's 0x69D030: itself, its +0x80 layer, the index 0..3, 0x6A2120's name)  // NAME 1 [view, layer, index, name]: what 0x69D030 passes
	void Unknown_639030(int arg);	// 0x639030 (the starbase view's 0x69C8F0 passes its +0x190's 0x64CF90)  // NAME 1 [arg]: neutral
	void Unknown_63CF30(int avatar_id, int new_room);	// 0x63CF30 (STARBASE_AVATAR_ROOM_UPDATE 160)  // NAME 1 [avatar_id, new_room]: the packet's fields
	void Unknown_63E5C0(int avatar_id, float orient, Vector3 *position, int action);	// 0x63E5C0 (STARBASE_AVATAR_UPDATE 158)  // NAME 1 [avatar_id, orient, position, action]: the packet's fields
	void Unknown_6435E0();	// 0x6435E0 (ui_avatar_cust.cpp 0x54EC30, once the new look is sent)
	virtual ~UIStarbaseClass();	// +0x00 0x633B50 (deleting 0x633A90)
	virtual void Unknown_54(); virtual void Unknown_58(); virtual void Unknown_5C(); virtual void Unknown_60();	// +0x54 0x634140, +0x58 0x6366A0, +0x5C 0x636D50, +0x60 0x638390 (the client's overrides, table 0xAF836C: declared so the constructor's table doesn't emit the base's inline defaults; not rebuilt)
	virtual bool Unknown_68(int key); virtual void Unknown_70(ControlKeysManager *manager); virtual void Unknown_74(ControlKeysManager *manager);	// +0x68 0x638500, +0x70 0x637130, +0x74 0x637A90 (not rebuilt)  // NAME 1 [key, manager]: InterfaceClass's
	virtual void Unknown_7C(UIViewClass *view, bool arg);	// +0x7C 0x638FF0 (not rebuilt)  // NAME 1 [view, arg]: InterfaceClass's
	virtual void Unknown_88();	// +0x88
	virtual void Unknown_8C();	// +0x8C (0x69CDE0 calls it when the view's interfaces go)
	// +0x84 the base's InterfaceMesh (both handlers test +0x7C Active; the starbase view's 0x69C8F0 hands +0x84 to its +0x1F4's 0x441A90)
	class SBaseViewUnknown1B4Class *Unknown_Info();	// inline in ui_starbase.cpp: the view's +0x1B4 (0x6333E0 reads it through this: it loads +0xB0 before Room, and the inline spends the budget that keeps the lists' begin() out of line)  // NAME 1: neutral (D10)
	RenderObjClass *Unknown_Model() { return(InterfaceMesh->InterfaceMesh); }	// the mesh's model (0x6333E0: the same budget)  // NAME 1: neutral (D10)
	int Room;	// +0xAC (158 moves an avatar only in this room; the constructor's index)  // NAME 2: what the handler compares with the packet's Room
	SBaseViewClass *UnknownB0;	// +0xB0 (the starbase view: 0x640D80 reads its +0x1B4's +0x18, 0x640E90 / 0x640EC0 call its +0x70 scene)
	UIStarbaseUnknownB4Struct *UnknownB4;	// +0xB4 (0x640D80: its bytes +0x0A / +0x0B; the constructor's 0x698730 of the room's key)
	int UnknownB8;	// +0xB8
	RefCountPtr<RenderObjClass> UnknownBC;	// +0xBC (the constructor's '%sextra_%s' model, added to the mesh's model)
	struct UnknownC0Struct { UnknownC0Struct() : A(-1), B(-1), C(-1), D(0), E(0) {} int A, B, C; UIStarbaseUnknownCCClass *D; int E; } UnknownC0;	// +0xC0 (-1, -1, -1, 0, 0: as a struct with its own constructor its stores follow the table's arguments, as the client's; five ints in the list come before them)  // NAME 1: neutral (D10)
	UIStarbaseUnknownD4Class UnknownD4;	// +0xD4 (100 buckets)
	std::list<int> UnknownEC;	// +0xEC (destructor 0x6439E0: plain elements)  // NAME 1: neutral (D10): element type unknown
	int UnknownF8[4];	// +0xF8
	RefCountPtr<RenderObjClass> Unknown108[4];	// +0x108 (vector constructor 0x5464F0 / 0x4740B0)
	int Unknown118[4];	// +0x118
	RefCountPtr<RenderObjClass> Unknown128[4];	// +0x128
	bool Unknown138[4];	// +0x138
	UIStarbaseUnknownCCClass *Unknown13C[14];	// +0x13C (the destructor deletes them)
	std::list<unsigned int> Unknown174;	// +0x174 (destructor 0x643A50: plain elements, another type than +0xEC's)  // NAME 1: neutral (D10): element type unknown
	GadgetClass *Unknown180;	// +0x180 (the destructor's GadgetClass::Destroy)
	RefCountPtr<RenderObjClass> Unknown184, Unknown188;	// +0x184
	bool Unknown18C;	// +0x18C (SClient 0x74D950 sets the starbase view's +0x174's)
	UIStarbaseUnknown190Class *Unknown190;	// +0x190 (the destructor deletes it)
	std::list<RefCountPtr<RenderObjClass> > Unknown194, Unknown1A0;	// +0x194 (made with NULL: list(0), insert 0x644070; destructor 0x643AC0)
	int Unknown1AC;	// +0x1AC
	bool Unknown1B0, Unknown1B1, Unknown1B2, Unknown1B3;	// +0x1B0
	float Unknown1B4, Unknown1B8;	// +0x1B4 (-1, 0.5)
	char Unknown1BC[0x1EC - 0x1BC];	// +0x1BC (not set by the constructor)
	Vector3 Unknown1EC;	// +0x1EC
	RefCountPtr<InterfaceClass> Unknown1F8;	// +0x1F8 (destructor 0x643BA0: a reference count at +0x38, an interface)  // NAME 1: neutral (D10): the interface's class unknown
	bool Unknown1FC, Unknown1FD, Unknown1FE, Unknown1FF, Unknown200, Unknown201;	// +0x1FC
	std::vector<LightClass *> Lights;	// +0x204 (0x640CC0 removes and releases them; 0x640DF0 hands a copy to the lights panel; destructor 0x643BE0)  // NAME 1: neutral
	bool Unknown214, Unknown215, Unknown216, Unknown217, Unknown218, Unknown219, Unknown21A, Unknown21B, Unknown21C;	// +0x214
	int Unknown220;	// +0x220
	void Unknown_640150(); void Unknown_6425D0(const char *name, const Vector3 &color);	// 0x640150 (SClient 0x74ABC0: '/starbase reset'); 0x6425D0 (SClient 0x750F70: the chat-event colour)  // NAME 1 [name, color]: what 0x750F70 passes
	void Unknown_6437A0(int unknown, AvatarDescClass *desc);	// 0x6437A0 (RECUSTOMIZE_AVATAR_UPDATE 133)  // NAME 1 [unknown, desc]: the packet's fields
	void Unknown_640110(int state);	// 0x640110 (TRADE 31: 8 when a trade opens, 0 when it ends)  // NAME 1 [state]: neutral
	int Unknown_Kind() { int c = 1; if (UnknownB4->Unknown0B) { c = 3; } else if (UnknownB4->Unknown0A) { c = 2; } return(c); }	// NAME 1: neutral (D10): the third light key as 0x640810 inlines it (an inline level: it moves 0x640810's budget to the client's out-of-line calls; 0x640D80 has its own setne form)
	void Unknown_640810();	// 0x640810 (0x6366A0: the lights rebuilt from the model's D_LIGHTnn bones, else one default light)
	void Unknown_640CC0();	// 0x640CC0 (no caller: the lights removed from their scene, released and cleared)
	void Unknown_640D80(int *a, int *b, int *c); void Unknown_640F10();	// 0x640D80 (+0xB0's +0x1B4's +0x18, Room, 1..3 from +0xB4); 0x640F10 (empty: ret); the lights panel's 0x6466F0 / 0x6467B0  // NAME 1 [a, b, c]: neutral
	std::vector<LightClass *> Unknown_640DF0();	// 0x640DF0 (a copy of the vector at +0x204: the lights panel's 0x646A00)
	void Unknown_640E90(const Vector3 &color); void Unknown_640EC0(float fill); void Unknown_640EF0(int index); void Unknown_640F30(int index, float range);	// 0x640E90 / 0x640EC0 (+0xB0's +0x70's 0x762940 / 0x762920); 0x640EF0 / 0x640F30 (empty: ret 4 / ret 8); the lights panel's 0x646F70 / 0x6474C0 pass them  // NAME 1 [fill, index, range]: what 0x646F70 passes
	void Unknown_63BDB0(const Matrix3D &transform);	// 0x63BDB0  // NAME 1 [transform]: what 0x6435E0 / 0x6390A0 pass
	void Unknown_63C080(const char *name, bool a, bool b);	// 0x63C080  // NAME 1 [name, a, b]: what 0x6366A0 / 0x6390A0 pass
	void Unknown_63D2F0(int index);					// 0x63D2F0  // NAME 1 [index]: neutral
	void Unknown_63E2B0();							// 0x63E2B0
	void Unknown_640F50(CharacterViewAvatarClass *avatar);	// 0x640F50  // NAME 1 [avatar]: neutral
};

// ui_ptrade.cpp's player trade interface UIPTradeClass (the starbase view's +0x1A4): sclient.h (the main view's +0x1E4)

class StarbaseShipCustomizeInterface;	// ui_ship_cust.h: the starbase view's +0x1A0 (RECUSTOMIZE_SHIP_START 129: 0x61F250; the client's RTTI 0xB89BB0)
class SBaseViewUnknown19CClass : public InterfaceClass { public: SBaseViewUnknown19CClass(UIViewClass *view, LayerClass *layer); void Unknown_551C40(RecustomizationCostClass *cost); char Unknown84[0x3D0 - 0x84]; };	// NAME 1: neutral (D10): the starbase view's +0x19C (RECUSTOMIZE_AVATAR_START 131: 0x551C40; ui_avatar_cust.cpp's StarbaseAvatarCreationInterface, 0x69F8F0 case 14: 0x54B7B0, 0x3D0 bytes)  // NAME 1 [cost]: the packet's field  // NAME 1 [view, layer]: what 0x69F8F0 passes
class SBaseViewUnknown184Class : public InterfaceClass { public: SBaseViewUnknown184Class(UIViewClass *view, LayerClass *layer); char Unknown84[0xB08 - 0x84]; };	// NAME 1: neutral (D10): the starbase view's +0x184 (0x69F8F0 case 5: 0x5EB810, 0xB08 bytes; 'TSM03A')  // NAME 1 [view, layer]: what 0x69F8F0 passes
class SBaseViewUnknown188Class : public InterfaceClass { public: SBaseViewUnknown188Class(UIViewClass *view, LayerClass *layer); char Unknown84[0x11C - 0x84]; };	// NAME 1: neutral (D10): the starbase view's +0x188 (0x69F8F0 case 6: 0x5435D0, 0x11C bytes; 'FA_TSM02A.INI')  // NAME 1 [view, layer]: what 0x69F8F0 passes
class SBaseViewUnknown198Class : public InterfaceClass { public: SBaseViewUnknown198Class(SClient *client); char Unknown84[0xB8 - 0x84]; };	// NAME 1: neutral (D10): the starbase view's +0x198 (0x69F8F0 case 9: 0x65A020, 0xB8 bytes; 'JWEB00')  // NAME 1 [client]: what 0x69F8F0 passes
class UIManuClass;	// ui_manu.h: the starbase view's +0x18C (0x69F8F0 case 7: 0x5AFA00)

// ui_jobs.cpp's interface (an InterfaceClass: its reference count at +0x3C) and sbaseview.cpp's view
// (SClient +0x135C; LOUNGE_NPC 82 calls its 0x6A0FD0) that holds it, declared as far as the job packets call them
class UIJobsClass : public InterfaceClass	// NAME 3: class-from-file (D24) for app\client\ui_jobs.cpp
{
public:
	UIJobsClass(SClient *client);	// 0x5A1170 (0x1F8 bytes; the starbase view's 0x69F8F0, case 8)  // NAME 1 [client]: what 0x69F8F0 passes
	void Unknown_5A3BC0(JobListPacketClass *packet);	// 0x5A3BC0 (JOB_LIST 147)  // NAME 1 [packet]: the handler's packet
	void Unknown_5A3F10(JobDescriptionPacketClass *packet);	// 0x5A3F10 (JOB_DESCRIPTION 148)  // NAME 1 [packet]: the handler's packet
	void Unknown_5A3FB0(JobAcceptReplyPacketClass *packet);	// 0x5A3FB0 (JOB_ACCEPT_REPLY 150)  // NAME 1 [packet]: the handler's packet
	void Unknown_5A4430(int job_id);	// 0x5A4430 (JOB_DELETE 149)  // NAME 1 [job_id]: the packet's JobID
	char Unknown84[0x1F8 - 0x84];	// +0x84
};
class FontSys;	// fontsys.cpp's text object, a render object (sbaseview.cpp declares it)
// ScriptedMovieViewerSettingsStruct (the view's +0x68): unknown_490900.h
// ui_talkt.cpp's talk tree UITalkTreeClass (the view's +0x190): sclient.h
struct SBaseViewUnknown698730Struct { char Unknown00[0x0A]; bool Unknown0A; bool Unknown0B; };	// NAME 1: neutral (D10): what 0x698730 finds (0x6A2120 tests +0x0A / +0x0B)
class SBaseViewUnknown1B4Class { public: ~SBaseViewUnknown1B4Class(); SBaseViewUnknown698730Struct *Unknown_698730(int key); char Unknown00[0x18]; int Unknown18; };	// NAME 1: neutral (D10): what sbase_info.cpp's 0x6987A0 returns (the starbase view's +0x1B4; the view's destructor 0x69B0C0 deletes it through 0x698370); 0x698730: a hash lookup by key; +0x18 the column of 0x6A2120's model names (0..9)  // NAME 1 [key]: neutral
class SBaseViewUnknown70Class;	// the +0x70 scene (sbaseview.cpp declares it)
class LightClass;
class HAnimClass;
class AudibleSoundClass;
class LayerClass;
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
	void Unknown_441A30(int index);	// 0x441A30 (the starbase view's 0x69D030 passes its index)  // NAME 1 [index]: what 0x69D030 passes
	void Unknown_441A90(int a, int b);	// 0x441A90 (0x69C8F0: the client's +0x10BC, the starbase interface's +0x84)  // NAME 1 [a, b]: neutral
	int Unknown00[0x4C / 4];	// +0x00 (0x441910 stores dwords up to +0x48)
};
class SBaseViewClass : public Unknown65B810ViewClass	// NAME 3: class-from-file (D24) for app\client\sbaseview.cpp
{
public:
	SBaseViewClass(ClientTaskClass *task, int arg);	// 0x69AD30 (table 0xAFD2FC; SClient 0x74B680 passes its +0x1384 task and +0x1324)  // NAME 1 [task, arg]: what the base / +0x1F4 get
	virtual ~SBaseViewClass();	// +0x00 0x69B0C0 (deleting 0x69B090)
	virtual void Unknown_04();	// +0x04 0x69B5E0 (Init: the scenes, cameras, layers, text object, interfaces and lights)
	virtual int Unknown_0C(bool on);	// +0x0C 0x69C5E0 (not rebuilt: the render, WW3D::Begin_Render with on)
	virtual void Unknown_10();	// +0x10 0x69C0D0 (not rebuilt)
	virtual void Unknown_14();	// +0x14 0x69C8F0 (not rebuilt: the frame, the base's 0x6B4860 first)
	virtual int Unknown_30() { return((int)this); }	// +0x30 0x69B050 (out of line after the constructor)
	virtual SimpleSceneClass *Unknown_5C();	// +0x5C 0x69B070 (the view's own slot: the +0x70 scene)
	virtual void Unknown_18(bool arg);	// +0x18 0x69C5A0 (its byte: set, +0x54 = 0 and +0x58 = +0x50; else 0x69D030(-1))  // NAME 1 [arg]: neutral
	void Unknown_69D030(int index);	// 0x69D030: starbase interface index (0..3: +0x174 / +0x178 / +0x180 / +0x17C) made once and activated as Starbase; -1 none  // NAME 1 [index]: neutral
	std::string Unknown_6A2120(int index);	// 0x6A2120: the name 0x69D030 hands the starbase interface (a tsd / bar model name, '3' or '2' added when that model exists)  // NAME 1 [index]: neutral
	void Unknown_69ED80();	// 0x69ED80: the +0x6C camera moved from +0x84 towards +0xB4 (0x69C5E0 calls it when +0x1C4 is 0)
	void Unknown_6A1180();	// 0x6A1180 (not rebuilt; 0x69C5E0 calls it before End_Render)
	void Unknown_6A1B80();	// 0x6A1B80: the GROUP_INVITE box's answer sent (0x69C8F0 calls it every frame; as ui_group.cpp's 0x597650)
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
	void Unknown_69DC80(RefCountPtr<CharacterViewAvatarClass> avatar, const char *bone, bool offset);	// 0x69DC80: the avatar (ui_starbase 0x63C080 passes it) at ViewRObjs[1]'s bone, kept in its +0x330; offset: its own turn at the bone's position plus +0x1E0  // NAME 1 [avatar, bone, offset]: neutral
	void Unknown_69DFC0(RefCountPtr<CharacterViewAvatarClass> avatar, const Matrix3D &tm, bool offset);	// 0x69DFC0: as 0x69DC80 at tm (ui_starbase 0x63BDB0)  // NAME 1 [avatar, tm, offset]: neutral
	void Unknown_69F440();	// 0x69F440: +0x1B0 set
	void Unknown_69F460(int arg);	// 0x69F460  // NAME 1 [arg]: neutral
	void Unknown_69F480(int index);	// 0x69F480: channel index stopped and released  // NAME 1 [index]: neutral
	void Unknown_69F540(const RefCountPtr<RenderObjClass> &model, const char *name, int index, bool once, bool backward);	// 0x69F540: channel index started: name's animation on model (the mode 1 or 2; backward from the last frame)  // NAME 1 [model, name, index, once, backward]: neutral
	void Unknown_6A0EC0();	// 0x6A0EC0: the text object +0x1B8 follows the pointer (inlined in 0x69C5E0)
	void Unknown_69E260(const char *bone);	// 0x69E260: with ViewRObjs[1], +0x84 = its bone transform turned -90 degrees about y and z, the +0x6C camera's  // NAME 1 [bone]: neutral
	void Unknown_69E510(Matrix3D tm);	// 0x69E510: with ViewRObjs[1], +0x84 = tm turned -90 degrees about y and z, the +0x6C camera's transform; +0xE8 / +0x1BC cleared  // NAME 1 [tm]: neutral
	void Unknown_6A1880(RefCountPtr<FontSys> &text, int kind);	// 0x6A1880: text made (kind 0 NameText_SBASE, 1 / 2 NPCText_SBASE, 3 GuildText_SBASE) and added to the +0x1C scene  // NAME 1 [text, kind]: neutral
	void Unknown_69E6E0(const char *bone, int seconds, bool keep, float lift);	// 0x69E6E0: with ViewRObjs[1], the move target +0xB4 = the bone transform turned as 0x69E260 (keep: +0x84 with only its position), lift added to its z, over seconds from now (ui_starbase 0x638500 / 0x63B630)  // NAME 1 [bone, seconds, keep, lift]: neutral
	void Unknown_69EF00(int mode, float speed);	// 0x69EF00: the +0x6C camera turned by the frame time * speed * 3 (mode 0 / 1 about y either way, 2 / 3 its pitch +0x1BC up / down, clamped to 0.25), kept as +0x84  // NAME 1 [mode, speed]: neutral
	void Unknown_69EB20(Matrix3D tm, int seconds, bool restart);	// 0x69EB20: with ViewRObjs[1], the move target +0xB4 = tm turned as 0x69E510, over seconds (+0xE4); restart: +0xE8 = 0, +0x1C4 = 3  // NAME 1 [tm, seconds, restart]: neutral
	void Unknown_69F710();	// 0x69F710: every channel one frame on
	bool Unknown_69F850(int index);	// 0x69F850: channel index at its end  // NAME 1 [index]: neutral
	void Unknown_69F8F0(int type, int arg);	// 0x69F8F0: interface type (5..14) made once and shown as +0x64 (11: the talk tree, SClient's too, and STARBASE_REQUEST action 4 with arg)  // NAME 1 [type, arg]: neutral
	void Unknown_6A0540();	// 0x6A0540: +0x68's object, +0x1CC and +0x1D0 out of their scenes
	void Unknown_6A0590(const char *name);	// 0x6A0590: the lights loader (FAIL_IF '!RKey.Peek()' / '!RCamera.Peek()', l.1261 / 1262): the ambient light and the RKEY / RCAMERA lights of the INI name  // NAME 1 [name]: what INIClass::Load gets
	void Unknown_6A1C90();	// 0x6A1C90: RCamera turned along the +0x6C camera's view plus the offset +0x1D4 (the lights loader's RKey math)
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
	ScriptedMovieViewerSettingsStruct *Unknown68;	// +0x68 (0x6A0540 removes its +0x00's render object; the destructor leaves it)
	RefCountPtr<CameraClass> Unknown6C;	// +0x6C (+0x70's camera: 0x69C5E0)
	RefCountPtr<SBaseViewUnknown70Class> Unknown70;	// +0x70 (ViewRObjs[1]'s scene; 0x6A20C0 / 0x6A2100: its fog)
	RefCountPtr<CameraClass> Unknown74;	// +0x74 (+0x78's camera: 0x69C5E0)
	RefCountPtr<SimpleSceneClass> Unknown78;	// +0x78 (ViewRObjs[2]'s scene)
	LayerClass *Unknown7C;	// +0x7C (0x69B5E0: a layer of the base's +0x1C scene and +0x20 camera; 0x69C0D0 deletes it)
	LayerClass *Unknown80;	// +0x80 (0x69B5E0: a layer of +0x70 and +0x6C; 0x69C0D0 deletes it)
	Matrix3D Unknown84;	// +0x84 (identity from the constructor)
	Matrix3D UnknownB4;	// +0xB4 (identity from the constructor)
	float UnknownE4;	// +0xE4: the +0x84 -> +0xB4 move's length in seconds (0x69ED80; 0 when none)
	float UnknownE8;	// +0xE8: its time so far (0x69ED80)
	RefCountPtr<RenderObjClass> ViewRObjs[3];	// +0xEC: by type, in the scenes +0x1C / +0x70 / +0x78 (0x69D930)  // NAME 3: the assert's 'ViewRObjs[type]'
	SBaseViewChannelStruct Channels[6];	// +0xF8  // NAME 2: what it holds
	bool Unknown170;	// +0x170 (0x69C8F0: set, 0x69D030(-1) and cleared)
	RefCountPtr<UIStarbaseClass> Unknown174;	// +0x174
	RefCountPtr<UIStarbaseClass> Unknown178, Unknown17C, Unknown180;	// +0x178 (0x69C8F0 picks +0x174..+0x180 by +0x50)
	RefCountPtr<SBaseViewUnknown184Class> Unknown184; RefCountPtr<SBaseViewUnknown188Class> Unknown188; RefCountPtr<UIManuClass> Unknown18C;	// +0x184 (0x69CDE0 releases them; 0x69F8F0 makes them)
	RefCountPtr<UITalkTreeClass> Unknown190;	// +0x190 (0x69F8F0, case 11: 0x64B6E0 with the client and 1)
	RefCountPtr<UIJobsClass> Jobs; RefCountPtr<SBaseViewUnknown198Class> Unknown198; RefCountPtr<SBaseViewUnknown19CClass> AvatarUI; RefCountPtr<StarbaseShipCustomizeInterface> ShipUI;	// +0x194  // NAME 2 [Jobs]: what it holds; +0x19C / +0x1A0  // NAME 3 [AvatarUI, ShipUI]: the recustomize handlers' '!avatar_ui' / '!ship_ui' tests
	RefCountPtr<UIPTradeClass> PTrade;	// +0x1A4 (TRADE 31 in a starbase)  // NAME 3: its class's file (ui_ptrade.cpp)
	RefCountPtr<UIGroupClass> Unknown1A8;	// +0x1A8 ui_group.cpp's interface (0x69C8F0: its slots +0x58 / +0x5C and 0x596770)
	RefCountPtr<UIBuffClass> Unknown1AC;	// +0x1AC (0x69B5E0: the main view's +0x1D4 class, 0x5608D0)
	bool Unknown1B0;	// +0x1B0 (0x69F440 sets it, 0x69CDE0 clears it)
	SBaseViewUnknown1B4Class *Unknown1B4;	// +0x1B4 (LOUNGE_NPC 82: sbase_info.cpp's 0x6987A0 of the packet; the destructor deletes it)
	RefCountPtr<FontSys> Unknown1B8;	// +0x1B8: a text object (0x6A0E80 changes its text)
	float Unknown1BC;	// +0x1BC: the +0x6C camera's pitch about x (0x69EF00: -0.25 .. 0.25)
	RefCountPtr<AudibleSoundClass> Unknown1C0;	// +0x1C0: a 2-D sound (0x6A1000 plays it looped, 0x6A1130 stops it)
	int Unknown1C4;	// +0x1C4 (0x69C5E0 counts it down)
	int Unknown1C8;	// +0x1C8
	RefCountPtr<LightClass> RKey;	// +0x1CC a light (0x69B5E0; 0x6A0540 removes it)  // NAME 3: the lights loader's G_ASSERT '!RKey.Peek()' (0x6A0590)
	RefCountPtr<LightClass> RCamera;	// +0x1D0 a light (0x69B5E0; 0x6A0540 removes it)  // NAME 3: the lights loader's G_ASSERT '!RCamera.Peek()' (0x6A0590)
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
