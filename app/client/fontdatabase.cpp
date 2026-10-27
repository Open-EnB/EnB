// Earth & Beyond client (reconstruction): app\Client\fontdatabase.cpp. Notes at the end of the file.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fontdatabase.h"
#include "..\shared\unknown_debug.h"
#include "..\..\wwlib\ini.h"
#include "..\..\wwlib\inisup.h"

class WW3D  // NAME 5: WW3D2's class (ww3d.h), declared here for the one call
{
public:
	static void Get_Device_Resolution(int &set_w, int &set_h, int &get_bits, bool &get_windowed);	// 0x9178F0
};


inline void FontDatabaseClass::Load()
{
	Clear_Infos();
	Clear_Tables();
	Unknown_6BF910();
	Unknown_6BFD00("fontcode.ini");
	Unknown_6BFD00("fontart.ini");
	Clear_Strings();
	Unknown_6C2080();
}

FontDatabaseClass::FontDatabaseClass()
{
	Load();
}


FontDatabaseClass::~FontDatabaseClass()
{
	Clear_Infos();
	Clear_Tables();
	Clear_Strings();
	for (int i = 0; i < MAXFONTS; i++) {
		Font[i].font32Ptr = NULL;
	}
}


void FontDatabaseClass::Load_Init(INIClass &ini, char *section, FontInitStruct *info)
{
	int index = 0;
	int misses = 10;
	for (; misses > 0; index++) {
		char entry[64];
		sprintf(entry, "Template%d", index);
		if (ini.Find_Entry(section, entry)->Is_Valid()) {
			StringClass name;
			ini.Get_String(name, section, entry, "");
			Load_Init(ini, name.Get_Buffer(name.Get_Length()), info);
			misses = 10;
		} else {
			misses--;
		}
	}

	info->ResWidth = ini.Get_Int(section, "ResWidth", 0);
	info->ResHeight = ini.Get_Int(section, "ResHeight", 0);
	char number[128];
	itoa(info->ResWidth, number, 10);
	info->Name = number;
	info->Name += 'x';
	itoa(info->ResHeight, number, 10);
	info->Name += number;
	info->Name += ':';

	for (int i = 0; i < MAXFONTS; i++) {
		char entry[64];
		sprintf(entry, "Font%d", i);
		if (ini.Find_Entry(section, entry)->Is_Valid()) {
			ini.Get_String(info->Font[i], section, entry, "");
		}
		sprintf(entry, "FontID%d", i);
		if (ini.Find_Entry(section, entry)->Is_Valid()) {
			ini.Get_String(info->FontID[i], section, entry, "");
		}
		sprintf(entry, "Scale%d", i);
		if (ini.Find_Entry(section, entry)->Is_Valid()) {
			info->Scale[i] = ini.Get_Float(section, entry, 0);
		}
		sprintf(entry, "Spacing%d", i);
		if (ini.Find_Entry(section, entry)->Is_Valid()) {
			info->Spacing[i] = ini.Get_Int(section, entry, 0);
		}
		info->Unknown98[i] = 0;
		info->DefaultFont[i] = info->Font[i];
		info->DefaultScale[i] = info->Scale[i];
		info->DefaultSpacing[i] = info->Spacing[i];
		info->DefaultUnknown98[i] = info->Unknown98[i];
	}
}


void FontDatabaseClass::Unknown_6BF910()
{
	StringClass unused;
	INIClass ini;
	ini.Load("fontsys.ini");
	char key[12];
	char value[64];
	char section[64];

	int index = 0;
	while (true) {
		sprintf(key, "%d", index);
		if (ini.Get_String("ALL_FONTS", key, "", value, sizeof(value)) == 0) break;
		AllFonts.push_back(std::string(value));
		index++;
	}

	int misses = 10;
	for (index = 0; misses > 0; index++) {
		sprintf(section, "INIT-%d", index);
		if (ini.Find_Section(section)->Is_Valid()) {
			FontInitStruct *info = new FontInitStruct;
			Load_Init(ini, section, info);
			Infos.push_back(info);
			misses = 10;
		} else {
			misses--;
		}
	}
}


void FontDatabaseClass::Clear_Infos()
{
	for (std::vector<FontInitStruct *>::iterator it = Infos.begin(); it != Infos.end(); it++) {
		delete *it;
	}
	Infos.clear();
}


void FontDatabaseClass::Clear_Tables()
{
	for (std::hash_map<std::string, FontColorStruct *>::iterator color = Colors.begin(); color != Colors.end(); ++color) {
		delete (*color).second;
	}
	Colors.clear();
	for (std::hash_map<std::string, FontEffectStruct *>::iterator effect = Effects.begin(); effect != Effects.end(); ++effect) {
		delete (*effect).second;
	}
	Effects.clear();
}


void FontDatabaseClass::Clear_Strings()
{
	for (std::hash_map<std::string, FontStringStruct *>::iterator str = Strings.begin(); str != Strings.end(); ++str) {
		delete (*str).second;
	}
	Strings.clear();
}


