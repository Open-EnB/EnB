// Earth & Beyond client (reconstruction): app\client\ui_mfd_extended_context_base.h: the multi-function display's
// extended context panel's base class.
// Interim placement (D137): out of ui_mfd_extended_context.h (the client's __FILE__ 0xB82458, the Watch's FAIL_IF),
// which needs DeltaCompoundClass (kv_encoder.h, delta.h, name.h, permstr.h): the client's
// ui_mfd_extended_context_strings.obj has no PermStringInit (CRT table: its run 0x5C1090-0x5C1F60 closes with
// <string>'s guard 0x5C1F60 and opens with none; harvestable's 0x5BF6E0 and ui_mfdm's 0x5C1F90 open theirs), so
// the class it derives from came from a header without the Watch.
#ifndef UI_MFD_EXTENDED_CONTEXT_BASE_H  // NAME 1 [ui_mfd_extended_context_base.h]: neutral, what it declares (D10)
#define UI_MFD_EXTENDED_CONTEXT_BASE_H

#include "refcount.h"
#include "ref_ptr.h"

class UIMFDClass;
class ClientObjectClass;

// The panel (vtable 0xAF3894, slots +0x08..+0x10 pure; constructor 0x5BF600, destructor 0x5BF660, deleting 0x5BF630, all in
// the object before ui_mfd_extended_context_harvestable.cpp's)
class UIMFDExtendedContextClass : public RefCountClass	// NAME 3: D24 class-from-file for ui_mfd_extended_context.h
{
public:
	UIMFDExtendedContextClass(UIMFDClass *owner);		// 0x5BF600  // NAME 2 [owner]: what it holds
	virtual ~UIMFDExtendedContextClass();				// 0x5BF660 (deleting 0x5BF630)
	virtual void Unknown_08() = 0;						// +0x08 shown (0x5C38A0)
	virtual void Unknown_0C() = 0;						// +0x0C hidden (0x5C38C0)
	virtual void Unknown_10() = 0;						// +0x10 every frame (0x5C38E0)

	static RefCountPtr<UIMFDExtendedContextClass> Create(ClientObjectClass *self, ClientObjectClass *co, UIMFDClass *owner);	// 0x5BF4F0  // NAME 2: what it does; NAME 1 [self]: neutral (D10)

	UIMFDClass *Owner;									// +0x08  // NAME 2: what it holds
};

#endif
