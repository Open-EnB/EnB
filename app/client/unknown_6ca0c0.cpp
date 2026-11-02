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

// 0x6CE4E0: the style's font set to Info.FontType[font_index] (clamped to 0..1) and the text rebuilt; a FontStyleClass
// member in the client, not yet reconstructed (FNT3's range): a link stand-in for now.  // NAME 1 [Unknown_6CE4E0]: neutral (D10)
void Unknown_6CE4E0(FontStyleClass *style, int font_index);

// Members fontmanager.h declares as int that this function reads and writes as floats (the client's fld / fstp):
// FontStyleClass::Unknown494.Unknown0C (the alpha), Unknown4A4.Color (the color, 0..1), Unknown458, Unknown488[0..1].
static inline float &As_Float(int &value) { return *(float *)&value; }

// Moves value toward target by step without passing it (the client's inlined pattern in every gradual effect).
static inline float Approach(float value, float target, float step)
{
	if (value > target) {
		value -= step;
		if (value < target) value = target;
	} else if (value < target) {
		value += step;
		if (value > target) value = target;
	}
	return value;
}

// A random character for RANDLETTER: '0'-'9', 'A'-'Z', 'a'-... (rand() / 442 is 0..74; the gaps after '9' and 'Z' skipped).
static inline char Random_Letter()
{
	int r = rand() / 442;
	int c = r + '0';
	if (c >= ':' && c <= '@') {
		c = r + 0x37;
	} else if (c > 'Z' && c < 'a') {
		c = r + 0x36;
	}
	return (char)c;
}

// The client writes the font's XSpacing (+0xD28) from MinSpacing (+0xD2C) directly; font32bit.h has both protected
// (an inline of the client's header, or public members: to settle). Reached through a derived class meanwhile.
struct Font32SpacingAccess : public Font32Class
{
	static void Reset_XSpacing(Font32Class *font) { Font32SpacingAccess *f = (Font32SpacingAccess *)font; f->XSpacing = f->MinSpacing; }
};

// The text as drawn after a change (the effects that rebuild the string: RANDLETTER, STAY, STRETCH share the tail).
static void Rebuild_Texture(FontStyleClass *style, StringClass &text, bool reset_spacing)
{
	if (reset_spacing) {
		Font32SpacingAccess::Reset_XSpacing(style->Font);
	}
	style->Unknown4C4 = style->Unknown2D0.Unknown_6D8DC0(&style->Font, text, 0);
}

// Clamps a property toward Intensity from Initial over time, or (Initial < 0) from its current value: FADE and SCALE.
static float Ramp(const FontEffectStruct *effect, float current, float rate, int elapsed, int delta, bool hold_equal)
{
	if (effect->Initial >= 0.0f) {
		float amount = rate * (float)elapsed;
		if (effect->Intensity <= effect->Initial) {
			if (hold_equal && !(effect->Intensity < effect->Initial)) return current;
			float value = effect->Initial - amount;
			if (value < effect->Intensity) value = effect->Intensity;
			return value;
		}
		float value = amount + effect->Initial;
		if (effect->Intensity < value) value = effect->Intensity;
		return value;
	}
	return Approach(current, effect->Intensity, (float)delta * rate);
}


