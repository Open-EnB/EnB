/*
**	Command & Conquer Renegade(tm)
**	Copyright 2025 Electronic Arts Inc.
**	Modified for the Earth & Beyond client reconstruction: each change is recorded at the end of this file.
**	This program is free software: you can redistribute it and/or modify
**	it under the terms of the GNU General Public License as published by
**	the Free Software Foundation, either version 3 of the License, or
**	(at your option) any later version.
**
**	This program is distributed in the hope that it will be useful,
**	but WITHOUT ANY WARRANTY; without even the implied warranty of
**	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**	GNU General Public License for more details.
**
**	You should have received a copy of the GNU General Public License
**	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "FastAllocator.h"

static FastAllocatorGeneral* generalAllocator; //This general allocator will do all allocations for us.

FastAllocatorGeneral* FastAllocatorGeneral::Get_Allocator()
{
	if (!generalAllocator) {
		generalAllocator=reinterpret_cast<FastAllocatorGeneral*>(::malloc(sizeof(FastAllocatorGeneral)));

		new (generalAllocator) FastAllocatorGeneral();
	}
	return generalAllocator;
}

FastAllocatorGeneral::FastAllocatorGeneral() : MemoryLeakLogEnabled(false), AllocatedWithMalloc(0), AllocatedWithMallocCount(0), ActualMemoryUsage(0), UnknownE18(0), UnknownE1C(0)
{
	int alloc_size=ALLOC_STEP;
	for (int i=0;i<MAX_ALLOC_SIZE/ALLOC_STEP;++i) {
	   allocators[i].Init(alloc_size);
		alloc_size+=ALLOC_STEP;
	}
}

unsigned FastAllocatorGeneral::Get_Total_Heap_Size()
{
	int size=AllocatedWithMalloc;
	for (int i=0;i<MAX_ALLOC_SIZE/ALLOC_STEP;++i) {
		FastCriticalSectionClass::LockClass lock(CriticalSections[i]);
		size+=allocators[i].Get_Heap_Size();
	}
	return size;
}

unsigned FastAllocatorGeneral::Get_Total_Allocated_Size()
{
	int size=AllocatedWithMalloc;
	for (int i=0;i<MAX_ALLOC_SIZE/ALLOC_STEP;++i) {
		FastCriticalSectionClass::LockClass lock(CriticalSections[i]);
		size+=allocators[i].Get_Allocated_Size();
	}
	return size;
}

unsigned FastAllocatorGeneral::Get_Total_Allocation_Count()
{
	int count=AllocatedWithMallocCount;
	for (int i=0;i<MAX_ALLOC_SIZE/ALLOC_STEP;++i) {
		FastCriticalSectionClass::LockClass lock(CriticalSections[i]);
		count+=allocators[i].Get_Allocation_Count();
	}
	return count;
}

// E&B (name not known): latches and clears the usage counters (0x9DADA0; called from the app at 0x53EE47
// and 0x6A1599)
void FastAllocatorGeneral::Unknown_Reset_Statistics()
{
	{
		FastCriticalSectionClass::LockClass lock(UnknownE00);
		UnknownE20[2]=UnknownE20[3];
		UnknownE20[3]=0;
	}
	{
		FastCriticalSectionClass::LockClass lock(UnknownE04);
		UnknownE18=UnknownE1C;
		UnknownE20[0]=UnknownE20[1];
		UnknownE1C=0;
		UnknownE20[1]=0;
	}
}

// E&B's Alloc (0x9DAE10, out of line): no re-entrancy counter; usage statistics under their own lock,
// the malloc path under another (both locks and the extra counters are E&B additions)
void* FastAllocatorGeneral::Alloc(unsigned int n)
{
	void* pMemory;

	//We actually allocate n+4 bytes. We store the # allocated
	//in the first 4 bytes, and return the ptr to the rest back
	//to the user.
	n += sizeof(unsigned int);

	{
		FastCriticalSectionClass::LockClass lock(UnknownE04);
		ActualMemoryUsage+=n;
		UnknownE1C+=n;
		UnknownE20[1]++;
	}
	if (n<MAX_ALLOC_SIZE) {
		unsigned index=n/ALLOC_STEP;
		FastCriticalSectionClass::LockClass lock(CriticalSections[index]);
		UnknownE1C+=n;
		pMemory = allocators[index].Alloc();
	}
	else {
		FastCriticalSectionClass::LockClass lock(UnknownE00);
		AllocatedWithMalloc+=n;
		AllocatedWithMallocCount++;
		pMemory = ::malloc(n);
	}

	*((unsigned int*)pMemory) = n;     //Write modified (augmented by 4) count into first four bytes.
	return ((unsigned int*)pMemory)+1; //return ptr to bytes after it back to user.
}

// E&B's Free (0x9DAF60, out of line): the malloc path under the 0xE00 lock, the usage counter under the
// 0xE04 lock afterwards (see Alloc)
void FastAllocatorGeneral::Free(void* pAlloc)
{
	if (pAlloc) {
		unsigned int* n = ((unsigned int*)pAlloc)-1; //Subtract four bytes and the count is stored there.

		unsigned size=*n;

		if (size<MAX_ALLOC_SIZE) {
			unsigned index=size/ALLOC_STEP;
			FastCriticalSectionClass::LockClass lock(CriticalSections[index]);
			allocators[index].Free(n);
		}
		else {
			FastCriticalSectionClass::LockClass lock(UnknownE00);
			AllocatedWithMallocCount--;
			AllocatedWithMalloc-=size;
			::free(n);
		}
		{
			FastCriticalSectionClass::LockClass lock(UnknownE04);
			ActualMemoryUsage-=size;
		}
	}
}


// Earth & Beyond client (reconstruction): changes to Renegade's FastAllocator.cpp (frozen 2026-11-07 from importer.py)
// EnB: FastAllocatorGeneral::FastAllocatorGeneral (line 33 at the freeze): E&B's FastAllocatorGeneral is 0xE30 bytes
//       (Get_Allocator 0x9DAC10 mallocs 0xE30): its ctor 0x9DAC40 zeroes 0xE00 and 0xE04 before MemoryLeakLogEnabled
//       (0xE08), puts AllocatedWithMalloc/Count/ActualMemoryUsage at 0xE0C/0xE10/0xE14 and zeroes 0xE18, 0xE1C;
//       0x9DADA0 locks 0xE00 as a FastCriticalSectionClass and reads and clears 0xE2C. Names unknown (D10 neutral
//       names); E&B's Get_Total_Heap_Size/Get_Total_Allocated_Size/Get_Total_Allocation_Count are out of line in
//       fastallocator.cpp (0x9DACE0, 0x9DAD20, 0x9DAD60; called from WWMemoryAndTimeLog 0x9EBBA0, 0x9EBD80, 0x9EBF20)
// EnB: FastAllocatorGeneral::FastAllocatorGeneral (line 33 at the freeze): E&B's FastAllocatorGeneral::Alloc is out of
//       line (0x9DAE10, called only from the app's operator new 0x4E9CB0 and 0x4E9CE0): no re-entrancy counter; under
//       the lock at 0xE04 it adds n to 0xE14 (ActualMemoryUsage) and 0xE1C and counts 0xE24; the bucket path adds n to
//       0xE1C again under the bucket lock; the malloc path (n >= 0x800) runs under the lock at 0xE00; a chunk is
//       ::malloc(0x1FF4) (0x9DAEA1)
// EnB: Line 71 at the freeze: E&B's FastAllocatorGeneral::Alloc is out of line (0x9DAE10, called only from the app's
//       operator new 0x4E9CB0 and 0x4E9CE0): no re-entrancy counter; under the lock at 0xE04 it adds n to 0xE14
//       (ActualMemoryUsage) and 0xE1C and counts 0xE24; the bucket path adds n to 0xE1C again under the bucket lock;
//       the malloc path (n >= 0x800) runs under the lock at 0xE00; a chunk is ::malloc(0x1FF4) (0x9DAEA1)
// EnB: Line 71 at the freeze: E&B's FastAllocatorGeneral has a function at 0x9DADA0 (after Get_Total_Allocation_Count
//       0x9DAD60, before Alloc 0x9DAE10) that copies 0xE2C to 0xE28 and clears it under the 0xE00 lock, then copies
//       0xE1C/0xE24 to 0xE18/0xE20 and clears them under the 0xE04 lock; name unknown (D10)
// EnB: Line 123 at the freeze: E&B's FastAllocatorGeneral::Free is out of line (0x9DAF60, called from the app's
//       operator delete 0x4E9D10 and 0x9EDED0): the malloc path runs under the lock at 0xE00, then ActualMemoryUsage
//       (0xE14) is reduced under the lock at 0xE04 (0x9DAFFA)
