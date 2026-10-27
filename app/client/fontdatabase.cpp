// Earth & Beyond client (reconstruction): app\Client\fontdatabase.cpp. Notes at the end of the file.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <algorithm>
#include "fontdatabase.h"
#include "..\shared\unknown_debug.h"
#include "..\..\wwlib\ini.h"
#include "..\..\wwlib\inisup.h"

class WW3D  // NAME 5: WW3D2's class (ww3d.h), declared here for the one call
{
public:
	static void Get_Device_Resolution(int &set_w, int &set_h, int &get_bits, bool &get_windowed);	// 0x9178F0
};


FontStringStruct::FontStringStruct()
{
	Reset();
}


FontStringStruct::FontStringStruct(const char *string_id, const char *text)
{
	Reset();
	StringID = string_id;
	Text = text;
}


// NOT EXACT: inline-budget: the client inlines the first nine string assignments with Store_Length called and calls operator= in the loop; ours inlines all
void FontStringStruct::Reset()
{
	StringID = "NewStringInfo";
	Unknown04 = "";
	Text = "";
	RenderObj = "";
	Attach = "";
	FontType[0] = "space6";
	FontType[1] = "space6";
	BlendType[0] = 0;
	BlendType[1] = 0;
	Color[0] = "WHITE";
	Color[1] = "BLACK";
	Justify[0] = 1;
	Justify[1] = 3;
	Offset[0] = 0;
	Offset[1] = 0;
	Target[0] = 0;
	Target[1] = 0;
	BoxOffset[0] = 0;
	BoxOffset[1] = 0;
	Width = 0;
	Wait = 0;
	Spacing = 1.0f;
	WrapWidth = 0;
	TimeSpacing = 0;
	TriggerTime = 1.0f;
	Rotate90 = false;
	Underline = false;
	SoundOnVol = 1.0f;
	SoundOnDur = -1.0f;
	SoundOverVol = 1.0f;
	SoundOverDur = -1.0f;
	SoundClickVol = 1.0f;
	SoundClickDur = -1.0f;
	for (int i = 0; i < 4; i++) {
		EffectOn[i] = "";
		EffectConst[i] = "";
		EffectOff[i] = "";
		MouseOver[i] = "";
		MouseClick[i] = "";
		EffectTriggered[i] = "";
	}
}


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


void FontDatabaseClass::Clear_Strings()
{
	for (std::hash_map<std::string, FontStringStruct *>::iterator str = Strings.begin(); str != Strings.end(); ++str) {
		delete (*str).second;
	}
	Strings.clear();
}

struct FontColorStruct  // NAME 2: one COLOR-n section of fontcode.ini / fontart.ini (0x10 bytes)
{
	FontColorStruct() { Name = ""; RGB.X = RGB.Y = RGB.Z = 0; }

	StringClass Name;														// +0x00, ColorID (the table's key; the destructor 0x6BF1B0 is its)  // NAME 2: the ini key
	TPoint3D<int> RGB;														// +0x04, RGB  // NAME 2: the ini key
};

struct FontEffectStruct  // NAME 2: one EFFECT-n section of fontcode.ini / fontart.ini (0x24 bytes)
{
	FontEffectStruct()
	{
		Name = "";
		EffectType = 0;
		Initial = 0;
		Intensity = 1.0f;
		Speed = 1.0f;
		Time = 0;
		Direction = 0;
		Offset.X = 0;
		Offset.Y = 0;
	}

	StringClass Name;														// +0x00, EffectID (the table's key; the destructor 0x6BF1D0 is its)  // NAME 2: the ini key
	int EffectType;														// +0x04, EffectType through 0x6C25A0  // NAME 2: the ini key
	float Initial;															// +0x08, Initial  // NAME 2: the ini key
	float Intensity;														// +0x0C, Intensity  // NAME 2: the ini key
	float Speed;															// +0x10, Speed  // NAME 2: the ini key
	float Time;															// +0x14, Time  // NAME 2: the ini key
	int Direction;															// +0x18, Direction through 0x6C2460  // NAME 2: the ini key
	TPoint2D<float> Offset;													// +0x1C, Offset  // NAME 2: the ini key
};


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


