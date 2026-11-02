// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Client\fontdatabase.h (name assumed from
**	fontdatabase.cpp, whose __FILE__ is 'e:\build\beta\G\app\Client\fontdatabase.cpp')
**
**	The font database: fontsys.ini's INIT-n sections (one per screen resolution, with the chat fonts'
**	ids, font files, scales and spacings), the font files the client knows (AllFonts) and the eight
**	fonts in use (Font[MAXFONTS]). The client's asserts name AllFonts, Font, font32Ptr, MAXFONTS,
**	Find_Font_File_Index and the locals info, chat_index, idx, fdb; the rest are D10 / D24 names.
*/



#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __FONTDATABASE_H  // NAME 3 [fontdatabase.h]: header name assumed from the client's fontdatabase.cpp
#define __FONTDATABASE_H

#include <string>
#include <vector>
#include <hash_map.h>
#include "..\..\wwlib\wwstring.h"
#include "..\shared\unknown_debug.h"
#include "..\..\wwlib\refcount.h"
#include "..\..\wwlib\ref_ptr.h"
#include "..\..\wwlib\point.h"

#define MAXFONTS 8

class FontDatabaseClass;
class INIClass;

struct FontInitStruct  // NAME 2: one INIT-n section of fontsys.ini (a screen resolution's fonts)
{
	FontInitStruct()
	{
		ResWidth = 0;
		ResHeight = 0;
		for (int i = 0; i < MAXFONTS; i++) {
			FontID[i] = "";
			Font[i] = "";
			Scale[i] = 1.0f;
			Spacing[i] = 0;
			Unknown98[i] = 0;
			DefaultFont[i] = "";
			DefaultScale[i] = 1.0f;
			DefaultSpacing[i] = 0;
			DefaultUnknown98[i] = 0;
		}
	}

	bool Load_Font(unsigned int idx, FontDatabaseClass *fdb);				// 0x6C5C60: the chat font's saved font file, scale, spacing  // NAME 2: what it does
	bool Save_Fonts(FontDatabaseClass *fdb);								// 0x6C57B0: the chat fonts into their options' text  // NAME 2: what it does
	int Find_Chat_Index(const char *chat_name);								// 0x6C5730  // NAME 2: the chat_index of the asserts

	int ResWidth;															// +0x00, ResWidth  // NAME 2: the ini key
	int ResHeight;															// +0x04, ResHeight  // NAME 2: the ini key
	std::string Name;														// +0x08, "WxH:"  // NAME 2: what it holds
	StringClass FontID[MAXFONTS];											// +0x18, FontID%d  // NAME 2: the ini key
	StringClass Font[MAXFONTS];												// +0x38, Font%d  // NAME 2: the ini key
	float Scale[MAXFONTS];													// +0x58, Scale%d  // NAME 2: the ini key
	int Spacing[MAXFONTS];													// +0x78, Spacing%d  // NAME 2: the ini key
	int Unknown98[MAXFONTS];												// +0x98  // NAME 1 [Unknown98]: neutral (D10)
	StringClass DefaultFont[MAXFONTS];										// +0xB8  // NAME 2: what 0x6C3D40 restores Font from
	float DefaultScale[MAXFONTS];											// +0xD8  // NAME 2: what 0x6C3D40 restores Scale from
	int DefaultSpacing[MAXFONTS];											// +0xF8  // NAME 2: what 0x6C3D40 restores Spacing from
	int DefaultUnknown98[MAXFONTS];											// +0x118  // NAME 1 [DefaultUnknown98]: neutral (D10)
};

#include "font32bit.h"												// Font32Class (font32bit.cpp, FNT1): what font32Ptr points at

























struct FontStruct  // NAME 2: one of the fonts in use (0xC bytes)
{
	FontStruct() { font32Ptr = NULL; Name = ""; Scale = 1.0f; }			// 0x6BE7C0 (destructor 0x6BEB60)

	RefCountPtr<Font32Class> font32Ptr;														// +0x00, the client's name (its assert)
	StringClass Name;														// +0x04  // NAME 2: what Find_Font compares
	float Scale;															// +0x08  // NAME 2: Scale%d is copied in
};

