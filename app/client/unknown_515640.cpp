// Earth & Beyond client (reconstruction): an interface with a gauge of the avatar's height (client 0x515640-0x515A53).
// File PROVISIONAL (D137): see unknown_515640.h.
#include "unknown_515640.h"
#include "client.h"

// @0x515640
Unknown515640Class::Unknown515640Class(UIViewClass *view, int unknown) :
	InterfaceClass(view, unknown),
	Unknown84(NULL)
{
}

// @0x5157A0
Unknown515640Class::~Unknown515640Class()
{
	if (Unknown84 != NULL) {
		delete Unknown84;
		Unknown84 = NULL;
	}
}

// @0x515840
void Unknown515640Class::Unknown_58()
{
	Unknown84 = new Unknown686010Class(View, Vector4(0.0f, 0.6f, 0.0f, 1.0f), Vector2(0.96f, 0.1f), Vector2(0.03f, 0.3f), 1, 2.4f);
	InterfaceClass::Unknown_58();
}

// @0x515930
void Unknown515640Class::Unknown_5C()
{
	if (Unknown84 != NULL) {
		delete Unknown84;
		Unknown84 = NULL;
	}
	InterfaceClass::Unknown_5C();
}

// @0x515980
void Unknown515640Class::Unknown_60()
{
	ClientObjectClass *avatar = ((ClientClass *)Client)->Get_Avatar();
	if (Unknown84 != NULL && avatar != NULL) {
		float low = ((ClientClass *)Client)->Unknown10D4;
		float high = ((ClientClass *)Client)->Unknown10D8;
		float t = (avatar->Unknown_24().Z - low) / (high - low);
		Unknown84->Unknown_6865E0(1.0f - t, 0);
	}
}
