// Earth & Beyond client (reconstruction): app\client\ui_mfd.cpp, the multi-function display (per the client's asserts)
#include <stdio.h>
#include <string.h>
#include <vector>
#include "sclient.h"
#include "client.h"
#include "rendobj.h"
#include "co.h"
#include "ui_mfd.h"

Unknown5BE4F0Class::Unknown5BE4F0Class(UIViewClass *view, RenderObjClass *robj, const LayerClass &layer)
	: UIMFDClass(view, robj, layer, 0)
{
}

void Unknown5BE4F0Class::Unknown_54()
{
	UIMFDClass::Unknown_54();
	Items.push_back(new Unknown5C3030Class(this));
	Items.push_back(new Unknown5C3920Class(this));
}

Unknown5BE4F0Class::~Unknown5BE4F0Class()
{
}

void Unknown5BE4F0Class::Unknown_90()
{
	if (Object != NULL) {
		Unknown_5BCDE0(Lines[0], 13);
		Unknown_5BCDE0(Lines[1], 5);
	} else {
		Unknown_5BCDE0(Lines[0], 0);
		Unknown_5BCDE0(Lines[1], 0);
	}
}

Unknown5BE980Class::Unknown5BE980Class(UIViewClass *view, RenderObjClass *robj, const LayerClass &layer)
	: UIMFDClass(view, robj, layer, 1)
{
}

void Unknown5BE980Class::Unknown_54()
{
	UIMFDClass::Unknown_54();
	Items.push_back(new Unknown5C2CD0Class(this));
	Items.push_back(new Unknown5C3650Class(this));
}

Unknown5BE980Class::~Unknown5BE980Class()
{
}

// NOT EXACT: 0x5BECF0, the case block's first load of each line's Font comes before the 'lea' ([esi+0x128]) where the client
// loads it after the stores ([edi+0x40]); the duplicated default and the zero in ebx do match (attempts.csv)
void Unknown5BE980Class::Unknown_90()
{
	if (Object != NULL) {
		switch (Object->Type) {
			case 0:
			case 1:
			case 2:
			case 3:
			case 4:
			case 9:
			case 10:
			case 11:
			case 12:
			case 0x19:
			case 0x25:
			case 0x26:
				Unknown_5BCDE0(Lines[0], 1);
				Unknown_5BCDE0(Lines[1], 3);
				break;
			default:
				Unknown_5BCDE0(Lines[0], 0);
				Unknown_5BCDE0(Lines[1], 0);
				break;
		}
	} else {
		Unknown_5BCDE0(Lines[0], 0);
		Unknown_5BCDE0(Lines[1], 0);
	}
}

/*
**	Notes (MF1, 2026-11-06)
**
**	The client's object runs 0x5BB660-0x5BF4B0: 0x5BD540 (ui_mfd.cpp's assert l.879) calls 0x5BF210 and 0x5BBCF0 (between
**	asserts l.184 and l.194) calls 0x5BEED0 / 0x5BF190 / 0x5BF1D0 / 0x5BF280, the COMDATs that follow 0x5BE4F0-0x5BEE60; a
**	second object's functions could not sit before the first object's COMDATs, so the two subclasses are this file's.
*/
