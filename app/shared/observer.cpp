// Earth & Beyond client (reconstruction): app\shared\observer.cpp, a subject's list of observers (SubjectClass,
// 0x49E890-0x49EB30 and the vector templates it emits to 0x49EF10; the assert sits on the client's line). Notes at the end.
#include "observer_impl.h"
#include "g_assert.h"

// @0x49E890
SubjectClass::SubjectClass(int size) : Observers(size)
{
	int step = size / 4;
	if (step < 2) {
		step = 2;
	} else if (step > 16) {
		step = 16;
	}
	Observers.Set_Growth_Step(step);
}

// @0x49E950
SubjectClass::~SubjectClass()
{
	Unknown_49EA60();
}









































































// @0x49EA60
void SubjectClass::Unknown_49EA60()
{
	int num;
	while ((num = Observers.Count()) != 0) {
		Observers[num - 1]->Unknown_04(this);
		G_ASSERT(num > Observers.Count());
	}
}

// @0x49EAB0
void SubjectClass::Unknown_49EAB0(void *event)
{
	int i = Observers.Count();
	while (i--) {
		Observers[i]->Unknown_00(this, event);
	}
}

// Reconstruction notes (UP1, 2026-11-07)
// - The object: __FILE__ e:\build\beta\G\app\shared\observer.cpp (0xB72C54) and its one assert, 'num > Observers.Count()'
//   on line 102 (0xB72C38), both in 0x49E950 and 0x49EA60: the destructor inlines 0x49EA60. Before it, 0x49E5C0 is a
//   std::vector instance closing damtype.cpp's object; after 0x49EAB0 come DynamicVectorClass<ObserverClass *>'s
//   members (0x49EAF0, 0x49EB30-0x49EEE0, emitted for the two tables the constructor stores), then 0x49EF10, the
//   referral.ini reader of the next object (no __FILE__ of its own, then selector.cpp).
// - SubjectClass is the vector alone (0x18 bytes): DynamicVectorClass's constructor stores VectorClass's table 0xAEA9A0
//   then its own 0xAEA984; the growth step is a quarter of the size, 2 to 16.
