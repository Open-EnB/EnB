// Earth & Beyond client (reconstruction): app\Client\ui_line_display.cpp. Notes at the end of the file.
#include <stdio.h>
#include <string.h>
#include "ui_line_display.h"
#include "..\shared\unknown_debug.h"
#include "..\..\wwlib\ini.h"
#include "..\..\wwlib\wwstring.h"
#include "..\..\wwlib\random.h"
#include "..\..\wwmath\vector3.h"

extern RandomClass Unknown_BDBB84;  // NAME 1 [Unknown_BDBB84]: neutral (D10): the random generator the fonts are picked with

class LineDisplayFontDatabaseClass  // NAME 1: neutral (D10): fontdatabase.cpp's class (the one at 0xBE67BC)
{
public:
	void Unknown_6C52B0(const char *file_name);				// 0x6C52B0: the constructor passes fc_line_display.ini
	void Unknown_6C55B0(const char *file_name);				// 0x6C55B0: the destructor passes it
};
extern LineDisplayFontDatabaseClass *Unknown_BE67BC;		// NAME 1 [Unknown_BE67BC]: neutral (D10)

class ControlKeysManagerClass  // NAME 3: by convention (D24) from the client's control_keys_manager.cpp
{
public:
	void Unknown_71A960(void *handler, const std::string &name, int id, int unknown);	// 0x71A960
	void Unknown_71AB40(void *handler, const std::string &name, int unknown);	// 0x71AB40
};

class LineDisplaySClientClass  // NAME 1: neutral (D10): sclient.cpp's class, as far as slot +0x7C reads it
{
public:
	char Unknown0000[0x1354];
	int Unknown1354;										// +0x1354
	int Unknown1358;										// +0x1358
	int Unknown135C;										// +0x135C
};

class LineDisplayClientClass  // NAME 1: neutral (D10)
{
public:
	virtual void Unknown_00();
	virtual void Unknown_04();
	virtual void Unknown_08();
	virtual void Unknown_0C();
	virtual void Unknown_10();
	virtual void Unknown_14();
	virtual LineDisplaySClientClass *Unknown_18();			// +0x18: the sclient
};


// NOT EXACT: register-only (explain.py), the Chance pointer stepped before the type string is freed; the client after it
EphemeralStruct::EphemeralStruct(int index, INIClass &ini) : Type(""), Text(""), TotalChance(0)
{
	for (int i = 0; i < 5; i++) {
		FontType[i] = "";
		Chance[i] = 0;
	}
	char section[64];
	_snprintf(section, sizeof(section), "Ephemeral-%02d", index);
	if (ini.Find_Section(section)) {
		StringClass value;
		ini.Get_String(value, section, "Type", "");
		Type = value;
		ini.Get_String(value, section, "Text", "");
		Text = value;
		int font = 0;
		while (true) {
			char entry[64];
			_snprintf(entry, sizeof(entry), "FontType%02d", font);
			if (!ini.Find_Entry(section, entry)) {
				break;
			}
			StringClass type;
			ini.Get_String(type, section, entry, "");
			FontType[font] = type;
			_snprintf(entry, sizeof(entry), "FontChance%02d", font);
			float chance = ini.Get_Float(section, entry, 0);
			TotalChance = TotalChance + chance;
			Chance[font] = TotalChance;
			font++;
		}
	}
}


std::string EphemeralStruct::Pick_Font()
{
	int roll = Unknown_BDBB84(0, TotalChance);
	for (int i = 0; i < 5; i++) {
		if (Chance[i] > roll && FontType[i] != "") {
			return FontType[i];
		}
	}
	return std::string("");
}


LineDisplayStringStruct::LineDisplayStringStruct(const char *name) : Name(name)
{
	Text = Create_NEW(new LineDisplayTextClass(Name.c_str(), 0, 0));
}


void LineDisplayStringStruct::Show(bool on)
{
	Text->Unknown_16C(on);
}





















































UILineDisplayClass::UILineDisplayClass(LineDisplayClientClass *client) :
	InterfaceClass(0, 0),
	Current(0),
	HistoryIndex(-1),
	HistoryMode(false),
	UnknownAD(true),
	UnknownB8(false),
	Style(""),
	UnknownFC(true)
{
	Unknown_BE67BC->Unknown_6C52B0("fc_line_display.ini");
	Client = client;
	G_ASSERT(Client);
}