void FontDatabaseClass::Unknown_6BFD00(const char *ini_name)
{
	StringClass type;
	INIClass ini;
	ini.Load(ini_name);
	char section[64];

	int misses = 10;
	int index = 0;
	for (; misses > 0; index++) {
		sprintf(section, "COLOR-%d", index);
		TPoint3D<int> black(0, 0, 0);
		if (ini.Find_Section(section)->Is_Valid()) {
			FontColorStruct *color = new FontColorStruct;
			ini.Get_String(color->Name, section, "ColorID", "");
			color->RGB = ini.Get_Point(section, "RGB", black);
			Add_Color(color);
			misses = 10;
		} else {
			misses--;
		}
	}

	misses = 10;
	index = 0;
	for (; misses > 0; index++) {
		sprintf(section, "EFFECT-%d", index);
		TPoint2D<float> origin(0, 0);
		if (ini.Find_Section(section)->Is_Valid()) {
			FontEffectStruct *effect = new FontEffectStruct;
			ini.Get_String(effect->Name, section, "EffectID", "");
			effect->Intensity = ini.Get_Float(section, "Intensity", 0);
			effect->Speed = ini.Get_Float(section, "Speed", 0);
			effect->Time = ini.Get_Float(section, "Time", 0);
			effect->Initial = ini.Get_Float(section, "Initial", 0);
			effect->Offset = ini.Get_Point(section, "Offset", origin);
			ini.Get_String(type, section, "EffectType", "");
			effect->EffectType = Get_Effect_Type(type);
			ini.Get_String(type, section, "Direction", "");
			effect->Direction = Get_Direction(type);
			Add_Effect(effect);
			misses = 10;
		} else {
			misses--;
		}
	}
}


void FontDatabaseClass::Reload()
{
	Clear_Infos();
	Clear_Tables();
	Unknown_6BF910();
	Unknown_6BFD00("fontcode.ini");
	Unknown_6BFD00("fontart.ini");
	Clear_Strings();
}


void FontDatabaseClass::Unknown_6C2080()
{
	int width = 0;
	int height = 0;
	int bits = 0;
	bool windowed = false;
	WW3D::Get_Device_Resolution(width, height, bits, windowed);
	for (int i = 0; i < MAXFONTS; i++) {
		Font[i].font32Ptr = NULL;
	}
	FontInitStruct *info = Get_Info(width, height);
	for (i = 0; i < MAXFONTS; i++) {
		Font[i].Name = info->FontID[i];
		Font[i].Scale = info->Scale[i];
	}
	for (i = 0; i < MAXFONTS; i++) {
		if (info->Font[i].Get_Length() > 0) {
			Font[i].font32Ptr = Create_NEW(new Font32Class(info->Font[i], info->Spacing[i], info->Scale[i]));
		}
	}
}


int FontDatabaseClass::Get_Blend_Type(StringClass &name)
{
	char *text = name.Get_Buffer(name.Get_Length());
	if (stricmp(text, "SCREEN") == 0) return 1;
	if (stricmp(text, "ADDITIVE") == 0) return 2;
	if (stricmp(text, "MULTIPLY") == 0) return 3;
	if (stricmp(text, "INVERSE") == 0) return 4;
	if (stricmp(text, "OPAQUE") == 0) return 5;
	if (stricmp(text, "ALPHA_TEST") == 0) return 6;
	return 0;
}


int FontDatabaseClass::Get_Direction(StringClass &name)
{
	char *text = name.Get_Buffer(name.Get_Length());
	if (stricmp(text, "UP") == 0) return 1;
	if (stricmp(text, "DOWN") == 0) return 2;
	if (stricmp(text, "LEFT") == 0) return 3;
	if (stricmp(text, "RIGHT") == 0) return 4;
	if (stricmp(text, "TOP") == 0) return 1;
	if (stricmp(text, "BOTTOM") == 0) return 2;
	if (stricmp(text, "RANDOM") == 0) return 5;
	return 0;
}


