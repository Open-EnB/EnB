// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\client\eff_o.h (name assumed from eff_o.cpp)
**
**	eff_o.cpp's effect object: ClientClass's second list (+0x444, keyed by ID). The base class
**	(vtable 0xB0002C, 0x80 bytes, constructor 0x6E6710, destructor 0x6E6930) holds the effect's
**	timing (client time +0x10BC against StartTime, Duration rounded up to CycleTime), its linked
**	effects (+0x5C started with it, +0x60 started when it ends), its sound and its camera. The
**	unary end (vtable 0xB00060, constructor 0x6E7540) and the binary end (vtable 0xB0009C,
**	constructor 0x6E7D70) derive from it; the effect packets reach them through slots +0x10 / +0x14.
**
**	client.h declares the class as far as the client's lists need it; with EFF_O_H defined first
**	(eff_o.cpp) it leaves the declaration to this header.
*/



#if defined(_MSC_VER)
#pragma once
#endif

#ifndef EFF_O_H  // NAME 3 [eff_o.h]: header name assumed from the client's eff_o.cpp
#define EFF_O_H

#include "hashlist.h"
#include "refcount.h"
#include "vector3.h"
#include "matrix3d.h"
#include "..\common\lego_identifier.h"

class ClientClass;
class ClientObjectClass;
class RenderObjClass;
class CameraClass;
class AudibleSoundClass;
class INIClass;
class UnknownB00060EffectClass;
class UnknownB0009CEffectClass;

// An object watcher: the base with only a virtual destructor (vtable 0xAF0DD0, its deleting destructor 0x57DBC0 first emitted
// in ui_cpit.cpp; influence.h's record has the same base)
class UnknownAF0DD0Class	// NAME 1: neutral (D10)
{
public:
	virtual ~UnknownAF0DD0Class() {}
	void Unknown_73FDB0(DataNode<UnknownAF0DD0Class *> *node, ClientObjectClass *object);	// 0x73FDB0 (the node out of its list, then into the object's (0x6F5090) with this as its value)  // NAME 1 [node, object]: what the effects pass
};

class EffectObjectClass : public UnknownAF0DD0Class	// NAME 3: class-from-file (D24) for eff_o.cpp
{
public:
	EffectObjectClass(ClientClass *client, unsigned id, unsigned start_time, unsigned duration, unsigned cycle_time);	// 0x6E6710  // NAME 1 [client .. cycle_time]: the members they set
	virtual ~EffectObjectClass();						// +0x00 0x6E6930
	virtual void Unknown_04(int arg);					// +0x04 0x6E6850 (empty)  // NAME 1 [arg]: unused
	virtual Vector3 Unknown_08(const Vector3 &position);	// +0x08 0x6E6870 (zero; the sound's position from the camera's)  // NAME 1 [position]: what 0x6E6AD0 passes
	virtual void Unknown_0C(unsigned duration);		// +0x0C 0x6E6F10 (Unknown_6E6F90, then the start link's with flag 0x20)  // NAME 1 [duration]: what it sets
	virtual UnknownB00060EffectClass *Unknown_10() { return(NULL); }	// +0x10 0x6E5420 (the unary end)
	virtual UnknownB0009CEffectClass *Unknown_14() { return(NULL); }	// +0x14 0x6E4860 (the binary end)
	virtual void Unknown_18();							// +0x18 0x6E68A0 (empty; called when the effect starts)
	virtual void Unknown_1C();							// +0x1C 0x6E68C0 (empty; called every frame while it runs)
	virtual void Unknown_20();							// +0x20 0x6E68E0 (empty; called when it ends)
	virtual void Unknown_24(int arg) = 0;				// +0x24 (_purecall: the ends link a new effect to theirs; 0x6E6AD0 passes 1 for NextLink)  // NAME 1 [arg]: what it gets

