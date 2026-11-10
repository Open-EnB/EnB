// Earth & Beyond client (reconstruction): GadgetCollectionInterface, the side of a window that takes gadgets and
// emits their events (moved here from ui_dynamicwindow.h; no __FILE__ names its header: interim placement, D137).
#ifndef GADGET_COLLECTION_INTERFACE_H  // NAME 1 [gadget_collection_interface.h]: interim header name (D137) from the class
#define GADGET_COLLECTION_INTERFACE_H

class GadgetClass;

// A gadget's event, emitted with the collection as sender (dynamic_window_toggle.cpp's 0x51E730 compares the id)
class GadgetEvent  // NAME 5: the client's RTTI .?AVGadgetEvent@@ (MemberCallback<GadgetCollectionInterface, ..., GadgetEvent>)
{
public:
	int Id;																						// +0x00 (the gadget's id: slot +0x10's)  // NAME 1: what it holds
};

// The windows' gadget side (ui_dynamicwindow.cpp's +0x84, table 0xAF1380: the destructor 0x58A030 / deleting 0x58A050
// and four pure slots). The panels and the toggle buttons get it as their listener; the callbacks' sender type
// (typeid(GadgetCollectionInterface), the toggle's MemberCallback 0x51E980) names it
class GadgetCollectionInterface  // NAME 5: the client's RTTI .?AVGadgetCollectionInterface@@
{
public:
	virtual ~GadgetCollectionInterface() {}													// 0x58A030 (deleting 0x58A050)
	virtual GadgetClass *Unknown84_04() = 0;													// +0x04 (0x58AA40: +0x8C, the main gadget)  // NAME 1: neutral (D10)
	virtual void Unknown84_08(GadgetClass *gadget, int arg1, int arg2) = 0;					// +0x08 (0x58AA60: FAIL_IF 'Is_Active()', '!gadget')  // NAME 1 [gadget, arg1, arg2]: neutral
	virtual void Unknown84_0C(GadgetClass *gadget) = 0;										// +0x0C (0x58AD00: out of the gadget list)  // NAME 1 [gadget]: neutral
	virtual int Unknown84_10() = 0;															// +0x10 (0x58AD80: +0x90, the next id)  // NAME 1: neutral (D10)
};

#endif