struct FontColorStruct;		// fontdatabase.cpp (its TPoint3D<int> member instantiated after the INIT loaders, see the notes)
struct FontEffectStruct;	// fontdatabase.cpp

struct FontSoundStruct  // NAME 2 [FontSoundStruct]: a string template's sound: the ini keys Sound*ID / Sound*Vol / Sound*Dur
{
	StringClass ID;														// +0x00  // NAME 2: the ini key
	float Vol;															// +0x04  // NAME 2: the ini key
	float Dur;															// +0x08  // NAME 2: the ini key
	void Reset() { ID = ""; Vol = 1.0f; Dur = -1.0f; }		// inlined in FontStringStruct::Reset 0x6BE0E0  // NAME 2: what it does
};

struct FontStringStruct  // NAME 2: one string template of a string file (Template%d / String%d keys; 0xEC bytes; "NewStringInfo" by default)
{
	FontStringStruct();													// 0x6BDA40
	FontStringStruct(const char *string_id, const char *text);				// 0x6BDD30
	FontStringStruct &operator=(const FontStringStruct &that)	// inlined in 0x6D0A70, which calls the sounds' 0x6D0F70 (the implicit one is not inlined in it)
	{
		StringID = that.StringID; Unknown04 = that.Unknown04; Text = that.Text; RenderObj = that.RenderObj; Attach = that.Attach;
		int i;
		for (i = 0; i < 2; i++) BlendType[i] = that.BlendType[i];
		for (i = 0; i < 2; i++) FontType[i] = that.FontType[i];
		for (i = 0; i < 2; i++) Color[i] = that.Color[i];
		for (i = 0; i < 2; i++) Justify[i] = that.Justify[i];
		Offset[0] = that.Offset[0]; Offset[1] = that.Offset[1]; Target[0] = that.Target[0]; Target[1] = that.Target[1];
		BoxOffset[0] = that.BoxOffset[0]; BoxOffset[1] = that.BoxOffset[1]; Width = that.Width; Wait = that.Wait;
		TimeSpacing = that.TimeSpacing; TriggerTime = that.TriggerTime; Spacing = that.Spacing; WrapWidth = that.WrapWidth;
		Rotate90 = that.Rotate90; Underline = that.Underline;
		SoundOn = that.SoundOn; SoundOver = that.SoundOver; SoundClick = that.SoundClick;
		for (i = 0; i < 4; i++) EffectOn[i] = that.EffectOn[i];
		for (i = 0; i < 4; i++) EffectConst[i] = that.EffectConst[i];
		for (i = 0; i < 4; i++) EffectOff[i] = that.EffectOff[i];
		for (i = 0; i < 4; i++) MouseOver[i] = that.MouseOver[i];
		for (i = 0; i < 4; i++) MouseClick[i] = that.MouseClick[i];
		for (i = 0; i < 4; i++) EffectTriggered[i] = that.EffectTriggered[i];
		return *this;
	}
	void Reset();														// 0x6BE0E0: the defaults  // NAME 2: what it does

