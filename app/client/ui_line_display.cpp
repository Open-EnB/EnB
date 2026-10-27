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

class LineDisplayUnknown138Class  // NAME 1: neutral (D10)
{
public:
	char Unknown000[0xC8];
	float UnknownC8;										// +0xC8: slot +0x60 moves the Simple and MessageLine texts by it
};

class LineDisplayUnknown127CClass  // NAME 1: neutral (D10)
{
public:
	char Unknown000[0x138];
	LineDisplayUnknown138Class *Unknown138;					// +0x138
};

class LineDisplaySClientClass  // NAME 1: neutral (D10): sclient.cpp's class, as far as this file reads it
{
public:
	char Unknown0000[0x127C];
	LineDisplayUnknown127CClass *Unknown127C;				// +0x127C
	char Unknown1280[0x1354 - 0x1280];
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

// The text wrapper Add_Line splits a text with (0x2C bytes, constructor 0x4FA8F0)
class LineDisplayWrapClass  // NAME 1: neutral (D10)
{
public:
	LineDisplayWrapClass(void *font, int unknown1, int unknown2, int unknown3);	// 0x4FA8F0
	virtual ~LineDisplayWrapClass();
	void Unknown_4F9D90(const char *text, float width, int unknown);			// 0x4F9D90: Add_Line passes 575
	int Unknown_4FA710();														// 0x4FA710: the number of lines
	const char *Unknown_4FA740(int index);										// 0x4FA740: one line

private:
	char Unknown04[0x2C - 0x04];
};

// What slot +0x60 builds around the text's +0x34 call (local_interface_sort.cpp's 0x6E1090 / 0x6E18B0)
class LineDisplaySortClass  // NAME 1: neutral (D10)
{
public:
	LineDisplaySortClass(void *unknown1, int unknown2);		// 0x6E1090: slot +0x60 passes 1
	~LineDisplaySortClass();								// 0x6E18B0

private:
	char Unknown00[8];
};

extern unsigned int Unknown_BFAE3C;							// NAME 1 [Unknown_BFAE3C]: neutral (D10): the time now, as slot +0x60 reads it
extern unsigned int Unknown_BFAE40;							// NAME 1 [Unknown_BFAE40]: neutral (D10): the time of the last frame


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






















// NOT EXACT: inline budget (explain.py), the client calls the 30-line trim's push_back insert 0x5A8DD0 out of line, ours inlines it
void UILineDisplayClass::Unknown_60()
{
	Unknown_5A8350();
	if (!Unknown44) {
		return;
	}
	if (Current) {
		unsigned int elapsed = Unknown_BFAE3C - Unknown_BFAE40;
		if (Current->DisplayTimeMS) {
			if (Current->DisplayTimeMS <= elapsed) {
				Current->DisplayTimeMS = 0;
				if (HistoryMode) {
					HistoryIndex = -1;
					Clear();
				} else {
					if (Lines.size() && Current == Lines[0]) {
						while (History.size() >= 30) {
							delete History[0];
							History.erase(History.begin());
						}
						History.push_back(Current);
						Lines.erase(Lines.begin());
					}
					Clear();
				}
			} else {
				if (!HistoryMode && Lines.size() && Current != Lines[0]) {
					Clear();
				} else {
					Current->DisplayTimeMS -= elapsed;
					G_ASSERT(Current->DisplayTimeMS > 0);
				}
			}
			if (Current) {
				return;
			}
		} else {
			LineDisplayTextClass *text = Find_Text(Style.c_str()).Peek();
			if (!text) {
				return;
			}
			if (!((text->Unknown440 == 2 && text->Unknown_1A8()) || text->Unknown440 == 3 || text->Unknown_6D89C0())) {
				return;
			}
			if (Lines.size() && Current == Lines[0]) {
				History.push_back(Current);
				Lines.erase(Lines.begin());
			}
			Clear();
		}
	}







	FAIL_IF(Current) {
		Current = 0;
	}
	if (Lines.size()) {
		Current = Lines[0];
	}
	if (!Current || Current->DisplayTimeMS) {
		if (HistoryIndex >= 0) {
			Current = History[HistoryIndex];
			Current->DisplayTimeMS = 5000;
			HistoryMode = true;
		}
		if (!Current) {
			return;
		}
	}
	Style = Current->Font;
	LineDisplayTextClass *text = Find_Text(Style.c_str()).Peek();
	if (!text) {
		return;
	}
	if (Current->DisplayTimeMS) {
		UnknownFC = false;
	} else {
		UnknownFC = true;
	}
	text->Unknown_6CE910(HistoryMode);
	if (HistoryMode) {
		text->Unknown_6CE420(2, "(%d) %s", History.size() - HistoryIndex, Current->Text.c_str());
	} else {
		text->Unknown_6CDBC0(Current->Text.c_str(), 1);
	}
	if (Style == "Simple" || Style == "MessageLine") {
		LineDisplaySClientClass *sclient;
		LineDisplayUnknown138Class *object;
		if (Client && (sclient = Client->Unknown_18()) != 0 && sclient->Unknown127C && (object = sclient->Unknown127C->Unknown138) != 0) {
			Vector3 offset(0.0f, object->UnknownC8, 0.0f);
			text->Unknown_054(offset);
		}
	}
	LineDisplaySortClass sort(Unknown_541800(), 1);
	text->Unknown_034(Unknown44);
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


std::string UILineDisplayClass::Format_Line(const char *text, int count, int index)
{
	std::string line("");
	if (count == 1) {
		line = text;
	} else if (index == 0) {
		line = text;
		line += "...";
	} else if (index == count - 1) {
		line = "...";
		line += text;
	} else {
		line = "...";
		line += text;
		line += "...";
	}
	return line;
}


void UILineDisplayClass::Add_Line(const char *text, int display_time, const char *font, int priority)
{
	if (!text || !font) {
		return;
	}
	RefCountPtr<LineDisplayTextClass> font_text = Find_Text(font);
	if (font_text) {
		LineDisplayWrapClass *wrap = new LineDisplayWrapClass(font_text->Unknown24C, 0, 0, 0);
		wrap->Unknown_4F9D90(text, 575.0f, 0);
		int count = wrap->Unknown_4FA710();
		if (!count) {
			return;
		}
		std::vector<LineDisplayLineStruct *>::iterator pos = Lines.begin();
		while (pos != Lines.end() && (*pos)->Priority <= priority) {
			pos++;
		}
		for (int index = count - 1; index >= 0; index--) {
			const char *line_text = wrap->Unknown_4FA740(index);
			if (line_text) {
				LineDisplayLineStruct *line = new LineDisplayLineStruct;
				line->Font = font;
				line->Text = Format_Line(line_text, count, index);
				line->DisplayTimeMS = display_time;
				line->Priority = priority;
				pos = Lines.insert(pos, line);
			}
		}
		delete wrap;
	}
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



EphemeralStruct *UILineDisplayClass::Find_Ephemeral(const char *type)
{
	std::hash_map<std::string, EphemeralStruct *>::const_iterator i = Ephemerals.find(std::string(type));
	return i == Ephemerals.end() ? NULL : (*i).second;



}



RefCountPtr<LineDisplayTextClass> UILineDisplayClass::Find_Text(const char *name)
{
	std::hash_map<std::string, LineDisplayStringStruct *>::const_iterator i = Strings.find(std::string(name));
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
**	Slot +0x60 0x5A69B0 (shows the next line, asserts on lines 347 / 375), Format_Line 0x5A72B0 and Add_Line
**	0x5A7690 (splits a text through the 0x4FA8F0 wrapper, '...' where it was split) follow slot +0x74.
**	Not written yet (declared only): 0x5A8350, the mission messages (FAIL_IFs on lines 658 / 661 / 671 / 735).
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
