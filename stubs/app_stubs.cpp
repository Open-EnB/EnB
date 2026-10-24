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
#include "linearbu.h"
#include "encode.h"
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

// DeltaVisitorClass::Visit_Children (0x4B0860). Not a stub: the client's function, whose object
// (0x4AF.../0x4B0...) isn't attributed to a file yet, so it can't go into the tree.
void DeltaVisitorClass::Visit_Children(DeltaCompoundClass *delta)
{
	int count = delta->Children.size();
	for (int i = 0; i < count; i++) {
		delta->Children[i]->Accept(*this);
	}
}

// DeltaCompoundClass::Decode (0x4A3FA0, the root's decode: one byte picks the codec, encode.h).
// Not a stub: the client's function, byte for byte (checked with appfile.py match), but its
// object (0x4A3990-0x4A4780, the root class, vtable 0xAEAB88) is not attributed to a file yet.
bool DeltaCompoundClass::Decode(LinearBufferClass &buf, const DurationClass &base)
{
	char encoding;
	buf.Read(&encoding, 1);
	DeltaCodecClass *codec;
	if (encoding == 0) {
		codec = &RecordCodec;
	} else {
		codec = Get_Tree_Codec();
	}
	return codec->Decode(this, buf, base);
}

// 0x4B6760
void KVListClass::Add(KVPairClass *item)
{
	STUB_CALLED("KVListClass::Add (0x4B6760)");
}

// KVPairTemplateClass<float> (vtable 0xAE7860): the destructor and six virtuals (the client's are
// in a client object, 0x4484C0-0x448B10; the slots are described in kv_pair.h)
KVPairTemplateClass<float>::~KVPairTemplateClass()
{
	STUB_CALLED("KVPairTemplateClass<float>::~KVPairTemplateClass");
}

KVPairClass *KVPairTemplateClass<float>::Clone() const
{
	STUB_CALLED("KVPairTemplateClass<float>::Clone (0x4484C0)");
	return 0;
}

const char *KVPairTemplateClass<float>::Get_Type_Name() const
{
	return "float";		// 0x448580, not a stub
}

char *KVPairTemplateClass<float>::Get_Value_String(char *buf, int size) const
{
	STUB_CALLED("KVPairTemplateClass<float>::Get_Value_String (0x4485A0)");
	if (size > 0) {
		buf[0] = 0;
	}
	return buf;
}

bool KVPairTemplateClass<float>::Set_Value_String(const char *text)
{
	STUB_CALLED("KVPairTemplateClass<float>::Set_Value_String (0x448B10)");
	return false;
}

bool KVPairTemplateClass<float>::Is_Changed() const
{
	STUB_CALLED("KVPairTemplateClass<float>::Is_Changed (0x4485D0)");
	return false;
}

void KVPairTemplateClass<float>::Update_Saved()
{
	STUB_CALLED("KVPairTemplateClass<float>::Update_Saved (0x448610)");
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