	void Unknown_6E74B0(int arg);						// 0x6E74B0 (Add_Effect's first: the render object at +0x4C)  // NAME 1 [arg]: what client.h declares
	void Unknown_6E6ED0(int arg);						// 0x6E6ED0 (Add_Effect's second: the camera at +0x50)  // NAME 1 [arg]: what client.h declares
	void Unknown_6E6E30();								// 0x6E6E30 (cpacket.cpp's REMOVE_EFFECT 15)
	bool Unknown_6E6AD0();								// 0x6E6AD0 (SClient::Process_AI removes the effect when it is false)
	void Unknown_6E6F90(unsigned duration)				// 0x6E6F90 (Duration from it, rounded up to CycleTime)  // NAME 1 [duration]: what it sets
	{
		if (CycleTime == 0 || duration == (unsigned)-1) {
			Duration = duration;
		} else if (duration == 0) {
			Duration = CycleTime;
		} else {
			Duration = (duration / CycleTime) * CycleTime;
			if (duration > Duration) {
				Duration += CycleTime;
			}
		}
	}
	bool Get_Flag(int bit) const { return(((Flags >> bit) & 1) != 0); }	// (0x6E7ED0 reads flags 9 / 10 through it: the extra expansions put its first inlined 0x6E6E30's callees out of line, as the client's)  // NAME 2: what it does
	void Set_Flag(unsigned flag, bool on) { if (on) Flags |= flag; else Flags &= ~flag; }	// 0x6E8090 (out of line in 0x6E7ED0)  // NAME 2: what it does
	void Unknown_6E6FE0();								// 0x6E6FE0 (the start link from StartLinkID)
	void Unknown_6E7080();								// 0x6E7080 (the next link from NextLinkID; no caller, 0x6E6AD0 writes it out)
	void Unknown_6E70F0();								// 0x6E70F0 (the SFX played)
	void Unknown_6E73F0(const char *name, bool sfx_2d, float radius, int min_time, int max_time);	// 0x6E73F0 (the SFX: shiputils 0x812210 calls it)  // NAME 1 [name .. max_time]: effect.ini's SFX, SFX2D, TriggerSFXRadius, TriggerSFXMin/MaxElapsedTime
	void Unknown_6E7260();								// 0x6E7260 (the SFX trigger within its radius of the camera; no caller, 0x6E6AD0 writes it out)
	void Unknown_6E7330();								// 0x6E7330 (the sound at +0x38 stopped and released)

