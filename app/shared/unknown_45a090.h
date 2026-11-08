#if defined(_MSC_VER)
#pragma once
#endif

#ifndef UNKNOWN_45A090_H
#define UNKNOWN_45A090_H

#include "rs_track.h"

// The 'link' render state (vtable 0xAE7ED4, 0x2C bytes; rs.ini TYPE 'link'): it owns no data of its own;
// Unknown_08 re-points it at another state (the chain's LinkID) and carries its own enabled state over
// to that state, Peek_Render_Object answers with the linked state's model.
class Unknown45A090Class : public RenderStateClass	// NAME 1: neutral (D10)
{
public:
	Unknown45A090Class(RenderStateListClass *list, int id, int link_id, unsigned int flags);	// 0x45A090
	Unknown45A090Class(RenderStateListClass *list, const Unknown45A090Class &source);	// 0x45A100  // NAME 1 [list, source]: neutral
	virtual ~Unknown45A090Class();									// 0x45A1B0 (deleting 0x45A0D0)
	virtual RenderStateClass *Unknown_04(RenderStateListClass *list);	// +0x04 0x45A130
	virtual void Unknown_08(RsTrackClass *owner, int link_id);		// +0x08 0x45A1D0 (re-links to link_id)
	virtual void Unknown_14(RsTrackClass *owner);					// +0x14 0x45A290
	virtual RenderObjClass *Peek_Render_Object();					// +0x20 0x45A270 (the linked state's model)
};

#endif
