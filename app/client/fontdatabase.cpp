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

struct FontOptionStruct  // NAME 1 [FontOptionStruct]: neutral (D10): what option.cpp's 0x7F95E0 returns
{
	char Unknown00[0x10];											// +0x00  // NAME 1 [Unknown00]: neutral (D10)
	std::string Value;												// +0x10: a file name  // NAME 2: what it holds
};

class Unknown_BE8B38Class  // NAME 1 [Unknown_BE8B38Class]: neutral (D10): option.cpp's object (tsk_game.h declares the same class)
{
public:
	FontOptionStruct *Unknown_7F95E0(const char *name);				// 0x7F95E0  // NAME 1 [Unknown_7F95E0]: neutral (D10)
	void Unknown_7F8EF0();											// 0x7F8EF0: after the fonts are saved  // NAME 1 [Unknown_7F8EF0]: neutral (D10)
};
extern Unknown_BE8B38Class *Unknown_BE8B38;							// 0xBE8B38  // NAME 1 [Unknown_BE8B38]: neutral (D10)

class FontTextFileClass  // NAME 1 [FontTextFileClass]: neutral (D10): a text file read into lines (0x28 bytes)
{
public:
	FontTextFileClass(const char *file_name, float unknown, int unknown2);	// 0x4F9BF0  // NAME 1 [unknown]: neutral (D10)
	~FontTextFileClass();											// 0x4F9CC0
	unsigned int Count();											// 0x4FA710: the number of lines  // NAME 2: what it returns
	char *Get(unsigned int index);									// 0x4FA740: one line  // NAME 2: what it returns

private:
	char Unknown00[0x28];											// +0x00  // NAME 1 [Unknown00]: neutral (D10)
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


// The first nine assignments inline (Store_Length called), the sounds' and the loop's call operator= (HL2, 2026-10-29)
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
	// Each sound back to its defaults through FontSoundStruct's inline Reset: its ID = "" is the
	// client's out-of-line operator= at 0x6BE39B / 0x6BE3B5 / 0x6BE3CD (VC6's budget is spent by
	// then); the loop's 24 assignments are called too, from EffectConst's address (+0x9C).
	SoundOn.Reset();
	SoundOver.Reset();
	SoundClick.Reset();
	for (int i = 0; i < 4; i++) {
		EffectOn[i] = "";
		EffectConst[i] = "";
		EffectOff[i] = "";
		MouseOver[i] = "";
		MouseClick[i] = "";
		EffectTriggered[i] = "";
	}
}


