// Earth & Beyond client (reconstruction): app\client\unknown_6ca0c0.cpp. Notes at the end of the file.
#include "unknown_6ca0c0.h"
#include "fontmanager.h"
#include "fontdatabase.h"
#include "font32bit.h"
#include "..\..\wwlib\ref_ptr.h"
#include "..\..\wwlib\wwstring.h"
#include "..\..\wwmath\aabox.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>

// fontdatabase.cpp defines these two in its own file; their layouts restated here (data members only, no
// constructors, so nothing of theirs is emitted from this object). Proposal: move both to fontdatabase.h.
struct FontColorStruct  // fontdatabase.cpp's (0x10 bytes)
{
	StringClass Name;														// +0x00
	TPoint3D<int> RGB;														// +0x04
};

struct FontEffectStruct  // fontdatabase.cpp's (0x24 bytes)
{
	StringClass Name;														// +0x00
	int EffectType;															// +0x04: FontEffectType
	float Initial;															// +0x08
	float Intensity;														// +0x0C
	float Speed;															// +0x10
	float Time;																// +0x14
	int Direction;															// +0x18: FontEffectDirection
	TPoint2D<float> Offset;													// +0x1C
};

// Moves value toward target by step without passing it (the client's inlined pattern in every gradual effect).
static inline float Approach(float value, float target, float step)
{
	if (value > target) {
		value -= step;
		if (target > value) value = target;
	} else if (value < target) {
		value += step;
		if (target < value) value = target;
	}
	return value;
}

// A random character for RANDLETTER: '0'-'9', 'A'-'Z', 'a'-... (rand() / 442 is 0..74; the gaps after '9' and 'Z' skipped).
char Random_Letter()	// NAME 2: random ASCII alphanumeric for FONT_EFFECT_RANDLETTER (0x6CA070)
{
	int c = rand() / 442 + '0';
	if (c > '9' && c < 'A') {
		c += 7;
	} else if (c > 'Z' && c < 'a') {
		c += 6;
	}

	return c;
}

// The client writes the font's XSpacing (+0xD28) from MinSpacing (+0xD2C) directly; font32bit.h has both protected
// (an inline of the client's header, or public members: to settle). Reached through a derived class meanwhile.
struct Font32SpacingAccess : public Font32Class
{
	static void Reset_XSpacing(Font32Class *font) { Font32SpacingAccess *f = (Font32SpacingAccess *)font; f->XSpacing = f->MinSpacing; }
};

