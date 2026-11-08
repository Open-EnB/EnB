#include "unknown_45a090.h"

Unknown45A090Class::Unknown45A090Class(RenderStateListClass *list, int id, int link_id, unsigned int flags) :
	RenderStateClass(list, id, link_id, flags)
{
}

Unknown45A090Class::Unknown45A090Class(RenderStateListClass *list, const Unknown45A090Class &source) :
	RenderStateClass(list, source)
{
}

RenderStateClass *Unknown45A090Class::Unknown_04(RenderStateListClass *list)
{
	return(new Unknown45A090Class(list, *this));
}

Unknown45A090Class::~Unknown45A090Class()
{
}

void Unknown45A090Class::Unknown_08(RsTrackClass *owner, int link_id)
{
	if (LinkID != link_id) {
		if (Is_Enabled() && Link != NULL && Link->Is_Enabled()) {
			Link->Unknown_18(owner);
		}
		LinkID = link_id;
		Link = NULL;
		Unknown_458010();
		if (Link != NULL) {
			if (Is_Enabled()) {
				if (!Link->Is_Enabled()) {
					Link->Unknown_14(owner);
				}
			} else if (Link->Is_Enabled()) {
				Link->Unknown_18(owner);
			}
		}
	}
}

RenderObjClass *Unknown45A090Class::Peek_Render_Object()
{
	if (Link != NULL) {
		return(Link->Peek_Render_Object());
	}
	return(NULL);
}

void Unknown45A090Class::Unknown_14(RsTrackClass *owner)
{
	Unknown_457E10();
}


/*
** unknown_45a090.cpp: reconstruction notes
**
** The client's window 0x45A090-0x45A2A0, between rs_hanim.cpp (ends 0x45A010) and rs_list.cpp (starts
** 0x45A2B0, its static's initialiser): the 'link' render state, vtable 0xAE7ED4 (slots +0x00 0x45A0D0,
** +0x04 0x45A130, +0x08 0x45A1D0, +0x14 0x45A290, +0x20 0x45A270; the rest are RenderStateClass's). No
** assert, no __FILE__ string: the directory is known (app\shared, between two of its files), the name is
** neutral (D10); the alphabetical link order of app\shared puts it between rs_hanim.cpp and rs_list.cpp
** (rs_link.cpp would fit, unconfirmed).
*/