	HashNodeClass<EffectObjectClass *> Node;			// +0x04 ClientClass::EffectList's node  // NAME 2: what it is
	ClientClass *Client;								// +0x20 (its +0x10BC: the time)  // NAME 2: what it holds
	unsigned StartTime;									// +0x24  // NAME 2: what it holds
	unsigned Duration;									// +0x28 (-1: infinite)  // NAME 3: effect.ini's 'Duration'
	unsigned CycleTime;									// +0x2C  // NAME 3: effect.ini's 'CycleTime'
	unsigned ID;										// +0x30 the list's key  // NAME 2: what it is
	char *Unknown34;									// +0x34 (nstrdup; the SFX name)
	AudibleSoundClass *Unknown38;						// +0x38 (the sound playing)
	float Unknown3C;									// +0x3C (TriggerSFXRadius, squared)
	int Unknown40;										// +0x40 (TriggerSFXMinElapsedTime)
	int Unknown44;										// +0x44 (TriggerSFXMaxElapsedTime)
	unsigned Unknown48;									// +0x48 (the next trigger time)
	RenderObjClass *Unknown4C;							// +0x4C (Unknown_6E74B0's)
	CameraClass *Camera;								// +0x50  // NAME 3: 0x6E6AD0's FAIL_IF '!Camera'
	int StartLinkID;									// +0x54 (-1: none)  // NAME 3: effect.ini's 'StartLinkID'
	int NextLinkID;										// +0x58 (-1: none)  // NAME 3: effect.ini's 'NextLinkID'
	EffectObjectClass *StartLink;						// +0x5C  // NAME 2: the effect StartLinkID makes
	EffectObjectClass *NextLink;						// +0x60  // NAME 2: the effect NextLinkID makes
	int Unknown64;										// +0x64
	float Scale;										// +0x68 (1; passed to the linked effects)  // NAME 3: the effect packets' Scale
	Vector3 HSVShift;									// +0x6C (passed to the linked effects)  // NAME 3: the effect packets' HSVShift
	float Speedup;										// +0x78 (1; passed to the linked effects)  // NAME 3: the effect packets' Speedup
	union {
		unsigned Flags;									// +0x7C  // NAME 2: what it holds
		struct {
			unsigned State : 2;							// 0 waiting, 1 running, 2 done  // NAME 2: what it holds
			unsigned Unknown2 : 1;						// 0x04 (0x6E6E30)
			unsigned Unknown3 : 1;						// 0x08 (an SFX that triggers)
			unsigned Unknown4 : 1;						// 0x10
			unsigned Unknown5 : 1;						// 0x20 (PassDurationToStartLink)
			unsigned Unknown6 : 1;						// 0x40
			unsigned Unknown7 : 1;						// 0x80 (the unary end: TrackSource)
			unsigned Unknown8 : 1;						// 0x100 (the unary end: GoAwayWhenSourceDoes; the binary end: TrackTarget)
			unsigned Unknown9 : 1;						// 0x200 (the binary end: GoAwayWhenSourceDoes)
			unsigned Unknown10 : 1;						// 0x400 (the binary end: GoAwayWhenTargetDoes)
			unsigned Unknown11 : 1;						// 0x800 (the binary end: UnaryStartLinkToTarget)
			unsigned Unknown12 : 1;						// 0x1000 (the binary end: UnaryNextLinkToTarget)
			unsigned Unknown13 : 1;						// 0x2000 (the binary end: the effect packets' OutsideTargetRadius)
		} Flag;
	};
};

// The unary end (vtable 0xB00060, 0xD8 bytes, constructor 0x6E7540; its destructor is the implicit one,
// first emitted in eff_attach.cpp at 0x6E4970): an effect on one object or point. The object is SourceObject,
// watched through SourceNode (slot +0x04 is the object's notice that it goes); Transform is where it plays.
class UnknownB00060EffectClass : public EffectObjectClass	// NAME 1: neutral (D10)
{
public:
	UnknownB00060EffectClass(ClientClass *client, unsigned id, unsigned start_time, unsigned duration, unsigned cycle_time);	// 0x6E7540  // NAME 1 [client .. cycle_time]: the base's
	virtual void Unknown_04(int arg);					// +0x04 0x6E7690 (arg the node that goes: the source's, then flag 0x100 ends it)  // NAME 1 [arg]: the base's
	virtual UnknownB00060EffectClass *Unknown_10() { return(this); }	// +0x10 0x6E4880
	virtual void Unknown_18();							// +0x18 0x6E7850
	virtual void Unknown_1C();							// +0x1C 0x6E7870 (with flag 0x80)
	virtual void Unknown_24(int next);					// +0x24 0x6E78A0 (StartLink or NextLink takes this effect's render object, camera, end and source; G_ASSERT l.553)  // NAME 1 [next]: 0x6E6AD0 passes 1 for NextLink
	virtual void Unknown_28(int game_id);				// +0x28 0x6E7770 ("Set_Source_Object_ID couldn't find the object!")  // NAME 1 [game_id]: OBJECT_EFFECT 9's GameID
	virtual void Unknown_2C(const Matrix3D &transform) { Transform = transform; }	// +0x2C 0x6E48A0  // NAME 1 [transform]: POINT_EFFECT 10's location

	void Unknown_6E7810(int game_id, const LegoIdentifierClass &lego);	// 0x6E7810 (no caller)  // NAME 1 [game_id, lego]: what it sets
	void Unknown_6E7AA0();								// 0x6E7AA0 (the transform from the source's model: FAIL_IF '!SourceObject')

