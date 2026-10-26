// Earth & Beyond client (reconstruction): the bytes-per-second average, client 0x820830-0x820AF7.
// PROVISIONAL file name (D10, with unknown_rate.h): no assert or __FILE__ names the file; its code
// comes just before the composite listener (unknown_listener.cpp), likely the same object.
#include "unknown_rate.h"
#include <time.h>

// Types 0 and 1 share their counts (0xBE8E80-0xBE8E94) and periods (0xBA82A0 / 0xBA829C, 1 s)
static int Bytes1;				// 0xBE8E80  // NAME 2: what it holds
static int Bytes0;				// 0xBE8E84  // NAME 2: what it holds
static int Rate1;				// 0xBE8E88  // NAME 2: what it holds
static int Rate0;				// 0xBE8E8C  // NAME 2: what it holds
static int Start1;				// 0xBE8E90  // NAME 2: what it holds
static int Start0;				// 0xBE8E94  // NAME 2: what it holds
static int Period1 = 1;			// 0xBA829C (also every other object's first period)  // NAME 2: what it holds
static int Period0 = 1;			// 0xBA82A0  // NAME 2: what it holds

Unknown_RateClass::Unknown_RateClass(int type) :
	Bytes(0),
	Rate(0),
	Start(0),
	Period(Period1)
{
	Type = type;
}

Unknown_RateClass::~Unknown_RateClass()
{
}

// Counts bytes; a total near overflow is folded into the average first (1 when bytes < 1)
int Unknown_RateClass::Add(int bytes)
{
	int *count, *rate, *start, *period;
	Get_Fields(&count, &rate, &start, &period);
	if (*count > 0x0FFFFFFF) {
		int now = time(0);
		if (*start != 0) {
			*rate = (*count / (now - *start) + *rate) / 2;
		} else {
			*rate = 0;
		}
		*count = 0;
		*start = now;
	}
	if (bytes <= 0) {
		return(1);
	}
	*count += bytes;
	return(0);
}

// The average, folding in the bytes since the last sample once a period has passed
int Unknown_RateClass::Get()
{
	int *count, *rate, *start, *period;
	Get_Fields(&count, &rate, &start, &period);
	int now = time(0);
	if (*count > 0x0FFFFFFF || now - *start >= *period) {
		if (*start != 0) {
			*rate = (*count / (now - *start) + *rate) / 2;
			*count = 0;
			*start = now;
			return(*rate);
		}
		*rate = 0;
		*count = 0;
		*start = now;
	}
	return(*rate);
}

// The seconds between samples (1 when seconds < 1: unchanged)
int Unknown_RateClass::Set_Period(int seconds)
{
	int *count, *rate, *start, *period;
	Get_Fields(&count, &rate, &start, &period);
	if (seconds < 1) {
		return(1);
	}
	*period = seconds;
	return(0);
}

// This type's fields: the shared ones for types 0 and 1, else the object's
void Unknown_RateClass::Get_Fields(int **count, int **rate, int **start, int **period)
{
	switch (Type) {
		case 0:
			*count = &Bytes0;
			*rate = &Rate0;
			*start = &Start0;
			*period = &Period0;
			break;
		case 1:
			*count = &Bytes1;
			*rate = &Rate1;
			*start = &Start1;
			*period = &Period1;
			break;
		default:
			*count = &Bytes;
			*rate = &Rate;
			*start = &Start;
			*period = &Period;
			break;
	}
}
