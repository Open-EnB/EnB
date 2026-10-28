// Earth & Beyond client (reconstruction): app\shared\dynamic_obj.cpp. Notes at the end of the file.
#define ENABLE_WWPROFILE
#include "dynamic_obj.h"
#include "px_wrapper.h"
#include "..\shared\unknown_debug.h"
#include "wwprofile.h"

// 0x44AC70
// NOT EXACT: shape (the client duplicates the epilogue into the allocation-failed path; attempts.py show 0x44AC70)
DynObj::DynObj() :
	Unknown04(0),
	ProximityObject(NULL)
{
	ProximityObject = new PxWrapper(this);
}

// 0x44ADA0
// NOT EXACT: register (the source's PxWrapper in edi, ours in ecx; attempts.py show 0x44ADA0)
DynObj::DynObj(const DynObj &src) :
	Unknown04(src.Unknown04),
	Unknown08(src.Unknown08),
	ProximityObject(NULL)
{
	ProximityObject = new PxWrapper(this, *src.ProximityObject);
}









































// 0x44AE70
DynObj::~DynObj()
{
	FAIL_IF(ProximityObject->Is_In_List()) {
		ProximityObject->Unknown_456730();
	}
	delete ProximityObject;
	ProximityObject = NULL;
}





// 0x44AEF0
void DynObj::Unknown_Dyn68(Vector3 pos)
{
	G_ASSERT(0);
}




// 0x44AF20
void DynObj::Unknown_Dyn6C(const Matrix3D &tm)
{
	G_ASSERT(0);
}

// 0x44AF50
void *DynObj::Unknown_44AF50(int a, int b, int c)
{
	return(ProximityObject->Unknown_456F60(a, b, c));
}

// 0x44AF80
void *DynObj::Unknown_44AF80(int a, int b, int c)
{
	return(ProximityObject->Unknown_457190(a, b, c));
}

// 0x44AFB0
bool DynObj::Unknown_44AFB0(void *callback)
{
	return(ProximityObject->Unknown_4575E0(callback));
}

// 0x44AFD0
bool DynObj::Unknown_44AFD0()
{
	return(ProximityObject->Unknown0D);
}

// 0x44AFF0
bool DynObj::Unknown_44AFF0()
{
	return(ProximityObject->Unknown0C);
}

// 0x44B010
bool DynObj::Unknown_44B010()
{
	return(ProximityObject->Unknown0E);
}

// 0x44B030
void DynObj::Unknown_44B030(bool value)
{
	ProximityObject->Unknown_456890(value);
}

// 0x44B050
void DynObj::Unknown_44B050(bool value)
{
	ProximityObject->Unknown_456840(value);
}

// 0x44B070
void DynObj::Unknown_44B070(bool value)
{
	ProximityObject->Unknown_4567D0(value);
}

// 0x44B090
void DynObj::Update_Proximity()
{
	WWPROFILE("DynObj::Update_Proximity");
	float radius = Unknown04;
	ProximityObject->Update(Unknown_24(), radius);
}

// 0x44B150
void DynObj::Update_Avoidance(const Vector3 &low, const Vector3 &high)
{
	WWPROFILE("DynObj::Update_Avoidance");
	ProximityObject->Unknown_456900(low, high);
}

/*
** Notes
**
** dynamic_obj.cpp is client 0x44AC70-0x44B740, after fp_tt.cpp (its __FILE__ string precedes this file's) and before
** px.cpp (0x44B760 on; its FAIL_IF '!(low && high)' 0x44BA70, line 180, is px.cpp's although asserts.txt files it
** here). The class name DynObj is the client's (its profile strings); the header name is provisional.
**
** The constructor allocates the object's PxWrapper (px_wrapper.cpp, not rebuilt; px_wrapper.h declares what this
** file calls), the copy constructor copies the source's, the destructor takes it off its list first (FAIL_IF line 70).
** Slots +0x68 / +0x6C must be overridden (G_ASSERT(0), lines 84 / 93); the non-virtual members forward to the
** PxWrapper. The inline slots follow in vtable order (0x44B1F0-0x44B740). The scalar deleting destructor 0x44AD10
** comes from the declaration.
*/
