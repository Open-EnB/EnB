// Earth & Beyond client (reconstruction): app\client\inv_transaction.cpp, one move of inventory items (the client's file per
// its asserts, 'e:\build\beta\G\app\Client\inv_transaction.cpp' at 0xB90764). Notes at the end.
#include <string>
#include <string.h>
#include "sclient.h"
#include "..\shared\g_assert.h"
#include "..\packets\pkinventorymove.h"
#include "gadget.h"
#include "local_interface_sort.h"
#include "inv_transaction.h"

enum { INV_NONE = 0, MAX_ITEMS = -1, USER_SELECT_NO = 1 };	// NAME 3: the asserts'

// The quantity gadget
#include "gd_selct.h"
// The view it works in, past UIViewClass's 0x4C bytes
class Unknown690880ViewClass : public UIViewClass { public: Unknown690880ViewClass(); ClientClass *Unknown4C; };	// NAME 1: neutral (D10)
// What 0x6908E0 takes: its slot +0x20 gives the view
class Unknown6908E0Class { public: virtual void Unknown_00(); virtual void Unknown_04(); virtual void Unknown_08(); virtual void Unknown_0C(); virtual void Unknown_10(); virtual void Unknown_14(); virtual void Unknown_18(); virtual void Unknown_1C(); virtual Unknown690880ViewClass *Unknown_20(); };	// NAME 1: neutral (D10)

// 0x690880
InvTransactionClass::InvTransactionClass() :
	State(0),
	View(0),
	QuantityGadget(0),
	DropQuery(0),
	Num(0),
	FromInv(0),
	FromSlot(-1),
	ToInv(0),
	ToSlot(-1)
{
}

// 0x6908C0
void InvTransactionClass::Unknown_6908C0()
{
	Unknown_6908E0(0);
}

// 0x6908E0
void InvTransactionClass::Unknown_6908E0(Unknown6908E0Class *arg)
{
	if (arg == (Unknown6908E0Class *)View) return;
	Unknown_690FA0();
	if (QuantityGadget) {
		GadgetClass::Destroy(QuantityGadget);
		QuantityGadget = 0;
	}
	DropQuery = 0;
	View = arg ? arg->Unknown_20() : 0;
	if (View) {
		QuantityGadget = new SelectGadgetClass(View, 0);
		QuantityGadget->Set_First_Sort_Group(true);
		QuantityGadget->Unknown_40(false);
		LayerClass layer((SceneClass *)View->Unknown_50(), (CameraClass *)View->Unknown_54());
		SClient *sclient = (SClient *)View->Unknown4C->As_Space_Client();	// NAME 1 [sclient, layer]: neutral
		if (sclient) {
			if (sclient->Unknown_744B60()) {
				DropQuery = sclient->Unknown135C->Get_Unknown34();
			} else {
				DropQuery = sclient->MainView->Get_Unknown34();
			}
		}
	}
}



















































// NOT EXACT: 0x690AA0, 17 rows: register, and the client keeps State = 2 before the else
// 0x690AA0
bool InvTransactionClass::Unknown_690AA0(int from_inv, int from_slot, int num, bool all, int to_inv, int to_slot)	// NAME 3 [from_inv, from_slot, num, all]: the FAIL_IFs; NAME 1 [to_inv, to_slot]: Unknown_690D20's packet
{
	if (!View) return(false);
	FAIL_IF(num == 0) return(false);
	FAIL_IF(!all && (num == MAX_ITEMS)) return(false);
	FAIL_IF(from_inv == INV_NONE) return(false);
	FAIL_IF(from_slot < 0) return(false);
	if (State != 0) return(false);
	if (DropQuery->Active) {
		if (!strcmp(DropQuery->Type, "UI_TRANSACTION_DUMP_ITEM")) return(false);
	}
	FromSlot = from_slot;
	FromInv = from_inv;
	ToInv = to_inv;
	ToSlot = to_slot;
	Num = num;
	if (num == 1 || all) {
		State = 2;
		goto done;
	}
	{
		InterfaceSortClass sort(View, true);	// NAME 1 [sort]: neutral
		State = 1;
		QuantityGadget->Max = Num;
		QuantityGadget->Unknown_54(View->Unknown2C, 0);
		QuantityGadget->Unknown_40(true);
	}
done:
	Unknown_690D20();
	return(true);
}



















// NOT EXACT: 0x690D20, 20 rows: register and order of the packet's arguments
// 0x690D20
void InvTransactionClass::Unknown_690D20()
{
	FAIL_IF(!View) return;
	if (State == 0) return;
	if (State == 1) {
		Num = QuantityGadget->Value;
		if (!QuantityGadget->Finished) return;
		QuantityGadget->Unknown_40(false);
		QuantityGadget->Unknown_58(View->Unknown2C);
		if (Num == 0) {
			State = 0;
			return;
		}
		State = 2;
	}
	if (State == 2) {
		bool active = DropQuery->Active;	// NAME 1 [active]: neutral
		if (active && !strcmp(DropQuery->Type, "UI_TRANSACTION_DUMP_ITEM")) {
			int choice = DropQuery->Unknown_5890B0("UI_TRANSACTION_DUMP_ITEM");	// NAME 3 [choice]: the assert's
			switch (choice) {
				case 0:
					return;
				default:
					DropQuery->Unknown_5C();























					G_ASSERT(choice == USER_SELECT_NO);
					State = 0;
					return;
				case 2:
					DropQuery->Unknown_5C();
					break;
			}
		} else if (ToInv == 11) {
			if (FromInv != 12 && FromInv != 14) {
				if (!active) {
					DropQuery->Unknown_588A70("UI_TRANSACTION_DUMP_ITEM", true, 0, 0, false, false);
				}
				return;
			}
			ToInv = 1;
		}
		State = 3;
	}
	if (State != 3) return;
	ClientClass *client = View->Unknown4C;	// NAME 1 [client, packet]: neutral
	if (client) {
		InventoryMovePacketClass packet(client->ShipID, (InventoryEnum)FromInv, FromSlot, (InventoryEnum)ToInv, ToSlot, Num);
		client->Send_Packet(&packet);
	}
	State = 0;
}



















// 0x690FA0
void InvTransactionClass::Unknown_690FA0()
{
	switch (State) {
		case 1:
			FAIL_IF(!QuantityGadget) return;
			QuantityGadget->Unknown_40(false);
			QuantityGadget->Unknown_58(View->Unknown2C);
			break;
		case 2:


			FAIL_IF(!DropQuery) return;
			if (DropQuery->Active && !strcmp(DropQuery->Type, "UI_TRANSACTION_DUMP_ITEM")) {
				DropQuery->Unknown_5C();
			}
			break;
	}
	State = 0;
}

// 0x6910C0
bool InvTransactionClass::Unknown_6910C0()
{
	return(DropQuery->Active && !strcmp(DropQuery->Type, "UI_TRANSACTION_DUMP_ITEM"));
}

/*
**	Reconstruction notes. The client's file per its asserts ('e:\build\beta\G\app\Client\inv_transaction.cpp' at 0xB90764; lines
**	123-276), its object 0x690880-~0x691160: inv_trade_render.cpp's ends with the initialiser 0x690850. The class name is D24's, from the file.
**	The values of the asserts' names: num == MAX_ITEMS is -1, from_inv == INV_NONE is 0, choice == USER_SELECT_NO is 1.
*/
