// Earth & Beyond client (reconstruction): app\shared\auxdata\delta_time.cpp. Notes at the end of the file.
#include <time.h>
#include "delta_template.h"
#include "linearbu.h"
#include "..\duration.h"
#include "..\kv_encoder_base.h"
#include "..\kv_pair.h"
#include "..\unknown_debug.h"

// (the client's file has no assert before line 31)

// The per-class global (0xBE3AA4) and the constructor (0x4A8150): an explicit specialisation with
// delta_template.h's body (the header declares the instance `extern template`, and VC6 can't
// instantiate a constructor alone: C2909)
template<> int DeltaTemplateClass<DurationClass>::UnknownGlobal = 0;

template<> DeltaTemplateClass<DurationClass>::DeltaTemplateClass(const char *name, const DurationClass &value) :
	DeltaClass(name),
	Value(value)
{
	m_Unknown04 = UnknownGlobal;
}


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


// the time since the first call (0x4A3B40; delta_interpolator.cpp uses it too)
unsigned int Unknown_Get_Ticks();

// The time field's value as the key/value pairs see it (vtable 0xAEB244, size 0xC): whole seconds
// of wall-clock time (time()), 0 for a zero time. The name is a guess (D10).
class DurationAccessorClass : public KVAccessorClass<unsigned int>  // NAME 2: describes what it does
{
public:
	DurationAccessorClass(DurationClass *duration) : Duration(duration), Seconds(0) {}
	virtual const unsigned int &Get();													// 0x4A83B0
	virtual void Set(const unsigned int &value);										// 0x4A84E0
	virtual KVAccessorClass<unsigned int> *Clone() const;							// 0x4A8550

	DurationClass *Duration;			// +0x04  // NAME 2: describes what it does
	unsigned int Seconds;				// +0x08 Get's result  // NAME 2: describes what it does
};

static KVAccessorClass<unsigned int> *Make_Accessor(DurationClass *duration);  // NAME 2: describes what it does

template<> void DeltaTemplateClass<DurationClass>::Unknown_00(DeltaChildVisitorClass *visitor)
{
	visitor->Add_Value(m_Unknown80, Name.Get_String(), Make_Accessor(&Value));
}

static KVAccessorClass<unsigned int> *Make_Accessor(DurationClass *duration)
{
	return new DurationAccessorClass(duration);
}

// NOT EXACT: the zero time is a function-local static the client destroys at exit (atexit
// 0x4A8590: its DurationClass has a destructor ours lacks); the 64-bit arithmetic follows the
// client's order
const unsigned int &DurationAccessorClass::Get()
{
	static DurationClass zero(0);
	if (*Duration == zero) {
		Seconds = 0;
		return Seconds;
	}
	unsigned int ticks = Unknown_Get_Ticks();
	__int64 now = (__int64)time(NULL) * 1000;
	DurationClass seconds((now + Duration->Time - ticks) / 1000);
	Seconds = seconds.As_Int();
	return Seconds;
}

// NOT EXACT: the client keeps time()'s 0 argument in ebx and adds the ticks' zero high half from
// it; VC6 here pushes an immediate (tried: other orders, a DurationClass temporary)
void DurationAccessorClass::Set(const unsigned int &value)
{
	unsigned int ticks = Unknown_Get_Ticks();
	int now = time(NULL);
	Duration->Time = ((__int64)value - now) * 1000 + ticks;
}

KVAccessorClass<unsigned int> *DurationAccessorClass::Clone() const
{
	return new DurationAccessorClass(Duration);
}


/*
**	Reconstruction notes
**
**	client.exe: the time field's Read (0x4A81D0) and Write (0x4A8270), DeltaTemplateClass<DurationClass>
**	specialisations (slots 0x2C and 0x30). The value is 8 bytes at +0x88 but goes on the wire as 4:
**	Read stores 0 for 0, otherwise the value less the base Read is given; Write clamps with
**	DurationClass::As_Int (duration.h line 190). The asserts' expression is "0" (0xB6E944), on the
**	client's lines 31 and 58. The constructor 0x4A8150 (vtable 0xAEB1EC) and the visitor slot
**	0x4A8330 (Unknown_00: a key/value pair through DurationAccessorClass, vtable 0xAEB244, made by
**	the static 0x4A8370; its Get 0x4A83B0, Set 0x4A84E0, Clone 0x4A8550), with the unsigned int
**	pair they instantiate (KVEncoderBaseClass::Add_Value 0x4A8630, KVPairTemplateClass 0x4A8770-
**	0x4A8990, vtable 0xAEB26C). Not rebuilt yet: 0x4A8060 (vtable 0xAEB13C, statics with atexit,
**	permstr.cpp 0x4AE1E0).
*/
