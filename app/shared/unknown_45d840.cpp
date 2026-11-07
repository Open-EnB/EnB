// Earth & Beyond client (reconstruction): the message render state, 0x45D840-0x45D9C6 (PROVISIONAL file, D137:
// no __FILE__; its own object between rs_list.cpp's template copies and rs_proj_sup.cpp). Notes at the end.
#include "rs_track.h"

// 0x45D840
Unknown45D840Class::Unknown45D840Class(RenderStateListClass *list, int id, int link_id, unsigned int flags, int message) :
	RenderStateClass(list, id, link_id, flags),
	Message(message)
{
}

// 0x45D8C0
Unknown45D840Class::Unknown45D840Class(RenderStateListClass *list, const Unknown45D840Class &source) :
	RenderStateClass(list, source),
	Message(source.Message)
{
}

// 0x45D900
RenderStateClass *Unknown45D840Class::Unknown_04(RenderStateListClass *list)
{
	return(new Unknown45D840Class(list, *this));
}

// 0x45D990
Unknown45D840Class::~Unknown45D840Class()
{
}

// 0x45D9B0: activated, the state hands itself and its message to the owner
void Unknown45D840Class::Unknown_14(RsTrackClass *owner)
{
	owner->Unknown_1D8((int)this, Message);
}

/*
** Reconstruction notes (RS1, 2026-11-07)
**
** rs.ini's TYPE 'message' (rs_list.cpp's 0x45B1E0 / 0x45B280). A file of its own: rs_list.cpp's maker 0x45B1E0
** calls the constructor through its incremental-link thunk instead of inlining it, the code sits after rs_list.cpp's
** template copies (0x45CA00-0x45D820), and its vtable 0xAE8050 after rs_list.cpp's last one (0xAE8034).
** RsTrackClass::Unknown_1D8 takes the state as an int (rs_track.h's declaration).
*/
