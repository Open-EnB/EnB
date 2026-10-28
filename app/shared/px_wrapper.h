/*
**	Earth & Beyond client (reconstruction)
**	Original location: e:\build\beta\G\app\shared\. px_wrapper.cpp is the client's (its asserts, client
**	0x4560B0-0x4575E0); this header's name is provisional: no string names it.
**
**	PxWrapper, the client's name ('PxWrapper::Update(LooseBox)' in 0x456B60): a dynamic object's entry in the
**	proximity system (DynObj's ProximityObject, dynamic_obj.cpp), 0x2C bytes. Declared as far as
**	dynamic_obj.cpp calls it; px_wrapper.cpp is not rebuilt.
*/
#if defined(_MSC_VER)
#pragma once
#endif
#ifndef PX_WRAPPER_H  // NAME 1 [px_wrapper.h]: provisional header name (px_wrapper.cpp's)
#define PX_WRAPPER_H

#include "vector3.h"

class DynObj;

class PxWrapper
{
public:
	PxWrapper(DynObj *owner);											// 0x4560B0  // NAME 1 [owner]: it stores it at +0x00
	PxWrapper(DynObj *owner, const PxWrapper &src);					// 0x456190 (DynObj's copy)  // NAME 1 [owner, src]: as above
	~PxWrapper();														// 0x456500 (FAIL_IF 'List' l.108)

	bool Is_In_List() const { return(List != NULL); }					// dynamic_obj.cpp's FAIL_IF 'ProximityObject->Is_In_List()'
	void Unknown_456730();												// 0x456730 (FAIL_IF '!List' l.154: leaves the list)
	void Unknown_4567D0(bool value);									// 0x4567D0 (sets +0x0E)  // NAME 1 [value]: what it stores
	void Unknown_456840(bool value);									// 0x456840 (sets +0x0C)  // NAME 1 [value]: what it stores
	void Unknown_456890(bool value);									// 0x456890 (sets +0x0D)  // NAME 1 [value]: what it stores
	void Unknown_456900(const Vector3 &low, const Vector3 &high);		// 0x456900 (FAIL_IF '!low.Is_Valid()' l.247, '!high.Is_Valid()' l.248)
	void Update(const Vector3 &pos, float radius);						// 0x456B60 'PxWrapper::Update' (FAIL_IF '!pos.Is_Valid()' l.272, '!WWMath::Is_Valid_Float(radius)')
	void *Unknown_456F60(int a, int b, int c);							// 0x456F60 (adds a callback object to +0x1C's vector)  // NAME 1 [a, b, c]: neutral
	void *Unknown_457190(int a, int b, int c);							// 0x457190 (as 0x456F60 with another factory)  // NAME 1 [a, b, c]: neutral
	bool Unknown_4575E0(void *callback);								// 0x4575E0 (removes it from +0x1C's vector)  // NAME 1 [callback]: what it searches for

	DynObj *Owner;														// +0x00  // NAME 1: the constructor's argument
	int Unknown04;														// +0x04 0
	void *List;															// +0x08 0 (Is_In_List tests it; the client's name, the FAIL_IFs 'List' / '!List')
	bool Unknown0C;														// +0x0C false
	bool Unknown0D;														// +0x0D false
	bool Unknown0E;														// +0x0E false (the constructor sets it through 0x4567D0)
	void *Unknown10;													// +0x10
	void *Unknown14;													// +0x14
	void *Unknown18;													// +0x18
	char Unknown1C[0x2C - 0x1C];										// +0x1C a std::vector of callback objects (allocator, first, last, end)
};

#endif
