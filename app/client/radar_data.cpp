// radar_data.cpp: a client object's radar colours (0x7BB1D0-0x7BC030); notes at the end
#include <string>
#include "radar_data.h"
#include "sclient.h"
#include "client.h"
#include "co.h"
#include "gutter_data.h"
#include "client_object_events.h"
#include "unknown_signalbase.h"
#include "wwmath.h"
#include "..\shared\g_assert.h"

unsigned int Unknown_Get_Ticks();				// 0x4A3B40, the aux-data clock


















































// NOT EXACT: inline budget (the client calls GutterColorStruct::operator= 0x70AC60 four times, ours inlines the first)
// @0x7BB1D0
RadarDataClass::RadarDataClass(ClientObjectClass *co) :
	ClientObjectDataClass(co),
	Unknown08(0),
	Color(0.0f, 0.5f, 1.0f),
	Unknown78(false)
{

	G_ASSERT(co);
	Unknown_7BB960();
	Unknown_4F7EF0().Add(co, this, &RadarDataClass::Unknown_7BBA90, PlayerObjectRelationshipChangeEvent());
	Unknown_4F7EF0().Add(co, this, &RadarDataClass::Unknown_7BBC00, ClientGroupEvent());
	Unknown_7BB910();
	Unknown_7BB570();
}

// @0x7BB450
RadarDataClass::~RadarDataClass()
{
	Unknown_4F7EF0().Remove(ClientObject, this, &RadarDataClass::Unknown_7BBA90, PlayerObjectRelationshipChangeEvent());
	Unknown_4F7EF0().Remove(ClientObject, this, &RadarDataClass::Unknown_7BBC00, ClientGroupEvent());
}

// NOT EXACT: block layout and registers (the client's case 0 sits after the common tail, sharing case 1's last two stores)
// @0x7BB570
void RadarDataClass::Unknown_7BB570()
{
	ClientObjectClass *co = Peek_Client_Object();
	SClient *client = Peek_SClient();
	if (client && co && client->Unknown_74D8B0(co)) {
		Unknown_7BB960();
	}

	GutterColorStruct color;
	switch (Unknown08) {
		case 0:
			if (Peek_Client_Object() && Peek_Client_Object()->Unknown_700860()) {
				GutterColorStruct c = Colors.JumpStart;
				color = c;
			} else {
				GutterColorStruct c = Colors.NotTarget;
				color = c;
			}
			break;
		case 1:
		{
			GutterColorStruct c = Colors.Mouse;
			color = c;
		}
			break;
		case 2:
		{
			color = Colors.Target;
			float t = (float)fabs((float)(int)(Unknown_Get_Ticks() % 1000) * 0.001f - 0.5f) * 2.0f;
			Color.X = (color.Color2.X - color.Color1.X) * t + color.Color1.X;
			Color.Y = (color.Color2.Y - color.Color1.Y) * t + color.Color1.Y;
			Color.Z = (color.Color2.Z - color.Color1.Z) * t + color.Color1.Z;
			return;
		}
	}
	if (Unknown_Get_Ticks() % 2000 > 1000) {
		Color = color.Color1;
	} else {
		Color = color.Color2;
	}
}

// @0x7BB8C0
void RadarDataClass::Unknown_7BB8C0(int state)
{
	if (Unknown08 != state) {
		GutterDataClass *gutter = Get_Gutter_Data(Peek_Client_Object(), false);
		if (gutter) {
			gutter->Set_Sort_Group(Unknown08, state);
		}
		Unknown08 = state;
	}
}

// @0x7BB910
void RadarDataClass::Unknown_7BB910()
{
	SClient *client = Peek_SClient();





















































	FAIL_IF(!client) return;
	Unknown78 = Unknown_70BAC0(client, Peek_Client_Object());
}

// @0x7BB960
void RadarDataClass::Unknown_7BB960()
{
	SClient *client = Peek_SClient();
	FAIL_IF(!client) return;
	Colors = Unknown_70B0A0(client, Peek_Client_Object());
	Get_Client()->ObjectsChanged = true;
}

// @0x7BBA90
void RadarDataClass::Unknown_7BBA90(PlayerObjectRelationshipChangeEvent *event, ClientObjectClass *object)
{




	FAIL_IF(object != Peek_Client_Object()) return;
	Unknown_7BB960(); }
// NOT EXACT: inline budget (the client calls GutterColorStruct::operator= 0x70AC60 for the last of the four, ours inlines all four)
// @0x7BBC00
void RadarDataClass::Unknown_7BBC00(ClientGroupEvent *event, ClientObjectClass *object)
{
	FAIL_IF(object != Peek_Client_Object()) return;
	Unknown_7BB960();
	Unknown_7BB910();
}

/*
**	Notes
**
**	The client's __FILE__ 0xB9F070. The object runs from 0x7BB1D0 (a PermStringInitClass static's initialiser at
**	0x7BB180 precedes it) to the callbacks' template members, 0x7BC02B.
*/
