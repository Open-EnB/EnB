// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Common\average.h (the client's assert names it, l.98)
**
**	A running average over Count samples, as dbgconn.cpp's rate wrapper 0x824A90 keeps two (bytes
**	in and out per second, in its RateCounterStruct). The constructor (inline, l.98) allocates the
**	samples; Add puts one over the oldest. The client inlines all of it: the only out-of-line copy is
**	the counter's destructor 0x824C30 (the rate wrapper's unwinding). That the samples are a class of
**	their own, inside the counter, is the client's code: the samples' stores go through Add's own
**	`this` (0x824EC0's order). No string names the class: AverageClass after the file (D24).
**
**	The spacing below keeps the assert on the client's line (D25).
*/


#if defined(_MSC_VER)
#pragma once
#endif

#ifndef AVERAGE_H
#define AVERAGE_H	// NAME 3: after the client's average.h (D24)

#include "..\shared\unknown_debug.h"

class AverageClass	// NAME 3: after the client's average.h (D24)
{
public:


































































	AverageClass(int count) : Sample(0), Count(count), Index(0)
	{
		Sample = new int[Count];
		G_ASSERT(Sample);
		for (int i = 0; i < Count; i++) {
			Sample[i] = 0;
		}
	}
	~AverageClass() { delete [] Sample; }							// 0x824C30

	// The mean of the samples
	int Get_Average()	// NAME 2: what it does
	{
		int sum = 0;
		for (int i = 0; i < Count; i++) {
			sum += Sample[i];
		}
		return(sum / Count);
	}

	// One more sample, over the oldest
	void Add(int value)	// NAME 2: what it does
	{
		Sample[Index] = value;
		Index = (Index + 1) % Count;
	}

	int *Sample;		// +0x00 (S: the assert)
	int Count;			// +0x04  // NAME 2: what it holds
	int Index;			// +0x08 the next sample  // NAME 2: what it holds
};

#endif