// NOT EXACT: shape (explain.py: 31 vs 30 calls), as buff_database.cpp's 0x677410: the hash_map loops keep other registers and ours calls the vector's empty _Destroy
UILineDisplayClass::~UILineDisplayClass()
{
	Unknown_BE67BC->Unknown_6C55B0("fc_line_display.ini");
	Clear();
	for (std::vector<LineDisplayLineStruct *>::iterator line = Lines.begin(); line != Lines.end(); ++line) {
		delete *line;
	}
	Lines.clear();
	for (line = History.begin(); line != History.end(); ++line) {
		delete *line;
	}
	History.clear();
	for (std::hash_map<std::string, EphemeralStruct *>::iterator e = Ephemerals.begin(); e != Ephemerals.end(); ++e) {
		delete (*e).second;
	}
	Ephemerals.clear();
	for (std::hash_map<std::string, LineDisplayStringStruct *>::iterator s = Strings.begin(); s != Strings.end(); ++s) {
		delete (*s).second;
	}
	Strings.clear();
}


void UILineDisplayClass::Unknown_54()
{
	char section[64];
	INIClass ini("ephemeral.ini");
	Ephemerals.clear();
	int index = 0;
	while (true) {
		_snprintf(section, sizeof(section), "Ephemeral-%02d", index);
		if (!ini.Find_Section(section)) {
			break;
		}
		EphemeralStruct *ephemeral = new EphemeralStruct(index, ini);
		Ephemerals[ephemeral->Get_Type()] = ephemeral;
		index++;
	}
	Strings.clear();
	INIClass strings("fc_line_display.ini");
	int id = 1;
	while (true) {
		{
			_snprintf(section, sizeof(section), "STR-%d", id);
			if (!strings.Find_Section(section)) {
				break;
			}
			StringClass name;
			strings.Get_String(name, section, "StringID", "");
			LineDisplayStringStruct *string = new LineDisplayStringStruct(name);
			string->Show(true);
			Strings[string->Get_Name()] = string;
		}
		id++;
	}
}


void UILineDisplayClass::Unknown_58()
{
	UnknownB8 = false;
	InterfaceClass::Unknown_58();
}


void UILineDisplayClass::Unknown_5C()
{
	Clear();
	InterfaceClass::Unknown_5C();
}


































void UILineDisplayClass::Unknown_70(ControlKeysManagerClass *manager)
{
	FAIL_IF(!manager) return;
	manager->Unknown_71A960(Unknown4C, std::string("Prev Ephemeral History"), 1, 0);
	manager->Unknown_71A960(Unknown4C, std::string("Next Ephemeral History"), 2, 0);
}

void UILineDisplayClass::Unknown_74(ControlKeysManagerClass *manager)
{
	FAIL_IF(!manager) return;
	manager->Unknown_71AB40(Unknown4C, std::string("Prev Ephemeral History"), 0);
	manager->Unknown_71AB40(Unknown4C, std::string("Next Ephemeral History"), 0);
}


bool UILineDisplayClass::Unknown_68(int key)
{
	switch (key) {
		case 1:
			if (HistoryIndex == -1) {
				if (!UnknownFC && Style != "") {
					Clear();
				}
				HistoryIndex = History.size() - 1;
			} else if (HistoryIndex > 0) {
				Skip();
				HistoryIndex--;
			} else {
				Skip();
				HistoryIndex = -1;
			}
			break;
		case 2:
			if (HistoryIndex == -1) {
				if (Current && Current->DisplayTimeMS) {
					Current->DisplayTimeMS = 1;
				}
			} else {
				HistoryIndex++;
				if (HistoryIndex >= (int)History.size()) {
					HistoryIndex = -1;
				}
				Skip();
			}
			break;
	}
	return(true);
}


void UILineDisplayClass::Add_Line_7(const char *text, int display_time, const char *font)
{
	Add_Line(text, display_time, font, 7);
}


void UILineDisplayClass::Add_Line_3(const char *text, int display_time, const char *font)
{
	Add_Line(text, display_time, font, 3);
}


// NOT EXACT: inline budget (explain.py), the client converts find's and end()'s iterators to const_iterator out of line (0x5A9010), ours inline
void UILineDisplayClass::Show_Ephemeral(const char *type)
{
	if (type) {
		std::hash_map<std::string, EphemeralStruct *>::const_iterator i = Ephemerals.find(std::string(type));
		if (i != Ephemerals.end() && (*i).second) {
			Add_Line((*i).second->Get_Text().c_str(), 0, (*i).second->Pick_Font().c_str(), 2);
		}
	}
}


void UILineDisplayClass::Flush_Lines()
{
	for (std::vector<LineDisplayLineStruct *>::iterator line = Lines.begin(); line != Lines.end(); ++line) {
		History.push_back(*line);
	}
	Lines.erase(Lines.begin(), Lines.end());
	Clear();
}



































































































































































































































