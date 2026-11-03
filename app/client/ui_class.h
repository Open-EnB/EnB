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
	void Unknown_576AC0();							// 0x576AC0
	void Unknown_575D50();							// 0x575D50
	void Unknown_577DA0(FlashingGadgetEvent *event, GadgetClass *sender);	// 0x577DA0  // NAME 1: neutral (D10); NAME 1 [sender]: neutral

	CharacterViewClass *CharacterView;				// +0x84
	LoginTaskClass *LoginTask;						// +0x88
	int Unknown8C;									// +0x8C
	GadgetClass *Buttons[14];						// +0x90
	RefCountPtr<RenderObjClass> Models[7];			// +0xC8
	float UnknownE4;								// +0xE4 (20.0f)
	float UnknownE8;								// +0xE8 (20.0f)
	int UnknownEC;									// +0xEC
	int UnknownF0;									// +0xF0
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
