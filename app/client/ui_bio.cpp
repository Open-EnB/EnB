// Earth & Beyond client (reconstruction): app\client\ui_bio.cpp, the character info window's bio tab (interim placement,
// D137: the client's object has no __FILE__ string; file name provisional, NAME 1). Notes at the end of the file.
#include "sclient.h"
#include "rendobj.h"
#include "..\..\wwlib\ref_ptr.h"
#include "gadget.h"
#include "unknown_675120.h"
#include "gd_slider.h"
#include "fontmanager.h"
#include "ui_bio.h"
#include "..\shared\auxdata\permstr.h"	// (its PermStringInitClass static: 0xBE5B38, initialised by 0x560880)

extern FontManagerClass *Unknown_BE67C4;			// NAME 1 [Unknown_BE67C4]: 0xBE67C4

#include "unknown_674260.h"		// the gadget 0x674260 makes (0x60 bytes)






// 0x5603E0
UIBioClass::UIBioClass(SClient *client) :
	Unknown5BB320Class(new UIMeshClass("TSI10A", 2), client),
	UnknownAC(NULL)
{
	Unknown_BE67BC->Load("fa_bio.ini");
	UnknownAC = new GadgetSlider(false);
	UnknownAC->Unknown_66E460(Create_Peek(Mesh->Peek_Object()), 0, 0);
	Unknown64.Unknown_6870E0(UnknownAC, 1, View, 1, 0);
	GadgetClass *gadget = new Unknown_674260Class(Create_Peek(Mesh->Peek_Object()));
	Unknown64.Unknown_6870E0(gadget, 2, View, 1, 0);
}

// 0x560660
UIBioClass::~UIBioClass()
{
	Unknown_BE67BC->Unload("fa_bio.ini");
	if (UnknownAC) {
		UnknownAC->Unknown_66E460(RefCountPtr<RenderObjClass>(), 0, 0);
	}
}

// 0x560740
void UIBioClass::Unknown_54()
{
	Unknown_10();
}

// 0x560760
void UIBioClass::Unknown_58()
{
	if (!Active) {
		Unknown_BE67C4->Add(Mesh->Peek_Mesh_Object());
		Unknown5BB320Class::Unknown_58();
	}
}

// 0x5607A0
void UIBioClass::Unknown_5C()
{
	if (Active) {
		Unknown_BE67C4->Remove(Mesh->Peek_Mesh_Object());
		Unknown5BB320Class::Unknown_5C();
	}
}

// 0x5607E0
void UIBioClass::Unknown_60()
{
	Unknown5BB320Class::Unknown_60();
}

// 0x560800
bool UIBioClass::Unknown_68(int key)
{
	switch (key) {
		case 1:
			return(true);
		case 2:
			return(true);
	}
	return(false);
}

/*
**	Notes (BIO, 2026-11-04)
**
**	The client's object 0x5603E0-0x5608BF: linked between ui_avsel.obj and ui_buff.obj (0x5608D0 loads 'fc_buff.ini'),
**	no __FILE__ string, no asserts. Its only caller, 0x5738B0, builds the character info window's four tabs (0x549640,
**	0x5C4170, 0x58D080 and this one). After the slots: the adjustor thunks 0x560860 (-4) / 0x560870 (-0x38) of the
**	deleting destructor 0x560630, and the PermStringInitClass static's initialiser 0x560880 / atexit 0x5608B0 (0xBE5B38).
*/
