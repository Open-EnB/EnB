/*
**	Earth & Beyond client (reconstruction)
**	Original location: e:\build\beta\G\app\shared\. dynamic_obj.cpp is the client's (its asserts, client
**	0x44AC70-0x44B740); this header's name is provisional: no string names it.
**
**	DynObj, the client's name ('DynObj::Update_Proximity' 0x44B090, 'DynObj::Update_Avoidance' 0x44B150): an
**	object the proximity system tracks through its PxWrapper (px_wrapper.cpp). Type 24's base 0x70DBC0
**	(co_subtypes.h) is a ClientObjectClass and a DynObj. Notes at the end of dynamic_obj.cpp.
*/
#if defined(_MSC_VER)
#pragma once
#endif
#ifndef DYNAMIC_OBJ_H  // NAME 1 [dynamic_obj.h]: provisional header name (dynamic_obj.cpp's)
#define DYNAMIC_OBJ_H

#include "vector3.h"
#include "matrix3d.h"
#include "aabox.h"
#include "px_callback.h"

class PxWrapper;

// vtable 0xAE796C, 42 slots; 0x24 bytes. The slots defined here are the client's inline ones (after Update_Avoidance,
// in vtable order). Slot names are neutral except +0x34 / +0x40, which share ClientObjectClass's names: one function
// overrides both in type 24's base (0x70DD40 through the thunk 0x70E2C0, 0x70DDE0 through 0x70E2D0).
class DynObj	// its Get_Name text is 'Unnamed ProximityObject' (0x44B310)
{
public:
	DynObj();											// 0x44AC70
	DynObj(const DynObj &src);							// 0x44ADA0
	virtual ~DynObj();									// +0x00 0x44AE70 (scalar deleting 0x44AD10)
	virtual DynObj *Unknown_Dyn04() { return(this); }	// +0x04 0x44B1F0
	virtual int Unknown_Dyn08() { return(0); }			// +0x08 0x44B210
	virtual int Unknown_Dyn0C() { return(0); }			// +0x0C 0x44B230
	virtual int Unknown_Dyn10() { return(0); }			// +0x10 0x44B250
	virtual int Unknown_Dyn14() { return(0); }			// +0x14 0x44B270
	virtual int Unknown_Dyn18() { return(0); }			// +0x18 0x44B290
	virtual unsigned Unknown_Dyn1C() = 0;				// +0x1C
	virtual int Unknown_Dyn20() = 0;					// +0x20
	virtual bool Unknown_Dyn24(int a) { return(false); }	// +0x24 0x44B2B0  // NAME 1 [a]: neutral
	virtual bool Unknown_Dyn28(int a, int b) { return(false); }	// +0x28 0x44B2D0  // NAME 1 [a, b]: neutral
	virtual bool Unknown_Dyn2C() { return(false); }		// +0x2C 0x44B2F0
	virtual const char *Unknown_Dyn30() { return("Unnamed ProximityObject"); }	// +0x30 0x44B310
	virtual Vector3 Unknown_24() = 0;					// +0x34 (named after ClientObjectClass's slot +0x24, see above)
	virtual Vector3 Unknown_Dyn38() { return(Vector3(1.0f, 0.0f, 0.0f)); }	// +0x38 0x44B330
	virtual Matrix3D Unknown_Dyn3C() { return(Matrix3D(Unknown_24())); }	// +0x3C 0x44B360 (the identity at slot +0x34's position)
	virtual Vector3 Unknown_2C() { return(Vector3(0.0f, 0.0f, 0.0f)); }	// +0x40 0x44B3E0 (named after ClientObjectClass's slot +0x2C)
	virtual float Unknown_Dyn44() { return(0.0f); }		// +0x44 0x44B410
	virtual Vector3 Unknown_Dyn48() = 0;				// +0x48
	virtual Vector3 Unknown_Dyn4C() { return(Vector3(1.0f, 0.0f, 0.0f)); }	// +0x4C 0x44B430
	virtual Matrix3D Unknown_Dyn50() { return(Matrix3D(Unknown_Dyn48())); }	// +0x50 0x44B460
	virtual Vector3 Unknown_Dyn54() { return(Vector3(0.0f, 0.0f, 0.0f)); }	// +0x54 0x44B4E0
	virtual float Unknown_Dyn58() { return(0.0f); }		// +0x58 0x44B510
	virtual float Unknown_Dyn5C() { return(0.0f); }		// +0x5C 0x44B530
	virtual float Unknown_Dyn60() { return(0.0f); }		// +0x60 0x44B550
	virtual void Unknown_Dyn64() = 0;					// +0x64
	virtual void Unknown_Dyn68(Vector3 pos);			// +0x68 0x44AEF0 (G_ASSERT l.84)  // NAME 1 [pos]: type 24's override reads its z
	virtual void Unknown_Dyn6C(const Matrix3D &tm);		// +0x6C 0x44AF20 (G_ASSERT l.93)  // NAME 1 [tm]: type 24's override reads its translation
	virtual Vector3 Unknown_Dyn70() { return(Unknown_Dyn4C()); }	// +0x70 0x44B570
	virtual void Unknown_Dyn74(int a, int b, int c, int d) {}	// +0x74 0x44B5A0 (ret 0x10)  // NAME 1 [a, b, c, d]: neutral
	virtual void Unknown_Dyn78(int a, int b, int c) {}	// +0x78 0x44B5C0 (ret 0xC)  // NAME 1 [a, b, c]: neutral
	virtual AABoxClass Unknown_Dyn7C() { return(Unknown08); }	// +0x7C 0x44B5E0
	virtual int Unknown_Dyn80() { return(0); }			// +0x80 0x44B620
	virtual int Unknown_Dyn84() { return(0); }			// +0x84 0x44B640
	virtual float Unknown_Dyn88() { return(0.0f); }		// +0x88 0x44B660
	virtual float Unknown_Dyn8C() { return(0.0f); }		// +0x8C 0x44B680
	virtual float Unknown_Dyn90(float value) { return(value); }	// +0x90 0x44B6A0  // NAME 1 [value]: what it returns
	virtual float Unknown_Dyn94() { return(0.25f); }	// +0x94 0x44B6C0
	virtual bool Unknown_Dyn98() { return(false); }		// +0x98 0x44B6E0
	virtual bool Unknown_Dyn9C() { return(false); }		// +0x9C 0x44B700
	virtual bool Unknown_DynA0(int a, int b, int c, int d) { return(false); }	// +0xA0 0x44B720  // NAME 1 [a, b, c, d]: neutral
	virtual bool Unknown_DynA4(int a) { return(true); }	// +0xA4 0x44B740  // NAME 1 [a]: neutral

