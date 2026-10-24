/*
**	Earth & Beyond client (reconstruction): link-time stubs for app.lib (D44, docs/milestones.md 3.2)
**
**	The functions app.lib's objects call that the tree doesn't have yet (build.py unresolved,
**	2026-10-24). Each stub logs its first call (stub_log.h) and does as little as it can; the client
**	address, where known, is the real function to rebuild. The auxdata ones are M1.2's to-do list
**	(milestones.md 2.3). The three globals are data, so they can't log: they start at 0.
*/

#include <windows.h>
#include <stdio.h>
#include <stdarg.h>
#include "stub_log.h"

#include "unknown_debug.h"
#include "kv_list.h"
#include "kv_pair_string.h"
#include "lookup.h"
#include "delta.h"
#include "delta_string.h"
#include "duration.h"
#include "delta_template.h"

// ------------------------------------------------------------------------- E&B debug hooks

// 0x4E9410. An assert is worth a line each time, not only the first.
void Unknown_Assert_Failed(const char *expr, const char *file, int line)
{
	STUB_CALLED("Unknown_Assert_Failed (0x4E9410)");
	Stub_Log_Line("G_ASSERT failed: %s, %s(%d)", expr, file, line);
}

// 0x4E9450
void Unknown_Fail_If_Report(const char *expr, const char *file, int line)
{
	STUB_CALLED("Unknown_Fail_If_Report (0x4E9450)");
	Stub_Log_Line("FAIL_IF: %s, %s(%d)", expr, file, line);
}

// permstr.cpp's statistics line
void Unknown_Debug_Printf(const char *format, ...)
{
	STUB_CALLED("Unknown_Debug_Printf");
	char text[1024];
	va_list va;
	va_start(va, format);
	_vsnprintf(text, sizeof(text) - 1, format, va);
	va_end(va);
	text[sizeof(text) - 1] = 0;
	Stub_Log_Line("Unknown_Debug_Printf: %s", text);
}

// delta_interpolator.cpp's clock (a DurationClass tick count). The real one is not located yet;
// the system's millisecond count stands in.
unsigned int Unknown_Get_Ticks()
{
	STUB_CALLED("Unknown_Get_Ticks");
	return GetTickCount();
}

// ------------------------------------------------------------------------- globals (data)

int DeltaUnknownGlobal = 0;					// delta.h
int DeltaCompoundUnknownGlobal = 0;			// delta.h
int DeltaStringClass::UnknownGlobal = 0;	// 0xBE3A9C

// ------------------------------------------------------------------------- auxdata (M1.2)

// 0x4AC250 (the members are default-constructed, so the object is usable, but empty)
LookupClass::LookupClass(DeltaCompoundClass *owner)
{
	STUB_CALLED("LookupClass::LookupClass (0x4AC250)");
}

// 0x4B6760
void KVListClass::Add(KVPairClass *item)
{
	STUB_CALLED("KVListClass::Add (0x4B6760)");
}

// declared in encode.cpp (the client's header is not known); 0x4A9C30
class RecordTableClass
{
public:
	void Add(DeltaClass **list, int id);
};

void RecordTableClass::Add(DeltaClass **list, int id)
{
	STUB_CALLED("RecordTableClass::Add (0x4A9C30)");
}

// 0x4B7FC0
KVPairStringClass::KVPairStringClass(const char *key, StringValueClass *accessor, bool even, bool odd) :
	KVPairClass(key, even, odd),
	Accessor(accessor),
	m_Unknown10(NULL)
{
	STUB_CALLED("KVPairStringClass::KVPairStringClass (0x4B7FC0)");
}

// KVPairTemplateClass<float> (vtable 0xAE7860): the destructor and six virtuals
KVPairTemplateClass<float>::~KVPairTemplateClass()
{
	STUB_CALLED("KVPairTemplateClass<float>::~KVPairTemplateClass");
}

void KVPairTemplateClass<float>::Unknown_04()
{
	STUB_CALLED("KVPairTemplateClass<float>::Unknown_04");
}

const char *KVPairTemplateClass<float>::Unknown_08() const
{
	STUB_CALLED("KVPairTemplateClass<float>::Unknown_08");
	return "";
}

void KVPairTemplateClass<float>::Unknown_0C()
{
	STUB_CALLED("KVPairTemplateClass<float>::Unknown_0C");
}

void KVPairTemplateClass<float>::Unknown_10()
{
	STUB_CALLED("KVPairTemplateClass<float>::Unknown_10");
}

void KVPairTemplateClass<float>::Unknown_14()
{
	STUB_CALLED("KVPairTemplateClass<float>::Unknown_14");
}

void KVPairTemplateClass<float>::Unknown_18()
{
	STUB_CALLED("KVPairTemplateClass<float>::Unknown_18");
}

// The per-class global (time: 0xBE3AA4, data), specialised before the constructor uses it; the
// constructor's vtable also needs the visitor slot, below (the `extern template` keeps both out of
// the header's instantiation).
template<> int DeltaTemplateClass<DurationClass>::UnknownGlobal = 0;

// DeltaTemplateClass<DurationClass>'s constructor (the client's 0x4A8150). Not a stub: the body is
// delta_template.h's, repeated; delta_interpolator.cpp declares the instance `extern template`
// (VC6's C4231, as the client inlines it there) and delta_time.cpp, which holds it in the client,
// doesn't instantiate it yet. VC6 can't instantiate the constructor alone (C2909) nor the class after that
// `extern` (C2949), so this is an explicit specialisation with the template's body (same symbol).
template<> DeltaTemplateClass<DurationClass>::DeltaTemplateClass(const char *name, const DurationClass &value) :
	DeltaClass(name),
	Value(value)
{
	m_Unknown04 = UnknownGlobal;
}

template<> void DeltaTemplateClass<DurationClass>::Unknown_00(DeltaChildVisitorClass *visitor)
{
	STUB_CALLED("DeltaTemplateClass<DurationClass>::Unknown_00");
}

// KVPairStringClass (vtable 0xAEB810): the stub constructor above emits the vtable, which needs
// the destructor and the six virtuals.
KVPairStringClass::~KVPairStringClass()
{
	STUB_CALLED("KVPairStringClass::~KVPairStringClass (0x4B80E0)");
}

void KVPairStringClass::Unknown_04()
{
	STUB_CALLED("KVPairStringClass::Unknown_04 (0x4B8250)");
}

const char *KVPairStringClass::Unknown_08() const
{
	STUB_CALLED("KVPairStringClass::Unknown_08 (0x4B8090)");
	return "";
}

void KVPairStringClass::Unknown_0C()
{
	STUB_CALLED("KVPairStringClass::Unknown_0C (0x4B8360)");
}

void KVPairStringClass::Unknown_10()
{
	STUB_CALLED("KVPairStringClass::Unknown_10 (0x4B83A0)");
}

void KVPairStringClass::Unknown_14()
{
	STUB_CALLED("KVPairStringClass::Unknown_14 (0x4B83D0)");
}

void KVPairStringClass::Unknown_18()
{
	STUB_CALLED("KVPairStringClass::Unknown_18 (0x4B8450)");
}
