// Earth & Beyond client (reconstruction): app\Client\local_interface_sort.cpp. Notes at the end of the file.
#define __INTRFACE_MEMBERS
#include "intrface.h"
#include "local_interface_sort.h"
#include "interface_static_sort_list.h"
#include "..\shared\unknown_debug.h"

















































// NOT EXACT: inline-budget (0.48; the client's case 2 calls RefCountPtr's assignment out of line, 0x6B55D0, EH state 5)
// 0x6E0E80
InterfaceSortClass::InterfaceSortClass(InterfaceClass *ui) :
	SortList(NULL)
{
	FAIL_IF(!ui) return;
	ui = ui->Unknown78;
	FAIL_IF(!ui) return;
	SortList = ui->Unknown_541800();
	if (!SortList) return;
	switch (ui->Unknown74) {




		default:
			G_ASSERT(false);
		case 0:
			Previous = Create_Peek(SortList->Set_Current_Interface(ui));
			break;
		case 1:
			Previous = Create_Peek(SortList->Set_Current_Front());
			break;
		case 2:
			Previous = Create_Peek(SortList->Set_Current_Back());
			break;
	}
}




































































// 0x6E18B0
InterfaceSortClass::~InterfaceSortClass()
{
	if (SortList) {
		SortList->Set_Current_Interface(Previous.Peek());
		SortList = NULL;
	}
}

/*
**	Reconstruction notes
**
**	The client's window 0x6E0E80-0x6E2280 (the asserts' lines 61-153): the constructors over an interface
**	(0x6E0E80: its top interface's (+0x78) sort mode +0x74 picks the current interface, the front or the back
**	list), 0x6E1090 ('!SortList' 91), 0x6E1200 ('!ui_scene' 100), 0x6E1390 ('!castable_ui_scene' 113),
**	0x6E1560 ('!uiview' 130) and over a render object 0x6E1730 ('!scene' 147), the destructor 0x6E18B0 and
**	0x6E1940-0x6E2280. Rebuilt here: 0x6E0E80 and 0x6E18B0 (the windows' scope, 0x541980, is intrface.cpp's);
**	the others are not yet.
**
**	0x6E0E80: the client's case 2 calls RefCountPtr<InterfaceClass>'s assignment out of line (0x6B55D0) where
**	cases 0 and 1 inline it.
*/
