// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Common\average.h (the client's assert names it, l.98)
**
**	A running average over Count one-second samples, as dbgconn.cpp's rate wrapper 0x824A90 keeps
**	two (bytes in and out). The constructor (inline, l.98) allocates the samples; the owner adds to
**	Current and rolls it into Sample[Index] each second (Time). The client inlines all of it: the
**	only out-of-line copy is the destructor 0x824C30 (the rate wrapper's unwinding). No string names
**	the class: AverageClass after the file (D24).
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

	int Current;		// +0x00 this second's total  // NAME 2: what it holds
	unsigned int Time;	// +0x04 the start of this second (Unknown_Get_Ticks)  // NAME 2: what it holds
	int *Sample;		// +0x08 (S: the assert)
	int Count;			// +0x0C  // NAME 2: what it holds
	int Index;			// +0x10 the next sample  // NAME 2: what it holds
};

#endif