// NOT EXACT: functional (D64), not yet worked toward the client's bytes
void Unknown_6CA0C0(FontStyleClass *style, FontEffectStruct *effect)
{
	if (effect == NULL) {
		return;
	}

	// Unknown270 is the effect clock (ms), Unknown274[0] its value at the last step. Unknown274[2], [3], [4], [5], [10]
	// are per-effect elapsed times, [7] is 1 on an effect's first step, [11] the STAY wait. Unknown254[k] = 2 marks a
	// property changed (0 the shake offset, 1 the color, 2 the rotation, 3 the alpha, 4 the scale, 5 the spacing).
	// Unknown404 is 'complete' (Is_Complete), Unknown4C5 cleared means the mesh must be rebuilt.
	int now = style->Unknown270;
	int delta = now - style->Unknown274[0];
	float &alpha = As_Float(style->Unknown494.Unknown0C);
	float &scale = style->Unknown2F4[0];
	float &rotation = style->Unknown2F4[1];
	int *rect = style->Unknown420;			// left, right, top, bottom (pixels of the text texture)

	switch (effect->EffectType) {

	case FONT_EFFECT_PULSE: {
		if (style->Unknown274[7] == 1) {
			style->Unknown274[3] = 0;
		}
		scale = (float)(sin((float)style->Unknown274[3] * effect->Speed * 0.0062831f) * effect->Intensity + 1.0f);
		style->Unknown254[4] = 2;
		style->Unknown404 = true;
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
		if (half < phase) {
			alpha = ((float)period - phase) / half;
		} else {
			alpha = phase / half;
		}
		style->Unknown254[3] = 2;
		style->Unknown404 = true;
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
			style->Unknown404 = true;
		} else {
			if (angle > 1.5707963f) {
				angle = 1.5707963f;
				style->Unknown404 = true;
			}
			if (effect->Intensity <= effect->Initial) {
				rotation = (float)-cos(angle);
			} else {
				rotation = (float)-sin(angle);
			}
		}
		style->Unknown254[2] = 2;
		style->Unknown274[2] += delta;
		style->Unknown4C5 = false;
		break;
	}

	case FONT_EFFECT_FADE: {
		alpha = Ramp(effect, alpha, effect->Speed * 0.0005f, style->Unknown274[4], delta, false);
		if (effect->Initial >= 0.0f) {
			style->Unknown274[4] += delta;
		}
		if (alpha == effect->Intensity) {
			style->Unknown404 = true;
		}
		style->Unknown254[3] = 2;
		style->Unknown4C5 = false;
		break;
	}

	case FONT_EFFECT_RANDLETTER: {
		// The letters revealed so far (Speed per 200 ms), the rest random characters (or spaces with Initial <= 0).
		const StringClass &source = style->Info.Text;
		int length = source.Get_Length();
		int target = (int)((float)now * effect->Speed) / 200;
		int shown = (target <= length) ? target : length;
		StringClass text;

		if (effect->Direction == FONT_DIRECTION_RIGHT) {
			// left to right: the first letters, then random ones
			StringClass head(source);
			head.Erase(shown, length - shown);
			text = head;
			for (int i = length - shown; i > 0; i--) {
				text += Random_Letter();
			}
		} else if (effect->Direction == FONT_DIRECTION_LEFT) {
			// right to left: random letters, then the last ones
			StringClass tail(source);
			for (int i = length - shown; i > 0; i--) {
				text += Random_Letter();
			}
			tail.Erase(0, length - shown);
			text += tail;
		} else {
			// in random order: Unknown303[i] marks letter i revealed, Unknown414 counts them
			while (style->Unknown414 != shown) {
				int pick = rand() / (0x8000 / (length - style->Unknown414));
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
				bool revealed = style->Unknown303[i] != 0;
				// Intensity >= 1 reveals the marked letters; below 1 it hides them
				bool letter = (effect->Intensity >= 1.0f) ? revealed : !revealed;
				if (letter) {
					text += source[i];
				} else if (effect->Initial > 0.0f) {
					text += Random_Letter();
				} else {
					text += " ";
				}
			}
		}

		if (style->Unknown406) {
			text += "...";
		}
		Rebuild_Texture(style, text, true);
		style->Unknown4C5 = false;
		rect[0] = 0;
		style->Unknown2C4 = rect[1] = style->Font->String_Pixel_Width(text);
		style->Unknown404 = length < target;
		style->Unknown4C5 = false;
		break;
	}

	case FONT_EFFECT_LETTER:
	case FONT_EFFECT_WIPE: {
		// A clip of the texture rectangle growing (Intensity > 0) or shrinking from one side: WIPE by time,
		// LETTER by the width of the letters shown so far.
		style->Unknown404 = false;
		int amount;
		if (effect->EffectType == FONT_EFFECT_WIPE) {
			amount = (int)((float)now * effect->Speed * 0.01f);
			if (amount < 1) amount = 1;
		} else {
			StringClass text(style->Info.Text);
			int length = style->Info.Text.Get_Length();
			int target = (int)((float)now * effect->Speed) / 200;
			int shown = (target <= length) ? target : length;
			if (effect->Direction == FONT_DIRECTION_RIGHT) {
				text.Erase(shown, length - shown);
			} else {
				text.Erase(0, length - shown);
			}
			amount = style->Font->String_Pixel_Width(text);
			if (amount < 1) amount = 1;
		}

		int width = style->Unknown2C4;
		int height = style->Unknown2C8;
		int old;
		if (effect->Intensity <= 0.0f) {
			// wiping out
			switch (effect->Direction) {
			case FONT_DIRECTION_UP:
				old = rect[3];
				rect[2] = 0;
				rect[3] = (height - amount < 2) ? 1 : height - amount;
				if (rect[3] == 1) style->Unknown404 = true;
				if (old != rect[3]) style->Unknown4C5 = false;
				break;
			case FONT_DIRECTION_DOWN:
				old = rect[2];
				rect[2] = (amount < height) ? amount : height;
				rect[3] = height;
				if (rect[2] == height) style->Unknown404 = true;
				if (old != rect[2]) style->Unknown4C5 = false;
				break;
			case FONT_DIRECTION_LEFT:
				old = rect[1];
				rect[0] = 0;
				rect[1] = (width - amount < 2) ? 1 : width - amount;
				if (rect[1] == 1) style->Unknown404 = true;
				if (old != rect[1]) style->Unknown4C5 = false;
				break;
			case FONT_DIRECTION_RIGHT:
				old = rect[0];
				rect[0] = (amount < width) ? amount : width;
				rect[1] = width;
				if (rect[0] == width) style->Unknown404 = true;
				if (old != rect[0]) style->Unknown4C5 = false;
				break;
			}
		} else {
			// wiping in
			switch (effect->Direction) {
			case FONT_DIRECTION_UP:
				old = rect[2];
				rect[2] = (height - amount < 1) ? 0 : height - amount;
				rect[3] = height;
				if (rect[2] == 0) style->Unknown404 = true;
				if (old != rect[2]) style->Unknown4C5 = false;
				break;
			case FONT_DIRECTION_DOWN:
				old = rect[3];
				rect[2] = 0;
				rect[3] = (amount < height) ? amount : height;
				if (rect[3] == height) style->Unknown404 = true;
				if (old != rect[3]) style->Unknown4C5 = false;
				break;
			case FONT_DIRECTION_LEFT:
				old = rect[0];
				rect[0] = (width - amount < 2) ? 1 : width - amount;
				rect[1] = width;
				if (rect[0] == 1) style->Unknown404 = true;
				if (old != rect[0]) style->Unknown4C5 = false;
				break;
			case FONT_DIRECTION_RIGHT:
				old = rect[1];
				rect[0] = 0;
				rect[1] = (amount < width) ? amount : width;
				if (rect[1] == width) style->Unknown404 = true;
				if (old != rect[1]) style->Unknown4C5 = false;
				break;
			}
		}
		break;
	}

	case FONT_EFFECT_SCROLL: {
		// Unknown47C is the text's position; Unknown4DC / Unknown4E0 the x / y scale Render undoes
		float step = (float)delta * effect->Speed * 0.02f;
		switch (effect->Direction) {
		case FONT_DIRECTION_UP:
			if (style->LineNumber < 1) {
				style->Unknown47C.Y = step / style->Unknown4E0 + style->Unknown47C.Y;
			} else {
				// a line of a multi-line template stops at its last line's place (Unknown454)
				float y = step + style->Unknown47C.Y;
				if (style->Unknown454 < y) {
					style->Unknown47C.Y = style->Unknown454;
					style->Unknown4C5 = false;
					return;
				}
				style->Unknown47C.Y = y;
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
				float &scrolled = As_Float(style->Unknown458);
				if (attach == NULL || style->Info.WrapWidth != 0.0f) {
					style->Unknown47C.X = style->Unknown47C.X - step / style->Unknown4DC;
					style->Unknown4C5 = false;
					if (style->Info.WrapWidth > 0.0f) {
						scrolled = step / style->Unknown4DC + scrolled;
						float wrap = style->Info.WrapWidth * 640.0f;
						if (wrap < scrolled) {
							style->Unknown47C.X = wrap + style->Unknown47C.X;
							scrolled = scrolled - wrap;
						}
					}
				} else {
					float extent = attach->Get_Bounding_Box().Extent.X;
					float width, height;
					Unknown_6D5B10(style, width, height);
					int box = (int)(extent * 1.9f);
					float box_width = (float)box;
					if (box_width < width) {
						style->Unknown47C.X = style->Unknown47C.X - step / style->Unknown4DC;
						style->Unknown4C5 = false;
						scrolled = step / style->Unknown4DC + scrolled;
						if (width < scrolled) {
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
		if (!style->Unknown302) {
			Vector3 pos = style->Get_Position();
			x = pos.X;
			y = pos.Y;
		} else {
			x = style->Unknown464[0];
			y = style->Unknown464[1];
		}
		x = style->Info.Target[0] * 640.0f - x;
		y = style->Info.Target[1] * 480.0f - y;
		if (style->Unknown47C.X == x && style->Unknown47C.Y == y) {
			style->Unknown404 = true;
			return;
		}
		style->Unknown47C.X = Approach(style->Unknown47C.X, x, step / style->Unknown4DC);
		style->Unknown47C.Y = Approach(style->Unknown47C.Y, y, step / style->Unknown4E0);
		style->Unknown404 = false;
		break;
	}

	case FONT_EFFECT_SCALE: {
		scale = Ramp(effect, scale, effect->Speed * 0.002f, style->Unknown274[5], delta, true);
		if (effect->Initial >= 0.0f) {
			style->Unknown274[5] += delta;
		}
		if (scale == effect->Intensity) {
			style->Unknown404 = true;
		}
		style->Unknown254[4] = 2;
		style->Unknown4C5 = false;
		break;
	}

	case FONT_EFFECT_GLOW: {
		// the brighter of the two colors, channel by channel
		style->Unknown4C5 = false;
		const TPoint3D<int> &second = style->Colors[1]->RGB;
		const TPoint3D<int> &current = style->Colors[style->Unknown44C]->RGB;
		As_Float(style->Unknown4A4.Color.X) = (float)(second.X <= current.X ? current.X : second.X) * 0.00392156f;
		As_Float(style->Unknown4A4.Color.Y) = (float)(second.Y <= current.Y ? current.Y : second.Y) * 0.00392156f;
		As_Float(style->Unknown4A4.Color.Z) = (float)(second.Z <= current.Z ? current.Z : second.Z) * 0.00392156f;
		style->Unknown254[1] = 2;
		break;
	}

	case FONT_EFFECT_DROP: {
		style->Unknown444 = 1;
		style->Unknown418 = (int)effect->Offset.X;
		style->Unknown41C = (int)effect->Offset.Y;
		style->Unknown404 = true;
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
		if (style->Info.TimeSpacing * 1000.0f * (float)(style->LineCount - style->LineNumber) <= (float)(style->Unknown274[11] + delta)
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
					if (style->Unknown450 == 0) {
						style->Unknown440 = 0;
					} else if (style->Unknown450 == 1) {
						style->Set_Hidden(1);
					} else if (style->Unknown450 == 2) {
						style->Remove();
					}
				} else {
					style->Unknown440 = (style->Unknown440 + 1) % 3;
				}
				if (style->Unknown405) {
					// the text saved in Unknown2F0 back
					style->Info.Text = style->Unknown2F0;
					StringClass text(style->Info.Text);
					if (style->Unknown406) {
						text += "...";
					}
					Rebuild_Texture(style, text, true);
					style->Unknown2C4 = rect[1] = style->Font->String_Pixel_Width(text);
					style->Unknown2C8 = rect[3] = style->Font->Get_Height();
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
			style->Unknown404 = false;
			rect[3] = style->Unknown2C8;
			style->Unknown414 = 0;
			memset(style->Unknown303, 0, sizeof(style->Unknown303));
			for (int i = 0; i < 6; i++) {
				style->Unknown254[i] = 0;
			}
		} else {
			style->Unknown274[11] += delta;
		}
		break;
	}

	case FONT_EFFECT_SHAKE: {
		style->Unknown4C5 = false;
		float dx = (float)rand() * effect->Intensity * (1.0f / 32768.0f);
		float dy = (float)rand() * effect->Intensity * (1.0f / 32768.0f);
		As_Float(style->Unknown488[0]) = effect->Intensity * 0.5f - dx;
		style->Unknown254[0] = 2;
		style->Unknown404 = true;
		As_Float(style->Unknown488[1]) = effect->Intensity * 0.5f - dy;
		break;
	}

	case FONT_EFFECT_TOGGLE: {
		// toward the second color
		style->Unknown4C5 = false;
		const TPoint3D<int> &second = style->Colors[1]->RGB;
		float step = (float)delta * effect->Speed * 0.01f;
		As_Float(style->Unknown4A4.Color.X) = Approach(As_Float(style->Unknown4A4.Color.X), (float)second.X * 0.00392156f, step);
		As_Float(style->Unknown4A4.Color.Y) = Approach(As_Float(style->Unknown4A4.Color.Y), (float)second.Y * 0.00392156f, step);
		As_Float(style->Unknown4A4.Color.Z) = Approach(As_Float(style->Unknown4A4.Color.Z), (float)second.Z * 0.00392156f, step);
		style->Unknown254[1] = 2;
		style->Unknown404 = true;
		break;
	}

	case FONT_EFFECT_REVERT:
	case FONT_EFFECT_SCROLL_REVERT: {
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
		if (style->Unknown274[7] == 0) {
			if (style->LineNumber > 0 && style->Unknown47C.Y != style->Unknown470.Y) {
				style->Unknown47C.Y = style->Unknown470.Y;
				style->Unknown4C5 = false;
			}
			if (effect->EffectType == FONT_EFFECT_REVERT && style->Unknown47C.X != style->Unknown470.X) {
				style->Unknown47C.X = style->Unknown470.X;
				style->Unknown458 = 0;
				style->Unknown4C5 = false;
			}
		}
		if (style->FontIndex == 1 && style->Unknown274[7] == 0) {
			style->FontIndex = 0;
			Unknown_6CE4E0(style, 0);
			style->Unknown4C5 = false;
		}
		break;
	}

	case FONT_EFFECT_CYCLE: {
		// between the two colors on a sine
		style->Unknown4C5 = false;
		if (style->Unknown274[7] == 1) {
			style->Unknown274[3] = 0;
		}
		const TPoint3D<int> &second = style->Colors[1]->RGB;
		const TPoint3D<int> &current = style->Colors[style->Unknown44C]->RGB;
		double wave = sin((float)style->Unknown274[3] * effect->Speed * 0.0062831f);
		As_Float(style->Unknown4A4.Color.X) = (float)(((second.X - current.X) / 2 * wave + (second.X + current.X) / 2) * 0.00392156f);
		As_Float(style->Unknown4A4.Color.Y) = (float)(((second.Y - current.Y) / 2 * wave + (current.Y + second.Y) / 2) * 0.00392156f);
		As_Float(style->Unknown4A4.Color.Z) = (float)(((second.Z - current.Z) / 2 * wave + (current.Z + second.Z) / 2) * 0.00392156f);
		style->Unknown254[1] = 2;
		break;
	}

	case FONT_EFFECT_SWITCH: {
		// to the other font (Info.FontType[1]) once, at the effect's start
		if (style->Unknown274[9] == 0 || (style->Unknown274[7] == 1 && style->Unknown274[8] == 0)) {
			style->FontIndex = 1 - style->FontIndex;
			Unknown_6CE4E0(style, style->FontIndex);
		}
		style->Unknown404 = true;
		style->Unknown4C5 = false;
		style->Unknown274[9]++;
		break;
	}

	case FONT_EFFECT_STRETCH: {
		// the letter spacing from Initial toward Intensity
		float amount = effect->Speed * 0.05f * (float)style->Unknown274[10];
		float spacing;
		if (effect->Initial < effect->Intensity) {
			spacing = amount + effect->Initial;
			if (effect->Intensity < spacing) spacing = effect->Intensity;
		} else {
			spacing = effect->Initial - amount;
			if (spacing < effect->Intensity) spacing = effect->Intensity;
		}
		style->Unknown448 = (int)spacing;
		style->Font->Set_XSpacing(style->Unknown448);
		StringClass text(style->Info.Text);
		if (style->Unknown406) {
			text += "...";
		}
		Rebuild_Texture(style, text, false);
		style->Unknown2C4 = rect[1] = style->Font->String_Pixel_Width(text);
		style->Unknown2C8 = rect[3] = style->Font->Get_Height();
		style->Unknown274[10] += delta;
		if ((float)style->Unknown448 == effect->Intensity) {
			style->Unknown404 = true;
		}
		style->Unknown254[5] = 2;
		style->Unknown4C5 = false;
		break;
	}
	}
}


// NOT EXACT: functional (D64): a FontStyleClass member (__thiscall) in the client; a free function until fontmanager.h declares it
void Unknown_6D5B10(FontStyleClass *style, float &width, float &height)
{
	width = (640.0f / (float)style->Width) * (float)(style->Unknown420[1] - style->Unknown420[0]);
	height = (480.0f / (float)style->Height) * (float)(style->Unknown420[3] - style->Unknown420[2]);
}


/*
**	Reconstruction notes
**
**	0x6CA0C0 (11,680 bytes) and 0x6D5B10 (0x80): FNT4 of the POC queue (.work/poc/ledger.md), builder TXT2, run 2026-11-02.
**	Interim file (D137): the function before 0x6CA0C0 is fontdatabase.cpp's, the next one unknown_creator's; no known file
**	fits. Both are FontStyleClass members in the client (__thiscall over the style; 0x6D5B10 sits just before fontsys.cpp's
**	Get_Obj_Space_Bounding_Sphere 0x6D5B90, so its file is probably fontsys.cpp): free functions here because fontmanager.h
**	(another builder's file this run) does not declare them yet. Proposal: declare them in FontStyleClass, and move
**	FontColorStruct / FontEffectStruct from fontdatabase.cpp to fontdatabase.h (restated above meanwhile).
**
**	The switch is the client's: EffectType 1..20 through the jump table 0x6CC4BC; the names are the ini words of
**	FontDatabaseClass::Get_Effect_Type 0x6C25A0 (PULSE ... STRETCH), the directions Get_Direction 0x6C2460's.
**	The constants from the client: 0.0062831 (2 pi / 1000 per ms), 500 / Speed (BLINK's period), 0.0005 / 0.002
**	(FADE / SCALE rates), 0.02 (SCROLL / MOVE), 0.01 (WIPE, TOGGLE), 0.001 (REVERT), 0.05 (STRETCH), 1/255, 1.9
**	(SCROLL's box: the attached object's bounding box Extent.X times 1.9), 1000 (STAY's Time in seconds); the
**	strings " " (0xB6E948) and "..." (0xB750A8, appended while Unknown406 is set). The text texture is rebuilt through
**	Unknown_6D8DC0 (fonttexture.cpp) on +0x2D0 with the font's XSpacing reset to MinSpacing first (RANDLETTER, STAY).
**
**	fontmanager.h types as int what this function treats as float: Unknown494.Unknown0C (alpha), Unknown4A4.Color
**	(0..1 floats; Set_Color copies an int color into it: to settle), Unknown458 (SCROLL's distance), Unknown488[0..1]
**	(SHAKE's offset): As_Float above until the header says float. 0x6CE4E0 (the font switch, FNT3's range) is called
**	through a free declaration and gets a link stand-in until it is reconstructed.
*/