// The text as drawn after a change (the effects that rebuild the string: RANDLETTER, STAY, STRETCH share the tail).
static void Rebuild_Texture(FontSys *style, char *buffer, bool reset_spacing)
{
	if (reset_spacing) {
		Font32SpacingAccess::Reset_XSpacing(style->Font.Peek());
	}
	style->Unknown4C4 = style->Unknown2D0.Unknown_6D8DC0(&style->Font, buffer, 0);
}
static inline void Append_Dots(StringClass &text, bool on) { if (on) { text += "..."; text.Peek_Buffer(); } }	// NAME 2: Update_Text's "..." layer (fontsys.cpp): the client calls the += copy's Get_Length out of line here too (0x6CAC4E, as 0x6D676E); the dead read is the layer's budget (D272)
static inline void Set_Complete(FontSys *style, bool done) { style->Unknown404 = done; }  // NAME 2: Is_Complete's setter (0x67EEE0 reads +0x404); an inline layer, as the client spends inline budget here
// NOT EXACT: worked toward the client's bytes (FX1 / FX, runs 2026-11-20 and -21; FS 2026-11-22): case order, branch senses, inline layers, budget, Append_Dots (RANDLETTER / STAY), STAY's buffer local (pushed to 0x6D8DC0 and String_Pixel_Width, 0x6CC3B8 / 0x6CC3D3)
void FontSys::Unknown_6CA0C0(FontEffectStruct *effect)
{
	FontSys *style = this;
	if (effect == NULL) {
		return;
	}

	// Unknown270 is the effect clock (ms), Unknown274[0] its value at the last step. Unknown274[2], [3], [4], [5], [10]
	// are per-effect elapsed times, [7] is 1 on an effect's first step, [11] the STAY wait. Unknown254[k] = 2 marks a
	// property changed (0 the shake offset, 1 the color, 2 the rotation, 3 the alpha, 4 the scale, 5 the spacing).
	// Unknown404 is 'complete' (Is_Complete), Unknown4C5 cleared means the mesh must be rebuilt.
	int now = style->Unknown270;
	int delta = now - style->Unknown274[0];
	float &alpha = style->Unknown494.Unknown0C;
	float &scale = style->Unknown2F4[0];
	float &rotation = style->Unknown2F4[1];
	int *rect = style->Unknown420;			// left, right, top, bottom (pixels of the text texture)

	switch (effect->EffectType) {

	case FONT_EFFECT_PULSE: {
		if (style->Unknown274[7] == 1) {
			style->Unknown274[3] = 0;
		}
		scale = (float)sin((float)style->Unknown274[3] * effect->Speed * 0.0062831f) * effect->Intensity + 1.0f;
		style->Unknown254[4] = 2;
		Set_Complete(style, true);
		style->Unknown4C5 = false;
		break;
	}

	case FONT_EFFECT_BLINK: {
		if (style->Unknown274[7] == 1) {
			style->Unknown274[3] = 0;
		}
		int period = (int)(500.0f / effect->Speed);
		float half = (float)period * 0.5f;
		float phase = (float)(style->Unknown274[3] % period);
		if (phase <= half) {
			alpha = phase / half;
		} else {
			alpha = ((float)period - phase) / half;
		}
		style->Unknown254[3] = 2;
		Set_Complete(style, true);
		style->Unknown4C5 = false;
		break;
	}

	case FONT_EFFECT_ROTATE: {
		if (style->Unknown274[7] == 1) {
			rotation = -1.0f;
			style->Unknown274[2] = 0;
		}
		float angle = (float)style->Unknown274[2] * effect->Speed * 0.0062831f;
		if (effect->Initial == effect->Intensity) {
			rotation = (float)-cos(angle);
			Set_Complete(style, true);
		} else {
			if (angle > 1.5707963f) {
				angle = 1.5707963f;
				Set_Complete(style, true);
			}
			if (effect->Initial < effect->Intensity) {
				rotation = (float)-sin(angle);
			} else {
				rotation = (float)-cos(angle);
			}
		}
		style->Unknown254[2] = 2;
		style->Unknown274[2] += delta;
		style->Unknown4C5 = false;
		break;
	}

	case FONT_EFFECT_FADE: {
		float rate = effect->Speed * 0.0005f;
		if (effect->Initial < 0.0) {
			alpha = Approach(alpha, effect->Intensity, (float)delta * rate);
		} else {
			float amount = rate * (float)style->Unknown274[4];
			if (effect->Initial < effect->Intensity) {
				float value = amount + effect->Initial;
				float high = effect->Intensity;
				if (high < value) value = high;
				alpha = value;
			} else {
				float value = effect->Initial - amount;
				float low = effect->Intensity;
				if (low > value) value = low;
				alpha = value;
			}
			style->Unknown274[4] += delta;
		}
		if (alpha == effect->Intensity) {
			Set_Complete(style, true);
		}
		style->Unknown254[3] = 2;
		style->Unknown4C5 = false;
		break;
	}

	case FONT_EFFECT_RANDLETTER: {
		// The letters revealed so far (Speed per 200 ms), the rest random characters (or spaces with Initial <= 0).
		const StringClass &source = style->Info.Text;
		int length = source.Get_Length();
		int target = (int)((float)style->Unknown270 * effect->Speed) / 200;
		int shown = (target <= length) ? target : length;
		StringClass text;

		if (effect->Direction == FONT_DIRECTION_RIGHT) {
			// left to right: the first letters, then random ones
			StringClass head(source); int count = length - shown;
			head.Erase(shown, count);
			head.Get_Length();
			text = head;
			for (int i = count; i > 0; i--) {
				text += Random_Letter();
			}
		} else if (effect->Direction == FONT_DIRECTION_LEFT) {
			// right to left: random letters, then the last ones
			StringClass tail(source); int count = length - shown;
			for (int i = count; i > 0; i--) {
				text += Random_Letter();
			}
			tail.Erase(0, count);
			tail.Get_Length();
			text += tail;
		} else {
			// in random order: Unknown303[i] marks letter i revealed, Unknown414 counts them
			while (shown != style->Unknown414) {
				int span = 0x8000 / (length - style->Unknown414);
				int pick = rand() / span;
				int hidden = 0;
				for (int i = 0; i < length; i++) {
					if (style->Unknown303[i] == 0) {
						if (hidden == pick) {
							style->Unknown303[i] = 1;
							break;
						}
						hidden++;
					}
				}
				style->Unknown414++;
			}
			for (int i = 0; i < length; i++) {
				// Intensity >= 1 reveals the marked letters; below 1 it hides them
				if (effect->Intensity < 1.0f) {
					if (style->Unknown303[i] != 0) {
						if (effect->Initial > 0.0f) {
							text += Random_Letter();
						} else {
							text += " ";
						}
					} else {
						text += style->Info.Text.Get_Buffer(length)[i];
					}
				} else {
					if (style->Unknown303[i] != 0) {
						text += style->Info.Text.Get_Buffer(length)[i];
					} else {
						if (effect->Initial > 0.0f) {
							text += Random_Letter();
						} else {
							text += " ";
						}
					}
				}
			}
		}

		Append_Dots(text, style->Unknown406);

		char *buffer = text.Get_Buffer(text.Get_Length()); Rebuild_Texture(style, buffer, true);
		style->Unknown4C5 = false;
		rect[0] = 0;
		rect[1] = style->Unknown2C4 = style->Font->String_Pixel_Width(buffer);
		if (target > length) {
			Set_Complete(style, true);
		} else {
			Set_Complete(style, false);
		}
		style->Unknown4C5 = false;
		break;
	}

	case FONT_EFFECT_SCROLL: {
		// Unknown47C is the text's position; Unknown4DC / Unknown4E0 the x / y scale Render undoes
		float step = (float)delta * effect->Speed * 0.02f;
		switch (effect->Direction) {
		case FONT_DIRECTION_UP:
			if (style->LineNumber > 0) {
				// a line of a multi-line template stops at its last line's place (Unknown454)
				float y = step + style->Unknown47C.Y;
				float high = style->Unknown454;
				if (high < y) y = high;
				style->Unknown47C.Y = y;
			} else {
				style->Unknown47C.Y = step / style->Unknown4E0 + style->Unknown47C.Y;
			}
			style->Unknown4C5 = false;
			break;
		case FONT_DIRECTION_DOWN:
			style->Unknown47C.Y = style->Unknown47C.Y - step / style->Unknown4E0;
			style->Unknown4C5 = false;
			break;
		case FONT_DIRECTION_LEFT:
			if (style->Object != NULL) {
				// a ticker: wraps after WrapWidth (screen widths), or after the attached sub-object's box
				RefCountPtr<RenderObjClass> attach = Create_Get(style->Object->Get_Sub_Object_By_Name(style->Info.Attach));
				float &scrolled = style->Unknown458;
				if (attach == NULL || style->Info.WrapWidth != 0.0f) {
					float move = step / style->Unknown4DC;
					style->Unknown47C.X = style->Unknown47C.X - move;
					style->Unknown4C5 = false;
					if (style->Info.WrapWidth > 0.0f) {
						scrolled = move + scrolled;
						float wrap = style->Info.WrapWidth * 640.0f;
						if (scrolled > wrap) {
							style->Unknown47C.X = wrap + style->Unknown47C.X;
							scrolled = scrolled - wrap;
						}
					}
				} else {
					float extent = attach->Get_Bounding_Box().Extent.X;
					float width, height;
					style->Unknown_6D5B10(width, height);
					int box = (int)(extent * 1.9f);
					float box_width = (float)box;
					if (width > box_width) {
						float move = step / style->Unknown4DC;
						style->Unknown47C.X = style->Unknown47C.X - move;
						style->Unknown4C5 = false;
						scrolled = move + scrolled;
						if (scrolled > width) {
							style->Unknown47C.X = width + box_width + style->Unknown47C.X;
							scrolled = (float)-box;
						}
					}
				}
			}
			break;
		case FONT_DIRECTION_RIGHT:
			style->Unknown47C.X = step / style->Unknown4DC + style->Unknown47C.X;
			style->Unknown4C5 = false;
			break;
		}
		break;
	}

	case FONT_EFFECT_MOVE: {
		// toward Info.Target (screen fractions), relative to the object's position
		style->Unknown4C5 = false;
		float step = (float)delta * effect->Speed * 0.02f;
		float x, y;
		if (style->Unknown302) {
			x = style->Unknown464[0];
			y = style->Unknown464[1];
		} else {
			Vector3 pos = style->Get_Position();
			x = pos.X;
			y = pos.Y;
		}
		x = style->Info.Target.X * 640.0f - x;
		y = style->Info.Target.Y * 480.0f - y;
		if (style->Unknown47C.X == x && style->Unknown47C.Y == y) {
			Set_Complete(style, true);
			return;
		}
		style->Unknown47C.X = Approach(style->Unknown47C.X, x, step / style->Unknown4DC);
		style->Unknown47C.Y = Approach(style->Unknown47C.Y, y, step / style->Unknown4E0);
		Set_Complete(style, false);
		break;
	}

	case FONT_EFFECT_SCALE: {
		float rate = effect->Speed * 0.002f;
		if (effect->Initial < 0.0f) {
			scale = Approach(scale, effect->Intensity, (float)delta * rate);
		} else {
			if (effect->Initial < effect->Intensity) {
				float value = rate * (float)style->Unknown274[5] + effect->Initial;
				float high = effect->Intensity;
				if (high < value) value = high;
				scale = value;
			} else if (effect->Initial > effect->Intensity) {
				float value = effect->Initial - rate * (float)style->Unknown274[5];
				float low = effect->Intensity;
				if (low > value) value = low;
				scale = value;
			}
			style->Unknown274[5] += delta;
		}
		if (scale == effect->Intensity) {
			Set_Complete(style, true);
		}
		style->Unknown254[4] = 2;
		style->Unknown4C5 = false;
		break;
	}

	case FONT_EFFECT_LETTER:
	case FONT_EFFECT_WIPE: {
		// A clip of the texture rectangle growing (Intensity > 0) or shrinking from one side: WIPE by time,
		// LETTER by the width of the letters shown so far.
		Set_Complete(style, false);
		int amount;
		if (effect->EffectType == FONT_EFFECT_WIPE) {
			int value = (int)((float)style->Unknown270 * effect->Speed * 0.01f);
			if (value < 1) amount = 1; else amount = value;
		} else {
			int length = style->Info.Text.Get_Length();
			int target = (int)((float)style->Unknown270 * effect->Speed) / 200;
			int shown = (target <= length) ? target : length;
			StringClass text(style->Info.Text.Get_Buffer(length));
			if (effect->Direction == FONT_DIRECTION_RIGHT) {
				text.Erase(shown, length - shown);
			} else {
				text.Erase(0, length - shown);
			}
			int value = style->Font->String_Pixel_Width(text.Get_Buffer(text.Get_Length()));
			amount = (value < 1) ? 1 : value;
		}



		int old;
		if (effect->Intensity > 0.0f) {
			// wiping in
			switch (effect->Direction) {
			case FONT_DIRECTION_LEFT:
				old = style->Unknown420[0];
				rect[0] = (style->Unknown2C4 - amount > 1) ? style->Unknown2C4 - amount : 1;
				style->Unknown420[1] = style->Unknown2C4;
				if (style->Unknown420[0] == 1) Set_Complete(style, true);
				if (old != style->Unknown420[0]) style->Unknown4C5 = false;
				break;
			case FONT_DIRECTION_RIGHT:
				old = style->Unknown420[1];
				rect[0] = 0;
				rect[1] = (amount < style->Unknown2C4) ? amount : style->Unknown2C4;
				if (rect[1] == style->Unknown2C4) Set_Complete(style, true);
				if (old != rect[1]) style->Unknown4C5 = false;
				break;
			case FONT_DIRECTION_UP:
				old = style->Unknown420[2];
				rect[2] = (style->Unknown2C8 - amount > 0) ? style->Unknown2C8 - amount : 0;
				rect[3] = style->Unknown2C8;
				if (rect[2] == 0) Set_Complete(style, true);
				if (old != rect[2]) style->Unknown4C5 = false;
				break;
			case FONT_DIRECTION_DOWN:
				old = style->Unknown420[3];
				rect[2] = 0; if (amount >= style->Unknown2C8) amount = style->Unknown2C8;
				rect[3] = amount;
				if (amount == style->Unknown2C8) Set_Complete(style, true);
				if (old != amount) style->Unknown4C5 = false;
				break;
			}
		} else {
			// wiping out
			switch (effect->Direction) {
			case FONT_DIRECTION_LEFT:
				old = style->Unknown420[1];
				rect[0] = 0;
				rect[1] = (style->Unknown2C4 - amount > 1) ? style->Unknown2C4 - amount : 1;
				if (rect[1] == 1) Set_Complete(style, true);
				if (old != rect[1]) style->Unknown4C5 = false;
				break;
			case FONT_DIRECTION_RIGHT:
				old = style->Unknown420[0];
				rect[0] = (amount < style->Unknown2C4) ? amount : style->Unknown2C4;
				rect[1] = style->Unknown2C4;
				if (style->Unknown420[0] == style->Unknown2C4) Set_Complete(style, true);
				if (old != rect[0]) style->Unknown4C5 = false;
				break;
			case FONT_DIRECTION_UP:
				old = style->Unknown420[3];
				rect[2] = 0;
				rect[3] = (style->Unknown2C8 - amount > 1) ? style->Unknown2C8 - amount : 1;
				if (rect[3] == 1) Set_Complete(style, true);
				if (old != rect[3]) style->Unknown4C5 = false;
				break;
			case FONT_DIRECTION_DOWN:
				old = style->Unknown420[2]; if (amount >= style->Unknown2C8) amount = style->Unknown2C8;
				rect[2] = amount;
				style->Unknown420[3] = style->Unknown2C8;
				if (style->Unknown420[2] == style->Unknown2C8) Set_Complete(style, true);
				if (old != rect[2]) style->Unknown4C5 = false;
				break;
			}
		}
		break;
	}

	case FONT_EFFECT_GLOW: {
		// the brighter of the two colors, channel by channel
		style->Unknown4C5 = false;


		style->Unknown4A4.Color.X = (float)(style->Colors[style->Unknown44C]->RGB.X < style->Colors[1]->RGB.X ? style->Colors[1]->RGB.X : style->Colors[style->Unknown44C]->RGB.X) * 0.00392156f;
		style->Unknown4A4.Color.Y = (float)(style->Colors[style->Unknown44C]->RGB.Y < style->Colors[1]->RGB.Y ? style->Colors[1]->RGB.Y : style->Colors[style->Unknown44C]->RGB.Y) * 0.00392156f;
		style->Unknown4A4.Color.Z = (float)(style->Colors[style->Unknown44C]->RGB.Z < style->Colors[1]->RGB.Z ? style->Colors[1]->RGB.Z : style->Colors[style->Unknown44C]->RGB.Z) * 0.00392156f;
		style->Unknown254[1] = 2;
		break;
	}

	case FONT_EFFECT_DROP: {
		style->Unknown444 = 1;
		style->Unknown418 = (int)effect->Offset.X;
		style->Unknown41C = (int)effect->Offset.Y;
		Set_Complete(style, true);
		break;
	}

	case FONT_EFFECT_CYCLE: {
		// between the two colors on a sine
		style->Unknown4C5 = false;
		if (style->Unknown274[7] == 1) {
			style->Unknown274[3] = 0;
		}
		const FontColorStruct *second = style->Colors[1];
		const FontColorStruct *current = style->Colors[style->Unknown44C];
		float wave = sin((float)style->Unknown274[3] * effect->Speed * 0.0062831f);
		TPoint3D<int> half = (second->RGB - current->RGB) / 2;
		TPoint3D<int> mid(second->RGB.X + current->RGB.X, second->RGB.Y + current->RGB.Y, second->RGB.Z + current->RGB.Z); mid = mid / 2;
		style->Unknown4A4.Color.X = (half.X * wave + mid.X) * 0.00392156f;
		style->Unknown4A4.Color.Y = (half.Y * wave + mid.Y) * 0.00392156f;
		style->Unknown4A4.Color.Z = (half.Z * wave + mid.Z) * 0.00392156f;
		style->Unknown254[1] = 2;
		break;
	}

	case FONT_EFFECT_TOGGLE: {
		// toward the second color
		style->Unknown4C5 = false;
		const FontColorStruct *second = style->Colors[1];
		float step = (float)delta * effect->Speed * 0.01f;
		style->Unknown4A4.Color.X = Approach(style->Unknown4A4.Color.X, (float)second->RGB.X * 0.00392156f, step);
		style->Unknown4A4.Color.Y = Approach(style->Unknown4A4.Color.Y, (float)second->RGB.Y * 0.00392156f, step);
		style->Unknown4A4.Color.Z = Approach(style->Unknown4A4.Color.Z, (float)second->RGB.Z * 0.00392156f, step);
		style->Unknown254[1] = 2;
		Set_Complete(style, true);
		break;
	}

	case FONT_EFFECT_SHAKE: {
		style->Unknown4C5 = false;
		float dx = (float)rand() * effect->Intensity * (1.0f / 32768.0f);
		float dy = (float)rand() * effect->Intensity * (1.0f / 32768.0f);
		style->Unknown488[0] = effect->Intensity * 0.5f - dx;
		style->Unknown254[0] = 2;
		Set_Complete(style, true);
		style->Unknown488[1] = effect->Intensity * 0.5f - dy;
		break;
	}

	case FONT_EFFECT_REVERT: {
		// the scale, alpha and rotation back to 1, 1, -1 unless another effect drives them; the position back
		float step = (float)delta * effect->Speed * 0.001f;
		if (style->Unknown254[4] == 0 && scale != 1.0f) {
			scale = Approach(scale, 1.0f, step);
			style->Unknown4C5 = false;
		}
		if (style->Unknown254[3] == 0 && alpha != 1.0f) {
			alpha = Approach(alpha, 1.0f, step);
			style->Unknown4C5 = false;
		}
		if (style->Unknown254[2] == 0 && rotation != -1.0f) {
			rotation = Approach(rotation, -1.0f, step);
			style->Unknown4C5 = false;
		}
		if (style->LineNumber > 0 && style->Unknown274[7] == 0 && style->Unknown47C.Y != style->Unknown470.Y) {
			style->Unknown47C.Y = style->Unknown470.Y;
			style->Unknown4C5 = false;
		}
		if (style->Unknown274[7] == 0 && style->Unknown47C.X != style->Unknown470.X) {
			style->Unknown47C.X = style->Unknown470.X;
			style->Unknown458 = 0;
			style->Unknown4C5 = false;
		}
		if (style->FontIndex == 1 && style->Unknown274[7] == 0) {
			style->FontIndex = 0;
			style->Unknown_6CE4E0(0);
			style->Unknown4C5 = false;
		}
		break;
	}

	case FONT_EFFECT_SCROLL_REVERT: {
		// as REVERT, the vertical position only
		float step = (float)delta * effect->Speed * 0.001f;
		if (style->Unknown254[4] == 0 && scale != 1.0f) {
			scale = Approach(scale, 1.0f, step);
			style->Unknown4C5 = false;
		}
		if (style->Unknown254[3] == 0 && alpha != 1.0f) {
			alpha = Approach(alpha, 1.0f, step);
			style->Unknown4C5 = false;
		}
		if (style->Unknown254[2] == 0 && rotation != -1.0f) {
			rotation = Approach(rotation, -1.0f, step);
			style->Unknown4C5 = false;
		}
		if (style->LineNumber > 0 && style->Unknown274[7] == 0 && style->Unknown47C.Y != style->Unknown470.Y) {
			style->Unknown47C.Y = style->Unknown470.Y;
			style->Unknown4C5 = false;
		}
		if (style->FontIndex == 1 && style->Unknown274[7] == 0) {
			style->FontIndex = 0;
			style->Unknown_6CE4E0(0);
			style->Unknown4C5 = false;
		}
		break;
	}

	case FONT_EFFECT_SWITCH: {
		// to the other font (Info.FontType[1]) once, at the effect's start
		if (style->Unknown274[9] == 0 || (style->Unknown274[7] == 1 && style->Unknown274[8] == 0)) {
			style->FontIndex = 1 - style->FontIndex;
			style->Unknown_6CE4E0(style->FontIndex);
		}
		Set_Complete(style, true);
		style->Unknown4C5 = false;
		style->Unknown274[9]++;
		break;
	}

	case FONT_EFFECT_STRETCH: {
		// the letter spacing from Initial toward Intensity
		float amount = effect->Speed * 0.05f * style->Unknown274[10];
		float spacing;
		if (effect->Initial < effect->Intensity) {
			spacing = amount + effect->Initial;
			float high = effect->Intensity;
			if (high < spacing) spacing = high;
		} else {
			spacing = effect->Initial - amount;
			float low = effect->Intensity;
			if (low > spacing) spacing = low;
		}
		style->Unknown448 = (int)spacing;
		style->Font->Set_XSpacing(style->Unknown448);
		StringClass text(style->Info.Text);
		if (style->Unknown406) {
			text += "..."; text.Peek_Buffer(); text.Peek_Buffer(); text.Peek_Buffer(); text.Peek_Buffer(); text.Peek_Buffer(); text.Peek_Buffer(); text.Peek_Buffer(); text.Peek_Buffer(); text.Peek_Buffer(); text.Peek_Buffer(); text.Peek_Buffer();
		}
		char *buffer = text.Get_Buffer(text.Get_Length()); Rebuild_Texture(style, buffer, false);
		rect[1] = style->Unknown2C4 = style->Font->String_Pixel_Width(buffer);
		rect[3] = style->Unknown2C8 = style->Font->Get_Height();
		style->Unknown274[10] += delta;
		if ((float)style->Unknown448 == effect->Intensity) {
			Set_Complete(style, true);
		}
		style->Unknown254[5] = 2;
		style->Unknown4C5 = false;
		break;
	}

	case FONT_EFFECT_STAY: {
		// Time seconds (negative: until the others complete), then the next stage of the text's life (Unknown440)
		int limit = (int)(effect->Time * 1000.0f);
		bool timed_out = !(now < limit || limit < 0);
		bool all_done = limit < 0 && style->Unknown404;
		if (!timed_out && !all_done) {
			break;
		}
		if (style->Unknown274[11] + delta >= (style->LineCount - style->LineNumber) * (style->Info.TimeSpacing * 1000.0f)
			|| style->Info.Spacing >= 0.0f) {
			style->Unknown_6D5D40();
			style->Unknown274[2] = 0;
			style->Unknown274[3] = 0;
			style->Unknown274[1] = 0;
			style->Unknown274[4] = 0;
			style->Unknown274[9] = 0;
			style->Unknown274[10] = 0;
			style->Unknown274[11] = 0;
			if (all_done) {
				style->Unknown270 = 0;
				style->Unknown274[0] = 0;
				if (style->Unknown440 == 2) {
					// the last stage: Unknown450 says what then (0 again, 1 hide, 2 remove)
					switch (style->Unknown450) {
					case 0:
						style->Unknown440 = 0;
						break;
					case 1:
						style->Set_Hidden(1);
						break;
					case 2:
						style->Remove();
						break;
					}
				} else {
					style->Unknown440 = (style->Unknown440 + 1) % 3;
				}
				if (style->Unknown405) {
					// the text saved in Unknown2F0 back
					style->Info.Text = style->Unknown2F0; style->Info.Text.Peek_Buffer();
					StringClass text(style->Info.Text); text.Peek_Buffer(); text.Peek_Buffer();
					Append_Dots(text, style->Unknown406);


					char *buffer = text.Get_Buffer(text.Get_Length()); Rebuild_Texture(style, buffer, true);
					rect[1] = style->Unknown2C4 = style->Font->String_Pixel_Width(buffer);
					rect[3] = style->Unknown2C8 = style->Font->Get_Height();
				}
			} else {
				rotation = -1.0f;
				style->Unknown270 -= limit;
				style->Unknown274[0] -= limit;
				style->Unknown440++;
			}
			rect[0] = 0;
			rect[1] = style->Unknown2C4;
			rect[2] = 0;
			Set_Complete(style, false);
			rect[3] = style->Unknown2C8;
			style->Unknown414 = 0;
			memset(style->Unknown303, 0, sizeof(style->Unknown303));
			style->Unknown254[0] = 0;
			style->Unknown254[1] = 0;
			style->Unknown254[2] = 0;
			style->Unknown254[3] = 0;
			style->Unknown254[4] = 0;
			style->Unknown254[5] = 0;
		} else {
			style->Unknown274[11] += delta;
		}
		break;
	}
	}
}

