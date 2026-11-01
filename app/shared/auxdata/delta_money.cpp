// Earth & Beyond client (reconstruction): app\shared\auxdata\delta_money.cpp. Notes at the end of the file.
#include "delta_template.h"
#include "linearbu.h"
#include "..\money.h"
#include "..\unknown_debug.h"

// (lines 7-64 hold the rest of the client's file, not rebuilt yet: the asserts below are on the
// client's lines 69 and 86)

// money.cpp's (not rebuilt), declared here only: in money.h / strconv_lib.h any new declaration moves
// aux_roots.cpp's inline budget (six of its functions, TK5); the comparisons are free functions
const char *Type_Name(MoneyClass value);	// 0x4F0AB0 (the value pushed, 8 bytes)
bool operator==(const MoneyClass &a, const MoneyClass &b);	// 0x4F0600
bool operator!=(const MoneyClass &a, const MoneyClass &b);	// 0x4F0640 (MoneyClass's member != stands in elsewhere)

// The pair's type name passes the value (0x4A7600, as the guild rank's 0x708240) and its change test
// calls money.cpp's != (0x4A7670); Set_Value tests money.cpp's == (0x4A7150, inlined in the accessor's
// Set 0x4A7360). Specialisations: the generic ones use MoneyClass's inline member !=
template<> inline const char *KVPairTemplateClass<MoneyClass>::Get_Type_Name() const { return Type_Name(Value); }
template<> inline bool KVPairTemplateClass<MoneyClass>::Is_Changed() const { return ::operator!=(Value, Accessor->Get()); }
template<> void DeltaTemplateClass<MoneyClass>::Set_Value(const MoneyClass &value) { if (!(Value == value)) { Value = value; Unknown_34(this); } }
// The per-class global (0xBE3A94) and the constructor (0x4A7080): an explicit specialisation with
// delta_template.h's body, as the time field's (delta_time.cpp); aux_roots.h declares the instance
// `extern template`, so the roots call it out of line as the client does
template<> int DeltaTemplateClass<MoneyClass>::UnknownGlobal = 0;

template<> DeltaTemplateClass<MoneyClass>::DeltaTemplateClass(const char *name, const MoneyClass &value) :
	DeltaClass(name),
	Value(value)
{
	Unknown04 = UnknownGlobal;
}
































template<> bool DeltaTemplateClass<MoneyClass>::Read(LinearBufferClass &buf, const DurationClass &)
{
	__int64 value;
	if (buf.Read(&value, sizeof(value)) != sizeof(value)) {
		G_ASSERT(0);
		return false;
	}
	Value.Unknown00 = value;
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

// Every member out of line (the destructor 0x4A7100 and Set_Value 0x4A7150 have no caller): an
// explicit instantiation
template class DeltaTemplateClass<MoneyClass>;


/*
**	Reconstruction notes
**
**	client.exe: the money field's Read (0x4A7470) and Write (0x4A74F0), DeltaTemplateClass<MoneyClass>
**	specialisations (slots 0x2C and 0x30; the generic ones are delta_template.h's, e.g. the float
**	field's 0x4A6DF0/0x4A6E60). The value is 8 bytes at +0x88. The asserts' expression is "0"
**	(0xB6E944), on the client's lines 69 and 86. Not rebuilt yet: the neighbours (whose file
**	isn't certain; 0x4A7340-0x4A7440: an 8-byte accessor object, vtable 0xAEB034, and its abstract base 0xAEB048;
**	0x4A7540-0x4A7700: vtable 0xAEB010, a kv_pair.cpp class 0x4B7EB0).
**	The pair's Get_Type_Name 0x4A7600 and Is_Changed 0x4A7670, Set_Value 0x4A7150 (an explicit
**	instantiation emits it; inlined in the accessor's Set 0x4A7360) call money.cpp's by-value Type_Name
**	and free == / != (0x4F0AB0, 0x4F0600, 0x4F0640). They belong in money.h / strconv_lib.h and the
**	generic templates, but any declaration added there (or a member in delta_template.h) moves six of
**	aux_roots.cpp's budget-bound functions (0x7014B0 0x701D70 0x7045F0 0x7048A0 0x705860 0x705A10):
**	declared here, with specialisations, until that balance is understood (TK5). Not exact: the explicit
**	instantiation's 0x4A7120 (Value = v; Unknown_34) and operator= 0x4A71A0 (two members delta_template.h
**	would need), Get_Value_String 0x4A7630 (order: the value's halves; attempts.csv).
*/
