// Earth & Beyond client (reconstruction): app\Client\camctl.cpp (the client's __FILE__), the camera control gadget.
// Notes at the end.
#include <string.h>
#include "camctl.h"
#include "..\shared\g_assert.h"

bool Unknown_42EC60(int unknown1, int unknown2);	// 0x42EC60 (input.cpp: the keyboard's slot +0x0C, a key held)  // NAME 1 [Unknown_42EC60, unknown1, unknown2]: as input.cpp's



// @0x65B930
// NOT EXACT (0.36, attempts.py 0x65B930): the client stores each scaled value to a stack temporary and copies it at once, and keeps this in edi (ours esi)
CamCtlGadgetClass::CamCtlGadgetClass(CamCtlControlClass *control, const Rect &outer, const Rect &inner) :
	RectangleGadgetClass(outer.X, outer.Y, outer.Width, outer.Height, 0x80),
	Control(control),
	Inside(false),
	Outer(0.0f, 0.0f, 0.0f, 0.0f),
	Inner(0.0f, 0.0f, 0.0f, 0.0f)






{




	G_ASSERT(outer.X <= inner.X);
	G_ASSERT(outer.Y <= inner.Y);
	G_ASSERT(outer.X + outer.Width >= inner.X + inner.Width);
	G_ASSERT(outer.Y + outer.Height >= inner.Y + inner.Height);
	Outer = RectClass((float)outer.X * 0.0015625f, (float)outer.Y * 0.0020833334f, (float)(outer.Width + 1) * 0.0015625f, (float)(outer.Height + 1) * 0.0020833334f);
	Inside = false;
	Inner = RectClass((float)inner.X * 0.0015625f, (float)inner.Y * 0.0020833334f, (float)(inner.Width + 1) * 0.0015625f, (float)(inner.Height + 1) * 0.0020833334f);
}

// @0x65BC20
bool CamCtlGadgetClass::Unknown_34(KernelEventClass *event, float x, float y, unsigned int mask)
{
	if ((x < Inner.Left || x >= Inner.Left + Inner.Right || y < Inner.Top || y >= Inner.Top + Inner.Bottom) &&
		(x >= Outer.Left && x < Outer.Left + Outer.Right && y >= Outer.Top && y < Outer.Top + Outer.Bottom)) {
		if (Unknown_BE5F28 == this || Unknown_BE5F28 == NULL) {
			if (!Inside) {
				Unknown_68();
				Inside = true;
			}
			float dx = 0.0f;
			float dy = 0.0f;
			if (x < Inner.Left) {
				dx = -1.0f;
			} else if (x > Inner.Left + Inner.Right) {
				dx = 1.0f;
			}
			if (y < Inner.Top) {
				dy = 1.0f;
			} else if (y > Inner.Top + Inner.Bottom) {
				dy = -1.0f;
			}
			unsigned int keys = 0;
			if (Unknown_42EC60(0x10, 0)) {
				keys = 0x100;
			}
			if (Unknown_42EC60(0x11, 0)) {
				keys |= 0x200;
			}
			if (Unknown_42EC60(0x12, 0)) {
				keys |= 0x400;
			}
			Control->Unknown_00(dx, dy, keys);
			return(true);
		}
	} else if (Inside) {
		Unknown_50();
		Unknown_6C();
		Inside = false;
		return(true);
	}
	return(false);
}

/*
** Reconstruction notes
** - The object: 0x65B930-0x65BDD4 (__FILE__ 'e:\build\beta\G\app\Client\camctl.cpp', the constructor's four
**   asserts on lines 30-33), after uiview.cpp's (to 0x65B920) and before chatgadg.cpp's, which opens with its
**   PermStringInit initialiser 0x65BE50. Its table 0xAF98C4 overrides RectangleGadgetClass's slots +0x00, +0x34
**   and +0x50; the rest are GadgetClass's and RectangleGadgetClass's. 0x65BBC0 (RectangleGadgetClass's implicit
**   destructor, the constructor's unwind) and 0x65BBE0 (this class's) are each a jmp to ~GadgetClass: the class
**   declares no destructor (a declared empty one stores the table first: 11 bytes).
** - Nothing in the client constructs it (no caller of 0x65B930 or of its thunk): a gadget left in.
** - The rectangles: Outer / Inner hold X and Y over 640 / 480 and (Width + 1), (Height + 1) the same way, in a
**   RectClass's Left, Top, Right, Bottom (the band test adds Right to Left: a width, not a right edge).
** - Unknown_34's form: one return(false) at the end for both the band held by another gadget and the pointer outside
**   when not Inside (the client's shared exit); early returns put an exit after each test.
** - NOT EXACT: the constructor (order and register: the client fstp's each scaled value to a stack temporary and
**   copies it at once; this in edi; the asserts, the base call and the zeroed members match).
*/
