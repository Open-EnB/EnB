// Earth & Beyond client (reconstruction): app\client\drag_mouse.cpp, a drag of the software mouse (the client's file per
// its asserts, 'e:\build\beta\G\app\Client\drag_mouse.cpp' at 0xB8FC14). Notes at the end of the file.
#include "drag_mouse.h"
#include "tasks\tsk_login.h"
#include "..\shared\g_assert.h"































// 0x684F20
// NOT EXACT: functional (D64), from the pseudocode
SMouseDragClass::SMouseDragClass(LoginUnknown18Class *mouse) :
	Mouse(mouse),
	X(0.0f),
	Y(0.0f),
	State(STATE_INACTIVE)
{
	G_ASSERT(Mouse);
}

// 0x684FB0
SMouseDragClass::~SMouseDragClass()
{
	Mouse = NULL;
}


// 0x684FD0
void SMouseDragClass::Begin()
{
	G_ASSERT(State == STATE_INACTIVE);
	State = 1;
	if (Mouse) {
		Mouse->Unknown_515510(this);
		float y = Mouse->Get_Y();
		X = Mouse->Get_X();
		Y = y;
	}
}

// 0x685040
float SMouseDragClass::Threshold()
{
	return(0.0009f);
}

// 0x685060
// NOT EXACT: functional (D64), from the pseudocode
bool SMouseDragClass::Update_Drag(float x, float y)
{
	if (State == 1) {
		if ((x - X) * (x - X) + (y - Y) * (y - Y) > Threshold()) {
			State = 2;
			return(true);
		}
	}
	if (State == 2 && Mouse) {
		Mouse->Unknown_5153B0();
	}
	return(false);
}

// 0x6850E0
bool SMouseDragClass::Drop()
{
	G_ASSERT(State != STATE_INACTIVE);
	if (State == 2) {
		State = 3;
		if (Mouse) {
			Mouse->Unknown_515400();
		}
		return(true);
	}
	Cancel();
	return(false);
}

// 0x685150
bool SMouseDragClass::Cancel()
{
	if (State != STATE_INACTIVE) {
		State = STATE_INACTIVE;
		if (Mouse) {
			Mouse->Unknown_5155A0(this);
			Mouse->Unknown_515400();
		}
		return(true);
	}
	return(false);
}

// 0x685190
void SMouseDragClass::Unknown_685190()
{
	if (State != STATE_INACTIVE) {
		Cancel();
	}
}


/*
**	Notes (SK1, 2026-11-09).
**	The object runs from 0x684F20 to 0x6851A0 (0x684D10-0x684E00 before it are rawfile.cpp's and a palette loader's).
**	SMouseDragClass is smouse.cpp's name for the object (its local declaration, 0x515510's parameter type). Written
**	from the pseudocode, the asserts on the client's lines; the functions so marked (D64) are functional only.
*/
