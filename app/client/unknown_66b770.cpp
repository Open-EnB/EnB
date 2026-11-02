// Earth & Beyond client (reconstruction): a model gadget with modes (PROVISIONAL file, D137). Notes at the end of the file.
#include "unknown_66b770.h"

Unknown_66B770Class::Unknown_66B770Class(RenderObjClass *robj, const LayerClass &layer, unsigned int mask, unsigned int flags) :
	RObjGadgetClass(robj, layer, mask),
	Unknown60(flags)
{
}

Unknown_66B770Class::Unknown_66B770Class(RenderObjClass *robj, unsigned int mask, unsigned int flags) :
	RObjGadgetClass(robj, mask),
	Unknown60(flags)
{
}

Unknown_66B770Class::~Unknown_66B770Class()
{
}

void Unknown_66B770Class::Unknown_40(bool on)
{
	if (on) {
		if ((Unknown60 & 4) == 4) {
			Unknown_8C(false);
		}
		if ((Unknown60 & 2) == 2) {
			Unknown_66C360();
		}
	} else {
		if ((Unknown60 & 2) == 2) {
			Unknown_66C2D0();
		} else if ((Unknown60 & 1) == 1 && (Unknown_BE5F24 == this || Unknown_BE5F28 == this)) {
			Unknown_66C360();
		}
		if ((Unknown60 & 4) == 4) {
			Unknown_8C(true);
		}
	}
	GadgetClass::Unknown_40(on);
}

// NOT EXACT: shape (block order): the client puts the 0x40 test of the held-button check right after the 4 test; tried ?:, && / ||, a bool, early returns, the branches swapped
bool Unknown_66B770Class::Unknown_2C(KernelEventClass *event, float x, float y, unsigned int mask)
{
	if (!Unknown_60()) {
		return(false);
	}
	if (mask & 0x11) {
		GadgetClass::Unknown_2C(event, x, y, mask);
		if (mask & 1) {
			if (Mask & 4) {
				Unknown_68();
				if ((Unknown60 & 1) == 1) {
					Unknown_66C2D0();
				}
			}
		} else if (Mask & 0x40) {
			Unknown_68();
			if ((Unknown60 & 1) == 1) {
				Unknown_66C2D0();
			}
		}
	} else if (((mask & 4) && Unknown_BE5F24 == this) || ((mask & 0x40) && Unknown_BE5F28 == this)) {
		if ((Unknown60 & 1) == 1) {
			Unknown_66C360();
		}
		GadgetClass::Unknown_2C(event, x, y, mask);
		Unknown_6C();
	} else if (((mask & 2) && Unknown_BE5F24 == this) || ((mask & 0x20) && Unknown_BE5F28 == this) || (mask & 0x1000)) {
		GadgetClass::Unknown_2C(event, x, y, mask);
	} else {
		return(false);
	}
	return(true);
}



/*
**	Reconstruction notes (GAD4, 2026-11-01). Client 0x66B770-0x66BA10, table 0xAFA890: no string, no assert.
**	Placement (D137, interim): it sits between gd_proxy.cpp (assert 'StickySave' at 0x66B420, its proxy's slots to
**	0x66B690) and gd_resize.cpp (from 0x66BA50), and its tables likewise (0xAFA7BC proxy, 0xAFA890 this, 0xAFA954
**	resize). gd_resize.cpp's FAIL_IF on line 64 leaves no room for it there; it is gd_proxy.cpp's tail or a file of
**	its own between the two. Its derived button (0x675240 / 0x675170) lives at 0x675xxx.
*/