void FontDatabaseClass::Unknown_6BE520()
{
	for (std::hash_map<std::string, FontStringStruct *>::iterator it = Strings.begin(); it != Strings.end(); ++it) {
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


// NOT EXACT: inline-budget: ours inlines Parse_Directions / Split_Four / Get_Blend_Type and calls Get_Length; the client the reverse
void FontDatabaseClass::Load_String(INIClass &ini, char *section, FontStringStruct *str)
{
	StringClass text;
	TPoint2D<float> origin(0, 0);
	char entry[64];

	int index = 0;
	int misses = 10;
	for (; misses > 0; index++) {
		sprintf(entry, "Template%d", index);
		if (ini.Find_Entry(section, entry)->Is_Valid()) {
			StringClass name;
			ini.Get_String(name, section, entry, "");
			Load_String(ini, name.Get_Buffer(name.Get_Length()), str);
			misses = 10;
		} else {
			misses--;
		}
	}

	if (ini.Find_Entry(section, "FontType")->Is_Valid()) {
		ini.Get_String(str->FontType[0], section, "FontType", "");
	}
	if (ini.Find_Entry(section, "AltFontType")->Is_Valid()) {
		ini.Get_String(str->FontType[1], section, "AltFontType", "");
	}
	if (ini.Find_Entry(section, "Color")->Is_Valid()) {
		ini.Get_String(str->Color[0], section, "Color", "");
	}
	if (ini.Find_Entry(section, "AltColor")->Is_Valid()) {
		ini.Get_String(str->Color[1], section, "AltColor", "");
	}
	if (ini.Find_Entry(section, "BlendType")->Is_Valid()) {
		ini.Get_String(text, section, "BlendType", "");
		str->BlendType[0] = Get_Blend_Type(text);
	}
	if (ini.Find_Entry(section, "AltBlendType")->Is_Valid()) {
		ini.Get_String(text, section, "AltBlendType", "");
		str->BlendType[1] = Get_Blend_Type(text);
	}
	if (ini.Find_Entry(section, "Justify")->Is_Valid()) {
		ini.Get_String(text, section, "Justify", "");
		Parse_Directions(text.Get_Buffer(text.Get_Length()), str->Justify);
	}
	if (ini.Find_Entry(section, "Wait")->Is_Valid()) {
		str->Wait = ini.Get_Float(section, "Wait", 0);
	}
	if (ini.Find_Entry(section, "Width")->Is_Valid()) {
		str->Width = ini.Get_Float(section, "Width", 0);
	}
	if (ini.Find_Entry(section, "Spacing")->Is_Valid()) {
		str->Spacing = ini.Get_Float(section, "Spacing", 0);
	}
	if (ini.Find_Entry(section, "WrapWidth")->Is_Valid()) {
		str->WrapWidth = ini.Get_Float(section, "WrapWidth", 0);
	}
	if (ini.Find_Entry(section, "TimeSpacing")->Is_Valid()) {
		str->TimeSpacing = ini.Get_Float(section, "TimeSpacing", 0);
	}
	if (ini.Find_Entry(section, "TriggerTime")->Is_Valid()) {
		str->TriggerTime = ini.Get_Float(section, "TriggerTime", 1.0f);
	}
	if (ini.Find_Entry(section, "Offset")->Is_Valid()) {
		const TPoint2D<float> &offset = ini.Get_Point(section, "Offset", origin);
		str->Offset[0] = offset.X;
		str->Offset[1] = offset.Y;
	}
	if (ini.Find_Entry(section, "Target")->Is_Valid()) {
		const TPoint2D<float> &target = ini.Get_Point(section, "Target", origin);
		str->Target[0] = target.X;
		str->Target[1] = target.Y;
	}
	if (ini.Find_Entry(section, "BoxOffset")->Is_Valid()) {
		const TPoint2D<float> &box = ini.Get_Point(section, "BoxOffset", origin);
		str->BoxOffset[0] = box.X;
		str->BoxOffset[1] = box.Y;
	}
	if (ini.Find_Entry(section, "Rotate90")->Is_Valid()) {
		str->Rotate90 = ini.Get_Bool(section, "Rotate90", false);
	}
	if (ini.Find_Entry(section, "Underline")->Is_Valid()) {
		str->Underline = ini.Get_Bool(section, "Underline", false);
	}
	if (ini.Find_Entry(section, "SoundOnID")->Is_Valid()) {
		ini.Get_String(str->SoundOn.ID, section, "SoundOnID", "");
	}
	if (ini.Find_Entry(section, "SoundOnVol")->Is_Valid()) {
		str->SoundOn.Vol = ini.Get_Float(section, "SoundOnVol", 1.0f);
	}
	if (ini.Find_Entry(section, "SoundOnDur")->Is_Valid()) {
		str->SoundOn.Dur = ini.Get_Float(section, "SoundOnDur", -1.0f);
	}
	if (ini.Find_Entry(section, "SoundOverID")->Is_Valid()) {
		ini.Get_String(str->SoundOver.ID, section, "SoundOverID", "");
	}
	if (ini.Find_Entry(section, "SoundOverVol")->Is_Valid()) {
		str->SoundOver.Vol = ini.Get_Float(section, "SoundOverVol", 1.0f);
	}
	if (ini.Find_Entry(section, "SoundOverDur")->Is_Valid()) {
		str->SoundOver.Dur = ini.Get_Float(section, "SoundOverDur", -1.0f);
	}
	if (ini.Find_Entry(section, "SoundClickID")->Is_Valid()) {
		ini.Get_String(str->SoundClick.ID, section, "SoundClickID", "");
	}
	if (ini.Find_Entry(section, "SoundClickVol")->Is_Valid()) {
		str->SoundClick.Vol = ini.Get_Float(section, "SoundClickVol", 1.0f);
	}
	if (ini.Find_Entry(section, "SoundClickDur")->Is_Valid()) {
		str->SoundClick.Dur = ini.Get_Float(section, "SoundClickDur", -1.0f);
	}
	if (ini.Find_Entry(section, "EffectOn")->Is_Valid()) {
		ini.Get_String(text, section, "EffectOn", "");
		Split_Four(text.Get_Buffer(text.Get_Length()), str->EffectOn);
	}
	if (ini.Find_Entry(section, "EffectConst")->Is_Valid()) {
		ini.Get_String(text, section, "EffectConst", "");
		Split_Four(text.Get_Buffer(text.Get_Length()), str->EffectConst);
	}
	if (ini.Find_Entry(section, "EffectOff")->Is_Valid()) {
		ini.Get_String(text, section, "EffectOff", "");
		Split_Four(text.Get_Buffer(text.Get_Length()), str->EffectOff);
	}
	if (ini.Find_Entry(section, "MouseOver")->Is_Valid()) {
		ini.Get_String(text, section, "MouseOver", "");
		Split_Four(text.Get_Buffer(text.Get_Length()), str->MouseOver);
	}
	if (ini.Find_Entry(section, "MouseClick")->Is_Valid()) {
		ini.Get_String(text, section, "MouseClick", "");
		Split_Four(text.Get_Buffer(text.Get_Length()), str->MouseClick);
	}
	if (ini.Find_Entry(section, "EffectTriggered")->Is_Valid()) {
		ini.Get_String(text, section, "EffectTriggered", "");
		Split_Four(text.Get_Buffer(text.Get_Length()), str->EffectTriggered);
	}
	if (ini.Find_Entry(section, "StringID")->Is_Valid()) {
		ini.Get_String(str->StringID, section, "StringID", "");
	}
	if (ini.Find_Entry(section, "RenderObj")->Is_Valid()) {
		ini.Get_String(str->RenderObj, section, "RenderObj", "");
	}
	if (ini.Find_Entry(section, "Attach")->Is_Valid()) {
		ini.Get_String(str->Attach, section, "Attach", "");
	}

	misses = 10;
	for (int line = 0; misses > 0; line++) {
		sprintf(entry, "String%d", line);
		if (ini.Find_Entry(section, entry)->Is_Valid()) {
			ini.Get_String(text, section, entry, "");
		} else {
			text = "";
		}
		if (text.Get_Length() == 0) {
			misses--;
		} else {
			if (line == 0) {
				str->Text = text;
			} else {
				str->Text += ' ';
				str->Text += text;
			}
			misses = 10;
		}
	}
}


// NOT EXACT: frame: the inlined Add_String's key and iterator temporaries sit in other stack slots (0.76)
void FontDatabaseClass::Unknown_6C1500(const char *file_name)
{
	INIClass ini;
	ini.Load(file_name);
	char section[64];

	int misses = 10;
	int index = 0;
	for (; misses > 0; index++) {
		sprintf(section, "STR-%d", index);
		if (ini.Find_Section(section)->Is_Valid()) {
			FontStringStruct *str = new FontStringStruct;
			Load_String(ini, section, str);
			Add_String(str);
			misses = 10;
		} else {
			misses--;
		}
	}

	misses = 10;
	index = 0;
	for (; misses > 0; index++) {
		FontStringStruct *str = NULL;
		sprintf(section, "WRAP-%d", index);
		if (ini.Find_Section(section)->Is_Valid()) {
			str = new FontStringStruct;
			Load_String(ini, section, str);
			Add_String(str);
			misses = 10;
		} else {
			misses--;
		}
	}
}


void FontDatabaseClass::Unknown_6C1AA0(const char *file_name)
{
	INIClass ini;
	ini.Load(file_name);
	char section[64];

	int misses = 10;
	int index = 0;
	for (; misses > 0; index++) {
		sprintf(section, "STR-%d", index);
		if (ini.Find_Section(section)->Is_Valid()) {
			StringClass id;
			ini.Get_String(id, section, "StringID", "");
			Remove_String(id);
			misses = 10;
		} else {
			misses--;
		}
	}

	misses = 10;
	index = 0;
	for (; misses > 0; index++) {
		sprintf(section, "WRAP-%d", index);
		if (ini.Find_Section(section)->Is_Valid()) {
			StringClass id;
			ini.Get_String(id, section, "StringID", "");
			Remove_String(id);
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


FontColorStruct *FontDatabaseClass::Find_Color(const char *name)
{
	std::hash_map<std::string, FontColorStruct *>::const_iterator it = Colors.find(std::string(name));
	return it == Colors.end() ? NULL : (*it).second;
}


FontEffectStruct *FontDatabaseClass::Find_Effect(const char *name)
{
	std::hash_map<std::string, FontEffectStruct *>::const_iterator it = Effects.find(std::string(name));
	return it == Effects.end() ? NULL : (*it).second;
}


FontStringStruct *FontDatabaseClass::Find_String(const char *name)
{
	std::hash_map<std::string, FontStringStruct *>::const_iterator it = Strings.find(std::string(name));
	return it == Strings.end() ? NULL : (*it).second;
}


// NOT EXACT: frame: the client keeps this in a stack slot (frame 0x1C, ours 0x18) around the inlined destructor
void FontDatabaseClass::Remove_String(const char *name)
{
	if (name != NULL) {
		std::hash_map<std::string, FontStringStruct *>::iterator it = Strings.find(std::string(name));
		if (it != Strings.end()) {
			delete (*it).second;
			Strings.erase(it);
		}
	}
}


















































































































void FontDatabaseClass::Add_String(FontStringStruct *str)
{
	if (str && str->StringID) {
		std::string name(str->StringID);
		if (Find_String(name.c_str()) != NULL) {
			G_ASSERT(0);
		} else {
			Strings[name] = str;
		}
	}
}



void FontDatabaseClass::Add_Effect(FontEffectStruct *effect)
{
	if (effect && effect->Name) {
		std::string name(effect->Name);
		if (Find_Effect(name.c_str()) != NULL) {
			G_ASSERT(0);
		} else {
			Effects[name] = effect;
		}
	}
}



void FontDatabaseClass::Add_Color(FontColorStruct *color)
{
	if (color && color->Name) {
		std::string name(color->Name);
		if (Find_Color(name.c_str()) != NULL) {
			G_ASSERT(0);
		} else {
			Colors[name] = color;
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
		if (info->Load_Font(i, this)) {
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
	info->Save_Fonts(this);
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


FontStringFileStruct *FontStringFilesClass::Find(const char *name)
{
	std::hash_map<std::string, FontStringFileStruct *>::const_iterator it = Files.find(std::string(name));
	return it == Files.end() ? NULL : (*it).second;
}

// NOT EXACT: frame: 0x50 vs 0x40, the inlined Find's key temporary has its own slot (the client's shares the operator[] pair's); 199 = 199 instructions
void FontStringFilesClass::Load(const char *name)
{
	if (name == NULL || *name == 0) return;
	std::string key = Lower(name);
	FontStringFileStruct *file = Find(key.c_str());
	if (file == NULL) {
		file = new FontStringFileStruct;
		file->Name = key;
		file->Count = 1;
		Files[file->Name.c_str()] = file;
		Unknown_BE67B8->Unknown_6C1500(file->Name.c_str());
	} else {
		file->Count++;
	}
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















































bool FontInitStruct::Save_Fonts(FontDatabaseClass *fdb)
{
	FAIL_IF(!fdb) return false;
	for (int i = 0; i < MAXFONTS; i++) {
		FontOptionStruct *option = Unknown_BE8B38->Unknown_7F95E0(FontID[i]);
		if (option != NULL) {
			std::string text(option->Value);
			int length = Name.length();
			FontTextFileClass file(text.c_str(), 1024.0f, 0);
			char buffer[1024];
			sprintf(buffer, "%1.1f,%d,%d", Scale[i], Spacing[i], Unknown98[i]);
			text = Name;
			text += Font[i];
			text += ':';
			text += buffer;
			text += "\\n";
			text.size();
			for (unsigned int j = 0; j < file.Count(); j++) {
				char *str = file.Get(j);









				FAIL_IF(!str) continue;
				if (strncmp(str, Name.c_str(), length) != 0) {
					text += str;
					text += "\\n";
				}
			}
			option->Value = text.c_str();
			text.size();
		}
	}
	Unknown_BE8B38->Unknown_7F8EF0();
	return true;
}

// NOT EXACT: register-only: the client zeroes scale / spacing / value98 from ebp (the loop counter's register), ours scale as an immediate and the rest from edi
bool FontInitStruct::Load_Font(unsigned int idx, FontDatabaseClass *fdb)
{
	FAIL_IF(idx >= MAXFONTS) return false;
	FAIL_IF(!fdb) return false;
	FontOptionStruct *option = Unknown_BE8B38->Unknown_7F95E0(FontID[idx]);
	if (option == NULL) return false;
	std::string file_name(option->Value);
	int length = Name.length();
	FontTextFileClass file(file_name.c_str(), 1024.0f, 0);
	for (unsigned int i = 0; i < file.Count(); i++) {
		char *str = file.Get(i);





		FAIL_IF(!str) return false;
		if (strncmp(str, Name.c_str(), length) == 0) {
			char *font_name = str + length;
			float scale = 0;
			int spacing = 0;
			int value98 = 0;
			char *data = strchr(font_name, ':');


			FAIL_IF(!data) return false;
			*data = 0;
			data++;

			FAIL_IF(fdb->Find_Font_File_Index(font_name) < 0) return false;
			int fields = sscanf(data, "%f,%d,%d", &scale, &spacing, &value98);

			FAIL_IF(fields != 3) return false;
			Font[idx] = font_name;
			Scale[idx] = scale;
			Spacing[idx] = spacing;
			Unknown98[idx] = value98;
			return true;
		}
	}
	return false;
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
**	struct's implicit destructor is the client's folded copy 0x67F490 in chat_window.cpp), 0x6BE520 (walks Strings, its body compiled out), the constructor 0x6BE5C0 (through an inline Load: Clear_Infos, Clear_Tables, the three ini
**	loaders, Clear_Strings, 0x6C2080), FontStruct's inline constructor 0x6BE7C0 and implicit destructor
**	0x6BEB60, the destructor 0x6BEBF0, Load_Init 0x6BF330 (an INIT section, its Template%d sections first,
**	recursively), 0x6BF910 (fontsys.ini: ALL_FONTS into AllFonts, the INIT-n sections into Infos, with
**	FontInitStruct's inline constructor), Load_String 0x6C0430 (a string template, its Template%d templates first, the String%d lines joined; NOT EXACT),
**	the string files' loader 0x6C1500 (NOT EXACT) and unloader 0x6C1AA0 (STR-n / WRAP-n sections), Remove_String
**	0x6C3030 (NOT EXACT), Reload 0x6C1EC0 (the ini files again; the
**	constructor's inline Load is the same plus 0x6C2080: written as one member the constructor inlines, it moved
**	Load_Init's registers), 0x6C2080 (the eight fonts for the resolution: Font32Class through RefCountPtr's Create_NEW), Get_Blend_Type / Get_Direction / Get_Effect_Type 0x6C2340 / 0x6C2460 / 0x6C25A0 (the ini words to
**	their values), Parse_Directions 0x6C2890 and Split_Four 0x6C2A10 (NOT EXACT), 0x6BFD00 (fontcode.ini / fontart.ini: the COLOR-n and EFFECT-n
**	sections through Add_Color / Add_Effect, inlined with the asserts on lines 909 / 895), Clear_Infos 0x6BEE60 (with FontInitStruct's implicit destructor 0x6BEEF0), Clear_Tables
**	0x6BEFD0 (Colors and Effects), Clear_Strings 0x6BF1F0, Find_Font 0x6C2BF0, Get_Info(width, height) 0x6C2CB0, the tables' Find_Color /
**	Find_Effect / Find_String 0x6C2D00-0x6C2F20 and Add_String / Add_Effect / Add_Color 0x6C3380-0x6C3720
**	(G_ASSERT(0) on the client's lines 881 / 895 / 909, D25), the accessors on lines
**	1023-1121 (FAIL_IF): Set_Font 0x6C38F0 (lines 923-928, NOT EXACT) and Restore_Font 0x6C3D40 (lines 986-991), Find_Font_File_Index 0x6C3FB0 (the client's name, its assert on line 1305), the chat
**	font's file index 0x6C4070, the font file name 0x6C4270 (AllFonts), the chat font's scale / spacing /
**	+0x98 value 0x6C42E0 / 0x6C4450 / 0x6C45B0, Get_Info 0x6C4720 (the INIT section for the current
**	resolution: the last whose ResWidth / ResHeight fit), 0x6C47B0 (line 1096), 0x6C49C0, and FontInitStruct::Find_Chat_Index
**	0x6C5730, Save_Fonts 0x6C57B0 (lines 1235 / 1261: the chat fonts' lines into their options' text, then
**	option.cpp's 0x7F8EF0; two dead text.size() place the budget as the client's) and Load_Font 0x6C5C60 (lines 1278-1308: the chat font's saved file through option.cpp's
**	0x7F95E0 and the text file reader 0x4F9BF0; NOT EXACT); FontStringFilesClass (0xBE67BC: the string files in use, counted): Lower 0x6C4A80, the constructor
**	0x6C4C80, Find 0x6C51A0, Load 0x6C52B0 (HL2: Find inlined, its find 0x6C62A0 and conversion 0x6C6370 called a level
**	deeper; NOT EXACT: frame) and Unload 0x6C55B0 (the destructor 0x6C4F50 declared only). Every other function of the
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
**	(the latter keeps it in another register); the table finds as `const_iterator it = Table.find(std::string(name));
**	return it == Table.end() ? NULL : (*it).second;` (the conversion copies the node into a register before the key's
**	destructor, and the client then calls find out of line and inlines the key: an iterator and two returns inline
**	find instead); the Add functions call them plainly (no dead reads).
*/