	StringClass StringID;													// +0x00, StringID (the table's key)  // NAME 2: the ini key
	StringClass Unknown04;												// +0x04  // NAME 1 [Unknown04]: neutral (D10)
	StringClass Text;														// +0x08: the String%d lines joined by spaces  // NAME 2: what it holds
	StringClass RenderObj;													// +0x0C, RenderObj  // NAME 2: the ini key
	StringClass Attach;													// +0x10, Attach  // NAME 2: the ini key
	int BlendType[2];														// +0x14, BlendType / AltBlendType  // NAME 2: the ini keys
	StringClass FontType[2];												// +0x1C, FontType / AltFontType  // NAME 2: the ini keys
	StringClass Color[2];													// +0x24, Color / AltColor  // NAME 2: the ini keys
	int Justify[2];															// +0x2C, Justify (two directions)  // NAME 2: the ini key
	float Offset[2];														// +0x34, Offset  // NAME 2: the ini key
	float Target[2];														// +0x3C, Target  // NAME 2: the ini key
	float BoxOffset[2];													// +0x44, BoxOffset  // NAME 2: the ini key
	float Width;															// +0x4C, Width  // NAME 2: the ini key
	float Wait;															// +0x50, Wait  // NAME 2: the ini key
	float TimeSpacing;														// +0x54, TimeSpacing  // NAME 2: the ini key
	float TriggerTime;														// +0x58, TriggerTime  // NAME 2: the ini key
	float Spacing;															// +0x5C, Spacing  // NAME 2: the ini key
	float WrapWidth;														// +0x60, WrapWidth  // NAME 2: the ini key
	bool Rotate90;															// +0x64, Rotate90  // NAME 2: the ini key
	bool Underline;															// +0x65, Underline  // NAME 2: the ini key
	FontSoundStruct SoundOn;													// +0x68, SoundOnID / SoundOnVol / SoundOnDur  // NAME 2: the ini keys
	FontSoundStruct SoundOver;												// +0x74, SoundOverID / SoundOverVol / SoundOverDur  // NAME 2: the ini keys
	FontSoundStruct SoundClick;												// +0x80, SoundClickID / SoundClickVol / SoundClickDur  // NAME 2: the ini keys
	StringClass EffectOn[4];												// +0x8C, EffectOn  // NAME 2: the ini key
	StringClass EffectConst[4];												// +0x9C, EffectConst  // NAME 2: the ini key
	StringClass EffectOff[4];												// +0xAC, EffectOff  // NAME 2: the ini key
	StringClass MouseOver[4];												// +0xBC, MouseOver  // NAME 2: the ini key
	StringClass MouseClick[4];												// +0xCC, MouseClick  // NAME 2: the ini key
	StringClass EffectTriggered[4];											// +0xDC, EffectTriggered  // NAME 2: the ini key
};

class FontDatabaseClass  // NAME 3: by convention (D24) from the client's fontdatabase.cpp
{
public:
	FontDatabaseClass();													// 0x6BE5C0
	~FontDatabaseClass();													// 0x6BEBF0
	void Reload();														// 0x6C1EC0: the ini files again (the constructor inlines it)  // NAME 2: what it does
	void Load();														// inline: what the constructor does  // NAME 2: what it does
	void Load_Init(INIClass &ini, char *section, FontInitStruct *info);	// 0x6BF330: one INIT section and its templates  // NAME 2: what it does
	void Unknown_6BE520();												// 0x6BE520: walks Strings, the body compiled out  // NAME 1 [Unknown_6BE520]: neutral (D10)
	void Load_String(INIClass &ini, char *section, FontStringStruct *str);	// 0x6C0430: one string template and its Template%d templates  // NAME 2: what it does
	void Unknown_6BF910();												// 0x6BF910: loads fontsys.ini's INIT sections  // NAME 1 [Unknown_6BF910]: neutral (D10)
	void Unknown_6BFD00(const char *ini_name);								// 0x6BFD00: COLOR-n and EFFECT-n sections  // NAME 1 [Unknown_6BFD00]: neutral (D10)
	void Unknown_6C2080();
	int Get_Blend_Type(StringClass &name);									// 0x6C2340: BlendType's text to its value  // NAME 2: what it does
	int Get_Direction(StringClass &name);								// 0x6C2460: Direction's text to its value  // NAME 2: what it does
	int Get_Effect_Type(StringClass &name);							// 0x6C25A0: EffectType's text to its value  // NAME 2: what it does												// 0x6C2080  // NAME 1 [Unknown_6C2080]: neutral (D10)
	void Parse_Directions(const char *text, int *values);					// 0x6C2890: two comma-separated directions  // NAME 2: what it does
	void Split_Four(const char *text, StringClass *values);				// 0x6C2A10: four comma-separated fields  // NAME 2: what it does
	FontStruct *Find_Font(const StringClass &name, float &scale);			// 0x6C2BF0  // NAME 2: what it does
	FontInitStruct *Get_Info(int width, int height);						// 0x6C2CB0  // NAME 2: as Get_Info, for a given resolution
	FontColorStruct *Find_Color(const char *name);							// 0x6C2D00  // NAME 2: what it does
	FontEffectStruct *Find_Effect(const char *name);						// 0x6C2E10  // NAME 2: what it does
	FontStringStruct *Find_String(const char *name);						// 0x6C2F20  // NAME 2: what it does
	void Remove_String(const char *name);									// 0x6C3030  // NAME 2: what it does
	void Add_String(FontStringStruct *str);									// 0x6C3380  // NAME 2: what it does
	void Add_Effect(FontEffectStruct *effect);								// 0x6C3550  // NAME 2: what it does
	void Add_Color(FontColorStruct *color);									// 0x6C3720  // NAME 2: what it does
	void Clear_Infos();													// 0x6BEE60  // NAME 2: what it does
	void Clear_Tables();													// 0x6BEFD0: Colors and Effects  // NAME 2: what it does
	void Clear_Strings();												// 0x6BF1F0  // NAME 2: what it does
	void Set_Font(const char *chat_name, unsigned int font_index, float scale, int spacing, int value98, bool force);	// 0x6C38F0: 0xFFFFFF (16777215.0f) keeps a value  // NAME 2: what it does
	void Restore_Font(const char *chat_name);								// 0x6C3D40: the INIT section's defaults  // NAME 2: what it does
	int Find_Font_File_Index(const char *name);							// 0x6C3FB0, the client's name (its assert)
	int Get_Font_File_Index(const char *chat_name);							// 0x6C4070  // NAME 2: what it does
	const char *Get_Font_File_Name(unsigned int index);						// 0x6C4270  // NAME 2: what it does
	float Get_Scale(const char *chat_name);									// 0x6C42E0  // NAME 2: what it returns
	int Get_Spacing(const char *chat_name);									// 0x6C4450  // NAME 2: what it returns
	int Get_Unknown98(const char *chat_name);								// 0x6C45B0  // NAME 1 [Get_Unknown98]: neutral (D10)
	FontInitStruct *Get_Info();												// 0x6C4720  // NAME 2: the INIT section for the resolution
	void Unknown_6C1500(const char *file_name);								// 0x6C1500: a string file's templates into Strings (FontStringFilesClass::Load)  // NAME 1 [Unknown_6C1500]: neutral (D10)
	void Unknown_6C1AA0(const char *file_name);								// 0x6C1AA0: their removal (FontStringFilesClass::Unload)  // NAME 1 [Unknown_6C1AA0]: neutral (D10)
	void Unknown_6C47B0();												// 0x6C47B0: the fonts whose INIT entry 0x6C5C60 changes, set again  // NAME 1 [Unknown_6C47B0]: neutral (D10)
	void Unknown_6C49C0();													// 0x6C49C0  // NAME 1 [Unknown_6C49C0]: neutral (D10)

