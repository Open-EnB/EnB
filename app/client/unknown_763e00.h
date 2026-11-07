// Earth & Beyond client (reconstruction): the render state list table, 0x763E00-0x764480 (PROVISIONAL file, D137)
#if defined(_MSC_VER)
#pragma once
#endif

#ifndef UNKNOWN_763E00_H
#define UNKNOWN_763E00_H

#include "vector.h"

class RenderStateListClass;

// The 0x10-byte object at ClientClass +0x18 (constructor 0x763E00, destructor 0x763E30): rs.ini's render state
// lists by RSLID. Its one member is a VectorClass (whose vtable 0xB03EC8 the constructor stores): not derived, as
// ClientClass's destructor 0x726C60 calls 0x763E30 directly before operator delete.
class Unknown763E00Class	// NAME 1: neutral (D10)
{
public:
	Unknown763E00Class();								// 0x763E00
	~Unknown763E00Class();								// 0x763E30
	void Unknown_763E70(const char *ini_name);			// 0x763E70: SClient::Init passes 'rs.ini'  // NAME 1 [ini_name]: neutral
	void Unknown_763FF0();								// 0x763FF0 (SClient::Shutdown, before the delete: deletes every list)
	void *Unknown_764030(int rslid);					// 0x764030 (co.cpp's model loader: the render state list of a base asset's RSLID)  // NAME 2 [rslid]: the INI key it is read from
	RenderStateListClass *Unknown_764060(const char *name);	// 0x764060 (0x5B8330: a one-state 'object' list for name.W3D, added at the end)  // NAME 1 [name]: neutral

	VectorClass<RenderStateListClass *> Lists;			// +0x00  // NAME 1: neutral (D10)
};

#endif
