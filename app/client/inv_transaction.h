// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: e:\build\beta\G\app\Client\inv_transaction.h (name assumed from inv_transaction.cpp, whose __FILE__ is
**	at 0xB90764; the client holds no string of this header)
**
**	One move of inventory items: how many (the quantity gadget), the drop query, then the INVENTORY_MOVE packet. 0x24 bytes; ui_inv.cpp holds one.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef INV_TRANSACTION_H
#define INV_TRANSACTION_H

class Unknown66CC00Class;
class Unknown690880ViewClass;
class Unknown6908E0Class;
class ViewUnknown34Class;

class InvTransactionClass	// NAME 3: class-from-file (D24) for app\Client\inv_transaction.cpp
{
public:
	InvTransactionClass();														// 0x690880 (ui_inv.cpp's 0x59F470)
	void Unknown_6908C0();														// 0x6908C0
	void Unknown_6908E0(Unknown6908E0Class *arg);								// 0x6908E0 (the view, the gadget, the query)  // NAME 1 [arg]: neutral
	bool Unknown_690AA0(int from_inv, int from_slot, int num, bool all, int to_inv, int to_slot);	// 0x690AA0 (starts one)
	void Unknown_690D20();														// 0x690D20 (the steps)
	void Unknown_690FA0();														// 0x690FA0 (cancels it)
	bool Unknown_6910C0();														// 0x6910C0 (the query is up)

	int State;																	// +0x00 (0 none, 1 the quantity, 2 the query, 3 send)  // NAME 1: what it holds
	Unknown690880ViewClass *View;												// +0x04  // NAME 3: the FAIL_IF '!View'
	Unknown66CC00Class *QuantityGadget;											// +0x08  // NAME 3: the FAIL_IF
	ViewUnknown34Class *DropQuery;												// +0x0C  // NAME 3: the FAIL_IF
	int Num;																	// +0x10  // NAME 3: the FAIL_IF's num
	int FromInv;																// +0x14  // NAME 3: the FAIL_IF's from_inv
	int FromSlot;																// +0x18  // NAME 3: the FAIL_IF's from_slot
	int ToInv;																	// +0x1C  // NAME 1: the packet's constructor
	int ToSlot;																	// +0x20  // NAME 1: the packet's constructor
};

#endif
