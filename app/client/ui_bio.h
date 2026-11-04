// Earth & Beyond client (reconstruction): app\client\ui_bio.h, the character info window's bio tab (interim placement,
// D137: the client's object is not known, it has no __FILE__ string; linked between ui_avsel.obj and ui_buff.obj, its
// string file 'fa_bio.ini'; header and file name provisional, NAME 1). Laid out from the constructor 0x5603E0 and the
// destructor 0x560660.
#ifndef UI_BIO_H  // NAME 1 [ui_bio.h]: provisional header name (the string file 'fa_bio.ini')
#define UI_BIO_H

#include "sclient.h"

class GadgetSlider;	// gd_slider.cpp's slider (+0xAC)

// The bio tab (0xB0 bytes, tables 0xAEFC84 / 0xAEFC78 / 0xAEFC6C; 0x5738B0 makes it, the fourth of the character info
// window's tabs): a 'TSI10A' mesh, the strings of 'fa_bio.ini', a slider (gadget 1) and a 0x674260 gadget (2)
class UIBioClass : public Unknown5BB320Class	// NAME 1: from the string file 'fa_bio.ini' (the bio tab)
{
public:
	UIBioClass(SClient *client);						// 0x5603E0 (0xB0 bytes; slot 0x560630)  // NAME 1 [client]: what the base's 0x5BB320 takes
	virtual ~UIBioClass();								// 0x560660
	virtual void Unknown_54();							// 0x560740 (InterfaceClass's: slot +0x10)
	virtual void Unknown_58();							// 0x560760 (the font for the mesh, when not active)
	virtual void Unknown_5C();							// 0x5607A0 (the font taken off, when active)
	virtual void Unknown_60();							// 0x5607E0 (the base's)
	virtual bool Unknown_68(int key);					// 0x560800 (keys 1 and 2)  // NAME 1 [key]: InterfaceClass's

	GadgetSlider *UnknownAC;							// +0xAC (the destructor takes its model off)
};

#endif // UI_BIO_H
