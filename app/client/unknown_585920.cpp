// Earth & Beyond client (reconstruction): the pull and choice helpers 0x585920 .. 0x585FB0, the point-to-screen 0x585FD0 and the blend weights 0x5860B0 (interim placement,
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
#include <math.h>
#include "vector2.h"
#include "ref_ptr.h"
#include "ww3d.h"
#include "camera.h"
#include "wwmath.h"

class UIMeshClass;
class LayerClass;
#include "unknown_671300.h"

// ui_avatr.cpp's pull: a gadget with its pull model (0x10 bytes; the avatar window's +0x160 / +0x16C / +0x180)
class UIAvatrPullClass	// NAME 1: neutral (D10), as ui_avatr.cpp's
{
public:
	UIAvatrPullClass();									// 0x585920
	~UIAvatrPullClass();								// 0x585950
	void Unknown_5859A0();								// 0x5859A0 (the pull model onto the gadget's box centre)
	void Unknown_585A80();								// 0x585A80 (the gadget's model and the pull shown)
	void Unknown_585AC0();								// 0x585AC0 (the gadget's model and the pull hidden)
	void Unknown_585B00(float delta);					// 0x585B00 (+0x0C moved, wrapping in 0 .. 1)  // NAME 1 [delta]: neutral

	Unknown_671300Class *Gadget;								// +0x00  // NAME 1: neutral (D10)
	RenderObjClass *Pull;								// +0x04  // NAME 2: what ui_avatr.cpp's 0x556290 puts there
	bool Unknown08;										// +0x08
	float Unknown0C;									// +0x0C (0.5)
};

// ui_avatr.cpp's choice (its +0x2DC .. +0x2F4; ui_avatar_cust.cpp's +0x368 .. +0x378 are the same class)
class UIAvatrUnknown2ECStruct	// NAME 1: neutral (D10), as ui_avatr.cpp's
{
public:
	UIAvatrUnknown2ECStruct(RenderObjClass *model, const char *name, const char *texture, const char *part, bool two_digits);	// 0x585B50 (0x24 bytes)  // NAME 1 [model, name, texture, part, two_digits]: what ui_avatr.cpp's 0x559900 passes
	~UIAvatrUnknown2ECStruct();							// 0x585BF0 (ui_avatr.cpp's 0x559F30)
	void Unknown_585C90();								// 0x585C90 (the next choice, wrapping to 0)
	void Unknown_585CC0();								// 0x585CC0 (the previous choice, wrapping to +0x20)
	void Unknown_585CF0(int choice);					// 0x585CF0 (+0x18 set)  // NAME 1 [choice]: neutral
	void Unknown_585D10();								// 0x585D10 (+0x20: how many '%s%02d' models, or '%s%02d.tga' textures, exist)
	void Unknown_585F30();								// 0x585F30 (both gadgets' slot +0x40 on)
	void Unknown_585F60();								// 0x585F60 (both gadgets' slot +0x40 off)
	void Unknown_585F90();								// 0x585F90 (empty)
	void Unknown_585FB0();								// 0x585FB0 (empty)
	GadgetClass *Unknown00[2];							// +0x00 (0x585BF0 destroys them)
	RenderObjClass *Unknown08;							// +0x08 (the window's model, referenced)
	char *Unknown0C;									// +0x0C (the models' name)
	char *Unknown10;									// +0x10 (the textures' name)
	char *Unknown14;									// +0x14 (the part: 'HEAD', 'HAIR', 'BEARD', 'FACE', 'TATTOO', ...)
	int Unknown18;										// +0x18 (the choice)
	bool Unknown1C;										// +0x1C ('%s%02d00' instead of '%s%02d')
	int Unknown20;										// +0x20 (the last choice)
};

// Interim placement (D137)
// @0x585920
UIAvatrPullClass::UIAvatrPullClass()
{
	Gadget = NULL;
	Pull = NULL;
	Unknown08 = false;
	Unknown0C = 0.5f;
}

// Interim placement (D137)
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

