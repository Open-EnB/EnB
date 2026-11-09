// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: unknown (PROVISIONAL, D137: a neutral name; the function's header is not known)
**
**	0x4BE9D0 (message_filter.cpp's, provisional): a name with its first letter upper case and the rest
**	lower case, cut to 16 characters. Its own header because <string> in message.h costs
**	message_server.cpp's 0x4BCB50 (workflow.md §3.0, "Headers and EA sources").
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef UNKNOWN_4BE9D0_H  // NAME 1 [unknown_4be9d0.h]: neutral (D137)
#define UNKNOWN_4BE9D0_H

#include <string>

std::string Unknown_4BE9D0(const char *name);	// NAME 1 [Unknown_4BE9D0]: neutral (D10); callers avatar_desc.cpp's 0x4C1460 / 0x4C15F0 / Convert_Names, 0x5D6DC0, 0x616220, 0x649FE0

#endif
