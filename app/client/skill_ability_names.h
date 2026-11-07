// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location assumed: e:\build\beta\G\app\Client\skill_ability_names.h, from skill_ability_names.cpp's __FILE__
**	0xBA7A18; the client holds no string of this header.
**
**	The abilities of cskill_ability_t.ini / cskill_ability_ex_t.ini: all of them by index (_AllAbilities, the asserts' name),
**	those of each skill (_SkillAbilities, the asserts' name), and an index by name. The client's interface to it is not
**	named: the functions keep neutral names, the ones skill_render.cpp and sclient.cpp already call them by.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __SKILL_ABILITY_NAMES_H
#define __SKILL_ABILITY_NAMES_H

#include <string>
#include <vector>
#include "vector3.h"

class TextureClass;

struct Unknown815320Struct	// NAME 1: neutral (D10)
{
	Unknown815320Struct(const std::string &name, unsigned int index, unsigned int skill, unsigned int min_level, bool activated, const Vector3 *color, int energy_cost, const std::string &description) :
		Name(name),
		Unknown10(index),
		Unknown14(skill),
		Unknown18(min_level),
		Unknown1C(activated),
		Unknown20(1.0f, 1.0f, 1.0f),
		EnergyCost(energy_cost),
		Description(description)
	{
		if (color) {
			Unknown20 = *color;
		}
	}
	std::string Name;				// +0x00 the ini's section  // NAME 2: 0x6A6A80 writes it as the ability's name
	int Unknown10;					// +0x10 its index in _AllAbilities
	int Unknown14;					// +0x14 its skill
	unsigned int Unknown18;		// +0x18 'MinLevel'
	bool Unknown1C;				// +0x1C from 'Activated Abilities'
	Vector3 Unknown20;			// +0x20 'Color', or AbilityColors' Level_N
	int EnergyCost;				// +0x2C 'Cost'  // NAME 2: 0x815720's 'Energy cost'
	std::string Description;	// +0x30 'Desc'  // NAME 2: the ini's key
};

struct Unknown8155B0Struct	// NAME 1: neutral (D10)
{
	std::vector<Unknown815320Struct *> Abilities;	// NAME 3: skill_render.cpp's FAIL_IF
	int NumActivatable;										// +0x10  // NAME 3: skill_render.cpp's FAIL_IF
	Unknown8155B0Struct() : NumActivatable(0) {}		// 0x815640
};

void Unknown_814430();													// 0x814430 (SClient::Unload_INIs)
void Unknown_814620(const char *ini, const char *ex_ini);		// 0x814620 (SClient's 'cskill_ability_t.ini', 'cskill_ability_ex_t.ini')
int Unknown_815230();													// 0x815230 (_AllAbilities' size)
int Unknown_815260(const std::string &name);						// 0x815260 (an ability by name, or -1)
unsigned int Unknown_815300(const char *name);					// 0x815300 (a skill by name)
Unknown815320Struct *Unknown_815320(int ability);					// 0x815320
const Unknown8155B0Struct &Unknown_8155B0(int skill);			// 0x8155B0
TextureClass *Unknown_815690(int ability, Vector3 *color);		// 0x815690
void Unknown_815720(std::string *string, int ability, bool below, unsigned int level);	// 0x815720

#endif //__SKILL_ABILITY_NAMES_H
