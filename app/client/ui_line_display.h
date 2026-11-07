// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Client\ui_line_display.h (name assumed from
**	ui_line_display.cpp, whose __FILE__ is 'e:\build\beta\G\app\Client\ui_line_display.cpp')
**
**	The line display: a screen of the UI kit (intrface.cpp's class, intrface.h) that shows one message line at a time from a queue, keeps a history of the shown
**	lines (Prev / Next Ephemeral History), picks random texts and fonts from ephemeral.ini and keeps one
**	text object per STR-n section of fc_line_display.ini, both in SGI hash_maps keyed by name
**	(libs/sgistl, D87). The client names neither the classes nor the members (D10, D24); the ini keys
**	and the asserts ('Client', 'Current->DisplayTimeMS > 0') name what they can.
*/



#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __UI_LINE_DISPLAY_H  // NAME 3 [ui_line_display.h]: header name assumed from the client's ui_line_display.cpp
#define __UI_LINE_DISPLAY_H

#include <string>
#include <vector>
#include <hash_map.h>
#include "..\shared\g_assert.h"
#include "..\..\wwlib\refcount.h"
#include "..\..\wwlib\ref_ptr.h"
#include "intrface.h"

class INIClass;
class Vector3;

// The text object of one STR-n section is fontmanager.h's FontSys (0x4E8 bytes, constructor 0x6D47C0)
class FontSys;
typedef FontSys LineDisplayTextClass;  // NAME 1: neutral (D10): what the line display keeps per STR-n

// One Ephemeral-%02d section of ephemeral.ini (0x88 bytes)
struct EphemeralStruct  // NAME 2: what it holds (the ini's section name)
{
	EphemeralStruct(int index, INIClass &ini);												// 0x5A4B50
	std::string Pick_Font();																// 0x5A5080: a FontType by the FontChance weights  // NAME 2: describes what it does
	std::string Get_Type() const { return Type; }											// 0x5A6200 (out of line in Load)  // NAME 2: describes what it does
	std::string Get_Text() const { return Text; }											// 0x5A7C60  // NAME 2: describes what it does

	std::string Type;																		// +0x00, Type  // NAME 2: the ini key
	std::string Text;																		// +0x10, Text  // NAME 2: the ini key
	std::string FontType[5];																// +0x20, FontType%02d  // NAME 2: the ini key
	int Chance[5];																			// +0x70, FontChance%02d summed up  // NAME 2: the ini key
	int TotalChance;																		// +0x84  // NAME 2: describes what it holds
};

// One STR-n section of fc_line_display.ini (0x14 bytes)
struct LineDisplayStringStruct  // NAME 2: what it holds (the ini's section name)
{
	LineDisplayStringStruct(const char *name);												// 0x5A52A0
	void Show(int on);																		// 0x5A53E0  // NAME 2: describes what it does
	std::string Get_Name() const { return Name; }											// 0x5A6380 (out of line in Load)  // NAME 2: describes what it does
	RefCountPtr<LineDisplayTextClass> Get_Text() const { return Text; }					// 0x5A80D0  // NAME 2: describes what it does

	std::string Name;																		// +0x00, StringID  // NAME 2: the ini key
	RefCountPtr<LineDisplayTextClass> Text;													// +0x10  // NAME 2: describes what it holds
};

// One queued or shown line (0x28 bytes)
struct LineDisplayLineStruct  // NAME 2: what it holds
{
	std::string Text;																		// +0x00  // NAME 2: describes what it holds
	std::string Font;																		// +0x10  // NAME 2: describes what it holds
	unsigned int DisplayTimeMS;																		// +0x20 (the assert's name)
	unsigned int Priority;																			// +0x24  // NAME 2: describes what it holds
};

// the UI kit's base classes: intrface.h

class UILineDisplayClass : public InterfaceClass  // NAME 3: by convention (D24) from the client's ui_line_display.cpp (table 0xAF2930)
{
public:
	UILineDisplayClass(LineDisplayClientClass *client);										// 0x5A5410
	virtual ~UILineDisplayClass();															// 0x5A58A0
	virtual void Unknown_54();																// 0x5A5DA0: loads ephemeral.ini and fc_line_display.ini  // NAME 1: neutral (D10)
	virtual void Unknown_58();																// 0x5A6500
	virtual void Unknown_5C();																// 0x5A6520
	virtual void Unknown_60();																// 0x5A69B0: shows the next line
	virtual bool Unknown_68(int key);														// 0x5A70B0: 1 = previous, 2 = next history line
	virtual void Unknown_70(ControlKeysManager *manager);								// 0x5A6550: registers the history keys
	virtual void Unknown_74(ControlKeysManager *manager);								// 0x5A6780: removes them
	virtual void Unknown_7C(UIViewClass *view, bool arg);									// 0x5A7E80

	std::string Format_Line(const char *text, int count, int index);				// 0x5A72B0: one of count lines, '...' where it was split  // NAME 2: describes what it does
	void Add_Line(const char *text, int display_time, const char *font, int priority);	// 0x5A7690  // NAME 2: describes what it does
	void Add_Line_7(const char *text, int display_time, const char *font);				// 0x5A7A20  // NAME 1: neutral (D10): Add_Line at priority 7
	void Add_Line_3(const char *text, int display_time, const char *font);				// 0x5A7A50  // NAME 1: neutral (D10): Add_Line at priority 3
	void Show_Ephemeral(const char *type);													// 0x5A7A80  // NAME 2: describes what it does
	void Flush_Lines();																		// 0x5A7DE0: every queued line into the history  // NAME 2: describes what it does
	void Clear();																			// 0x5A8100  // NAME 2: describes what it does
	void Skip();																			// 0x5A8310  // NAME 2: describes what it does
	void Unknown_5A8350();																	// 0x5A8350: the mission messages
	EphemeralStruct *Find_Ephemeral(const char *type);										// 0x5A8B60  // NAME 2: describes what it does
	RefCountPtr<LineDisplayTextClass> Find_Text(const char *name);							// 0x5A8C70  // NAME 2: describes what it does

private:
	LineDisplayLineStruct *Current;															// +0x84 (the assert's name)
	std::vector<LineDisplayLineStruct *> Lines;												// +0x88: queued, by priority  // NAME 2: describes what it holds
	std::vector<LineDisplayLineStruct *> History;											// +0x98: shown  // NAME 2: describes what it holds
	int HistoryIndex;																		// +0xA8: -1 when not browsing  // NAME 2: describes what it holds
	bool HistoryMode;																		// +0xAC  // NAME 2: describes what it holds
	bool UnknownAD;																			// +0xAD
	int UnknownB0;																			// +0xB0
	int UnknownB4;																			// +0xB4
	bool UnknownB8;																			// +0xB8
	std::hash_map<std::string, EphemeralStruct *> Ephemerals;								// +0xBC  // NAME 2: describes what it holds
	std::hash_map<std::string, LineDisplayStringStruct *> Strings;							// +0xD4  // NAME 2: describes what it holds
	std::string Style;																		// +0xEC: "Simple", "MessageLine"  // NAME 2: describes what it holds
	bool UnknownFC;																			// +0xFC
};

#endif //__UI_LINE_DISPLAY_H
