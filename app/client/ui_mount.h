// Earth & Beyond client (reconstruction): app\client\ui_mount.h, the equipment mount points window (the client's
// ui_mount.cpp per its asserts, 'e:\build\beta\G\app\Client\ui_mount.cpp' at 0xB82A4C; this header's name is provisional,
// NAME 1, D137). Laid out from the constructor 0x5C73B0, the destructor 0x5C74D0 and Build 0x5C77A0 (MT1, 2026-11-09).
#ifndef UI_MOUNT_H  // NAME 1 [ui_mount.h]: provisional header name for ui_mount.cpp
#define UI_MOUNT_H

#include "ui_inv.h"		// UIInvClass, ui_inv.cpp's inventory window base
#include "sclient.h"

class CameraClass;
class RenderObjClass;

// One mount point's button (0x28 bytes; the window holds 20, Inventory.Mounts' count)
struct UIMountPointStruct	// NAME 1: neutral (D10)
{
	RenderObjClass *Button;								// +0x00 'Equipbtn'  // NAME 2: the model's name
	RenderObjClass *Bttn;								// +0x04 the button's sub-object 'bttn'  // NAME 2: its name
	RenderObjClass *MntPoint;							// +0x08 'MntPoint', at the bone's projection  // NAME 2: the model's name
	int Unknown0C;										// +0x0C (the button's slot +0x7C of Bttn)  // NAME 1: neutral (D10)
	RenderObjClass *Name;								// +0x10 the FontSys 'MOUNT_NAME' on the button's bone 'frame'  // NAME 2: its string
	float X;											// +0x14 the bone's projection, -1..1, then the button's screen position  // NAME 2: describes what it holds
	float Y;											// +0x18  // NAME 2: describes what it holds
	float PointX;										// +0x1C the bone's screen position  // NAME 2: describes what it holds
	float PointY;										// +0x20  // NAME 2: describes what it holds
	RenderObjClass *Line;								// +0x24 the 0xF4-byte line from the button to the point (0x6DF890)  // NAME 2: describes what it is
};

// The mount points window (0x404 bytes, tables 0xAF3DE8 / 0xAF3DDC / 0xAF3DD0; 0x58B680 makes it): the player ship's
// mount bones (Inventory.MountBoneNames on Inventory.EquipMountModel) projected through the camera, a button at each
class UIMountClass : public UIInvClass  // NAME 3: class-from-file (D24) for ui_mount.cpp
{
public:
	UIMountClass(SClient *client, CameraClass *camera, int kind, int unused);	// 0x5C73B0 (deleting 0x5C7470; 0x58B680 passes 0 last)  // NAME 1 [client, camera, kind, unused]: what it does with them
	virtual ~UIMountClass();							// 0x5C74D0
	virtual void Unknown_54();							// 0x5C74A0 (the base's, then Build)
	virtual void Unknown_58();							// 0x5C7570 (Build under a sort scope; built: hidden, the base's)
	virtual void Unknown_5C();							// 0x5C7600 (hidden, the base's)
	virtual bool Is_Shown() { return(Shown); }			// +0xD4 (inline: 0x58B770, emitted with 0x58B680's file)  // NAME 2: what it returns
	virtual void Show() { if (!Shown) Toggle(); }		// +0xD8 (inline: 0x58B790)  // NAME 2: what it does
	virtual void Hide() { if (Shown) Toggle(); }		// +0xDC (inline: 0x58B7C0)  // NAME 2: what it does
	virtual void Toggle();								// +0xE0 0x5C7630 (the points into the view's scene, or out of it)  // NAME 2: what it does
	void Build();										// 0x5C77A0  // NAME 2: what it does
	void Clear();										// 0x5C85D0  // NAME 2: what it does

	CameraClass *Camera;								// +0xD0 (the constructor's, referenced)  // NAME 2: what it is
	UIMountPointStruct Points[20];						// +0xD4  // NAME 2: what they are
	int Unknown3F4;										// +0x3F4 (not set by the constructor)  // NAME 1: neutral (D10)
	RenderObjClass *Model;								// +0x3F8 Inventory.EquipMountModel, while Build runs  // NAME 2: the field's name
	float Distance;										// +0x3FC the camera's distance that shows the model whole (-1)  // NAME 2: describes what it holds
	bool Built;											// +0x400  // NAME 2: describes what it holds
	bool Unknown401;									// +0x401 (true)  // NAME 1: neutral (D10)
	bool Unknown402;									// +0x402 (chooses the model's transform: 0xBF92D0 when set, else 0xBF9330)  // NAME 1: neutral (D10)
	bool Shown;											// +0x403  // NAME 2: describes what it holds
};

#endif // UI_MOUNT_H
