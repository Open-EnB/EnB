// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: app\client\pt_data.h (PROVISIONAL, D137: the header of pt_data.cpp)
**
**	The petition terminal's three lists pt_data.cpp rebuilds (0x696320-0x6974F0): the missions, the news and
**	the people, each read whole from its ini file (pt_missions.ini, pt_news.ini, pt_people.ini) by its
**	constructor. The terminal's pages ui_ptmis.cpp, ui_ptnew.cpp and ui_ptppl.cpp make and read them.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PT_DATA_H  // NAME 1 [pt_data.h]: provisional header name for pt_data.cpp
#define PT_DATA_H

#include <stdlib.h>
#include <string.h>
#include <vector>

// One mission of pt_missions.ini (0x14 bytes; a section '%d' each)
struct Unknown696320RecordStruct  // NAME 1: neutral (D10)
{
	Unknown696320RecordStruct() { memset(this, 0, sizeof(*this)); }
	~Unknown696320RecordStruct() { free(Type); free(Sponsor); free(Short); delete [] Desc; free(Payout); }

	char *Type;											// +0x00 (the row's mission)  // NAME 2: its ini key 'Type'
	char *Sponsor;										// +0x04 (the row's request)  // NAME 2: its ini key 'Sponsor'
	char *Short;										// +0x08  // NAME 2: its ini key 'Short'
	char *Desc;											// +0x0C (the description)  // NAME 2: its ini section '%d\Desc'
	char *Payout;										// +0x10 (the row's payout)  // NAME 2: its ini key 'Payout'
};

// pt_data.cpp's missions list (0x10 bytes: constructor 0x696320, destructor 0x696700)
class Unknown696320Class  // NAME 1: neutral (D10)
{
public:
	Unknown696320Class();								// 0x696320
	~Unknown696320Class();								// 0x696700
	Unknown696320RecordStruct *Unknown_6967A0(int idx);	// 0x6967A0 (G_ASSERT 'idx >= 0')  // NAME 3 [idx]: the G_ASSERT's
	int Count() { return(List.size()); }				// NAME 1: what it returns

	std::vector<Unknown696320RecordStruct *> List;		// +0x00  // NAME 1: what it holds
};

// One news item of pt_news.ini (0x10 bytes; a section '%d' each)
struct Unknown696810RecordStruct  // NAME 1: neutral (D10)
{
	Unknown696810RecordStruct() { memset(this, 0, sizeof(*this)); }
	~Unknown696810RecordStruct() { free(Date); free(Location); free(Headline); delete [] Story; }

	char *Date;											// +0x00 (the row's date)  // NAME 2: its ini key 'Date'
	char *Location;										// +0x04 (the row's location)  // NAME 2: its ini key 'Location'
	char *Headline;										// +0x08 (the row's title)  // NAME 2: its ini key 'Headline'
	char *Story;										// +0x0C (the text)  // NAME 2: its ini section '%d\Story'
};

// pt_data.cpp's news list (0x10 bytes: constructor 0x696810, destructor 0x696BC0)
class Unknown696810Class  // NAME 1: neutral (D10)
{
public:
	Unknown696810Class();								// 0x696810
	~Unknown696810Class();								// 0x696BC0
	Unknown696810RecordStruct *Unknown_696C60(int idx);	// 0x696C60 (G_ASSERT 'idx >= 0')  // NAME 3 [idx]: the G_ASSERT's
	int Count() { return(List.size()); }				// NAME 1: what it returns

	std::vector<Unknown696810RecordStruct *> List;		// +0x00  // NAME 1: what it holds
};

// One person of pt_people.ini (0x20 bytes; a section '%d' each)
struct Unknown696CD0RecordStruct  // NAME 1: neutral (D10)
{
	Unknown696CD0RecordStruct() { memset(this, 0, sizeof(*this)); }
	~Unknown696CD0RecordStruct() { free(Name); free(Profession); free(Location); delete [] Background; }

	char *Name;											// +0x00 (the row's name)  // NAME 2: its ini key 'Name'
	char *Profession;									// +0x04 (the row's profession)  // NAME 2: its ini key 'Profession'
	char *Location;										// +0x08 (the row's location)  // NAME 2: its ini key 'Location'
	char *Background;									// +0x0C (the profile)  // NAME 2: its ini section '%d\Background'
	int Level;											// +0x10 (the row's level)  // NAME 2: its ini key 'Level'
	int Combat;											// +0x14 ('Combat: %d')  // NAME 2: its ini key 'Combat'
	int Explore;										// +0x18 ('Explore: %d')  // NAME 2: its ini key 'Explore'
	int Trade;											// +0x1C ('Trade: %d')  // NAME 2: its ini key 'Trade'
};

// pt_data.cpp's people list (0x10 bytes: constructor 0x696CD0, destructor 0x697110)
class Unknown696CD0Class  // NAME 1: neutral (D10)
{
public:
	Unknown696CD0Class();								// 0x696CD0
	~Unknown696CD0Class();								// 0x697110
	Unknown696CD0RecordStruct *Unknown_6971B0(int idx);	// 0x6971B0 (G_ASSERT 'idx >= 0')  // NAME 3 [idx]: the G_ASSERT's
	int Count() { return(List.size()); }				// NAME 1: what it returns

	std::vector<Unknown696CD0RecordStruct *> List;		// +0x00  // NAME 1: what it holds
};

#endif
