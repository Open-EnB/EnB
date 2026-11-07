// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: unknown (name PROVISIONAL, D137, as kv_acc_array.h): the time accessors of kv_acc.cpp, which
**	kv_time_adds.cpp's adds make.
*/
#ifndef __KV_ACC_TIME_H	// NAME 3 [kv_acc_time.h]: PROVISIONAL
#define __KV_ACC_TIME_H

#include "kv_pair.h"

// A tick count (milliseconds of Unknown_Get_Ticks) as wall-clock seconds (vtable 0xAEB39C, 0xC bytes)
class KVTickSecondsAccessorClass : public KVAccessorClass<unsigned int>	// NAME 2: what it does
{
public:
	KVTickSecondsAccessorClass(unsigned int *value);						// 0x4B1160
	virtual const unsigned int &Get();											// 0x4B11E0
	virtual void Set(const unsigned int &value);								// 0x4B1270
	virtual KVAccessorClass<unsigned int> *Clone() const { return new KVTickSecondsAccessorClass(Value); }	// 0x4B1300

	unsigned int *Value;		// +0x04  // NAME 2: what it holds
	unsigned int Seconds;	// +0x08 Get's result  // NAME 2: what it holds
};

// A count of tenths of a second as wall-clock seconds (vtable 0xAEB3B0, 0xC bytes)
class KVTenthsSecondsAccessorClass : public KVAccessorClass<unsigned int>	// NAME 2: what it does
{
public:
	KVTenthsSecondsAccessorClass(unsigned int *value);						// 0x4B1340
	virtual const unsigned int &Get();											// 0x4B13C0
	virtual void Set(const unsigned int &value);								// 0x4B14B0
	virtual KVAccessorClass<unsigned int> *Clone() const { return new KVTenthsSecondsAccessorClass(Value); }	// 0x4B15A0

	unsigned int *Value;		// +0x04  // NAME 2: what it holds
	unsigned int Seconds;	// +0x08 Get's result  // NAME 2: what it holds
};

// A count of tenths of a second as a span of seconds (vtable 0xAEB3C4, 0xC bytes)
class KVTenthsSpanAccessorClass : public KVAccessorClass<unsigned int>	// NAME 2: what it does
{
public:
	KVTenthsSpanAccessorClass(unsigned int *value);						// 0x4B15E0
	virtual const unsigned int &Get();											// 0x4B1660
	virtual void Set(const unsigned int &value);								// 0x4B16B0
	virtual KVAccessorClass<unsigned int> *Clone() const { return new KVTenthsSpanAccessorClass(Value); }	// 0x4B1700

	unsigned int *Value;		// +0x04  // NAME 2: what it holds
	unsigned int Seconds;	// +0x08 Get's result  // NAME 2: what it holds
};

#endif //__KV_ACC_TIME_H
