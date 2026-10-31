// Earth & Beyond client (reconstruction): app\client\unknown_screenfade.cpp (provisional name). Notes at the end of the file.
#include "..\..\ww3d2\render2d.h"
#include "..\..\wwmath\vector3.h"
#include "..\..\wwmath\wwmath.h"

// One faded value: Value moves to Target at Rate per second (Set: over a time; Update: one frame on)
class ScreenFadeValueClass	// NAME 1: neutral (D10)
{
public:
	ScreenFadeValueClass() : Value(0.0f), Target(0.0f), Rate(0.0f) {}
	void Set(float target, float time);	// 0x7FF010  // NAME 2: what it does
	void Update(float seconds);			// 0x7FF060  // NAME 2: what it does

	float Value;	// +0x00  // NAME 2: what it holds
	float Target;	// +0x04  // NAME 2: what it holds
	float Rate;		// +0x08  // NAME 2: what it holds
	int Unknown0C;	// +0x0C (the objects are 0x10 apart)
};

void ScreenFadeValueClass::Set(float target, float time)
{
	Target = target;
	if (time < 0.0001f) {
		Value = target;
		Rate = 0.0f;
	} else {
		Rate = (target - Value) / time;
	}
}

void ScreenFadeValueClass::Update(float seconds)
{
	float step = seconds * Rate;
	if (WWMath::Fabs(step) > WWMath::Fabs(Target - Value)) {
		Value = Target;
	} else {
		Value += step;
	}
}

// the client's initialisers, in this order: 0xBE8B90, 0xBE8B60, 0xBE8B80, 0xBE8B50, 0xBE8B70, 0xBE8BA0, 0xBE8B40
static ScreenFadeValueClass Letterbox;	// 0xBE8B90  // NAME 2: Enable_Letterbox's (0x7FF830)
static ScreenFadeValueClass Opacity;	// 0xBE8B60  // NAME 2: Set_Screen_Fade_Opacity's (0x7FFBE0)
static ScreenFadeValueClass Red;		// 0xBE8B80  // NAME 2: Set_Screen_Fade_Color's first (0x7FFA30)
static ScreenFadeValueClass Green;		// 0xBE8B50  // NAME 2: Set_Screen_Fade_Color's second
static ScreenFadeValueClass Blue;		// 0xBE8B70  // NAME 2: Set_Screen_Fade_Color's third
static ScreenFadeValueClass UnknownBE8BA0;	// 0xBE8BA0 (no reader)
static ScreenFadeValueClass UnknownBE8B40;	// 0xBE8B40 (no reader)
static Render2DClass *Renderer;		// 0xBE8BAC  // NAME 2: what it holds

extern unsigned int Unknown_BFAE3C;	// NAME 1: neutral (D10): the time now in ms (mainview.cpp declares it too)
extern unsigned int Unknown_BFAE40;	// NAME 1: neutral (D10): the last frame's (scriptedmovie.cpp declares it too)

// the scripted movie view's constructor: the renderer over the whole screen (0..1), every fade at rest
void Unknown_7FF230()
{
	Renderer = new Render2DClass;
	Renderer->Set_Coordinate_Range(Render2DClass::Get_Screen_Resolution());
	Renderer->Enable_Alpha(true);
	Renderer->Set_Coordinate_Range(RectClass(0.0f, 0.0f, 1.0f, 1.0f));
	Letterbox.Set(0.0f, 0.0f);
	Opacity.Set(0.0f, 0.0f);
	Red.Set(0.0f, 0.0f);
	Blue.Set(0.0f, 0.0f);
}

// the scripted movie view's destructor
void Unknown_7FF3A0()
{
	delete Renderer;
	Renderer = NULL;
}

// one frame on: the fades moved, the fade quad over the screen, the letterbox's bars (an eighth of the screen each)
void Unknown_7FF3D0()
{
	RectClass bar;	// (declared here: the client keeps the bars' rectangle apart from the fade's)
	float seconds = (float)(Unknown_BFAE3C - Unknown_BFAE40) * 0.001f;
	Letterbox.Update(seconds);
	Opacity.Update(seconds);
	Red.Update(seconds);
	Green.Update(seconds);
	Blue.Update(seconds);
	Renderer->Reset();
	if (Opacity.Value > 0.0f) {
		RectClass screen(-1.0f, -1.0f, 1.0f, 1.0f);
		unsigned long color = (int)(Opacity.Value * 255.0f);
		color = (color << 8) | (int)(Red.Value * 255.0f);
		color = (color << 8) | (int)(Green.Value * 255.0f);
		color = (color << 8) | (int)(Blue.Value * 255.0f);
		Renderer->Add_Quad(screen, color);
	}
	if (Letterbox.Value > 0.0f) {
		float height = Letterbox.Value * 0.125f;
		bar.Set(0.0f, 0.0f, 1.0f, height);
		Renderer->Add_Quad(bar, 0xFF000000);
		bar.Set(0.0f, 1.0f - height, 1.0f, 1.0f);
		Renderer->Add_Quad(bar, 0xFF000000);
	}
}

void Unknown_7FF810()
{
	Renderer->Render();
}

// the letterbox in (on) or out over time seconds
void Unknown_7FF830(bool on, float time)
{
	Letterbox.Set(on ? 1.0f : 0.0f, time);
}

void Unknown_7FF8A0(const Vector3 &color, float time)	// NAME 1 [color, time]: as 0x7FFA30's
{
	Red.Set(WWMath::Clamp(color.X), time);
	Green.Set(WWMath::Clamp(color.Y), time);
	Blue.Set(WWMath::Clamp(color.Z), time);
}

void Unknown_7FFA30(float red, float green, float blue, float time)
{
	Red.Set(WWMath::Clamp(red), time);
	Green.Set(WWMath::Clamp(green), time);
	Blue.Set(WWMath::Clamp(blue), time);
}

void Unknown_7FFBE0(float opacity, float time)
{
	Opacity.Set(opacity, time);
}

// Reconstruction notes (TG3, 2026-10-31; D163's boundary run 0x7FF230-0x7FF8A0):
// - The object: client 0x7FF010-0x7FFC4F, between option.cpp's (its last hash_map instance 0x7FEFD0) and
//   scriptedmovieline.cpp's (from 0x7FFC50). Its own: the fade value's Set 0x7FF010 / Update 0x7FF060 (no callers:
//   every user inlines them), the seven objects' initialisers 0x7FF0C0-0x7FF220 (three floats zeroed each, 0xBE8B40..
//   0xBE8BA0, 0x10 apart), and the functions on them and the renderer 0xBE8BAC. Only the scripted movie view
//   (scriptedmovie.cpp, ScriptedMovieViewClass) calls them. No asserts, so no __FILE__: the name is provisional
//   (NAME 1), and not scriptedmovie.cpp, whose object starts at 0x7FFEA0 after scriptedmovieline.cpp's.
// - Init zeroes four of the five faded values (not Green, 0xBE8B50), as the client does.
// - Set's store order (Value, then Rate) is the inlined copies' (0x7FF830 / 0x7FF8A0 / 0x7FFA30); the out-of-line one
//   comes out the same.
