// Earth & Beyond client (reconstruction): the pull and choice helpers 0x585920 .. 0x585CC0 (interim placement,
// D137: the client's object is not known; triage's low-confidence lead is ui_cpit_inv.cpp). Notes at the end of the file.
#include "rendobj.h"
#include "aabox.h"
#include "matrix3d.h"
#include "gadget.h"
#include "unknown_675120.h"

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
	void Unknown_585C90();								// 0x585C90 (the next choice, wrapping to 0)
	void Unknown_585CC0();								// 0x585CC0 (the previous choice, wrapping to +0x20)
	char Unknown00[0x18];								// +0x00
	int Unknown18;										// +0x18 (the choice)
	char Unknown1C[0x20 - 0x1C];						// +0x1C
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

	bool Unknown64;										// +0x64
	bool Unknown65;										// +0x65
	bool Unknown66;										// +0x66
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


/*
**	Notes.
**	Interim placement (D137): 0x585920 .. 0x585CC0 sit far from ui_avatr.cpp (0x553AF0 .. 0x55C640); the classes keep
**	ui_avatr.cpp's stand-in names (the same mangled names link ui_avatr.cpp's calls here). Defined in ui_avatr.cpp they
**	were inlined into its exact 0x5560E0 / 0x556290 / 0x554B50, or shifted its Create_Peek 0x55C490 and $E 0x55C5D0.
**	0x585BF0 (the choice's destructor: two gadgets destroyed, +0x08 released, +0x0C .. +0x14 deleted) is not built.
*/
