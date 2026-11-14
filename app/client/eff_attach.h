// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\client\eff_attach.h (name assumed from eff_attach.cpp)
**
**	The attach effect: effect.ini's EffectClass 'AttachEffect' (0xB92EB8), which the factory's helper 0x6EC570
**	makes. When it starts it puts a copy of its model on each bone of its source's model whose name begins with
**	BoneNamePrefix (every bone without one), sub-objects' bones included; when it ends it takes them off.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef EFF_ATTACH_H  // NAME 3 [eff_attach.h]: header name assumed from the client's eff_attach.cpp
#define EFF_ATTACH_H

#include "eff_o.h"
#include "ref_ptr.h"
#include <vector>

// vtable 0xAFFE88 (12 slots), 0xF4 bytes, constructor 0x6E4750, destructor 0x6E4A30 (scalar deleting 0x6E4940).
// Its own flag bits: 0x200 a copy hidden while the bone's first sub-object is (effect.ini's HideAsOtherObjects),
// 0x400 its model's texture mappers reset at the start (ResetMappers).
class AttachEffectClass : public UnknownB00060EffectClass	// NAME 3: effect.ini's EffectClass 'AttachEffect' (0xB92EB8), which the helper 0x6EC570 makes
{
public:
	AttachEffectClass(ClientClass *client, unsigned id, unsigned start_time, unsigned duration, unsigned cycle_time, const RefCountPtr<RenderObjClass> &robj, const char *bone_prefix);	// 0x6E4750  // NAME 1 [client .. cycle_time]: the base's; robj, bone_prefix: the members
	virtual ~AttachEffectClass();						// +0x00 0x6E4A30 (scalar deleting 0x6E4940)
	virtual Vector3 Unknown_08(const Vector3 &position);	// +0x08 0x6E4BA0 (Transform's translation)  // NAME 1 [position]: the base's
	virtual void Unknown_18();							// +0x18 0x6E4BE0 (the copies onto the source's model)
	virtual void Unknown_20();							// +0x20 0x6E4CC0 (the copies off again)

	void Unknown_6E4D00(const RefCountPtr<RenderObjClass> &robj);	// 0x6E4D00 (FAIL_IF '!robj' l.64, '!sub_obj' l.77; recursive over the sub-objects)  // NAME 1 [robj]: the FAIL_IF's

	std::vector<RefCountPtr<RenderObjClass> > Copies;	// +0xD8 (its allocator byte, then +0xDC..+0xE4)  // NAME 2: what it holds
	RefCountPtr<RenderObjClass> RObj;					// +0xE8  // NAME 2: as eff_robj.h's
	char *BonePrefix;									// +0xEC (nstrdup; NULL: every bone)  // NAME 3: effect.ini's 'BoneNamePrefix'
	int BonePrefixLength;								// +0xF0 (strlen)  // NAME 2: what it holds
};

#endif // EFF_ATTACH_H