// NOT EXACT: shape (explain.py), the hash_map loop's first bucket search laid out differently (as 0x677410)
void UILineDisplayClass::Unknown_7C(int unknown1, int unknown2)
{
	Clear();
	LineDisplaySClientClass *sclient = Client ? Client->Unknown_18() : 0;
	FAIL_IF(!sclient) {
		unknown1 = 0;
	}
	Vector3 offset(0.0f, 0.0f, 0.0f);
	if (unknown1) {
		if (unknown1 == sclient->Unknown1354) {
			offset.Set(0.0f, -161.0f, 0.0f);
			UnknownAD = false;
		} else if (unknown1 == sclient->Unknown135C) {
			offset.Set(0.0f, -161.0f, 0.0f);
			UnknownAD = true;
		} else {
			unknown1 = 0;
		}
	}
	for (std::hash_map<std::string, LineDisplayStringStruct *>::iterator s = Strings.begin(); s != Strings.end(); ++s) {
		(*s).second->Get_Text()->Unknown_054(offset);
	}
	InterfaceClass::Unknown_7C(unknown1, unknown2);
}


// NOT EXACT: shape (explain.py), the hash_map iterator kept in esi by the client, in the frame by ours (as 0x677410)
void UILineDisplayClass::Clear()
{
	Style = "";
	UnknownFC = false;
	for (std::hash_map<std::string, LineDisplayStringStruct *>::iterator s = Strings.begin(); s != Strings.end(); ++s) {
		LineDisplayStringStruct *string = (*s).second;
		if (string->Get_Text()->Unknown_040()) {
			string->Get_Text()->Unknown_038();
		}
	}
	Current = 0;
	HistoryMode = false;
}


void UILineDisplayClass::Skip()
{
	if (HistoryMode && Current) {
		Current->DisplayTimeMS = 0;
		Clear();
	}
}


// NOT EXACT: inline budget (explain.py), the client inlines the key's assign and calls find 0x5A9460 out of line, ours the reverse (as 0x6ACF50)
EphemeralStruct *UILineDisplayClass::Find_Ephemeral(const char *type)
{
	std::hash_map<std::string, EphemeralStruct *>::iterator i = Ephemerals.find(std::string(type));
	if (i == Ephemerals.end()) {
		return 0;
	}
	return (*i).second;
}


// NOT EXACT: inline budget (explain.py), the client calls find 0x5A95E0 and the key's _Eos out of line (as 0x4E57A0)
RefCountPtr<LineDisplayTextClass> UILineDisplayClass::Find_Text(const char *name)
{
	std::hash_map<std::string, LineDisplayStringStruct *>::iterator i = Strings.find(std::string(name));
	if (i == Strings.end()) {
		return 0;
	}
	return (*i).second->Get_Text();
}

/*
**	Reconstruction notes (builder H10, 2026-10-27)
**
**	The client's object: 0x5A4B50-0x5AAE30 (the __FILE__ window 0x5A5410-0x5A8350, its template
**	instances after it, the destructor's adjustor thunks 0x5AAE20 / 0x5AAE30 last). Order of the file:
**	the ephemeral entry (0x5A4B50 constructor, 0x5A5080 Pick_Font), the STR-n entry (0x5A52A0, 0x5A53E0),
**	the screen's constructor 0x5A5410 (G_ASSERT(Client) on the client's line 172), destructor 0x5A58A0,
**	slots +0x54 0x5A5DA0 (the ini loaders), +0x58, +0x5C, +0x70 / +0x74 (the history keys, FAIL_IF on
**	lines 283 / 290), +0x68 0x5A70B0 (the keys: 1 previous, 2 next), the two Add_Line wrappers, Show_Ephemeral,
**	Flush_Lines, slot +0x7C 0x5A7E80 (FAIL_IF(!sclient) on line 595), Clear, Skip, Find_Ephemeral, Find_Text.
**
**	Not written yet (declared only): slot +0x60 0x5A69B0 (shows the next line: asserts on lines 347 / 375),
**	Add_Line 0x5A7690 (splits a text into lines through the 0x4FA8F0 text object and 0x5A72B0),
**	0x5A72B0 (formats one line: the first, middle and last of several get '...'), and 0x5A8350 (the
**	mission messages: FAIL_IFs on lines 658 / 661 / 671 / 735).
**
**	Layout (the constructor's stores, in initialiser-list order before the vtables): +0x84 Current, +0x88
**	and +0x98 two std::vector<LineDisplayLineStruct *>, +0xA8 -1, +0xAC false, +0xAD true, +0xB8 false
**	(+0xB0 / +0xB4 are not set), +0xBC and +0xD4 the two hash_maps (their constructors inlined: next_prime(100),
**	reserve, insert), +0xEC a std::string from "" and +0xFC true; 0x100 bytes. The base's three tables
**	(+0x00, +0x04, +0x38 RefCountClass) give the derived destructor two adjustor thunks.
**
**	Forms that mattered: while (true) loops with the counter stepped after an inner block (the ini loops
**	rotate under for (;;)); History.push_back in Flush_Lines (insert(end()) reads end() through another
**	register); `Chance[i] > roll` (operand order); the 0x4E8-byte size of the text object (its new).
*/