	ClientObjectClass *SourceObject;					// +0x80  // NAME 3: 0x6E7AA0's FAIL_IF '!SourceObject'
	DataNode<UnknownAF0DD0Class *> SourceNode;	// +0x84 (vtable 0xAF0DDC; its value +0x90)  // NAME 2: SourceObject's watch node
	Matrix3D Transform;									// +0x94  // NAME 2: what it holds
	LegoIdentifierClass SourceLego;						// +0xC4  // NAME 2: what it holds
};

// The binary end (vtable 0xB0009C, 0xE0 bytes, constructor 0x6E7D70; its destructor is the implicit one,
// first emitted in eff_line.cpp at 0x6E5510): an effect from SourceObject (or SourcePosition) to TargetObject
// (TargetPosition, TargetOffset from the target's position). Slot +0x04 is an object's notice that it goes.
class UnknownB0009CEffectClass : public EffectObjectClass	// NAME 1: neutral (D10)
{
public:
	UnknownB0009CEffectClass(ClientClass *client, unsigned id, unsigned start_time, unsigned duration, unsigned cycle_time);	// 0x6E7D70  // NAME 1 [client .. cycle_time]: the base's
	virtual void Unknown_04(int arg);					// +0x04 0x6E7ED0 (the source's or the target's node goes: flags 0x200 / 0x400 end it)  // NAME 1 [arg]: the base's
	virtual UnknownB0009CEffectClass *Unknown_14() { return(this); }	// +0x14 0x6E5440
	virtual void Unknown_18();							// +0x18 0x6E8800
	virtual void Unknown_1C();							// +0x1C 0x6E8890
	virtual void Unknown_24(int next);					// +0x24 0x6E8B60 (as the unary end's, both ends; a unary link through slot +0x3C; G_ASSERT l.815)  // NAME 1 [next]: 0x6E6AD0 passes 1 for NextLink
	virtual void Unknown_28(int game_id);				// +0x28 0x6E80D0 ("Set_Source_Object_ID couldn't find the object!")  // NAME 1 [game_id]: the packets' SourceGameID
	virtual void Unknown_2C(int game_id, const LegoIdentifierClass &lego);	// +0x2C 0x6E8170  // NAME 1 [game_id, lego]: the packets' SourceGameID, SourceLego
	virtual void Unknown_30(const Vector3 &position) { SourcePosition = position; }	// +0x30 0x6E5460  // NAME 1 [position]: what it sets
	virtual void Unknown_34(int game_id);				// +0x34 0x6E81B0 ("Set_Target_Object_ID couldn't find the object!")  // NAME 1 [game_id]: the packets' TargetGameID
	virtual void Unknown_38(const Vector3 &position) { TargetPosition = position; }	// +0x38 0x6E54A0  // NAME 1 [position]: what it sets
	virtual void Unknown_3C(UnknownB00060EffectClass *unary, int next);	// +0x3C 0x6E8DD0 (slot +0x24's link to a unary effect)  // NAME 1 [unary, next]: what slot +0x24 passes

	void Unknown_6E82A0(short unknown36, short unknown34, Vector3 target_offset);	// 0x6E82A0 (the effect packets' offset)  // NAME 1 [unknown36 .. target_offset]: the packets' fields
	void Unknown_6E87C0(const Vector3 &offset);			// 0x6E87C0 (no caller)  // NAME 1 [offset]: what it sets
	void Unknown_6E8F80();								// 0x6E8F80 (SourcePosition from the source: FAIL_IF '!SourceObject')
	void Unknown_6E9190();								// 0x6E9190 (FAIL_IF '!TargetObject')

