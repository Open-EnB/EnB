// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Interim placement (D137): the fonts in use and the string templates, out of fontdatabase.h: the client's fonttexture, unknown_51c800 and unknown_6ca0c0 objects have no
**	<string> initialiser, so the header they took FontSys and its structs from reaches neither <string> nor hash_map.h.
*/
#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __FONTSTRING_H	// NAME 1 [fontstring.h]: neutral, what it declares (D10)
#define __FONTSTRING_H

#include "..\..\wwlib\wwstring.h"
#include "font32bit.h"	// Font32Class (FontStruct's font)

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
	// operator=: the implicit one, inlined in 0x6D0A70 and 0x6D47C0 (a loop per array member, the points and
	// scalars copied, the sounds' 0x6D0F70 called); written out, it costs 0x6D0A70 the inline budget its key needs



















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
	TPoint2D<float> Offset;														// +0x34, Offset  // NAME 2: the ini key
	TPoint2D<float> Target;														// +0x3C, Target  // NAME 2: the ini key
	TPoint2D<float> BoxOffset;													// +0x44, BoxOffset  // NAME 2: the ini key
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

#endif
