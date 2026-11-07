// Earth & Beyond client (reconstruction): app\client\ui_mfd_extended_context.h (the client's __FILE__ 0xB82458): the
// multi-function display's extended context, the ref-counted panel a monitored object adds to the display.
#ifndef UI_MFD_EXTENDED_CONTEXT_H
#define UI_MFD_EXTENDED_CONTEXT_H

#include "refcount.h"
#include "ref_ptr.h"
#include "..\shared\g_assert.h"
#include "..\shared\auxdata\delta.h"

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

RefCountPtr<UIMFDExtendedContextClass> Unknown_5BF790(ClientObjectClass *self, ClientObjectClass *co, UIMFDClass *owner);	// 0x5BF790 (ui_mfd_extended_context_harvestable.cpp's '0 == co', l.45)  // NAME 1: neutral (D10)

// The panel kept while an object's 'HarvestableResources' has no contents yet (0x20 bytes, vtable 0xAF395C): it makes the
// real panel, ExtendedContext, once the field is set and drops it when it empties
class UIMFDExtendedContextWatchClass : public UIMFDExtendedContextClass	// NAME 1: neutral (D10)
{
public:
	UIMFDExtendedContextWatchClass(DeltaCompoundClass *field, ClientObjectClass *self, ClientObjectClass *co, UIMFDClass *owner) :
		UIMFDExtendedContextClass(owner),
		Self(self),
		CO(co),
		Field(field),
		ExtendedContext(NULL),
		Shown(false)
	{
	}


	// 0x5C0B70
	static RefCountPtr<UIMFDExtendedContextWatchClass> Create(DeltaCompoundClass *field, ClientObjectClass *self, ClientObjectClass *co, UIMFDClass *owner);	// NAME 2 [field]: what it holds

	virtual void Unknown_08();							// 0x5C0C80
	virtual void Unknown_0C();							// 0x5C0CB0
	virtual void Unknown_10();							// 0x5C0CE0
	void Update_Context();								// 0x5C0D30  // NAME 2: what it does

	ClientObjectClass *Self;							// +0x0C  // NAME 1: neutral (D10)
	ClientObjectClass *CO;								// +0x10  // NAME 2: what it holds
	DeltaCompoundClass *Field;							// +0x14  // NAME 2: what it holds
	RefCountPtr<UIMFDExtendedContextClass> ExtendedContext;	// +0x18  // NAME 5: the client's assert
	bool Shown;											// +0x1C  // NAME 2: what it holds
};

inline RefCountPtr<UIMFDExtendedContextWatchClass> UIMFDExtendedContextWatchClass::Create(DeltaCompoundClass *field, ClientObjectClass *self, ClientObjectClass *co, UIMFDClass *owner)
{
	FAIL_IF(0 == co) {
		return(NULL);
	}
	return(Create_NEW(new UIMFDExtendedContextWatchClass(field, self, co, owner)));
}

inline void UIMFDExtendedContextWatchClass::Unknown_08()
{
	Update_Context();
	Shown = true;
	if (ExtendedContext != NULL) {
		ExtendedContext->Unknown_08();
	}
}

inline void UIMFDExtendedContextWatchClass::Unknown_0C()
{
	Update_Context();
	Shown = false;
	if (ExtendedContext != NULL) {
		ExtendedContext->Unknown_0C();
	}
}

inline void UIMFDExtendedContextWatchClass::Unknown_10()
{
	Update_Context();
	if (ExtendedContext != NULL) {
		ExtendedContext->Unknown_10();
	}
}











inline void UIMFDExtendedContextWatchClass::Update_Context()
{
	if (Field->Unknown70 != 0) {
		if (ExtendedContext == NULL) {
			UIMFDClass *owner = Owner; ExtendedContext = Unknown_5BF790(Self, CO, owner);







			G_ASSERT(ExtendedContext);
			if (Shown && ExtendedContext != NULL) {
				ExtendedContext->Unknown_08();
			}
		}
	} else if (ExtendedContext != NULL) {
		if (Shown) {
			ExtendedContext->Unknown_0C();
		}
		ExtendedContext = NULL;
	}
}

#endif
