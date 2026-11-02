// Earth & Beyond client (reconstruction): app\Client\smouse.cpp (__FILE__ 0xB76820). Notes at the end of the file.
#include "tasks\tsk_login.h"
#include "rendobj.h"
#include "bmp2d.h"
#include "matrix3d.h"
#include "vector3.h"
#include "..\shared\unknown_debug.h"
#include <math.h>

void Unknown_42ECF0(float *x, float *y);	// 0x42ECF0 (input.cpp): the pointer's position, 0..1  // NAME 1 [x, y]: as scriptedmovie.cpp's

// mousetbl.cpp's table (0xBE7C70, filled by 0x7228C0: 'arrow_blank.tga', 'arrow.tga', 'cursor_w.tga' ...)
struct MouseCursorStruct  // NAME 1: neutral (D10)
{
	TextureClass *Texture;		// +0x00
	int HotX;					// +0x04: the hot spot, pixels
	int HotY;					// +0x08
};
extern MouseCursorStruct *Unknown_BE7C70[];  // NAME 1 [Unknown_BE7C70]: neutral (D10)

// What the mouse drags (+0x2C), as far as this file calls it
class SMouseDragClass  // NAME 1: neutral (D10)
{
public:
	void Unknown_685190();										// 0x685190: let go (slot +0x0C when its +0x10 is set)
	virtual void Unknown_00();
	virtual void Unknown_04();
	virtual void Unknown_08();
	virtual void Unknown_0C();
	virtual void Unknown_10(float x, float y);					// +0x10: the pointer moved  // NAME 1 [x, y]: what 0x515440 passes
};

static inline void Release_Model(RenderObjClass *&model)
{
	model->Remove();
	model->Release_Ref();
	model = NULL;
}

// NOT EXACT: register-only (VC6 schedules the stores in another order)
LoginUnknown18Class::LoginUnknown18Class(int unknown)
{
	Unknown00 = unknown;
	Unknown0C = -1;
	Unknown10 = -1;
	Unknown04 = NULL;
	Unknown08 = NULL;
	Unknown14 = -1.0f;
	Unknown18 = -1.0f;
	Unknown1C = 0.0f;
	Unknown20 = 0.0f;
	Unknown24 = false;
	Unknown28 = 0;
	DragObject = NULL;
}

// NOT EXACT: shape (3 rows; mostly registers)
LoginUnknown18Class::~LoginUnknown18Class()
{
	if (Unknown04 != NULL) {
		Release_Model(Unknown04);
	}
	if (DragObject != NULL) {
		DragObject->Unknown_685190();
		DragObject = NULL;
	}
}

// NOT EXACT: shape (2 rows: the EH frame around the new)
RenderObjClass *LoginUnknown18Class::Unknown_515080()
{
	return(new Bitmap2DObjClass(Unknown_BE7C70[Unknown0C]->Texture, 0.0f, 0.0f, false, false, false, false));
}

// NOT EXACT: shape (1 row)
int LoginUnknown18Class::Unknown_515120(int cursor)
{
	int old = Unknown0C;
	if (cursor != old) {
		Matrix3D tm(true);
		if (Unknown04 != NULL) {
			tm = Unknown04->Get_Transform();
			Release_Model(Unknown04);
		}
		Unknown0C = cursor;
		Unknown1C = (float)Unknown_BE7C70[cursor]->HotX * -0.0015625f;
		Unknown20 = (float)Unknown_BE7C70[cursor]->HotY * -0.0020833334f;
		Unknown04 = Unknown_515080();
		if (Unknown04 != NULL) {
			Unknown04->Set_Transform(tm);
			Unknown04->Set_Position(Vector3(Unknown14 + Unknown1C, Unknown18 + Unknown20, 0.0f));
			if (Unknown08 != NULL) {
				Unknown04->Add((SceneClass *)Unknown08);
			}
		}
	}
	return(old);
}

void *LoginUnknown18Class::Unknown_515340(void *scene)
{
	if (DragObject != NULL) {
		DragObject->Unknown_685190();
		DragObject = NULL;
	}
	void *old = Unknown08;
	if (Unknown04 != NULL) {
		Unknown04->Remove();
	}
	Unknown08 = scene;
	if (Unknown04 != NULL && scene != NULL) {
		Unknown04->Add((SceneClass *)scene);
	}
	return(old);
}

// NOT EXACT: shape (3 rows; mostly registers)
void LoginUnknown18Class::Unknown_5153B0()
{
	if (!Unknown24 && Unknown04 != NULL) {
		Release_Model(Unknown04);
		Unknown24 = true;
	}
}

void LoginUnknown18Class::Unknown_515400()
{
	if (Unknown24 && Unknown08 != NULL) {
		int cursor = Unknown0C;
		Unknown0C = -1;
		Unknown_515120(cursor);
		Unknown24 = false;
	}
}

void LoginUnknown18Class::Unknown_515440()
{
	float x;
	float y;
	Unknown_42ECF0(&x, &y);
	if (fabs(Unknown14 - x) > 0.0001 || fabs(Unknown18 - y) > 0.0001) {
		Unknown14 = x;
		Unknown18 = y;
	}
	if (Unknown04 != NULL) {
		Unknown04->Set_Position(Vector3(Unknown1C + Unknown14, Unknown20 + Unknown18, 0.0f));
	}
	if (DragObject != NULL) {
		DragObject->Unknown_10(Unknown14, Unknown18);
	}
}

















































































































bool LoginUnknown18Class::Unknown_515510(SMouseDragClass *drag)
{
	FAIL_IF(!drag) return false;

	FAIL_IF(DragObject) { if (DragObject != NULL) { DragObject->Unknown_685190(); DragObject = NULL; } }
	DragObject = drag;
	return true;
}


bool LoginUnknown18Class::Unknown_5155A0(SMouseDragClass *drag)
{
	if (drag == NULL) return false;
	FAIL_IF(DragObject != drag) return false;
	DragObject->Unknown_685190();
	DragObject = NULL;
	return true;
}

/*
**	Reconstruction notes
**
**	client.exe: smouse.cpp 0x514960-0x515E20; the software mouse (tsk_login.h's LoginUnknown18Class, 0x30
**	bytes: LoginTaskClass +0x18, SClient +0x1274) 0x514FE0-0x5155D5: a Bitmap2DObjClass of mousetbl.cpp's
**	cursor texture moved to the pointer every frame, and the dragged object (+0x2C). FAIL_IFs on the
**	client's lines 267, 269 and 278 (D25). Exact: 0x515340, 0x515400, 0x515440, 0x515510, 0x5155A0;
**	NOT EXACT (D64, functional for the POC): 0x514FE0, 0x515030, 0x515080, 0x515120, 0x5153B0.
**	Unbuilt: the views' defaults before 0x514FE0 and the interface defaults 0x515690-0x515750. ACT (run 2026-11-02).
*/
