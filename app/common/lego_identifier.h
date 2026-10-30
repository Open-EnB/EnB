// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Common\lego_identifier.h (by convention from the client's
**	lego_identifier.cpp, D24)
**
**	A bone path in a "lego" (an assembled model): the bone names from the root down, written as one
**	string with '/' between them (vtable 0xB08C5C, 0x14 bytes). The object-to-object effects
**	(pkpointeffect.cpp, opcodes 11-14) send one as a string.
*/



#if defined(_MSC_VER)
#pragma once
#endif

#ifndef LEGO_IDENTIFIER_H  // NAME 3 [lego_identifier.h]: by convention from the client's lego_identifier.cpp (D24)
#define LEGO_IDENTIFIER_H

#include <string>
#include <vector>

// The lego types' names (0xB08C40: tractor, emitter)
void Lego_Type_Name(char *buffer, int len, int *type);			// 0x843810 (lego_identifier.cpp l.58)  // NAME 2: what it does
bool Lego_Type_From_Name(const char *name, int *type);			// 0x843890 (l.75)  // NAME 2: what it does

class LegoIdentifierClass	// NAME 3: by convention from the client's lego_identifier.cpp (D24)
{
public:
	LegoIdentifierClass();										// 0x843910
	LegoIdentifierClass(const std::string &name);				// 0x843970
	LegoIdentifierClass(const LegoIdentifierClass &src);		// 0x843B50
	virtual ~LegoIdentifierClass();								// +0x00 0x843940 (0x843DB0)

	bool Is_Empty() const;										// 0x843E40  // NAME 2: what it returns
	LegoIdentifierClass &operator=(const LegoIdentifierClass &src);	// 0x843E80
	LegoIdentifierClass &operator=(const std::string &name);	// 0x844120
	bool operator==(const LegoIdentifierClass &src) const;		// 0x844210
	int Get_Count() const;										// 0x844300  // NAME 2: what it returns
	std::string Get_Root() const;								// 0x844330 (the first bone)  // NAME 2: what it returns
	std::string Pop_Root();										// 0x8444C0 (l.136)  // NAME 2: what it does
	void Add(const std::string &bone);							// 0x844680  // NAME 2: what it does
	void Add(const LegoIdentifierClass &src);					// 0x8448A0  // NAME 2: what it does
	std::string Get_Name() const;								// 0x844A60 (the bones joined by '/')  // NAME 2: what it returns
	void Set_Name(const std::string &name);						// 0x844CD0 (l.175)  // NAME 2: what it does

	std::vector<std::string> BoneNameChain;		// +0x04  // NAME 5: the client's asserts '!BoneNameChain.empty()'
};

#endif