// Interim placement (D137)
// @0x5859A0
void UIAvatrPullClass::Unknown_5859A0()
{
	if (Gadget && Pull) {
		AABoxClass box;
		Gadget->Unknown_64(box);
		Matrix3D m = Pull->Get_Transform();
		m.Set_X_Translation(box.Center.X);
		m.Set_Y_Translation(box.Center.Y);
		Pull->Set_Transform(m);
	}
}

// Interim placement (D137)
// @0x585A80
void UIAvatrPullClass::Unknown_585A80()
{
	if (Gadget) {
		Gadget->Peek_Render_Obj()->Set_Hidden(false);
	}
	if (Pull) {
		Pull->Set_Hidden(false);
	}
}

// Interim placement (D137)
// @0x585AC0
void UIAvatrPullClass::Unknown_585AC0()
{
	if (Gadget) {
		Gadget->Peek_Render_Obj()->Set_Hidden(true);
	}
	if (Pull) {
		Pull->Set_Hidden(true);
	}
}

// Interim placement (D137)
// @0x585B00
void UIAvatrPullClass::Unknown_585B00(float delta)
{
	Unknown0C += delta;
	if (Unknown0C > 1.0f) {
		Unknown0C = 0.0f;
	} else if (Unknown0C < 0.0f) {
		Unknown0C = 1.0f;
	}
}

// Interim placement (D137)
// @0x585C90
void UIAvatrUnknown2ECStruct::Unknown_585C90()
{
	if (Unknown18 >= Unknown20) {
		Unknown18 = 0;
		return;
	}
	Unknown18++;
}

// Interim placement (D137)
// @0x585CC0
void UIAvatrUnknown2ECStruct::Unknown_585CC0()
{
	if (Unknown18 <= 0) {
		Unknown18 = Unknown20;
		return;
	}
	Unknown18--;
}