	void *Unknown_44AF50(int type, PxCallbackFunction callback, float radius);			// 0x44AF50 (PxWrapper 0x456F60)  // NAME 1 [type, callback, radius]: the factory 0x452140's
	void *Unknown_44AF80(int type, PxCallbackFunction callback, const Vector3 &extent);	// 0x44AF80 (PxWrapper 0x457190)  // NAME 1 [type, callback, extent]: the factory 0x452460's
	bool Unknown_44AFB0(void *callback);				// 0x44AFB0 (PxWrapper 0x4575E0)  // NAME 1 [callback]: as PxWrapper's
	bool Unknown_44AFD0();								// 0x44AFD0 (PxWrapper +0x0D; fp_tt.cpp's 0x449EA0 calls it)
	bool Unknown_44AFF0();								// 0x44AFF0 (PxWrapper +0x0C)
	bool Unknown_44B010();								// 0x44B010 (PxWrapper +0x0E)
	void Unknown_44B030(bool value);					// 0x44B030 (PxWrapper 0x456890, +0x0D)  // NAME 1 [value]: as PxWrapper's
	void Unknown_44B050(bool value);					// 0x44B050 (PxWrapper 0x456840, +0x0C)  // NAME 1 [value]: as PxWrapper's
	void Unknown_44B070(bool value);					// 0x44B070 (PxWrapper 0x4567D0, +0x0E)  // NAME 1 [value]: as PxWrapper's
	void Update_Proximity();							// 0x44B090 'DynObj::Update_Proximity'
	void Update_Avoidance(const Vector3 &low, const Vector3 &high);	// 0x44B150 'DynObj::Update_Avoidance'  // NAME 1 [low, high]: PxWrapper 0x456900's FAIL_IFs

	float Unknown04;									// +0x04 0 (type 24's slot +0x34 0x70DCF0 stores its radius here; Update_Proximity passes it as PxWrapper::Update's radius)
	AABoxClass Unknown08;								// +0x08 (slot +0x7C returns it by value, the copy constructor copies it)  // NAME 3 [AABoxClass]: 6 dwords returned by value; type a guess
	PxWrapper *ProximityObject;							// +0x20 (new 0x2C bytes, 0x4560B0; the client's name, the FAIL_IF 'ProximityObject->Is_In_List()')
};

#endif
