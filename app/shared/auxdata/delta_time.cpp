// Earth & Beyond client (reconstruction): app\shared\auxdata\delta_time.cpp. Notes at the end of the file.
#include "delta_template.h"
#include "linearbu.h"
#include "..\duration.h"
#include "..\unknown_debug.h"

// (lines 7-23 hold the rest of the client's file, not rebuilt yet: the asserts below are on the
// client's lines 31 and 58)
















template<> bool DeltaTemplateClass<DurationClass>::Read(LinearBufferClass &buf, const DurationClass &base)
{
	int value;
	if (buf.Read(&value, sizeof(value)) != sizeof(value)) {


		G_ASSERT(0);
		return false;
	}
	if (value == 0) {
		Value.Time = 0;
	} else {
		Value.Time = (__int64)value - base.Time;
	}
	return true;
}












template<> bool DeltaTemplateClass<DurationClass>::Write(LinearBufferClass &buf)
{
	int value = Value.As_Int();
	if (buf.Write(&value, sizeof(value)) != sizeof(value)) {

		G_ASSERT(0);
		return false;
	}
	return true;
}


/*
**	Reconstruction notes
**
**	client.exe: the time field's Read (0x4A81D0) and Write (0x4A8270), DeltaTemplateClass<DurationClass>
**	specialisations (slots 0x2C and 0x30). The value is 8 bytes at +0x88 but goes on the wire as 4:
**	Read stores 0 for 0, otherwise the value less the base Read is given; Write clamps with
**	DurationClass::As_Int (duration.h line 190). The asserts' expression is "0" (0xB6E944), on the
**	client's lines 31 and 58. Not rebuilt yet: the neighbours 0x4A8060 (vtable 0xAEB13C, statics
**	with atexit, permstr.cpp 0x4AE1E0), 0x4A8150 (a constructor: DeltaClass 0x4A4D40, vtable
**	0xAEB1EC), 0x4A81A0, 0x4A8330, 0x4A8370 (vtable 0xAEB244).
*/
