// Earth & Beyond client (reconstruction): app\client\ui_ping.h, the ping window (the client's ui_ping.cpp per its
// asserts; this header's name is provisional, NAME 1). Laid out from the constructor 0x5D7630 and the destructor
// 0x5D7A00 (PG1, 2026-11-06).
#ifndef UI_PING_H  // NAME 1 [ui_ping.h]: provisional header name for ui_ping.cpp
#define UI_PING_H

#include "intrface.h"
#include "..\..\wwlib\ref_ptr.h"

class RenderObjClass;		// ww3d2's
class FontSys;				// fontmanager.h's
class DynamicWindowClass;	// dynamic_window_class.h's
class Unknown51C800Class;	// unknown_51c800.h's
class ButtonGadgetClass;	// unknown_675120.h's

// ui_ping.cpp's ping window (0xC0 bytes; the cockpit's 0x693230 makes it, tables 0xAF5768 / 0xAF575C / 0xAF5750):
// the 'TSI05C' mesh with its 'color' button lit by the round trip time, the efficiency text, and a dialog box of
// three rows (latency, data volume, efficiency) shown by key 1
class UIPingClass : public InterfaceClass	// NAME 3: class-from-file (D24) for app\Client\ui_ping.cpp
{
public:
	UIPingClass(LineDisplayClientClass *client);		// 0x5D7630 (0xC0 bytes)  // NAME 1 [client]: InterfaceClass's
	virtual ~UIPingClass();								// 0x5D7A00 (deleting 0x5D79D0)
	virtual void Unknown_54();							// +0x54 0x5D7D60 (the mesh, its gadgets, the texts)
	virtual void Unknown_58();							// +0x58 0x5D83D0 (activated: the texts updated)
	virtual void Unknown_5C();							// +0x5C 0x5D8460 (deactivated: the dialog box taken off)
	virtual void Unknown_60();							// +0x60 0x5D84A0 (every frame: the texts once a second)
	virtual bool Unknown_68(int key);					// +0x68 0x5D8510 (key 1: the dialog box on / off)  // NAME 1 [key]: InterfaceClass's
	void Unknown_5D85B0();								// 0x5D85B0 (the connection's stats: FAIL_IF '!client' line 256, 'NET_STATS' lines 265-271)

	RefCountPtr<RenderObjClass> Mesh;					// +0x84 ('TSI05C')  // NAME 1: neutral
	RefCountPtr<FontSys> Efficiency;					// +0x88 ('PING_EFFICIENCY' on the mesh's bone 'value')  // NAME 1: its style's
	RefCountPtr<DynamicWindowClass> Window;				// +0x8C ('DIALOG_BOX_WINDOW', level 3)  // NAME 1: as unknown_msgbox.cpp's
	RefCountPtr<DynamicWindowClass> Info;				// +0x90 ('DIALOG_BOX_INFO', 1: the header's row)  // NAME 1: as unknown_msgbox.cpp's
	RefCountPtr<Unknown51C800Class> Header;				// +0x94 ('HEADER_INFO')  // NAME 1: its style's
	RefCountPtr<DynamicWindowClass> Unknown098;			// +0x98 ('DIALOG_BOX_INFO', 4: the rows)
	RefCountPtr<Unknown51C800Class> Rows[3];			// +0x9C ('PING_INFO_0': latency, data volume, efficiency)  // NAME 1: neutral
	ButtonGadgetClass *Button;							// +0xA8 (the mesh's 'color', gadget 1)  // NAME 1: neutral
	int Unknown0AC;										// +0xAC
	int Unknown0B0;										// +0xB0
	int Unknown0B4;										// +0xB4
	float Timer;										// +0xB8 (seconds since the texts were updated)  // NAME 1: neutral
	bool Shown;											// +0xBC (the dialog box is in the scene)  // NAME 1: neutral
};

#endif // UI_PING_H