// Interim placement (D137)
// @0x585B50
UIAvatrUnknown2ECStruct::UIAvatrUnknown2ECStruct(RenderObjClass *model, const char *name, const char *texture, const char *part, bool two_digits)
{
	Unknown0C = NULL;
	Unknown10 = NULL;
	Unknown14 = NULL;
	Unknown18 = 0;
	Unknown1C = two_digits;
	Unknown20 = 0;
	for (int i = 0; i < 2; i++) {
		Unknown00[i] = NULL;
	}
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

// Interim placement (D137)
// @0x585CF0
void UIAvatrUnknown2ECStruct::Unknown_585CF0(int choice)
{
	Unknown18 = choice;
}

// Interim placement (D137): the textures through RefCountPtr Create_Get (0x47D010), the models with WW3D's overbright flag set (0x474080)
// @0x585D10
void UIAvatrUnknown2ECStruct::Unknown_585D10()
{
	char name[64];
	int i = 1;
	if (Unknown10 != NULL) {
		while (true) {
			_snprintf(name, sizeof(name), "%s%02d.tga", Unknown10, i);
			RefCountPtr<TextureClass> texture = Create_Get(WW3DAssetManager::Get_Instance()->Get_Texture(name, (MipCountType)0, (WW3DFormat)0, true));
			if (texture != NULL && !texture->Is_Initialized()) {
				TextureLoader::Request_Foreground_Loading(texture.Peek());
			}
			if (texture == NULL || texture->Is_Missing_Texture()) {
				Unknown20 = i - 1;
				break;
			}
			i++;
		}
	} else {
		while (true) {
			if (Unknown1C) {
				_snprintf(name, sizeof(name), "%s%02d00", Unknown0C, i);
			} else {
				_snprintf(name, sizeof(name), "%s%02d", Unknown0C, i);
			}
			bool overbright = WW3D::Is_Overbright_Modify_On_Load_Enabled();
			WW3D::Enable_Overbright_Modify_On_Load(true);
			RefCountPtr<RenderObjClass> robj = Create_Get(WW3DAssetManager::Get_Instance()->Create_Render_Obj(name));
			WW3D::Enable_Overbright_Modify_On_Load(overbright);
			if (robj == NULL) {
				Unknown20 = i - 1;
				break;
			}
			i++;
		}
	}
}


/*
**	Notes.
**	Interim placement (D137): 0x585920 .. 0x585CC0 sit far from ui_avatr.cpp (0x553AF0 .. 0x55C640); the classes keep
**	ui_avatr.cpp's stand-in names (the same mangled names link ui_avatr.cpp's calls here). Defined in ui_avatr.cpp they
**	were inlined into its exact 0x5560E0 / 0x556290 / 0x554B50, or shifted its Create_Peek 0x55C490 and $E 0x55C5D0.
**	The pull's gadget is ui_avatr.cpp's Unknown_671300Class (0x585A80 / 0x585AC0 hide its +0x30 RObj, gd_robj.h's).
**	0x585FD0's x is a float local: with the negation on the double product VC6 moves the fchs after the * sx.
**	0x671300 news a 0x0C-byte object (4A1240Class: two ints after its vtable); its members are in its initialiser list (the vtable store follows them, as the client).
*/

// @0x585BF0
UIAvatrUnknown2ECStruct::~UIAvatrUnknown2ECStruct()
{
	for (int i = 0; i < 2; i++) {
		if (Unknown00[i]) {
			GadgetClass::Destroy(Unknown00[i]);
			Unknown00[i] = NULL;
		}
	}
	if (Unknown08) {
		Unknown08->Release_Ref();
		Unknown08 = NULL;
	}
	if (Unknown0C) {
		delete [] Unknown0C;
	}
	if (Unknown10) {
		delete [] Unknown10;
	}
	if (Unknown14) {
		delete [] Unknown14;
	}
}

// Interim placement (D137)
// @0x585F30
void UIAvatrUnknown2ECStruct::Unknown_585F30()
{
	for (int i = 0; i < 2; i++) {
		if (Unknown00[i]) {
			Unknown00[i]->Unknown_40(true);
		}
	}
}

// Interim placement (D137)
// @0x585F60
void UIAvatrUnknown2ECStruct::Unknown_585F60()
{
	for (int i = 0; i < 2; i++) {
		if (Unknown00[i]) {
			Unknown00[i]->Unknown_40(false);
		}
	}
}

// Interim placement (D137)
// @0x585F90
void UIAvatrUnknown2ECStruct::Unknown_585F90()
{
}

// Interim placement (D137)
// @0x585FB0
void UIAvatrUnknown2ECStruct::Unknown_585FB0()
{
}

// Interim placement (D137). A point to the screen through the camera's view plane (ui_starbase.cpp's 0x63F490 calls it)
// @0x585FD0
void Unknown_585FD0(CameraClass *camera, const Vector3 &point, Vector3 *screen)	// NAME 1 [Unknown_585FD0, camera, point, screen]: neutral (D10), as ui_starbase.cpp's
{
	Vector2 vmin, vmax;
	camera->Get_View_Plane(vmin, vmax);
	double sx = 1.0 / (vmax.X - vmin.X);
	double sy = 1.0 / (vmax.Y - vmin.Y);
	Vector3 view;
	camera->Transform_To_View_Space(view, point);
	float z = (WWMath::Fabs(view.Z) > 0.0001f) ? view.Z : 0.0001f;
	double inv = 1.0 / z;
	float x = view.X * inv;
	screen->Set(-x * sx, inv * view.Y * sy, z);
}

// Interim placement (D137). Five blend weights from a point (u, v) in 0 .. 1, written through weights: the centre's,
// then the four corners' less it, none below 0 (WWLib's min / max templates; the first min on doubles)
// @0x5860B0
void Unknown_5860B0(float *weights, Vector2 uv)	// NAME 1 [Unknown_5860B0, weights, uv]: neutral (D10)
{
	float centre = min(0.5 - fabs(uv.X - 0.5f), 0.5 - fabs(uv.Y - 0.5f));
	weights[0] = centre;
	weights[1] = max(min((1.0f - uv.X), (1.0f - uv.Y)) - centre, 0.0f);
	weights[2] = max(min(uv.X, (1.0f - uv.Y)) - centre, 0.0f);
	weights[3] = max(min(uv.X, uv.Y) - centre, 0.0f);
	weights[4] = max(min((1.0f - uv.X), uv.Y) - centre, 0.0f);
	for (int i = 0; i < 5; i++) {
		if (weights[i] < 0.0f) {
			weights[i] = 0.0f;
		}
	}
}
