// Earth & Beyond client (reconstruction): app\client\ui_slot_info.cpp, the slot information screens' base (the client's
// file per its asserts, 'e:\build\beta\G\app\Client\ui_slot_info.cpp' at 0xB8A6DC). Notes at the end of the file.
#include "ui_slot_info.h"
#include "sclient.h"
#include "slot_data.h"
#include "gadget.h"
#include "ui_item_examination.h"
#include "..\shared\g_assert.h"

void Unknown_42ECF0(float *x, float *y);				// 0x42ECF0 (input.h: the pointer's position, 0..1)  // NAME 1 [x, y]: as input.h's

// ui_skill_info.cpp's window (0xCC bytes, 'SKILL_INFO_MAIN', 'fc_skillinfo.ini'), as far as this file calls it
class UISkillInfoClass : public UISlotInfoClass	// NAME 3: class-from-file (D24) for app\Client\ui_skill_info.cpp
{
public:
	UISkillInfoClass(LineDisplayClientClass *client, SkillDataClass *skill);	// 0x62B930  // NAME 1 [client, skill]: what 0x630280 passes
	virtual UISlotInfoItemClass *Unknown_90();
	virtual bool Unknown_94();
	char UnknownB0[0xCC - 0xB0];
};

UISlotInfoClass *UISlotInfoClass::_ActiveInterface = NULL;	// 0xBE5E5C  // NAME 3: the asserts' '_ActiveInterface'

// 0x62FDF0
UISlotInfoClass::UISlotInfoClass(LineDisplayClientClass *client, const char *name) :
	UIDynamicWindowClass(client, name, 8),
	UnknownAC(0)
{
	Unknown84_04()->Unknown_65FFA0(-1);
}

















































// 0x62FEC0
UISlotInfoClass::~UISlotInfoClass()
{
	FAIL_IF(Is_Active()) {
		Unknown_5C();
	}
}





















// 0x62FFB0
void UISlotInfoClass::Unknown_58()
{
	if (!Active) {
		if (_ActiveInterface) {
			G_ASSERT(_ActiveInterface != this);
			_ActiveInterface->Unknown_5C();
		}
		_ActiveInterface = this;
		UIDynamicWindowClass::Unknown_58();
	}
}




// 0x630020
void UISlotInfoClass::Unknown_5C()
{
	if (Active) {
		G_ASSERT(this == _ActiveInterface);
		_ActiveInterface = NULL;
		UIDynamicWindowClass::Unknown_5C();
	}
}

// 0x630070
// NOT EXACT: functional (D64), from the pseudocode
bool UISlotInfoClass::Unknown_630070(UIViewClass *view, int slot, bool a, bool b)
{
	if (Active) {
		if (UnknownAC == slot) {
			Unknown_7C(NULL, false);
			return(false);
		}
		a = true;
	}
	Unknown_7C(NULL, false);
	UnknownAC = slot;
	if (slot == -1) return(false);
	if (!Unknown_94()) return(false);
	Unknown_58A580(Vector2(0.0f, 0.0f));
	Vector2 half(0.0f, 0.0f);
	if (MainWindow) {
		Vector2 size = MainWindow->Get_Size();
		half.X = size.X * 0.5f;
		half.Y = size.Y * 0.5f;
	}
	Vector2 position;
	if (!b) {
		float x, y;
		Unknown_42ECF0(&x, &y);
		x = x * 640.0f - 320.0f;
		y = 240.0f - y * 480.0f;
		position.X = (x < 0.0f) ? x + half.X : x - half.X;
		position.Y = (y < 0.0f) ? y + half.Y : y - half.Y;
	} else {
		Get_Center(position);
	}
	Unknown_58A580(position);
	Unknown_7C(view, a);
	return(true);
}






































































// 0x630280
UISlotInfoClass *Unknown_630280(SClient *client, SlotDataClass *slot_data)
{
	InvDataClass *inv = slot_data->Unknown_10();
	if (inv) {
		return(new UIItemExaminationClass((LineDisplayClientClass *)client, Create_Peek((UISlotInfoItemClass *)inv)));
	}
	SkillDataClass *skill = slot_data->Unknown_14();
	if (skill) {
		return(new UISkillInfoClass((LineDisplayClientClass *)client, skill));
	}
	G_ASSERT(false);
	return(NULL);
}


/*
**	Notes (SK1, 2026-11-09).
**	The object runs from 0x62FDF0 to the adjustor thunks 0x630460-0x630480. Written from the pseudocode, the
**	asserts on the client's lines (the ones so marked, D64, are functional only); 0x62FEC0 inlines 0x630020 (G_ASSERT l.128 inside the FAIL_IF l.83).
*/
