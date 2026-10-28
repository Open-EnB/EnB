/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
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

class ClientClass;
class RenderObjClass;
class CameraClass;
class AudibleSoundClass;
class UnknownB00060EffectClass;
class UnknownB0009CEffectClass;

// The base with only a virtual destructor (vtable 0xAF0DD0, its deleting destructor 0x57DBC0 first emitted
// in ui_cpit.cpp; influence.h's record has the same base)
class UnknownAF0DD0Class	// NAME 1: neutral (D10)
{
public:
	virtual ~UnknownAF0DD0Class() {}
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
		} Flag;
	};
};

void Unknown_6E6A90(EffectObjectClass *effect);		// 0x6E6A90: the effect ended if it runs, and deleted (SClient's Shutdown and Process_AI)

#endif // EFF_O_H
