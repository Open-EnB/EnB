// Earth & Beyond client (reconstruction)
// clienttest: stand-ins for aux_roots.cpp's Player constructor 0x752510, which sclient.cpp's Init 0x7518A0 news, and
// its Cargo item constructor 0x6FA150 (the Hull root's inventory, Init's too).
// aux_roots.obj stays out of the harness (clienttest.cpp stands in for its factory 0x6F6000, LNK2005 otherwise), and
// Init does not run here; the classes are declared only as far as their names (this file sees no aux_roots.h).
#include "clienttest.h"

class DeltaPlayerClass { public: DeltaPlayerClass(); };
DeltaPlayerClass::DeltaPlayerClass() { Trace("stub DeltaPlayerClass::DeltaPlayerClass (0x752510, Init: not run)"); }
class DeltaItemClass { public: DeltaItemClass(const char *name); };
DeltaItemClass::DeltaItemClass(const char *) { Trace("stub DeltaItemClass::DeltaItemClass (0x6FA150, Init: not run)"); }
