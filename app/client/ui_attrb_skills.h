// Earth & Beyond client (reconstruction): app\client\ui_attrb_skills.h, the character info window's attributes tab (the
// client's ui_attrb_skills.cpp, 'e:\build\beta\G\app\Client\ui_attrb_skills.cpp'; this header's name is provisional, NAME 1).
// Laid out from the constructor 0x549640 and the destructor 0x549830.
#ifndef UI_ATTRB_SKILLS_H  // NAME 1 [ui_attrb_skills.h]: provisional header name for ui_attrb_skills.cpp
#define UI_ATTRB_SKILLS_H
#include <vector>
#include "sclient.h"
#include "layer.h"
#include "..\..\wwlib\ref_ptr.h"
class GadgetSlider;			// gd_slider.h's sliders (+0xD4, +0xD8)
class RenderObjClass;
class Unknown6A7A00Class;	// what the skill base's 0x62D710 returns (ui_attrb_skills.cpp declares it as far as it calls it)
class Unknown664500Class;	// gd_attrb_skills.cpp's gadget (UnknownC4's elements; ui_attrb_skills.cpp declares it as far as it calls it)

// The skill windows' base (0xBC bytes; constructor 0x62D1F0 over 0x62DE80's, destructor 0x62D300; tables 0xAF7DBC /
// 0xAF7DB0 / 0xAF7DA4), as far as ui_attrb_skills.cpp uses it (its file is not known: near ui_skills.cpp; D137)
class Unknown62D1F0Class : public InterfaceClass	// NAME 1: neutral (D10)
{
public:
	Unknown62D1F0Class(SClient *client);				// 0x62D1F0  // NAME 1 [client]: as UIBioClass's
	virtual ~Unknown62D1F0Class();						// 0x62D300 (deleting 0x62D240)
	virtual void Unknown_54();							// 0x62D330
	virtual void Unknown_58();							// 0x62E7C0 (0x62DE80's class's)
	virtual void Unknown_5C();							// 0x62E800 (0x62DE80's class's)
	virtual void Unknown_60();							// 0x62E870 (0x62DE80's class's)
	virtual void Unknown_64();							// 0x62E890 (0x62DE80's class's)
	virtual bool Unknown_68(int key);					// 0x62E850 (0x62DE80's class's)  // NAME 1 [key]: InterfaceClass's
	virtual void Unknown_7C(UIViewClass *view, bool arg);	// 0x62E660 (0x62DE80's class's)  // NAME 1 [view, arg]: InterfaceClass's
	virtual void Unknown_88();							// +0x88 0x62D270
	virtual InterfaceClass *Unknown_8C() { return(this); }	// +0x8C 0x549760 (inline: emitted with the attributes tab's table)
	virtual InterfaceClass *Unknown_90() { return(NULL); }	// +0x90 0x549780 (inline)
	virtual InterfaceClass *Unknown_94();				// +0x94 (0x62DE80's class's 0x417297)
	virtual void Unknown_98();							// +0x98 0x62D360
	virtual void Unknown_9C();							// +0x9C 0x62DC00
	virtual void Unknown_A0();							// +0xA0 0x62D540
	virtual void Unknown_A4();							// +0xA4 0x62D590
	virtual const char *Unknown_A8();					// +0xA8 (a name: the attributes tab's 'Skills')
	virtual void Unknown_AC();							// +0xAC 0x62D6A0
	virtual void Unknown_B0();							// +0xB0 (0x62DE80's class's 0x4025BD)
	virtual void Unknown_B4();							// +0xB4 0x62EB90
	virtual void Unknown_B8();							// +0xB8 0x62EE10
	virtual void Unknown_BC();							// +0xBC 0x62EEA0 (0x54B050 calls it after emptying the gadgets)
	virtual void Unknown_C0();							// +0xC0 0x62F260
	virtual void Unknown_C4();							// +0xC4 0x62F190
	virtual void Unknown_C8();							// +0xC8 0x62F300
	virtual void Unknown_CC();							// +0xCC (0x62DE80's class's 0x406C7B)
	virtual void Unknown_D0();							// +0xD0
	virtual InterfaceClass *Unknown_D4();				// +0xD4
	Unknown6A7A00Class *Unknown_62D710();				// 0x62D710

