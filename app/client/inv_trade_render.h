// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Client\inv_trade_render.h (name assumed from inv_trade_render.cpp, whose __FILE__ is
**	at 0xB905A8; the client holds no string of this header)
**
**	An inventory slot on the trade screen: table 0xAFCDF0, 0x8C bytes, 'fa_ce02a.ini' (0x6A90A0 makes it).
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef INV_TRADE_RENDER_H
#define INV_TRADE_RENDER_H

#include "inv_render.h"

class FontSys;
class ManagedFontClass;

class InvTradeRenderClass : public InvRenderClass	// NAME 3: class-from-file (D24) for app\Client\inv_trade_render.cpp
{
public:
	InvTradeRenderClass();													// 0x68FB60 ('fa_ce02a.ini')
	virtual ~InvTradeRenderClass();											// +0x04 0x68FC80 (deleting 0x68FC50)
	virtual void *Unknown_24() { return(this); }							// +0x24 0x68FC30
	virtual void Unknown_2C(bool force);									// +0x2C 0x68FE10

	void Unknown_6902E0(RenderObjClass *robj, int index, const char *suffix);	// 0x6902E0 (0x650A40 calls it)  // NAME 1 [robj, index, suffix]: neutral

	std::string Unknown58;													// +0x58 ('WRAP_text%02d_%s')
	std::string Unknown68;													// +0x68 ('WRAP_BODY%02d_%s')
	ManagedFontClass *DescriptionGroup;											// +0x78  // NAME 3: DescriptionGroup, 0x6902E0's assert
	RefCountPtr<FontSys> CostText;									// +0x7C ('cost%02d_%s')  // NAME 3: CostText, 0x6902E0's assert
	RefCountPtr<FontSys> StackText;									// +0x80 ('stack%02d_%s')  // NAME 3: StackText, 0x6902E0's assert
	RefCountPtr<FontSys> OwnedText;									// +0x84 ('owned_title%02d_%s')  // NAME 3: OwnedText, 0x6902E0's assert
	RefCountPtr<FontSys> OwnedTextValue;									// +0x88 ('owned%02d_%s')  // NAME 3: OwnedTextValue, 0x6902E0's assert
};

#endif
