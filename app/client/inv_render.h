// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Client\inv_render.h (name assumed from inv_render.cpp, whose __FILE__ is
**	'e:\build\beta\G\app\Client\inv_render.cpp' at 0xB90220; the client holds no string of this header)
**
**	What draws an inventory item's slot: table 0xAFCBD8, constructor 0x68BF10, destructor 0x68C190, 0x58 bytes, and its
**	derived classes (tables 0xAFCCA0, 0xAFCD04, 0xAFCDF0), which 0x6A90A0 makes. Rebuilt as far as the listed addresses.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef INV_RENDER_H
#define INV_RENDER_H

#include <string>
#include "slot_data_render.h"
#include "..\shared\duration.h"

class FontSys;
class InvDataClass;
class Unknown6A3D60Class;
class Unknown688830Item;

class InvRenderClass : public SlotDataRenderClass	// NAME 3: class-from-file (D24) for app\Client\inv_render.cpp
{
public:
	InvRenderClass(bool arg);												// 0x68BF10  // NAME 1 [arg]: neutral
	virtual ~InvRenderClass();												// +0x04 0x68C190 (deleting 0x68C160)
	virtual bool Unknown_08(SlotDataClass *sd, RenderObjClass *robj);		// +0x08 0x68C2D0
	virtual void *Unknown_18() { return(this); }							// +0x18 0x68C110
	virtual const char *Unknown_28(std::string *string);					// +0x28 0x68CCC0
	virtual void Unknown_2C(bool force);									// +0x2C 0x68C6B0
	virtual int Unknown_30(int level);										// +0x30 0x68D690
	virtual TextureClass *Unknown_34();										// +0x34 0x68CBA0
	virtual void Unknown_50(int count);										// +0x50 0x68D790 (the count's text)  // NAME 1 [count]: neutral

	InvDataClass *Peek_Inv_Data();											// 0x68D620  // NAME 3: the assert '!Peek_Inv_Data()->Get_Client()'
	void Unknown_68D700(int value);											// 0x68D700 (the level's text)  // NAME 1 [value]: neutral
	int Unknown_68D7E0(int slot);											// 0x68D7E0  // NAME 1 [slot]: neutral
	void Unknown_68EAE0(bool on, const RefCountPtr<RenderObjClass> &robj);	// 0x68EAE0 (onto or off the container)  // NAME 1 [on, robj]: neutral
	void Unknown_68EB10(Unknown688830Item *item);							// 0x68EB10  // NAME 1 [item]: neutral

	FontSys *LevelText;														// +0x2C ('INVENTORY_LEVEL')  // NAME 2: its string template
	FontSys *CountText;														// +0x30 ('INVENTORY_COUNT')  // NAME 2: its string template
	char Unknown34[4];														// +0x34
	DurationClass Unknown38;												// +0x38 (the next time 0x68C6B0 looks)
	RefCountPtr<RenderObjClass> CannotUseIndicator;							// +0x40  // NAME 3: 0x68C2D0's assert
	std::string Unknown44;													// +0x44 ('All')
	char Unknown54[4];														// +0x54
};

class Unknown68D870Class : public InvRenderClass	// NAME 1: neutral (D10)
{
public:
	Unknown68D870Class();													// 0x68D870
	virtual ~Unknown68D870Class();											// +0x04 0x68D8E0 (deleting 0x68D8B0)
	virtual void Unknown_2C(bool force);									// +0x2C 0x68D9E0
	virtual void Unknown_40(int slot);										// +0x40 0x68D950
	virtual void Unknown_50(int count);										// +0x50 0x68D900 (Unknown58's text)

	bool Unknown_68DA20(bool force);										// 0x68DA20

	int Unknown58;															// +0x58
	int Unknown5C;															// +0x5C (-1)
};

struct Unknown78Struct	// NAME 1: neutral (D10); its own zeroing keeps 0x68DC20's stores in +0x74, +0x78 order
{
	Unknown78Struct() : Unknown00(0), Unknown04(0), Unknown08(0) {}
	int Unknown00;
	int Unknown04;
	int Unknown08;
};

class Unknown68DC20Class : public InvRenderClass	// NAME 1: neutral (D10)
{
public:
	Unknown68DC20Class();													// 0x68DC20
	virtual ~Unknown68DC20Class();											// +0x04 0x68E120 (deleting 0x68DC90)
	virtual bool Unknown_08(SlotDataClass *sd, RenderObjClass *robj);		// +0x08 0x68DCC0
	virtual const char *Unknown_28(std::string *string);					// +0x28 0x68F510
	virtual void Unknown_2C(bool force);									// +0x2C 0x68E360
	virtual void Unknown_3C() { Unknown88 = true; }							// +0x3C 0x68E340

	RefCountPtr<RenderObjClass> OnIndicator;	// +0x58  // NAME 3: 0x68DCC0's assert
	RefCountPtr<RenderObjClass> OutOfAmmoIndicator;	// +0x5C  // NAME 3: 0x68DCC0's assert
	RefCountPtr<RenderObjClass> ValidityIndicator;	// +0x60  // NAME 3: 0x68DCC0's assert
	RefCountPtr<RenderObjClass> WrongTargetTypeIndicator;	// +0x64 ('item_noskill')  // NAME 2: skill_render.cpp's, same model
	RefCountPtr<RenderObjClass> RechargeIndicator;	// +0x68  // NAME 3: 0x68DCC0's assert
	Unknown6A3D60Class *Unknown6C;											// +0x6C (destructor 0x6A3F10)
	int Unknown70;															// +0x70
	float Unknown74;														// +0x74 (-1)
	Unknown78Struct Unknown78;												// +0x78
	int Unknown84;															// +0x84 (0x100)
	bool Unknown88;															// +0x88
};

class Unknown68FB60Class : public InvRenderClass	// NAME 1: neutral (D10)
{
public:
	Unknown68FB60Class();													// 0x68FB60 ('fa_ce02a.ini')
	virtual ~Unknown68FB60Class();											// +0x04 0x68FC80 (deleting 0x68FC50)
	virtual void *Unknown_24() { return(this); }							// +0x24 0x68FC30
	virtual void Unknown_2C(bool force);									// +0x2C 0x68FE10

	std::string Unknown58;													// +0x58
	std::string Unknown68;													// +0x68
	int Unknown78;															// +0x78
	RefCountPtr<RenderObjClass> Unknown7C;									// +0x7C
	RefCountPtr<RenderObjClass> Unknown80;									// +0x80
	RefCountPtr<RenderObjClass> Unknown84;									// +0x84
	RefCountPtr<RenderObjClass> Unknown88;									// +0x88
};

#endif
