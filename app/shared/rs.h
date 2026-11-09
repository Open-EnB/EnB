// Earth & Beyond client (reconstruction)
/*
**	This file is a reconstruction from the retail client.exe (SHA-256 249f8524...51c4).
**	Original location: app\shared\rs.h (PROVISIONAL, D137: the header of rs.cpp, whose asserts name the masks)
**
**	RenderStateClass (rs.cpp, 0x457B60-0x4586F0) and the RS_* flag masks, moved out of rs_track.h.
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef RS_H  // NAME 1 [rs.h]: provisional header name for rs.cpp
#define RS_H

#include "listnode.h"

class RenderObjClass;
class RsTrackClass;
class RenderStateListClass;


// the flag groups rs.cpp's asserts name (0x458370-0x458460)
#define RS_MUTABLE_FLAG_MASK	0x300										// NAME 3 [RS_MUTABLE_FLAG_MASK]: the asserts' text
#define RS_THREAD_FLAG_MASK		0x3000										// NAME 3 [RS_THREAD_FLAG_MASK]: the asserts' text (the loader thread's bits 12 / 13)
#define RS_DATA_PRESENT			9											// NAME 3 [RS_DATA_PRESENT]: rs_robj.cpp's assert text (Test_Flag(RS_DATA_PRESENT): bit 9, 0x200, set with the model)

// rs.cpp's render state, declared as far as rs.cpp and rs_track.cpp use it
class RenderStateClass : public Node<RenderStateClass *>	// NAME 2: rs_track.cpp's assert names State and Get_Render_State_List()
{
public:
	RenderStateClass(RenderStateListClass *list, int id, int link_id, unsigned int flags);	// 0x457B60 (rs_robj.cpp's constructors call it)  // NAME 1 [list, id, link_id, flags]: the members they set
	RenderStateClass(RenderStateListClass *list, const RenderStateClass &source);	// 0x457BE0 (the derived states' copies)  // NAME 1 [list, source]: neutral
	virtual ~RenderStateClass();									// 0x457D20 (deleting 0x457BB0)
	virtual RenderStateClass *Unknown_04(RenderStateListClass *list) = 0;	// +0x04 (pure: E&B's _purecall 0x9CCD10; 0x458970 clones into list)  // NAME 1 [list]: neutral
	virtual void Unknown_08(RsTrackClass *owner, int link_id) {}	// +0x08 0x458590 (INIT_RENDER_STATE 47)  // NAME 1 [owner, link_id]: neutral; what 0x460880 passes
	virtual void Unknown_0C() {}									// +0x0C 0x4585B0  // NAME 1: neutral (D10)
	virtual bool Unknown_10() { return(true); }						// +0x10 0x4585D0 (the loader thread 0x458150 runs it)  // NAME 1: neutral (D10)
	virtual void Unknown_14(RsTrackClass *owner) = 0;				// +0x14 (pure; 0x460440 activates through it)  // NAME 1 [owner]: neutral
	virtual void Unknown_18(RsTrackClass *owner);					// +0x18 0x4582E0 (0x460710 deactivates through it)  // NAME 1 [owner]: neutral
	virtual void Unknown_1C(RsTrackClass *owner);					// +0x1C 0x458330  // NAME 1 [owner]: neutral
	virtual RenderObjClass *Peek_Render_Object() { return(NULL); }	// +0x20 0x4585F0  // NAME 2: the assert 'Peek_Render_Object()' at rs_track.cpp line 236
	virtual void Unknown_24(void *value, bool recursive);			// +0x24 0x4582C0 (rs_robj.cpp's 0x45EC90 passes them to Set_User_Data)  // NAME 1 [value, recursive]: Set_User_Data's
	virtual void Unknown_28(int a, int b) {}						// +0x28 0x458610  // NAME 1 [a, b]: neutral
	virtual void Unknown_2C(int a, int b) {}						// +0x2C 0x458630  // NAME 1 [a, b]: neutral
	virtual void Unknown_30(RsTrackClass *owner, float seconds);	// +0x30 0x458350 (rs_robj.cpp's 0x45ECD0 adds seconds to its timer)  // NAME 1 [owner, seconds]: Unknown_45A870's
	virtual void Unknown_34(float seconds) {}						// +0x34 0x458650 (0x4584B0 passes the time left over)  // NAME 1 [seconds]: neutral
	virtual void Unknown_38(RsTrackClass *owner) {}				// +0x38 0x458670 (rs_hanim.cpp's 0x459FE0 hands its channel to owner)  // NAME 1 [owner]: neutral
	virtual void Unknown_3C() {}									// +0x3C 0x458690 (ACTIVATE_NEXT_RENDER_STATE 49)  // NAME 1: neutral (D10)
	virtual void Unknown_40(float scale, float *hsv) {}				// +0x40 0x4586B0 (rs_robj.cpp's 0x45F280 remakes its model at scale / hsv)  // NAME 1 [scale, hsv]: what the asset manager's Unknown_78 takes
	virtual void Unknown_44(float seconds) {}						// +0x44 0x4586D0 (rs_hanim.cpp's 0x459A30 fades out over seconds)  // NAME 1 [seconds]: neutral
	virtual void Unknown_48(float seconds) {}						// +0x48 0x4586F0 (rs_hanim.cpp's 0x459AA0 fades in over seconds)  // NAME 1 [seconds]: neutral
	void Unknown_457E10();											// 0x457E10 (flag 4 on, into the list's +0x20 chain)  // NAME 1: neutral (D10)
	void Unknown_457E90();											// 0x457E90 (flag 4 off, unlinked)  // NAME 1: neutral (D10)
	void Unknown_457EE0(const char *text);							// 0x457EE0 (copies text into Unknown14)  // NAME 1 [text]: neutral
	void Unknown_457F60(RenderStateListClass *list);				// 0x457F60 (pushes the chain onto list's Head)  // NAME 1 [list]: neutral
	void Unknown_458010();											// 0x458010 (resolves Link from LinkID)  // NAME 1: neutral (D10)
	void Unknown_458040();											// 0x458040 (waits for the loader thread)  // NAME 1: neutral (D10)
	void Unknown_4580D0();											// 0x4580D0 (starts the loader thread 0x458150)  // NAME 1: neutral (D10)
	void Unknown_458370(int flag);									// 0x458370 (sets a RS_MUTABLE_FLAG_MASK flag)  // NAME 1 [flag]: the asserts' text
	void Unknown_4583C0(int flag);									// 0x4583C0 (clears it)  // NAME 1 [flag]: the asserts' text
	void Unknown_458410(int flag);									// 0x458410 (sets a RS_THREAD_FLAG_MASK flag)  // NAME 1 [flag]: the asserts' text
	void Unknown_458460(int flag);									// 0x458460 (clears it)  // NAME 1 [flag]: the asserts' text
	void Unknown_4584B0(RsTrackClass *owner, float seconds, RenderStateClass *next);	// 0x4584B0  // NAME 1 [owner, seconds, next]: neutral; 0x45ECD0 passes the time left over
	void Unknown_458540(const char *text);							// 0x458540 (Unknown24 from text)  // NAME 1 [text]: neutral
	RenderStateListClass *Get_Render_State_List() { return(List); }	// the assert at rs_track.cpp line 191
	bool Is_Enabled() const { return((Flags >> 4) & 1); }			// NAME 2: rs_list.cpp's assert 'state->Is_Enabled()' (lines 198, 215, 226)
	bool Unknown_Flag10() const { return((Flags >> 10) & 1); }		// NAME 1: neutral (D10)
	bool Unknown_Flag12() const { return((Flags >> 12) & 1); }		// NAME 1: neutral (D10)
	bool Unknown_Flag13() const { return((Flags >> 13) & 1); }		// NAME 1: neutral (D10)
	bool Unknown_Flag16() const { return((Flags >> 16) & 1); }		// NAME 1: neutral (D10)
	bool Test_Flag(int flag) const { return((Flags >> flag) & 1); }	// NAME 2: rs_robj.cpp's assert text
	const char *Get_Source_Filename() const { return(Unknown14); }	// NAME 2: rs_robj.cpp's assert text (0x45EF80 loads it when the model is missing)

	RenderStateClass *Next;											// +0x0C  // NAME 2: the list walk
	RenderStateListClass *List;										// +0x10  // NAME 2: Get_Render_State_List()
	char *Unknown14;												// +0x14 (0x457EE0's copy)  // NAME 1: neutral (D10)
	int ID;															// +0x18  // NAME 2: the render state id the packets name
	int LinkID;														// +0x1C  // NAME 2: -1 ends the chain (INIT_RENDER_STATE's LinkID)
	RenderStateClass *Link;											// +0x20  // NAME 2: the next state of the chain
	char *Unknown24;												// +0x24 (nstrdup'd from the source's)  // NAME 1: neutral (D10)
	unsigned int Flags;												// +0x28  // NAME 2: bit tests
};

#endif
