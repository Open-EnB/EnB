// Earth & Beyond client (reconstruction): app\client\unknown_5bb250.h, the interface base over a mesh and a layer (the
// object 0x5BB250-0x5BB6D0 between ui_mesh.cpp and ui_mfd.cpp; it has no assert, so the client names neither the file
// nor the class: interim placement, D137, NAME 1). Moved out of sclient.h (L3, run 2026-11-10b); sclient.h includes it.
#ifndef UNKNOWN_5BB250_H  // NAME 1 [unknown_5bb250.h]: provisional name (D137): the client's file sorts between ui_mesh.cpp and ui_mfd.cpp
#define UNKNOWN_5BB250_H

#include "intrface.h"
#include "layer.h"

class UIMeshClass;		// ui_mesh.h
class UIViewClass;		// sclient.h (uiview.cpp's view)
class SClient;
class GadgetClass;

// The base of every window drawn as an animated mesh (0xAC bytes; the cockpit, the group window, the PDA pages, the
// starbase windows: 30 constructors call 0x5BB250 / 0x5BB320): intrface.cpp's InterfaceClass, a UIMeshClass it owns and
// the layer its gadgets are made over. Tables 0xAF3390 (36 slots: InterfaceClass's 34 and +0x88 / +0x8C), 0xAF3384
// (GClass's second base at +0x04, the adjustor 0x5BB6B0) and 0xAF3378 (the RefCountClass at +0x38, the adjustor 0x5BB6C0).
// The slots it does not override are InterfaceClass's (intrface.h). The layout and every slot are settled from the
// twelve functions and the 30 derived tables; a derived window only includes this header.
class Unknown5BB320Class : public InterfaceClass	// NAME 1: neutral (D10)
{
public:
	Unknown5BB320Class(UIMeshClass *mesh, SClient *client);	// 0x5BB320 (the mesh at +0x84, the client to 0x540D30)  // NAME 1 [mesh, client]: what 0x593BD0 passes
	Unknown5BB320Class(UIMeshClass *mesh, UIViewClass *view, const LayerClass &layer);	// 0x5BB250 (the view and the layer's scene to 0x540A90, the layer copied)  // NAME 1 [mesh, view, layer]: what 0x57D830 passes
	virtual ~Unknown5BB320Class();						// +0x00 0x5BB3B0 (deletes the mesh; the deleting destructor 0x5BB2F0)
	virtual void Unknown_58();							// +0x58 0x5BB550 (activates: the mesh's model into the layer's scene, then InterfaceClass's, under a sort scope)
	virtual void Unknown_5C();							// +0x5C 0x5BB5E0 (deactivates: the model out of its scene, then InterfaceClass's; the group window's destructor calls it first)
	virtual void Unknown_60();							// +0x60 0x5BB660 (every frame: the mesh's processes, 0x5B9410; InterfaceClass's is not called)
	virtual void Unknown_7C(UIViewClass *view, bool arg);	// +0x7C 0x5BB470 (the layer onto the view's scene and camera, or off them, then InterfaceClass's)  // NAME 1 [view, arg]: InterfaceClass's
	virtual void Unknown_88() {}						// +0x88 0x543750 (inline: emitted in ui_analyze.cpp, the first object that makes one; only the starbase window overrides it, 0x638CB0)
	virtual void Unknown_8C() {}						// +0x8C 0x543770 (inline, as +0x88; the starbase window's 0x638DA0)
	void Unknown_5BB4F0(GadgetClass *gadget);			// 0x5BB4F0 (onto the mesh's gadgets, 0x5B99C0)  // NAME 1 [gadget]: what 0x594040 passes
	void Unknown_5BB520(GadgetClass *gadget);			// 0x5BB520 (the mesh's 0x5B99F0; nothing calls it)  // NAME 1 [gadget]: UIMeshClass's

	UIMeshClass *InterfaceMesh;							// +0x84 (the destructor deletes it)  // NAME 3: the FAIL_IFs' '!InterfaceMesh' (ui_faction.cpp's 0x58D400 l.118; ui_attrb_skills.cpp's 0x549AA0 l.121 names its own +0xBC)
	LayerClass Layer;									// +0x88 (0x24 bytes, to +0xAC: LayerClass::LayerClass in the constructors; the gadgets are made over it)  // NAME 2: what it is
};

#endif // UNKNOWN_5BB250_H
