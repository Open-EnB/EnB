/*
**	Earth & Beyond client (reconstruction)
**	Portions Copyright Electronic Arts Inc., released under the GNU General Public License v3.
**
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Client\ui_line_display.h (name assumed from
**	ui_line_display.cpp, whose __FILE__ is 'e:\build\beta\G\app\Client\ui_line_display.cpp')
**
**	The line display: a screen of the UI kit (intrface.cpp's class, declared here only as far as this
**	file needs it, D10) that shows one message line at a time from a queue, keeps a history of the shown
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
#include "..\shared\unknown_debug.h"
#include "..\..\wwlib\refcount.h"
#include "..\..\wwlib\ref_ptr.h"

class INIClass;
class Vector3;

// The 0x4E8-byte text object of one STR-n section (constructor 0x6D47C0), reference counted
class LineDisplayTextClass : public RefCountClass  // NAME 1: neutral (D10): what the line display keeps per STR-n
{
public:
	LineDisplayTextClass(const char *name, int unknown1, int unknown2);		// 0x6D47C0
	bool Unknown_6D89C0();												// 0x6D89C0
	void Unknown_6CDBC0(const char *text, int unknown);					// 0x6CDBC0: slot +0x60 passes the line and 1
	void Unknown_6CE910(bool history);									// 0x6CE910
	void __cdecl Unknown_6CE420(int unknown, const char *format, ...);		// 0x6CE420: slot +0x60 passes 2, '(%d) %s'

	virtual void Unknown_008();
	virtual void Unknown_00C();
	virtual void Unknown_010();
	virtual void Unknown_014();
	virtual void Unknown_018();
	virtual void Unknown_01C();
	virtual void Unknown_020();
	virtual void Unknown_024();
	virtual void Unknown_028();
	virtual void Unknown_02C();
	virtual void Unknown_030();
	virtual void Unknown_034(int unknown);								// +0x34: slot +0x60 passes the screen's +0x44
	virtual void Unknown_038();									// +0x38: Clear calls it when +0x40 says so
	virtual void Unknown_03C();
	virtual int Unknown_040();									// +0x40
	virtual void Unknown_044();
	virtual void Unknown_048();
	virtual void Unknown_04C();
	virtual void Unknown_050();
	virtual void Unknown_054(const Vector3 &offset);				// +0x54: the slot 0x7C override passes (0, -161 or 0, 0)
	virtual void Unknown_058();
	virtual void Unknown_05C();
	virtual void Unknown_060();
	virtual void Unknown_064();
	virtual void Unknown_068();
	virtual void Unknown_06C();
	virtual void Unknown_070();
	virtual void Unknown_074();
	virtual void Unknown_078();
	virtual void Unknown_07C();
	virtual void Unknown_080();
	virtual void Unknown_084();
	virtual void Unknown_088();
	virtual void Unknown_08C();
	virtual void Unknown_090();
	virtual void Unknown_094();
	virtual void Unknown_098();
	virtual void Unknown_09C();
	virtual void Unknown_0A0();
	virtual void Unknown_0A4();
	virtual void Unknown_0A8();
	virtual void Unknown_0AC();
	virtual void Unknown_0B0();
	virtual void Unknown_0B4();
	virtual void Unknown_0B8();
	virtual void Unknown_0BC();
	virtual void Unknown_0C0();
	virtual void Unknown_0C4();
	virtual void Unknown_0C8();
	virtual void Unknown_0CC();
	virtual void Unknown_0D0();
	virtual void Unknown_0D4();
	virtual void Unknown_0D8();
	virtual void Unknown_0DC();
	virtual void Unknown_0E0();
	virtual void Unknown_0E4();
	virtual void Unknown_0E8();
	virtual void Unknown_0EC();
	virtual void Unknown_0F0();
	virtual void Unknown_0F4();
	virtual void Unknown_0F8();
	virtual void Unknown_0FC();
	virtual void Unknown_100();
	virtual void Unknown_104();
	virtual void Unknown_108();
	virtual void Unknown_10C();
	virtual void Unknown_110();
	virtual void Unknown_114();
	virtual void Unknown_118();
	virtual void Unknown_11C();
	virtual void Unknown_120();
	virtual void Unknown_124();
	virtual void Unknown_128();
	virtual void Unknown_12C();
	virtual void Unknown_130();
	virtual void Unknown_134();
	virtual void Unknown_138();
	virtual void Unknown_13C();
	virtual void Unknown_140();
	virtual void Unknown_144();
	virtual void Unknown_148();
	virtual void Unknown_14C();
	virtual void Unknown_150();
	virtual void Unknown_154();
	virtual void Unknown_158();
	virtual void Unknown_15C();
	virtual void Unknown_160();
	virtual void Unknown_164();
	virtual void Unknown_168();
	virtual void Unknown_16C(bool on);							// +0x16C: Load passes true
	virtual void Unknown_170();
	virtual void Unknown_174();
	virtual void Unknown_178();
	virtual void Unknown_17C();
	virtual void Unknown_180();
	virtual void Unknown_184();
	virtual void Unknown_188();
	virtual void Unknown_18C();
	virtual void Unknown_190();
	virtual void Unknown_194();
	virtual void Unknown_198();
	virtual void Unknown_19C();
	virtual void Unknown_1A0();
	virtual void Unknown_1A4();
	virtual bool Unknown_1A8();									// +0x1A8

	char Unknown008[0x24C - 0x08];														// +0x08
	char Unknown24C[0x440 - 0x24C];														// +0x24C: what Add_Line's wrapper is built on
	int Unknown440;																			// +0x440: 2 or 3 let a line show
	char Unknown444[0x4E8 - 0x444];														// +0x444
};

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
	void Show(bool on);																		// 0x5A53E0  // NAME 2: describes what it does
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

class ControlKeysManagerClass;  // control_keys_manager.cpp's (0x71A960 / 0x71AB40)
class LineDisplayClientClass;  // what +0x40 points to (slot +0x18 gives the sclient)

// The UI kit's base classes as far as this file needs them (kernel-map-step9.md section 3.2): an inline base
// with tables at +0x00 and +0x04, gclass.cpp's class, intrface.cpp's class with RefCountClass at +0x38
class UnknownGadgetBase00Class  // NAME 1: neutral (D10): the inline base's first table (0xAEAB48)
{
public:
	virtual ~UnknownGadgetBase00Class();
	virtual void Unknown_04();
	virtual void Unknown_08();
	virtual void Unknown_0C();
	virtual void Unknown_10();
	virtual void Unknown_14();
	virtual void Unknown_18();
	virtual void Unknown_1C();
	virtual void Unknown_20();
	virtual void Unknown_24();
	virtual void Unknown_28();
	virtual void Unknown_2C();
	virtual void Unknown_30();
	virtual void Unknown_34();
	virtual void Unknown_38();
	virtual void Unknown_3C();
	virtual void Unknown_40();
	virtual void Unknown_44();
	virtual void Unknown_48();
	virtual void Unknown_4C();
	virtual void Unknown_50();
};

class UnknownGadgetBase04Class  // NAME 1: neutral (D10): the inline base's second table (0xAEAB3C)
{
public:
	virtual ~UnknownGadgetBase04Class();
	virtual void Unknown_04();																// 0x4A1930
};

class GClassUnknownClass : public UnknownGadgetBase00Class, public UnknownGadgetBase04Class  // NAME 1: neutral (D10): gclass.cpp's class (constructor 0x4A1880)
{
protected:
	char Unknown08[0x38 - 0x08];															// +0x08
};

class InterfaceClass : public GClassUnknownClass, public RefCountClass  // NAME 3: by convention (D24) from the client's intrface.cpp (constructors 0x540A90 / 0x540D30, table 0xAEEF98)
{
public:
	InterfaceClass(int unknown1, int unknown2);												// 0x540A90
	virtual ~InterfaceClass();																// 0x540E90 (slot 0x540D00)
	virtual void Unknown_54();																// 0x515690
	virtual void Unknown_58();																// 0x541010
	virtual void Unknown_5C();																// 0x541210
	virtual void Unknown_60();																// 0x540C70
	virtual void Unknown_64();																// 0x5156B0
	virtual bool Unknown_68(int key);														// 0x5156D0
	virtual void Unknown_6C();																// 0x5156F0
	virtual void Unknown_70(ControlKeysManagerClass *manager);								// 0x515710
	virtual void Unknown_74(ControlKeysManagerClass *manager);								// 0x515730
	virtual void Unknown_78();																// 0x515750
	virtual void Unknown_7C(int unknown1, int unknown2);									// 0x5412F0
	virtual void Unknown_80();																// 0x541550
	virtual void Unknown_84();																// 0x541690
	void *Unknown_541800();													// 0x541800

protected:
	LineDisplayClientClass *Client;															// +0x40 (the assert's name)
	int Unknown44;																			// +0x44: nothing is shown while it is 0
	int Unknown48;																			// +0x48
	char Unknown4C[0x84 - 0x4C];															// +0x4C: what the control keys are registered with
};

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
	virtual void Unknown_70(ControlKeysManagerClass *manager);								// 0x5A6550: registers the history keys
	virtual void Unknown_74(ControlKeysManagerClass *manager);								// 0x5A6780: removes them
	virtual void Unknown_7C(int unknown1, int unknown2);									// 0x5A7E80

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