	ClientObjectClass *SourceObject;					// +0x80  // NAME 3: 0x6E8F80's FAIL_IF '!SourceObject'
	ClientObjectClass *TargetObject;					// +0x84  // NAME 3: 0x6E9190's FAIL_IF '!TargetObject'
	DataNode<UnknownAF0DD0Class *> SourceNode;	// +0x88 (vtable 0xAF0DDC)  // NAME 2: SourceObject's watch node
	DataNode<UnknownAF0DD0Class *> TargetNode;	// +0x98 (vtable 0xAF0DDC)  // NAME 2: TargetObject's watch node
	Vector3 SourcePosition;								// +0xA8  // NAME 2: what it holds
	Vector3 TargetPosition;								// +0xB4  // NAME 2: what it holds
	Vector3 TargetOffset;								// +0xC0  // NAME 3: the effect packets' TargetOffset
	LegoIdentifierClass SourceLego;						// +0xCC  // NAME 3: the effect packets' SourceLego
};

// A unary end that tracks its source from the start (vtable 0xB000F0, 0xD8 bytes, constructor 0x6E9230; a
// factory helper 0x6EB850 makes one too): slot +0x08 gives its transform's translation
class UnknownB000F0EffectClass : public UnknownB00060EffectClass	// NAME 1: neutral (D10)
{
public:
	UnknownB000F0EffectClass(ClientClass *client, unsigned id, unsigned start_time, unsigned duration, unsigned cycle_time);	// 0x6E9230 (flag 0x80 set)  // NAME 1 [client .. cycle_time]: the base's
	virtual Vector3 Unknown_08(const Vector3 &position);	// +0x08 0x6E9360  // NAME 1 [position]: the base's
};

