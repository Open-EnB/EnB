#if defined(_MSC_VER)
#pragma once
#endif

#ifndef RS_HANIM_H
#define RS_HANIM_H

#include "rs_track.h"
#include "hanim.h"

class HTreeClass;

// rs_hanim.cpp's animation state (vtable 0xAE7E78, 0x60 bytes; rs.ini TYPE 'anim'): activated, it finds the
// animation for the owner's model (HAnimName on the model's hierarchy, or the hierarchy's own name plus
// HAnimSuffix), plays it in its combo channel, and at its end moves on to the state NextID names.
// Unknown_44 / Unknown_48 fade the channel's weight out / in over a time.
class Unknown4596A0Class : public RenderStateClass	// NAME 1: neutral (D10)
{
public:
	Unknown4596A0Class(RenderStateListClass *list, int id, int link_id, unsigned int flags, const char *hAnimName, const char *suffix, int next_id, HAnimComboDataClass *combo);	// 0x4596A0  // NAME 2 [hAnimName]: the assert's text; NAME 1 [suffix, next_id, combo]: rs_list.cpp's keys
	Unknown4596A0Class(RenderStateListClass *list, const Unknown4596A0Class &source);	// 0x4597E0  // NAME 1 [list, source]: neutral
	virtual ~Unknown4596A0Class();									// 0x459940 (deleting 0x4597B0)
	virtual RenderStateClass *Unknown_04(RenderStateListClass *list);	// +0x04 0x4598B0
	virtual void Unknown_14(RsTrackClass *owner);					// +0x14 0x459D50 (finds the animation for the owner's model)
	virtual void Unknown_18(RsTrackClass *owner);					// +0x18 0x459FA0
	virtual void Unknown_30(RsTrackClass *owner, float seconds);	// +0x30 0x459B10 (steps the frame)
	virtual void Unknown_34(float seconds);							// +0x34 0x459D20
	virtual void Unknown_38(RsTrackClass *owner);					// +0x38 0x459FE0 (hands the combo channel to the owner)
	virtual void Unknown_3C();										// +0x3C 0x45A010
	virtual void Unknown_44(float seconds);							// +0x44 0x459A30 (fades the weight out)
	virtual void Unknown_48(float seconds);							// +0x48 0x459AA0 (fades the weight in)
	const char *Get_HAnim_Name() const { return(HAnimName); }		// (the copy constructor reads both through getters)  // NAME 1: after HAnimName
	const char *Get_HAnim_Suffix() const { return(HAnimSuffix); }	// NAME 1: after HAnimSuffix
	RenderStateClass *Get_Next_Render_State();						// 0x4599E0 (inlined into its callers)  // NAME 1: the assert's NextRenderState

	HAnimComboDataClass Combo;										// +0x2C  // NAME 1: neutral
	int NextID;														// +0x44 (the next state's id)  // NAME 1: after NextRenderState
	RenderStateClass *NextRenderState;								// +0x48  // NAME 2: the assert's text
	char *HAnimName;												// +0x4C (nstrdup'd)  // NAME 2: the assert's text
	char *HAnimSuffix;												// +0x50 (nstrdup'd)  // NAME 2: the assert's text
	const HTreeClass *HTree;										// +0x54 (the hierarchy the animation was found for)  // NAME 1: neutral
	float Unknown58;												// +0x58 (the channel's weight while fading, -1 when not)  // NAME 1: neutral (D10)
	float Unknown5C;												// +0x5C (the weight's change per millisecond)  // NAME 1: neutral (D10)
};

#endif