int FontDatabaseClass::Get_Effect_Type(StringClass &name)
{
	char *text = name.Get_Buffer(name.Get_Length());
	if (stricmp(text, "PULSE") == 0) return 1;
	if (stricmp(text, "BLINK") == 0) return 2;
	if (stricmp(text, "ROTATE") == 0) return 3;
	if (stricmp(text, "FADE") == 0) return 4;
	if (stricmp(text, "LETTER") == 0) return 6;
	if (stricmp(text, "RANDLETTER") == 0) return 5;
	if (stricmp(text, "SCROLL") == 0) return 7;
	if (stricmp(text, "MOVE") == 0) return 8;
	if (stricmp(text, "SCALE") == 0) return 9;
	if (stricmp(text, "WIPE") == 0) return 10;
	if (stricmp(text, "GLOW") == 0) return 11;
	if (stricmp(text, "DROP") == 0) return 12;
	if (stricmp(text, "STAY") == 0) return 13;
	if (stricmp(text, "SHAKE") == 0) return 14;
	if (stricmp(text, "TOGGLE") == 0) return 15;
	if (stricmp(text, "REVERT") == 0) return 16;
	if (stricmp(text, "SCROLL_REVERT") == 0) return 17;
	if (stricmp(text, "CYCLE") == 0) return 18;
	if (stricmp(text, "SWITCH") == 0) return 19;
	if (stricmp(text, "STRETCH") == 0) return 20;
	return 0;
}


// NOT EXACT: order-only: the client reads the first character into bl before the parts' constructor and saves ebp only around the loop (shrink-wrapped); ours in the prologue
void FontDatabaseClass::Parse_Directions(const char *text, int *values)
{
	StringClass parts[2];
	StringClass *part = parts;
	for (; *text != 0; text++) {
		if (*text == ',') {
			part++;
		} else {
			*part += *text;
		}
	}
	for (int i = 0; i < 2; i++) {
		values[i] = Get_Direction(parts[i]);
	}
}