void Unknown_6E6A90(EffectObjectClass *effect);
INIClass *Unknown_6EA550();						// 0x6EA550: effect.ini, loaded on first use (sclient.cpp 0x74CA90; the factory inlines it)		// 0x6E6A90: the effect ended if it runs, and deleted (SClient's Shutdown and Process_AI)
EffectObjectClass *Unknown_6EC950(INIClass *ini, const char *section, unsigned id, unsigned start_time, unsigned duration, unsigned cycle_time, ClientClass *client, float scale, float *hsv);	// 0x6EC950: the factory's helper for eff_particlejet.cpp's effect (FAIL_IF '!robj' l.1695; not an emitter: the report, l.1708)  // NAME 1 [ini .. hsv]: what the factory passes
EffectObjectClass *Unknown_6EA5E0(INIClass *ini, const char *section, unsigned id, unsigned start_time, unsigned duration, unsigned cycle_time, ClientClass *client, float scale, float *hsv);	// 0x6EA5E0: the factory's helper for eff_robj.cpp's effect on a BaseAssetID model (FAIL_IF '!robj' l.1235)  // NAME 1 [ini .. hsv]: what the factory passes
EffectObjectClass *Unknown_6EA830(INIClass *ini, const char *section, unsigned id, unsigned start_time, unsigned duration, unsigned cycle_time, ClientClass *client, float scale, float *hsv);	// 0x6EA830: the factory's helper for eff_robj.cpp's second model effect (FAIL_IF '!robj' l.1265)  // NAME 1 [ini .. hsv]: what the factory passes
EffectObjectClass *Unknown_6EAA40(INIClass *ini, const char *section, unsigned id, unsigned start_time, unsigned duration, unsigned cycle_time, ClientClass *client, float scale, float *hsv);	// 0x6EAA40: the factory's helper for 0x6F25A0's model effect (FAIL_IF '!robj' l.1293)  // NAME 1 [ini .. hsv]: what the factory passes
EffectObjectClass *Unknown_6EAC50(INIClass *ini, const char *section, unsigned id, unsigned start_time, unsigned duration, unsigned cycle_time, ClientClass *client, float scale, float *hsv, float speedup);	// 0x6EAC50: the factory's helper for eff_line.cpp's line effect (FAIL_IF '!robj' l.1321; not a line: the report, l.1350)  // NAME 1 [ini .. speedup]: what the factory passes
EffectObjectClass *Unknown_6EAFA0(INIClass *ini, const char *section, unsigned id, unsigned start_time, unsigned duration, unsigned cycle_time, ClientClass *client, float scale, float *hsv);	// 0x6EAFA0: the factory's helper for eff_seek.cpp's effect on a BaseAssetID model (FAIL_IF '!robj' l.1362)  // NAME 1 [ini .. hsv]: what the factory passes
EffectObjectClass *Unknown_6EB150(INIClass *ini, const char *section, unsigned id, unsigned start_time, unsigned duration, unsigned cycle_time, ClientClass *client, float scale, float *hsv);	// 0x6EB150: the factory's helper for eff_seek.cpp's spiral effect (FAIL_IF '!robj' l.1386)  // NAME 1 [ini .. hsv]: what the factory passes
EffectObjectClass *Unknown_6EB330(INIClass *ini, const char *section, unsigned id, unsigned start_time, unsigned duration, unsigned cycle_time, ClientClass *client, float scale, float *hsv);	// 0x6EB330: the factory's helper for eff_seek.cpp's changing effect (FAIL_IF '!robj' l.1417)  // NAME 1 [ini .. hsv]: what the factory passes
EffectObjectClass *Unknown_6EB590(INIClass *ini, const char *section, unsigned id, unsigned start_time, unsigned duration, unsigned cycle_time, ClientClass *client, float scale, float *hsv);	// 0x6EB590: the factory's helper for eff_seek.cpp's timed changing effect (FAIL_IF '!robj' l.1451)  // NAME 1 [ini .. hsv]: what the factory passes
EffectObjectClass *Unknown_6EB850(INIClass *ini, const char *section, unsigned id, unsigned start_time, unsigned duration, unsigned cycle_time, ClientClass *client, float scale, float *hsv);	// 0x6EB850: the factory's helper for a tracking unary end (effect.ini, the section, scale and hsv unused)  // NAME 1 [ini .. hsv]: what the factory passes
EffectObjectClass *Unknown_6EC570(INIClass *ini, const char *section, unsigned id, unsigned start_time, unsigned duration, unsigned cycle_time, ClientClass *client, float scale, float *hsv);	// 0x6EC570: the factory's helper for eff_attach.cpp's effect on bones (FAIL_IF '!robj' l.1640)  // NAME 1 [ini .. hsv]: what the factory passes
EffectObjectClass *Unknown_6EC7D0(INIClass *ini, const char *section, unsigned id, unsigned start_time, unsigned duration, unsigned cycle_time, ClientClass *client, float scale, float *hsv);	// 0x6EC7D0: the factory's helper for a render-state effect (0x6F0200's class; scale and hsv unused)  // NAME 1 [ini .. hsv]: what the factory passes
EffectObjectClass *Unknown_6EB9B0(INIClass *ini, const char *section, unsigned id, unsigned start_time, unsigned duration, unsigned cycle_time, ClientClass *client, float scale, float *hsv);	// 0x6EB9B0: the factory's helper for eff_pass.cpp's pass effect (a MaterialPassClass from the Swatch mesh, or missingtexture.tga; the projections)  // NAME 1 [ini .. hsv]: what the factory passes
// 0x6E9490: the effect factory (cdecl): effect.ini's EFFECT-<desc_id> section made into an effect of its EffectClass, with
// its links, SFX and tracking flags (17 callers: the effect packets, the links 0x6E6FE0 / 0x6E7080, co.cpp's warp state)
EffectObjectClass *Unknown_6E9490(int desc_id, unsigned id, unsigned duration, unsigned start_time, ClientClass *client, float scale, const Vector3 &hsv_shift, float speedup, int linked_desc_id, bool unknown);	// NAME 1 [desc_id .. unknown]: what the effect packets pass; linked_desc_id: their DurationLinkedEffectDescID

#endif // EFF_O_H
