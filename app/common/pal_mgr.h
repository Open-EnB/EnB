// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Common\pal_mgr.h (name assumed from pal_mgr.cpp; no assert
**	names it)
**
**	Public and secret palette loader and sampler (pal_mgr.cpp, 0x848130-0x848C61).
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __PAL_MGR_H  // NAME 3 [pal_mgr.h]: header name assumed from the client's pal_mgr.cpp
#define __PAL_MGR_H

void Unknown_848250(void);										// 0x848250  // NAME 1: neutral (D10)
void Unknown_848810(void);										// 0x848810  // NAME 1: neutral (D10)
void Unknown_8488A0(int palette, float u, float v, float *rgb);	// 0x8488A0  // NAME 1: neutral (D10)
void Unknown_848A10(int palette, float u, float v, float *rgb);	// 0x848A10  // NAME 1: neutral (D10)

#endif //__PAL_MGR_H
