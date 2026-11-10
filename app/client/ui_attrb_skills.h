// Earth & Beyond client (reconstruction): app\client\ui_attrb_skills.h, the character info window's attributes tab (the
// client's ui_attrb_skills.cpp, 'e:\build\beta\G\app\Client\ui_attrb_skills.cpp'; this header's name is provisional, NAME 1).
// Laid out from the constructor 0x549640 and the destructor 0x549830.
#ifndef UI_ATTRB_SKILLS_H  // NAME 1 [ui_attrb_skills.h]: provisional header name for ui_attrb_skills.cpp
#define UI_ATTRB_SKILLS_H
#include <vector>
#include "sclient.h"
#include "layer.h"
#include "..\..\wwlib\ref_ptr.h"
#include "ui_skills.h"
class GadgetSlider;			// gd_slider.h's sliders (+0xD4, +0xD8)
class RenderObjClass;
class Unknown664500Class;	// gd_attrb_skills.cpp's gadget (UnknownC4's elements; ui_attrb_skills.cpp declares it as far as it calls it)

// The attributes tab (0x124 bytes, tables 0xAEF4FC / 0xAEF4F0 / 0xAEF4E4; 0x5738B0 makes it, the first of the character
// info window's tabs): the strings of 'fa_att.ini', two sliders, two panels and a layer
class UIAttrbSkillsClass : public UISkillsClass	// NAME 3: class-from-file (D24) for app\Client\ui_attrb_skills.cpp
{
public:
	UIAttrbSkillsClass(SClient *client);				// 0x549640 (0x124 bytes; deleting 0x549800)  // NAME 1 [client]: as UIBioClass's
	virtual ~UIAttrbSkillsClass();						// 0x549830 ('fa_att.ini' unloaded)
	virtual void Unknown_54();							// 0x549AA0 (FAIL_IF '!InterfaceMesh' l.121, G_ASSERT 'robj' l.147 / 155)
	virtual void Unknown_58();							// 0x54A200
	virtual void Unknown_5C();							// 0x54A3C0 (deactivated: the two panels' +0x5C, then the base's)
	virtual void Unknown_60();							// 0x54A400
	virtual bool Unknown_68(int key);					// 0x54A480  // NAME 1 [key]: InterfaceClass's
	virtual void Unknown_7C(UIViewClass *view, bool arg);	// 0x54A5B0 (the layer onto the view's scene and camera, the panels', the base's)  // NAME 1 [view, arg]: InterfaceClass's
	void Unknown_54A640(bool arg);						// 0x54A640  // NAME 1 [arg]: neutral
	void Unknown_54A860(int skill);						// 0x54A860 (no caller: +0xEC set, 0x54B400, 0x54A900)  // NAME 1 [skill]: neutral
	void Unknown_54A900();								// 0x54A900 (FAIL_IF '!gadget' l.358: the shown gadgets laid out, the slider's range)
	void Unknown_54AB20();								// 0x54AB20 (FAIL_IF '!gadget' l.402: the shown gadgets' +0xA4(false))
	void Unknown_54ABE0();								// 0x54ABE0 (no direct caller: the shown slider's value, as +0x60 inlines it)
	virtual void Unknown_B0();							// +0xB0 0x54AC50 (empty)
	virtual void Unknown_CC();							// +0xCC 0x54AC70 (0x54ACE0)
	virtual void Unknown_D0();							// +0xD0 0x54AC90 (each gadget's 0x664500 while its +0x60)
	virtual InterfaceClass *Unknown_D4() { return(this); }	// +0xD4 0x5497E0 (inline; this class's own slot)
	void Unknown_54ACE0();								// 0x54ACE0
	void Unknown_54B050();								// 0x54B050 (G_ASSERT '!Is_Active()' l.486)
	void Unknown_54B140();								// 0x54B140 (G_ASSERT '!gadget' l.577)
	void Unknown_54B400();								// 0x54B400 (no caller out of line: the avatar's class index and the selected skill to the panel +0xFC; inlined by 0x54A480 / 0x54A640 / 0x54A860)
	void Unknown_54B370();								// 0x54B370 (FAIL_IF '!gadget' l.577: the gadgets from the last, +0xF0 of them, +0xF8 counting the set ones; inlined by 0x54A200 / 0x54B140)

	RefCountPtr<RenderObjClass> InterfaceMesh;			// +0xBC (the mesh, Init's 'TSI12B': its fonts and its interface list entry)  // NAME 3: Init's FAIL_IF '!InterfaceMesh' (l.121)
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
