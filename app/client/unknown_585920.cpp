// Earth & Beyond client (reconstruction): the pull and choice helpers 0x585920 .. 0x585CC0 (interim placement,
// D137: the client's object is not known; triage's low-confidence lead is ui_cpit_inv.cpp). Notes at the end of the file.
#include "rendobj.h"
#include "aabox.h"
#include "matrix3d.h"
#include "gadget.h"
#include "unknown_675120.h"
#include "assetmgr.h"
#include "texture.h"
#include "textureloader.h"
#include "nstrdup.h"
#include <stdio.h>

// ui_avatr.cpp's pull: a gadget with its pull model (0x10 bytes; the avatar window's +0x160 / +0x16C / +0x180)
class UIAvatrPullClass	// NAME 1: neutral (D10), as ui_avatr.cpp's
{
public:
	UIAvatrPullClass();									// 0x585920
	~UIAvatrPullClass();								// 0x585950
	void Unknown_5859A0();								// 0x5859A0 (the pull model onto the gadget's box centre)

	GadgetClass *Gadget;								// +0x00  // NAME 1: neutral (D10)
	RenderObjClass *Pull;								// +0x04  // NAME 2: what ui_avatr.cpp's 0x556290 puts there
	bool Unknown08;										// +0x08
	float Unknown0C;									// +0x0C (0.5)
};

// ui_avatr.cpp's choice (its +0x2DC .. +0x2F4; ui_avatar_cust.cpp's +0x368 .. +0x378 are the same class)
class UIAvatrUnknown2ECStruct	// NAME 1: neutral (D10), as ui_avatr.cpp's
{
public:
	UIAvatrUnknown2ECStruct(RenderObjClass *model, const char *name, const char *texture, const char *part, bool two_digits);	// 0x585B50 (0x24 bytes)  // NAME 1 [model, name, texture, part, two_digits]: what ui_avatr.cpp's 0x559900 passes
	void Unknown_585C90();								// 0x585C90 (the next choice, wrapping to 0)
	void Unknown_585CC0();								// 0x585CC0 (the previous choice, wrapping to +0x20)
	void Unknown_585CF0(int choice);					// 0x585CF0 (+0x18 set)  // NAME 1 [choice]: neutral
	void Unknown_585D10();								// 0x585D10 (+0x20: how many '%s%02d' models, or '%s%02d.tga' textures, exist)
	GadgetClass *Unknown00[2];							// +0x00 (0x585BF0 destroys them)
	RenderObjClass *Unknown08;							// +0x08 (the window's model, referenced)
	char *Unknown0C;									// +0x0C (the models' name)
	char *Unknown10;									// +0x10 (the textures' name)
	char *Unknown14;									// +0x14 (the part: 'HEAD', 'HAIR', 'BEARD', 'FACE', 'TATTOO', ...)
	int Unknown18;										// +0x18 (the choice)
	bool Unknown1C;										// +0x1C ('%s%02d00' instead of '%s%02d')
	int Unknown20;										// +0x20 (the last choice)
};

class UIMeshClass;
class LayerClass;
// 0x4A1240's object (0x0C bytes, deleted through its slot 0): what 0x671300 makes when its second argument is set
class UIAvatrUnknown4A1240Class	// NAME 1: neutral (D10)
{
public:
	UIAvatrUnknown4A1240Class(int a, int b, int c);	// 0x4A1240 (0x671300: its second argument, 0, 0)  // NAME 1 [a, b, c]: neutral
	virtual ~UIAvatrUnknown4A1240Class();
};

