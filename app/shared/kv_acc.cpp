// Earth & Beyond client (reconstruction): notes at the end of the file
#include <time.h>
#include "kv_acc_string.h"
#include "kv_acc_time.h"

unsigned int Unknown_Get_Ticks();	// 0x4A3B40: the time since the first call (delta_time.cpp)

// @0x4B0A90
KVFixedStringAccessorClass::KVFixedStringAccessorClass(const char *value)
{
	Value = nstrdup(value);
}

inline unsigned int Round_Tenths(unsigned int value)	// NAME 2: what it does
{
	return (unsigned int)((__int64)value + 0.5);
}

// @0x4B1160
KVTickSecondsAccessorClass::KVTickSecondsAccessorClass(unsigned int *value) : Value(value), Seconds(0)
{
}

// @0x4B11E0
const unsigned int &KVTickSecondsAccessorClass::Get()
{
	if (*Value == 0) {
		Seconds = 0;
		return Seconds;
	}
	unsigned int ticks = Unknown_Get_Ticks();
	unsigned int value = *Value;
	bool later;
	unsigned int span;
	if (value > ticks) {
		later = true;
		span = value - ticks;
	} else {
		later = false;
		span = ticks - value;
	}
	span /= 1000;
	unsigned int now = time(NULL);
	if (later) {
		Seconds = now + span;
	} else {
		Seconds = now - span;
	}
	return Seconds;
}

// @0x4B1270
void KVTickSecondsAccessorClass::Set(const unsigned int &value)
{
	if (value == 0) {
		*Value = 0;
		return;
	}
	unsigned int now = time(NULL);
	unsigned int seconds = value;
	bool later;
	unsigned int span;
	if (seconds > now) {
		later = true;
		span = seconds - now;
	} else {
		later = false;
		span = now - seconds;
	}
	span *= 1000;
	unsigned int ticks = Unknown_Get_Ticks();
	if (later) {
		*Value = ticks + span;
	} else {
		*Value = ticks - span;
	}
}

// @0x4B1340
KVTenthsSecondsAccessorClass::KVTenthsSecondsAccessorClass(unsigned int *value) : Value(value), Seconds(0)
{
}

// @0x4B13C0
const unsigned int &KVTenthsSecondsAccessorClass::Get()
{
	if (*Value == 0) {
		Seconds = 0;
		return Seconds;
	}
	unsigned int tenths = Round_Tenths(Unknown_Get_Ticks() / 1000 * 10);
	unsigned int value = *Value;
	bool later;
	unsigned int span;
	if (value > tenths) {
		later = true;
		span = value - tenths;
	} else {
		later = false;
		span = tenths - value;
	}
	span = Round_Tenths(span / 10);
	unsigned int now = time(NULL);
	if (later) {
		Seconds = now + span;
	} else {
		Seconds = now - span;
	}
	return Seconds;
}

// @0x4B14B0
void KVTenthsSecondsAccessorClass::Set(const unsigned int &value)
{
	if (value == 0) {
		*Value = 0;
		return;
	}
	unsigned int now = time(NULL);
	unsigned int seconds = value;
	bool later;
	unsigned int span;
	if (seconds > now) {
		later = true;
		span = seconds - now;
	} else {
		later = false;
		span = now - seconds;
	}
	span = Round_Tenths(span * 10);
	unsigned int tenths = Round_Tenths(Unknown_Get_Ticks() / 1000 * 10);
	if (later) {
		*Value = tenths + span;
	} else {
		*Value = tenths - span;
	}
}

// @0x4B15E0
KVTenthsSpanAccessorClass::KVTenthsSpanAccessorClass(unsigned int *value) : Value(value)
{
}

// @0x4B1660
const unsigned int &KVTenthsSpanAccessorClass::Get()
{
	Seconds = Round_Tenths(*Value / 10);
	return Seconds;
}

// @0x4B16B0
void KVTenthsSpanAccessorClass::Set(const unsigned int &value)
{
	*Value = Round_Tenths(value * 10);
}

/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: unknown; no assert, no __FILE__: the name is PROVISIONAL (D137). The accessors' object, client 0x4B0630 (permstr.h's
**	static) - 0x4B1730: their out-of-line members; the callers in other objects keep an EH state around these constructors, so they are
**	not defined there. 0x4B0680-0x4B08D0 at its start (a deque walk) is not rebuilt.
*/