/*
**	Reconstruction notes
**
**	0x6CA070 (56 bytes, Random_Letter, exact) sits right before 0x6CA0C0 in this object (inlined at its three
**	RANDLETTER call sites in 0x6CA0C0, out-of-line copy with thunk 0x40F2D6).
**	0x6CA0C0 (11,680 bytes) and 0x6D5B10 (0x80, now in fontsys.cpp): FNT4 of the POC queue (.work/poc/ledger.md), builder TXT2, run 2026-11-02.
**	Interim file (D137): the function before 0x6CA0C0 is fontdatabase.cpp's, the next one unknown_creator's; no known file
**	fits. Both are FontSys members, as in the client (__thiscall over the style; PF, run 2026-11-02-night; 0x6D5B10
**	sits just before fontsys.cpp's Get_Obj_Space_Bounding_Sphere 0x6D5B90, so its file is probably fontsys.cpp).
**	FontColorStruct / FontEffectStruct stay restated above: defining them in fontdatabase.h instantiates their TPoints
**	there, which costs fontdatabase.cpp's Load_Init its match (fontdatabase.cpp's notes).
**
**	The switch is the client's: EffectType 1..20 through the jump table 0x6CC4BC; the names are the ini words of
**	FontDatabaseClass::Get_Effect_Type 0x6C25A0 (PULSE ... STRETCH), the directions Get_Direction 0x6C2460's.
**	The constants from the client: 0.0062831 (2 pi / 1000 per ms), 500 / Speed (BLINK's period), 0.0005 / 0.002
**	(FADE / SCALE rates), 0.02 (SCROLL / MOVE), 0.01 (WIPE, TOGGLE), 0.001 (REVERT), 0.05 (STRETCH), 1/255, 1.9
**	(SCROLL's box: the attached object's bounding box Extent.X times 1.9), 1000 (STAY's Time in seconds); the
**	strings " " (0xB6E948) and "..." (0xB750A8, appended while Unknown406 is set). The text texture is rebuilt through
**	Unknown_6D8DC0 (fonttexture.cpp) on +0x2D0 with the font's XSpacing reset to MinSpacing first (RANDLETTER, STAY).
**
**	fontmanager.h types as floats what this function reads and writes so: Unknown494.Unknown0C (alpha), Unknown4A4.Color
**	(0..1), Unknown458 (SCROLL's distance), Unknown488 (SHAKE's offset). 0x6CE4E0 (the font switch) is in unknown_6cce60.cpp.
**
**	Inline budget (FX, run 2026-11-21): Set_Complete is a file-local layer over the +0x404 stores; with it the client's
**	out-of-line Store_Length in LETTER (0x6CB343, 0x6CB425) follows. The Peek_Buffer reads after STRETCH's "..." and
**	STAY's copy are dead budget reads (D272); their counts are swept (11 / 2 give the client's zero register in edi).
*/
