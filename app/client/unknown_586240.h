// Earth & Beyond client (reconstruction): app\client\unknown_586240.h, the space lights panel (its model 'light01a',
// 'fa_light01a.ini'): the editor of the sector's light rig (OrbitalManager's [LIGHTS] / [PICKER], light_sector%04d.ini).
// No string names the file: unknown_586240.cpp and this header are interim names (D137), the object 0x586240-0x587530
// (its guarded <string> id 0x5874F0 and the two deleting-destructor thunks after it; ViewUnknown34MessageStruct's
// constructor 0x587540 opens the message box's object).
#ifndef UNKNOWN_586240_H  // NAME 1 [unknown_586240.h]: interim header name (D137)
#define UNKNOWN_586240_H

#include "ui_chat_panel_base.h"
#include "vector3.h"

class UIAvatrPullClass;		// unknown_585920.cpp's pull (0x10 bytes)

// The space lights panel (0x1AC bytes; constructor 0x586240 over ui_chat_panel_base.cpp's 0x56D280, tables 0xAF1020 /
// 0xAF1014 / 0xAF1008; no caller of the constructor in the client: a debug panel, as ui_starbase.cpp's lights panel
// 0x646170). Five lights, as OrbitalManager's [LIGHTS]: key, reverse key, camera, reverse camera, ambient (its buttons'
// texts 'key_light01a' .. 'ambient_light01a'). The pad's colour and point and the four sliders go straight into the
// main view's OrbitalManager; 'save' writes light_sector%04d.ini, 'exit' puts back what was read.
class UISpaceLightsClass : public ChatPanelUIBase	// NAME 1: neutral, after its model's 'light01a' names and the starbase's lights panel
{
public:
	UISpaceLightsClass(SClient *client);				// 0x586240 (deleting 0x5862E0)  // NAME 1 [client]: the base's
	virtual ~UISpaceLightsClass();						// 0x586310
	virtual void Unknown_54();							// +0x54 0x5863D0 ('light01a': seven buttons, four sliders, the pad; Escape)
	virtual void Unknown_58();							// +0x58 0x586720 (activated: the lights read, light 0)
	virtual void Unknown_5C();							// +0x5C 0x5867B0 (InterfaceClass's when Active)
	virtual void Unknown_60();							// +0x60 0x5867D0 (every frame: the sliders, the pad)
	virtual bool Unknown_68(int key);					// +0x68 0x586800 (1 exit: the lights put back; 2 save; 3-7 a light chosen)  // NAME 1 [key]: InterfaceClass's
	void Unknown_5868D0();								// 0x5868D0 (the defaults: 0.5, the offsets 0.95)
	void Unknown_586940();								// 0x586940 (the lights read from the main view's OrbitalManager)
	void Unknown_586A70();								// 0x586A70 (the lights saved: OrbitalManager 0x4F3EF0; inlined into 0x586800)
	void Unknown_586AA0();								// 0x586AA0 (key 1: the lights read again and put back)
	void Unknown_586B90();								// 0x586B90 (the sliders into the current light)
	void Unknown_586D90(int key);						// 0x586D90 (light key - 3 chosen: the sliders, the pad, the buttons' flash)  // NAME 1 [key]: what 0x586800 passes
	void Unknown_586F60();								// 0x586F60 (the colour pad dragged)
	void Unknown_587310(Vector3 point);					// 0x587310 (the pull placed at a pad point, the colour read)  // NAME 1 [point]: neutral

	GadgetClass *Buttons[7];							// +0xA0 'exit', 'save', 'light00' .. 'light04'  // NAME 1: neutral
	GadgetSlider *Slider[4];							// +0xBC (the offset's X, Y, Z; the intensity)  // NAME 1: as the starbase's lights panel
	float SliderValue[4];								// +0xCC (the values last seen: 0x586B90 compares them)  // NAME 1: as the starbase's lights panel
	UIAvatrPullClass *Pull;								// +0xDC (the colour pad and its pull)  // NAME 1: its class's
	int Current;										// +0xE0 (the light chosen; -1 from the constructor)  // NAME 1: as the starbase's lights panel
	float Intensity[5];									// +0xE4 (OrbitalManager's [LIGHTS] I)  // NAME 1: neutral
	Vector3 Color[5];									// +0xF8 ([LIGHTS] R, G, B: from the pad)  // NAME 1: neutral
	Vector3 Position[5];								// +0x134 ([PICKER]: the pad points)  // NAME 1: neutral
	Vector3 Offset[5];									// +0x170 ([LIGHTS] X, Y, Z, kept 0.95 up: the sliders' range 0 .. 1.9)  // NAME 1: neutral
};

#endif // UNKNOWN_586240_H
