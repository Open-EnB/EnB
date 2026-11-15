// Earth & Beyond client (reconstruction): the shield gadget of constructor 0x62A7C0 (notes in unknown_62a7c0.cpp)
#ifndef UNKNOWN_62A7C0_H  // NAME 1 [unknown_62a7c0.h]: neutral (D137)
#define UNKNOWN_62A7C0_H

class ClientObjectClass;
class RenderObjClass;
class UVControllerClass;

// Three shield meshes' texture scrollers following a client object's ShieldPercent (0x18 bytes, table 0xAF7AD0: one slot)
class Unknown62A7C0Class	// NAME 1: neutral (D10)
{
public:
	Unknown62A7C0Class(RenderObjClass *robj);			// 0x62A7C0 (its shield01 / shield02 / shield03 sub-objects)  // NAME 1 [robj]: what it reads
	~Unknown62A7C0Class();								// 0x62A9D0
	void Unknown_62AA20(ClientObjectClass *co);			// 0x62AA20 (a new object: the shields back to empty)  // NAME 1 [co]: what it holds
	virtual void Unknown_00();							// +0x00 0x62AAE0 (the object's ShieldPercent now)
	void Unknown_62ADE0(float percent);					// 0x62ADE0 (one third of the percent to each shield)  // NAME 1 [percent]: what it is

	UVControllerClass *Shield[3];						// +0x04 shield01, +0x08 shield02, +0x0C shield03  // NAME 2: the sub-objects' names
	ClientObjectClass *Object;							// +0x10  // NAME 2: what it holds
	float Percent;										// +0x14 the last percent shown  // NAME 2: what it holds
};

#endif
