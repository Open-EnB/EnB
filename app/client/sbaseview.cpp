#include "sbaseview.h"
#include "..\shared\unknown_debug.h"
#include "..\..\ww3d2\rendobj.h"
#include "..\..\ww3d2\scene.h"

// fontsys.cpp's text object (a render object), as far as the starbase view calls it
class FontSys : public RenderObjClass	// NAME 3: the client's profile label 'FontSys::Change_Text' (0x6CDBC0)
{
public:
	void Change_Text(const char *text, int unknown);	// 0x6CDBC0  // NAME 1 [text, unknown]: what 0x6A0E80 passes
};

SBaseViewUnknown1B4Class *Unknown_6987A0(LoungeNPCPacketClass *packet);	// 0x6987A0 (sbase_info.cpp)  // NAME 1 [packet]: what 0x6A0FD0 passes

// 0x69D930: ViewRObjs[type] out of its scene (0 the view's +0x1C, 1 +0x70, 2 +0x78), then released
void SBaseViewClass::Unknown_69D930(int type)
{
	if (ViewRObjs[type]) {
		switch (type) {
			case 0:
				Unknown1C->Remove_Render_Object(ViewRObjs[0].Peek());
				break;
			case 1:
				Unknown70->Remove_Render_Object(ViewRObjs[1].Peek());
				break;
			case 2:
				Unknown78->Remove_Render_Object(ViewRObjs[2].Peek());
				break;
		}
		ViewRObjs[type] = NULL;
	}
}

// 0x69DB50: ViewRObjs[type] released
void SBaseViewClass::Unknown_69DB50(int type)
{
	if (ViewRObjs[type]) {
		ViewRObjs[type] = NULL;
	}
}

// 0x69DC40
void SBaseViewClass::Unknown_69DC40()
{
	Unknown_69D9D0(2, "FE_COMP1");
}

// 0x69DC60
void SBaseViewClass::Unknown_69DC60()
{
	Unknown_69D930(2);
}

// 0x69F440
void SBaseViewClass::Unknown_69F440()
{
	Unknown1B0 = true;
}

// 0x69F460
void SBaseViewClass::Unknown_69F460(int arg)
{
	Unknown54 = arg;
	Unknown58 = Unknown50;
}

// 0x6A0540: the render objects out of their scenes
void SBaseViewClass::Unknown_6A0540()
{
	if (Unknown68) {
		Unknown68->Unknown00->Remove();
	}
	if (Unknown1CC) {
		Unknown1CC->Remove();
	}
	if (Unknown1D0) {
		Unknown1D0->Remove();
	}
}

// 0x6A0E00: the text object away
void SBaseViewClass::Unknown_6A0E00()
{
	if (Unknown1B8) {
		Unknown1B8->Remove();
		Unknown1B8 = NULL;
	}
}

// 0x6A0E50: the text object hidden
void SBaseViewClass::Unknown_6A0E50()
{
	if (Unknown1B8) {
		Unknown1B8->Set_Hidden(1);
	}
}

// 0x6A0E80: the text object's text, shown
void SBaseViewClass::Unknown_6A0E80(const char *text)
{
	if (Unknown1B8) {
		Unknown1B8->Change_Text(text, 0);
		Unknown1B8->Set_Hidden(0);
	}
}

// 0x6A0FD0: LOUNGE_NPC 82
void SBaseViewClass::Unknown_6A0FD0(LoungeNPCPacketClass *packet)
{
	Unknown1B4 = Unknown_6987A0(packet);
}

// 0x6A20C0: the +0x70 scene's fog on
void SBaseViewClass::Unknown_6A20C0(const Vector3 &color, float start, float end)
{
	Unknown70->Set_Fog_Enable(true);
	Unknown70->Set_Fog_Color(color);
	Unknown70->Set_Fog_Range(start, end);
}

// 0x6A2100: the +0x70 scene's fog off
void SBaseViewClass::Unknown_6A2100()
{
	Unknown70->Set_Fog_Enable(false);
}

// 0x6A23E0
void SBaseViewClass::Unknown_6A23E0(bool a, bool b, bool c, bool d, bool e)
{
	Unknown1EC = a;
	Unknown1ED = b;
	Unknown1EE = c;
	Unknown1EF = d;
	Unknown1F0 = e;
}

// 0x6A2430
void SBaseViewClass::Unknown_6A2430(bool *a, bool *b, bool *c, bool *d, bool *e)
{
	*a = Unknown1EC;
	*b = Unknown1ED;
	*c = Unknown1EE;
	*d = Unknown1EF;
	*e = Unknown1F0;
}

/*
**	Notes (UI1, 2026-10-29)
**
**	sbaseview.cpp is the starbase view (SBaseViewClass, sbaseview.h; SClient +0x135C), client 0x69CDE0-0x6A2530
**	(appfile.py show app\client\sbaseview.cpp; __FILE__ e:\build\beta\G\app\Client\sbaseview.cpp, asserts on the
**	client's lines 738 ('ViewRObjs[type]', 0x69D9D0), 1261 / 1262 (0x6A0590) and 1667 (0x6A2480)). Written so far:
**	the short members, none with an assert, so their lines are free; the asserting functions go on their lines
**	(D25) when they are written. The constructor and destructor are not in the window (the view is made outside
**	it); the window's large functions (0x69D030, 0x69F8F0, 0x6A0590, 0x6A1180) are not rebuilt.
*/
