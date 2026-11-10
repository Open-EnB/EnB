// Earth & Beyond client (reconstruction): app\client\drag_slot_data.cpp, a slot window's drag (the client's file per its
// asserts, 'e:\build\beta\G\app\Client\drag_slot_data.cpp' at 0xB8FC84). Notes at the end of the file.
#include "drag_slot_data.h"
#include "sclient.h"
#include "slot_data_render.h"
#include "local_interface_sort.h"
#include "vector3.h"
#include "..\audio\soundlib.h"
#include "..\shared\g_assert.h"













































// 0x6851B0
DragSlotDataClass::DragSlotDataClass(SClient *client, SlotDataRenderClass *render) :
	SMouseDragClass(client->Mouse),
	Client(client),
	SlotRender(NULL),
	Unknown1C(0),
	Unknown20(false)
{
	REF_PTR_SET(SlotRender, render);
	FAIL_IF(!SlotRender) return;
	SlotRender->Unknown_30(6);
	SlotRender->Unknown_6A8EC0();
}

// 0x6852F0
DragSlotDataClass::~DragSlotDataClass()
{
	REF_PTR_RELEASE(SlotRender);
}




















// 0x685380
// NOT EXACT: functional (D64), from the pseudocode
bool DragSlotDataClass::Update(float x, float y)
{
	bool moved = Update_Drag(x, y);
	FAIL_IF(!SlotRender) return(moved);
	if (State == 2) {
		if (moved) {
			SoundLibraryClass::Play_2D_Sound("SR_ziprollover02.wav", 2);
			UIViewClass *view = Client->CurrentView;
			SceneClass *scene = NULL;
			if (view) {
				view = view->Unknown_1C();
				if (view) {
					scene = (SceneClass *)view->Unknown_50();
				}
			}


			FAIL_IF(!scene) return(moved);
			InterfaceSortClass sort(scene, true);
			SlotRender->Unknown_6A8B60(scene);
		}
		Vector3 position(x * 640.0f - 320.0f, y * -480.0f + 240.0f, -1.0f);
		SlotRender->Unknown_6A8CC0(position);
	}
	return(moved);
}


























// 0x685500
// NOT EXACT: functional (D64), from the pseudocode
void DragSlotDataClass::Unknown_685500(int slot, int mode, bool arg)
{
	SMouseDragClass::Begin();
	Unknown20 = arg;
	Unknown1C = mode;
	FAIL_IF(!SlotRender) return;
	SlotRender->Unknown_40(slot);
}

// 0x685570
bool DragSlotDataClass::Drop()
{
	if (SMouseDragClass::Drop()) {
		SoundLibraryClass::Play_2D_Sound("TextClick02.wav", 2);
		return(true);
	}
	return(false);
}























// 0x6855A0
// NOT EXACT: functional (D64), from the pseudocode
bool DragSlotDataClass::Cancel()
{
	if (SMouseDragClass::Cancel()) {
		FAIL_IF(!SlotRender) return(true);
		SlotRender->Unknown_6A8C40();
		SlotRender->Unknown_40(-1);
		return(true);
	}
	return(false);
}




// 0x685610
int DragSlotDataClass::Get_Slot()
{
	FAIL_IF(!SlotRender) return(-1);
	return(SlotRender->Get_Slot());
}


/*
**	Notes (SK1, 2026-11-09).
**	The object runs from 0x6851B0 to 0x685680 (0x685290, the table's +0x04, is drag_slot_data.h's inline Begin: its
**	G_ASSERT 'false' names the header, line 44). Written from the pseudocode, the asserts on the client's lines; the
**	functions so marked (D64) are functional only.
*/
