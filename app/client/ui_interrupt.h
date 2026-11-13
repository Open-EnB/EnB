// Earth & Beyond client (reconstruction): app\client\ui_interrupt.h, the scanner's interrupt progress display (the
// client's ui_interrupt.cpp per its G_ASSERT l.125; this header's name is provisional, NAME 1). Laid out from the
// constructor 0x59E2C0 and the destructor 0x59E540 (table 0xAF235C); moved out of ui_scan.h, which includes it (O15).
#ifndef UI_INTERRUPT_H  // NAME 1 [ui_interrupt.h]: provisional header name for ui_interrupt.cpp
#define UI_INTERRUPT_H

#include "sclient.h"		// ui_cpit.h's UnknownObjectWatchClass (interim placement, D137)
#include "..\shared\duration.h"
#include "..\common\interrupt_defs.h"

class UIScanClass;
class RenderObjClass;
class LayerClass;
class ClientObjectClass;
class TrailGaugeGadgetClass;

// The threat's +0x2C (0x48 bytes, constructor 0x59E2C0, table 0xAF235C; ui_scan.cpp's 0x60E6A0 makes it when an interrupt
// starts, 0x60FEE0 / 0x60E6A0 call 0x59E970 and delete it): a watcher of the target with its two bars, 'inter_bar%d' (the
// interrupt's progress, tooltip INT_PROGRESS) and 'inter2_bar%d' (the skill's, SKILL_PROGRESS), both in section UI_INTERRUPT
#pragma pack(push, 4)		// (as before the move: the client's new asks 0x48)
// The owner, a plain base declared first: the constructor stores +0x18 before the watcher base (0x59E2E9; ui_experience.cpp
// 0x58C450's finding, docs/attempts/0x58C450). MSVC lays the base with the vtable first and constructs in declaration order.
class Unknown59E2C0OwnerClass	// NAME 1: neutral (D10)
{
public:
	Unknown59E2C0OwnerClass(UIScanClass *owner) : Owner(owner) {}	// NAME 2 [owner]: what it holds
	UIScanClass *Owner;									// +0x18 in the display (its +0x48 view and +0x64 gadget list)  // NAME 2: what it holds
};

class Unknown59E2C0Class : public Unknown59E2C0OwnerClass, public UnknownObjectWatchClass	// NAME 1: neutral (D10)
{
public:
	Unknown59E2C0Class(RenderObjClass *robj, LayerClass *layer, UIScanClass *owner, ClientObjectClass *object, int unknown);	// 0x59E2C0 (0x60E6A0's: the threat's model, the scanner's layer, the scanner, the target, -1)  // NAME 1 [robj, layer, owner, object, unknown]: what 0x60E6A0 passes
	virtual ~Unknown59E2C0Class();						// +0x00 0x59E540 (deleting 0x59E510)
	virtual void Unknown_08(ClientObjectClass *object);	// +0x08 0x59E700 (the bars emptied, then the base's)  // NAME 1 [object]: the base's
	virtual void Unknown_Watch_0C();					// +0x0C 0x59E680 (the bars emptied)
	virtual void Unknown_10();							// +0x10 0x59E790 (every frame: the bars from +0x40 and the times; G_ASSERT l.125)
	void Unknown_59E600();								// 0x59E600 (the bars emptied; 0x60E6A0's before the times)
	void Unknown_59E970();								// 0x59E970 (the bars out of the scanner's +0x64; 0x60E6A0 / 0x60FEE0 before deleting it)
	void Unknown_60EC70(DurationClass time);			// 0x60EC70 (ui_scan.cpp's, after 0x60E6A0: the client clock)  // NAME 1 [time]: neutral
	void Unknown_60ECA0(DurationClass time);			// 0x60ECA0 (ui_scan.cpp's, after 0x60E6A0: the target's InterruptibleActivationTime)  // NAME 1 [time]: neutral

	TrailGaugeGadgetClass *Unknown1C;					// +0x1C ('inter_bar%d': the interrupt's progress)
	TrailGaugeGadgetClass *Unknown20;					// +0x20 ('inter2_bar%d': the skill's progress)
	int Unknown24;										// +0x24 (the constructor leaves it)
	DurationClass StartTime;							// +0x28 (0x60EC70; G_ASSERT '(ActivationTime - StartTime) > Millisecond(0)')  // NAME 3: the assert's
	DurationClass ActivationTime;						// +0x30 (0x60ECA0)  // NAME 3: the assert's
	InterruptStateType Unknown38;						// +0x38 (0x60F1A0: the threat's state +0x28; 1 'Powering Up' / 3 'Interrupt Failed' run the skill's bar)
	float Unknown3C;									// +0x3C (the skill's progress: the times' share, 0..1)
	float Unknown40;									// +0x40 (0x60F1A0: the target's InterruptProgress, else 0)
	int Unknown44;										// +0x44
};
#pragma pack(pop)

#endif