// ui_avatr.cpp's pull gadget: a ButtonGadgetClass (0x80 bytes, vtable 0xAFB278, the drag of a pull), as far as made here
class UIAvatrGadget671300Class : public ButtonGadgetClass	// NAME 1: neutral (D10), as ui_avatr.cpp's
{
public:
	UIAvatrGadget671300Class(RenderObjClass *area, int a, UIMeshClass *mesh, int b, int c, int d);	// 0x671300 (mesh: the window's layer)  // NAME 1 [area, a, mesh, b, c, d]: as ui_avatr.cpp's
	virtual ~UIAvatrGadget671300Class();				// +0x00 0x6714F0 (deleting 0x6713E0)
	virtual bool Unknown_2C(KernelEventClass *event, float x, float y, unsigned int mask);	// +0x2C 0x671570 (the drag)  // NAME 1 [event, x, y, mask]: gadget.h's
	virtual void Unknown_A4();							// +0xA4 0x671650
	virtual void Unknown_A8();							// +0xA8 0x671670
	virtual void Unknown_AC();							// +0xAC 0x6716B0
	void Unknown_671630();								// 0x671630 (the frame's end: the drag's flags cleared)

	bool Unknown64;										// +0x64 (pressed: mask 0x11)
	bool Unknown65;										// +0x65 (held: mask 0x22)
	bool Unknown66;										// +0x66 (released: mask 0x44)
	UIAvatrUnknown4A1240Class *Unknown68;				// +0x68
	int Unknown6C;										// +0x6C
	int Unknown70;										// +0x70
	bool Unknown74;										// +0x74
	int Unknown78;										// +0x78 (the constructor's b)
	int Unknown7C;										// +0x7C (the constructor's c)
};


// NOT EXACT: interim placement (D137), functional (the client's slots +0x2C / +0xA4 .. +0xAC not built: gen stand-ins)
// @0x671300
UIAvatrGadget671300Class::UIAvatrGadget671300Class(RenderObjClass *area, int a, UIMeshClass *mesh, int b, int c, int d) :
	ButtonGadgetClass(area, *(LayerClass *)mesh, 0x77, d)
{
	Unknown64 = false;
	Unknown65 = false;
	Unknown66 = false;
	Unknown68 = NULL;
	Unknown6C = 0;
	Unknown70 = 0;
	Unknown74 = false;
	Unknown78 = b;
	Unknown7C = c;
	if (a) {
		Unknown68 = new UIAvatrUnknown4A1240Class(a, 0, 0);
	}
}

// NOT EXACT: interim placement (D137), functional
// @0x6714F0
UIAvatrGadget671300Class::~UIAvatrGadget671300Class()
{
	if (Unknown68) {
		delete Unknown68;
		Unknown68 = NULL;
	}
}


// Interim placement (D137): an enabled pull passes the event to its
// base and notes which kind it was (the window reads the flags each frame, then 0x671630 clears them)
// @0x671570
bool UIAvatrGadget671300Class::Unknown_2C(KernelEventClass *event, float x, float y, unsigned int mask)
{
	if (Unknown_60()) {
		if (mask & 0x11) {
			Unknown_66B770Class::Unknown_2C(event, x, y, mask);
			Unknown64 = true;
			return true;
		}
		if (mask & 0x22) {
			Unknown_66B770Class::Unknown_2C(event, x, y, mask);
			Unknown65 = true;
			return true;
		}
		if (mask & 0x44) {
			Unknown_66B770Class::Unknown_2C(event, x, y, mask);
			Unknown66 = true;
			return true;
		}
	}
	return false;
}

// Interim placement (D137)
// @0x671630
void UIAvatrGadget671300Class::Unknown_671630()
{
	Unknown64 = false;
	Unknown65 = false;
	Unknown66 = false;
}

// NOT EXACT: interim placement (D137), functional
// @0x585920
UIAvatrPullClass::UIAvatrPullClass()
{
	Gadget = NULL;
	Pull = NULL;
	Unknown08 = false;
	Unknown0C = 0.5f;
}

// NOT EXACT: interim placement (D137), functional
// @0x585950
UIAvatrPullClass::~UIAvatrPullClass()
{
	if (Gadget) {
		GadgetClass::Destroy(Gadget);
		Gadget = NULL;
	}
	if (Pull) {
		Pull->Remove();
		Pull->Release_Ref();
		Pull = NULL;
	}
}