	char Unknown84[0xBC - 0x84];						// +0x84 (0x62DE80: +0x84, a container at +0x88, +0x98..+0xB4, flags +0xB8)
};

// The attributes tab (0x124 bytes, tables 0xAEF4FC / 0xAEF4F0 / 0xAEF4E4; 0x5738B0 makes it, the first of the character
// info window's tabs): the strings of 'fa_att.ini', two sliders, two panels and a layer
class UIAttrbSkillsClass : public Unknown62D1F0Class	// NAME 3: class-from-file (D24) for app\Client\ui_attrb_skills.cpp
{
public:
	UIAttrbSkillsClass(SClient *client);				// 0x549640 (0x124 bytes; deleting 0x549800)  // NAME 1 [client]: as UIBioClass's
	virtual ~UIAttrbSkillsClass();						// 0x549830 ('fa_att.ini' unloaded)
	virtual void Unknown_54();							// 0x549AA0 (G_ASSERT '!InterfaceMesh' l.121)
	virtual void Unknown_58();							// 0x54A200
	virtual void Unknown_5C();							// 0x54A3C0 (deactivated: the two panels' +0x5C, then the base's)
	virtual void Unknown_60();							// 0x54A400
	virtual bool Unknown_68(int key);					// 0x54A480  // NAME 1 [key]: InterfaceClass's
	virtual void Unknown_7C(UIViewClass *view, bool arg);	// 0x54A5B0 (the layer onto the view's scene and camera, the panels', the base's)  // NAME 1 [view, arg]: InterfaceClass's
	void Unknown_54A640(bool arg);						// 0x54A640  // NAME 1 [arg]: neutral
	void Unknown_54A900();								// 0x54A900 (FAIL_IF '!gadget' l.358: the shown gadgets laid out, the slider's range)
	void Unknown_54AB20();								// 0x54AB20 (FAIL_IF '!gadget' l.402: the shown gadgets' +0xA4(false))
	void Unknown_54ABE0();								// 0x54ABE0 (no direct caller: the shown slider's value, as +0x60 inlines it)
	virtual InterfaceClass *Unknown_94() { return(this); }	// +0x94 0x5497A0 (inline)
	virtual const char *Unknown_A8() { return("Skills"); }	// +0xA8 0x5497C0 (inline)
	virtual void Unknown_B0();							// +0xB0 0x54AC50 (empty)
	virtual void Unknown_CC();							// +0xCC 0x54AC70 (0x54ACE0)
	virtual void Unknown_D0();							// +0xD0 0x54AC90 (each gadget's 0x664500 while its +0x60)
	virtual InterfaceClass *Unknown_D4() { return(this); }	// +0xD4 0x5497E0 (inline)
	void Unknown_54ACE0();								// 0x54ACE0
	void Unknown_54B050();								// 0x54B050 (G_ASSERT '!Is_Active()' l.486)
	void Unknown_54B140();								// 0x54B140 (G_ASSERT '!gadget' l.577)
	void Unknown_54B370();								// 0x54B370 (FAIL_IF '!gadget' l.577: the gadgets from the last, +0xF0 of them, +0xF8 counting the set ones; inlined by 0x54A200 / 0x54B140)

	RefCountPtr<RenderObjClass> UnknownBC;				// +0xBC (the mesh: its fonts and its interface list entry)
	RefCountPtr<InterfaceClass> UnknownC0;				// +0xC0
	std::vector<Unknown664500Class *> UnknownC4;		// +0xC4 (its allocator byte, then +0xC8 first / +0xCC last / +0xD0 end)
	GadgetSlider *UnknownD4;							// +0xD4
	GadgetSlider *UnknownD8;							// +0xD8
	GadgetClass *UnknownDC;								// +0xDC
	GadgetClass *UnknownE0;								// +0xE0
	bool UnknownE4;										// +0xE4
	int UnknownE8;										// +0xE8 (a slider's value, as an int: 0x54A400)
	int UnknownEC;										// +0xEC (-1)
	int UnknownF0;										// +0xF0
	int UnknownF4;										// +0xF4
	int UnknownF8;										// +0xF8
	RefCountPtr<InterfaceClass> UnknownFC;				// +0xFC
	LayerClass Layer;									// +0x100 (0x54A5B0 sets its scene and camera)  // NAME 2: what it is
};
#endif // UI_ATTRB_SKILLS_H
