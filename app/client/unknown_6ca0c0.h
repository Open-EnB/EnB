// Earth & Beyond client (reconstruction): app\client\unknown_6ca0c0.h. Interim file (D137): see unknown_6ca0c0.cpp's notes.
#ifndef UNKNOWN_6CA0C0_H
#define UNKNOWN_6CA0C0_H

class FontStyleClass;
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

// 0x6CA0C0: one step of one effect on a text object (FontSys::Update_Text 0x6D64C0 is its only caller).
// In the client a FontStyleClass member (__thiscall, this = the style); a free function here until fontmanager.h
// declares it (proposal in the notes). Update_Text calls it as Unknown_6CA0C0(this, effect).  // NAME 1 [Unknown_6CA0C0]: neutral (D10)
void Unknown_6CA0C0(FontStyleClass *style, FontEffectStruct *effect);

// 0x6D5B10: the text rectangle (+0x420) in 640 x 480 screen units; also a FontStyleClass member in the client.  // NAME 1 [Unknown_6D5B10]: neutral (D10)
void Unknown_6D5B10(FontStyleClass *style, float &width, float &height);

#endif
