// Earth & Beyond client (reconstruction): app\client\ui_mfdm.h, the multi-function display's items (the client's ui_mfdm.cpp
// per its asserts; this header's name is provisional, NAME 1). Included by ui_mfd.h after UIMFDItemClass.
#ifndef UI_MFDM_H  // NAME 1 [ui_mfdm.h]: provisional header name for ui_mfdm.cpp
#define UI_MFDM_H

#include "ref_ptr.h"
#include "matrix3d.h"
#include "vector3.h"

class CameraClass;
class SimpleSceneClass;
class LightClass;
class TextureClass;
class FontSys;
class RenderObjClass;
class Unknown47FA70Class;
class UIMFDExtendedContextClass;

// An item that shows a 3-D object in its own scene on the 'scanner_click' viewport (0x54 bytes before its subclass's
// members; constructor 0x5C2000, destructor 0x5C2470, deleting 0x5C2440, vtable 0xAF3AAC, slot +0x30 pure)
class UIMFDPIPItemClass : public UIMFDItemClass	// NAME 1: neutral (D10)
{
public:
	UIMFDPIPItemClass(UIMFDClass *owner);				// 0x5C2000
	virtual ~UIMFDPIPItemClass();						// 0x5C2470
	virtual void Unknown_08();							// 0x5C25C0
	virtual void Unknown_0C();							// 0x5C28D0
	virtual void Unknown_10();							// 0x5C2920
	virtual bool Unknown_14();							// 0x5C25A0
	virtual int Unknown_18() { return((int)this); }		// 0x5C2420
	virtual RenderObjClass *Get_RObj() = 0;				// +0x30  // NAME 2: what it returns

	RefCountPtr<CameraClass> Camera;					// +0x08  // NAME 2: what it holds
	RefCountPtr<SimpleSceneClass> Scene;				// +0x0C  // NAME 2: what it holds
	Matrix3D Transform;									// +0x10 (the camera's when UnknownBool is set)  // NAME 2: what it holds
	bool UnknownBool;									// +0x40 (0: the camera looks at the object)  // NAME 1: neutral (D10)
	RenderObjClass *PIPVP;								// +0x44  // NAME 5: the client's assert
	float Width;										// +0x48  // NAME 2: the viewport's size
	float Height;										// +0x4C  // NAME 2: the viewport's size
	LightClass *Light;									// +0x50  // NAME 5: the client's assert
};

// 0x5BEA00's item: the display's object through a carrier (0x58 bytes; constructor 0x5C2CD0, vtable 0xAF3AFC)
class Unknown5C2CD0Class : public UIMFDPIPItemClass	// NAME 1: neutral (D10)
{
public:
	Unknown5C2CD0Class(UIMFDClass *owner);				// 0x5C2CD0
	virtual ~Unknown5C2CD0Class();						// 0x5C2DC0
	virtual void Unknown_04(ClientObjectClass *object);	// 0x5C2E60
	virtual void Unknown_08();							// 0x5C2ED0
	virtual void Unknown_10();							// 0x5C2F00
	virtual int Unknown_1C() { return((int)this); }		// 0x5C2D70
	virtual RenderObjClass *Get_RObj();					// 0x5C2E40

	Unknown47FA70Class *Carrier;						// +0x54  // NAME 2: what it holds
};

// 0x5BE570's item: the object rendered once into the viewport's texture (0xC bytes; constructor 0x5C3030, vtable 0xAF3B3C)
class Unknown5C3030Class : public UIMFDItemClass	// NAME 1: neutral (D10)
{
public:
	Unknown5C3030Class(UIMFDClass *owner);				// 0x5C3030
	virtual ~Unknown5C3030Class();						// 0x5C30B0
	virtual void Unknown_08();							// 0x5C3150
	virtual void Unknown_0C();							// 0x5C34C0
	virtual void Unknown_10();							// 0x5C35D0
	virtual bool Unknown_14();							// 0x5C3130
	virtual int Unknown_20() { return((int)this); }		// 0x5C3060

	TextureClass *Texture;								// +0x08 (the viewport's own, kept for 0x5C34C0)  // NAME 2: what it holds
};

// 0x5BEA00's item: the object's extended context (0xC bytes; constructor 0x5C3650, vtable 0xAF3B78)
class Unknown5C3650Class : public UIMFDItemClass	// NAME 1: neutral (D10)
{
public:
	Unknown5C3650Class(UIMFDClass *owner);				// 0x5C3650
	virtual ~Unknown5C3650Class();						// 0x5C36D0
	virtual void Unknown_04(ClientObjectClass *object);	// 0x5C3790
	virtual void Unknown_08();							// 0x5C38A0
	virtual void Unknown_0C();							// 0x5C38C0
	virtual void Unknown_10();							// 0x5C38E0
	virtual bool Unknown_14();							// 0x5C3900
	virtual int Unknown_2C() { return((int)this); }		// 0x5C3680

	RefCountPtr<UIMFDExtendedContextClass> Context;		// +0x08  // NAME 2: what it holds
};

// 0x5BE570's item: position, heading, pitch and direction lines (0x44 bytes; constructor 0x5C3920, vtable 0xAF3BB4)
class Unknown5C3920Class : public UIMFDItemClass	// NAME 1: neutral (D10)
{
public:
	Unknown5C3920Class(UIMFDClass *owner);				// 0x5C3920
	virtual ~Unknown5C3920Class();						// 0x5C3AB0
	virtual void Unknown_08();							// 0x5C3B80
	virtual void Unknown_0C();							// 0x5C3C00
	virtual void Unknown_10();							// 0x5C3C40
	virtual bool Unknown_14();							// 0x5C3B40
	virtual int Unknown_28() { return((int)this); }		// 0x5C3A60

	FontSys *Lines[8];									// +0x08 ('MFD_X_DIRECTION' ...)  // NAME 2: what it holds
	int Pitch;											// +0x28  // NAME 3: its label 'Pitch: %d'
	int Heading;										// +0x2C  // NAME 3: its label 'Heading: %d'
	int Unknown30;										// +0x30
	char DirectionY;									// +0x34  // NAME 2: 'Y %c'
	char DirectionX;									// +0x35  // NAME 2: 'X %c'
	char DirectionZ;									// +0x36  // NAME 2: 'Z %c'
	char Axis;											// +0x37 ('x', 'y', 'z': the largest)  // NAME 2: what it holds
	Vector3 Position;									// +0x38  // NAME 2: what it holds
};

#endif
