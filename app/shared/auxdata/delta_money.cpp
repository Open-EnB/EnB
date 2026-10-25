// Earth & Beyond client (reconstruction): app\shared\auxdata\delta_money.cpp. Notes at the end of the file.
#include "delta_template.h"
#include "linearbu.h"
#include "..\money.h"
#include "..\unknown_debug.h"

// (lines 7-64 hold the rest of the client's file, not rebuilt yet: the asserts below are on the
// client's lines 69 and 86)

// The per-class global (0xBE3A94) and the constructor (0x4A7080): an explicit specialisation with
// delta_template.h's body, as the time field's (delta_time.cpp); aux_roots.h declares the instance
// `extern template`, so the roots call it out of line as the client does
template<> int DeltaTemplateClass<MoneyClass>::UnknownGlobal = 0;

template<> DeltaTemplateClass<MoneyClass>::DeltaTemplateClass(const char *name, const MoneyClass &value) :
	DeltaClass(name),
	Value(value)
{
	m_Unknown04 = UnknownGlobal;
}












































template<> bool DeltaTemplateClass<MoneyClass>::Read(LinearBufferClass &buf, const DurationClass &)
{
	__int64 value;
	if (buf.Read(&value, sizeof(value)) != sizeof(value)) {
		G_ASSERT(0);
		return false;
	}
	Value.m_Unknown00 = value;
	return true;
}








template<> bool DeltaTemplateClass<MoneyClass>::Write(LinearBufferClass &buf)
{
	if (buf.Write(&Value, sizeof(Value)) != sizeof(Value)) {
		G_ASSERT(0);
		return false;
	}
	return true;
}


/*
**	Reconstruction notes
**
**	client.exe: the money field's Read (0x4A7470) and Write (0x4A74F0), DeltaTemplateClass<MoneyClass>
**	specialisations (slots 0x2C and 0x30; the generic ones are delta_template.h's, e.g. the float
**	field's 0x4A6DF0/0x4A6E60). The value is 8 bytes at +0x88. The asserts' expression is "0"
**	(0xB6E944), on the client's lines 69 and 86. Not rebuilt yet: the neighbours (whose file
**	isn't certain; 0x4A7340-0x4A7440: an 8-byte accessor object, vtable 0xAEB034, and its abstract base 0xAEB048;
**	0x4A7540-0x4A7700: vtable 0xAEB010, a kv_pair.cpp class 0x4B7EB0).
*/
