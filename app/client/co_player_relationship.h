// Earth & Beyond client (reconstruction)
/*
**	Original location: e:\build\beta\G\app\client\co_player_relationship.h (name assumed from co_player_relationship.cpp)
**
**	The gutter colours of one relationship, as radar_data.cpp and the file itself use them.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __CO_PLAYER_RELATIONSHIP_H  // NAME 3 [co_player_relationship.h]: header name assumed from the client's co_player_relationship.cpp
#define __CO_PLAYER_RELATIONSHIP_H

#include "vector3.h"

namespace std { template<class _E> struct char_traits; template<class _Ty> class allocator; template<class _E, class _Tr, class _A> class basic_string; }

class INISection;
class SClient;
class ClientObjectClass;

struct GutterColorStruct	// NAME 2: what it holds ('Failed to lookup gutter color %s')
{
	GutterColorStruct() {}
	GutterColorStruct(const Vector3 &color_1, const Vector3 &color_2) : Color1(color_1), Color2(color_2) {}	// 0x70CE10 (out of line in 0x70A7E0)

	Vector3 Color1;	// +0x00  // NAME 2: the assert's color_1
	Vector3 Color2;	// +0x0C  // NAME 2: the assert's color_2
};

// The colours of one gutter ini section (0x60 bytes; copied whole by 0x70ACA0)
struct GutterColorSetStruct	// NAME 2: what it holds
{
	GutterColorSetStruct();	// 0x70A790 (JumpStart left as it is)
	GutterColorSetStruct(INISection *section);	// 0x70A010
	void Read_Color(INISection *section, const std::basic_string<char, std::char_traits<char>, std::allocator<char> > &name, GutterColorStruct &color);	// 0x70A2E0 (ecx unused)  // NAME 2: what it does

	GutterColorStruct Target;	// +0x00  // NAME 3: the ini name 'Target'
	GutterColorStruct Mouse;	// +0x18  // NAME 3: the ini name 'Mouse'
	GutterColorStruct NotTarget;	// +0x30  // NAME 3: the ini name 'NotTarget'
	GutterColorStruct JumpStart;	// +0x48  // NAME 3: the ini name 'JumpStart'
};

GutterColorSetStruct Unknown_70B0A0(SClient *client, ClientObjectClass *object);	// 0x70B0A0 (radar_data.cpp)  // NAME 1 [Unknown_70B0A0]: the gutter colours of an object
bool Unknown_70BAC0(SClient *client, ClientObjectClass *object);	// 0x70BAC0  // NAME 1 [Unknown_70BAC0]: the object is a player in the client's group

#endif //__CO_PLAYER_RELATIONSHIP_H
