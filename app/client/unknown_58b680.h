// Earth & Beyond client (reconstruction): app\client\unknown_58b680.h, the hull mount points window: ui_mount.cpp's
// window over a model of its own, the player's ship rendered into its 'ship' part. No string names the file:
// unknown_58b680.cpp and this header are interim names (D137), the object 0x58B630-0x58BF8F (its PermStringInitClass
// static 0x58B630 opens it after ui_dynamicwindow.cpp's thunks; its guarded <string> id 0x58BF40 and the two
// deleting-destructor thunks 0x58BF70 / 0x58BF80 close it, before the next object's PermStringInitClass static).
#ifndef UNKNOWN_58B680_H  // NAME 1 [unknown_58b680.h]: interim header name (D137)
#define UNKNOWN_58B680_H

#include "ui_mount.h"
#include "layer.h"

class ProxyRObj;			// ro_proxy.h's

// The hull mount points window (0x430 bytes; constructor 0x58B680 over ui_mount.cpp's 0x5C73B0, tables 0xAF1438 /
// 0xAF142C / 0xAF1420; no caller of the constructor in the client). The mount points window's buttons over Model,
// whose bones 'vp_tl' / 'vp_br' give the camera's viewport; shown, the ship's model (through Proxy) is rendered into
// a texture on Model's 'ship'; the window is rebuilt when the ship's Inventory.Hull changes.
class UIMountHullClass : public UIMountClass	// NAME 1: neutral, after its base and the field 'Inventory.Hull' it watches
{
public:
	UIMountHullClass(SClient *client, const LayerClass &layer, RenderObjClass *model, int kind);	// 0x58B680 (deleting 0x58B7F0; the base gets the layer's camera and 0 last)  // NAME 1 [client, layer, model, kind]: what it does with them
	virtual ~UIMountHullClass();						// 0x58B9B0
	virtual void Unknown_54();							// +0x54 0x58B820 (the viewport, then the base's)
	virtual void Unknown_58();							// +0x58 0x58BB10 (the base's under a sort scope; the ship's model into Proxy)
	virtual void Unknown_5C();							// +0x5C 0x58BBB0 (the base's; Proxy out of its scene)
	virtual void Unknown_60();							// +0x60 0x58BA70 (Inventory.Hull changed: hidden, cleared, built again; then the base's)
	virtual void Toggle();								// +0xE0 0x58BBF0 (the base's; shown: the ship rendered onto Model's 'ship')
	void Unknown_58B850();								// 0x58B850 (the camera's viewport and view plane from Model's 'vp_tl' / 'vp_br')

	LayerClass Layer;									// +0x404 (the constructor's, copied)  // NAME 1: its type
	RenderObjClass *Model;								// +0x428 (the constructor's; not referenced)  // NAME 1: neutral
	ProxyRObj *Proxy;									// +0x42C (0xC4 bytes, ProxyRObj(false); released by the destructor)  // NAME 1: its type
};

#endif // UNKNOWN_58B680_H
