// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location unknown: the object after attrib_dir_collective.cpp's (client 0x4DFBF0-0x4E07C8,
**	vtables 0xAEC3D0-0xAEC410 and its own __stl_prime_list 0xAEC414), with no string of its own;
**	the provisional name stays (D137).
**
**	Mods that tie one attribute to others: a link mod hands one directory's attribute to another's
**	(and recalculates it when the source changes), a two-source mod combines two attributes of one
**	directory (the sum is the one the client has). Named by what they do (D24).
*/



#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __UNKNOWN_4DFBF0_H  // NAME 1: provisional file name (D137)
#define __UNKNOWN_4DFBF0_H

#include <string>
#include "attrib_dir.h"

// A mod whose value is another directory's attribute, recalculated when that one changes (vtables 0xAEC3D4 / 0xAEC3D0, 0x3C bytes)
class AttribLinkModClass : public AttribModClass, public AttribCallbackClass  // NAME 2: what it is
{
public:
	AttribLinkModClass(AttribDir *src, const std::string &src_name, AttribDir *dst, const std::string &dst_name, int type, void *owner);	// 0x4DFCD0
	virtual ~AttribLinkModClass();																// 0x4DFF00
	virtual int Get_Type();																			// 0x4E0010
	virtual float Get_Value();																		// 0x4E0050
	virtual void *Get_Owner();																		// 0x4E0030
	virtual void Value_Changed(const char *name, float value);							// 0x4E0070

	AttribShadowClass Shadow;																		// +0x08: Src's attribute  // NAME 2: what it holds
	AttribDir *Src;																					// +0x0C  // NAME 2: what it holds
	std::string SrcName;																				// +0x10  // NAME 2: what it holds
	AttribDir *Dst;																					// +0x20  // NAME 2: what it holds
	std::string DstName;																				// +0x24  // NAME 2: what it holds
	int Type;																							// +0x34  // NAME 2: as AttribModValueClass
	void *Owner;																						// +0x38  // NAME 2: as AttribModValueClass
};

// A mod of Name whose value comes from two attributes A and B of the same directory (vtable 0xAEC3EC, 0x48 bytes)
class AttribBinaryModClass : public AttribModClass  // NAME 2: what it is
{
public:
	AttribBinaryModClass(int type, void *owner, AttribDir *dir, const std::string &name, const std::string &a, const std::string &b);	// 0x4E00A0
	virtual ~AttribBinaryModClass();																// 0x4E02D0
	virtual int Get_Type();																			// 0x4E0260
	virtual void *Get_Owner();																		// 0x4E0280

	int Type;																							// +0x04  // NAME 2: as AttribModValueClass
	void *Owner;																						// +0x08  // NAME 2: as AttribModValueClass
	AttribDir *Dir;																					// +0x0C  // NAME 2: what it holds
	std::string Name;																					// +0x10: the attribute modded  // NAME 2: what it holds
	std::string A;																						// +0x20  // NAME 2: what it holds
	std::string B;																						// +0x30  // NAME 2: what it holds
	AttribShadowClass ShadowA;																		// +0x40  // NAME 2: what it holds
	AttribShadowClass ShadowB;																		// +0x44  // NAME 2: what it holds
};

// A + B (vtable 0xAEC400, 0x48 bytes)
class AttribSumModClass : public AttribBinaryModClass  // NAME 2: what it computes
{
public:
	AttribSumModClass(int type, void *owner, AttribDir *dir, const std::string &name, const std::string &a, const std::string &b);	// 0x4E04C0
	virtual ~AttribSumModClass();																	// 0x4E0540
	virtual float Get_Value();																		// 0x4E06A0
};

AttribLinkModClass *Link_Attribute(AttribDir *src, const std::string &src_name, AttribDir *dst, const std::string &dst_name, int type, void *owner);	// 0x4DFBF0  // NAME 2: what it does
AttribSumModClass *Create_Sum_Mod(int type, void *owner, AttribDir *dir, const std::string &name, const std::string &a, const std::string &b);	// 0x4E0430  // NAME 2: what it does

#endif //__UNKNOWN_4DFBF0_H
