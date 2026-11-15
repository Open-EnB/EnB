// Earth & Beyond client (reconstruction): app\input\unknown_42f360.cpp, the drag tracker (notes at the end)
#include "unknown_42f360.h"

// @0x42F360
InputUnknownAE71D0Class::InputUnknownAE71D0Class(unsigned short button, float threshold, unsigned int click_time) :
	Button(button),
	State(0),
	Active(false),
	Threshold(threshold * threshold),
	ClickTime(click_time)
{
}

// @0x42F3A0
// NOT EXACT (0.49, attempts.py 0x42F3A0): ours keeps 0 in ebx (one more saved register), the client in edi
bool InputUnknownAE71D0Class::Handle_Event(KernelEventClass *event)
{
	bool mine = (event->Key == Button);
	int action;
	if (mine) {
		action = (event->Flags != 0);
	} else {
		if (State == 0) {
			return false;
		}
		action = 2;
	}
	Vector2 pos(event->Unknown04, event->Unknown08);
	switch (action) {
		case 0:
			if (State == 0) {
				State = 1;
				PressTime = event->Unknown0C;
				Start = pos;
				Unknown_Track_Mouse(1);
				Press(pos);
				Active = true;
				return mine;
			}
			State = 3;
			Active = true;
			break;

		case 1:
			switch (State) {
				case 2:
					Unknown_Track_Mouse(-1);
					State = 0;
					break;
				case 3:
					State = 0;
					if (event->Unknown0C - PressTime < ClickTime) {
						Click(Start);
					}
					Unknown_Track_Mouse(-1);
					break;
				case 4:
				{
					Vector2 min = Start;
					if (min.X > pos.X) {
						float t = min.X;
						min.X = pos.X;
						pos.X = t;
					}
					if (min.Y > pos.Y) {
						float t = min.Y;
						min.Y = pos.Y;
						pos.Y = t;
					}
					State = 0;
					Unknown_Track_Mouse(-1);
					Drag_End(min, pos);
					break;
				}
			}
			Active = false;
			break;

		case 2:
			switch (State) {
				case 1:
					if (event->Unknown0C - PressTime > ClickTime) {
						State = 2;
						return mine;
					}
					break;
				case 2:
				case 4:
					break;
				default:
					return mine;
			}
			if (!Active) {
				if (State == 2) {
					Unknown_Track_Mouse(-1);
					State = 0;
				}
				return mine;
			}
			{
				float dx = pos.X - Start.X;
				float dy = pos.Y - Start.Y;
				if (dx * dx + dy * dy > Threshold) {
					if (State != 4) {
						Drag_Begin(Start);
						State = 4;
					}
					Vector2 min = Start;
					if (min.X > pos.X) {
						float t = min.X;
						min.X = pos.X;
						pos.X = t;
					}
					if (min.Y > pos.Y) {
						float t = min.Y;
						min.Y = pos.Y;
						pos.Y = t;
					}
					Drag(min, pos);
				}
			}
			break;
	}
	return mine;
}

/*
**	Reconstruction notes
**
**	client.exe: the object between input.cpp's (to 0x42F340 with input.h's inlines) and winmsg.cpp's (0x42F880):
**	the constructor 0x42F360, Handle_Event 0x42F3A0 and the five empty virtuals 0x42F7E0 / 0x42F800 / 0x42F820 /
**	0x42F840 / 0x42F860 of table 0xAE71D0 (one slot each, no destructor). Nothing in the client constructs it or
**	calls it (no caller of 0x42F360, no other reference to 0xAE71D0): its own file, kept by the linker for its
**	non-inline functions. No string names the file or the class (D10, D137).
**	Handle_Event's TrackMouseCounter asserts carry input.h's __FILE__ on line 174: input.h's
**	Unknown_Track_Mouse, inlined five times.
**	The event: Key against Button, Flags non-zero for the release, Unknown04 / Unknown08 the position and
**	Unknown0C the time (unsigned arithmetic against ClickTime: jbe / jae).
*/
