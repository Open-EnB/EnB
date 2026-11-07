// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location assumed: e:\build\beta\G\app\Client\skillnam.h, from skillnam.cpp's __FILE__ 0xBA7E00;
**	the client holds no string of this header.
**
**	The skills of cskill_t.ini / cskill_ex_t.ini. Only Max_Skill's owner is named by the client (0x6A4F40's assert); the
**	other functions take no this and keep the neutral names their callers already used, though they are probably
**	static members of the same class.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __SKILLNAM_H
#define __SKILLNAM_H

#include <string>

class TextureClass;

class SkillNamesDatabase	// NAME 5: 0x6A4F40's assert
{
public:
	static int Max_Skill();	// 0x818590  // NAME 5: 0x6A4F40's assert
};

void Unknown_816E60();																// NAME 1: neutral (D10)
void Unknown_817070(const char *ini_name, const char *ex_ini_name);	// NAME 1: neutral (D10)
int Unknown_818170(const std::string &name);									// a skill's index by its name  // NAME 1: neutral (D10)
const char *Unknown_818200(int index);											// its name  // NAME 1: neutral (D10)
const char *Unknown_818250(int index);											// its description  // NAME 1: neutral (D10)
const char *Unknown_8182A0(int index, int level);							// a level's description  // NAME 1: neutral (D10)
int Unknown_818340(int index, unsigned int idx);							// a range  // NAME 1: neutral (D10)
const char *Unknown_8183E0(int index);											// its icon's name  // NAME 1: neutral (D10)
TextureClass *Unknown_818430(int index);										// its icon  // NAME 1: neutral (D10)
const char *Unknown_8184D0(int index);											// its category  // NAME 1: neutral (D10)
int Unknown_818520(const char *name);											// a skill's index by its name  // NAME 1: neutral (D10)
unsigned int Unknown_8185C0(int index, int char_tmpl);					// a template's maximum  // NAME 1: neutral (D10)
const char *Unknown_818640(int index, int char_tmpl);						// a template's trainer  // NAME 1: neutral (D10)

#endif
