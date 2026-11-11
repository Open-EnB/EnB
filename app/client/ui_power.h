// Earth & Beyond client (reconstruction): app\client\ui_power.h, the power gauges (energy, shields, hull). The file name
// is provisional (NAME 1: the client holds no __FILE__ of it; its tooltips' section is UI_POWER and it links between
// ui_ptrade / ui_proc's neighbours). Laid out from the constructor 0x5DBFC0, the destructor 0x5DC2C0 and the table
// 0xAF5D70; the cockpit (ui_cpit.cpp), the group window's members (ui_group.cpp) and the MFD (ui_mfd.cpp) make it.
#ifndef UI_POWER_H  // NAME 1 [ui_power.h]: provisional header name
#define UI_POWER_H

#include "ui_cpit.h"		// UnknownObjectWatchClass (interim placement, D137)

class RenderObjClass;
class LayerClass;
class InterfaceClass;
class ClientObjectClass;
class UIPowerGaugeClass;	// ui_power.cpp's (over gd_gauge.h's gauge)

// The energy, shield and hull gauges of a mesh (0x28 bytes, table 0xAF5D70): a watcher of a ship whose slot +0x10 fills
// them from its EnergyPercent / ShieldPercent / HullPoints every frame
class UIPowerClass : public UnknownObjectWatchClass	// NAME 1: neutral (D10), from the tooltips' section UI_POWER
{
public:
	UIPowerClass(RenderObjClass *mesh, LayerClass *layer, InterfaceClass *parent, int arg);	// 0x5DBFC0 ('reactor_bar%d', 'shield_bar%d', 'hull_bar%d')  // NAME 1 [mesh, layer, parent, arg]: what the set-ups pass
	virtual ~UIPowerClass();							// +0x00 0x5DC2C0 (deleting 0x5DC290)
	virtual void Unknown_08(ClientObjectClass *object);	// +0x08 0x5DC470 (the gauges emptied, then the base's)  // NAME 1 [object]: the base's
	virtual void Unknown_Watch_0C();					// +0x0C 0x5DC450 (the gauges emptied)
	virtual void Unknown_10();							// +0x10 0x5DC4A0 (every frame)
	void Unknown_5DC390();								// 0x5DC390 (the gauges emptied: NO_SHIELDS / NO_ENERGY / NO_HULL; ui_group.cpp calls it)
	void Unknown_5DCAE0();								// 0x5DCAE0 (the gauges out of the parent's +0x64, before its delete)
	void operator = (ClientObjectClass *object) { Unknown_08(object); }	// (the base's: the implicit copy assignment hides it; ui_cpit.cpp sets +0xD0's object so)

	InterfaceClass *Parent;								// +0x18  // NAME 1: what its callers pass
	UIPowerGaugeClass *EnergyGauge;						// +0x1C ('reactor_bar%d', tooltip ENERGY)  // NAME 2: its tooltip
	UIPowerGaugeClass *ShieldGauge;						// +0x20 ('shield_bar%d', tooltip SHIELDS)  // NAME 2: its tooltip
	UIPowerGaugeClass *HullGauge;						// +0x24 ('hull_bar%d', tooltip HULL)  // NAME 2: its tooltip
};

#endif
