// Earth & Beyond client (reconstruction): app\client\unknown_6ca0c0.h. Interim file (D137): see unknown_6ca0c0.cpp's notes.
#ifndef UNKNOWN_6CA0C0_H
#define UNKNOWN_6CA0C0_H

struct FontEffectStruct;

// FontEffectStruct::EffectType: the ini words FontDatabaseClass::Get_Effect_Type 0x6C25A0 maps  // NAME 2: the client's ini words
enum FontEffectType
{
	FONT_EFFECT_NONE = 0,
	FONT_EFFECT_PULSE = 1,
	FONT_EFFECT_BLINK = 2,
	FONT_EFFECT_ROTATE = 3,
	FONT_EFFECT_FADE = 4,
	FONT_EFFECT_RANDLETTER = 5,
	FONT_EFFECT_LETTER = 6,
	FONT_EFFECT_SCROLL = 7,
	FONT_EFFECT_MOVE = 8,
	FONT_EFFECT_SCALE = 9,
	FONT_EFFECT_WIPE = 10,
	FONT_EFFECT_GLOW = 11,
	FONT_EFFECT_DROP = 12,
	FONT_EFFECT_STAY = 13,
	FONT_EFFECT_SHAKE = 14,
	FONT_EFFECT_TOGGLE = 15,
	FONT_EFFECT_REVERT = 16,
	FONT_EFFECT_SCROLL_REVERT = 17,
	FONT_EFFECT_CYCLE = 18,
	FONT_EFFECT_SWITCH = 19,
	FONT_EFFECT_STRETCH = 20
};

// FontEffectStruct::Direction: FontDatabaseClass::Get_Direction 0x6C2460 (TOP / BOTTOM are UP / DOWN)  // NAME 2: the client's ini words
enum FontEffectDirection
{
	FONT_DIRECTION_NONE = 0,
	FONT_DIRECTION_UP = 1,
	FONT_DIRECTION_DOWN = 2,
	FONT_DIRECTION_LEFT = 3,
	FONT_DIRECTION_RIGHT = 4,
	FONT_DIRECTION_RANDOM = 5
};

// 0x6CA0C0 (one step of one effect) and 0x6D5B10 (the text rectangle in screen units) are FontStyleClass
// members (fontmanager.h), as the client's __thiscall calls have them.

#endif
