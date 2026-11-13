// 0x58C450 UIExperienceClass::UIExperienceClass (P3, run 2026-11-11b): the form that leaves ONLY GenericNode() inlined
// (with GenericNode() forced out of line the function is byte-exact, 406 bytes).
// The client stores Parent (+0x18) BEFORE the watcher base's vtable 0xAF0DD0 and before its node (0x58C473 against
// 0x58C476 / 0x58C48A): Parent is constructed before the watcher, so it belongs to a base declared FIRST that has no
// vtable (MSVC lays the first base with a vfptr at +0x00 and the plain one after it, but constructs in declaration order).
// ui_power.cpp's 0x5DBFC0 shows the same order (0x5DBFE3 against 0x5DBFE6).
//
// ui_experience.h:
class UnknownParentClass { public: UnknownParentClass(InterfaceClass *parent) : Parent(parent) {} InterfaceClass *Parent; };	// +0x18 in the derived class
class UIExperienceClass : public UnknownParentClass, public UnknownObjectWatchClass { /* ... CombatBar, TradeBar, ExploreBar */ };
// ui_experience.cpp:
UIExperienceClass::UIExperienceClass(RenderObjClass *mesh, InterfaceClass *parent) :
	UnknownParentClass(parent),
	CombatBar(NULL),
	TradeBar(NULL),
	ExploreBar(NULL)
{
	// (body as the tree's)
}
// Tried on top of it, none sends GenericNode() out of line: inline Add / Remove members on the plain base, an inline
// InterfaceClass::Add_Gadget, an Is_Exists accessor, 2 / 4 / 6 dead accessor reads in the body, 1..6 dead inline calls in the
// plain base's constructor. `#pragma inline_depth(0|1|2)` at the file's END does send it out (406 bytes) but breaks
// 0x58BFE0 (~RefCountPtr's Release_Ref goes out of line there).