FontStruct *FontDatabaseClass::Find_Font(const StringClass &name, float &scale)
{
	for (int i = 0; i < MAXFONTS; i++) {
		if (strcmp(name, Font[i].Name) == 0 && Font[i].font32Ptr) {
			scale = Font[i].Scale;
			return &Font[i];
		}
	}
	scale = 1.0f;
	return &Font[0];
}


FontInitStruct *FontDatabaseClass::Get_Info(int width, int height)
{
	FontInitStruct *info = NULL;
	for (std::vector<FontInitStruct *>::iterator it = Infos.begin(); it != Infos.end(); it++) {
		if (width < (*it)->ResWidth || height < (*it)->ResHeight) break;
		info = *it;
	}
	return info;
}


// NOT EXACT: inline-budget: the client calls the table's find out of line and inlines the key string, ours the reverse (H6's parked pattern)
FontColorStruct *FontDatabaseClass::Find_Color(const char *name)
{
	std::hash_map<std::string, FontColorStruct *>::iterator it = Colors.find(name);
	if (it == Colors.end()) return NULL;
	return (*it).second;
}


// NOT EXACT: inline-budget: as Find_Color
FontEffectStruct *FontDatabaseClass::Find_Effect(const char *name)
{
	std::hash_map<std::string, FontEffectStruct *>::iterator it = Effects.find(std::string(name));
	if (it == Effects.end()) return NULL;
	return (*it).second;
}


// NOT EXACT: inline-budget: as Find_Color
FontStringStruct *FontDatabaseClass::Find_String(const char *name)
{
	std::hash_map<std::string, FontStringStruct *>::iterator it = Strings.find(name);
	return it == Strings.end() ? NULL : (*it).second;
}

























































































































































































































































































































































































































































































































































































































































































// NOT EXACT: register-only: the client copies the found node into edi before the key's destructor; the dead size() keeps find_or_insert out of line
void FontDatabaseClass::Add_String(FontStringStruct *str)
{
	if (str && str->Name) {
		std::string name(str->Name);
		if (Find_String(name.c_str()) != NULL) {
			G_ASSERT(0);
		} else {
			Strings[name] = str;
			Strings.size();
		}
	}
}

// NOT EXACT: register-only: as Add_String
void FontDatabaseClass::Add_Effect(FontEffectStruct *effect)
{
	if (effect && effect->Name) {
		std::string name(effect->Name);
		if (Find_Effect(name.c_str()) != NULL) {
			G_ASSERT(0);
		} else {
			Effects[name] = effect;
			Effects.size();
		}
	}
}

// NOT EXACT: register-only: as Add_String
void FontDatabaseClass::Add_Color(FontColorStruct *color)
{
	if (color && color->Name) {
		std::string name(color->Name);
		if (Find_Color(name.c_str()) != NULL) {
			G_ASSERT(0);
		} else {
			Colors[name] = color;
			Colors.size();
		}
	}
}


int FontDatabaseClass::Find_Font_File_Index(const char *name)
{
	int index = 0;
	for (std::vector<std::string>::iterator it = AllFonts.begin(); it != AllFonts.end(); it++) {
		if (*it == name) {
			return index;
		}
		index++;
	}
	return -1;
}



























































































int FontDatabaseClass::Get_Font_File_Index(const char *chat_name)
{
	FontInitStruct *info = Get_Info();
	FAIL_IF(!info) return -1;
	unsigned int chat_index;
	for (chat_index = 0; (int)chat_index < MAXFONTS; chat_index++) if (strcmp(info->FontID[chat_index], chat_name) == 0) break;
	FAIL_IF(chat_index >= MAXFONTS) return -1;
	return Find_Font_File_Index(info->Font[chat_index]);
}




const char *FontDatabaseClass::Get_Font_File_Name(unsigned int index)
{
	FAIL_IF(index >= AllFonts.size()) return NULL;
	return AllFonts[index].c_str();
}



float FontDatabaseClass::Get_Scale(const char *chat_name)
{
	FontInitStruct *info = Get_Info();
	FAIL_IF(!info) return -1.0f;
	unsigned int chat_index;
	for (chat_index = 0; (int)chat_index < MAXFONTS; chat_index++) if (strcmp(info->FontID[chat_index], chat_name) == 0) break;
	FAIL_IF(chat_index >= MAXFONTS) return -1.0f;
	return info->Scale[chat_index];
}





int FontDatabaseClass::Get_Spacing(const char *chat_name)
{
	FontInitStruct *info = Get_Info();
	FAIL_IF(!info) return -1;
	unsigned int chat_index;
	for (chat_index = 0; (int)chat_index < MAXFONTS; chat_index++) if (strcmp(info->FontID[chat_index], chat_name) == 0) break;
	FAIL_IF(chat_index >= MAXFONTS) return -1;
	return info->Spacing[chat_index];
}




