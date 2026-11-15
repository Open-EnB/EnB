// Earth & Beyond client (reconstruction): a gauge drawn with screen lines (client 0x686010-0x68672F).
// File PROVISIONAL (D137): see unknown_686010.h.
#include <string>
#include "unknown_686010.h"
#include "line2d.h"
#include "sclient.h"

// @0x686010
Unknown686010Class::Unknown686010Class(UIViewClass *view, const Vector4 &color, const Vector2 &pos, const Vector2 &size, int type, float width) :
	View(view),
	Level(0)
{
	Frame[0] = new Line2DClass(color, Vector2(0.0f, 0.0f), Vector2(1.0f, 1.0f), width);
	Frame[1] = new Line2DClass(color, Vector2(0.0f, 0.0f), Vector2(1.0f, 1.0f), width);
	Frame[2] = new Line2DClass(color, Vector2(0.0f, 0.0f), Vector2(1.0f, 1.0f), width);
	Bar = new Line2DClass(Vector4(color.Y, color.Z, color.X, color.W), Vector2(0.0f, 0.0f), Vector2(1.0f, 1.0f), width);
	Unknown_6863E0(pos, size, type, width);
	Unknown_6865E0(-1.0f, 0.0f);
	Frame[0]->Set_Hidden(false);
	Frame[1]->Set_Hidden(false);
	Frame[2]->Set_Hidden(false);
	Bar->Set_Hidden(false);
	Frame[0]->Add((SceneClass *)View->Unknown_3C());
	Frame[1]->Add((SceneClass *)View->Unknown_3C());
	Frame[2]->Add((SceneClass *)View->Unknown_3C());
	Bar->Add((SceneClass *)View->Unknown_3C());
}

// @0x686350
Unknown686010Class::~Unknown686010Class()
{
	Frame[0]->Remove();
	Frame[1]->Remove();
	Frame[2]->Remove();
	Bar->Remove();
	Frame[0]->Release_Ref();
	Frame[1]->Release_Ref();
	Frame[2]->Release_Ref();
	Bar->Release_Ref();
}

// @0x6863E0
void Unknown686010Class::Unknown_6863E0(const Vector2 &pos, const Vector2 &size, int type, float width)
{
	Position = pos;
	Size = size;
	if (type != 0) {
		Type = type;
	}
	Vector2 p0, p1, p2, p3;
	switch (Type) {
		case 0:
		case 1:
			p0.X = pos.X;
			p0.Y = pos.Y;
			p1.X = pos.X + size.X;
			p1.Y = pos.Y;
			p2.X = pos.X + size.X;
			p2.Y = size.Y + pos.Y;
			p3.X = pos.X;
			p3.Y = size.Y + pos.Y;
			break;
		case 2:
			p0.X = pos.X + size.X;
			p0.Y = pos.Y;
			p1.X = pos.X;
			p1.Y = pos.Y;
			p2.X = pos.X;
			p2.Y = size.Y + pos.Y;
			p3.X = pos.X + size.X;
			p3.Y = size.Y + pos.Y;
			break;
		case 3:
			p0.X = pos.X;
			p0.Y = size.Y + pos.Y;
			p1.X = pos.X;
			p1.Y = pos.Y;
			p2.X = pos.X + size.X;
			p2.Y = pos.Y;
			p3.X = pos.X + size.X;
			p3.Y = size.Y + pos.Y;
			break;
		case 4:
			p0.X = pos.X;
			p0.Y = pos.Y;
			p1.X = pos.X;
			p1.Y = size.Y + pos.Y;
			p2.X = pos.X + size.X;
			p2.Y = size.Y + pos.Y;
			p3.X = pos.X + size.X;
			p3.Y = pos.Y;
			break;
	}
	Frame[0]->Set(p0, p1, width);
	Frame[1]->Set(p1, p2, width);
	Frame[2]->Set(p2, p3, width);
	Unknown_6865E0(-1.0f, 0.0f);
}

// @0x6865E0
bool Unknown686010Class::Unknown_6865E0(float level, float width)
{
	if (level != Level) {
		if (level != -1.0) {
			Level = (level < 0.0f) ? 0.0f : ((level > 1.0f) ? 1.0f : level);
		}
		Vector2 from, to;
		switch (Type) {
			case 1:
			case 2:
				to.X = Size.X + Position.X;
				from.X = Position.X;
				from.Y = to.Y = Size.Y * Level + Position.Y;
				break;
			case 3:
			case 4:
				from.X = to.X = Size.X * Level + Position.X;
				from.Y = Position.Y;
				to.Y = Size.Y + Position.Y;
				break;
		}
		Bar->Set(from, to, width);
		return(true);
	}
	return(false);
}

/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**
**	Unknown686010Class (unknown_686010.h): 0x686010-0x68672F, after the action entries' object 0x685E80 / 0x685F90
**	and before intrlist.cpp (0x686740). No table: +0x00 holds the view. The constructor news four 0xF4-byte
**	Line2DClass (0x6DF890, each from (0, 0) to (1, 1); one EH state a new), the fourth with the color's first three
**	components rotated (y, z, x, w: the client's copy), places the frame (0x6863E0) and the level (0x6865E0 with -1),
**	shows the lines (RenderObjClass's Set_Hidden, slot +0x184) and adds them to the view's scene (slot +0x3C).
**	0x6863E0 sets the three frame lines through four corners by the type (1-4; 0 as 1); 0x6865E0 clamps the level to
**	0..1 (-1 keeps it; the same level answers false) and lays the fourth line across the frame at it. Its point
**	members are assigned one by one (Vector2 temporaries give a larger frame). The object ends with <string>'s
**	guarded static 0x686710 (guard 0xC416FC, atexit 0x42B320).
*/
