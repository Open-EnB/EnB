// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\shared\graphics_settings.h (the client's
**	'..\shared/graphics_settings.h' in two FAIL_IFs 'max == min', lines 86 and 93)
**
**	Only the two inline helpers option.cpp uses (out of line at 0x7F8410 / 0x7F8480): a setting's
**	position in its range as a percentage, for the graphics sliders' defaults. The rest of the
**	header is unknown; the blank lines keep the FAIL_IFs on the client's lines (D25).
*/



#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __GRAPHICS_SETTINGS_H  // NAME 3 [graphics_settings.h]: the client's header path
#define __GRAPHICS_SETTINGS_H

#include "unknown_debug.h"






























































inline float Unknown_Percent(int value, int min, int max)	// 0x7F8410  // NAME 1 [Unknown_Percent]: neutral (D10): value's place in [min, max] as a percentage
{
	FAIL_IF(max == min) return(0.0f);
	return((value - min) * 100.0f / (max - min));
}


inline float Unknown_Percent(float value, float min, float max)	// 0x7F8480  // NAME 1 [Unknown_Percent]: neutral (D10)
{
	FAIL_IF(max == min) return(0.0f);
	return((value - min) * 100.0f / (max - min));
}

#endif //__GRAPHICS_SETTINGS_H
