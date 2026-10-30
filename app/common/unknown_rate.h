// Earth & Beyond client (reconstruction)
/*
**	PROVISIONAL file name (no assert or __FILE__ names it): a bytes-per-second average, client
**	0x820830-0x820A20 (just before connect.cpp's first function, unattributed). Type 0 and 1 keep
**	their counts in two shared sets (0xBE8E80-0xBE8E94), any other type in the object; Add counts
**	bytes, Get averages them over the time since the last sample (time(), at most every Period
**	seconds, 0xBA829C). UDPConnection keeps two of type 2 (the bytes in and out its CordCon counted).
**
**	Rebuilt in unknown_rate.cpp (same provisional name).
*/



#if defined(_MSC_VER)
#pragma once
#endif

#ifndef UNKNOWN_RATE_H  // NAME 1 [unknown_rate.h]: neutral file name (D37, D10)
#define UNKNOWN_RATE_H

class Unknown_RateClass	// NAME 1: neutral placeholder (D10)
{
public:
	Unknown_RateClass(int type);		// 0x820830
	~Unknown_RateClass();				// 0x820860 (empty)
	int Add(int bytes);				// 0x820880 (1 for bytes < 1)  // NAME 2: what it does
	int Get();							// 0x820950: bytes per second  // NAME 2: what it returns
	int Set_Period(int seconds);		// 0x820A20 (no caller)  // NAME 2: what it does
	void Get_Fields(int **count, int **rate, int **start, int **period);	// 0x820A70 (inlined in the others)  // NAME 2: what it does

	int Type;			// +0x00 NAME 2: 0 / 1 shared counts, else this object's
	int Bytes;			// +0x04 NAME 2: counted since Start
	int Rate;			// +0x08 NAME 2: the running average
	int Start;			// +0x0C NAME 2: time() of the last sample
	int Period;			// +0x10 NAME 2: seconds between samples (the global 0xBA829C)
};

#endif