// NOT EXACT: order-only: as Parse_Directions
void FontDatabaseClass::Split_Four(const char *text, StringClass *values)
{
	StringClass parts[4];
	StringClass *part = parts;
	for (; *text != 0; text++) {
		if (*text == ',') {
			part++;
		} else {
			*part += *text;
		}
	}
	for (int i = 0; i < 4; i++) {
		values[i] = parts[i];
	}
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
	if (str && str->StringID) {
		std::string name(str->StringID);
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

// NOT EXACT: inline-budget: the client calls Get_Info(width, height) 0x6C2CB0 out of line, ours inlines it (the rest is the same)
void FontDatabaseClass::Set_Font(const char *chat_name, unsigned int font_index, float scale, int spacing, int value98, bool force)
{
	int width = 0; int height = 0; int bits = 0; bool windowed = false;
	WW3D::Get_Device_Resolution(width, height, bits, windowed);
	FontInitStruct *info = Get_Info(width, height);
	FAIL_IF(!info) return;
	unsigned int chat_index;
	for (chat_index = 0; (int)chat_index < MAXFONTS; chat_index++) if (strcmp(info->FontID[chat_index], chat_name) == 0) break;
	FAIL_IF(chat_index >= MAXFONTS) return;

	FAIL_IF(!Font[chat_index].font32Ptr) return;
	bool changed = force;
	if (font_index < AllFonts.size() && strcmp(AllFonts[font_index].c_str(), info->Font[chat_index]) != 0) {
		info->Font[chat_index] = AllFonts[font_index].c_str();
		changed = true;
	}
	if (scale != 16777215.0f && info->Scale[chat_index] != scale) {
		changed = true;
		info->Scale[chat_index] = scale;
		Font[chat_index].Scale = scale;
	}
	if (spacing != 0xFFFFFF && info->Spacing[chat_index] != spacing) {
		info->Spacing[chat_index] = spacing;
		Font[chat_index].font32Ptr->Set_Spacing(spacing);
		Font[chat_index].font32Ptr->Set_Spacing(spacing);
		changed = true;
	}
	if (value98 != 0xFFFFFF && info->Unknown98[chat_index] != value98) {
		info->Unknown98[chat_index] = value98;
	}
	if (changed) {
		Font[chat_index].font32Ptr = Create_NEW(new Font32Class(info->Font[chat_index], info->Spacing[chat_index], info->Scale[chat_index]));
	} else {
		Font[chat_index].font32Ptr->Set_Spacing(info->Spacing[chat_index]);
	}
	info->Unknown98[chat_index] = Font[chat_index].font32Ptr->Unknown_1C(info->Unknown98[chat_index]);
}




























void FontDatabaseClass::Restore_Font(const char *chat_name)
{
	FontInitStruct *info = Get_Info();
	FAIL_IF(!info) return;
	unsigned int chat_index;
	for (chat_index = 0; (int)chat_index < MAXFONTS; chat_index++) if (strcmp(info->FontID[chat_index], chat_name) == 0) break;
	FAIL_IF(chat_index >= MAXFONTS) return;

	FAIL_IF(!Font[chat_index].font32Ptr) return;
	info->Font[chat_index] = info->DefaultFont[chat_index];
	Font[chat_index].Scale = info->DefaultScale[chat_index];
	info->Scale[chat_index] = info->DefaultScale[chat_index];
	info->Spacing[chat_index] = info->DefaultSpacing[chat_index];
	info->Unknown98[chat_index] = info->DefaultUnknown98[chat_index];
	Set_Font(chat_name, 0xFFFFFF, 16777215.0f, 0xFFFFFF, 0xFFFFFF, true);
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
	int width = 0; int height = 0; int bits = 0; bool windowed = false;
	WW3D::Get_Device_Resolution(width, height, bits, windowed);
	FontInitStruct *info = NULL;
	for (std::vector<FontInitStruct *>::iterator it = Infos.begin(); it != Infos.end(); it++) {
		if (width < (*it)->ResWidth || height < (*it)->ResHeight) break;
		info = *it;
	}
	return info;
}



void FontDatabaseClass::Unknown_6C47B0()
{
	FontInitStruct *info = Get_Info();
	FAIL_IF(!info) return;
	for (int i = 0; i < MAXFONTS; i++) {
		if (info->Unknown_6C5C60(i, this)) {
			Font[i].Scale = info->Scale[i];
			int index = Find_Font_File_Index(info->Font[i]);
			Set_Font(info->FontID[i], index, info->Scale[i], info->Spacing[i], info->Unknown98[i], true);
		}
	}
}













void FontDatabaseClass::Unknown_6C49C0()
{
	FontInitStruct *info = Get_Info();
	FAIL_IF(!info) return;
	info->Unknown_6C57B0(this);
}


std::string FontStringFilesClass::Lower(const char *name)
{
	std::string lower(name);
	std::transform(lower.begin(), lower.end(), lower.begin(), tolower);
	return lower;
}


FontStringFilesClass::FontStringFilesClass()
{
	Load("fc_default.ini");
}


// NOT EXACT: inline-budget: as FontDatabaseClass::Find_Color (the client calls find out of line)
FontStringFileStruct *FontStringFilesClass::Find(const char *name)
{
	std::hash_map<std::string, FontStringFileStruct *>::iterator it = Files.find(std::string(name));
	if (it == Files.end()) return NULL;
	return (*it).second;
}


void FontStringFilesClass::Unload(const char *name)
{
	if (name == NULL) return;
	std::string key = Lower(name);
	std::hash_map<std::string, FontStringFileStruct *>::iterator it = Files.find(key);
	if (it == Files.end()) return;
	FontStringFileStruct *file = (*it).second;
	if (--file->Count == 0) {
		delete file;
		Files.erase(key);
		Unknown_BE67B8->Unknown_6C1AA0(key.c_str());
	}
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
**	Written so far: FontStringStruct's constructors 0x6BDA40 / 0x6BDD30 and Reset 0x6BE0E0 (NOT EXACT; the
**	struct's implicit destructor is the client's folded copy 0x67F490 in chat_window.cpp), the constructor 0x6BE5C0 (through an inline Load: Clear_Infos, Clear_Tables, the three ini
**	loaders, Clear_Strings, 0x6C2080), FontStruct's inline constructor 0x6BE7C0 and implicit destructor
**	0x6BEB60, the destructor 0x6BEBF0, Load_Init 0x6BF330 (an INIT section, its Template%d sections first,
**	recursively), 0x6BF910 (fontsys.ini: ALL_FONTS into AllFonts, the INIT-n sections into Infos, with
**	FontInitStruct's inline constructor), Reload 0x6C1EC0 (the ini files again; the
**	constructor's inline Load is the same plus 0x6C2080: written as one member the constructor inlines, it moved
**	Load_Init's registers), 0x6C2080 (the eight fonts for the resolution: Font32Class through RefCountPtr's Create_NEW), Get_Blend_Type / Get_Direction / Get_Effect_Type 0x6C2340 / 0x6C2460 / 0x6C25A0 (the ini words to
**	their values), Parse_Directions 0x6C2890 and Split_Four 0x6C2A10 (NOT EXACT), 0x6BFD00 (fontcode.ini / fontart.ini: the COLOR-n and EFFECT-n
**	sections through Add_Color / Add_Effect, inlined with the asserts on lines 909 / 895), Clear_Infos 0x6BEE60 (with FontInitStruct's implicit destructor 0x6BEEF0), Clear_Tables
**	0x6BEFD0 (Colors and Effects), Clear_Strings 0x6BF1F0, Find_Font 0x6C2BF0, Get_Info(width, height) 0x6C2CB0, the tables' Find_Color /
**	Find_Effect / Find_String 0x6C2D00-0x6C2F20 and Add_String / Add_Effect / Add_Color 0x6C3380-0x6C3720
**	(G_ASSERT(0) on the client's lines 881 / 895 / 909, D25; the six NOT EXACT), the accessors on lines
**	1023-1121 (FAIL_IF): Set_Font 0x6C38F0 (lines 923-928, NOT EXACT) and Restore_Font 0x6C3D40 (lines 986-991), Find_Font_File_Index 0x6C3FB0 (the client's name, its assert on line 1305), the chat
**	font's file index 0x6C4070, the font file name 0x6C4270 (AllFonts), the chat font's scale / spacing /
**	+0x98 value 0x6C42E0 / 0x6C4450 / 0x6C45B0, Get_Info 0x6C4720 (the INIT section for the current
**	resolution: the last whose ResWidth / ResHeight fit), 0x6C47B0 (line 1096), 0x6C49C0, and FontInitStruct::Find_Chat_Index
**	0x6C5730; FontStringFilesClass (0xBE67BC: the string files in use, counted): Lower 0x6C4A80, the constructor
**	0x6C4C80, Find 0x6C51A0 (NOT EXACT) and Unload 0x6C55B0 (its destructor 0x6C4F50 and Load 0x6C52B0 declared only: written
**	from the pseudocode they missed and moved vector<std::string>::insert 0x432D50). Every other function of the
**	file is not written yet.
**
**	Tables (the Add functions' offsets and instances): +0x10 Colors (find 0x6C65A0, find_or_insert 0x6C87A0),
**	+0x28 Effects (0x6C68B0, 0x6C8950), +0x40 Strings (0x6C6BC0, 0x6C8B80); 0x6BFD00 adds the COLOR-n
**	sections through line 909 first, then the EFFECT-n ones through line 895.
**
**	Forms that mattered: chat_index unsigned with the search loop's bound signed (`jl` in the loop, `jb` in
**	the FAIL_IF); the ALL_FONTS loop as while (true) (a for loop is rotated), the section tests with the
**	found branch first and the ini loader's buffers at function scope (else they share slots); the table loops step with ++it (it++ spends the budget that inlines the vector's size()
**	in operator++); FontColorStruct and FontEffectStruct (TPoint3D<int> / TPoint2D<float> members, assigned
**	from INIClass::Get_Point) are defined in this file after the INIT loaders, with Clear_Tables after them:
**	instantiating the TPoints in the header moves Load_Init's registers (the section pointer reloaded in its
**	font loop); Find_Font_File_Index's counter stepped in the body after the test, not in the for header
**	(the latter keeps it in another register); in the Add functions a dead size() after the insert keeps
**	find_or_insert out of line as in the client (D40's lever), and the found test first (the assert falls
**	through, the insert follows).
*/
