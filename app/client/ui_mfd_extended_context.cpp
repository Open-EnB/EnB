// Earth & Beyond client (reconstruction): app\client\ui_mfd_extended_context.cpp, the extended context panel's base and
// its factory (no __FILE__ of its own: this name is provisional, NAME 1; the object sits between ui_mfd.cpp's and
// ui_mfd_extended_context_harvestable.cpp's, client 0x5BF4B0-~0x5BF6A0)
#include <string.h>
#include "ui_mfd_extended_context.h"

void Unknown_5BF730();	// 0x5BF730 (ui_mfd_extended_context_harvestable.cpp's: 'fa_harvesting.ini' loaded)  // NAME 1: neutral (D10)
void Unknown_5BF760();	// 0x5BF760 (the same file unloaded)  // NAME 1: neutral (D10)

// 0x5BF4B0
void Unknown_5BF4B0()
{
	Unknown_5BF730();
}

// 0x5BF4D0
void Unknown_5BF4D0()
{
	Unknown_5BF760();
}

// 0x5BF4F0
RefCountPtr<UIMFDExtendedContextClass> UIMFDExtendedContextClass::Create(ClientObjectClass *self, ClientObjectClass *co, UIMFDClass *owner)
{
	if (co == NULL) {
		return(NULL);
	}
	RefCountPtr<UIMFDExtendedContextClass> context = Unknown_5BF790(self, co, owner);
	if (context != NULL) {
		return(context);
	}
	return(NULL);
}

// 0x5BF600
UIMFDExtendedContextClass::UIMFDExtendedContextClass(UIMFDClass *owner) :
	Owner(owner)
{
}

// 0x5BF660
UIMFDExtendedContextClass::~UIMFDExtendedContextClass()
{
}