	std::vector<FontInitStruct *> Infos;									// +0x00  // NAME 2: the INIT sections, by resolution
	std::hash_map<std::string, FontColorStruct *> Colors;					// +0x10  // NAME 2: what it holds
	std::hash_map<std::string, FontEffectStruct *> Effects;					// +0x28  // NAME 2: what it holds
	std::hash_map<std::string, FontStringStruct *> Strings;					// +0x40  // NAME 2: what it holds
	std::vector<std::string> AllFonts;										// +0x58, the client's name (its assert)
	FontStruct Font[MAXFONTS];												// +0x68, the client's name (its assert)
};

extern FontDatabaseClass *Unknown_BE67B8;									// 0xBE67B8, created by 0x526DE0  // NAME 1 [Unknown_BE67B8]: neutral (D10)

struct FontStringFileStruct  // NAME 2: one loaded string file (0x14 bytes)
{
	FontStringFileStruct() : Name(""), Count(0) {}

	std::string Name;														// +0x00: the file name in lower case  // NAME 2: what it holds
	int Count;															// +0x10: Load / Unload references  // NAME 2: what it counts
};

class FontStringFilesClass  // NAME 2: the string files in use, counted (0x18 bytes, 0xBE67BC; fc_default.ini always)
{
public:
	FontStringFilesClass();												// 0x6C4C80
	~FontStringFilesClass();											// 0x6C4F50

	static std::string Lower(const char *name);							// 0x6C4A80  // NAME 2: what it does
	FontStringFileStruct *Find(const char *name);							// 0x6C51A0  // NAME 2: what it does
	void Load(const char *name);											// 0x6C52B0  // NAME 2: what it does
	void Unload(const char *name);											// 0x6C55B0  // NAME 2: what it does

	std::hash_map<std::string, FontStringFileStruct *> Files;				// +0x00  // NAME 2: what it holds
};

#endif //__FONTDATABASE_H
