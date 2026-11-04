// Earth & Beyond client (reconstruction): app\client\ui_class.h, the class select window (the client's
// ui_class.cpp per its asserts; this header's name is provisional, NAME 1). Notes at the end of ui_class.cpp.
#ifndef UI_CLASS_H  // NAME 1 [ui_class.h]: provisional header name for ui_class.cpp
#define UI_CLASS_H

#include "intrface.h"
#include "wwstring.h"
#include "..\..\wwlib\ref_ptr.h"
#include "..\..\wwlib\ini.h"
#include <vector>
#include <string>

class GadgetClass;
class LayerClass;
class LoginTaskClass;
class RenderObjClass;
class FlashingGadgetEvent;
class CharacterViewClass;
class GadgetSlider;

// The class select window (ui_class.cpp's __FILE__; the client's RTTI: the type name of its FlashingGadgetEvent
// callback, 0xB7E308: SignalBaseClass::MemberCallback<GadgetClass,ClassSelectInterface,...>), 0x154 bytes,
// tables 0xAF084C / 0xAF0840 / 0xAF0834
class ClassSelectInterface : public InterfaceClass	// NAME 5: the client's RTTI (0xB7E308)
{
public:
	ClassSelectInterface(UIViewClass *view, LayerClass *layer);	// 0x574710 (charview.cpp's 0x679160, step 3)  // NAME 1 [view, layer]: neutral
	virtual ~ClassSelectInterface();				// 0x575220 (0x5751F0 the deleting one)
	virtual void Unknown_60();						// +0x60 0x5754E0 (every frame)
	virtual bool Unknown_68(int key);				// +0x68 0x575660 (the events)  // NAME 1 [key]: InterfaceClass's

	void Unknown_577B80();							// 0x577B80
	void Unknown_5779D0();							// 0x5779D0
	void Unknown_5760C0(bool arg);					// 0x5760C0  // NAME 1 [arg]: neutral
	void Unknown_576270();							// 0x576270
	void Unknown_576360();							// 0x576360 (0x5779D0 calls it)
	void Unknown_5767F0();							// 0x5767F0 (0x5779D0 calls it)
	void Unknown_576AC0();							// 0x576AC0 (Unknown_60: the planet and moons turned)
	void Unknown_575D50();							// 0x575D50
	void Unknown_577DA0(FlashingGadgetEvent *event, GadgetClass *sender);	// 0x577DA0  // NAME 1: neutral (D10); NAME 1 [sender]: neutral
	void Unknown_575CD0();							// 0x575CD0 (Unknown_60 inlines it)
	void Unknown_5761D0();							// 0x5761D0 (Unknown_5779D0 inlines it)
	void Unknown_576A40();							// 0x576A40 (empty, no code callers)
	void Unknown_576A60();							// 0x576A60 (Unknown_5779D0 inlines it)
	void Unknown_576A80();							// 0x576A80
	void Unknown_577D30();							// 0x577D30 (Unknown_60 inlines it)
	void Unknown_577E70(int count);					// 0x577E70 (Unknown_576360 inlines it: the skills' count)  // NAME 1 [count]: what 0x576360 passes
	void Unknown_577EE0();							// 0x577EE0 (Unknown_60 inlines it)
	void Unknown_577F20();							// 0x577F20 (Unknown_60 inlines it)

	CharacterViewClass *CharacterView;				// +0x84
	LoginTaskClass *LoginTask;						// +0x88
	int Unknown8C;									// +0x8C
	GadgetClass *Buttons[14];						// +0x90
	RefCountPtr<RenderObjClass> Models[7];			// +0xC8
	float UnknownE4;								// +0xE4 (20.0f)
	float UnknownE8;								// +0xE8 (20.0f)
	float UnknownEC;								// +0xEC (the planet turn's time left, 1.0f at its start: 0x5779D0, 0x576AC0)
	float UnknownF0;								// +0xF0 (the moons' turn, 0.33 per second: 0x576AC0)
	GadgetClass *SkillGadgets[8];					// +0xF4
	std::vector<std::string> Descriptions;			// +0x114
	std::vector<std::string> SkillDescriptions;		// +0x124
	GadgetClass *Slider;							// +0x134
	int SliderValue;								// +0x138
	bool Unknown13C;								// +0x13C
	bool Unknown13D;								// +0x13D
	bool Unknown13E;								// +0x13E
	bool Unknown13F;								// +0x13F
	bool Unknown140;								// +0x140
	bool Unknown141;								// +0x141
	INIClass INI;									// +0x144
};

typedef ClassSelectInterface Unknown574710InterfaceClass;	// NAME 1 [Unknown574710InterfaceClass]: transition alias

#endif // UI_CLASS_H
