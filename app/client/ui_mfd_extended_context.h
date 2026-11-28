// Earth & Beyond client (reconstruction): app\client\ui_mfd_extended_context.h (the client's __FILE__ 0xB82458): the
// multi-function display's extended context, the ref-counted panel a monitored object adds to the display.
#ifndef UI_MFD_EXTENDED_CONTEXT_H
#define UI_MFD_EXTENDED_CONTEXT_H

#include "ui_mfd_extended_context_base.h"	// UIMFDExtendedContextClass (refcount.h, ref_ptr.h)

#include "..\shared\g_assert.h"
#include "..\shared\kv_encoder.h"	// delta.h, and <string>: the client's object ends with <string>'s initialiser (0x5BF6B0); which header gave it is not known

class MFDInterface;
class ClientObjectClass;

// UIMFDExtendedContextClass, the panel every extended context derives from, is in ui_mfd_extended_context_base.h:
// ui_mfd_extended_context_strings.cpp derives from it without this header (its client object has no PermStringInit,
// so it never saw kv_encoder.h's delta.h / name.h / permstr.h, which the Watch below needs for DeltaContainer).













RefCountPtr<UIMFDExtendedContextClass> Unknown_5BF790(ClientObjectClass *self, ClientObjectClass *co, MFDInterface *owner);	// 0x5BF790 (ui_mfd_extended_context_harvestable.cpp's '0 == co', l.45)  // NAME 1: neutral (D10)

// The panel kept while an object's 'HarvestableResources' has no contents yet (0x20 bytes, vtable 0xAF395C): it makes the
// real panel, ExtendedContext, once the field is set and drops it when it empties
class UIMFDExtendedContextWatchClass : public UIMFDExtendedContextClass	// NAME 1: neutral (D10)
{
public:
	UIMFDExtendedContextWatchClass(DeltaContainer *field, ClientObjectClass *self, ClientObjectClass *co, MFDInterface *owner) :
		UIMFDExtendedContextClass(owner),
		Self(self),
		CO(co),
		Field(field),
		ExtendedContext(NULL),
		Shown(false)
	{
	}


	// 0x5C0B70
	static RefCountPtr<UIMFDExtendedContextWatchClass> Create(DeltaContainer *field, ClientObjectClass *self, ClientObjectClass *co, MFDInterface *owner);	// NAME 2 [field]: what it holds

	virtual void Unknown_08();							// 0x5C0C80
	virtual void Unknown_0C();							// 0x5C0CB0
	virtual void Unknown_10();							// 0x5C0CE0
	void Update_Context();								// 0x5C0D30  // NAME 2: what it does

	ClientObjectClass *Self;							// +0x0C  // NAME 1: neutral (D10)
	ClientObjectClass *CO;								// +0x10  // NAME 2: what it holds
	DeltaContainer *Field;							// +0x14  // NAME 2: what it holds
	RefCountPtr<UIMFDExtendedContextClass> ExtendedContext;	// +0x18  // NAME 5: the client's assert
	bool Shown;											// +0x1C  // NAME 2: what it holds
};

inline RefCountPtr<UIMFDExtendedContextWatchClass> UIMFDExtendedContextWatchClass::Create(DeltaContainer *field, ClientObjectClass *self, ClientObjectClass *co, MFDInterface *owner)
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
			MFDInterface *owner = Owner; ExtendedContext = Unknown_5BF790(Self, CO, owner);







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
