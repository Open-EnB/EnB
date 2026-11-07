// Earth & Beyond client (reconstruction): app\client\slot_data.cpp, what a slot's data derives from
// (name assumed: no __FILE__; the class is the client's RTTI, the link order puts it between skill_render and slot_data_render)
#include "slot_data.h"

// 0x6A7420
SlotDataClass::SlotDataClass(SClient *client) :
	Client(client),
	AuxData(0),
	Unknown30(0),
	Unknown40(0)
{
}

// 0x6A7530
SlotDataClass::~SlotDataClass()
{
	Unknown30.clear();
	Unknown40.clear();
	Client = 0;
	AuxData = 0;
}

// 0x6A76A0
void SlotDataClass::Unknown_Watch_0C()
{
	AuxData = 0;
	Unknown_08(NULL);
	Unknown_24();
	Unknown_6A7AB0().emit(this, SlotDataEventDataChange(2));
}

// NOT EXACT: shape, the client lays out the second emit before the shared failure exit (three forms of the branches)
// 0x6A7730
bool SlotDataClass::Unknown_6A7730()
{
	if (AuxData) {
		if (!Unknown_18()) {
			goto failed;
		}
	} else {
		AuxData = Unknown_1C();
		if (!AuxData) {
			goto failed;
		}
	}
	if (Unknown_20()) {
		Unknown_6A7AB0().emit(this, SlotDataEventDataChange(2));
		return(true);
	}
	Unknown_6A7AB0().emit(this, SlotDataEventDataChange(1));
	return(true);
failed:
	return(false);
}

// NOT EXACT: inlines 0x6A7730, the same shape
// 0x6A7860
void SlotDataClass::Unknown_6A7860()
{
	AuxData = 0;
	Unknown_08(NULL);
	Unknown_24();
	Unknown_6A7730();
}

// 0x6A7990
int SlotDataClass::Unknown_6A7990()
{
	return(Unknown40.size());
}

// 0x6A79C0
int SlotDataClass::Unknown_6A79C0(unsigned int index)
{
	if (index >= Unknown30.size()) {
		return(-1);
	}
	return(Unknown30[index]);
}

// 0x6A7A00
int SlotDataClass::Unknown_6A7A00(unsigned int index)
{
	if (index >= Unknown40.size()) {
		return(-1);
	}
	return(Unknown40[index]);
}

// 0x6A7A40
void SlotDataClass::Unknown_24()
{
	Unknown30.clear();
	Unknown40.clear();
}

// 0x6A7AB0
SignalMap &SlotDataClass::Unknown_6A7AB0()
{
	static SignalMap signal;
	return(signal);
}

/*
**	Notes.
**	Object 0x6A7420-0x6A7B4F, between skill_render.cpp's <string> initialiser 0x6A73F0 and its own 0x6A7B20.
*/
