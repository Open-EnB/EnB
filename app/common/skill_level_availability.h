// Earth & Beyond client (reconstruction): app\common\skill_level_availability.h
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Common\skill_level_availability.h (name assumed from
**	skill_level_availability.cpp, whose __FILE__ is 0xBB0C44; no assert names the header)
**
**	A skill level's availability to a class ('NotFound', 'NotAvailable', 'Automatic', 'QuestOnly',
**	'Trainable': the client's _ClassQualificationNames 0xBB0B80) and the four-number availability value
**	the skill roots carry, with their text converters (skill_level_availability.cpp, 0x84A470-0x84A62D).
**	aux_roots.h includes this header for the Availability field; it sees only what it declared before
**	(its declaration count moves the SYMBOL_PAD windows, workflow.md 3.4), the rest is behind
**	__SKILL_LEVEL_AVAILABILITY_ALL.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef SKILL_LEVEL_AVAILABILITY_H  // NAME 3 [skill_level_availability.h]: header name assumed from the client's skill_level_availability.cpp
#define SKILL_LEVEL_AVAILABILITY_H

// NAME 5: the client's Get_Type_Name string 'SkillLevelAvailabilityStruct' (KVPair vtable 0xB03AB4, 0x759CE0 inline), leaf vtable 0xB03694
struct SkillLevelAvailabilityStruct { int I00; int I04; int I08; int I0C; SkillLevelAvailabilityStruct() : I00(0), I04(0), I08(0), I0C(0) {} };	// vtable 0xB03694, 16 bytes, zeroed (0x757B00)
inline bool operator!=(const SkillLevelAvailabilityStruct &a, const SkillLevelAvailabilityStruct &b) { return a.I00 != b.I00 || a.I04 != b.I04 || a.I08 != b.I08 || a.I0C != b.I0C; }
inline const char *Type_Name(SkillLevelAvailabilityStruct *) { return "SkillLevelAvailabilityStruct"; }
bool String_To_Value(const char *text, SkillLevelAvailabilityStruct *value);	// 0x84A600
char *Value_To_String(char *buf, int size, const SkillLevelAvailabilityStruct &value);	// 0x84A560

#ifdef __SKILL_LEVEL_AVAILABILITY_ALL	// skill_level_availability.cpp only (aux_roots.cpp's declaration count, above)

// A skill level's availability to a class: the names 'NotFound' (0) .. 'Trainable' (4); the client's
// FAIL_IF texts name the parameter class_qualification and the table _ClassQualificationNames.
enum ClassQualificationType { CLASS_QUALIFICATION_NOT_FOUND = 0 };	// NAME 2: the client's FAIL_IF texts (class_qualification, _ClassQualificationNames)

bool String_To_Value(const char *buf, ClassQualificationType *class_qualification);			// 0x84A470 (no caller in the client)
char *Value_To_String(char *buf, int size, ClassQualificationType class_qualification);	// 0x84A4C0 (no caller in the client)

#endif

#endif