int FontDatabaseClass::Get_Unknown98(const char *chat_name)
{
	FontInitStruct *info = Get_Info();
	FAIL_IF(!info) return -1;
	unsigned int chat_index;
	for (chat_index = 0; (int)chat_index < MAXFONTS; chat_index++) if (strcmp(info->FontID[chat_index], chat_name) == 0) break;
	FAIL_IF(chat_index >= MAXFONTS) return -1;
	return info->Unknown98[chat_index];
}


FontInitStruct *FontDatabaseClass::Get_Info()
{
	int width = 0;
	int height = 0;
	int bits = 0;
	bool windowed = false;
	WW3D::Get_Device_Resolution(width, height, bits, windowed);
	FontInitStruct *info = NULL;
	for (std::vector<FontInitStruct *>::iterator it = Infos.begin(); it != Infos.end(); it++) {
		if (width < (*it)->ResWidth || height < (*it)->ResHeight) break;
		info = *it;
	}
	return info;
}

























void FontDatabaseClass::Unknown_6C49C0()
{
	FontInitStruct *info = Get_Info();
	FAIL_IF(!info) return;
	info->Unknown_6C57B0(this);
}


int FontInitStruct::Find_Chat_Index(const char *chat_name)
{
	for (int i = 0; i < MAXFONTS; i++) {
		if (strcmp(FontID[i], chat_name) == 0) return i;
	}
	return -1;
}

/*
**	Reconstruction notes (fontdatabase.cpp)
**
**	The client's object runs 0x6BDA40-0x6C9E30 (between <string>'s guarded statics 0x6BD9E0 and 0x6C9E30):
**	the INIT / template structures' constructors, FontDatabaseClass (0xC8 bytes, created by 0x526DE0 into
**	0xBE67B8: constructor 0x6BE5C0 loads fontsys.ini, fontcode.ini and fontart.ini; destructor 0x6BEBF0), the
**	0x18-byte string table at 0xBE67BC (constructor 0x6C4C80, the ini loaders 0x6C52B0 / 0x6C55B0) and the SGI
**	hash_map instances (libs/sgistl, D87; the bucket functions over the string hash 0x42DB00 at 0x6C7D80 /
**	0x6C7F00 / 0x6C8080 / 0x6C8200).
**
**	Written so far: the constructor 0x6BE5C0 (through an inline Load: Clear_Infos, Clear_Tables, the three ini
**	loaders, Clear_Strings, 0x6C2080), FontStruct's inline constructor 0x6BE7C0 and implicit destructor
**	0x6BEB60, the destructor 0x6BEBF0, Load_Init 0x6BF330 (an INIT section, its Template%d sections first,
**	recursively), 0x6BF910 (fontsys.ini: ALL_FONTS into AllFonts, the INIT-n sections into Infos, with
**	FontInitStruct's inline constructor), Clear_Infos 0x6BEE60 (with FontInitStruct's implicit destructor 0x6BEEF0), Clear_Tables
**	0x6BEFD0 (Colors and Effects), Clear_Strings 0x6BF1F0, Find_Font 0x6C2BF0, Get_Info(width, height) 0x6C2CB0, the tables' Find_Color /
**	Find_Effect / Find_String 0x6C2D00-0x6C2F20 and Add_String / Add_Effect / Add_Color 0x6C3380-0x6C3720
**	(G_ASSERT(0) on the client's lines 881 / 895 / 909, D25; the six NOT EXACT), the accessors on lines
**	1023-1121 (FAIL_IF): Find_Font_File_Index 0x6C3FB0 (the client's name, its assert on line 1305), the chat
**	font's file index 0x6C4070, the font file name 0x6C4270 (AllFonts), the chat font's scale / spacing /
**	+0x98 value 0x6C42E0 / 0x6C4450 / 0x6C45B0, Get_Info 0x6C4720 (the INIT section for the current
**	resolution: the last whose ResWidth / ResHeight fit), 0x6C49C0, and FontInitStruct::Find_Chat_Index
**	0x6C5730. Every other function of the file is not written yet.
**
**	Tables (the Add functions' offsets and instances): +0x10 Colors (find 0x6C65A0, find_or_insert 0x6C87A0),
**	+0x28 Effects (0x6C68B0, 0x6C8950), +0x40 Strings (0x6C6BC0, 0x6C8B80); 0x6BFD00 adds the COLOR-n
**	sections through line 909 first, then the EFFECT-n ones through line 895.
**
**	Forms that mattered: chat_index unsigned with the search loop's bound signed (`jl` in the loop, `jb` in
**	the FAIL_IF); the ALL_FONTS loop as while (true) (a for loop is rotated), the section tests with the
**	found branch first and the ini loader's buffers at function scope (else they share slots); the table loops step with ++it (it++ spends the budget that inlines the vector's size()
**	in operator++); Find_Font_File_Index's counter stepped in the body after the test, not in the for header
**	(the latter keeps it in another register); in the Add functions a dead size() after the insert keeps
**	find_or_insert out of line as in the client (D40's lever), and the found test first (the assert falls
**	through, the insert follows).
*/