// NOT EXACT: interim placement (D137), functional
// @0x5859A0
void UIAvatrPullClass::Unknown_5859A0()
{
	if (Gadget && Pull) {
		AABoxClass box;
		Gadget->Unknown_64(box);
		Matrix3D m = Pull->Get_Transform();
		m[0][3] = box.Center.X;
		m[1][3] = box.Center.Y;
		Pull->Set_Transform(m);
	}
}

// NOT EXACT: interim placement (D137), functional
// @0x585C90
void UIAvatrUnknown2ECStruct::Unknown_585C90()
{
	if (Unknown18 >= Unknown20) {
		Unknown18 = 0;
		return;
	}
	Unknown18++;
}

// NOT EXACT: interim placement (D137), functional
// @0x585CC0
void UIAvatrUnknown2ECStruct::Unknown_585CC0()
{
	if (Unknown18 < 1) {
		Unknown18 = Unknown20;
		return;
	}
	Unknown18--;
}


// NOT EXACT: interim placement (D137), functional
// @0x585B50
UIAvatrUnknown2ECStruct::UIAvatrUnknown2ECStruct(RenderObjClass *model, const char *name, const char *texture, const char *part, bool two_digits)
{
	Unknown0C = NULL;
	Unknown10 = NULL;
	Unknown14 = NULL;
	Unknown18 = 0;
	Unknown1C = two_digits;
	Unknown20 = 0;
	Unknown00[0] = NULL;
	Unknown00[1] = NULL;
	Unknown08 = model;
	model->Add_Ref();
	if (name) {
		Unknown0C = nstrdup(name);
	}
	if (texture) {
		Unknown10 = nstrdup(texture);
	}
	if (part) {
		Unknown14 = nstrdup(part);
	}
}

// NOT EXACT: interim placement (D137), functional
// @0x585CF0
void UIAvatrUnknown2ECStruct::Unknown_585CF0(int choice)
{
	Unknown18 = choice;
}

// NOT EXACT: interim placement (D137), functional (the client also sets WW3D's 0xBFAE52 around the model lookup)
// @0x585D10
void UIAvatrUnknown2ECStruct::Unknown_585D10()
{
	char name[64];
	int i = 1;
	if (Unknown10 == NULL) {
		while (true) {
			_snprintf(name, sizeof(name), Unknown1C ? "%s%02d00" : "%s%02d", Unknown0C, i);
			RenderObjClass *robj = WW3DAssetManager::Get_Instance()->Create_Render_Obj(name);
			if (robj == NULL) {
				break;
			}
			robj->Release_Ref();
			i++;
		}
		Unknown20 = i - 1;
	} else {
		while (true) {
			_snprintf(name, sizeof(name), "%s%02d.tga", Unknown10, i);
			TextureClass *texture = WW3DAssetManager::Get_Instance()->Get_Texture(name, (MipCountType)0, (WW3DFormat)0, true);
			if (texture == NULL) {
				break;
			}
			if (!texture->Is_Initialized()) {
				TextureLoader::Request_Foreground_Loading(texture);
			}
			bool missing = texture->Is_Missing_Texture();
			texture->Release_Ref();
			if (missing) {
				break;
			}
			i++;
		}
		Unknown20 = i - 1;
	}
}


/*
**	Notes.
**	Interim placement (D137): 0x585920 .. 0x585CC0 sit far from ui_avatr.cpp (0x553AF0 .. 0x55C640); the classes keep
**	ui_avatr.cpp's stand-in names (the same mangled names link ui_avatr.cpp's calls here). Defined in ui_avatr.cpp they
**	were inlined into its exact 0x5560E0 / 0x556290 / 0x554B50, or shifted its Create_Peek 0x55C490 and $E 0x55C5D0.
**	0x585BF0 (the choice's destructor: two gadgets destroyed, +0x08 released, +0x0C .. +0x14 deleted) is not built.
*/
