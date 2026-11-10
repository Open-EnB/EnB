// Earth & Beyond client (reconstruction): app\client\drag_slot_data.h, a slot window's drag (the client's
// drag_slot_data.cpp per its asserts, 'e:\build\beta\G\app\Client\drag_slot_data.cpp' at 0xB8FC84; the header's name is
// the client's own: its inline Begin's G_ASSERT 'false' names 'e:\build\beta\G\app\Client\drag_slot_data.h', line 44).
// Laid out from the constructor 0x6851B0 (table 0xAFC47C).
#ifndef DRAG_SLOT_DATA_H  // NAME 3 [drag_slot_data.h]: the G_ASSERT's __FILE__
#define DRAG_SLOT_DATA_H

#include "drag_mouse.h"
#include "..\shared\g_assert.h"

class SClient;
class SlotDataRenderClass;	// slot_data_render.h's (reference counted)

// drag_slot_data.cpp's drag of a slot window's slot (0x24 bytes; ui_slot_data.cpp's 0x62DF90 news it as the window's
// SlotDrag): the dragged slot's render follows the pointer
class DragSlotDataClass : public SMouseDragClass	// NAME 3: class-from-file (D24) for app\Client\drag_slot_data.cpp
{
public:
	DragSlotDataClass(SClient *client, SlotDataRenderClass *render);	// 0x6851B0 (FAIL_IF '!SlotRender' l.64)  // NAME 1 [client, render]: what 0x62DF90 passes
	virtual ~DragSlotDataClass();						// +0x00 0x6852F0 (deleting 0x6852C0)
	virtual bool Drop();								// +0x08 0x685570 ('TextClick02.wav')
	virtual bool Cancel();								// +0x0C 0x6855A0 (FAIL_IF '!SlotRender' l.196)
	virtual bool Update(float x, float y);				// +0x10 0x685380 (FAIL_IFs '!SlotRender' l.99, '!scene' l.113; 'SR_ziprollover02.wav')  // NAME 1 [x, y]: the base's
	void Unknown_685500(int slot, int mode, bool arg);	// 0x685500 (FAIL_IF '!SlotRender' l.155: the base's Begin, the slot into the render)  // NAME 1 [slot, mode, arg]: what ui_inv.cpp's 0x5A05B0 passes
	int Get_Slot();										// 0x685610 (FAIL_IF '!SlotRender' l.210)  // NAME 3: ui_skills.cpp's assert 'from_slot != SlotDrag->Get_Slot()'

	SClient *Client;									// +0x14  // NAME 1: neutral (D10)
	SlotDataRenderClass *SlotRender;					// +0x18 (referenced)  // NAME 3: the FAIL_IFs'
	int Unknown1C;										// +0x1C (1, 2, 3: how much is moved; ui_inv.cpp's)  // NAME 1: neutral (D10)
	bool Unknown20;										// +0x20 (ui_inv.cpp's 0x5A0EB0: all)  // NAME 1: neutral (D10)













	virtual void Begin() { G_ASSERT(false); }			// +0x04 0x685290 (only Unknown_685500 begins this drag)
};

#endif // DRAG_SLOT_DATA_H
